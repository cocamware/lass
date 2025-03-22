#!/usr/bin/env -S uv run --quiet

"""
stubgen.py - Generate Python stubs from C++ code using libclang.
"""

# /// script
# dependencies = [
#     "libclang==18.1.1",
# ]
# ///

# *** BEGIN LICENSE INFORMATION ***
#
# The contents of this file are subject to the Common Public Attribution License
# Version 1.0 (the "License"); you may not use this file except in compliance with
# the License. You may obtain a copy of the License at
# https://lass.cocamware.com/cpal-license. The License is based on the
# Mozilla Public License Version 1.1 but Sections 14 and 15 have been added to cover
# use of software over a computer network and provide for limited attribution for
# the Original Developer. In addition, Exhibit A has been modified to be consistent
# with Exhibit B.
#
# Software distributed under the License is distributed on an "AS IS" basis, WITHOUT
# WARRANTY OF ANY KIND, either express or implied. See the License for the specific
# language governing rights and limitations under the License.
#
# The Original Code is LASS - Library of Assembled Shared Sources.
#
# The Initial Developer of the Original Code is Bram de Greve and Tom De Muer.
# The Original Developer is the Initial Developer.
#
# All portions of the code written by the Initial Developer are:
# Copyright (C) 2025 the Initial Developer.
# All Rights Reserved.
#
# Contributor(s):
#
# Alternatively, the contents of this file may be used under the terms of the
# GNU General Public License Version 2 or later (the GPL), in which case the
# provisions of GPL are applicable instead of those above.  If you wish to allow use
# of your version of this file only under the terms of the GPL and not to allow
# others to use your version of this file under the CPAL, indicate your decision by
# deleting the provisions above and replace them with the notice and other
# provisions required by the GPL License. If you do not delete the provisions above,
# a recipient may use your version of this file under either the CPAL or the GPL.
#
# *** END LICENSE INFORMATION ***

from __future__ import annotations

import dataclasses
import json
import os
import re
import sys
import textwrap
from argparse import ArgumentParser
from dataclasses import dataclass
from os import PathLike
from pathlib import Path
from typing import Any, Callable, Self, TextIO, TypeAlias
from types import EllipsisType
import sysconfig
import multiprocessing
import functools

from clang import cindex  # type: ignore
from clang.cindex import CursorKind, TypeKind  # type: ignore

StrPath: TypeAlias = str | PathLike[str]


def main(argv: list[str]) -> int:
    parser = ArgumentParser(description=__doc__)
    parser.add_argument("path", nargs="+")
    parser.add_argument("-I", "--include", dest="includes", action="append", default=[])
    parser.add_argument("-D", "--define", dest="defines", action="append", default=[])
    parser.add_argument("--obj", dest="object_files", action="append", default=[])
    parser.add_argument("--output-dir", type=str)
    parser.add_argument("--num-threads", type=int, default=0)
    parser.add_argument("--package", type=str)
    parser.add_argument("--import", dest="imports", action="append", default=[])
    parser.add_argument("--export", type=str)

    args, rest = parser.parse_known_args(argv)
    if rest and rest[0] == "--":
        rest = rest[1:]

    try:
        stubgen = parse(
            source_paths=args.path,
            object_files=args.object_files,
            includes=args.includes,
            defines=args.defines,
            args=rest,
            output_dir=args.output_dir,
            num_threads=args.num_threads,
            package=args.package,
            imports=args.imports,
            export=args.export,
        )
    except ParseError as err:
        print(f"{len(err.errors)} parse errors found, aborting", file=sys.stderr)
        return 1

    generate(stubgen, output_dir=args.output_dir)
    return 0


def parse(
    source_paths: list[StrPath],
    *,
    object_files: list[StrPath] | None = None,
    includes: list[StrPath] | None = None,
    defines: list[str] | None = None,
    args: list[str] | None = None,
    output_dir: StrPath | None = None,
    num_threads: int = 0,
    package: str | None = None,
    imports: list[StrPath] | None = None,
    export: StrPath | None = None,
) -> Stubgen:
    source_paths_ = [Path(path) for path in source_paths]
    output_dir_ = Path(output_dir) if output_dir else Path.cwd()
    imports_ = [Path(path) for path in imports or []]
    export = Path(export) if export else None

    if object_files:
        object_files_map = {Path(path).stem: Path(path) for path in object_files}
        assert len(object_files_map) == len(object_files), "duplicate object files"
    else:
        object_files_map = {}

    parser = functools.partial(
        _parse_file,
        object_files_map=object_files_map,
        includes=includes,
        defines=defines,
        args=args,
        output_dir=output_dir_,
        package=package,
    )

    stubgen = Stubgen(package=package)
    for import_ in imports_:
        stubgen.load(import_, clear=False, imported=True)

    if num_threads == 1:
        for path in source_paths_:
            intermediate_path = parser(path)
            stubgen.load(intermediate_path, clear=False)
    else:
        if not num_threads:
            num_threads = multiprocessing.cpu_count()
        num_threads = min(num_threads, len(source_paths_)) or 1
        pool = multiprocessing.Pool(num_threads)
        for intermediate_path in pool.imap_unordered(parser, source_paths_):
            stubgen.load(intermediate_path, clear=False)

    for mod_def in stubgen._modules.values():
        assert mod_def.fully_qualified_name
        for shadow_name in mod_def.classes:
            class_def = stubgen._shadow_classes[shadow_name]
            stubgen._fix_fully_qualified_name(class_def, mod_def.fully_qualified_name)
        for enum_name in mod_def.enums:
            enum_def = stubgen._enums[enum_name]
            fqname = f"{mod_def.fully_qualified_name}.{enum_def.py_name}"
            assert (
                enum_def.fully_qualified_name is None
                or enum_def.fully_qualified_name == fqname
            )
            enum_def.fully_qualified_name = fqname

    if export:
        stubgen.store(export)

    return stubgen


def _parse_file(
    source_path: Path,
    *,
    object_files_map: dict[str, Path],
    includes: list[StrPath] | None,
    defines: list[str] | None,
    args: list[str] | None,
    output_dir: Path,
    package: str | None,
) -> Path:
    stem = source_path.stem
    try:
        object_file = object_files_map[stem]
    except KeyError:
        intermediate_path = (output_dir / source_path.name).with_suffix(".stubgen.json")
    else:
        intermediate_path = object_file.with_suffix(".stubgen.json")
        try:
            intermediate_mtime = os.path.getmtime(intermediate_path)
            object_mtime = os.path.getmtime(object_file)
        except FileNotFoundError:
            pass
        else:
            self_mtime = os.path.getmtime(__file__)
            if intermediate_mtime > object_mtime and intermediate_mtime > self_mtime:
                return intermediate_path

    print(f"Parsing {source_path}...", file=sys.stderr)
    stubgen = Stubgen(package=package)
    stubgen.parse(source_path, includes=includes, defines=defines, args=args)

    stubgen.store(intermediate_path)
    return intermediate_path


def generate(stubgen: Stubgen, output_dir: StrPath | None = None) -> None:
    if output_dir:
        output_dir = Path(output_dir)
        output_dir.mkdir(parents=True, exist_ok=True)
    for mod_def in stubgen._modules.values():
        if mod_def.imported:
            continue
        if output_dir:
            output = f"{output_dir}/{mod_def.py_name}.pyi"
            print(f"Writing {output}...", file=sys.stderr)
            with open(output, "w") as file:
                stubgen.write_module(mod_def, file=file)
        else:
            print(f"# ============= {mod_def.py_name} =============", file=sys.stderr)
            stubgen.write_module(mod_def, file=sys.stdout)


