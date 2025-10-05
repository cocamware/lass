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

import re
import sysconfig
from collections.abc import Collection, Iterator
from pathlib import Path
from typing import TYPE_CHECKING, Any, NamedTuple, cast

from clang import cindex  # type: ignore
from clang.cindex import AccessSpecifier, Cursor, CursorKind, TypeKind  # type: ignore

from .stubdata import (
    ClassDefinition,
    ConstDefinition,
    ConstructorDefinition,
    EnumDefinition,
    ExportTraits,
    FunctionDefinition,
    GetSetterDefinition,
    MethodDefinition,
    ModuleDefinition,
    ParamInfo,
    StubData,
    TypeInfo,
)

if TYPE_CHECKING:
    from _typeshed import StrPath

__all__ = ["ParseError", "Parser"]


class Parser:
    def __init__(
        self,
        *,
        package: str | None = None,
        include_dirs: list[StrPath] | None = None,
        system_include_dirs: list[StrPath] | None = None,
        defines: list[str] | None = None,
        args: list[str] | None = None,
        object_files_map: dict[str, Path] | None = None,
        pch_path: StrPath | None = None,
        libclang_library: StrPath | None = None,
    ) -> None:
        self.stubdata = StubData(package=package)
        self.include_dirs = include_dirs or []
        self.system_include_dirs = system_include_dirs or []
        self.defines = defines or []
        self.args = args or []
        self.object_files_map = object_files_map or {}
        self.pch_path = pch_path

        if libclang_library and not cindex.Config.loaded:
            cindex.Config.set_library_file(str(libclang_library))

        self._handlers = {
            "ModuleDefinition": self._handle_declare_module,
            "ClassDefinition": self._handle_declare_class,
            "IntEnumDefinition": self._handle_declare_enum,
            "IntFlagDefinition": self._handle_declare_enum,
            "EnumDefinition": self._handle_declare_enum,
            "addFunctionDispatcher": self._handle_module_add_function,
            "addClass": self._handle_module_add_class,
            "addEnum": self._handle_module_add_enum,
            "injectObject": self._handle_module_inject_object,
            "injectClass": self._handle_module_inject_class,
            "injectLong": self._handle_module_inject_constant,
            "injectString": self._handle_module_inject_constant,
            "addIntegerConstantsToModule": self._handle_module_add_integer_constants,
            "addLong": self._handle_module_add_long,
            "addString": self._handle_module_add_string,
            "addConstructor": self._handle_class_add_constructor,
            "addConverter": self._handle_class_add_converter,
            "addMethod": self._handle_class_add_method,
            "addStaticMethod": self._handle_class_add_static_method,
            "addGetSetter": self._handle_class_add_get_setter,
            "addStaticConst": self._handle_class_add_static_const,
            "addInnerClass": self._handle_class_add_inner_class,
            "addInnerEnum": self._handle_class_add_inner_enum,
        }

    def parse(self, path: StrPath, *, save_pch: bool = False) -> None:
        args = [f"-I{include}" for include in self.include_dirs]
        args.append(f"-I{sysconfig.get_path('include')}")
        args += [f"-isystem{include}" for include in self.system_include_dirs]
        args += [f"-D{define}" for define in self.defines]
        args += ["-DLASS_STUBGEN=1"]
        args += self.args or []

        # Add the precompiled header if provided
        if save_pch:
            if not self.pch_path:
                raise ValueError("pch_path must be provided when saving PCH.")
            args += ["-xc++"]
        elif self.pch_path:
            args += ["-include-pch", str(self.pch_path)]

        index = cindex.Index.create()
        try:
            tu = index.parse(path, args=args)
        except cindex.TranslationUnitLoadError as err:
            raise ParseError(str(err)) from None

        errors = [
            str(diag)
            for diag in tu.diagnostics
            if diag.severity >= cindex.Diagnostic.Error
        ]
        if errors:
            raise ParseError(errors)

        self.stubdata.add_included_files(
            path, (include.location.file.name for include in tu.get_includes())
        )

        if save_pch:
            assert self.pch_path
            ast_file = Path(self.pch_path)
            ast_file.parent.mkdir(parents=True, exist_ok=True)
            tu.save(str(ast_file))
            return  # don't bother scanning the AST

        visitor = NodeVisitor(self)
        visitor.visit_node(tu.cursor, tu.cursor, tu)
        if visitor.error:
            raise visitor.error

    def _handle_declare_module(self, node: Cursor) -> bool:
        assert node.kind == CursorKind.CALL_EXPR
        if canonical_type(node).spelling != "lass::python::ModuleDefinition":
            return False

        var_decl = node.semantic_parent
        if not var_decl:
            if tu := node.translation_unit:
                for n in tu.cursor.get_children():
                    if n.kind == CursorKind.VAR_DECL and n.location == node.location:
                        var_decl = n
                        break
        ensure_kind(var_decl, CursorKind.VAR_DECL)
        cpp_name = fully_qualified(var_decl)

        args = list(node.get_arguments())
        py_name = self._parse_name(args[0])
        doc = self._parse_doc(args[1])

        self.stubdata.add_module_definition(
            ModuleDefinition(
                cpp_name=cpp_name,
                py_name=py_name,
                doc=doc,
            )
        )
        return True

    def _handle_declare_class(self, node: Cursor) -> bool:
        assert node.kind == CursorKind.CALL_EXPR
        if canonical_type(node).spelling != "lass::python::impl::ClassDefinition":
            return False

        args = list(node.get_arguments())

        py_name = self._parse_name(args[0])
        doc = self._parse_doc(args[1])

        # the fifth argument is the parent class definition
        arg4 = ensure_kind(args[4], CursorKind.UNARY_OPERATOR)
        parent_def = ensure_only_child(arg4, CursorKind.DECL_REF_EXPR).referenced
        parent_type = type_info(parent_def.semantic_parent)

        # we're assuming the last argument is _lassPyClassRegisterHook
        # which is defined on the C++ type, and is the way we can get to the C++ type
        arg5 = ensure_kind(args[5], CursorKind.UNARY_OPERATOR)
        register_hook = ensure_only_child(arg5, CursorKind.DECL_REF_EXPR)
        assert register_hook.spelling == "_lassPyClassRegisterHook"
        shadow_node = ensure_last_child(register_hook, CursorKind.TYPE_REF)
        shadow_decl = canonical_type(shadow_node).get_declaration()
        shadow_type = type_info(shadow_node)

        for base in iter_children(shadow_decl, CursorKind.CXX_BASE_SPECIFIER):
            # if this is a shadow class, then we derive from ShadowClass
            # which will have the C++ class as first argument.
            base_type = type_info(base)
            if base_type.name == "lass::python::ShadowClass":
                assert base_type.args and shadow_type == base_type.args[0], (
                    f"{shadow_type=}, {base_type=}"
                )
                cpp_type = base_type.args[1]

                # our C++ class is a template argument of the ShadowClass base class
                # search for it as a type reference in the base class
                for type_ref in iter_children(base, CursorKind.TYPE_REF):
                    if type_info(type_ref.referenced) == cpp_type:
                        cpp_class = type_ref.referenced
                        if cpp_class.kind == CursorKind.TYPEDEF_DECL:
                            cpp_class = canonical_type(cpp_class).get_declaration()
                        assert cpp_class.kind in {
                            CursorKind.CLASS_DECL,
                            CursorKind.STRUCT_DECL,
                        }

                        if cpp_type.args:
                            # it's in fact a template, and cpp_class is just the
                            # template instantation, which will have an empty body.
                            # There's no way to get the template class directly, we
                            # can only match it on *location* from the semantic parent.
                            assert not list(cpp_class.get_children())
                            for template in iter_children(
                                cpp_class.semantic_parent, CursorKind.CLASS_TEMPLATE
                            ):
                                if template.location == cpp_class.location:
                                    cpp_class = template
                                    break
                            else:
                                assert False
                        assert list(cpp_class.get_children())

                        break
                else:
                    # it's not mentioned as a TYPE_REF, it'll be a TEMPLATE_REF
                    # However, those TEMPLATE_REFs don't have a type that we can
                    # compare with cpp_type.
                    # So we're first going to the base type to find our C++ class as
                    # a template argument. It should always be the second one.
                    base_type_ = canonical_type(base)
                    n = base_type_.get_num_template_arguments()
                    for i in range(n):
                        tmpl_arg = base_type_.get_template_argument_type(i)
                        if type_info(tmpl_arg) == cpp_type:
                            assert i == 1  # it should always be the second argument
                            cpp_class = tmpl_arg.get_declaration()
                            assert cpp_class.kind in {
                                CursorKind.CLASS_DECL,
                                CursorKind.STRUCT_DECL,
                            }
                            break
                    else:
                        assert False

                    # cpp_class is the instantiated template, which will be empty.
                    # We can use the cpp_class *location* to match it with a
                    # TEMPLATE_REF from the base class.
                    assert not list(cpp_class.get_children())
                    assert cpp_type.args  # it's a template
                    for template_ref in iter_children(base, CursorKind.TEMPLATE_REF):
                        template = template_ref.referenced
                        if template.location == cpp_class.location:
                            cpp_class = template
                            break
                    else:
                        assert False
                    assert cpp_class.kind == CursorKind.CLASS_TEMPLATE
                    assert list(cpp_class.get_children())

                break
        else:
            # there's no shadow class, the C++ class already inherits from PyObjectPlus
            cpp_class = shadow_node.referenced
            if cpp_class.kind == CursorKind.TYPEDEF_DECL:
                cpp_class = canonical_type(cpp_class).get_declaration()
            assert cpp_class.kind in {CursorKind.CLASS_DECL, CursorKind.STRUCT_DECL}
            cpp_type = type_info(cpp_class)
            assert cpp_type.args is None

        # Gather signatures of all constructors in the C++ class, so we can match them
        # later with the exported constructors to get the correct parameter names.
        cpp_constructors: dict[str, ConstructorDefinition] = {}
        if cpp_class:
            template_args: dict[str, TypeInfo] = {}
            if cpp_class.kind == CursorKind.CLASS_TEMPLATE:
                # match template parameters with arguments of cpp_type.
                tmpl_params = list(
                    iter_children(cpp_class, CursorKind.TEMPLATE_TYPE_PARAMETER)
                )
                assert cpp_type.args and len(tmpl_params) == len(cpp_type.args)
                for param, arg in zip(tmpl_params, cpp_type.args):
                    template_args[canonical_type(param).spelling] = arg
            elif cpp_class.kind in {CursorKind.CLASS_DECL, CursorKind.STRUCT_DECL}:
                pass
            else:
                self._error(
                    cpp_class, "Expected CLASS_DECL, STRUCT_DECL or CLASS_TEMPLATE"
                )

            for constructor in iter_children(cpp_class, CursorKind.CONSTRUCTOR):
                if constructor.access_specifier != AccessSpecifier.PUBLIC:
                    continue
                params = list(constructor.get_arguments())
                cpp_params = [
                    ParamInfo(p.spelling, type_info(p).substitute(template_args))
                    for p in params
                ]

                # figure out how many parameters are required (i.e., without defaults)
                has_default_value: list[bool] = [
                    any(t.spelling == "=" for t in p.get_tokens()) for p in params
                ]
                num_required: int = -1
                for i, has_default in enumerate(has_default_value):
                    if num_required == -1:
                        if has_default:
                            num_required = i
                    else:
                        assert has_default, (
                            "non default parameter after default parameter?"
                        )
                if num_required == -1:
                    num_required = len(params)

                cpp_signature = f"void ({', '.join(str(t) for _, t in cpp_params)})"
                cpp_constructors[cpp_signature] = ConstructorDefinition(
                    cpp_params, cpp_signature, num_required=num_required
                )

        self.stubdata.add_class_definition(
            ClassDefinition(
                py_name=py_name,
                cpp_type=cpp_type,
                shadow_type=shadow_type,
                parent_type=parent_type,
                doc=doc,
                _cpp_constructors=cpp_constructors,
            )
        )

        return True

    def _handle_declare_enum(self, node: Cursor) -> bool:
        assert node.kind == CursorKind.CALL_EXPR
        enum_definition = type_info(node)
        if enum_definition.name == "lass::python::IntEnumDefinition":
            assert node.spelling == "IntEnumDefinition"
            base_py_type = "enum.IntEnum"
            value_py_type = "int"
        elif enum_definition.name == "lass::python::StrEnumDefinition":
            assert node.spelling == "EnumDefinition"
            base_py_type = "enum.StrEnum"
            value_py_type = "str"
        elif enum_definition.name == "lass::python::EnumDefinition":
            assert node.spelling == "EnumDefinition"
            base_py_type = "enum.Enum"
            value_py_type = "Any"
        elif enum_definition.name == "lass::python::IntFlagDefinition":
            assert node.spelling in ("IntEnumDefinition", "IntFlagDefinition")
            base_py_type = "enum.IntFlag"
            value_py_type = "int"
        else:
            return False

        assert enum_definition.args and enum_definition.args[0].args is None
        enum_type = enum_definition.args[0]

        args = list(node.get_arguments())
        assert 0 < len(args) <= 4

        py_name = self._parse_name(args[0])
        doc: str | None = None
        boundary: str | None = None
        init_list: Iterator[Cursor] = iter([])

        if len(args) == 1:
            pass
        if len(args) == 2:
            arg = args[1]
            while arg.kind.is_unexposed():
                arg = ensure_only_child(arg)
            if arg.kind == CursorKind.INIT_LIST_EXPR:
                doc = None
                init_list = arg.get_children()
            else:
                doc = self._parse_doc(arg)
                init_list = iter([])
        elif len(args) == 3:
            doc = self._parse_doc(args[1])
            init_list = ensure_kind(args[2], CursorKind.INIT_LIST_EXPR).get_children()
        elif len(args) == 4:
            doc = self._parse_doc(args[1])
            bound = ensure_kind(args[2], CursorKind.DECL_REF_EXPR).referenced
            ensure_kind(bound, CursorKind.ENUM_CONSTANT_DECL)
            assert canonical_type(bound).spelling == "lass::python::FlagBoundary"
            boundary = f"enum.{bound.spelling.upper()}"
            init_list = ensure_kind(args[3], CursorKind.INIT_LIST_EXPR).get_children()

        values: dict[str, Any] = {}
        for init_list_expr in init_list:
            args = list(init_list_expr.get_children())
            # zeroth argument is the name of the enum value.
            key = string_literal(args[0])
            value: Any
            if value_py_type == "int":
                # first argument is the cpp value _and_ the python value
                # (there is no second argument)
                value_ref = ensure_kind(args[1], CursorKind.DECL_REF_EXPR)
                value_decl = value_ref.referenced
                assert value_decl.kind == CursorKind.ENUM_CONSTANT_DECL
                value = int(value_decl.enum_value)
            elif value_py_type == "str":
                # first argument is the cpp value, which we ignore
                # the second argument is a string that we want as python value
                value = string_literal(args[2])
            else:
                value = Ellipsis
            values[key] = value

        self.stubdata.add_enum_definition(
            EnumDefinition(
                py_name=py_name,
                cpp_type=enum_type,
                base_py_type=base_py_type,
                boundary=boundary,
                value_py_type=value_py_type,
                values=values,
                doc=doc,
                preamble=["import enum"],
            )
        )
        return True

    def _handle_module_add_function(self, node: Cursor) -> bool:
        assert node.kind == CursorKind.CALL_EXPR
        children = list(node.get_children())
        if not is_member_ref_expr(
            children[0], "lass::python::ModuleDefinition::addFunctionDispatcher"
        ):
            return False
        module_def = self._parse_module_ref(children[0])

        dispatcher_ref = ensure_only_child(children[1], CursorKind.DECL_REF_EXPR)
        dispatcher = self._parse_dispatcher_ref(dispatcher_ref)
        assert not dispatcher.is_free_method, (
            "Module functions never use free method call"
        )

        py_name = self._parse_name(children[2])
        doc = self._parse_doc(children[3])

        module_def.add_function(
            FunctionDefinition(
                py_name=py_name,
                doc=doc,
                cpp_return_type=dispatcher.cpp_return_type,
                cpp_params=dispatcher.cpp_params,
                cpp_signature=dispatcher.cpp_signature,
            )
        )

        return True

    def _handle_module_add_class(self, node: Cursor) -> bool:
        assert node.kind == CursorKind.CALL_EXPR
        children = list(node.get_children())
        if not is_member_ref_expr(
            children[0], "lass::python::ModuleDefinition::addClass"
        ):
            return False
        module_def = self._parse_module_ref(children[0])

        shadow_type = self._parse_class_ref(children[1])

        module_def.add_class(shadow_type=shadow_type)

        return True

    def _handle_module_add_enum(self, node: Cursor) -> bool:
        assert node.kind == CursorKind.CALL_EXPR
        children = list(node.get_children())
        if not is_member_ref_expr(
            children[0], "lass::python::ModuleDefinition::addEnum"
        ):
            return False
        module_def = self._parse_module_ref(children[0])

        unary_op = ensure_only_child(children[1], CursorKind.UNARY_OPERATOR)
        decl_ref = ensure_only_child(unary_op, CursorKind.DECL_REF_EXPR)
        enum_definition = type_info(decl_ref)
        assert enum_definition.args, f"{enum_definition=} must have template args"
        enum_type = enum_definition.args[0]

        module_def.add_enum(enum_type)

        return True

    def _handle_module_add_long(self, node: Cursor) -> bool:
        assert node.kind == CursorKind.CALL_EXPR
        children = list(node.get_children())
        if not is_member_ref_expr(
            children[0], "lass::python::ModuleDefinition::addLong"
        ):
            return False
        location = node.location.file.name.replace("\\", "/")
        if location.endswith("/lass/python/bulk_add_integer.inl"):
            return False

        module_def = self._parse_module_ref(children[0])

        _, value = self._parse_constant(children[1])
        py_name = self._parse_name(children[2])

        module_def.add_constant(
            ConstDefinition(py_name=py_name, cpp_type=TypeInfo("int"), value=value)
        )
        return True

    def _handle_module_add_string(self, node: Cursor) -> bool:
        assert node.kind == CursorKind.CALL_EXPR
        children = list(node.get_children())
        if not is_member_ref_expr(
            children[0], "lass::python::ModuleDefinition::addString"
        ):
            return False
        module_def = self._parse_module_ref(children[0])

        _, value = self._parse_constant(children[1])
        py_name = self._parse_name(children[2])

        module_def.add_constant(
            ConstDefinition(py_name=py_name, cpp_type=TypeInfo("str"), value=value)
        )
        return True

    def _handle_module_inject_object(self, node: Cursor) -> bool:
        assert node.kind == CursorKind.CALL_EXPR
        children = list(node.get_children())
        if not is_member_ref_expr(
            children[0], "lass::python::ModuleDefinition::injectObject"
        ):
            return False
        module_def = self._parse_module_ref(children[0])

        obj = ensure_kind(
            children[1],
            {
                CursorKind.DECL_REF_EXPR,
                CursorKind.CALL_EXPR,
                CursorKind.CXX_FUNCTIONAL_CAST_EXPR,
            },
        )
        cpp_type = type_info(obj)

        py_name = self._parse_name(children[2])

        module_def.add_constant(ConstDefinition(py_name=py_name, cpp_type=cpp_type))

        return True

    def _handle_module_inject_class(self, node: Cursor) -> bool:
        assert node.kind == CursorKind.CALL_EXPR
        children = list(node.get_children())
        if not is_member_ref_expr(
            children[0], "lass::python::ModuleDefinition::injectClass"
        ):
            return False
        module_def = self._parse_module_ref(children[0])

        shadow_type = self._parse_class_ref(children[1])

        module_def.add_class(shadow_type=shadow_type)

        return True

    def _handle_module_inject_constant(self, node: Cursor) -> bool:
        assert node.kind == CursorKind.CALL_EXPR
        children = list(node.get_children())
        if not is_member_ref_expr(
            children[0],
            {
                "lass::python::ModuleDefinition::injectLong",
                "lass::python::ModuleDefinition::injectString",
            },
        ):
            return False
        module_def = self._parse_module_ref(children[0])

        py_name = self._parse_name(children[1])

        _, value = self._parse_constant(children[2])

        cpp_type = {
            "injectLong": TypeInfo("int"),
            "injectString": TypeInfo("str"),
        }[node.spelling]

        module_def.add_constant(
            ConstDefinition(py_name=py_name, cpp_type=cpp_type, value=value)
        )
        return True

    def _handle_module_add_integer_constants(self, node: Cursor) -> bool:
        assert node.kind == CursorKind.CALL_EXPR
        children = list(node.get_children())
        decl_ref_expr = ensure_kind(children[0], CursorKind.DECL_REF_EXPR)
        if (
            fully_qualified(decl_ref_expr.referenced)
            != "lass::python::impl::addIntegerConstantsToModule"
        ):
            return False

        args = list(node.get_arguments())

        # first argument is the module definition
        module_def = self._parse_module_ref(args[0])

        # second argument is the name of the constants as a call to basic_string
        names_str = self._parse_name(args[1])
        names = [name.strip() for name in names_str.split(",")]

        # then there should follow as many arguments as there are names:
        assert len(args) == 2 + len(names)
        for name, arg in zip(names, args[2:]):
            decl_ref_expr = ensure_kind(arg, CursorKind.DECL_REF_EXPR)
            enum_constant = ensure_kind(
                decl_ref_expr.referenced, CursorKind.ENUM_CONSTANT_DECL
            )
            value = enum_constant.enum_value
            module_def.add_constant(
                ConstDefinition(py_name=name, cpp_type=TypeInfo("long"), value=value)
            )

        return True

    def _handle_class_add_constructor(self, node: Cursor) -> bool:
        assert node.kind == CursorKind.CALL_EXPR
        children = list(node.get_children())
        if not is_member_ref_expr(
            children[0], "lass::python::impl::ClassDefinition::addConstructor"
        ):
            return False
        class_def = self._get_class_def(children[0])

        dispatcher_arg = children[1]
        dispatcher_ref = ensure_only_child(dispatcher_arg, CursorKind.DECL_REF_EXPR)
        ensure_type(dispatcher_ref, "_object *(_typeobject *, _object *, _object *)")
        dispatcher = dispatcher_ref.referenced

        compound = ensure_last_child(dispatcher, CursorKind.COMPOUND_STMT)
        return_stmt = ensure_last_child(compound, CursorKind.RETURN_STMT)
        call_expr = ensure_only_child(return_stmt, CursorKind.CALL_EXPR)

        if call_expr.spelling == "callConstructor":
            call_constructor_ptr = ensure_first_child(
                call_expr, CursorKind.UNEXPOSED_EXPR
            )
            call_constructor_ref = ensure_only_child(
                call_constructor_ptr, CursorKind.DECL_REF_EXPR
            )
            assert call_constructor_ref.spelling == "callConstructor"

            t_params_ref = ensure_last_child(
                call_constructor_ref, [CursorKind.TYPE_REF, CursorKind.TEMPLATE_REF]
            )
            t_params_type = type_info(t_params_ref)

            t_params_args = t_params_type.args or tuple()
            cpp_params = [ParamInfo("", arg) for arg in t_params_args]
            cpp_signature = f"void ({', '.join(map(str, t_params_args))})"

            class_def.add_constructor(
                ConstructorDefinition(
                    cpp_params=cpp_params,
                    cpp_signature=cpp_signature,
                )
            )

        elif call_expr.spelling == "callFunction":
            # Free function as construtor
            args = list(call_expr.get_arguments())
            func = deref_decl_ref_expr(args[1])
            cpp_signature = canonical_type(func).spelling
            cpp_params = [
                ParamInfo(arg.spelling, type_info(arg)) for arg in func.get_arguments()
            ]

            class_def.add_constructor(
                ConstructorDefinition(
                    cpp_params=cpp_params,
                    cpp_signature=cpp_signature,
                    free=True,
                )
            )

        else:
            self._debug(node)
            assert False, call_expr.spelling

        return True

    def _handle_class_add_converter(self, node: Cursor) -> bool:
        assert node.kind == CursorKind.CALL_EXPR
        children = list(node.get_children())

        addConverter_ref = children[0]
        while addConverter_ref.kind.is_unexposed():
            addConverter_ref = ensure_only_child(addConverter_ref)
        if addConverter_ref.kind != CursorKind.DECL_REF_EXPR:
            return False
        addConverter = addConverter_ref.referenced
        if (
            fully_qualified(addConverter)
            != "lass::python::impl::ShadowTraits::addConverter"
        ):
            return False

        converter_ref = children[1]
        while (
            converter_ref.kind.is_unexposed()
            or converter_ref.kind == CursorKind.PAREN_EXPR
        ):
            converter_ref = ensure_only_child(converter_ref)
        ensure_kind(converter_ref, CursorKind.DECL_REF_EXPR)
        if (
            fully_qualified(converter_ref.referenced)
            != "lass::python::impl::defaultConvertor"
        ):
            return False

        converter = converter_ref.referenced
        assert converter.get_num_template_arguments() == 2
        shadow_type = type_info(converter.get_template_argument_type(0))
        source_type = type_info(converter.get_template_argument_type(1))

        assert shadow_type.name, shadow_type
        assert not shadow_type.args, shadow_type

        class_def = self._get_shadow_class_def(shadow_type)
        class_def.add_implicit_converter(source_type)

        return True

    def _handle_class_add_method(self, node: Cursor) -> bool:
        assert node.kind == CursorKind.CALL_EXPR
        children = list(node.get_children())
        if not is_member_ref_expr(
            children[0], "lass::python::impl::ClassDefinition::addMethod"
        ):
            return False
        class_def = self._get_class_def(children[0])

        name_arg = children[1]
        if canonical_type(name_arg).spelling == "const char *":
            py_name = string_literal(ensure_only_child(name_arg))
        else:
            assert name_arg.kind == CursorKind.DECL_REF_EXPR
            method = fully_qualified(name_arg.referenced)
            py_name = SPECIAL_METHODS[method]

        doc = self._parse_doc(children[2])

        dispatcher_arg = children[3]
        fun = ensure_only_child(dispatcher_arg, CursorKind.DECL_REF_EXPR)
        fun_children = list(fun.get_children())
        assert fun_children[-1].kind == CursorKind.UNEXPOSED_EXPR
        dispatcher_ref = ensure_only_child(fun_children[-1], CursorKind.DECL_REF_EXPR)
        dispatcher = self._parse_dispatcher_ref(dispatcher_ref)

        cpp_params = dispatcher.cpp_params
        if dispatcher.is_free_method:
            # free method normally has self as first parameter, let's strip it.
            # Except if it's the signature of a reflected operator like __radd__,
            # then the second parameter is self, and we need to keep the first parameter
            # __pow__ is a bit special as it can have three parameters.
            self_type = self._self_type(cpp_params[0])
            if (
                py_rname := REFLECTED_OPERATORS.get(py_name)
            ) and self_type.name != class_def.cpp_type.name:
                assert len(cpp_params) == 2 or (
                    len(cpp_params) == 3 and py_name == "__pow__"
                )
                other_type = self._self_type(cpp_params[1])
                assert other_type.name == class_def.cpp_type.name, (
                    f"Binary op type mismatch: neither {self_type} or {other_type} is "
                    f"{class_def.cpp_type}."
                )
                py_name = py_rname
                cpp_params = [cpp_params[0]] + cpp_params[2:]
            else:
                assert self_type.name == class_def.cpp_type.name, (
                    f"self type mismatch: {self_type} is not {class_def.cpp_type}."
                )
                cpp_params = cpp_params[1:]

        class_def.add_method(
            MethodDefinition(
                py_name=py_name,
                doc=doc,
                cpp_return_type=dispatcher.cpp_return_type,
                cpp_params=cpp_params,
                cpp_signature=dispatcher.cpp_signature,
            )
        )

        return True

    def _self_type(self, param: tuple[str, TypeInfo]) -> TypeInfo:
        _, self_type = param
        if self_type.is_pointer:
            self_type = self_type.base_type
        if self_type.name == "lass::util::SharedPtr":
            assert self_type.args
            self_type = self_type.args[0]
        return self_type

    def _handle_class_add_static_method(self, node: Cursor) -> bool:
        assert node.kind == CursorKind.CALL_EXPR
        children = list(node.get_children())
        if not is_member_ref_expr(
            children[0], "lass::python::impl::ClassDefinition::addStaticMethod"
        ):
            return False
        class_def = self._get_class_def(children[0])

        py_name = self._parse_name(children[1])
        doc = self._parse_doc(children[2])

        dispatcher_arg = children[3]
        dispatcher_ref = ensure_only_child(dispatcher_arg, CursorKind.DECL_REF_EXPR)
        dispatcher = self._parse_dispatcher_ref(dispatcher_ref)
        assert not dispatcher.is_free_method, "Static methods are use free method call"

        class_def.add_method(
            MethodDefinition(
                py_name=py_name,
                doc=doc,
                cpp_return_type=dispatcher.cpp_return_type,
                cpp_params=dispatcher.cpp_params,
                cpp_signature=dispatcher.cpp_signature,
                is_static=True,
            )
        )

        return True

    def _handle_class_add_get_setter(self, node: Cursor) -> bool:
        assert node.kind == CursorKind.CALL_EXPR
        children = list(node.get_children())
        if not is_member_ref_expr(
            children[0], "lass::python::impl::ClassDefinition::addGetSetter"
        ):
            return False
        class_def = self._get_class_def(children[0])

        py_name = self._parse_name(children[1])
        doc = self._parse_doc(children[2])

        # getter dispatcher
        dispatch = ensure_only_child(children[3], CursorKind.DECL_REF_EXPR).referenced
        compound = ensure_last_child(dispatch, CursorKind.COMPOUND_STMT)
        if return_stmt := _find_first_child(compound, CursorKind.RETURN_STMT):
            call_expr = ensure_only_child(return_stmt, CursorKind.CALL_EXPR)
            if call_expr.spelling == "freeGet":
                args = list(call_expr.get_children())
                getter = ensure_only_child(args[2], CursorKind.DECL_REF_EXPR).referenced
                get_type = type_info(canonical_type(getter).get_result())
            else:
                # public member
                assert call_expr.spelling == "pyBuildSimpleObject"
                member_ref = _find_first_child(call_expr, CursorKind.MEMBER_REF_EXPR)
                assert member_ref
                get_type = type_info(member_ref)
        else:
            try_stmt = ensure_only_child(compound, CursorKind.CXX_TRY_STMT)
            try_compound = ensure_first_child(try_stmt, CursorKind.COMPOUND_STMT)
            if_stmt = _find_first_child(try_compound, CursorKind.IF_STMT)
            if_compound = ensure_last_child(if_stmt, CursorKind.COMPOUND_STMT)
            return_stmt = ensure_only_child(if_compound, CursorKind.RETURN_STMT)
            call_builder = _ensure_last_child_recursive(
                return_stmt, CursorKind.CALL_EXPR
            )
            assert call_builder.spelling == "pyBuildSimpleObject"
            call_getter = _ensure_last_child_recursive(
                call_builder, CursorKind.CALL_EXPR
            )
            get_type = type_info(call_getter)

        # optional setter dispatcher
        if dispatch_ref := _get_only_child(children[4], CursorKind.DECL_REF_EXPR):
            dispatch = dispatch_ref.referenced
            compound = ensure_last_child(dispatch, CursorKind.COMPOUND_STMT)
            return_stmt = ensure_last_child(compound, CursorKind.RETURN_STMT)
            if unary_min := _get_only_child(return_stmt, CursorKind.UNARY_OPERATOR):
                call_expr = ensure_only_child(unary_min, CursorKind.CALL_EXPR)
                member_ref = ensure_last_child(call_expr, CursorKind.MEMBER_REF_EXPR)
                set_type = type_info(member_ref)
                set_value_name = py_name
            else:
                call_expr = ensure_only_child(return_stmt, CursorKind.CALL_EXPR)
                args = list(call_expr.get_children())
                setter = deref_decl_ref_expr(args[3])
                params = list(iter_children(setter, CursorKind.PARM_DECL))
                if call_expr.spelling == "set":
                    if params:
                        # it's a tradional setter, with value as only parameter
                        assert len(params) == 1, params
                        value = params[0]
                        set_type = type_info(value)
                        set_value_name = value.spelling or py_name
                    else:
                        # it returns a non-const reference to the property
                        set_type = type_info(canonical_type(setter).get_result())
                        set_value_name = py_name
                elif call_expr.spelling == "freeSet":
                    # first parameter is self, second is value
                    assert len(params) == 2, params
                    value = params[1]
                    set_type = type_info(value)
                    set_value_name = value.spelling or py_name
                else:
                    self._debug(dispatch)
                    assert False, call_expr.spelling
        else:
            set_type = None
            set_value_name = None

        class_def.add_getsetter(
            GetSetterDefinition(
                py_name=py_name,
                doc=doc,
                get_type=get_type,
                set_type=set_type,
                set_value_name=set_value_name,
            )
        )

        return True

    def _handle_class_add_static_const(self, node: Cursor) -> bool:
        assert node.kind == CursorKind.CALL_EXPR
        children = list(node.get_children())
        if not is_member_ref_expr(
            children[0], "lass::python::impl::ClassDefinition::addStaticConst"
        ):
            return False
        class_def = self._get_class_def(children[0])

        py_name = self._parse_name(children[1])
        cpp_type, value = self._parse_constant(children[2])

        class_def.add_const(
            ConstDefinition(py_name=py_name, cpp_type=cpp_type, value=value)
        )

        return True

    def _handle_class_add_inner_class(self, node: Cursor) -> bool:
        assert node.kind == CursorKind.CALL_EXPR
        children = list(node.get_children())
        if not is_member_ref_expr(
            children[0], "lass::python::impl::ClassDefinition::addInnerClass"
        ):
            return False
        class_def = self._get_class_def(children[0])

        # first argument is the class definition of the inner class
        type_ref = ensure_only_child(children[1], CursorKind.TYPE_REF)
        inner_class = type_info(type_ref)

        class_def.add_inner_class(inner_class)

        return True

    def _handle_class_add_inner_enum(self, node: Cursor) -> bool:
        assert node.kind == CursorKind.CALL_EXPR
        children = list(node.get_children())
        if not is_member_ref_expr(
            children[0], "lass::python::impl::ClassDefinition::addInnerEnum"
        ):
            return False
        class_def = self._get_class_def(children[0])

        unary_op = ensure_only_child(children[1], CursorKind.UNARY_OPERATOR)
        decl_ref = ensure_only_child(unary_op, CursorKind.DECL_REF_EXPR)
        enum_definition = type_info(decl_ref)
        assert enum_definition.args, f"{enum_definition=} must have template args"
        enum_type = enum_definition.args[0]

        assert enum_type in self.stubdata.enums
        enum_def = self.stubdata.enums[enum_type]
        assert "." not in enum_def.py_name

        class_def.add_enum(enum_type)

        return True

    def _handle_export_traits(self, node: Cursor) -> bool:
        if node.kind in (CursorKind.TEMPLATE_REF, CursorKind.CLASS_TEMPLATE):
            return False
        assert fully_qualified(node) == "lass::python::PyExportTraits"
        type_ = type_info(node)
        if type_.name != "lass::python::PyExportTraits":
            return False
        assert node.kind in (
            CursorKind.CLASS_TEMPLATE_PARTIAL_SPECIALIZATION,
            CursorKind.STRUCT_DECL,
        ), node.kind
        assert type_.args and len(type_.args) == 1
        cpp_type = type_.args[0]

        def find_py_typing(n: Cursor, spelling: str) -> str | None:
            for child in iter_children(n, CursorKind.VAR_DECL):
                if child.spelling == spelling:
                    typing = _ensure_only_child_recursive(
                        child, CursorKind.STRING_LITERAL
                    )
                    return string_literal(typing)

            # look in base class
            if cxx_base_specifier := _find_first_child(
                n, CursorKind.CXX_BASE_SPECIFIER
            ):
                try:
                    template_ref = ensure_first_child(
                        cxx_base_specifier,
                        CursorKind.TEMPLATE_REF,
                        skip=CursorKind.NAMESPACE_REF,
                    )
                except AssertionError:
                    self._debug(cxx_base_specifier)
                    raise
                base = template_ref.referenced
                for child in iter_children(base, CursorKind.VAR_DECL):
                    if child.spelling == spelling:
                        typing = _ensure_only_child_recursive(
                            child, CursorKind.STRING_LITERAL
                        )
                        return string_literal(typing)

            return None

        py_type = find_py_typing(node, "py_typing")
        if not py_type:
            return False
        py_type_param = find_py_typing(node, "py_typing_param")
        preamble_str = find_py_typing(node, "py_typing_preamble")
        if preamble_str:
            preamble = [line for line in preamble_str.splitlines() if line]
        else:
            preamble = []

        if node.kind == CursorKind.STRUCT_DECL:
            # A full specialization
            self.stubdata.add_export_traits(
                ExportTraits(
                    cpp_type=cpp_type,
                    py_type=py_type,
                    py_type_param=py_type_param,
                    preamble=tuple(preamble),
                )
            )

        elif node.kind == CursorKind.CLASS_TEMPLATE_PARTIAL_SPECIALIZATION:
            # A partial specialization

            # intermediate_params maps template parameters like type-parameter-0-0
            # to the template parameters of the specialization like T
            intermediate_params: dict[str, str] = {}
            for param in iter_children(node, CursorKind.TEMPLATE_TYPE_PARAMETER):
                intermediate_params[canonical_type(param).spelling] = param.spelling

            cpp_type = cpp_type.substitute(intermediate_params)

            self.stubdata.add_export_traits(
                ExportTraits(
                    cpp_type=cpp_type,
                    py_type=py_type,
                    py_type_param=py_type_param,
                    preamble=tuple(preamble),
                    template_params=tuple(intermediate_params.values()),
                )
            )

            return True

        else:
            self._debug(node)
            assert False, f"Unhandled node kind {node.kind}"

        return True

    @classmethod
    def _debug(cls, node: Cursor, indent: str = "") -> None:
        if not indent:
            print(f"---- {node.location.file}:{node.location.line}")
        kind = node.kind.name
        type_ = node.type.get_canonical().spelling
        print(f"{indent}{kind} {node.spelling} type={type_}")
        for child in node.get_children():
            cls._debug(child, indent + "  ")

    @classmethod
    def _error(cls, node: Cursor, message: str) -> None:
        cls._debug(node)
        raise AssertionError(message)

    def _parse_module_ref(self, node: Cursor) -> ModuleDefinition:
        if node.kind == CursorKind.MEMBER_REF_EXPR:
            module_ref = ensure_only_child(node, CursorKind.DECL_REF_EXPR)
        else:
            module_ref = ensure_kind(node, CursorKind.DECL_REF_EXPR)
        assert canonical_type(module_ref).spelling == "lass::python::ModuleDefinition"
        module_name = fully_qualified(module_ref.referenced)
        try:
            return self.stubdata.modules[module_name]
        except KeyError:
            raise ParseError(
                rf"""Cannot find module definition for '{module_name}'.
Make sure you add functions and other properties (with the PY_MODULE_* macros) 
in the same source file as the module declaration (the PY_DECLARE_MODULE* macro).
You must use the same identifier for the module in both macros, you cannot use a
reference to a module defined in another file.
"""
            ) from None

    def _get_class_def(self, node: Cursor) -> ClassDefinition:
        """
        Get the class definition from a _lassPyClassDef method call.
        """
        _lassPyClassDef = ensure_only_child(node, CursorKind.DECL_REF_EXPR)
        shadow_type = self._parse_class_ref(_lassPyClassDef)
        return self._get_shadow_class_def(shadow_type)

    def _get_shadow_class_def(self, shadow_type: TypeInfo) -> ClassDefinition:
        """
        Get the class definition from the shadow class name.
        """
        try:
            return self.stubdata.shadow_classes[shadow_type]
        except KeyError:
            raise ParseError(
                rf"""Cannot find class definition for '{shadow_type}'.
Make sure you add functions and other properties (with the PY_CLASS_* macros) 
in the same source file as the class declaration (the PY_DECLARE_CLASS* macro).
You must use the same identifier for the class in both macros, you cannot use a
reference to a class defined in another file.
"""
            ) from None

    def _parse_class_ref(self, node: Cursor) -> TypeInfo:
        """
        Get the shadow class name from a _lassPyClassDef reference.
        """
        ensure_kind(node, CursorKind.DECL_REF_EXPR)
        assert node.spelling == "_lassPyClassDef"
        assert canonical_type(node).spelling == "lass::python::impl::ClassDefinition"
        shadow_node = node.referenced.semantic_parent
        shadow_type = type_info(shadow_node)
        return shadow_type

    def _parse_name(self, node: Cursor) -> str:
        s = self._parse_string(node)
        assert s is not None
        return s

    def _parse_doc(self, node: Cursor) -> str | None:
        return self._parse_string(node)

    def _parse_string(self, node: Cursor) -> str | None:
        if node.kind.is_unexposed():
            assert node.type.spelling in (
                "const char *",
                "std::string",
                "const std::string",
            )
            children = list(node.get_children())
            if not children:
                return None
            assert len(children) == 1
            return self._parse_string(children[0])
        if node.kind == CursorKind.CALL_EXPR:
            assert node.spelling == "basic_string"
            children = list(node.get_children())
            return self._parse_string(children[0])
        if node.kind == CursorKind.INTEGER_LITERAL:
            assert node.type.spelling == "int"
            assert node.spelling == ""
            return None
        if node.kind == CursorKind.CXX_NULL_PTR_LITERAL_EXPR:
            return None
        return string_literal(node)

    def _parse_constant(self, node: Cursor) -> tuple[TypeInfo, str | int | None]:
        while node.kind.is_unexposed():
            node = ensure_only_child(node)
        cpp_type = type_info(node)
        value: str | int | None
        if node.kind == CursorKind.STRING_LITERAL:
            value = string_literal(node)
        elif node.kind == CursorKind.INTEGER_LITERAL:
            try:
                value = int(node.spelling)
            except ValueError:
                value = None
        else:
            value = None
        return cpp_type, value

    @classmethod
    def _parse_dispatcher_ref(
        cls,
        dispatcher_ref: Cursor,
    ) -> DispatcherSignature:
        assert (
            canonical_type(dispatcher_ref).spelling == "_object *(_object *, _object *)"
        )
        dispatcher = dispatcher_ref.referenced

        compound = ensure_last_child(dispatcher, CursorKind.COMPOUND_STMT)
        return_stmt = ensure_last_child(compound, CursorKind.RETURN_STMT)
        call_expr = _ensure_only_child_recursive(return_stmt, CursorKind.CALL_EXPR)
        assert call_expr.spelling in (
            "call",
            "callFunction",
            "callMethod",
            "callFree",
            "callFreeMethod",
        ), call_expr.spelling

        call_args = list(call_expr.get_arguments())
        assert call_args, f"{call_expr.spelling} ({canonical_type(call_expr).spelling})"

        func_arg = call_args[-1]
        while func_arg.kind in [CursorKind.UNEXPOSED_EXPR, CursorKind.UNARY_OPERATOR]:
            func_arg = ensure_only_child(func_arg)
        if func_arg.kind == CursorKind.CALL_EXPR:
            func = func_arg
        else:
            func = deref_decl_ref_expr(func_arg)

        cpp_signature = canonical_type(func).spelling

        func_type = type_info(func)
        if func_type.name == "std::function":
            assert func_type.args and len(func_type.args) == 1, f"{func_type.args=}"
            func_type = func_type.args[0]
            cpp_return_type = func_type.result
            assert cpp_return_type, f"{func_type=} must have a result"
            cpp_params = [ParamInfo("", arg) for arg in func_type.args or []]
        elif func.kind == CursorKind.VAR_DECL:
            func = ensure_only_child(func, CursorKind.LAMBDA_EXPR)
            cpp_return_type = TypeInfo("Any")
            cpp_params = [
                ParamInfo(arg.spelling, type_info(arg))
                for arg in iter_children(func, CursorKind.PARM_DECL)
            ]
        else:
            cpp_return_type = type_info(func.type.get_result())
            cpp_params = [
                ParamInfo(arg.spelling, type_info(arg)) for arg in func.get_arguments()
            ]

        if "lambda at python_embedding.cpp" in cpp_signature:
            print(f"{func_type=}")
            print(f"{cpp_return_type=}")
            print(f"{cpp_params=}")
            cls._debug(func)
            assert False

        is_free_method = call_expr.spelling in ("callFree", "callFreeMethod")

        return DispatcherSignature(
            cpp_signature, cpp_return_type, cpp_params, is_free_method
        )


