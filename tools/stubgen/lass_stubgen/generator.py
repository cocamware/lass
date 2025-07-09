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

import functools
import re
import sys
import textwrap
from typing import Callable, NamedTuple, ParamSpec, TextIO, TypeAlias, TypeVar

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
    TypeArgs,
    TypeInfo,
)

__all__ = [
    "StubGenerator",
    "StubGeneratorError",
]


T = TypeVar("T")
P = ParamSpec("P")


def _add_note(func: Callable[P, T]) -> Callable[P, T]:
    @functools.wraps(func)
    def wrapper(*args: P.args, **kwargs: P.kwargs) -> T:
        _self, subject = args[:2]
        try:
            return func(*args, **kwargs)
        except Exception as err:
            err.add_note(f"While generating stubs for {subject}")
            raise

    return wrapper


class StubGeneratorError(Exception):
    pass


class StubGenerator:
    def __init__(self, stubdata: StubData, *, verbose: bool = False) -> None:
        self.stubdata = stubdata
        self.verbose = verbose

    @_add_note
    def write_module(
        self,
        module_def: ModuleDefinition,
        *,
        file: TextIO = sys.stdout,
        with_signature: bool = False,
    ) -> None:
        print("import datetime", file=file)
        print("import enum", file=file)
        print("import os", file=file)
        print(
            "from typing import Any, Callable, Final, Iterator, Literal, Mapping, Sequence, Union, overload",
            file=file,
        )
        for other_module in self.stubdata.modules.values():
            if other_module.fully_qualified_name != module_def.fully_qualified_name:
                print(f"import {other_module.fully_qualified_name}", file=file)
        print("", file=file)

        scope = module_def.fully_qualified_name
        for enum_name in module_def.enums:
            print("", file=file)
            enum_def = self.stubdata.enums[enum_name]
            self.write_enum(enum_def, file=file, with_signature=with_signature)

        for class_name in module_def.classes:
            print("", file=file)
            class_def = self.stubdata.shadow_classes[class_name]
            self.write_class(
                class_def,
                scope=scope,
                file=file,
                with_signature=with_signature,
            )

        if len(module_def.constants) > 0:
            print("", file=file)
        for const in module_def.constants.values():
            self.write_const(const, scope=scope, file=file)

        for funcs in module_def.functions.values():
            print("", file=file)
            is_overload = len(funcs) > 1
            for func in funcs:
                if is_overload:
                    print("@overload", file=file)
                self.write_module_function(
                    func,
                    scope=scope,
                    file=file,
                    with_signature=with_signature,
                )

    @_add_note
    def write_module_function(
        self,
        func: FunctionDefinition,
        *,
        file: TextIO,
        with_signature: bool,
        scope: str | None = None,
    ) -> None:
        name = func.py_name
        params = self.python_params(None, func.cpp_params, scope=scope)
        ret_type = self.python_type(func.cpp_return_type, scope=scope)
        signature = f"  # {func.cpp_signature}" if with_signature else ""
        print(
            f"def {name}({', '.join(params)}) -> {ret_type}:{signature}",
            file=file,
        )
        self.write_doc(func.doc, indent=4, file=file)
        print("    ...", file=file)

    @_add_note
    def write_const(
        self,
        const: ConstDefinition,
        *,
        file: TextIO,
        scope: str | None = None,
        indent: int = 0,
    ) -> None:
        py_name = const.py_name
        py_type = self.python_type(const.cpp_type, scope=scope)
        if const.value is None:
            py_value = "..."
        else:
            py_value = repr(const.value)
        print(f"{' ' * indent}{py_name}: Final[{py_type}] = {py_value}", file=file)

    @_add_note
    def write_class(
        self,
        class_def: ClassDefinition,
        *,
        file: TextIO,
        with_signature: bool,
        scope: str | None = None,
        indent: int = 0,
    ) -> None:
        parent = self.stubdata.shadow_classes.get(class_def.parent_type)
        if parent:
            assert parent.fully_qualified_name
            bases = f"({_strip_scope(parent.fully_qualified_name, scope)})"
        else:
            bases = ""

        if with_signature:
            cpp_class = str(class_def.cpp_type)
            if cpp_class != class_def.shadow_name:
                cpp_class += f" (via {class_def.shadow_name})"
            signature = f"  # {cpp_class}"
        else:
            signature = ""

        print(f"{' ' * indent}class {class_def.py_name}{bases}:{signature}", file=file)

        self.write_doc(class_def.doc, indent=indent + 4, file=file)
        is_empty = not class_def.doc

        for inner_enum in class_def.inner_enums:
            print("", file=file)
            self.write_enum(
                self.stubdata.enums[inner_enum],
                indent=indent + 4,
                file=file,
                with_signature=with_signature,
            )
            is_empty = False

        for inner_class in class_def.inner_classes:
            print("", file=file)
            self.write_class(
                self.stubdata.shadow_classes[inner_class],
                indent=indent + 4,
                scope=scope,
                file=file,
                with_signature=with_signature,
            )
            is_empty = False

        for const in class_def.consts.values():
            print("", file=file)
            self.write_const(const, indent=indent + 4, scope=scope, file=file)
            is_empty = False

        print("", file=file)
        is_overload = len(class_def.constructors) > 1
        for constructor in class_def.constructors:
            if is_overload:
                print(f"{' ' * indent}    @overload", file=file)
            self.write_constructor(
                constructor,
                indent=indent + 4,
                scope=scope,
                file=file,
                with_signature=with_signature,
            )
            is_empty = False

        for getsetter in class_def.getsetters.values():
            print("", file=file)
            self.write_getsetter(
                getsetter,
                indent=indent + 4,
                scope=scope,
                file=file,
                with_signature=with_signature,
            )
            is_empty = False

        for methods in class_def.methods.values():
            print("", file=file)
            is_overload = len(methods) > 1
            for method in methods:
                if is_overload:
                    print(f"{' ' * indent}    @overload", file=file)
                self.write_method(
                    method,
                    indent=indent + 4,
                    scope=scope,
                    file=file,
                    with_signature=with_signature,
                )
            is_empty = False

        if is_empty:
            print(f"{' ' * indent}    ...", file=file)

    @_add_note
    def write_constructor(
        self,
        constructor: ConstructorDefinition,
        *,
        file: TextIO,
        with_signature: bool,
        scope: str | None = None,
        indent: int = 0,
    ) -> None:
        params = self.python_params("self", constructor.cpp_params, scope=scope)
        signature = f"  # {constructor.cpp_signature}" if with_signature else ""
        print(
            f"{' ' * indent}def __init__({', '.join(params)}) -> None:{signature}",
            file=file,
        )
        print(f"{' ' * indent}    ...", file=file)

    @_add_note
    def write_getsetter(
        self,
        getsetter: GetSetterDefinition,
        *,
        file: TextIO,
        with_signature: bool,
        scope: str | None = None,
        indent: int = 0,
    ) -> None:
        name = getsetter.py_name
        get_type = getsetter.get_type
        py_get_type = self.python_type(get_type, scope=scope)
        get_signature = f"  # {get_type}" if with_signature else ""
        print(f"{' ' * indent}@property", file=file)
        print(
            f"{' ' * indent}def {name}(self, /) -> {py_get_type}:{get_signature}",
            file=file,
        )
        self.write_doc(getsetter.doc, indent=indent + 4, file=file)
        print(f"{' ' * indent}    ...", file=file)

        if set_type := getsetter.set_type:
            value_name = getsetter.set_value_name
            assert value_name, "set_value_name must be set when set_type is provided"
            py_set_type = self.python_type(set_type, scope=scope)
            set_signature = f"  # {set_type}" if with_signature else ""
            print(f"{' ' * indent}@{name}.setter", file=file)
            print(
                f"{' ' * indent}def {name}(self, {value_name}: {py_set_type}, /) -> None:{set_signature}",
                file=file,
            )
            print(f"{' ' * indent}    ...", file=file)

    @_add_note
    def write_method(
        self,
        method: MethodDefinition,
        *,
        file: TextIO,
        with_signature: bool,
        scope: str | None = None,
        indent: int = 0,
    ) -> None:
        name = method.py_name

        if name in {"__lt__", "__le__", "__eq__", "__ne__", "__gt__", "__ge__"}:
            # these are the rich comparison methods. Regardless of the C++ method,
            # we always use the same Python signature
            params = ["self", "other: object", "/"]
            ret_type = "bool"
        else:
            params = self.python_params(
                "cls" if method.is_static else "self",
                method.cpp_params,
                scope=scope,
            )
            ret_type = self.python_type(method.cpp_return_type, scope=scope)
        signature = f"  # {method.cpp_signature}" if with_signature else ""
        if method.is_static:
            print(f"{' ' * indent}@classmethod", file=file)
        print(
            f"{' ' * indent}def {name}({', '.join(params)}) -> {ret_type}:{signature}",
            file=file,
        )
        self.write_doc(method.doc, indent=indent + 4, file=file)
        print(f"{' ' * (indent)}    ...", file=file)

    @_add_note
    def write_enum(
        self,
        enum_def: EnumDefinition,
        *,
        file: TextIO,
        with_signature: bool,
        indent: int = 0,
    ) -> None:
        py_name = enum_def.py_name
        value_py_type = enum_def.value_py_type
        if value_py_type == "int":
            base = "enum.IntEnum"
        elif value_py_type == "str":
            if sys.version_info < (3, 11):
                base = "str, enum.Enum"  # Python 3.10 and earlier do not have StrEnum
            else:
                base = "enum.StrEnum"
        else:
            base = "enum.Enum"
        signature = f"  # {enum_def.cpp_name}" if with_signature else ""
        print(f"{' ' * indent}class {py_name}({base}):{signature}", file=file)
        if enum_def.doc:
            self.write_doc(enum_def.doc, indent=indent + 4, file=file)
        print(f"{' ' * indent}    _value_: {value_py_type}", file=file)
        for name, value in enum_def.values.items():
            if value is Ellipsis:
                py_value = "..."
            else:
                py_value = repr(value)
            print(f"{' ' * indent}    {name} = {py_value}", file=file)

    def write_doc(self, doc: str | None, *, file: TextIO, indent: int = 0) -> None:
        if not doc:
            return
        # doc = textwrap.indent(
        #    doc, prefix=" " * indent, predicate=lambda _: True
        # ).strip()
        doc = textwrap.fill(
            doc, width=72, initial_indent="", subsequent_indent=" " * indent
        )
        print(f'{" " * indent}"""{doc}"""', file=file)

    def python_params(
        self,
        self_: str | None,
        cpp_params: list[ParamInfo],
        scope: str | None,
    ) -> list[str]:
        params = [
            f"{p.name or f'_{i}'}: {self.python_type(p.type_, scope=scope)}"
            for i, p in enumerate(cpp_params)
        ]
        if self_:
            params.insert(0, self_)
        if params:
            params.append("/")
        return params

    @functools.cache
    def python_type(self, cpp_type: TypeInfo, *, scope: str | None) -> str:
        """Return the Python type for a given C++ type.

        If the C++ type is not recognized, return the C++ type as a string.
        If scope is specified, strip the scope from the fully qualified name.
        """
        cpp_name = cpp_type.name
        if class_def := self.stubdata.cpp_classes.get(str(cpp_type)):
            if not class_def.fully_qualified_name:
                raise StubGeneratorError(
                    f"Class {class_def.py_name} ({cpp_name}) "
                    + "is not part of a module or class"
                )
            return _strip_scope(class_def.fully_qualified_name, scope)

        if enum_def := self.stubdata.enums.get(cpp_name):
            if not enum_def.fully_qualified_name:
                raise StubGeneratorError(
                    f"Class {enum_def.py_name} ({cpp_name}) "
                    + "is not part of a module or class"
                )
            return _strip_scope(enum_def.fully_qualified_name, scope)

        if specializations := self.stubdata.export_traits.get(cpp_name):
            if py_type := self._match_export_traits(
                cpp_type=cpp_type, specializations=specializations, scope=scope
            ):
                return py_type

        if py_typer := BUILTIN_TYPES.get(cpp_name):
            if callable(py_typer):
                return py_typer(self, cpp_type.args, scope)
            return py_typer

        for regex, py_type_match in BUILTIN_TYPES_REGEX:
            if match := re.match(regex, cpp_name):
                if callable(py_type_match):
                    return py_type_match(self, cpp_type.args, scope, match)
                return py_type_match

        if cpp_name.endswith("*"):
            # pointer to a type, strip the pointer
            cpp_type_ = TypeInfo(cpp_name[:-1].rstrip(), cpp_type.args)
            return f"{self.python_type(cpp_type_, scope=scope)} | None"

        return cpp_name

    def _match_export_traits(
        self,
        *,
        cpp_type: TypeInfo,
        specializations: dict[str, ExportTraits],
        scope: str | None,
    ) -> str | None:
        # do we have a full specialization?
        if full_specialization := specializations.get(str(cpp_type)):
            if not full_specialization.template_params:
                return full_specialization.py_type

        # do we have a partial specialization?
        matches = [
            MatchedExportTraits(export_traits, *match)
            for export_traits in specializations.values()
            if (
                match := self._python_type_export_traits(
                    cpp_type=cpp_type, export_traits=export_traits, scope=scope
                )
            )
        ]
        if not matches:
            return None

        if len(matches) == 1:
            return matches[0].py_type

        best = _find_best_matches(matches)
        if self.verbose:
            print(f"Multiple matches for {cpp_type}:", file=sys.stderr)
            for i, m in enumerate(matches):
                print(
                    f"{'*' if m in best else ' '}  {i + 1}: {m}",
                    file=sys.stderr,
                )
        if len(best) > 1:
            best_str = "\n".join(map(str, best))
            raise StubGeneratorError(f"Ambigous matches found:\n{best_str}\n")
        return best[0].py_type

    def _python_type_export_traits(
        self, *, cpp_type: TypeInfo, export_traits: ExportTraits, scope: str | None
    ) -> tuple[str, MatchedParams] | None:
        assert cpp_type.name == export_traits.cpp_type.name

        # match template parameters
        matched_params: MatchedParams = {
            param: None for param in export_traits.template_params
        }
        if not _match_template(
            type_=cpp_type, tmpl=export_traits.cpp_type, matched_params=matched_params
        ):
            return None

        # now substitute template parameters in py_type
        py_type = export_traits.py_type
        for param, arg in matched_params.items():
            assert arg is not None
            if isinstance(arg, tuple):
                py_arg = ", ".join(self.python_type(a, scope=scope) for a in arg)
                py_type = re.sub(rf"\b{re.escape(param)}\.\.\.", py_arg, py_type)
            else:
                py_arg = self.python_type(arg, scope=scope)
                py_type = re.sub(rf"\b{re.escape(param)}\b", py_arg, py_type)

        return py_type, matched_params


