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
from conan.tools.microsoft import check_min_vs

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
    }
    default_options = {
        "shared": True,
        "fPIC": True,
        "simd_aligned": False,
        "without_iterator_debugging": False,
    }
    requires = "syspython/1.0.0@cocamware/stable"

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

        for src in git.included_files():
            dest = os.path.join(self.export_sources_folder, src)
            os.makedirs(os.path.dirname(dest), exist_ok=True)
            shutil.copy2(src, dest)

    def config_options(self):
        if str(self.settings.compiler) in ["msvc", "Visual Studio"]:
            del self.options.fPIC

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

        self.cpp.source.includedirs = ["."]
        self.cpp.build.includedirs = ["local"]
        self.cpp.build.builddirs = ["."]

    def generate(self):
        python = self.dependencies["syspython"]
 
        tc = CMakeToolchain(self)
        tc.variables["CMAKE_CONFIGURATION_TYPES"] = self.settings.build_type
        tc.variables["BUILD_SIMD_ALIGNED"] = bool(self.options.simd_aligned)
        tc.variables["BUILD_WITHOUT_ITERATOR_DEBUGGING"] = bool(
            self.options.without_iterator_debugging
        )
        tc.variables["Lass_PYTHON_VERSION"] = python.options.python_version
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
        # We need to know if we're in editable mode, as some cpp_info depends on it,
        # And we can't fix everything in layout(). However, there's no way to tell
        # if we're in editable mode, so we'll have to figure it out by simply trying
        # to read LassConfig.py from different locations: from the package dir
        # (non-editable) or build dir (editable)
        try:
            lass_config = self._load_module_from_file("share/Lass/LassConfig.py")
        except FileNotFoundError:
            lass_config = self._load_module_from_file("build/LassConfig.py")
            is_editable_mode = True
        else:
            is_editable_mode = False

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

        # We generate our own LassConfig.cmake, but we have to tell conan
        # were it can be found ...
        self.cpp_info.set_property("cmake_find_mode", "none")
        if is_editable_mode:
            self.cpp_info.builddirs.append("build")
        else:
            self.cpp_info.builddirs.append(os.path.join("share", "Lass"))

    def _load_module_from_file(self, filename):
        file_path = Path(self.package_folder) / filename
        module_name = file_path.stem
        spec = spec_from_file_location(module_name, file_path)
        module = module_from_spec(spec)
        spec.loader.exec_module(module)
        return module