class Stubgen:
    _modules: dict[str, ModuleDefinition]
    _shadow_classes: dict[str, ClassDefinition]
    _cpp_classes: dict[str, ClassDefinition]
    _enums: dict[str, EnumDefinition]
    _export_traits: dict[str, dict[str, ExportTraits]]
    _package: str | None

    def __init__(self, package: str | None = None):
        self._modules = {}
        self._shadow_classes = {}
        self._cpp_classes = {}
        self._enums = {}
        self._export_traits = {}
        self._package = package

    def parse(
        self,
        path: StrPath,
        *,
        includes: list[StrPath] | None = None,
        defines: list[str] | None = None,
        args: list[str] | None = None,
    ) -> None:
        args_ = [f"-I{include}" for include in includes or []]
        args_.append(f"-I{sysconfig.get_path('include')}")
        args_ += [f"-D{define}" for define in defines or []]
        args_ += args or []

        index = cindex.Index.create()
        try:
            tu = index.parse(path, args=args_)
        except cindex.TranslationUnitLoadError as err:
            print(dir(err))
            raise

        for diag in tu.diagnostics:
            print(diag, file=sys.stderr)
        errors = [
            str(diag)
            for diag in tu.diagnostics
            if diag.severity >= cindex.Diagnostic.Error
        ]
        if errors:
            raise ParseError(errors)

        self._parse_node(tu.cursor)

    def _parse_node(self, node: cindex.Cursor) -> None:
        if node.kind == CursorKind.CALL_EXPR:
            type_ = node.type.get_canonical().spelling
            if type_ == "lass::python::ModuleDefinition":
                if self._handle_declare_module(node):
                    return
            if type_ == "lass::python::impl::ClassDefinition":
                if self._handle_declare_class(node):
                    return
            if node.spelling == "IntEnumDefinition":
                if self._handle_declare_enum(node):
                    return
            if node.spelling == "EnumDefinition":
                if self._handle_declare_enum(node):
                    return
            if node.spelling == "addClass":
                if self._handle_module_add_class(node):
                    return
            if node.spelling == "addEnum":
                if self._handle_module_add_enum(node):
                    return
            if node.spelling == "addFunctionDispatcher":
                if self._handle_module_add_function(node):
                    return
            if node.spelling == "addConstructor":
                if self._handle_class_add_constructor(node):
                    return
            if node.spelling == "addMethod":
                if self._handle_class_add_method(node):
                    return
            if node.spelling == "addStaticMethod":
                if self._handle_class_add_static_method(node):
                    return
            if node.spelling == "addGetSetter":
                if self._handle_class_add_get_setter(node):
                    return
            if node.spelling == "addStaticConst":
                if self._handle_class_add_static_const(node):
                    return
            if node.spelling == "addIntegerConstantsToModule":
                if self._handle_module_add_integer_constants(node):
                    return
            if node.spelling == "addLong":
                if self._handle_module_add_long(node):
                    return
            if node.spelling == "addInnerClass":
                if self._handle_class_add_inner_class(node):
                    return
            if node.spelling == "addInnerEnum":
                if self._handle_class_add_inner_enum(node):
                    return
        if node.spelling == "PyExportTraits":
            if self._handle_export_traits(node):
                return

        for child in node.get_children():
            self._parse_node(child)

    def _handle_declare_module(self, node: cindex.Cursor) -> bool:
        assert node.kind == CursorKind.CALL_EXPR
        assert _get_type(node).spelling == "lass::python::ModuleDefinition"

        var_decl = _ensure_kind(node.semantic_parent, CursorKind.VAR_DECL)
        cpp_name = fully_qualified(var_decl)

        args = list(node.get_arguments())
        py_name = _ensure_string_literal_recursive(args[0])
        doc = _ensure_string_literal_or_none_recursive(args[1])

        if self._package:
            fully_qualified_name = f"{self._package}.{py_name}"
        else:
            fully_qualified_name = py_name

        self._modules[cpp_name] = ModuleDefinition(
            cpp_name, py_name, doc, fully_qualified_name=fully_qualified_name
        )
        return True

    def _handle_declare_class(self, node: cindex.Cursor) -> bool:
        assert node.kind == CursorKind.CALL_EXPR
        assert _get_type(node).spelling == "lass::python::impl::ClassDefinition"
        args = list(node.get_arguments())

        py_name = _ensure_string_literal_recursive(args[0])
        doc = _ensure_string_literal_or_none_recursive(args[1])

        # the fifth argument is the parent class definition
        arg4 = _ensure_kind(args[4], CursorKind.UNARY_OPERATOR)
        parent_def = _ensure_only_child(arg4, CursorKind.DECL_REF_EXPR).referenced
        parent_type = fully_qualified(parent_def.semantic_parent)

        # we're assuming the last argument is _lassPyClassRegisterHook
        # which is defined on the C++ type, and is the way we can get to the C++ type
        arg5 = _ensure_kind(args[5], CursorKind.UNARY_OPERATOR)
        register_hook = _ensure_only_child(arg5, CursorKind.DECL_REF_EXPR)
        assert register_hook.spelling == "_lassPyClassRegisterHook"
        shadow_node = _ensure_only_child(register_hook, CursorKind.TYPE_REF)
        shadow_type = _get_type(shadow_node)
        shadow_name: str = shadow_type.spelling

        shadow_decl = shadow_type.get_declaration()
        for base in _iter_children(shadow_decl, CursorKind.CXX_BASE_SPECIFIER):
            # if this is a shadow class, then we derive from ShadowClass
            # which will have the C++ class as first argument.
            base_type = type_info(_get_type(base))
            if base_type.name == "lass::python::ShadowClass":
                assert (
                    base_type.args and shadow_name == base_type.args[0].name
                ), f"{shadow_name=}, {base_type=}"
                cpp_name = base_type.args[1].name
                break
        else:
            cpp_name = shadow_name

        class_def = ClassDefinition(
            py_name=py_name,
            cpp_name=cpp_name,
            shadow_name=shadow_name,
            parent_type=parent_type,
            doc=doc,
        )
        self._shadow_classes[shadow_name] = class_def
        self._cpp_classes[cpp_name] = class_def

        return True

    def _handle_declare_enum(self, node: cindex.Cursor) -> bool:
        assert node.kind == CursorKind.CALL_EXPR
        enum_type = type_info(_get_type(node))
        if node.spelling == "IntEnumDefinition":
            assert enum_type.name == "lass::python::IntEnumDefinition"
            value_py_type = "int"
        elif node.spelling == "EnumDefinition":
            assert enum_type.name == "lass::python::StrEnumDefinition"
            value_py_type = "str"
        assert enum_type.args and enum_type.args[0].args is None
        cpp_name = enum_type.args[0].name

        children = list(node.get_children())

        # zeroth child is the enum name
        call_expr = _ensure_kind_recursive(children[0], CursorKind.CALL_EXPR)
        assert call_expr.spelling == "basic_string"
        name_arg = _ensure_only_child_recursive(call_expr, CursorKind.STRING_LITERAL)
        py_name = _ensure_string_literal(name_arg)

        # first child is the initializer list
        init_list = _ensure_kind_recursive(children[1], CursorKind.INIT_LIST_EXPR)
        values = {}
        for init_list_expr in init_list.get_children():
            args = list(init_list_expr.get_children())
            # zeroth argument is the name of the enum value.
            key = _ensure_string_literal(args[0])
            value: str | int | EllipsisType
            if value_py_type == "int":
                # first argument is the cpp value _and_ the python value
                # (there is no second argument)
                value_ref = _ensure_kind(args[1], CursorKind.DECL_REF_EXPR)
                value_decl = value_ref.referenced
                assert value_decl.kind == CursorKind.ENUM_CONSTANT_DECL
                _ensure_only_child(value_decl, CursorKind.INTEGER_LITERAL)
                value = int(value_decl.enum_value)
            elif value_py_type == "str":
                # first argument is the cpp value, which we ignore
                # the second argument is a string that we want as python value
                value = _ensure_string_literal(args[2])
            else:
                value = Ellipsis
            values[key] = value

        self._enums[cpp_name] = EnumDefinition(
            py_name=py_name,
            cpp_name=cpp_name,
            value_py_type=value_py_type,
            values=values,
        )
        return True

    def _handle_module_add_class(self, node: cindex.Cursor) -> bool:
        children = list(node.get_children())

        if children[0].kind != CursorKind.MEMBER_REF_EXPR:
            return False
        if (
            fully_qualified(children[0].referenced)
            != "lass::python::ModuleDefinition::addClass"
        ):
            return False

        module_ref = _ensure_only_child(children[0], CursorKind.DECL_REF_EXPR)
        assert _get_type(module_ref).spelling == "lass::python::ModuleDefinition"
        module_name = fully_qualified(module_ref.referenced)

        _lassPyClassDefRef = children[1]
        assert _lassPyClassDefRef.kind == CursorKind.DECL_REF_EXPR
        assert _lassPyClassDefRef.spelling == "_lassPyClassDef"
        assert (
            _get_type(_lassPyClassDefRef).spelling
            == "lass::python::impl::ClassDefinition"
        )

        shadow_class = _lassPyClassDefRef.referenced.semantic_parent
        shadow_name = fully_qualified(shadow_class)

        self._modules[module_name].classes.append(shadow_name)
        return True

    def _handle_module_add_enum(self, node: cindex.Cursor) -> bool:
        children = list(node.get_children())

        if children[0].kind != CursorKind.MEMBER_REF_EXPR:
            return False
        if (
            fully_qualified(children[0].referenced)
            != "lass::python::ModuleDefinition::addEnum"
        ):
            return False

        module_ref = _ensure_only_child(children[0], CursorKind.DECL_REF_EXPR)
        assert _get_type(module_ref).spelling == "lass::python::ModuleDefinition"
        module_name = fully_qualified(module_ref.referenced)

        unary_op = _ensure_only_child(children[1], CursorKind.UNARY_OPERATOR)
        decl_ref = _ensure_only_child(unary_op, CursorKind.DECL_REF_EXPR)
        enum_type = type_info(_get_type(decl_ref))
        assert enum_type.args, f"{enum_type=} must have template args"
        cpp_name = enum_type.args[0].name
        assert cpp_name in self._enums

        self._modules[module_name].enums.append(cpp_name)
        return True

    def _handle_module_add_function(self, node: cindex.Cursor) -> bool:
        children = list(node.get_children())
        if children[0].kind != CursorKind.MEMBER_REF_EXPR:
            return False
        if (
            fully_qualified(children[0].referenced)
            != "lass::python::ModuleDefinition::addFunctionDispatcher"
        ):
            return False

        # zeroth argument is the module definition
        module_ref = _ensure_only_child(children[0], CursorKind.DECL_REF_EXPR)
        assert _get_type(module_ref).spelling == "lass::python::ModuleDefinition"
        module_name = fully_qualified(module_ref.referenced)

        # first argument is the dispatcher
        dispatcher_arg = children[1]
        dispatcher_ref = _get_only_child(dispatcher_arg, CursorKind.DECL_REF_EXPR)
        cpp_type, cpp_return_type, params = self._handle_dispatcher_ref(dispatcher_ref)

        # second argument is the name of the function
        name_arg = _ensure_only_child(children[2], CursorKind.STRING_LITERAL)
        name = _ensure_string_literal(name_arg)

        # third argument is the documentation
        doc_arg = _get_only_child(children[3], CursorKind.STRING_LITERAL)
        doc = _ensure_string_literal(doc_arg) if doc_arg else None

        func_def = FunctionDefinition(
            py_name=name,
            doc=doc,
            cpp_return_type=cpp_return_type,
            cpp_args=params,
            cpp_signature=cpp_type,
        )
        self._modules[module_name].functions.setdefault(name, []).append(func_def)
        return True

    def _handle_module_add_integer_constants(self, node: cindex.Cursor) -> bool:
        children = list(node.get_children())

        # zeroth argument in reference to addIntegerConstantsToModule
        decl_ref_expr = _ensure_kind_recursive(children[0], CursorKind.DECL_REF_EXPR)
        if (
            fully_qualified(decl_ref_expr.referenced)
            != "lass::python::impl::addIntegerConstantsToModule"
        ):
            return False

        args = list(node.get_arguments())

        # first argument is the module definition
        module_ref = _ensure_kind(args[0], CursorKind.DECL_REF_EXPR)
        assert _get_type(module_ref).spelling == "lass::python::ModuleDefinition"
        module_name = fully_qualified(module_ref.referenced)

        # second argument is the name of the constants as a call to basic_string
        basic_string = _ensure_kind_recursive(args[1], CursorKind.CALL_EXPR)
        assert basic_string.spelling == "basic_string"
        name_arg = _ensure_string_literal(
            _ensure_only_child_recursive(basic_string, CursorKind.STRING_LITERAL)
        )
        names = [name.strip() for name in name_arg.split(",")]

        # then there should follow as many arguments as there are names:
        assert len(args) == 2 + len(names)
        for name, arg in zip(names, args[2:]):
            decl_ref_expr = _ensure_kind(arg, CursorKind.DECL_REF_EXPR)
            enum_constant = _ensure_kind(
                decl_ref_expr.referenced, CursorKind.ENUM_CONSTANT_DECL
            )
            value = enum_constant.enum_value
            self._modules[module_name].constants[name] = ConstDefinition(
                py_name=name, cpp_type=TypeInfo("long"), py_value=value
            )

        return True

    def _handle_module_add_long(self, node: cindex.Cursor) -> bool:
        children = list(node.get_children())
        if not _is_member_ref_expr(
            children[0], "lass::python::ModuleDefinition::addLong"
        ):
            return False

        parent = node.semantic_parent
        if not parent:
            return False

        self.debug(node)
        assert False

    def _handle_class_add_inner_class(self, node: cindex.Cursor) -> bool:
        children = list(node.get_children())
        if children[0].kind != CursorKind.MEMBER_REF_EXPR:
            return False
        if (
            fully_qualified(children[0].referenced)
            != "lass::python::impl::ClassDefinition::addInnerClass"
        ):
            return False

        # zeroth argument is the class definition of the parent
        shadowType = _ensure_shadow_class(children[0])

        # first argument is the class definition of the inner class
        type_ref = _ensure_only_child(children[1], CursorKind.TYPE_REF)
        inner_class = _get_type(type_ref).spelling

        self._shadow_classes[shadowType].inner_classes.append(inner_class)
        return True

    def _handle_class_add_constructor(self, node: cindex.Cursor) -> bool:
        children = list(node.get_children())
        if children[0].kind != CursorKind.MEMBER_REF_EXPR:
            return False
        if (
            fully_qualified(children[0].referenced)
            != "lass::python::impl::ClassDefinition::addConstructor"
        ):
            return False
        shadowType = _ensure_shadow_class(children[0])

        dispatcher_arg = children[1]
        dispatcher_ref = _ensure_only_child(dispatcher_arg, CursorKind.DECL_REF_EXPR)
        _ensure_type(dispatcher_ref, "_object *(_typeobject *, _object *, _object *)")
        dispatcher = dispatcher_ref.referenced

        compound = _ensure_last_child(dispatcher, CursorKind.COMPOUND_STMT, num=5)
        return_stmt = _ensure_last_child(compound, CursorKind.RETURN_STMT, num=2)
        call_expr = _ensure_only_child(return_stmt, CursorKind.CALL_EXPR)
        call_constructor_ptr = _ensure_first_child(call_expr, CursorKind.UNEXPOSED_EXPR)
        _ensure_type(call_constructor_ptr, "_object *(*)(_typeobject *, _object *)")
        call_constructor_ref = _get_only_child(
            call_constructor_ptr, CursorKind.DECL_REF_EXPR
        )
        _ensure_type(call_constructor_ref, "_object *(_typeobject *, _object *)")

        t_params_ref = _ensure_last_child(
            call_constructor_ref, [CursorKind.TYPE_REF, CursorKind.TEMPLATE_REF]
        )
        t_params_type = type_info(_get_type(t_params_ref))

        cpp_args = [("", type_) for type_ in (t_params_type.args or [])]
        cpp_signature = f"({', '.join(str(type_) for _, type_ in cpp_args)})"
        self._shadow_classes[shadowType].constructors.append(
            ConstructorDefinition(
                cpp_args=cpp_args,
                cpp_signature=cpp_signature,
            )
        )
        return True

    def _handle_class_add_method(self, node: cindex.Cursor) -> bool:
        children = list(node.get_children())
        if not _is_member_ref_expr(
            children[0], "lass::python::impl::ClassDefinition::addMethod"
        ):
            return False
        shadowType = _ensure_shadow_class(children[0])

        name_arg = children[1]
        if _get_type(name_arg).spelling == "const char *":
            name = _ensure_string_literal(_ensure_only_child(name_arg))
        else:
            assert name_arg.kind == CursorKind.DECL_REF_EXPR
            method = fully_qualified(name_arg.referenced)
            name = SPECIAL_METHODS[method]

        dispatcher_arg = children[3]
        fun = _ensure_only_child(dispatcher_arg, CursorKind.DECL_REF_EXPR)
        fun_children = list(fun.get_children())
        assert fun_children[-1].kind == CursorKind.UNEXPOSED_EXPR
        dispatcher_ref = _get_only_child(fun_children[-1], CursorKind.DECL_REF_EXPR)
        cpp_type, return_type, params = self._handle_dispatcher_ref(dispatcher_ref)

        self._shadow_classes[shadowType].methods.setdefault(name, []).append(
            MethodDefinition(name, return_type, params, cpp_type)
        )
        return True

    def _handle_class_add_static_method(self, node: cindex.Cursor) -> bool:
        children = list(node.get_children())
        if not _is_member_ref_expr(
            children[0], "lass::python::impl::ClassDefinition::addStaticMethod"
        ):
            return False
        shadowType = _ensure_shadow_class(children[0])

        name_arg = children[1]
        assert _get_type(name_arg).spelling == "const char *"
        name = _ensure_string_literal(_ensure_only_child(name_arg))

        dispatcher_arg = children[3]
        dispatcher_ref = _ensure_only_child(dispatcher_arg, CursorKind.DECL_REF_EXPR)
        cpp_type, return_type, params = self._handle_dispatcher_ref(dispatcher_ref)

        assert shadowType in self._shadow_classes

        self._shadow_classes[shadowType].methods.setdefault(name, []).append(
            MethodDefinition(name, return_type, params, cpp_type, is_static=True)
        )

        return True

    @classmethod
    def _handle_dispatcher_ref(
        cls,
        dispatcher_ref: cindex.Cursor,
    ) -> tuple[str, TypeInfo, list[tuple[str, TypeInfo]]]:
        assert _get_type(dispatcher_ref).spelling == "_object *(_object *, _object *)"
        dispatcher = dispatcher_ref.referenced
        # cls.debug(dispatcher)

        compound = _ensure_last_child(dispatcher, CursorKind.COMPOUND_STMT, num=4)
        return_stmt = _ensure_last_child(compound, CursorKind.RETURN_STMT)
        call_expr = _ensure_only_child(return_stmt, CursorKind.CALL_EXPR)
        assert call_expr.spelling in (
            "call",
            "callFunction",
            "callMethod",
            "callFree",
            "callFreeMethod",
        ), call_expr.spelling

        func_arg = _ensure_last_child(
            call_expr, [CursorKind.UNEXPOSED_EXPR, CursorKind.UNARY_OPERATOR]
        )
        func_ref = _ensure_only_child(func_arg, CursorKind.DECL_REF_EXPR)
        func = func_ref.referenced

        cpp_type = _get_type(func).spelling
        return_type = type_info(func.type.get_result())

        params = list(_iter_children(func, CursorKind.PARM_DECL))
        if call_expr.spelling in ("callFree", "callFreeMethod"):
            # first parameter is self
            params = params[1:]
        cpp_args = [(p.spelling, type_info(_get_type(p))) for p in params]

        return cpp_type, return_type, cpp_args

    def _handle_class_add_get_setter(self, node: cindex.Cursor) -> bool:
        children = list(node.get_children())
        if not _is_member_ref_expr(
            children[0], "lass::python::impl::ClassDefinition::addGetSetter"
        ):
            return False

        # zeroth argument is the class definition
        shadowType = _ensure_shadow_class(children[0])

        # first argument is the name of the property
        name_arg = _ensure_only_child(children[1], CursorKind.STRING_LITERAL)
        name = _ensure_string_literal(name_arg)

        # second argument is the documentation
        doc_arg = _get_only_child(children[2], CursorKind.STRING_LITERAL)
        doc = _ensure_string_literal(doc_arg) if doc_arg else None

        # third argument is the getter dispatcher
        dispatch = _ensure_only_child(children[3], CursorKind.DECL_REF_EXPR).referenced
        compound = _ensure_last_child(dispatch, CursorKind.COMPOUND_STMT, num=4)
        if return_stmt := _find_first_child(compound, CursorKind.RETURN_STMT):
            call_expr = _ensure_only_child(return_stmt, CursorKind.CALL_EXPR)
            if call_expr.spelling == "freeGet":
                args = list(call_expr.get_children())
                getter = _ensure_only_child(
                    args[2], CursorKind.DECL_REF_EXPR
                ).referenced
                get_type = type_info(_get_type(getter).get_result())
            else:
                # public member
                assert call_expr.spelling == "pyBuildSimpleObject"
                member_ref = _find_first_child(call_expr, CursorKind.MEMBER_REF_EXPR)
                assert member_ref
                get_type = type_info(_get_type(member_ref))
        else:
            try_stmt = _ensure_only_child(compound, CursorKind.CXX_TRY_STMT)
            try_compound = _ensure_first_child(try_stmt, CursorKind.COMPOUND_STMT)
            if_stmt = _find_first_child(try_compound, CursorKind.IF_STMT)
            if_compound = _ensure_last_child(if_stmt, CursorKind.COMPOUND_STMT, num=2)
            return_stmt = _ensure_only_child(if_compound, CursorKind.RETURN_STMT)
            call_builder = _ensure_last_child_recursive(
                return_stmt, CursorKind.CALL_EXPR
            )
            assert call_builder.spelling == "pyBuildSimpleObject"
            call_getter = _ensure_last_child_recursive(
                call_builder, CursorKind.CALL_EXPR
            )
            get_type = type_info(_get_type(call_getter))

        # forth arugment is an optional setter dispatcher
        if dispatch_ref := _get_only_child(children[4], CursorKind.DECL_REF_EXPR):
            dispatch = dispatch_ref.referenced
            compound = _ensure_last_child(dispatch, CursorKind.COMPOUND_STMT)
            return_stmt = _ensure_last_child(compound, CursorKind.RETURN_STMT)
            if unary_min := _get_only_child(return_stmt, CursorKind.UNARY_OPERATOR):
                call_expr = _ensure_only_child(unary_min, CursorKind.CALL_EXPR)
                member_ref = _ensure_last_child(
                    call_expr, CursorKind.MEMBER_REF_EXPR, num=3
                )
                set_type = type_info(_get_type(member_ref))
                set_value_name = "value"
            else:
                call_expr = _ensure_only_child(return_stmt, CursorKind.CALL_EXPR)
                args = list(call_expr.get_children())
                setter = _ensure_only_child(
                    args[3], CursorKind.DECL_REF_EXPR
                ).referenced
                params = list(_iter_children(setter, CursorKind.PARM_DECL))
                if call_expr.spelling == "set":
                    # only one parameter, the value
                    assert len(params) == 1, params
                    value = params[0]
                elif call_expr.spelling == "freeSet":
                    # first parameter is self, second is value
                    assert len(params) == 2, params
                    value = params[1]
                else:
                    self.debug(dispatch)
                    assert False, call_expr.spelling
                set_type = type_info(_get_type(value))
                set_value_name = value.spelling or "value"
        else:
            set_type = None
            set_value_name = "value"

        self._shadow_classes[shadowType].getsetters[name] = GetSetterDefinition(
            py_name=name,
            doc=doc,
            get_type=get_type,
            set_type=set_type,
            set_value_name=set_value_name,
        )

        return True

    def _handle_class_add_static_const(self, node: cindex.Cursor) -> bool:
        children = list(node.get_children())
        if not _is_member_ref_expr(
            children[0], "lass::python::impl::ClassDefinition::addStaticConst"
        ):
            return False

        # zeroth argument is the class definition
        shadowType = _ensure_shadow_class(children[0])

        # first argument is the name of the const
        name_arg = _ensure_kind_recursive(children[1], CursorKind.STRING_LITERAL)
        name = _ensure_string_literal(name_arg)

        # second argument is the value of the const
        value_arg = children[2]
        while value_arg.kind.is_unexposed():
            value_arg = _ensure_only_child(value_arg)
        value_type = type_info(_get_type(value_arg))

        self._shadow_classes[shadowType].consts[name] = StaticConstDefinition(
            py_name=name,
            cpp_type=value_type,
        )

        return True

    def _handle_class_add_inner_enum(self, node: cindex.Cursor) -> bool:
        children = list(node.get_children())
        if not _is_member_ref_expr(
            children[0], "lass::python::impl::ClassDefinition::addInnerEnum"
        ):
            return False

        # zeroth argument is the class definition
        shadowType = _ensure_shadow_class(children[0])
        class_def = self._shadow_classes[shadowType]

        # first argument is the enum definition
        unary_op = _ensure_only_child(children[1], CursorKind.UNARY_OPERATOR)
        decl_ref = _ensure_only_child(unary_op, CursorKind.DECL_REF_EXPR)
        enum_type = type_info(_get_type(decl_ref))
        assert enum_type.args, f"{enum_type=} must have template args"
        cpp_name = enum_type.args[0].name
        assert cpp_name in self._enums
        enum_def = self._enums[cpp_name]

        assert "." not in enum_def.py_name
        class_def.inner_enums.append(cpp_name)

        return True

    def _handle_export_traits(self, node: cindex.Cursor) -> bool:
        if node.kind in (CursorKind.TEMPLATE_REF, CursorKind.CLASS_TEMPLATE):
            return False
        assert fully_qualified(node) == "lass::python::PyExportTraits"
        type_ = type_info(_get_type(node))
        if type_.name != "lass::python::PyExportTraits":
            return False
        assert type_.args and len(type_.args) == 1
        cpp_type = type_.args[0]

        def find_py_typing(n: cindex.Cursor) -> str | None:
            for child in _iter_children(n, CursorKind.VAR_DECL):
                if child.spelling == "py_typing":
                    typing = _ensure_only_child_recursive(
                        child, CursorKind.STRING_LITERAL
                    )
                    return _ensure_string_literal(typing)
            return None

        py_type = find_py_typing(node)
        if py_type is None:
            # look in base class
            if cxx_base_specifier := _get_first_child(
                node, CursorKind.CXX_BASE_SPECIFIER
            ):
                template_ref = _ensure_first_child(
                    cxx_base_specifier, CursorKind.TEMPLATE_REF
                )
                base = template_ref.referenced
                py_type = find_py_typing(base)
        if not py_type:
            return False

        if node.kind == CursorKind.STRUCT_DECL:
            # A full specialization
            specializations = self._export_traits.setdefault(cpp_type.name, {})
            specializations[str(cpp_type)] = ExportTraits(
                cpp_type=cpp_type,
                py_type=py_type,
            )
            return True

        elif node.kind == CursorKind.CLASS_TEMPLATE_PARTIAL_SPECIALIZATION:
            # A partial specialization

            # intermediate_params maps template parameters like type-parameter-0-0
            # to the template parameters of the specialization like T
            intermediate_params: dict[str, str] = {}
            for param in _iter_children(node, CursorKind.TEMPLATE_TYPE_PARAMETER):
                intermediate_params[_get_type(param).spelling] = param.spelling

            cpp_type = cpp_type.substitute(intermediate_params)
            specializations = self._export_traits.setdefault(cpp_type.name, {})
            specializations[str(cpp_type)] = ExportTraits(
                cpp_type=cpp_type,
                py_type=py_type,
                template_params=list(intermediate_params.values()),
            )

            return True

        else:
            self.debug(node)
            assert False, f"Unhandled node kind {node.kind}"

        return True

    def store(self, path: StrPath) -> None:
        """
        Store state as JSON file.
        """
        data = {
            "version": 1,
            "modules": [mod_def.asdict() for mod_def in self._modules.values()],
            "classes": [
                class_def.asdict() for class_def in self._shadow_classes.values()
            ],
            "enums": [enum_def.asdict() for enum_def in self._enums.values()],
            "export_traits": {
                class_: [traits.asdict() for traits in specializations.values()]
                for class_, specializations in self._export_traits.items()
            },
        }
        os.makedirs(os.path.dirname(path), exist_ok=True)
        with open(path, "w") as file:
            json.dump(data, fp=file, indent=2)

    def load(
        self, path: StrPath, *, clear: bool = True, imported: bool = False
    ) -> None:
        """
        Load state from JSON file.
        """
        if clear:
            self._modules.clear()
            self._shadow_classes.clear()
            self._cpp_classes.clear()
            self._enums.clear()
            self._export_traits.clear()
        with open(path, "r") as file:
            data = json.load(file)
        for mod in data["modules"]:
            mod_def = ModuleDefinition.fromdict(mod)
            if imported:
                mod_def.imported = True
            self._modules[mod_def.py_name] = mod_def
        for class_ in data["classes"]:
            class_def = ClassDefinition.fromdict(class_)
            self._shadow_classes[class_def.shadow_name] = class_def
            self._cpp_classes[class_def.cpp_name] = class_def
        for enum_ in data["enums"]:
            enum_def = EnumDefinition.fromdict(enum_)
            self._enums[enum_def.cpp_name] = enum_def
        for class_, traits_list in data["export_traits"].items():
            specializations = self._export_traits.setdefault(class_, {})
            for traits_ in traits_list:
                traits = ExportTraits.fromdict(traits_)
                specializations[str(traits.cpp_type)] = traits

    def _fix_fully_qualified_name(self, class_def: ClassDefinition, scope: str) -> None:
        """Fix the fully qualified name of a class definition."""
        fully_qualified_name = f"{scope}.{class_def.py_name}"
        assert (
            not class_def.fully_qualified_name
            or class_def.fully_qualified_name == fully_qualified_name
        )
        class_def.fully_qualified_name = fully_qualified_name

        for inner_class in class_def.inner_classes:
            inner_class_def = self._shadow_classes[inner_class]
            self._fix_fully_qualified_name(inner_class_def, fully_qualified_name)
        for enum_ in class_def.inner_enums:
            enum_def = self._enums[enum_]
            fqname = f"{fully_qualified_name}.{enum_def.py_name}"
            assert (
                not enum_def.fully_qualified_name
                or enum_def.fully_qualified_name == fqname
            )
            enum_def.fully_qualified_name = fqname

    @classmethod
    def debug(cls, node: cindex.Cursor, indent: str = "") -> None:
        if not indent:
            print(f"---- {node.location.file}:{node.location.line}")
        kind = node.kind.name
        type_ = node.type.get_canonical().spelling
        print(f"{indent}{kind} {node.spelling} type={type_}")
        for child in node.get_children():
            cls.debug(child, indent + "  ")

    def write_module(
        self, module_def: ModuleDefinition, file: TextIO = sys.stdout
    ) -> None:
        print("import enum", file=file)
        print("import datetime", file=file)
        print("import os", file=file)
        print(
            "from typing import Any, Callable, Final, Literal, Iterator, Mapping, Sequence, overload",
            file=file,
        )
        for other_module in self._modules.values():
            if other_module.fully_qualified_name != module_def.fully_qualified_name:
                print(f"import {other_module.fully_qualified_name}", file=file)
        print("", file=file)

        all = [self._enums[name].py_name for name in module_def.enums]
        all += [self._shadow_classes[name].py_name for name in module_def.classes]
        all += [py_name for py_name in module_def.functions]
        all += [py_name for py_name in module_def.constants]
        all = [name for name in sorted(all) if not name.startswith("_")]
        print(f"__all__ = {all}", file=file)
        print("", file=file)

        strip_scope = module_def.fully_qualified_name
        for enum_name in module_def.enums:
            print("", file=file)
            print("", file=file)
            enum_def = self._enums[enum_name]
            self._write_enum(enum_def, indent="", file=file)

        for class_name in module_def.classes:
            print("", file=file)
            print("", file=file)
            class_def = self._shadow_classes[class_name]
            self._write_class(class_def, indent="", strip_scope=strip_scope, file=file)

        for funcs in module_def.functions.values():
            print("", file=file)
            is_overload = len(funcs) > 1
            for func in funcs:
                if is_overload:
                    print("@overload", file=file)
                self._write_module_function(func, strip_scope=strip_scope, file=file)

        if len(module_def.constants) > 0:
            print("", file=file)
        for const in module_def.constants.values():
            self._write_module_constant(const, strip_scope=strip_scope, file=file)

    def _write_module_function(
        self, func: FunctionDefinition, *, strip_scope: str | None, file: TextIO
    ) -> None:
        name = func.py_name
        args = [
            f"{name or f'_{i}'}: {self.python_type(cpp_type, strip_scope=strip_scope)}"
            for i, (name, cpp_type) in enumerate(func.cpp_args)
        ]
        if args:
            args.append("/")
        ret_type = self.python_type(func.cpp_return_type, strip_scope=strip_scope)
        signature = func.cpp_signature
        # assert not func.doc, func.doc
        print(
            f"def {name}({', '.join(args)}) -> {ret_type}: ...  # {signature}",
            file=file,
        )

    def _write_module_constant(
        self, const: ConstDefinition, *, strip_scope: str | None, file: TextIO
    ) -> None:
        py_name = const.py_name
        py_type = self.python_type(const.cpp_type, strip_scope=strip_scope)
        if const.py_value is Ellipsis:
            py_value = "..."
        else:
            py_value = repr(const.py_value)
        print(f"{py_name}: Final[{py_type}] = {py_value}", file=file)

    def _write_class(
        self,
        class_def: ClassDefinition,
        *,
        indent: str,
        strip_scope: str | None,
        file: TextIO,
    ) -> None:
        parent = self._shadow_classes.get(class_def.parent_type)
        if parent:
            assert parent.fully_qualified_name
            bases = f"({_strip_scope(parent.fully_qualified_name, strip_scope)})"
        else:
            bases = ""

        comment = class_def.cpp_name
        if class_def.cpp_name != class_def.shadow_name:
            comment += f" (via {class_def.shadow_name})"

        print(f"{indent}class {class_def.py_name}{bases}:", file=file)
        is_first: bool = True
        indent += "    "

        if class_def.doc:
            doc = textwrap.indent(class_def.doc, indent, predicate=lambda _: True)
            print(f'{indent}"""{doc}"""', file=file)
            is_first = False

        for inner_enum in class_def.inner_enums:
            if not is_first:
                print("", file=file)
            self._write_enum(self._enums[inner_enum], indent=indent, file=file)
            is_first = False

        for inner_class in class_def.inner_classes:
            if not is_first:
                print("", file=file)
            self._write_class(
                self._shadow_classes[inner_class],
                indent=indent,
                strip_scope=strip_scope,
                file=file,
            )
            is_first = False

        is_overload = len(class_def.constructors) > 1
        for constructor in class_def.constructors:
            if not is_first:
                print("", file=file)
            if is_overload:
                print(f"{indent}@overload", file=file)
            self._write_constructor(
                constructor, indent=indent, strip_scope=strip_scope, file=file
            )
            is_first = False

        for getsetter in class_def.getsetters.values():
            if not is_first:
                print("", file=file)
            self._write_getsetter(
                getsetter, indent=indent, strip_scope=strip_scope, file=file
            )
            is_first = False

        for methods in class_def.methods.values():
            if not is_first:
                print("", file=file)
            is_overload = len(methods) > 1
            for method in methods:
                if is_overload:
                    print(f"{indent}@overload", file=file)
                self._write_method(
                    method, indent=indent, strip_scope=strip_scope, file=file
                )
                is_first = False

        for const in class_def.consts.values():
            if not is_first:
                print("", file=file)
            self._write_static_const(
                const, indent=indent, strip_scope=strip_scope, file=file
            )
            is_first = False

        if is_first:
            print(f"{indent}...", file=file)

    def _write_constructor(
        self,
        constructor: ConstructorDefinition,
        *,
        indent: str,
        strip_scope: str | None,
        file: TextIO,
    ) -> None:
        args = (
            ["self"]
            + [
                f"{name or f'_{i}'}: {self.python_type(cpp_type, strip_scope=strip_scope)}"
                for i, (name, cpp_type) in enumerate(constructor.cpp_args)
            ]
            + ["/"]
        )
        signature = constructor.cpp_signature
        print(
            f"{indent}def __init__({', '.join(args)}): ...  # {signature}",
            file=file,
        )

    def _write_getsetter(
        self,
        getsetter: GetSetterDefinition,
        *,
        indent: str,
        strip_scope: str | None,
        file: TextIO,
    ) -> None:
        name = getsetter.py_name
        get_type = getsetter.get_type
        set_type = getsetter.set_type
        value_name = getsetter.set_value_name
        py_get_type = self.python_type(get_type, strip_scope=strip_scope)
        print(f"{indent}@property", file=file)
        if getsetter.doc:
            indentmore = indent + "    "
            doc = textwrap.indent(getsetter.doc, indentmore, predicate=lambda _: True)
            doc = doc[len(indentmore) :]
            print(
                f"{indent}def {name}(self, /) -> {py_get_type}:",
                file=file,
            )
            print(f'{indentmore}"""{doc}"""', file=file)
            print(f"{indentmore}...", file=file)
        else:
            print(
                f"{indent}def {name}(self, /) -> {py_get_type}: ...  # {get_type}",
                file=file,
            )
        if set_type:
            py_set_type = self.python_type(set_type, strip_scope=strip_scope)
            print(f"{indent}@{name}.setter", file=file)
            print(
                f"{indent}def {name}(self, {value_name}: {py_set_type}, /) -> None: ...  # {set_type}",
                file=file,
            )

    def _write_method(
        self,
        method: MethodDefinition,
        *,
        indent: str,
        strip_scope: str | None,
        file: TextIO,
    ) -> None:
        name = method.py_name
        args = (
            ["cls" if method.is_static else "self"]
            + [
                f"{name or f'_{i}'}: {self.python_type(cpp_type, strip_scope=strip_scope)}"
                for i, (name, cpp_type) in enumerate(method.cpp_args)
            ]
            + ["/"]
        )
        ret_type = self.python_type(method.cpp_return_type, strip_scope=strip_scope)
        signature = method.cpp_signature
        if method.is_static:
            print(f"{indent}@classmethod", file=file)
        print(
            f"{indent}def {name}({', '.join(args)}) -> {ret_type}: ...  # {signature}",
            file=file,
        )

    def _write_static_const(
        self,
        const: StaticConstDefinition,
        *,
        indent: str,
        strip_scope: str | None,
        file: TextIO,
    ) -> None:
        py_name = const.py_name
        py_type = self.python_type(const.cpp_type, strip_scope=strip_scope)
        print(f"{indent}{py_name}: {py_type}  # {const.cpp_type}", file=file)

    def _write_enum(
        self, enum_def: EnumDefinition, *, indent: str, file: TextIO
    ) -> None:
        py_name = enum_def.py_name
        cpp_name = enum_def.cpp_name
        value_py_type = enum_def.value_py_type
        if value_py_type == "int":
            base = "enum.IntEnum"
        elif value_py_type == "str":
            base = "enum.StrEnum"
        else:
            base = "enum.Enum"
        print(f"{indent}class {py_name}({base}):  # {cpp_name}", file=file)
        print(f"{indent}    _value_: {value_py_type}", file=file)
        for name, value in enum_def.values.items():
            if value is Ellipsis:
                py_value = "..."
            else:
                py_value = repr(value)
            print(f"{indent}    {name} = {py_value}", file=file)

    def python_type(self, cpp_type: TypeInfo, *, strip_scope: str | None) -> str:
        cpp_name = cpp_type.name
        if class_def := self._cpp_classes.get(cpp_name):
            assert class_def.fully_qualified_name
            return _strip_scope(class_def.fully_qualified_name, strip_scope)
        if enum_def := self._enums.get(cpp_name):
            assert enum_def.fully_qualified_name
            return _strip_scope(enum_def.fully_qualified_name, strip_scope)
        if specializations := self._export_traits.get(cpp_name):
            # do we have a full specialization?
            if full_specialization := specializations.get(str(cpp_type)):
                if not full_specialization.template_params:
                    return full_specialization.py_type
            # do we have a partial specialization?
            if matches := [
                (len(spec.template_params), py_type)
                for spec in specializations.values()
                if (
                    py_type := spec.python_type(
                        cpp_type=cpp_type, strip_scope=strip_scope, stubgen=self
                    )
                )
            ]:
                # take the one with the fewest template parameters,
                # but make sure there's no ambiguity
                matches.sort()
                num_params, py_type = matches[0]
                if len(matches) > 1:
                    assert matches[1][0] > num_params
                return py_type
        if py_type := EXPORT_TRAITS.get(cpp_name):
            if callable(py_type):
                return py_type(self, cpp_type.args, strip_scope=strip_scope)
            return py_type
        for regex, py_type_match in EXPORT_TRAITS_REGEX:
            if match := re.match(regex, cpp_name):
                if callable(py_type_match):
                    return py_type_match(
                        self, cpp_type.args, match, strip_scope=strip_scope
                    )
                return py_type_match
        return cpp_name