def _strip_scope(fqname: str, scope: str | None) -> str:
    """
    Strip the scope from a fully qualified name.
    """
    if scope and fqname.startswith(scope + "."):
        return fqname[len(scope) + 1 :]
    return fqname


MatchedParams: TypeAlias = dict[str, TypeInfo | tuple[TypeInfo, ...] | None]


def _match_template(
    type_: TypeInfo | None, tmpl: TypeInfo | None, matched_params: MatchedParams
) -> bool:
    """
    Match a template type with a concrete type.
    """
    if tmpl is None and type_ is None:
        return True  # both are None
    if tmpl is None or type_ is None:
        return False  # one is None, the other is not
    if tmpl.name in matched_params:
        # it's a template parameter
        assert tmpl.args is None
        if matched_params[tmpl.name] is None:
            matched_params[tmpl.name] = type_
        elif matched_params[tmpl.name] != type_:
            return False
        return True

    # it's not a template parameter, so it need to match exactly
    if type_.name != tmpl.name:
        return False
    return _match_template_args(type_.args, tmpl.args, matched_params)


def _match_template_args(
    type_args: TypeArgs, tmpl_args: TypeArgs, matched_params: MatchedParams
) -> bool:
    """
    Match arguments of contrete type with template arguments and store the
    matched parameters.
    """
    if type_args is None and tmpl_args is None:
        return True  # both are None
    if type_args is None or tmpl_args is None:
        return False  # one is None, the other is not

    if tmpl_args and tmpl_args[-1].name.endswith("..."):
        pack_name = tmpl_args[-1].name[:-3]
        assert pack_name in matched_params
        n = len(tmpl_args) - 1
        if len(type_args) < n:
            return False
        type_pack = type_args[n:]
        if matched_params[pack_name] is None:
            matched_params[pack_name] = type_pack
        elif matched_params[pack_name] != type_pack:
            return False
        type_args, tmpl_args = type_args[:n], tmpl_args[:n]

    if len(type_args) != len(tmpl_args):
        return False
    for type_arg, tmpl_arg in zip(type_args, tmpl_args):
        if not _match_template(type_arg, tmpl_arg, matched_params):
            return False
    return True


