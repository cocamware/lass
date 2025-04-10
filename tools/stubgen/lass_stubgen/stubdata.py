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
from collections.abc import Mapping
from dataclasses import dataclass, field
from typing import Any, Self, TypeAlias

StrPath: TypeAlias = str | os.PathLike[str]


__all__ = [
    "StubData",
    "ModuleDefinition",
    "FunctionDefinition",
    "ConstDefinition",
    "ClassDefinition",
    "EnumDefinition",
    "ConstructorDefinition",
    "MethodDefinition",
    "GetSetterDefinition",
    "TypeInfo",
    "ExportTraits",
    "DuplicateError",
]


class StubData:
    package: str | None
    modules: dict[str, ModuleDefinition]
    shadow_classes: dict[str, ClassDefinition]
    cpp_classes: dict[str, ClassDefinition]
    enums: dict[str, EnumDefinition]
    export_traits: dict[str, dict[str, ExportTraits]]

    def __init__(self, package: str | None = None):
        self.package = package
        self.modules = {}
        self.shadow_classes = {}
        self.cpp_classes = {}
        self.enums = {}
        self.export_traits = {}

    def asdict(self) -> dict[str, Any]:
        return {
            "package": self.package,
            "modules": [mod_def.asdict() for mod_def in self.modules.values()],
            "classes": [
                class_def.asdict() for class_def in self.shadow_classes.values()
            ],
            "enums": [enum_def.asdict() for enum_def in self.enums.values()],
            "export_traits": [
                traits.asdict()
                for specializations in self.export_traits.values()
                for traits in specializations.values()
            ],
        }

    def dump(self, path: StrPath) -> None:
        """
        Dump stub data to a JSON file.
        """
        os.makedirs(os.path.dirname(path), exist_ok=True)
        with open(path, "w", encoding="utf-8", newline="\n") as fp:
            json.dump(self.asdict(), fp, indent=2)

    @classmethod
    def fromdict(cls, data: dict[str, Any]) -> Self:
        stubdata = cls(data["package"])
        for mod_data in data["modules"]:
            stubdata.add_module_definition(ModuleDefinition.fromdict(mod_data))
        for class_data in data["classes"]:
            stubdata.add_class_definition(ClassDefinition.fromdict(class_data))
        for enum_data in data["enums"]:
            stubdata.add_enum_definition(EnumDefinition.fromdict(enum_data))
        for traits_data in data["export_traits"]:
            stubdata.add_export_traits(ExportTraits.fromdict(traits_data))
        return stubdata

    @classmethod
    def load(cls, path: StrPath, imported: bool = False) -> Self:
        """
        Load stub data from a JSON file.
        """
        with open(path, "r", encoding="utf-8") as fp:
            data = json.load(fp)
        stubdata = cls.fromdict(data)
        if imported:
            for mod_def in stubdata.modules.values():
                mod_def.imported = True
        return stubdata

    def update(self, stubdata: StubData) -> None:
        """
        Merge the current state with another StubData instance.

        Both instances must be from the same package.
        """
        assert self.package == stubdata.package
        for mod_def in stubdata.modules.values():
            self.add_module_definition(mod_def)
        for class_def in stubdata.shadow_classes.values():
            self.add_class_definition(class_def)
        for enum_def in stubdata.enums.values():
            self.add_enum_definition(enum_def)
        for class_, specializations in stubdata.export_traits.items():
            # duplicates are allowed
            self.export_traits.setdefault(class_, {}).update(specializations)

    def add_module_definition(self, mod_def: ModuleDefinition) -> None:
        if mod_def.cpp_name in self.modules:
            raise DuplicateError(mod_def.cpp_name, "module")
        self.modules[mod_def.cpp_name] = mod_def

    def add_class_definition(self, class_def: ClassDefinition) -> None:
        if class_def.shadow_name in self.shadow_classes:
            raise DuplicateError(class_def.shadow_name, "shadow class")
        cpp_class = str(class_def.cpp_type)
        if cpp_class in self.cpp_classes:
            raise DuplicateError(cpp_class, "C++ class")
        self.shadow_classes[class_def.shadow_name] = class_def
        self.cpp_classes[cpp_class] = class_def

    def add_enum_definition(self, enum_def: EnumDefinition) -> None:
        if enum_def.cpp_name in self.enums:
            raise DuplicateError(enum_def.cpp_name, "enum")
        self.enums[enum_def.cpp_name] = enum_def

    def add_export_traits(self, export_traits: ExportTraits) -> None:
        cpp_type = export_traits.cpp_type
        specializations = self.export_traits.setdefault(cpp_type.name, {})
        specializations[str(cpp_type)] = export_traits

    def fix_fully_qualified_names(self) -> None:
        """Fix the fully qualified names of all modules, classes and enums."""
        for mod_def in self.modules.values():
            if mod_def.imported:
                continue
            self._fix_fully_qualified_name_module(mod_def, scope=self.package)

    def _fix_fully_qualified_name_module(
        self, mod_def: ModuleDefinition, *, scope: str | None
    ) -> None:
        if scope:
            fully_qualified_name = f"{scope}.{mod_def.py_name}"
        else:
            fully_qualified_name = mod_def.py_name
        assert (
            not mod_def.fully_qualified_name
            or mod_def.fully_qualified_name == fully_qualified_name
        )
        mod_def.fully_qualified_name = fully_qualified_name

        for shadow_name in mod_def.classes:
            class_def = self.shadow_classes[shadow_name]
            self._fix_fully_qualified_name_class(class_def, scope=fully_qualified_name)
        for enum_name in mod_def.enums:
            enum_def = self.enums[enum_name]
            self._fix_fully_qualified_name_enum(enum_def, scope=fully_qualified_name)

    def _fix_fully_qualified_name_class(
        self, class_def: ClassDefinition, *, scope: str
    ) -> None:
        fully_qualified_name = f"{scope}.{class_def.py_name}"
        assert (
            not class_def.fully_qualified_name
            or class_def.fully_qualified_name == fully_qualified_name
        )
        class_def.fully_qualified_name = fully_qualified_name

        for inner_class in class_def.inner_classes:
            inner_def = self.shadow_classes[inner_class]
            self._fix_fully_qualified_name_class(inner_def, scope=fully_qualified_name)
        for enum_ in class_def.inner_enums:
            enum_def = self.enums[enum_]
            self._fix_fully_qualified_name_enum(enum_def, scope=fully_qualified_name)

    def _fix_fully_qualified_name_enum(
        self, enum_def: EnumDefinition, *, scope: str
    ) -> None:
        fully_qualified_name = f"{scope}.{enum_def.py_name}"
        assert (
            not enum_def.fully_qualified_name
            or enum_def.fully_qualified_name == fully_qualified_name
        )
        enum_def.fully_qualified_name = fully_qualified_name