class ParseError(Exception):
    def __init__(self, errors: str | list[str]):
        if isinstance(errors, str):
            errors = [errors]
        super().__init__(errors)
        self.errors = errors

    def __str__(self) -> str:
        return "\n".join(self.errors)


class DispatcherSignature(NamedTuple):
    cpp_signature: str
    cpp_return_type: TypeInfo
    cpp_params: list[ParamInfo]
    is_free_method: bool = False


class NodeVisitor:
    __slots__ = (
        "parser",
        "error",
        "_handlers",
        "_visitor_cb",
        "_clang_visitChildren",
        "_clang_getCursorSpelling",
        "_kind_ids",
    )

    def __init__(self, parser: Parser) -> None:
        self.parser = parser
        self.error: BaseException | None = None
        self._handlers = parser._handlers
        self._clang_visitChildren = cindex.conf.lib.clang_visitChildren

        if cursor_visit_callback := getattr(cindex, "cursor_visit_callback", None):
            # clang >= 20
            self._visitor_cb = cursor_visit_callback(self.visit_node)

            def _clang_getCursorSpelling(
                cursor: Cursor,
                clang_getCursorSpelling: Any = cindex.conf.lib.clang_getCursorSpelling,
            ) -> str:
                return cast(
                    str, cindex._CXString.from_result(clang_getCursorSpelling(cursor))
                )

            self._clang_getCursorSpelling = _clang_getCursorSpelling
        else:
            self._visitor_cb = cindex.callbacks["cursor_visit"](self.visit_node)
            self._clang_getCursorSpelling = cindex.conf.lib.clang_getCursorSpelling

        self._kind_ids = (
            2,  # STRUCT_DECL
            32,  # CLASS_TEMPLATE_PARTIAL_SPECIALIZATION
            103,  # CALL_EXPR
        )

    def visit_node(
        self, node: Cursor, _parent: Cursor, tu: cindex.TranslationUnit
    ) -> int:
        try:
            if node._kind_id in self._kind_ids:
                kind_id = node._kind_id
                if kind_id == 103:  # CALL_EXPR
                    spelling = self._clang_getCursorSpelling(node)
                    if handler := self._handlers.get(spelling):
                        node._tu = tu
                        if handler(node):
                            return 1
                elif (
                    kind_id == 2  # STRUCT_DECL
                    or kind_id == 32  # CLASS_TEMPLATE_PARTIAL_SPECIALIZATION
                ):
                    if self._clang_getCursorSpelling(node) == "PyExportTraits":
                        node._tu = tu
                        if self.parser._handle_export_traits(node):
                            return 1
            self._clang_visitChildren(node, self._visitor_cb, tu)
            if self.error:
                return 0
            return 1
        except BaseException as err:
            if isinstance(err, AssertionError):
                self.parser._debug(node)
            if node.location:
                err.add_note(f"While parsing {node.location.file}:{node.location.line}")
            self.error = err
            return 0


