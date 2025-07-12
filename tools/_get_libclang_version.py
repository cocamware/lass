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

"""
This script loads the libclang shared library, retrieves its version,
and prints it to standard output. It is intended to be used in a CMake
project to determine the version of libclang being used.
"""

from __future__ import annotations

import re
import sys
from ctypes import CDLL, Structure, c_char_p, c_uint, c_void_p, cdll


def load_libclang(path: str) -> CDLL:
    """Load the libclang shared library from the given path."""
    try:
        lib = cdll.LoadLibrary(path)
    except OSError as e:
        raise RuntimeError(f"Error loading libclang from {path}: {e}") from None

    class CXString(Structure):
        _fields_ = [
            ("data", c_void_p),
            ("private_flags", c_uint),
        ]

        def __del__(self) -> None:
            lib.clang_disposeString(self)

        def __str__(self) -> str:
            pBytes = lib.clang_getCString(self)
            if pBytes is None:
                return ""
            assert isinstance(pBytes, bytes)
            return pBytes.decode("utf8")

    lib.clang_getClangVersion.restype = CXString
    lib.clang_getClangVersion.argtypes = []

    lib.clang_getCString.restype = c_char_p
    lib.clang_getCString.argtypes = [CXString]

    lib.clang_disposeString.argtypes = [CXString]

    return lib


def get_version(lib: CDLL) -> str:
    """Retrieve the version of the libclang library."""
    version_str = str(lib.clang_getClangVersion())
    if not version_str:
        raise RuntimeError("Failed to retrieve libclang version.")
    if match := re.search(r"clang version (?P<version>\d+\.\d+\.\d+)", version_str):
        return match.group("version")
    raise RuntimeError(f"Failed to parse libclang version: {version_str}")


def main() -> int:
    if len(sys.argv) != 2:
        print(f"Usage: python {sys.argv[0]} <path-to-libclang>")
        return 1

    try:
        lib = load_libclang(sys.argv[1])
        version = get_version(lib)
        sys.stdout.write(version)
    except RuntimeError as err:
        print(f"Error: {err}", file=sys.stderr)
        return 1
    return 0


if __name__ == "__main__":
    sys.exit(main())
