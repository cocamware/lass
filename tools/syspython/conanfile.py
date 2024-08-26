"""
*** BEGIN LICENSE INFORMATION ***

The contents of this file are subject to the Common Public Attribution License
Version 1.0 (the "License"); you may not use this file except in compliance with
the License. You may obtain a copy of the License at
https://lass.cocamware.com/cpal-license. The License is based on the
Mozilla Public License Version 1.1 but Sections 14 and 15 have been added to cover
use of software over a computer network and provide for limited attribution for
the Original Developer. In addition, Exhibit A has been modified to be consistent
with Exhibit B.

Software distributed under the License is distributed on an "AS IS" basis, WITHOUT
WARRANTY OF ANY KIND, either express or implied. See the License for the specific
language governing rights and limitations under the License.

The Original Code is LASS - Library of Assembled Shared Sources.

The Initial Developer of the Original Code is Bram de Greve and Tom De Muer.
The Original Developer is the Initial Developer.

All portions of the code written by the Initial Developer are:
Copyright (C) 2023-2024 the Initial Developer.
All Rights Reserved.

Contributor(s):

Alternatively, the contents of this file may be used under the terms of the
GNU General Public License Version 2 or later (the GPL), in which case the
provisions of GPL are applicable instead of those above.  If you wish to allow use
of your version of this file only under the terms of the GPL and not to allow
others to use your version of this file under the CPAL, indicate your decision by
deleting the provisions above and replace them with the notice and other
provisions required by the GPL License. If you do not delete the provisions above,
a recipient may use your version of this file under either the CPAL or the GPL.

*** END LICENSE INFORMATION ***
"""

import os
import re
import subprocess
from functools import cache, cached_property
from string import Template
from typing import Optional

from conan import ConanFile
from conan.errors import ConanInvalidConfiguration
from conan.tools.files import copy, load, save


