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
from collections.abc import Callable, Iterator
from pathlib import Path

from clang import cindex  # type: ignore
from clang.cindex import AccessSpecifier, CursorKind, TypeKind  # type: ignore

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
    StrPath,
    StubData,
    TypeInfo,
)

__all__ = ["Parser", "ParseError"]


class Parser:
    stubdata: StubData
    includes: list[StrPath]
    defines: list[str]
    args: list[str]
    object_files_map: dict[str, Path]
    cache_dir: StrPath

    _handlers: dict[str, Callable[[cindex.Cursor], bool]]

    def __init__(
        self,
        *,
        package: str | None = None,
        includes: list[StrPath] | None = None,
        defines: list[str] | None = None,
        args: list[str] | None = None,
        object_files_map: dict[str, Path] | None = None,
        cache_dir: StrPath | None = None,
    ) -> None:
        self.stubdata = StubData(package=package)
        self.includes = includes or []
        self.defines = defines or []
        self.args = args or []
        self.object_files_map = object_files_map or {}
        self.cache_dir = cache_dir or Path.cwd() / ".lass_stubgen_cache"

        self._handlers = {
            "ModuleDefinition": self._handle_declare_module,
            "ClassDefinition": self._handle_declare_class,
            "IntEnumDefinition": self._handle_declare_enum,
            "EnumDefinition": self._handle_declare_enum,
            "addFunctionDispatcher": self._handle_module_add_function,
            "addClass": self._handle_module_add_class,
            "addEnum": self._handle_module_add_enum,
            "injectObject": self._handle_module_inject_object,
            "addIntegerConstantsToModule": self._handle_module_add_integer_constants,
            "addLong": self._handle_module_add_long,
            "addConstructor": self._handle_class_add_constructor,
            "addMethod": self._handle_class_add_method,
            "addStaticMethod": self._handle_class_add_static_method,
            "addGetSetter": self._handle_class_add_get_setter,
            "addStaticConst": self._handle_class_add_static_const,
            "addInnerClass": self._handle_class_add_inner_class,
            "addInnerEnum": self._handle_class_add_inner_enum,
        }

    def parse(self, path: StrPath) -> None:
        args = [f"-I{include}" for include in self.includes]
        args.append(f"-I{sysconfig.get_path('include')}")
        args += [f"-D{define}" for define in self.defines]
        args += self.args or []

        index = cindex.Index.create()
        try:
            tu = index.parse(path, args=args)
        except cindex.TranslationUnitLoadError as err:
            raise ParseError([str(err)]) from None

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
            if handler := self._handlers.get(node.spelling):
                if handler(node):
                    return
        if node.spelling == "PyExportTraits":
            if self._handle_export_traits(node):
                return

        for child in node.get_children():
            self._parse_node(child)

    def _handle_declare_module(self, node: cindex.Cursor) -> bool:
        assert node.kind == CursorKind.CALL_EXPR
        if canonical_type(node).spelling != "lass::python::ModuleDefinition":
            return False

        var_decl = ensure_kind(node.semantic_parent, CursorKind.VAR_DECL)
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

    def _handle_declare_class(self, node: cindex.Cursor) -> bool:
        assert node.kind == CursorKind.CALL_EXPR
        if canonical_type(node).spelling != "lass::python::impl::ClassDefinition":
            return False

        args = list(node.get_arguments())

        py_name = self._parse_name(args[0])
        doc = self._parse_doc(args[1])

        # the fifth argument is the parent class definition
        arg4 = ensure_kind(args[4], CursorKind.UNARY_OPERATOR)
        parent_def = ensure_only_child(arg4, CursorKind.DECL_REF_EXPR).referenced
        parent_type = fully_qualified(parent_def.semantic_parent)

        # we're assuming the last argument is _lassPyClassRegisterHook
        # which is defined on the C++ type, and is the way we can get to the C++ type
        arg5 = ensure_kind(args[5], CursorKind.UNARY_OPERATOR)
        register_hook = ensure_only_child(arg5, CursorKind.DECL_REF_EXPR)
        assert register_hook.spelling == "_lassPyClassRegisterHook"
        shadow_node = ensure_only_child(register_hook, CursorKind.TYPE_REF)
        shadow_type = canonical_type(shadow_node)
        shadow_name: str = shadow_type.spelling

        shadow_decl = shadow_type.get_declaration()
        for base in iter_children(shadow_decl, CursorKind.CXX_BASE_SPECIFIER):
            # if this is a shadow class, then we derive from ShadowClass
            # which will have the C++ class as first argument.
            base_type = type_info(base)
            if base_type.name == "lass::python::ShadowClass":
                assert base_type.args and shadow_name == base_type.args[0].name, (
                    f"{shadow_name=}, {base_type=}"
                )
                cpp_type = base_type.args[1]

                # our C++ class is a template argument of the ShadowClass base class
                # search for it as a type reference in the base class
                for type_ref in iter_children(base, CursorKind.TYPE_REF):
                    if type_info(type_ref.referenced) == cpp_type:
                        cpp_class = type_ref.referenced
                        if cpp_class.kind == CursorKind.TYPEDEF_DECL:
                            cpp_class = canonical_type(cpp_class).get_declaration()
                        assert cpp_class.kind == CursorKind.CLASS_DECL

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
                            assert cpp_class.kind == CursorKind.CLASS_DECL
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
            assert cpp_class.kind == CursorKind.CLASS_DECL
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
            elif cpp_class.kind == CursorKind.CLASS_DECL:
                pass
            else:
                self._error(cpp_class, "Expected CLASS_DECL or CLASS_TEMPLATE")

            for constructor in iter_children(cpp_class, CursorKind.CONSTRUCTOR):
                if constructor.access_specifier != AccessSpecifier.PUBLIC:
                    continue
                params = list(iter_children(constructor, CursorKind.PARM_DECL))
                cpp_params = [
                    (p.spelling, type_info(p).substitute(template_args)) for p in params
                ]
                cpp_signature = f"void ({', '.join(str(t) for _, t in cpp_params)})"
                cpp_constructors[cpp_signature] = ConstructorDefinition(
                    cpp_params, cpp_signature
                )

        self.stubdata.add_class_definition(
            ClassDefinition(
                py_name=py_name,
                cpp_type=cpp_type,
                shadow_name=shadow_name,
                parent_type=parent_type,
                doc=doc,
                _cpp_constructors=cpp_constructors,
            )
        )

        return True

    def _handle_declare_enum(self, node: cindex.Cursor) -> bool:
        assert node.kind == CursorKind.CALL_EXPR
        enum_type = type_info(node)
        if enum_type.name == "lass::python::IntEnumDefinition":
            assert node.spelling == "IntEnumDefinition"
            value_py_type = "int"
        elif enum_type.name == "lass::python::StrEnumDefinition":
            assert node.spelling == "EnumDefinition"
            value_py_type = "str"
        elif enum_type.name == "lass::python::EnumDefinition":
            assert node.spelling == "EnumDefinition"
            value_py_type = "Any"
        else:
            return False
        assert enum_type.args and enum_type.args[0].args is None
        cpp_name = enum_type.args[0].name

        children = list(node.get_children())

        # zeroth child is the enum name
        py_name = self._parse_name(children[0])

        # first child is the initializer list
        init_list = ensure_kind(children[1], CursorKind.INIT_LIST_EXPR)
        values = {}
        for init_list_expr in init_list.get_children():
            args = list(init_list_expr.get_children())
            # zeroth argument is the name of the enum value.
            key = string_literal(args[0])
            value: str | int | None
            if value_py_type == "int":
                # first argument is the cpp value _and_ the python value
                # (there is no second argument)
                value_ref = ensure_kind(args[1], CursorKind.DECL_REF_EXPR)
                value_decl = value_ref.referenced
                assert value_decl.kind == CursorKind.ENUM_CONSTANT_DECL
                ensure_only_child(value_decl, CursorKind.INTEGER_LITERAL)
                value = int(value_decl.enum_value)
            elif value_py_type == "str":
                # first argument is the cpp value, which we ignore
                # the second argument is a string that we want as python value
                value = string_literal(args[2])
            else:
                value = None
            values[key] = value

        self.stubdata.add_enum_definition(
            EnumDefinition(
                py_name=py_name,
                cpp_name=cpp_name,
                value_py_type=value_py_type,
                values=values,
            )
        )
        return True

    def _handle_module_add_function(self, node: cindex.Cursor) -> bool:
        assert node.kind == CursorKind.CALL_EXPR
        children = list(node.get_children())
        if not is_member_ref_expr(
            children[0], "lass::python::ModuleDefinition::addFunctionDispatcher"
        ):
            return False
        module_def = self._parse_module_ref(children[0])

        dispatcher_ref = ensure_only_child(children[1], CursorKind.DECL_REF_EXPR)
        cpp_signature, cpp_return_type, cpp_params = self._handle_dispatcher_ref(
            dispatcher_ref
        )

        py_name = self._parse_name(children[2])
        doc = self._parse_doc(children[3])

        module_def.add_function(
            FunctionDefinition(
                py_name=py_name,
                doc=doc,
                cpp_return_type=cpp_return_type,
                cpp_params=cpp_params,
                cpp_signature=cpp_signature,
            )
        )

        return True

    def _handle_module_add_class(self, node: cindex.Cursor) -> bool:
        assert node.kind == CursorKind.CALL_EXPR
        children = list(node.get_children())
        if not is_member_ref_expr(
            children[0], "lass::python::ModuleDefinition::addClass"
        ):
            return False
        module_def = self._parse_module_ref(children[0])

        shadow_class = self._parse_class_ref(children[1])

        module_def.add_class(shadow_class=shadow_class)

        return True

    def _handle_module_add_enum(self, node: cindex.Cursor) -> bool:
        assert node.kind == CursorKind.CALL_EXPR
        children = list(node.get_children())
        if not is_member_ref_expr(
            children[0], "lass::python::ModuleDefinition::addEnum"
        ):
            return False
        module_def = self._parse_module_ref(children[0])

        unary_op = ensure_only_child(children[1], CursorKind.UNARY_OPERATOR)
        decl_ref = ensure_only_child(unary_op, CursorKind.DECL_REF_EXPR)
        enum_type = type_info(decl_ref)
        assert enum_type.args, f"{enum_type=} must have template args"
        cpp_name = enum_type.args[0].name
        assert cpp_name in self.stubdata.enums

        module_def.add_enum(cpp_name)

        return True

    def _handle_module_add_long(self, node: cindex.Cursor) -> bool:
        assert node.kind == CursorKind.CALL_EXPR
        children = list(node.get_children())
        if not is_member_ref_expr(
            children[0], "lass::python::ModuleDefinition::addLong"
        ):
            return False
        # module_def = self._parse_module_ref(children[0])
        parent = node.semantic_parent
        if not parent:
            return False

        self._debug(node)
        assert False

    def _handle_module_inject_object(self, node: cindex.Cursor) -> bool:
        assert node.kind == CursorKind.CALL_EXPR
        children = list(node.get_children())
        if not is_member_ref_expr(
            children[0], "lass::python::ModuleDefinition::injectObject"
        ):
            return False
        module_def = self._parse_module_ref(children[0])

        decl_ref_expr = ensure_kind(children[1], CursorKind.DECL_REF_EXPR)
        cpp_type = type_info(decl_ref_expr)

        py_name = self._parse_name(children[2])

        module_def.add_constant(ConstDefinition(py_name=py_name, cpp_type=cpp_type))

        return True

    def _handle_module_add_integer_constants(self, node: cindex.Cursor) -> bool:
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

    def _handle_class_add_constructor(self, node: cindex.Cursor) -> bool:
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
        call_constructor_ptr = ensure_first_child(call_expr, CursorKind.UNEXPOSED_EXPR)
        call_constructor_ref = ensure_only_child(
            call_constructor_ptr, CursorKind.DECL_REF_EXPR
        )
        assert call_constructor_ref.spelling == "callConstructor"

        t_params_ref = ensure_last_child(
            call_constructor_ref, [CursorKind.TYPE_REF, CursorKind.TEMPLATE_REF]
        )
        t_params_type = type_info(t_params_ref)

        cpp_params = [("", type_) for type_ in (t_params_type.args or [])]
        cpp_signature = f"void ({', '.join(str(type_) for _, type_ in cpp_params)})"

        class_def.add_constructor(
            ConstructorDefinition(
                cpp_params=cpp_params,
                cpp_signature=cpp_signature,
            )
        )

        return True

    def _handle_class_add_method(self, node: cindex.Cursor) -> bool:
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
        cpp_signature, cpp_return_type, cpp_params = self._handle_dispatcher_ref(
            dispatcher_ref
        )

        class_def.add_method(
            MethodDefinition(
                py_name=py_name,
                doc=doc,
                cpp_return_type=cpp_return_type,
                cpp_params=cpp_params,
                cpp_signature=cpp_signature,
            )
        )

        return True

    def _handle_class_add_static_method(self, node: cindex.Cursor) -> bool:
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
        cpp_signature, cpp_return_type, cpp_params = self._handle_dispatcher_ref(
            dispatcher_ref
        )

        class_def.add_method(
            MethodDefinition(
                py_name=py_name,
                doc=doc,
                cpp_return_type=cpp_return_type,
                cpp_params=cpp_params,
                cpp_signature=cpp_signature,
                is_static=True,
            )
        )

        return True

    def _handle_class_add_get_setter(self, node: cindex.Cursor) -> bool:
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
                set_value_name = "value"
            else:
                call_expr = ensure_only_child(return_stmt, CursorKind.CALL_EXPR)
                args = list(call_expr.get_children())
                setter = ensure_only_child(args[3], CursorKind.DECL_REF_EXPR).referenced
                params = list(iter_children(setter, CursorKind.PARM_DECL))
                if call_expr.spelling == "set":
                    # only one parameter, the value
                    assert len(params) == 1, params
                    value = params[0]
                elif call_expr.spelling == "freeSet":
                    # first parameter is self, second is value
                    assert len(params) == 2, params
                    value = params[1]
                else:
                    self._debug(dispatch)
                    assert False, call_expr.spelling
                set_type = type_info(value)
                set_value_name = value.spelling or "value"
        else:
            set_type = None
            set_value_name = "value"

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

    def _handle_class_add_static_const(self, node: cindex.Cursor) -> bool:
        assert node.kind == CursorKind.CALL_EXPR
        children = list(node.get_children())
        if not is_member_ref_expr(
            children[0], "lass::python::impl::ClassDefinition::addStaticConst"
        ):
            return False
        class_def = self._get_class_def(children[0])

        py_name = self._parse_name(children[1])

        value_arg = children[2]
        while value_arg.kind.is_unexposed():
            value_arg = ensure_only_child(value_arg)
        cpp_type = type_info(value_arg)

        class_def.add_const(ConstDefinition(py_name=py_name, cpp_type=cpp_type))

        return True

    def _handle_class_add_inner_class(self, node: cindex.Cursor) -> bool:
        assert node.kind == CursorKind.CALL_EXPR
        children = list(node.get_children())
        if not is_member_ref_expr(
            children[0], "lass::python::impl::ClassDefinition::addInnerClass"
        ):
            return False
        class_def = self._get_class_def(children[0])

        # first argument is the class definition of the inner class
        type_ref = ensure_only_child(children[1], CursorKind.TYPE_REF)
        inner_class = canonical_type(type_ref).spelling

        class_def.add_inner_class(inner_class)

        return True

    def _handle_class_add_inner_enum(self, node: cindex.Cursor) -> bool:
        assert node.kind == CursorKind.CALL_EXPR
        children = list(node.get_children())
        if not is_member_ref_expr(
            children[0], "lass::python::impl::ClassDefinition::addInnerEnum"
        ):
            return False
        class_def = self._get_class_def(children[0])

        unary_op = ensure_only_child(children[1], CursorKind.UNARY_OPERATOR)
        decl_ref = ensure_only_child(unary_op, CursorKind.DECL_REF_EXPR)
        enum_type = type_info(decl_ref)
        assert enum_type.args, f"{enum_type=} must have template args"
        cpp_name = enum_type.args[0].name
        assert cpp_name in self.stubdata.enums
        enum_def = self.stubdata.enums[cpp_name]

        assert "." not in enum_def.py_name
        class_def.add_enum(cpp_name)

        return True

    def _handle_export_traits(self, node: cindex.Cursor) -> bool:
        if node.kind in (CursorKind.TEMPLATE_REF, CursorKind.CLASS_TEMPLATE):
            return False
        assert fully_qualified(node) == "lass::python::PyExportTraits"
        type_ = type_info(node)
        if type_.name != "lass::python::PyExportTraits":
            return False
        assert type_.args and len(type_.args) == 1
        cpp_type = type_.args[0]

        def find_py_typing(n: cindex.Cursor) -> str | None:
            for child in iter_children(n, CursorKind.VAR_DECL):
                if child.spelling == "py_typing":
                    typing = _ensure_only_child_recursive(
                        child, CursorKind.STRING_LITERAL
                    )
                    return string_literal(typing)
            return None

        py_type = find_py_typing(node)
        if py_type is None:
            # look in base class
            if cxx_base_specifier := _get_first_child(
                node, CursorKind.CXX_BASE_SPECIFIER
            ):
                template_ref = ensure_first_child(
                    cxx_base_specifier, CursorKind.TEMPLATE_REF
                )
                base = template_ref.referenced
                py_type = find_py_typing(base)
        if not py_type:
            return False

        if node.kind == CursorKind.STRUCT_DECL:
            # A full specialization
            self.stubdata.add_export_traits(
                ExportTraits(cpp_type=cpp_type, py_type=py_type)
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
                    template_params=list(intermediate_params.values()),
                )
            )

            return True

        else:
            self._debug(node)
            assert False, f"Unhandled node kind {node.kind}"

        return True

    @classmethod
    def _debug(cls, node: cindex.Cursor, indent: str = "") -> None:
        if not indent:
            print(f"---- {node.location.file}:{node.location.line}")
        kind = node.kind.name
        type_ = node.type.get_canonical().spelling
        print(f"{indent}{kind} {node.spelling} type={type_}")
        for child in node.get_children():
            cls._debug(child, indent + "  ")

    @classmethod
    def _error(cls, node: cindex.Cursor, message: str) -> None:
        cls._debug(node)
        raise AssertionError(message)

    def _parse_module_ref(self, node: cindex.Cursor) -> ModuleDefinition:
        if node.kind == CursorKind.MEMBER_REF_EXPR:
            module_ref = ensure_only_child(node, CursorKind.DECL_REF_EXPR)
        else:
            module_ref = ensure_kind(node, CursorKind.DECL_REF_EXPR)
        assert canonical_type(module_ref).spelling == "lass::python::ModuleDefinition"
        module_name = fully_qualified(module_ref.referenced)
        return self.stubdata.modules[module_name]

    def _get_class_def(self, node: cindex.Cursor) -> ClassDefinition:
        """
        Get the class definition from a _lassPyClassDef method call.
        """
        _lassPyClassDef = ensure_only_child(node, CursorKind.DECL_REF_EXPR)
        shadow_class = self._parse_class_ref(_lassPyClassDef)
        return self.stubdata.shadow_classes[shadow_class]

    def _parse_class_ref(self, node: cindex.Cursor) -> str:
        """
        Get the shadow class name from a _lassPyClassDef reference.
        """
        ensure_kind(node, CursorKind.DECL_REF_EXPR)
        assert node.spelling == "_lassPyClassDef"
        assert canonical_type(node).spelling == "lass::python::impl::ClassDefinition"
        shadow_node = node.referenced.semantic_parent
        shadow_class = fully_qualified(shadow_node)
        return shadow_class

    def _parse_name(self, node: cindex.Cursor) -> str:
        if node.kind == CursorKind.UNEXPOSED_EXPR:
            assert node.type.spelling in (
                "const char *",
                "std::string",
                "const std::string",
            )
            children = list(node.get_children())
            assert len(children) == 1
            return self._parse_name(children[0])
        if node.kind == CursorKind.CALL_EXPR:
            assert node.spelling == "basic_string"
            children = list(node.get_children())
            assert len(children) == 1
            return self._parse_name(children[0])
        return string_literal(node)

    def _parse_doc(self, node: cindex.Cursor) -> str | None:
        if node.kind == CursorKind.UNEXPOSED_EXPR:
            assert node.type.spelling == "const char *"
            children = list(node.get_children())
            assert len(children) == 1
            return self._parse_doc(children[0])
        if node.kind == CursorKind.INTEGER_LITERAL:
            assert node.type.spelling == "int"
            assert node.spelling == ""
            return None
        return string_literal(node)

    @classmethod
    def _handle_dispatcher_ref(
        cls,
        dispatcher_ref: cindex.Cursor,
    ) -> tuple[str, TypeInfo, list[tuple[str, TypeInfo]]]:
        assert (
            canonical_type(dispatcher_ref).spelling == "_object *(_object *, _object *)"
        )
        dispatcher = dispatcher_ref.referenced
        # cls.debug(dispatcher)

        compound = ensure_last_child(dispatcher, CursorKind.COMPOUND_STMT)
        return_stmt = ensure_last_child(compound, CursorKind.RETURN_STMT)
        call_expr = ensure_only_child(return_stmt, CursorKind.CALL_EXPR)
        assert call_expr.spelling in (
            "call",
            "callFunction",
            "callMethod",
            "callFree",
            "callFreeMethod",
        ), call_expr.spelling

        func_arg = ensure_last_child(
            call_expr, [CursorKind.UNEXPOSED_EXPR, CursorKind.UNARY_OPERATOR]
        )
        func_ref = ensure_only_child(func_arg, CursorKind.DECL_REF_EXPR)
        func = func_ref.referenced

        cpp_signature = canonical_type(func).spelling
        cpp_return_type = type_info(func.type.get_result())

        params = list(iter_children(func, CursorKind.PARM_DECL))
        if call_expr.spelling in ("callFree", "callFreeMethod"):
            # first parameter is self
            params = params[1:]
        cpp_params = [(p.spelling, type_info(p)) for p in params]

        return cpp_signature, cpp_return_type, cpp_params