def is_member_ref_expr(node: Cursor, member: str | Collection[str]) -> bool:
    if isinstance(member, str):
        member = {member}
    return (
        node.kind == CursorKind.MEMBER_REF_EXPR
        and fully_qualified(node.referenced) in member
    )


def ensure_kind(node: Cursor, kind: CursorKind | Collection[CursorKind]) -> Cursor:
    """
    Ensure node is of kind; or unexposed in which case you recurse.
    """
    if isinstance(kind, CursorKind):
        kind = [kind]
    if node.kind in kind:
        return node
    assert node.kind == CursorKind.UNEXPOSED_EXPR, (
        f"expected {'|'.join(k.name for k in kind)}|UNEXPOSED_EXPR, got {node.kind.name}"
    )
    children = list(node.get_children())
    assert len(children) == 1, f"expected 1 child, got {len(children)}"
    return ensure_kind(children[0], kind)


def _get_first_child(
    node: Cursor, kind: CursorKind, *, skip: CursorKind | None = None
) -> Cursor | None:
    children = list(node.get_children())
    if skip is not None:
        children = [c for c in children if c.kind != skip]
    if children and children[0].kind == kind:
        return children[0]
    return None


def ensure_first_child(
    node: Cursor, kind: CursorKind, *, skip: CursorKind | None = None
) -> Cursor:
    children = list(node.get_children())
    if skip is not None:
        children = [c for c in children if c.kind != skip]
    assert children, len(children)
    assert children[0].kind == kind
    return children[0]