def _strip_scope(fqname: str, scope: str | None) -> str:
    """
    Strip the scope from a fully qualified name.
    """
    if scope and fqname.startswith(scope + "."):
        return fqname[len(scope) + 1 :]
    return fqname


@dataclass
class ModuleDefinition:
    cpp_name: str
    py_name: str
    doc: str
    fully_qualified_name: str | None = None
    classes: list[str] = dataclasses.field(default_factory=list)
    enums: list[str] = dataclasses.field(default_factory=list)
    functions: dict[str, list[FunctionDefinition]] = dataclasses.field(
        default_factory=dict
    )
    constants: dict[str, ConstDefinition] = dataclasses.field(default_factory=dict)
    imported: bool = False

    def asdict(self) -> dict[str, Any]:
        functions: list[dict[str, Any]] = []
        for func_defs in self.functions.values():
            for func_def in func_defs:
                functions.append(func_def.asdict())
        constants = [const.asdict() for const in self.constants.values()]
        return {
            "cpp_name": self.cpp_name,
            "py_name": self.py_name,
            "doc": self.doc,
            "fully_qualified_name": self.fully_qualified_name,
            "classes": self.classes,
            "enums": self.enums,
            "functions": functions,
            "constants": constants,
            "imported": self.imported,
        }

    @classmethod
    def fromdict(cls, data: dict[str, Any]) -> Self:
        functions: dict[str, list[FunctionDefinition]] = {}
        for func in data["functions"]:
            func_def = FunctionDefinition.fromdict(func)
            functions.setdefault(func_def.py_name, []).append(func_def)
        constants: dict[str, ConstDefinition] = {}
        for const in data["constants"]:
            const_def = ConstDefinition.fromdict(const)
            constants[const_def.py_name] = const_def
        return cls(
            cpp_name=data["cpp_name"],
            py_name=data["py_name"],
            doc=data["doc"],
            fully_qualified_name=data["fully_qualified_name"],
            classes=data["classes"],
            enums=data["enums"],
            functions=functions,
            constants=constants,
            imported=data["imported"],
        )