class MatchedExportTraits(NamedTuple):
    export_traits: ExportTraits
    py_type: str
    matched_params: MatchedParams

    def __str__(self) -> str:
        params = ", ".join(f"{p}={typ}" for p, typ in self.matched_params.items())
        return f"{self.py_type} <- {self.export_traits.cpp_type} with {params}"


def _find_best_matches(matches: list[MatchedExportTraits]) -> list[MatchedExportTraits]:
    """
    Find the best match for a given export traits.
    The best match is the one with the most specific template parameters.
    """
    assert matches, "No matches found"
    best = [matches[0]]
    for match in matches[1:]:
        new_best: list[MatchedExportTraits] = []
        better_than_best = True
        incomparable_with_all = True
        for b in best:
            if match.export_traits > b.export_traits:
                incomparable_with_all = False
            elif b.export_traits > match.export_traits:
                incomparable_with_all = False
                better_than_best = False
                new_best.append(b)
            else:
                better_than_best = False
                new_best.append(b)

        if better_than_best:
            best = [match]
        elif incomparable_with_all:
            best.append(match)
        else:
            best = new_best

    return best


def _pytype_sequence(stubgen: StubGenerator, args: TypeArgs, scope: str | None) -> str:
    assert args and len(args) >= 1, args
    element_type = stubgen.python_type(args[0], scope=scope)
    return f"Sequence[{element_type}]"


