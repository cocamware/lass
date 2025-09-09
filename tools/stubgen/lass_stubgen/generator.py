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
import functools
import re
import sys
import textwrap
from collections.abc import Sequence
from io import StringIO
from typing import Callable, NamedTuple, ParamSpec, TextIO, TypeAlias, TypeVar, overload

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
        file.write("# mypy: disable-error-code = overload-overlap\n")
        file.write("from collections.abc import Callable, Mapping, Sequence\n")
        file.write("from typing import Any, Final, Literal, Union, overload\n")

        preamble: list[str] = []

        with StringIO() as buf:
            self._write_module_enums(
                module_def, file=buf, preamble=preamble, with_signature=with_signature
            )
            self._write_module_classes(
                module_def, file=buf, preamble=preamble, with_signature=with_signature
            )
            self._write_module_constants(module_def, file=buf, preamble=preamble)
            self._write_module_functions(
                module_def, file=buf, preamble=preamble, with_signature=with_signature
            )
            body = buf.getvalue()

        self._write_module_preamble(preamble, file=file)
        file.write(body)

    def _write_module_enums(
        self,
        module_def: ModuleDefinition,
        *,
        file: TextIO,
        preamble: list[str],
        with_signature: bool,
    ) -> None:
        for enum_type in module_def.enums:
            enum_def = self.stubdata.enums[enum_type]
            self.write_enum(
                enum_def,
                file=file,
                preamble=preamble,
                indent=0,
                with_signature=with_signature,
            )

    def _write_module_classes(
        self,
        module_def: ModuleDefinition,
        *,
        file: TextIO,
        preamble: list[str],
        with_signature: bool,
    ) -> None:
        for class_name in module_def.classes:
            class_def = self.stubdata.shadow_classes[class_name]
            self.write_class(
                class_def,
                file=file,
                scope=module_def.fully_qualified_name,
                preamble=preamble,
                indent=0,
                with_signature=with_signature,
            )

    def _write_module_constants(
        self,
        module_def: ModuleDefinition,
        *,
        file: TextIO,
        preamble: list[str],
    ) -> None:
        for const in module_def.constants.values():
            self.write_const(
                const,
                file=file,
                scope=module_def.fully_qualified_name,
                preamble=preamble,
                indent=0,
            )

    def _write_module_functions(
        self,
        module_def: ModuleDefinition,
        *,
        file: TextIO,
        preamble: list[str],
        with_signature: bool,
    ) -> None:
        for name, funcs in module_def.functions.items():
            funcs = self._remove_duplicates(funcs)
            is_overload = len(funcs) > 1
            common_doc, funcs = _extract_common_docstring(funcs)
            for func in funcs:
                if is_overload:
                    file.write("@overload\n")
                self.write_module_function(
                    func,
                    file=file,
                    scope=module_def.fully_qualified_name,
                    preamble=preamble,
                    with_signature=with_signature,
                )
            if common_doc:
                file.write(f"def {name}(*args, **kwargs): # type: ignore[misc]\n")
                self.write_doc(common_doc, indent=4, file=file)
                print("    ...", file=file)

    def _write_module_preamble(self, preamble: list[str], *, file: TextIO) -> None:
        imports: set[str] = set()
        type_vars: set[str] = set()
        type_aliases: dict[str, None] = {}  # preserve insertion order
        for line in preamble:
            if line.startswith("import "):
                imports.add(line)
            elif line.startswith("from "):
                imports.add(line)
            elif line.startswith("type "):
                if sys.version_info >= (3, 12):
                    # Python 3.12+ supports type directly
                    type_aliases[line] = None
                else:
                    imports.add("from typing import TypeAlias, TypeVar")
                    name, alias_def = line[5:].split("=")
                    name = name.strip()
                    alias_def = alias_def.strip()
                    if match := re.match(r"^(\w+)\[(.+)\]$", name):
                        name = match.group(1)
                        for type_var in match.group(2).split(","):
                            type_var = type_var.strip()
                            if not type_var.startswith("_"):
                                alias_def = re.sub(
                                    rf"\b{re.escape(type_var)}\b",
                                    f"_{type_var}",
                                    alias_def,
                                )
                                type_var = f"_{type_var}"
                            type_vars.add(f"{type_var} = TypeVar('{type_var}')")
                    type_aliases[f"{name}: TypeAlias = {alias_def}"] = None
            else:
                raise StubGeneratorError(f"Unexpected line in preamble: {line}")

        for line in sorted(imports):
            file.write(f"{line}\n")
        for line in sorted(type_vars):
            file.write(f"{line}\n")
        for line in type_aliases:  # in order of definition
            file.write(f"{line}\n")

    @_add_note
    def write_module_function(
        self,
        func: FunctionDefinition,
        *,
        file: TextIO,
        scope: str | None,
        preamble: list[str],
        with_signature: bool,
    ) -> None:
        name = func.py_name
        params = self.python_params(
            None, func.cpp_params, scope=scope, preamble=preamble
        )
        ret_type = self.python_type(
            func.cpp_return_type, scope=scope, preamble=preamble, as_param=False
        )
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
        scope: str | None,
        preamble: list[str],
        indent: int,
    ) -> None:
        py_name = const.py_name
        py_type = self.python_type(
            const.cpp_type, scope=scope, preamble=preamble, as_param=False
        )
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
        scope: str | None,
        preamble: list[str],
        indent: int,
        with_signature: bool,
    ) -> None:
        parent = self.stubdata.shadow_classes.get(class_def.parent_type)
        if parent:
            assert parent.fully_qualified_name
            base = self._strip_scope(
                parent.fully_qualified_name, scope=scope, preamble=preamble
            )
            bases = f"({base})"
        else:
            bases = ""

        if with_signature:
            cpp_class = str(class_def.cpp_type)
            if class_def.cpp_type != class_def.shadow_type:
                cpp_class += f" (via {class_def.shadow_type})"
            signature = f"  # {cpp_class}"
        else:
            signature = ""

        file.write(f"{' ' * indent}class {class_def.py_name}{bases}:{signature}\n")

        with StringIO() as buf:
            self._write_class_body(
                class_def,
                file=buf,
                scope=scope,
                preamble=preamble,
                indent=indent + 4,
                with_signature=with_signature,
            )
            if body := buf.getvalue():
                file.write(body)
            else:
                file.write(f"{' ' * indent}    ...\n")

    def _write_class_body(
        self,
        class_def: ClassDefinition,
        *,
        file: TextIO,
        scope: str | None,
        preamble: list[str],
        indent: int,
        with_signature: bool,
    ) -> None:
        self.write_doc(class_def.doc, indent=indent, file=file)

        funcs = [
            self._write_class_enums,
            self._write_class_classes,
            self._write_class_constants,
            self._write_class_constructors,
            self._write_class_getsetters,
            self._write_class_methods,
        ]
        for func in funcs:
            func(
                class_def,
                scope=scope,
                preamble=preamble,
                indent=indent,
                with_signature=with_signature,
                file=file,
            )

    def _write_class_enums(
        self,
        class_def: ClassDefinition,
        *,
        file: TextIO,
        scope: str | None,
        preamble: list[str],
        indent: int,
        with_signature: bool,
    ) -> None:
        for inner_enum in class_def.inner_enums:
            self.write_enum(
                self.stubdata.enums[inner_enum],
                file=file,
                preamble=preamble,
                indent=indent,
                with_signature=with_signature,
            )

    def _write_class_classes(
        self,
        class_def: ClassDefinition,
        *,
        file: TextIO,
        scope: str | None,
        preamble: list[str],
        indent: int,
        with_signature: bool,
    ) -> None:
        for inner_class in class_def.inner_classes:
            self.write_class(
                self.stubdata.shadow_classes[inner_class],
                file=file,
                scope=scope,
                preamble=preamble,
                indent=indent,
                with_signature=with_signature,
            )

    def _write_class_constants(
        self,
        class_def: ClassDefinition,
        *,
        file: TextIO,
        scope: str | None,
        preamble: list[str],
        indent: int,
        with_signature: bool,
    ) -> None:
        for const in class_def.consts.values():
            self.write_const(
                const, file=file, scope=scope, preamble=preamble, indent=indent
            )

    def _write_class_constructors(
        self,
        class_def: ClassDefinition,
        *,
        file: TextIO,
        scope: str | None,
        preamble: list[str],
        indent: int,
        with_signature: bool,
    ) -> None:
        is_overload = len(class_def.constructors) > 1
        for constructor in class_def.constructors:
            if is_overload:
                file.write(f"{' ' * indent}@overload\n")
            self.write_constructor(
                constructor,
                file=file,
                scope=scope,
                preamble=preamble,
                indent=indent,
                with_signature=with_signature,
            )

    def _write_class_getsetters(
        self,
        class_def: ClassDefinition,
        *,
        file: TextIO,
        scope: str | None,
        preamble: list[str],
        indent: int,
        with_signature: bool,
    ) -> None:
        for getsetter in class_def.getsetters.values():
            self.write_getsetter(
                getsetter,
                file=file,
                scope=scope,
                preamble=preamble,
                indent=indent,
                with_signature=with_signature,
            )

    def _write_class_methods(
        self,
        class_def: ClassDefinition,
        *,
        file: TextIO,
        scope: str | None,
        preamble: list[str],
        indent: int,
        with_signature: bool,
    ) -> None:
        for name, methods in class_def.methods.items():
            methods = self._remove_duplicates(methods)
            is_overload = len(methods) > 1
            common_doc, methods = _extract_common_docstring(methods)
            for method in methods:
                if is_overload:
                    file.write(f"{' ' * indent}@overload\n")
                self.write_method(
                    method,
                    indent=indent,
                    scope=scope,
                    preamble=preamble,
                    file=file,
                    with_signature=with_signature,
                )
            if common_doc:
                file.write(
                    f"{' ' * indent}def {name}(self, *args, **kwargs): # type: ignore[misc]\n"
                )
                self.write_doc(common_doc, indent=indent + 4, file=file)

    @_add_note
    def write_constructor(
        self,
        constructor: ConstructorDefinition,
        *,
        file: TextIO,
        scope: str | None,
        preamble: list[str],
        indent: int,
        with_signature: bool,
    ) -> None:
        params = self.python_params(
            "self", constructor.cpp_params, scope=scope, preamble=preamble
        )
        signature = f"  # {constructor.cpp_signature}" if with_signature else ""
        file.write(
            f"{' ' * indent}def __init__({', '.join(params)}) -> None:{signature}\n"
        )
        file.write(f"{' ' * indent}    ...\n")

    @_add_note
    def write_getsetter(
        self,
        getsetter: GetSetterDefinition,
        *,
        file: TextIO,
        scope: str | None,
        preamble: list[str],
        indent: int,
        with_signature: bool,
    ) -> None:
        name = getsetter.py_name
        get_type = getsetter.get_type
        py_get_type = self.python_type(
            get_type, scope=scope, preamble=preamble, as_param=False
        )
        get_signature = f"  # {get_type}" if with_signature else ""
        file.write(f"{' ' * indent}@property\n")
        file.write(
            f"{' ' * indent}def {name}(self, /) -> {py_get_type}:{get_signature}\n"
        )
        self.write_doc(getsetter.doc, indent=indent + 4, file=file)
        file.write(f"{' ' * indent}    ...\n")

        if set_type := getsetter.set_type:
            value_name = getsetter.set_value_name
            assert value_name, "set_value_name must be set when set_type is provided"
            py_set_type = self.python_type(
                set_type, scope=scope, preamble=preamble, as_param=True
            )
            set_signature = f"  # {set_type}" if with_signature else ""
            file.write(f"{' ' * indent}@{name}.setter\n")
            file.write(
                f"{' ' * indent}def {name}(self, {value_name}: {py_set_type}, /) -> None:{set_signature}\n"
            )
            file.write(f"{' ' * indent}    ...\n")

    @_add_note
    def write_method(
        self,
        method: MethodDefinition,
        *,
        file: TextIO,
        scope: str | None,
        preamble: list[str],
        indent: int,
        with_signature: bool,
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
                preamble=preamble,
            )
            ret_type = self.python_type(
                method.cpp_return_type, scope=scope, preamble=preamble, as_param=False
            )
        signature = f"  # {method.cpp_signature}" if with_signature else ""
        if method.is_static:
            file.write(f"{' ' * indent}@classmethod\n")
        file.write(
            f"{' ' * indent}def {name}({', '.join(params)}) -> {ret_type}:{signature}\n"
        )
        self.write_doc(method.doc, indent=indent + 4, file=file)
        file.write(f"{' ' * (indent)}    ...\n")

    @_add_note
    def write_enum(
        self,
        enum_def: EnumDefinition,
        *,
        file: TextIO,
        preamble: list[str],
        indent: int,
        with_signature: bool,
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
        signature = f"  # {enum_def.cpp_type}" if with_signature else ""
        preamble.append("import enum")

        file.write(f"{' ' * indent}class {py_name}({base}):{signature}\n")
        if enum_def.doc:
            self.write_doc(enum_def.doc, indent=indent + 4, file=file)
        file.write(f"{' ' * indent}    _value_: {value_py_type}\n")

        for name, value in enum_def.values.items():
            if value is Ellipsis:
                py_value = "..."
            else:
                py_value = repr(value)
            file.write(f"{' ' * indent}    {name} = {py_value}\n")

    def write_doc(self, doc: str | None, *, file: TextIO, indent: int = 0) -> None:
        if not doc:
            return
        # doc = textwrap.indent(
        #    doc, prefix=" " * indent, predicate=lambda _: True
        # ).strip()
        doc = textwrap.fill(
            doc, width=72, initial_indent="", subsequent_indent=" " * indent
        )
        file.write(f'{" " * indent}"""{doc}"""\n')

    def python_params(
        self,
        self_: str | None,
        cpp_params: list[ParamInfo],
        *,
        scope: str | None,
        preamble: list[str],
    ) -> list[str]:
        params = [
            f"{p.name or f'_{i}'}: {self.python_type(p.type_, scope=scope, preamble=preamble, as_param=True)}"
            for i, p in enumerate(cpp_params)
        ]
        if self_:
            params.insert(0, self_)
        if params:
            params.append("/")
        return params

    def python_type(
        self,
        cpp_type: TypeInfo,
        *,
        scope: str | None,
        preamble: list[str],
        as_param: bool,
    ) -> str:
        py_type, preamble_ = self._python_type(cpp_type, scope=scope, as_param=as_param)
        preamble += preamble_
        return py_type

    @functools.cache
    def _python_type(
        self, cpp_type: TypeInfo, *, scope: str | None, as_param: bool
    ) -> tuple[str, list[str]]:
        """Return the Python type for a given C++ type.

        If the C++ type is not recognized, return the C++ type as a string.
        If scope is specified, strip the scope from the fully qualified name.
        """
        preamble: list[str] = []
        py_type: str | None = None
        py_typer: str | BuiltinTyper | BuiltinTyperRegex | None = None

        if class_def := self.stubdata.cpp_classes.get(cpp_type):
            if not class_def.fully_qualified_name:
                raise StubGeneratorError(
                    f"Class {class_def.py_name} ({cpp_type}) "
                    + "is not part of a module or class"
                )
            py_type = self._strip_scope(
                class_def.fully_qualified_name, scope=scope, preamble=preamble
            )
            if as_param and class_def.implicit_converters:
                conv_types = [
                    self.python_type(
                        conv, scope=scope, preamble=preamble, as_param=True
                    )
                    for conv in class_def.implicit_converters
                ]
                py_type = f"{py_type} | {' | '.join(conv_types)}"
            return py_type, preamble

        if enum_def := self.stubdata.enums.get(cpp_type):
            if not enum_def.fully_qualified_name:
                raise StubGeneratorError(
                    f"Class {enum_def.py_name} ({cpp_type}) "
                    + "is not part of a module or class"
                )
            py_type = self._strip_scope(
                enum_def.fully_qualified_name, scope=scope, preamble=preamble
            )
            if as_param:
                py_type = f"{py_type} | {enum_def.value_py_type}"
            return py_type, preamble

        if specializations := self.stubdata.export_traits.get(cpp_type.name):
            if py_type := self._match_export_traits(
                cpp_type=cpp_type,
                specializations=specializations,
                scope=scope,
                preamble=preamble,
                as_param=as_param,
            ):
                return py_type, preamble

        if py_typer := BUILTIN_TYPES.get(cpp_type.name):
            if callable(py_typer):
                py_type = py_typer(self, cpp_type.args, scope, preamble, as_param)
                return py_type, preamble
            return py_typer, preamble

        for regex, py_typer in BUILTIN_TYPES_REGEX:
            if match := re.match(regex, cpp_type.name):
                if callable(py_typer):
                    py_type = py_typer(
                        self, cpp_type.args, scope, preamble, as_param, match
                    )
                    return py_type, preamble
                return py_typer, preamble

        if cpp_type.is_pointer:
            py_type = self.python_type(
                cpp_type.base_type, scope=scope, preamble=preamble, as_param=as_param
            )
            return f"{py_type} | None", preamble

        return str(cpp_type), preamble

    def _match_export_traits(
        self,
        *,
        cpp_type: TypeInfo,
        specializations: dict[str, ExportTraits],
        scope: str | None,
        preamble: list[str],
        as_param: bool,
    ) -> str | None:
        # do we have a full specialization?
        if full_specialization := specializations.get(str(cpp_type)):
            if not full_specialization.template_params:
                preamble += full_specialization.preamble
                if as_param and full_specialization.py_type_param:
                    return full_specialization.py_type_param
                return full_specialization.py_type

        # do we have a partial specialization?
        matches = [
            MatchedExportTraits(export_traits, *match)
            for export_traits in specializations.values()
            if (
                match := self._python_type_export_traits(
                    cpp_type=cpp_type,
                    export_traits=export_traits,
                    scope=scope,
                    as_param=as_param,
                )
            )
        ]
        if not matches:
            return None

        if len(matches) == 1:
            preamble += matches[0].preamble
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
        preamble += best[0].preamble
        return best[0].py_type

    def _python_type_export_traits(
        self,
        *,
        cpp_type: TypeInfo,
        export_traits: ExportTraits,
        scope: str | None,
        as_param: bool,
    ) -> tuple[str, MatchedParams, list[str]] | None:
        assert cpp_type.name == export_traits.cpp_type.name
        preamble: list[str] = []

        # match template parameters
        matched_params: MatchedParams = {
            param: None for param in export_traits.template_params
        }
        if not _match_template(
            type_=cpp_type, tmpl=export_traits.cpp_type, matched_params=matched_params
        ):
            return None

        # now substitute template parameters in py_type
        if as_param and export_traits.py_type_param:
            py_type = export_traits.py_type_param
        else:
            py_type = export_traits.py_type
        for param, arg in matched_params.items():
            assert arg is not None
            if isinstance(arg, tuple):
                # this is a pack expansion
                # first substitute strict type T!..., aka not as a parameter
                py_arg = ", ".join(
                    self.python_type(a, scope=scope, preamble=preamble, as_param=False)
                    for a in arg
                )
                py_type = re.sub(rf"\b{re.escape(param)}\!\.\.\.", py_arg, py_type)
                # then substitute non-strict type T..., maybe as a parameter
                py_arg = ", ".join(
                    self.python_type(
                        a, scope=scope, preamble=preamble, as_param=as_param
                    )
                    for a in arg
                )
                py_type = re.sub(rf"\b{re.escape(param)}\.\.\.", py_arg, py_type)
            else:
                # first substitute strict type T!, aka not as a parameter
                py_arg = self.python_type(
                    arg, scope=scope, preamble=preamble, as_param=False
                )
                py_type = re.sub(rf"\b{re.escape(param)}\!", py_arg, py_type)
                # then substitute non-strict type T..., maybe as a parameter
                py_arg = self.python_type(
                    arg, scope=scope, preamble=preamble, as_param=as_param
                )
                py_type = re.sub(rf"\b{re.escape(param)}\b", py_arg, py_type)

        preamble += export_traits.preamble
        return py_type, matched_params, preamble

    def _strip_scope(
        self, fqname: str, *, scope: str | None, preamble: list[str]
    ) -> str:
        """
        Strip the scope from a fully qualified name.

        Adds imports to `preamble` if required.
        """
        if scope and fqname.startswith(scope + "."):
            return fqname[len(scope) + 1 :]
        else:
            if matches := [
                module_def.fully_qualified_name
                for module_def in self.stubdata.modules.values()
                if module_def.fully_qualified_name
                and fqname.startswith(module_def.fully_qualified_name + ".")
            ]:
                longest_match = max(matches, key=len)
                preamble.append(f"import {longest_match}")
            return fqname

    @overload
    def _remove_duplicates(
        self, funcs: Sequence[FunctionDefinition]
    ) -> list[FunctionDefinition]: ...
    @overload
    def _remove_duplicates(
        self, funcs: Sequence[MethodDefinition]
    ) -> list[MethodDefinition]: ...
    def _remove_duplicates(
        self, funcs: Sequence[FunctionDefinition | MethodDefinition]
    ) -> Sequence[FunctionDefinition | MethodDefinition]:
        uniques: dict[
            tuple[tuple[tuple[str, str], ...], str, str | None],
            FunctionDefinition | MethodDefinition,
        ] = {}
        for func in funcs:
            py_params = tuple(
                (
                    p.name,
                    self.python_type(p.type_, scope=None, preamble=[], as_param=True),
                )
                for p in func.cpp_params
            )
            py_return_type = self.python_type(
                func.cpp_return_type, scope=None, preamble=[], as_param=False
            )
            sig_nodoc = (py_params, py_return_type, None)
            if dup := uniques.get(sig_nodoc):
                if isinstance(func, MethodDefinition):
                    assert isinstance(dup, MethodDefinition)
                    assert func.is_static == dup.is_static
                if not dup.doc:
                    if func.doc:
                        # we already have a version without doc, replace it
                        uniques[sig_nodoc] = func
                elif func.doc and dup.doc != func.doc:
                    # both have doc, but different, keep both
                    sig_doc = (py_params, py_return_type, func.doc)
                    uniques[sig_doc] = func
            else:
                uniques[sig_nodoc] = func
        return list(uniques.values())


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

    if tmpl.is_pointer:
        if not type_.is_pointer:
            return False
        tmpl = tmpl.base_type
        type_ = type_.base_type

    if tmpl.name in matched_params:
        # it's a template parameter
        assert tmpl.args is None and tmpl.result is None
        if matched_params[tmpl.name] is None:
            matched_params[tmpl.name] = type_
        elif matched_params[tmpl.name] != type_:
            return False
        return True

    # it's not a template parameter, so it need to match exactly
    if type_.name != tmpl.name:
        return False
    if not _match_template(type_.result, tmpl.result, matched_params):
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
    preamble: list[str]

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


@overload
def _extract_common_docstring(
    funcs: Sequence[FunctionDefinition],
) -> tuple[str | None, list[FunctionDefinition]]: ...
@overload
def _extract_common_docstring(
    funcs: Sequence[MethodDefinition],
) -> tuple[str | None, list[MethodDefinition]]: ...
def _extract_common_docstring(
    funcs: Sequence[FunctionDefinition | MethodDefinition],
) -> tuple[str | None, Sequence[FunctionDefinition | MethodDefinition]]:
    """
    If there's only one overload with a docstring, or they all have the
    same docstring, remove it from the overloads, and add an additional
    stub function with the docstring
    """
    if len(funcs) < 2:
        return None, funcs
    if docs := [doc for func in funcs if (doc := func.doc)]:
        first = docs[0]
        if all(doc == first for doc in docs[1:]):
            funcs = [dataclasses.replace(func, doc=None) for func in funcs]
            return first, funcs
    return None, funcs


def _pytype_sequence(
    stubgen: StubGenerator,
    args: TypeArgs,
    scope: str | None,
    preamble: list[str],
    as_param: bool,
) -> str:
    assert args and len(args) >= 1, args
    element_type = stubgen.python_type(
        args[0], scope=scope, preamble=preamble, as_param=as_param
    )
    return f"Sequence[{element_type}]"


def _pytype_mapping(
    stubgen: StubGenerator,
    args: TypeArgs,
    scope: str | None,
    preamble: list[str],
    as_param: bool,
) -> str:
    assert args and len(args) >= 2, args
    key_type = stubgen.python_type(
        args[0], scope=scope, preamble=preamble, as_param=as_param
    )
    value_type = stubgen.python_type(
        args[1], scope=scope, preamble=preamble, as_param=as_param
    )
    return f"Mapping[{key_type}, {value_type}]"


BuiltinTyper: TypeAlias = Callable[
    [StubGenerator, TypeArgs, str | None, list[str], bool], str
]

BUILTIN_TYPES: dict[str, str | BuiltinTyper] = {
    "void": "None",
    "_object *": "Any",
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
    "lass::stde::static_vector": _pytype_sequence,
    "lass::stde::vector_map": _pytype_mapping,
}

BuiltinTyperRegex: TypeAlias = Callable[
    [StubGenerator, TypeArgs, str | None, list[str], bool, re.Match[str]], str
]


BUILTIN_TYPES_REGEX: list[tuple[re.Pattern[str], str | BuiltinTyperRegex]] = [
    (re.compile(r"char\[\d+\]"), "str"),
    (re.compile(r"wchar_t\[\d+\]"), "str"),
]