@dataclass
class FunctionDefinition:
    py_name: str
    doc: str | None
    cpp_return_type: TypeInfo
    cpp_args: list[tuple[str, TypeInfo]]
    cpp_signature: str

    def asdict(self) -> dict[str, Any]:
        return dataclasses.asdict(self)

    @classmethod
    def fromdict(cls, data: Any) -> Self:
        cpp_args = [
            (name, TypeInfo.fromdict(type_)) for (name, type_) in data["cpp_args"]
        ]
        cpp_return_type = TypeInfo.fromdict(data["cpp_return_type"])
        return cls(
            py_name=data["py_name"],
            doc=data["doc"],
            cpp_return_type=cpp_return_type,
            cpp_args=cpp_args,
            cpp_signature=data["cpp_signature"],
        )


@dataclass
class ConstDefinition:
    py_name: str
    cpp_type: TypeInfo
    py_value: Any

    def asdict(self) -> dict[str, Any]:
        return dataclasses.asdict(self)

    @classmethod
    def fromdict(cls, data: Any) -> Self:
        cpp_type = TypeInfo.fromdict(data["cpp_type"])
        return cls(
            py_name=data["py_name"],
            cpp_type=cpp_type,
            py_value=data["py_value"],
        )


@dataclass
class ClassDefinition:
    py_name: str
    cpp_name: str
    shadow_name: str
    parent_type: str
    doc: str | None = None
    fully_qualified_name: str | None = None
    inner_classes: list[str] = dataclasses.field(default_factory=list)
    inner_enums: list[str] = dataclasses.field(default_factory=list)
    constructors: list[ConstructorDefinition] = dataclasses.field(default_factory=list)
    methods: dict[str, list[MethodDefinition]] = dataclasses.field(default_factory=dict)
    getsetters: dict[str, GetSetterDefinition] = dataclasses.field(default_factory=dict)
    consts: dict[str, StaticConstDefinition] = dataclasses.field(default_factory=dict)

    def asdict(self) -> dict[str, Any]:
        constructors: list[dict[str, Any]] = []
        for constr_def in self.constructors:
            constructors.append(constr_def.asdict())
        methods: list[dict[str, Any]] = []
        for meth_defs in self.methods.values():
            for meth_def in meth_defs:
                methods.append(meth_def.asdict())
        getsetters: list[dict[str, Any]] = []
        for getsetter in self.getsetters.values():
            getsetters.append(getsetter.asdict())
        consts: list[dict[str, Any]] = []
        for const in self.consts.values():
            consts.append(const.asdict())
        return {
            "py_name": self.py_name,
            "cpp_name": self.cpp_name,
            "shadow_name": self.shadow_name,
            "parent_type": self.parent_type,
            "doc": self.doc,
            "fully_qualified_name": self.fully_qualified_name,
            "inner_classes": self.inner_classes,
            "inner_enums": self.inner_enums,
            "constructors": constructors,
            "methods": methods,
            "getsetters": getsetters,
            "consts": consts,
        }

    @classmethod
    def fromdict(cls, data: dict[str, Any]) -> Self:
        constructors: list[ConstructorDefinition] = []
        for constr in data["constructors"]:
            constr_def = ConstructorDefinition.fromdict(constr)
            constructors.append(constr_def)
        methods: dict[str, list[MethodDefinition]] = {}
        for meth in data["methods"]:
            meth_def = MethodDefinition.fromdict(meth)
            methods.setdefault(meth_def.py_name, []).append(meth_def)
        getsetters: dict[str, GetSetterDefinition] = {}
        for getsetter in data["getsetters"]:
            getsetter_def = GetSetterDefinition.fromdict(getsetter)
            getsetters[getsetter_def.py_name] = getsetter_def
        consts: dict[str, StaticConstDefinition] = {}
        for const in data["consts"]:
            const_def = StaticConstDefinition.fromdict(const)
            consts[const_def.py_name] = const_def
        return cls(
            py_name=data["py_name"],
            cpp_name=data["cpp_name"],
            shadow_name=data["shadow_name"],
            parent_type=data["parent_type"],
            doc=data["doc"],
            fully_qualified_name=data["fully_qualified_name"],
            inner_classes=data["inner_classes"],
            inner_enums=data["inner_enums"],
            constructors=constructors,
            methods=methods,
            getsetters=getsetters,
            consts=consts,
        )