@dataclass
class ModuleDefinition:
    cpp_name: str
    py_name: str
    doc: str | None
    fully_qualified_name: str | None = None
    classes: list[str] = field(default_factory=list)
    enums: list[str] = field(default_factory=list)
    functions: dict[str, list[FunctionDefinition]] = field(default_factory=dict)
    constants: dict[str, ConstDefinition] = field(default_factory=dict)
    imported: bool = False

    def add_class(self, shadow_class: str) -> None:
        self.classes.append(shadow_class)

    def add_enum(self, enum_name: str) -> None:
        self.enums.append(enum_name)

    def add_function(self, func_def: FunctionDefinition) -> None:
        self.functions.setdefault(func_def.py_name, []).append(func_def)

    def add_constant(self, const_def: ConstDefinition) -> None:
        self.constants[const_def.py_name] = const_def

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
class ClassDefinition:
    py_name: str
    shadow_name: str
    cpp_type: TypeInfo
    parent_type: str
    doc: str | None = None
    fully_qualified_name: str | None = None
    inner_classes: list[str] = field(default_factory=list)
    inner_enums: list[str] = field(default_factory=list)
    constructors: list[ConstructorDefinition] = field(default_factory=list)
    methods: dict[str, list[MethodDefinition]] = field(default_factory=dict)
    getsetters: dict[str, GetSetterDefinition] = field(default_factory=dict)
    consts: dict[str, ConstDefinition] = field(default_factory=dict)
    _cpp_constructors: dict[str, ConstructorDefinition] = field(default_factory=dict)

    def add_inner_class(self, shadow_class: str) -> None:
        self.inner_classes.append(shadow_class)

    def add_enum(self, cpp_name: str) -> None:
        self.inner_enums.append(cpp_name)

    def add_constructor(self, constr_def: ConstructorDefinition) -> None:
        # if constructor doesn't have parameter names, then search for a
        # C++ constructor with the same signature and reuse its definition
        if not any(name for name, _ in constr_def.cpp_params):
            if cpp_constr := self._cpp_constructors.get(constr_def.cpp_signature):
                constr_def = cpp_constr
        self.constructors.append(constr_def)

    def add_method(self, method_def: MethodDefinition) -> None:
        self.methods.setdefault(method_def.py_name, []).append(method_def)

    def add_getsetter(self, getsetter_def: GetSetterDefinition) -> None:
        self.getsetters[getsetter_def.py_name] = getsetter_def

    def add_const(self, const_def: ConstDefinition) -> None:
        self.consts[const_def.py_name] = const_def

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
        _cpp_constructors: list[dict[str, Any]] = []
        for constr_def in self._cpp_constructors.values():
            _cpp_constructors.append(constr_def.asdict())
        return {
            "py_name": self.py_name,
            "cpp_type": self.cpp_type.asdict(),
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
            "_cpp_constructors": _cpp_constructors,
        }

    @classmethod
    def fromdict(cls, data: dict[str, Any]) -> Self:
        cpp_type = TypeInfo.fromdict(data["cpp_type"])
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
        consts: dict[str, ConstDefinition] = {}
        for const in data["consts"]:
            const_def = ConstDefinition.fromdict(const)
            consts[const_def.py_name] = const_def
        _cpp_constructors: dict[str, ConstructorDefinition] = {}
        for constr in data["constructors"]:
            constr_def = ConstructorDefinition.fromdict(constr)
            _cpp_constructors[constr_def.cpp_signature] = constr_def
        return cls(
            py_name=data["py_name"],
            shadow_name=data["shadow_name"],
            cpp_type=cpp_type,
            parent_type=data["parent_type"],
            doc=data["doc"],
            fully_qualified_name=data["fully_qualified_name"],
            inner_classes=data["inner_classes"],
            inner_enums=data["inner_enums"],
            constructors=constructors,
            methods=methods,
            getsetters=getsetters,
            consts=consts,
            _cpp_constructors=_cpp_constructors,
        )