class SysPython(ConanFile):
    name = "syspython"
    version = "1.0.2"
    user = "cocamware"
    channel = "stable"
    description = "Discovers your system's Python and allow to use it as a requirement"
    settings = "os", "arch"
    package_type = "library"
    build_policy = "missing"
    upload_policy = "skip"

    options = {
        "shared": [None, True, False],
        "python_executable": [None, "ANY"],
        "python_version": [None, "ANY"],
        "python_debug": [None, True, False],
    }
    default_options = {
        "shared": None,
        "python_executable": None,
        "python_version": None,
        "python_debug": None,
    }

    exports_sources = "*.tmpl.cmake"

    def configure(self):
        # Do *not* rely on any checks or queries involving
        # options.python_executable in configure. Potential build requirements
        # can influence PATH, and that's no in effect in configure nor in
        # package_id.
        #
        # Exception is when python_version and python_debug are not explicitly
        # configured, because this is your last change to do so.
        #
        # This means, if you rely on python to be found on a build requirement's
        # PATH, you must be explicit about python_version and python_debug.
        #
        if not self.options.python_executable:
            if self.settings.os == "Windows":
                if str(self.options.python_debug) == "True":
                    self.options.python_executable = "python_d.exe"
                else:
                    # if we don't don't explicitly use a debug build of Python, assume
                    # we build against a release build of it, even if our build_type is
                    # Debug. This is the most common use case.
                    self.options.python_executable = "python.exe"
                    self.options.python_debug = False
            else:
                self.options.python_executable = "python3"
        # _ = self._python_executable
        if not self.options.python_version:
            self.options.python_version = self._python_version_short
        if self.options.python_debug.value in [None, "None"]:
            self.options.python_debug = self._python_debug
        if self.options.shared.value in [None, "None"]:
            self.options.shared = self._python_has_shared

    def validate(self):
        if str(self.options.python_version) != self._python_version_short:
            raise ConanInvalidConfiguration(
                "python_version option not compatible with python_executable:"
                f"{self.options.python_version!s} != {self._python_version_short}"
            )
        if self.options.python_debug != self._python_debug:
            raise ConanInvalidConfiguration(
                "python_debug option not compatible with python_executable."
            )
        if self.options.shared != self._python_shared:
            raise ConanInvalidConfiguration(
                f"shared={self.options.shared} option not compatible with "
                f"python_executable={self._python_executable}."
            )
        if self._python_bitness != self._arch_bitness:
            raise ConanInvalidConfiguration(
                "arch not compatible with python_executable."
            )

    def layout(self):
        self.folders.source = "."
        self.folders.build = "build"
        self.folders.generators = os.path.join(self.folders.build, "conan")
        for cpp_info in [self.cpp.package, self.cpp.build]:
            cpp_info.libs = [self._python_library]
            cpp_info.includedirs = self._python_include_dirs
            cpp_info.libdirs = self._python_library_dirs
            cpp_info.bindirs = self._python_bin_dirs
            cpp_info.builddirs = ["."]
            cpp_info.set_property("cmake_find_mode", "none")

    def build(self):
        def escape(value):
            if value is None:
                return ""
            if isinstance(value, bool):
                return "TRUE" if value else "FALSE"
            if isinstance(value, int):
                return value
            if isinstance(value, str):
                value = value.replace("\\", "/").replace('"', '\\"')
                return f'"{value}"'
            if isinstance(value, list):
                return " ".join(escape(val) for val in value)
            raise TypeError(value)

        major, minor, patch = self._python_version.split(".")
        values = {
            "executable": self._python_executable,
            "library": self._python_library,
            "library_dirs": self._python_library_dirs,
            "runtime_library": self._python_runtime_library,
            "runtime_library_dirs": self._python_runtime_library_dirs,
            "sabi_library": self._python_sabi_library,
            "sabi_library_dirs": self._python_sabi_library_dirs,
            "sabi_runtime_library": self._python_sabi_runtime_library,
            "sabi_runtime_library_dirs": self._python_sabi_runtime_library_dirs,
            "sabi_found": bool(self._python_sabi_library),
            "include_dirs": self._python_include_dirs,
            "link_libraries": self._python_link_libraries,
            "version": self._python_version,
            "major": major,
            "minor": minor,
            "patch": patch,
            "stdlib": self._python_get_path("stdlib"),
            "stdarch": self._python_get_path("platstdlib"),
            "sitelib": self._python_get_path("purelib"),
            "sitearch": self._python_get_path("platlib"),
            "soabi": self._python_soabi,
            "sosabi": self._python_sosabi,
            "postfix": self._python_postfix,
            "library_type": "SHARED" if self._python_shared else "STATIC",
            "sizeof_void_p": self._python_sizeof_void_p,
            "bitness": self._python_bitness,
        }

        for file in ["PythonConfig", "PythonConfigVersion"]:
            template = AtTemplate(
                load(self, os.path.join(self.source_folder, f"{file}.tmpl.cmake"))
            )
            substituted = template.substitute(
                {key: escape(value) for key, value in values.items()}
            )
            save(self, f"{file}.cmake", substituted)

    def package(self):
        copy(self, "*.cmake", self.build_folder, self.package_folder)

    def package_id(self):
        self.info.options.python_executable = self._python_executable

    @cached_property
    def _python_executable(self):
        try:
            return subprocess.check_output(
                [
                    str(self.options.python_executable),
                    "-c",
                    "import sys; print(sys.executable)",
                ],
                text=True,
            ).strip()
        except FileNotFoundError as err:
            raise ConanInvalidConfiguration(
                f"Python executable cannot be found: {self.options.python_executable!s}"
            ) from err

    @property
    def _python_version(self) -> str:
        """Short version like 3.7"""
        return self._python_get_config_var("py_version")

    @property
    def _python_version_short(self) -> str:
        """Short version like 3.7"""
        return self._python_get_config_var("py_version_short")

    @property
    def _python_version_short_int(self) -> str:
        """Short version like (3, 7)"""
        major, minor = self._python_version_short.split(".")
        return int(major), int(minor)

    @property
    def _python_debug(self) -> bool:
        """True if python library is built with Py_DEBUG"""
        output = self._python_query(
            "import sys; print(int(hasattr(sys, 'gettotalrefcount')))"
        )
        return bool(int(output))

    @property
    def _python_has_shared(self) -> bool:
        if self.settings.os == "Windows":
            return True
        else:
            return bool(int(self._python_get_config_var("Py_ENABLE_SHARED") or 0))

    @property
    def _python_shared(self) -> bool:
        if self.settings.os == "Windows":
            return True
        else:
            return self._python_library.endswith(".so")

    @cached_property
    def _python_library(self) -> str:
        """Full path to python library you should link to"""
        if self.settings.os == "Windows":
            base = self._python_get_config_var("installed_platbase")
            version = self._python_get_config_var("py_version_nodot")
            return os.path.join(
                base, "libs", f"python{version}{self._python_postfix}.lib"
            )
        else:
            hints: list[str] = []
            if self.options.shared:
                fname = self._python_get_config_var("LDLIBRARY")
                if not fname:
                    raise ConanInvalidConfiguration(f"no shared library")
                ld_library_path = os.getenv("LD_LIBRARY_PATH")
                if ld_library_path:
                    hints.extend(ld_library_path.split(os.pathsep))
            else:
                fname = self._python_get_config_var("LIBRARY")
                if not fname:
                    raise ConanInvalidConfiguration(f"no static library")
            hints += [
                self._python_get_config_var("LIBDIR"),
                self._python_get_config_var("LIBPL"),
            ]
            for dirname in filter(None, hints):
                candidate = os.path.join(dirname, fname)
                if os.path.isfile(candidate):
                    return candidate
            raise RuntimeError(f"Unable to find {fname}")

    @property
    def _python_library_dirs(self) -> list[str]:
        return [os.path.dirname(self._python_library)]

    @property
    def _python_runtime_library(self) -> Optional[str]:
        if self.settings.os == "Windows":
            base = self._python_get_config_var("installed_platbase")
            version = self._python_get_config_var("py_version_nodot")
            return os.path.join(base, f"python{version}{self._python_postfix}.dll")
        return None

    @property
    def _python_runtime_library_dirs(self) -> list[str]:
        if not self._python_runtime_library:
            return []
        return [os.path.dirname(self._python_runtime_library)]

    @property
    def _python_bin_dirs(self) -> list[str]:
        bin_dirs = [self._python_get_config_var("BINDIR")]
        bin_dirs.extend(self._python_runtime_library_dirs)
        if self.settings.os == "Windows":
            base = self._python_get_config_var("installed_platbase")
            platlibdir = self._python_get_config_var("platlibdir")
            bin_dirs.append(os.path.join(base, platlibdir))
        return unique(bin_dirs)

    @property
    def _python_sabi_library(self) -> Optional[str]:
        if self.settings.os == "Windows":
            base = self._python_get_config_var("installed_platbase")
            major, _minor = self._python_get_config_var("py_version_short").split(".")
            return os.path.join(base, f"python{major}{self._python_postfix}.lib")
        return None

    @property
    def _python_sabi_library_dirs(self) -> list[str]:
        if not self._python_sabi_library:
            return []
        return [os.path.dirname(self._python_sabi_library)]

    @property
    def _python_sabi_runtime_library(self) -> Optional[str]:
        if self.settings.os == "Windows":
            base = self._python_get_config_var("installed_platbase")
            major, _minor = self._python_get_config_var("py_version_short").split(".")
            return os.path.join(base, f"python{major}{self._python_postfix}.dll")
        return None

    @property
    def _python_sabi_runtime_library_dirs(self) -> list[str]:
        if not self._python_sabi_runtime_library:
            return []
        return [os.path.dirname(self._python_sabi_runtime_library)]

    @property
    def _python_link_libraries(self) -> list[str]:
        if self._python_shared:
            return []
        libs = self._python_get_config_var("LIBS")
        return [lib for lib in libs.split() if lib.startswith("-l")]

    @property
    def _python_include_dirs(self) -> list[str]:
        includedirs = [
            self._python_get_path("include"),
            self._python_get_path("platinclude"),
            self._python_get_config_var("INCLUDEPY"),
        ]
        return unique(includedirs)

    @property
    def _python_soabi(self) -> str:
        soabi = self._python_get_config_var("SOABI")
        if not soabi:
            version = self._python_version_short_int
            if self.settings.os == "Windows" and version <= (3, 9):
                ext_suffix = self._python_query(
                    "from distutils import sysconfig\n"
                    + "print(sysconfig.get_config_var('EXT_SUFFIX') or '')"
                )
            else:
                ext_suffix = self._python_get_config_var("EXT_SUFFIX")
            match = re.match(r"^(_d)?\.(?P<soabi>.+)\.(so|lib|pyd)$", ext_suffix)
            if not match:
                raise ConanInvalidConfiguration(f"Unexpected EXT_SUFFIX: {ext_suffix}")
            soabi = match.group("soabi")
        return soabi

    @property
    def _python_sosabi(self) -> str:
        if self.settings.os == "Windows":
            return ""
        else:
            major, _minor = self._python_version_short.split(".")
            return f"abi{major}"

    @property
    def _python_postfix(self) -> str:
        if self.settings.os == "Windows" and self._python_debug:
            return "_d"
        else:
            return ""

    @property
    def _python_sizeof_void_p(self) -> int:
        return int(self._python_query("import struct; print(struct.calcsize('P'))"))

    @property
    def _python_bitness(self) -> int:
        return 8 * self._python_sizeof_void_p

    def _python_get_path(self, key: str) -> str:
        return self._python_query(
            f"import sysconfig; print(sysconfig.get_path({key!r}))"
        )

    def _python_get_config_var(self, key: str) -> str:
        return self._python_query(
            f"import sysconfig; print(sysconfig.get_config_var({key!r}) or '')"
        )

    @cache
    def _python_query(self, script: str) -> str:
        return subprocess.check_output(
            [self._python_executable, "-c", script], text=True
        ).strip()

    @property
    def _arch_bitness(self) -> int:
        arch = str(self.settings.arch)
        if arch == "x86":
            return 32
        if arch == "x86_64":
            return 64
        if arch.startswith("arm64"):
            return 64
        if match := re.match(r"armv(\d+)", arch):
            if int(match.group(1)) < 8:
                return 32
            else:
                return 64
        raise ConanInvalidConfiguration(f"Unsupported arch: {arch}")


class AtTemplate(Template):
    """String template class using @ as delimiter"""

    delimiter = "@"


def unique(iterable):
    return list(dict.fromkeys(iterable))