@dataclass
class EnumDefinition:
    py_name: str
    cpp_name: str
    value_py_type: str
    values: dict[str, Any]
    fully_qualified_name: str | None = None

    def asdict(self) -> dict[str, Any]:
        return dataclasses.asdict(self)

    @classmethod
    def fromdict(cls, data: Any) -> Self:
        return cls(**data)


@dataclass
class ConstructorDefinition:
    cpp_args: list[tuple[str, TypeInfo]]
    cpp_signature: str

    def asdict(self) -> dict[str, Any]:
        return dataclasses.asdict(self)

    @classmethod
    def fromdict(cls, data: Any) -> Self:
        cpp_args = [
            (name, TypeInfo.fromdict(type_)) for (name, type_) in data["cpp_args"]
        ]
        return cls(
            cpp_args=cpp_args,
            cpp_signature=data["cpp_signature"],
        )


@dataclass
class MethodDefinition:
    py_name: str
    cpp_return_type: TypeInfo
    cpp_args: list[tuple[str, TypeInfo]]
    cpp_signature: str
    is_static: bool = False

    def asdict(self) -> dict[str, Any]:
        return dataclasses.asdict(self)

    @classmethod
    def fromdict(cls, data: Any) -> Self:
        cpp_args = [
            (name, TypeInfo.fromdict(type_)) for (name, type_) in data["cpp_args"]
        ]
        cpp_return_type = data["cpp_return_type"]
        cpp_return_type = TypeInfo.fromdict(cpp_return_type)
        return cls(
            py_name=data["py_name"],
            cpp_return_type=cpp_return_type,
            cpp_args=cpp_args,
            cpp_signature=data["cpp_signature"],
            is_static=data["is_static"],
        )