def _get_only_child(node: Cursor, kind: CursorKind) -> Cursor | None:
    children = list(node.get_children())
    if len(children) != 1:
        return None
    child = children[0]
    if child.kind != kind:
        return None
    return child


def ensure_only_child(node: Cursor, kind: CursorKind | None = None) -> Cursor:
    children = list(node.get_children())
    assert len(children) == 1, f"expected 1 child, got {len(children)}"
    if kind is not None:
        assert children[0].kind == kind, (
            f"expected {kind.name}, got {children[0].kind.name}"
        )
    return children[0]


def _ensure_only_child_recursive(node: Cursor, kind: CursorKind) -> Cursor:
    children = list(node.get_children())
    assert len(children) == 1, f"expected 1 child, got {len(children)}"
    child = children[0]
    if child.kind == CursorKind.UNEXPOSED_EXPR:
        return _ensure_only_child_recursive(child, kind)
    assert child.kind == kind, (
        f"expected {kind.name} or UNEXPOSED_EXPR, got {child.kind.name}"
    )
    return child


def canonical_type(node: Cursor) -> cindex.Type:
    return node.type.get_canonical()


def ensure_type(node: Cursor, type_: str) -> Cursor:
    t = canonical_type(node).spelling
    assert t == type_, f"expected {type_}, got {t}"
    return node