@dataclass
class FunctionDefinition:
    py_name: str
    doc: str | None
    cpp_return_type: TypeInfo
    cpp_params: list[ParamInfo]
    cpp_signature: str

    def asdict(self) -> dict[str, Any]:
        return dataclasses.asdict(self)

    @classmethod
    def fromdict(cls, data: Any) -> Self:
        cpp_params = [
            (name, TypeInfo.fromdict(type_)) for (name, type_) in data["cpp_params"]
        ]
        cpp_return_type = TypeInfo.fromdict(data["cpp_return_type"])
        return cls(
            py_name=data["py_name"],
            doc=data["doc"],
            cpp_return_type=cpp_return_type,
            cpp_params=cpp_params,
            cpp_signature=data["cpp_signature"],
        )


@dataclass
class ConstDefinition:
    py_name: str
    cpp_type: TypeInfo
    value: Any = None

    def asdict(self) -> dict[str, Any]:
        return dataclasses.asdict(self)

    @classmethod
    def fromdict(cls, data: Any) -> Self:
        cpp_type = TypeInfo.fromdict(data["cpp_type"])
        return cls(
            py_name=data["py_name"],
            cpp_type=cpp_type,
            value=data["value"],
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
    cpp_params: list[ParamInfo]
    cpp_signature: str

    def asdict(self) -> dict[str, Any]:
        return dataclasses.asdict(self)

    @classmethod
    def fromdict(cls, data: Any) -> Self:
        cpp_params = [
            (name, TypeInfo.fromdict(type_)) for (name, type_) in data["cpp_params"]
        ]
        return cls(
            cpp_params=cpp_params,
            cpp_signature=data["cpp_signature"],
        )


@dataclass
class MethodDefinition:
    py_name: str
    doc: str | None
    cpp_return_type: TypeInfo
    cpp_params: list[ParamInfo]
    cpp_signature: str
    is_static: bool = False

    def asdict(self) -> dict[str, Any]:
        return dataclasses.asdict(self)

    @classmethod
    def fromdict(cls, data: Any) -> Self:
        cpp_params = [
            (name, TypeInfo.fromdict(type_)) for (name, type_) in data["cpp_params"]
        ]
        cpp_return_type = data["cpp_return_type"]
        cpp_return_type = TypeInfo.fromdict(cpp_return_type)
        return cls(
            py_name=data["py_name"],
            doc=data["doc"],
            cpp_return_type=cpp_return_type,
            cpp_params=cpp_params,
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

    def substitute(self, template_args: Mapping[str, str | TypeInfo]) -> TypeInfo:
        """
        Substitute template parameters in the type.
        """
        if tmpl_arg := template_args.get(self.name):
            assert not self.args
            return TypeInfo(tmpl_arg) if isinstance(tmpl_arg, str) else tmpl_arg
        if self.name.endswith("..."):
            # this is a variadic template
            assert self.args is None
            tmpl_arg = template_args[self.name[:-3]]
            assert isinstance(tmpl_arg, str)
            return TypeInfo(f"{tmpl_arg}...")
        if self.args is None:
            return self
        args = [arg.substitute(template_args) for arg in self.args]
        return TypeInfo(self.name, args)


ParamInfo: TypeAlias = tuple[str, TypeInfo]


@dataclass
class ExportTraits:
    cpp_type: TypeInfo
    py_type: str
    template_params: list[str] = field(default_factory=list)

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


class DuplicateError(Exception):
    """
    Exception raised when a duplicate is found in the stub data.
    """

    def __init__(self, name: str, kind: str):
        super().__init__(kind, name)
        self.name = name
        self.kind = kind

    def __str__(self):
        return f"Duplicate {self.kind} found: {self.name}"