@dataclass
class GetSetterDefinition:
    py_name: str
    doc: str | None
    get_type: TypeInfo
    set_type: TypeInfo | None = None
    set_value_name: str = "value"

    def asdict(self) -> dict[str, Any]:
        get_type = self.get_type.asdict()
        set_type = self.set_type.asdict() if self.set_type else None
        return {
            "py_name": self.py_name,
            "doc": self.doc,
            "get_type": get_type,
            "set_type": set_type,
            "set_value_name": self.set_value_name,
        }

    @classmethod
    def fromdict(cls, data: Any) -> Self:
        get_type = TypeInfo.fromdict(data["get_type"])
        if set_type_data := data["set_type"]:
            set_type = TypeInfo.fromdict(set_type_data)
        else:
            set_type = None
        return cls(
            py_name=data["py_name"],
            doc=data["doc"],
            get_type=get_type,
            set_type=set_type,
            set_value_name=data["set_value_name"],
        )


@dataclass
class StaticConstDefinition:
    py_name: str
    cpp_type: TypeInfo

    def asdict(self) -> dict[str, Any]:
        return dataclasses.asdict(self)

    @classmethod
    def fromdict(cls, data: Any) -> Self:
        cpp_type = TypeInfo.fromdict(data["cpp_type"])
        return cls(
            py_name=data["py_name"],
            cpp_type=cpp_type,
        )