def string_literal(node: Cursor) -> str:
    assert node.kind == CursorKind.STRING_LITERAL
    s: str = node.spelling
    assert s.startswith('"') and s.endswith('"')
    s = s[1:-1]
    s = s.replace(r"\n", "\n")
    s = s.replace(r"\t", "\t")
    s = s.replace(r"\r", "\r")
    s = s.replace(r"\"", '"')
    return s


def deref_decl_ref_expr(node: Cursor) -> Cursor:
    while node.kind != CursorKind.DECL_REF_EXPR:
        assert node.kind in [CursorKind.UNEXPOSED_EXPR, CursorKind.UNARY_OPERATOR]
        node = ensure_only_child(node)
    return node.referenced


def ensure_last_child(node: Cursor, kind: CursorKind | list[CursorKind]) -> Cursor:
    if isinstance(kind, CursorKind):
        kind = [kind]
    children = list(node.get_children())
    assert children[-1].kind in kind, (
        f"expected {'|'.join(k.name for k in kind)}, got {children[-1].kind.name}"
    )
    return children[-1]


def _ensure_last_child_recursive(
    node: Cursor, kind: CursorKind | list[CursorKind]
) -> Cursor:
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
    assert last_child.kind in kind, (
        f"expected {'|'.join(k.name for k in kind)}, got {last_child.kind.name}"
    )
    return last_child


