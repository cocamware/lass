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
Copyright (C) 2018-2024 the Initial Developer.
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

import json
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
from conan.tools.microsoft import check_min_vs
from conan.tools.scm import Git

try:
    from conan.tools import CppInfo
except ImportError:
    # Fallback for Conan 1
    from conans.model.new_build_info import NewCppInfo as _CppInfo

    def CppInfo(conanfile):
        return _CppInfo()


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
        "have_avx": [True, False],
    }
    default_options = {
        "shared": True,
        "fPIC": True,
        "simd_aligned": False,
        "without_iterator_debugging": False,
        "have_avx": True,
    }
    requires = "syspython/1.0.2@cocamware/stable"

    def export_sources(self):
        git = Git(self)

        regex = re.compile(r"i/(?P<eol_index>\S+)\s+w/(?P<eol_worktree>\S+)")
        for line in git.run("ls-files --eol").splitlines():
            match = regex.match(line)
            assert match, f"Failed to parse line {line}"
            if match["eol_worktree"] != match["eol_index"]:
                raise AssertionError(
                    "Some files are checked out in the worktree with "
                    f"{match['eol_worktree'].upper()} line endings while they have "
                    f"{match['eol_index'].upper()} line endings in the index. This "
                    "would result in the wrong Conan recipe revision ID.\n"
                    "Fix this with following commands (backup local changes first!):\n"
                    "\n"
                    "  git rm --cached -r .\n"
                    "  git reset --hard\n"
                    "\n"
                    "You can verify that all files have the expected line endings with "
                    "the following command:\n"
                    "\n"
                    "  git ls-files --eol\n"
                )

        for src in git.run("ls-files").splitlines():
            dest = os.path.join(self.export_sources_folder, src)
            os.makedirs(os.path.dirname(dest), exist_ok=True)
            shutil.copy2(src, dest)

    def config_options(self):
        if self.settings.os == "Windows":
            del self.options.fPIC
        if self.settings.arch not in ["x86", "x86_64"]:
            self.options.rm_safe("have_avx")

    def configure(self):
        if self.options.shared:
            self.options.rm_safe("fPIC")

    def validate(self):
        check_min_cppstd(self, 17)
        compiler = self.settings.get_safe("compiler")
        if compiler in ["gcc", "clang"]:
            if self.settings.get_safe("compiler.libcxx") in ["libstdc++"]:
                raise ConanInvalidConfiguration(
                    "gcc and clang require C++11 compatible libcxx"
                )
        elif compiler in ["msvc", "Visual Studio"]:
            check_min_vs(self, "193")  # require VS 2019 as minimum

    def layout(self):
        cmake_layout(self)

        subdir = self.cpp.build.libdirs[0]
        build_libdir = os.path.join("lib", subdir)
        build_bindir = os.path.join("bin", subdir)
        share_dir = os.path.join("share", "Lass")

        # include dirs for local build
        self.cpp.source.includedirs = ["."]
        self.cpp.build.includedirs = ["local"]

        # binary dirs for local build
        self.cpp.build.libdirs = [build_libdir]
        self.cpp.build.bindirs = [build_bindir]

        # We generate our own LassConfig.cmake, but we have to tell conan
        # where it can be found, relative to the package or build dir ...
        self.cpp.package.builddirs = [share_dir]
        self.cpp.build.builddirs = ["."]

        # Repeat for components
        for comp in ["lass", "lass_python"]:
            self.cpp.source.components[comp].includedirs = ["."]
            self.cpp.build.components[comp].includedirs = ["local"]

            self.cpp.build.components[comp].libdirs = [build_libdir]
            self.cpp.build.components[comp].bindirs = [build_bindir]

            self.cpp.package.components[comp].builddirs = [share_dir]
            self.cpp.build.components[comp].builddirs = ["."]

    def generate(self):
        python = self.dependencies["syspython"]

        tc = CMakeToolchain(self)
        tc.cache_variables["BUILD_SIMD_ALIGNED"] = bool(self.options.simd_aligned)
        tc.cache_variables["BUILD_WITHOUT_ITERATOR_DEBUGGING"] = bool(
            self.options.without_iterator_debugging
        )
        tc.cache_variables["Lass_PYTHON_VERSION"] = python.options.python_version
        tc.cache_variables["BUILD_TESTING"] = True
        tc.cache_variables["Lass_HAVE_AVX"] = self.options.get_safe("have_avx", False)
        tc.generate()

        vs = VSCodeCCppProperties(self)
        vs.generate()

    def build(self):
        cmake = CMake(self)
        cmake.configure()
        cmake.build()
        cmake.test()

    def package(self):
        cmake = CMake(self)
        cmake.install()

    def package_info(self):
        # We need to know if we're in editable mode, as some cpp_info depends on it,
        # And we can't fix everything in layout(). However, there's no way to tell
        # if we're in editable mode, so we'll have to figure it out by simply trying
        # to read LassConfig.py from different locations: from the package dir
        # (non-editable) or build dir (editable)
        if conan_version.major == "1":
            if self.package_folder:
                lass_config = self._load_module_from_file(
                    os.path.join(self.package_folder, "share/Lass/LassConfig.py")
                )
                self.env_info.PATH.extend(
                    os.path.join(self.package_folder, bindir)
                    for bindir in self.cpp.package.components["lass"].bindirs
                )
            else:
                lass_config = self._load_module_from_file(
                    os.path.join(self.build_folder, "LassConfig.py")
                )
                self.env_info.PATH.extend(
                    os.path.join(self.build_folder, bindir)
                    for bindir in self.cpp.build.components["lass"].bindirs
                )
        else:
            try:
                lass_config = self._load_module_from_file(
                    os.path.join(self.package_folder, "share/Lass/LassConfig.py")
                )
            except FileNotFoundError:
                lass_config = self._load_module_from_file(
                    os.path.join(
                        self.package_folder, self.folders.build, "LassConfig.py"
                    )
                )

        if self.settings.build_type == "Debug":
            components = {
                "lass": lass_config.LASS_OUTPUT_NAME_DEBUG,
                "lass_python": lass_config.LASS_PYTHON_OUTPUT_NAME_DEBUG,
            }
        else:
            components = {
                "lass": lass_config.LASS_OUTPUT_NAME,
                "lass_python": lass_config.LASS_PYTHON_OUTPUT_NAME,
            }

        # Define the components that this package provides
        for comp, lib in components.items():
            self.cpp_info.components[comp].libs = [lib]
            self.cpp_info.components[comp].cxxflags = lass_config.LASS_EXTRA_CXX_FLAGS
            self.cpp_info.components[comp].set_property(
                "cmake_target_name", f"Lass::{comp}"
            )
        self.cpp_info.components["lass_python"].requires = ["lass"]

        # We generate our own LassConfig.cmake, so CMakeDeps should generate none
        self.cpp_info.set_property("cmake_find_mode", "none")
        self.cpp_info.set_property("cmake_file_name", "Lass")  # for LassConfig.cmake

        # The default CMake target that a consumer's CMakeDeps's generated config will
        # link against. This is the main lass library, as it's the most common use case.
        #
        # If Conan package pkg requires Lass and has CMakeDeps generate its
        # pkg-config.cmake (i.e. doesn't provides its own, like Lass does), by default
        # its imported target pkg::pkg will *only* link against the main lass library.
        #
        # If pkg also requires the Python bindings, it should specify that in
        # package_info of its conanfile.py:
        #
        #   def package_info(self):
        #       self.cpp_info.requires = ["lass::lass_python"]
        #
        # Note that it should be spelled as lowercase lass:: for Conan 2, as this is the
        # name of the Conan package, and not the CMake namepace Lass::
        #
        self.cpp_info.set_property("cmake_target_name", "Lass::lass")

    def _load_module_from_file(self, filename):
        file_path = Path(filename)
        module_name = file_path.stem
        spec = spec_from_file_location(module_name, file_path)
        module = module_from_spec(spec)
        spec.loader.exec_module(module)
        return module