@dataclass
class TypeInfo:
    name: str
    args: list[TypeInfo] | None = None

    def __str__(self):
        if self.args is None:
            return self.name
        return f"{self.name}<{', '.join(str(arg) for arg in self.args)}>"

    def asdict(self) -> dict[str, Any]:
        return dataclasses.asdict(self)

    @classmethod
    def fromdict(cls, data: dict[str, Any]) -> Self:
        args = data.get("args")
        if args:
            args = [TypeInfo.fromdict(arg) for arg in args]
        return cls(
            name=data["name"],
            args=args,
        )

    def substitute(self, template_params: dict[str, str]) -> TypeInfo:
        """
        Substitute template parameters in the type.
        """
        if self.name in template_params:
            assert not self.args
            return TypeInfo(template_params[self.name])
        if self.name.endswith("..."):
            # this is a variadic template
            assert self.args is None
            return TypeInfo(template_params[self.name[:-3]] + "...")
        if self.args is None:
            return self
        args = [arg.substitute(template_params) for arg in self.args]
        return TypeInfo(self.name, args)


@dataclass
class ExportTraits:
    cpp_type: TypeInfo
    py_type: str
    template_params: list[str] = dataclasses.field(default_factory=list)

    def asdict(self) -> dict[str, Any]:
        cpp_type = self.cpp_type.asdict()
        return {
            "cpp_type": cpp_type,
            "py_type": self.py_type,
            "template_params": self.template_params,
        }

    @classmethod
    def fromdict(cls, data: dict[str, Any]) -> Self:
        cpp_type = TypeInfo.fromdict(data["cpp_type"])
        return cls(
            cpp_type=cpp_type,
            py_type=data["py_type"],
            template_params=data["template_params"],
        )

    def python_type(
        self, *, cpp_type: TypeInfo, strip_scope: str | None, stubgen: Stubgen
    ) -> str | None:
        assert len(cpp_type.args) == len(self.cpp_type.args)

        # match template parameters
        matched_params: dict[str, TypeInfo | None] = {
            param: None for param in self.template_params
        }
        if not self._match_template_args(
            type_args=cpp_type.args,
            tmpl_args=self.cpp_type.args,
            matched_params=matched_params,
        ):
            return None

        # now substitute template parameters in py_type
        assert not any(arg is None for arg in matched_params.values())
        py_type = self.py_type
        for param, arg in matched_params.items():
            py_arg = stubgen.python_type(arg, strip_scope=strip_scope)
            py_type = re.sub(rf"\b{re.escape(param)}\b", py_arg, py_type)

        return py_type

    @classmethod
    def _match_template_args(
        cls,
        *,
        type_args: list[TypeInfo] | None,
        tmpl_args: list[TypeInfo] | None,
        matched_params: dict[str, TypeInfo | None],
    ) -> bool:
        """
        Match arguments of contrete type with template arguments and store the
        matched parameters.
        """
        if type_args is None and tmpl_args is None:
            return True
        assert type_args is not None and tmpl_args is not None
        assert len(type_args) == len(tmpl_args)
        for type_arg, tmpl_arg in zip(type_args, tmpl_args):
            if tmpl_arg.name in matched_params:
                # it's a template parameter
                # store the argument but check it's the same as previously matched
                assert not tmpl_arg.args
                if matched_params[tmpl_arg.name] is None:
                    matched_params[tmpl_arg.name] = type_arg
                elif matched_params[tmpl_arg.name] != type_arg:
                    return False
            else:
                # it's not a template parameter, so it need to match exactly
                if type_arg.name != tmpl_arg.name:
                    return False
                if not cls._match_template_args(
                    type_args=type_arg.args,
                    tmpl_args=tmpl_arg.args,
                    matched_params=matched_params,
                ):
                    return False
        return True


class ParseError(Exception):
    def __init__(self, errors: list[str]):
        super().__init__("\n".join(errors))
        self.errors = errors


def _is_member_ref_expr(node: cindex.Cursor, member: str) -> bool:
    return (
        node.kind == CursorKind.MEMBER_REF_EXPR
        and fully_qualified(node.referenced) == member
    )


def _ensure_shadow_class(node: cindex.Cursor) -> str:
    """
    Get the shadow class name from a _lassPyClassDef method reference
    """
    assert node.kind == CursorKind.MEMBER_REF_EXPR
    _lassPyClassDef = _ensure_only_child(node, CursorKind.DECL_REF_EXPR)
    assert _lassPyClassDef.spelling == "_lassPyClassDef"
    assert _get_type(_lassPyClassDef).spelling == "lass::python::impl::ClassDefinition"
    type_ref = _ensure_only_child(_lassPyClassDef, CursorKind.TYPE_REF)
    return _get_type(type_ref).spelling


def _ensure_kind(node: cindex.Cursor, kind: CursorKind) -> cindex.Cursor:
    assert node.kind == kind, f"expected {kind.name}, got {node.kind.name}"
    return node


def _ensure_kind_recursive(node: cindex.Cursor, kind: CursorKind) -> cindex.Cursor:
    """
    Ensure node is of kind; or unexposed in which case you recurse.
    """
    if node.kind == kind:
        return node
    assert (
        node.kind == CursorKind.UNEXPOSED_EXPR
    ), f"expected {kind.name} or UNEXPOSED_EXPR, got {node.kind.name}"
    children = list(node.get_children())
    assert len(children) == 1, f"expected 1 child, got {len(children)}"
    return _ensure_kind_recursive(children[0], kind)


def _get_first_child(node: cindex.Cursor, kind: CursorKind) -> cindex.Cursor:
    children = list(node.get_children())
    if children and children[0].kind == kind:
        return children[0]


def _ensure_first_child(node: cindex.Cursor, kind: CursorKind) -> cindex.Cursor:
    children = list(node.get_children())
    assert children, len(children)
    assert children[0].kind == kind
    return children[0]


def _get_only_child(node: cindex.Cursor, kind: CursorKind) -> cindex.Cursor | None:
    children = list(node.get_children())
    if len(children) != 1:
        return None
    child = children[0]
    if child.kind != kind:
        return None
    return child


def _ensure_only_child(
    node: cindex.Cursor, kind: cindex.Cursor | None = None
) -> cindex.Cursor:
    children = list(node.get_children())
    assert len(children) == 1, f"expected 1 child, got {len(children)}"
    if kind is not None:
        assert (
            children[0].kind == kind
        ), f"expected {kind.name}, got {children[0].kind.name}"
    return children[0]


def _ensure_only_child_recursive(
    node: cindex.Cursor, kind: CursorKind
) -> cindex.Cursor:
    children = list(node.get_children())
    assert len(children) == 1, f"expected 1 child, got {len(children)}"
    child = children[0]
    if child.kind == CursorKind.UNEXPOSED_EXPR:
        return _ensure_only_child_recursive(child, kind)
    assert (
        child.kind == kind
    ), f"expected {kind.name} or UNEXPOSED_EXPR, got {child.kind.name}"
    return child


def _get_type(node: cindex.Cursor) -> cindex.Type:
    return node.type.get_canonical()