def _find_first_child(node: Cursor, kind: CursorKind) -> Cursor | None:
    for child in node.get_children():
        if child.kind == kind:
            return child
    return None


def iter_children(
    node: Cursor, kind: CursorKind | list[CursorKind]
) -> Iterator[Cursor]:
    if isinstance(kind, CursorKind):
        kind = [kind]
    for child in node.get_children():
        if child.kind in kind:
            yield child


def fully_qualified(node: Cursor) -> str:
    if node is None:
        return ""
    elif node.kind == CursorKind.TRANSLATION_UNIT:
        return ""
    elif parent := fully_qualified(node.semantic_parent):
        return f"{parent}::{node.spelling}"
    return cast(str, node.spelling)


def type_info(node_or_type: Cursor | cindex.Type) -> TypeInfo:
    type_ = node_or_type if isinstance(node_or_type, cindex.Type) else node_or_type.type
    type_ = type_.get_canonical()

    if type_.kind == TypeKind.FUNCTIONPROTO:
        return_type = type_info(type_.get_result())
        param_types = [type_info(arg) for arg in type_.argument_types()]
        return TypeInfo("", param_types, return_type)

    pointee = type_.get_pointee()
    if pointee.kind != TypeKind.INVALID:
        pointee_type = type_info(pointee)
        if type_.kind == TypeKind.POINTER:
            if pointee_type.result is not None:
                assert not pointee_type.name, pointee_type
                return pointee_type
            assert pointee_type.name, pointee_type
            return TypeInfo(f"{pointee_type.name} *", pointee_type.args)
        return pointee_type

    num_args = type_.get_num_template_arguments()
    if num_args >= 0:
        args = tuple(
            type_info(type_.get_template_argument_type(i)) for i in range(num_args)
        )
    else:
        args = None

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
    "lass::python::methods::_matmul_": "__matmul__",
    "lass::python::methods::_imatmul_": "__imatmul__",
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

REFLECTED_OPERATORS = {
    "__add__": "__radd__",
    "__sub__": "__rsub__",
    "__mul__": "__rmul__",
    "__mod__": "__rmod__",
    "__divmod__": "__rdivmod__",
    "__pow__": "__rpow__",
    "__lshift__": "__rlshift__",
    "__rshift__": "__rrshift__",
    "__and__": "__rand__",
    "__xor__": "__rxor__",
    "__or__": "__ror__",
    "__floordiv__": "__rfloordiv__",
    "__truediv__": "__rtruediv__",
    "__matmul__": "__rmatmul__",
}