class ParseError(Exception):
    def __init__(self, errors: list[str]):
        super().__init__("\n".join(errors))
        self.errors = errors


def is_member_ref_expr(node: cindex.Cursor, member: str) -> bool:
    return (
        node.kind == CursorKind.MEMBER_REF_EXPR
        and fully_qualified(node.referenced) == member
    )


def ensure_kind(node: cindex.Cursor, kind: CursorKind) -> cindex.Cursor:
    """
    Ensure node is of kind; or unexposed in which case you recurse.
    """
    if node.kind == kind:
        return node
    assert node.kind == CursorKind.UNEXPOSED_EXPR, (
        f"expected {kind.name} or UNEXPOSED_EXPR, got {node.kind.name}"
    )
    children = list(node.get_children())
    assert len(children) == 1, f"expected 1 child, got {len(children)}"
    return ensure_kind(children[0], kind)


def _get_first_child(node: cindex.Cursor, kind: CursorKind) -> cindex.Cursor | None:
    children = list(node.get_children())
    if children and children[0].kind == kind:
        return children[0]
    return None


def ensure_first_child(node: cindex.Cursor, kind: CursorKind) -> cindex.Cursor:
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


def ensure_only_child(
    node: cindex.Cursor, kind: cindex.Cursor | None = None
) -> cindex.Cursor:
    children = list(node.get_children())
    assert len(children) == 1, f"expected 1 child, got {len(children)}"
    if kind is not None:
        assert children[0].kind == kind, (
            f"expected {kind.name}, got {children[0].kind.name}"
        )
    return children[0]


