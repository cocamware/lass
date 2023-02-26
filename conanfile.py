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
Copyright (C) 2018-2023 the Initial Developer.
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
import shutil
import subprocess
from importlib.util import module_from_spec, spec_from_file_location
from pathlib import Path
from typing import List, Optional

from conan import ConanFile, conan_version
from conan.errors import ConanException, ConanInvalidConfiguration
from conan.tools.build import check_min_cppstd
from conan.tools.cmake import CMake, CMakeToolchain, cmake_layout
from conan.tools.scm import Git

required_conan_version = ">=1.54.0"

def _get_version(conanfile: ConanFile) -> Optional[str]:
    try:
        content = (Path(conanfile.recipe_folder) / "CMakeLists.txt").read_text()
    except FileNotFoundError:
        return None  # Assume conan metadata already knows
    match = re.search(r"project\(Lass VERSION (\d+\.\d+\.\d+)\)", content)
    assert match
    version = match.group(1)

    git = Git(conanfile)

    # if this version has been tagged in Git, it's either this very commit (and a
    # release version), or a post-release (which is not allowed).
    version_tag = f"lass-{version}"
    try:
        describe = git.run(f"describe --abbrev=8 --dirty --tags --match {version_tag}")
    except (ConanException, subprocess.CalledProcessError):
        pass  # release tag doesn't exist yet, it's a pre-release.
    else:
        assert describe == version_tag, (
            f"Post-release builds are not allowed. Set base_tag to {version_tag!r} and "
            + "increment project(Lass VERSION X.Y.Z) in CMakeLists.txt."
        )
        return version

    try:
        describe = git.run("describe --abbrev=8 --dirty --tags --match lass-*")
    except ConanException:
        return None  # Assume conan metadata already knows
    match = re.match(r"lass-(\d+\.\d+\.\d+)-(\d+)-(g\w+(-dirty)?)$", describe)
    assert match, "unexpected describe format: {!r}".format(describe)
    prev_version, pre_release, rev = match.group(1), match.group(2), match.group(3)
    assert _int_version(prev_version) < _int_version(version)
    return f"{version}-{pre_release}+{rev}"


def _int_version(str_version: str):
    return [int(x) for x in str_version.split(".")]


class LassConan(ConanFile):
    name = "Lass" if conan_version.major == "1" else "lass"
    package_type = "library"

    def set_version(self):
        self.version = self.version or _get_version(self)

    license = "CPAL-1.0", "GPL-2.0-or-later"
    author = "Cocamware <info@cocamware.com>"
    url = "https://github.com/cocamware/lass.git"
    description = "Library of Assembled Shared Source."
    topics = "C++", "Python"
    settings = "os", "compiler", "build_type", "arch"
    options = {
        "shared": [True, False],
        "fPIC": [True, False],
        "simd_aligned": [True, False],
        "without_iterator_debugging": [True, False],
        "python_executable": [None, "ANY"],
        "python_version": [None, "ANY"],
        "python_debug": [None, True, False],
    }
    default_options = {
        "shared": True,
        "fPIC": True,
        "simd_aligned": False,
        "without_iterator_debugging": False,
        "python_executable": None,
        "python_version": None,
        "python_debug": None,
    }

    def export_sources(self):
        git = Git(self)
        for src in git.included_files():
            dest = os.path.join(self.export_sources_folder, src)
            os.makedirs(os.path.dirname(dest), exist_ok=True)
            shutil.copy2(src, dest)

    def config_options(self):
        if str(self.settings.compiler) in ["msvc", "Visual Studio"]:
            del self.options.fPIC

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
        if not self.options.python_version:
            self.options.python_version = Python(
                self.options.python_executable, self.settings
            ).version
        if self.options.python_debug.value in [None, "None"]:
            self.options.python_debug = Python(
                self.options.python_executable, self.settings
            ).debug

    def validate(self):
        check_min_cppstd(self, 17)
        if self.settings.get_safe('compiler') in ["gcc", "clang"]:
            if self.settings.get_safe('compiler.libcxx') in ["libstdc++"]:
                raise ConanInvalidConfiguration(
                    "gcc and clang require C++11 compatible libcxx"
                )

    def layout(self):
        cmake_layout(self)

    def generate(self):
        python = Python(self.options.python_executable, self.settings)
        if str(self.options.python_version) != python.version:
            raise ConanInvalidConfiguration(
                "python_version option not compatible with python_executable:"
                "{} != {}".format(str(self.options.python_version), python.version)
            )
        if self.options.python_debug != python.debug:
            raise ConanInvalidConfiguration(
                "python_debug option not compatible with python_executable."
            )
        if self.settings.os == "Windows":
            if python.debug and self.settings.build_type != "Debug":
                raise ConanInvalidConfiguration(
                    "Can't build non-debug Lass with debug Python on Windows."
                )

        tc = CMakeToolchain(self)
        tc.variables["CMAKE_CONFIGURATION_TYPES"] = self.settings.build_type
        tc.variables["BUILD_SIMD_ALIGNED"] = bool(self.options.simd_aligned)
        tc.variables["BUILD_WITHOUT_ITERATOR_DEBUGGING"] = bool(
            self.options.without_iterator_debugging
        )
        tc.variables["Lass_PYTHON_VERSION"] = python.version
        tc.variables["Python_EXECUTABLE"] = python.executable.as_posix()
        tc.variables["Python_LIBRARY_RELEASE"] = python.library.as_posix()
        tc.variables["Python_LIBRARY_DEBUG"] = python.library.as_posix()
        tc.generate()

    def build(self):
        cmake = CMake(self)
        cmake.configure()
        cmake.build()
        cmake.test()

    def package(self):
        cmake = CMake(self)
        cmake.install()

    def package_info(self):
        lass_config = self._lass_config()
        if self.settings.build_type == "Debug":
            libs = [
                lass_config.LASS_OUTPUT_NAME_DEBUG,
                lass_config.LASS_PYTHON_OUTPUT_NAME_DEBUG,
            ]
        else:
            libs = [
                lass_config.LASS_OUTPUT_NAME,
                lass_config.LASS_PYTHON_OUTPUT_NAME,
            ]
        self.cpp_info.libs = [lib for lib in libs if lib]
        self.cpp_info.cxxflags = lass_config.LASS_EXTRA_CXX_FLAGS

    def _lass_config(self):
        module_name = "LassConfig"
        file_path = Path(self.package_folder) / "share/Lass/LassConfig.py"
        spec = spec_from_file_location(module_name, file_path)
        module = module_from_spec(spec)
        spec.loader.exec_module(module)
        return module