class VSCodeCCppProperties:
    name: str
    include_path: List[str]
    defines: List[str]
    compiler_args: List[str]
    cpp_standard: str
    intellisense_mode: str
    configuration_provider: Optional[str]
    compile_commands: Optional[str]

    def __init__(self, conanfile: ConanFile, *, cmake: bool = True):
        self._conanfile = conanfile

        self.name = f"conan-{str(conanfile.settings.build_type).lower()}"

        cpp_info = CppInfo(conanfile)
        cpp_info.merge(conanfile.cpp.source)
        cpp_info.set_relative_base_folder(conanfile.source_folder)
        cpp_info.merge(conanfile.cpp.build)
        cpp_info.set_relative_base_folder(conanfile.build_folder)
        deps = conanfile.dependencies.host.topological_sort
        for dep in reversed(deps.values()):
            cpp_info.merge(dep.cpp_info.aggregated_components())

        self.include_path = cpp_info.includedirs
        self.defines = cpp_info.defines
        self.compiler_args = cpp_info.cxxflags

        cppstd = conanfile.settings.get_safe("compiler.cppstd")
        self.cpp_standard = f"c++{cppstd.replace('gnu', '')}"

        os_ = str(conanfile.settings.os).lower()
        compiler = str(conanfile.settings.compiler).lower()
        if compiler == "visual studio":  # old compiler setting in Conan 1
            compiler = "msvc"
        arch = str(conanfile.settings.arch)
        if arch == "x86":
            pass
        elif arch == "x86_64":
            arch = "x64"
        elif arch.startswith("arm64"):
            arch = "arm64"
        elif match := re.match(r"armv(\d+)", arch):
            if int(match.group(1)) < 8:
                arch = "arm"
            else:
                arch = "arm64"
        else:
            raise ConanInvalidConfiguration(f"Unsupported architecture {arch}")
        self.intellisense_mode = f"{os_}-{compiler}-{arch}"

        if cmake:
            self.configuration_provider = "ms-vscode.cmake-tools"
            self.compile_commands = (
                os.path.join(conanfile.build_folder, "compile_commands.json")
                if compiler != "msvc"
                else None
            )
        else:
            self.configuration_provider = None
            self.compile_commands = None

    def generate(self):
        vscode_folder = os.path.join(self._conanfile.source_folder, ".vscode")
        c_cpp_properties = os.path.join(vscode_folder, "c_cpp_properties.json")

        try:
            with open(c_cpp_properties) as fp:
                data = json.load(fp)
        except FileNotFoundError:
            data = {
                "configurations": [],
                "version": 4,
            }

        configuration = {
            "name": self.name,
            "includePath": self.include_path,
            "defines": self.defines,
            "compilerArgs": self.compiler_args,
            "cppStandard": self.cpp_standard,
            "intelliSenseMode": self.intellisense_mode,
        }
        if self.configuration_provider:
            configuration["configurationProvider"] = self.configuration_provider
        if self.compile_commands:
            configuration["compileCommands"] = self.compile_commands

        for index, config in enumerate(data["configurations"]):
            if config["name"] == configuration["name"]:
                data["configurations"][index] = configuration
                break
        else:
            data["configurations"].append(configuration)

        os.makedirs(vscode_folder, exist_ok=True)
        with open(c_cpp_properties, "w") as fp:
            json.dump(data, fp, indent=2)