def _ensure_only_child_recursive(
    node: cindex.Cursor, kind: CursorKind
) -> cindex.Cursor:
    children = list(node.get_children())
    assert len(children) == 1, f"expected 1 child, got {len(children)}"
    child = children[0]
    if child.kind == CursorKind.UNEXPOSED_EXPR:
        return _ensure_only_child_recursive(child, kind)
    assert child.kind == kind, (
        f"expected {kind.name} or UNEXPOSED_EXPR, got {child.kind.name}"
    )
    return child


def canonical_type(node: cindex.Cursor) -> cindex.Type:
    return node.type.get_canonical()


def ensure_type(node: cindex.Cursor, type_: str) -> cindex.Cursor:
    t = canonical_type(node).spelling
    assert t == type_, f"expected {type_}, got {t}"
    return node


def string_literal(node: cindex.Cursor) -> str:
    assert node.kind == CursorKind.STRING_LITERAL
    s = node.spelling
    assert s.startswith('"') and s.endswith('"')
    s = s[1:-1]
    s = s.replace(r"\n", "\n")
    s = s.replace(r"\t", "\t")
    s = s.replace(r"\r", "\r")
    s = s.replace(r"\"", '"')
    return s


def ensure_last_child(
    node: cindex.Cursor, kind: CursorKind | list[CursorKind]
) -> cindex.Cursor:
    if isinstance(kind, CursorKind):
        kind = [kind]
    children = list(node.get_children())
    assert children[-1].kind in kind, (
        f"expected {'|'.join(k.name for k in kind)}, got {children[-1].kind.name}"
    )
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
    assert last_child.kind in kind, (
        f"expected {'|'.join(k.name for k in kind)}, got {last_child.kind.name}"
    )
    return last_child


def _find_first_child(node: cindex.Cursor, kind: CursorKind) -> cindex.Cursor | None:
    for child in node.get_children():
        if child.kind == kind:
            return child
    return None


def iter_children(
    node: cindex.Cursor, kind: CursorKind | list[CursorKind]
) -> Iterator[cindex.Cursor]:
    if isinstance(kind, CursorKind):
        kind = [kind]
    for child in node.get_children():
        if child.kind in kind:
            yield child


def fully_qualified(node: cindex.Cursor) -> str:
    if node is None:
        return ""
    elif node.kind == CursorKind.TRANSLATION_UNIT:
        return ""
    elif parent := fully_qualified(node.semantic_parent):
        return f"{parent}::{node.spelling}"
    return node.spelling


def type_info(node_or_type: cindex.Cursor | cindex.Type) -> TypeInfo:
    type_ = node_or_type if isinstance(node_or_type, cindex.Type) else node_or_type.type
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