class Python(object):
    """Build and config info of a Python"""

    def __init__(self, executable, settings):
        found_executable = shutil.which(str(executable))
        if not found_executable:
            raise ConanInvalidConfiguration(
                f"Python executable cannot be found: {executable!s}"
            )
        self._executable = Path(found_executable)
        self._os = settings.os

    @property
    def executable(self) -> Path:
        """Full path to Python executable"""
        return self._executable

    @property
    def version(self) -> str:
        """Short version like 3.7"""
        # pylint: disable=attribute-defined-outside-init
        try:
            return self._version
        except AttributeError:
            self._version = self._get_config_var("py_version_short")
            return self._version

    @property
    def debug(self) -> bool:
        """True if python library is built with Py_DEBUG"""
        # pylint: disable=attribute-defined-outside-init
        try:
            return self._debug
        except AttributeError:
            output = self._query(
                "import sys; print(int(hasattr(sys, 'gettotalrefcount')))"
            )
            self._debug = bool(int(output))
            return self._debug

    @property
    def library(self) -> Path:
        """Full path to python library you should link to"""
        # pylint: disable=attribute-defined-outside-init
        try:
            return self._library
        except AttributeError:
            if self._os == "Windows":
                stdlib = self._get_python_path("stdlib")
                version = self._get_config_var("py_version_nodot")
                postfix = "_d" if self.debug else ""
                self._library = stdlib.parent / f"libs/python{version}{postfix}.lib"
            else:
                fname = self._get_config_var("LDLIBRARY")
                hints: List[Path] = []
                ld_library_path = os.getenv("LD_LIBRARY_PATH")
                if ld_library_path:
                    hints.extend(
                        Path(dirname) for dirname in ld_library_path.split(os.pathsep)
                    )
                hints += [
                    Path(self._get_config_var("LIBDIR")),
                    Path(self._get_config_var("LIBPL")),
                ]
                for dirname in hints:
                    candidate = dirname / fname
                    if candidate.is_file():
                        self._library = candidate
                        break
                else:
                    raise RuntimeError(f"Unable to find {fname}")
            return self._library

    def _get_python_path(self, key: str) -> Path:
        return Path(
            self._query(f"import sysconfig; print(sysconfig.get_path({key!r}))")
        )

    def _get_config_var(self, key: str) -> str:
        return self._query(
            f"import sysconfig; print(sysconfig.get_config_var({key!r}))"
        )

    def _query(self, script: str) -> str:
        return subprocess.check_output(
            [str(self.executable), "-c", script], text=True
        ).strip()