def _ensure_type(node: cindex.Cursor, type_: str) -> cindex.Cursor:
    t = _get_type(node).spelling
    assert t == type_, f"expected {type_}, got {t}"
    return node


def _ensure_string_literal(node: cindex.Cursor) -> str:
    assert node.kind == CursorKind.STRING_LITERAL
    s = node.spelling
    assert s.startswith('"') and s.endswith('"')
    return s[1:-1]


def _ensure_string_literal_recursive(node: cindex.Cursor) -> str:
    if node.kind == CursorKind.STRING_LITERAL:
        s = node.spelling
        assert s.startswith('"') and s.endswith('"')
        return s[1:-1]
    assert node.kind == CursorKind.UNEXPOSED_EXPR
    assert node.type.spelling == "const char *"
    children = list(node.get_children())
    assert len(children) == 1
    return _ensure_string_literal_recursive(children[0])


def _ensure_string_literal_or_none_recursive(node: cindex.Cursor) -> str | None:
    if node.kind == CursorKind.STRING_LITERAL:
        s = node.spelling
        assert s.startswith('"') and s.endswith('"')
        return s[1:-1]
    if node.kind == CursorKind.INTEGER_LITERAL:
        assert node.type.spelling == "int"
        assert node.spelling == ""
        return None
    assert node.kind == CursorKind.UNEXPOSED_EXPR
    assert node.type.spelling == "const char *"
    children = list(node.get_children())
    assert len(children) == 1
    return _ensure_string_literal_or_none_recursive(children[0])


def _ensure_last_child(
    node: cindex.Cursor, kind: CursorKind | list[CursorKind], num: int | None = None
) -> cindex.Cursor:
    children = list(node.get_children())
    if num is not None:
        assert len(children) == num, f"expected {num} children, got {len(children)}"
    if isinstance(kind, CursorKind):
        kind = [kind]
    assert (
        children[-1].kind in kind
    ), f"expected {'|'.join(k.name for k in kind)}, got {children[-1].kind.name}"
    return children[-1]


def _ensure_last_child_recursive(
    node: cindex.Cursor, kind: CursorKind | list[CursorKind]
) -> cindex.Cursor:
    """ "
    Return last child, ensuring it's one of kind."
    If child is unexposed, recurse and find the last child of that.
    """
    if isinstance(kind, CursorKind):
        kind = [kind]
    assert CursorKind.UNEXPOSED_EXPR not in kind, "Can't search of UNEXPOSED_EXPR"
    children = list(node.get_children())
    assert children, "Must have children"
    last_child = children[-1]
    if last_child.kind == CursorKind.UNEXPOSED_EXPR:
        return _ensure_last_child_recursive(last_child, kind)
    assert (
        last_child.kind in kind
    ), f"expected {'|'.join(k.name for k in kind)}, got {last_child.kind.name}"
    return last_child


def _find_first_child(node: cindex.Cursor, kind: CursorKind) -> cindex.Cursor | None:
    for child in node.get_children():
        if child.kind == kind:
            return child
    return None


def _iter_children(node: cindex.Cursor, kind: CursorKind):
    for child in node.get_children():
        if child.kind == kind:
            yield child


def fully_qualified(node: cindex.Cursor) -> str:
    if node is None:
        return ""
    elif node.kind == CursorKind.TRANSLATION_UNIT:
        return ""
    elif parent := fully_qualified(node.semantic_parent):
        return f"{parent}::{node.spelling}"
    return node.spelling


def type_info(type_: cindex.Type) -> TypeInfo:
    type_ = type_.get_canonical()

    pointee = type_.get_pointee()
    if pointee.kind != TypeKind.INVALID and type_.kind != TypeKind.POINTER:
        pointee_type = type_info(pointee)
        return pointee_type

    num_args = type_.get_num_template_arguments()
    if num_args >= 0:
        args = [type_info(type_.get_template_argument_type(i)) for i in range(num_args)]
    else:
        args = None

    # print(f"{type_.spelling=}, {type_.kind=}, const={type_.is_const_qualified()} volatile={type_.is_volatile_qualified()}, {args=}")
    # if type_.kind == TypeKind.RECORD:
    #    decl = type_.get_declaration()
    if (decl := type_.get_declaration()) and decl.spelling:
        spelling = fully_qualified(decl)
    else:
        spelling = re.sub(r"\bconst\b", "", type_.spelling)
        spelling = re.sub(r"\bvolatile\b", "", spelling)
        spelling = spelling.strip()

    return TypeInfo(spelling, args)


SPECIAL_METHODS = {
    "lass::python::methods::_call_": "__call__",
    "lass::python::methods::_repr_": "__repr__",
    "lass::python::methods::_str_": "__str__",
    "lass::python::methods::_neg_": "__neg__",
    "lass::python::methods::_pos_": "__pos__",
    "lass::python::methods::_abs_": "__abs__",
    "lass::python::methods::_invert_": "__invert__",
    "lass::python::methods::_int_": "__int__",
    "lass::python::methods::_long_": "__long__",
    "lass::python::methods::_float_": "__float__",
    "lass::python::methods::_add_": "__add__",
    "lass::python::methods::_sub_": "__sub__",
    "lass::python::methods::_mul_": "__mul__",
    "lass::python::methods::_mod_": "__mod__",
    "lass::python::methods::_lshift_": "__lshift__",
    "lass::python::methods::_rshift_": "__rshift__",
    "lass::python::methods::_and_": "__and__",
    "lass::python::methods::_xor_": "__xor__",
    "lass::python::methods::_or_": "__or__",
    "lass::python::methods::_iadd_": "__iadd__",
    "lass::python::methods::_isub_": "__isub__",
    "lass::python::methods::_imul_": "__imul__",
    "lass::python::methods::_imod_": "__imod__",
    "lass::python::methods::_ilshift_": "__ilshift__",
    "lass::python::methods::_irshift_": "__irshift__",
    "lass::python::methods::_iand_": "__iand__",
    "lass::python::methods::_ixor_": "__ixor__",
    "lass::python::methods::_ior_": "__ior__",
    "lass::python::methods::_truediv_": "__truediv__",
    "lass::python::methods::_itruediv_": "__itruediv__",
    "lass::python::methods::_floordiv_": "__floordiv__",
    "lass::python::methods::_ifloordiv_": "__ifloordiv__",
    "lass::python::methods::_bool_": "__bool__",
    "lass::python::methods::_pow_": "__pow__",
    "lass::python::methods::_lt_": "__lt__",
    "lass::python::methods::_le_": "__le__",
    "lass::python::methods::_eq_": "__eq__",
    "lass::python::methods::_ne_": "__ne__",
    "lass::python::methods::_gt_": "__gt__",
    "lass::python::methods::_ge_": "__ge__",
    "lass::python::methods::_len_": "__len__",
    "lass::python::methods::_getitem_": "__getitem__",
    "lass::python::methods::_setitem_": "__setitem__",
    "lass::python::methods::seq_len_": "__len__",
    "lass::python::methods::_concat_": "__concat__",
    "lass::python::methods::_repeat_": "__repeat__",
    "lass::python::methods::seq_getitem_": "__getitem__",
    "lass::python::methods::seq_setitem_": "__setitem__",
    "lass::python::methods::_contains_": "__contains__",
    "lass::python::methods::_iconcat_": "__iconcat__",
    "lass::python::methods::_irepeat_": "__irepeat__",
    "lass::python::methods::map_len_": "__len__",
    "lass::python::methods::map_getitem_": "__getitem__",
    "lass::python::methods::map_setitem_": "__setitem__",
    "lass::python::methods::map_delitem_": "__delitem__",
    "lass::python::methods::_iter_": "__iter__",
    "lass::python::methods::next": "next",
}


ExportTraitsTyper: TypeAlias = Callable[[Stubgen, list[TypeInfo] | None], str]


def _pytype_sequence(
    stubgen: Stubgen, args: list[TypeInfo] | None, *, strip_scope: str | None
) -> str:
    assert args and len(args) >= 1, args
    element_type = stubgen.python_type(args[0], strip_scope=strip_scope)
    return f"Sequence[{element_type}]"


def _pytype_mapping(
    stubgen: Stubgen, args: list[TypeInfo] | None, *, strip_scope: str | None
) -> str:
    assert args and len(args) >= 2, args
    key_type = stubgen.python_type(args[0], strip_scope=strip_scope)
    value_type = stubgen.python_type(args[1], strip_scope=strip_scope)
    return f"Mapping[{key_type}, {value_type}]"


EXPORT_TRAITS: dict[str, str | ExportTraitsTyper] = {
    "void": "None",
    "std::vector": _pytype_sequence,
    "std::list": _pytype_sequence,
    "std::deque": _pytype_sequence,
    "std::map": _pytype_mapping,
    "lass::python::PyIteratorRange *": "Iterator[Any]",
    "lass::stde::static_vector": _pytype_sequence,
    "lass::stde::vector_map": _pytype_mapping,
}

ExportTraitsTyperMatch: TypeAlias = Callable[
    [Stubgen, list[TypeInfo] | None, re.Match], str
]


EXPORT_TRAITS_REGEX: list[tuple[re.Pattern[str], str | ExportTraitsTyperMatch]] = [
    (re.compile(r"char\[\d+\]"), "str"),
    (re.compile(r"wchar_t\[\d+\]"), "str"),
]


if __name__ == "__main__":
    sys.exit(main(sys.argv[1:]))