def _pytype_mapping(stubgen: StubGenerator, args: TypeArgs, scope: str | None) -> str:
    assert args and len(args) >= 2, args
    key_type = stubgen.python_type(args[0], scope=scope)
    value_type = stubgen.python_type(args[1], scope=scope)
    return f"Mapping[{key_type}, {value_type}]"


BuiltinTyper: TypeAlias = Callable[[StubGenerator, TypeArgs, str | None], str]

BUILTIN_TYPES: dict[str, str | BuiltinTyper] = {
    "void": "None",
    "std::vector": _pytype_sequence,
    "std::list": _pytype_sequence,
    "std::deque": _pytype_sequence,
    "std::map": _pytype_mapping,
    "std::__cxx11::vector": _pytype_sequence,
    "std::__cxx11::list": _pytype_sequence,
    "std::__cxx11::deque": _pytype_sequence,
    "std::__cxx11::map": _pytype_mapping,
    "std::__ndk1::vector": _pytype_sequence,
    "std::__ndk1::list": _pytype_sequence,
    "std::__ndk1::deque": _pytype_sequence,
    "std::__ndk1::map": _pytype_mapping,
    "lass::python::PyIteratorRange *": "Iterator[Any]",
    "lass::stde::static_vector": _pytype_sequence,
    "lass::stde::vector_map": _pytype_mapping,
}

BuiltinTyperRegex: TypeAlias = Callable[
    [StubGenerator, TypeArgs, str | None, re.Match[str]], str
]


BUILTIN_TYPES_REGEX: list[tuple[re.Pattern[str], str | BuiltinTyperRegex]] = [
    (re.compile(r"char\[\d+\]"), "str"),
    (re.compile(r"wchar_t\[\d+\]"), "str"),
]
