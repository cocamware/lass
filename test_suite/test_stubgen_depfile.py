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
# Copyright (C) 2026 the Initial Developer.
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
Test that the stub generator tracks every header it parses.

usage: python test_stubgen_depfile.py <libclang-library>

The depfiles written by the stub generator decide when ninja or make rebuild the
saved PCH and the stub data. Each header clang reads must be listed, including
headers whose own #include directives are all skipped by include guards: clang
validates those when loading the PCH as well.
"""

from __future__ import annotations

import os
import sys
import tempfile
import time
import unittest
from pathlib import Path

from lass_stubgen.parser import ParseError, Parser
from lass_stubgen.stubdata import StubData

LIBCLANG_LIBRARY: str | None = None

SOURCES = {
    "guarded.h": "#pragma once\nstruct Guarded {};\n",
    # leaf.h only includes an already included header, so it never includes anything
    "leaf.h": '#ifndef LEAF_H\n#define LEAF_H\n#include "guarded.h"\nstruct Leaf {};\n#endif\n',
    "pch.hxx": '#include "guarded.h"\n#include "leaf.h"\n',
    "main.cpp": '#include "guarded.h"\n#include "leaf.h"\nLeaf leaf;\n',
}


class TestStubgenDepfile(unittest.TestCase):
    def setUp(self) -> None:
        self._tmpdir = tempfile.TemporaryDirectory()
        self.dir = Path(self._tmpdir.name).resolve()
        for name, content in SOURCES.items():
            (self.dir / name).write_text(content)
        self.pch_path = self.dir / "pch.pch"

    def tearDown(self) -> None:
        self._tmpdir.cleanup()

    def _parser(self) -> Parser:
        return Parser(
            args=["-std=c++17"],
            pch_path=self.pch_path,
            libclang_library=LIBCLANG_LIBRARY,
        )

    def _depfile_dependencies(self, stubdata: StubData) -> str:
        depfile = self.dir / "out.d"
        stubdata.write_depfile(depfile, target=self.dir / "out")
        return depfile.read_text()

    def test_save_pch_lists_all_headers(self) -> None:
        parser = self._parser()
        parser.parse(self.dir / "pch.hxx", save_pch=True)
        dependencies = self._depfile_dependencies(parser.stubdata)
        for name in ("pch.hxx", "guarded.h", "leaf.h"):
            self.assertIn((self.dir / name).as_posix(), dependencies)

    def test_parse_lists_all_headers(self) -> None:
        parser = Parser(args=["-std=c++17"], libclang_library=LIBCLANG_LIBRARY)
        parser.parse(self.dir / "main.cpp")
        dependencies = self._depfile_dependencies(parser.stubdata)
        for name in ("main.cpp", "guarded.h", "leaf.h"):
            self.assertIn((self.dir / name).as_posix(), dependencies)

    def test_out_of_date_pch_is_reported(self) -> None:
        self._parser().parse(self.dir / "pch.hxx", save_pch=True)
        leaf = self.dir / "leaf.h"
        leaf.write_text(leaf.read_text() + "struct Grown {};\n")
        later = time.time() + 10
        os.utime(leaf, (later, later))
        with self.assertRaisesRegex(ParseError, "precompiled header"):
            self._parser().parse(self.dir / "main.cpp")


if __name__ == "__main__":
    if len(sys.argv) > 1 and not sys.argv[1].startswith("-"):
        LIBCLANG_LIBRARY = sys.argv.pop(1)
    unittest.main()
