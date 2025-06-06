"""
*** BEGIN LICENSE INFORMATION ***

The contents of this file are subject to the Common Public Attribution License
Version 1.0 (the "License"); you may not use this file except in compliance with
the License. You may obtain a copy of the License at
http://lass.sourceforge.net/cpal-license. The License is based on the
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
Copyright (C) 2004-2025 the Initial Developer.
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

import os.path
import re
import sys
import time
from typing import Callable, Dict, List, Mapping, Sequence, TypeVar, Union

_T = TypeVar("_T", str, int)

maxLevel = 3
begins = ["<", r"\{", r"\[", r"\("]
ends = [">", r"\}", r"\]", r"\)"]
now = time.asctime()


def _blockRegex(opener: str, closer: str) -> "re.Pattern[str]":
    return re.compile(
        r"([^$]|\A)\$%s(.*?)%s\$([^$]|\Z)" % (opener, closer), re.MULTILINE | re.DOTALL
    )


def _symbolRegex(symbol: str) -> "re.Pattern[str]":
    return re.compile(r"([^$]|\A)\$(\w)")


def _substituteSymbols(text: str, symbolTable: Mapping[str, Union[str, int]]) -> str:
    return _symbolRegex(r"\w").sub(
        lambda m: m.group(1) + str(symbolTable[m.group(2)]), text
    )


def _substituteIndex(text: str, i: int) -> str:
    """substitute instances of $x, $y and $g, except if it's preceeded by a second $ like $$x ...."""
    symbolTable: Dict[str, Union[str, int]] = {
        "w": i - 1,
        "x": i,
        "y": i + 1,
        "g": "Generated by param_expander.py on %s" % now,
    }
    return _substituteSymbols(text, symbolTable)


def _simple_substitute(block: str, x: Union[str, int]) -> str:
    return _substituteSymbols(block, {"x": x})


def _blockExpander(
    matchObj: "re.Match[str]",
    Xs: Sequence[_T],
    blockTransform: Callable[[str, _T], str] = _simple_substitute,
) -> str:
    before, block, after = matchObj.groups()
    newBlocks = [blockTransform(block, x) for x in Xs]
    hasNewLine = "\n" in block
    joint = (", ", "")[hasNewLine]
    return before + joint.join(newBlocks) + after


def _header(source: str) -> str:
    return """/*
 * *** ATTENTION!  DO NOT MODIFY THIS FILE DIRECTLY! ***
 * 
 * It has automatically been generated from %(source)s
 * by %(generator)s on %(now)s.
 */

""" % {
        "source": os.path.basename(source),
        "generator": os.path.basename(__file__),
        "now": now,
    }


def expandBlocksWithList(text: str, Xs: List[str]) -> str:
    """this one does only one level of expanding, but does it on items in a list Xs instead of numbers"""
    regex = _blockRegex(r"\(", r"\)")
    return regex.sub(lambda match: _blockExpander(match, Xs), text)


def expandBlocksWithRange(text: str, n: int, level: int = 0) -> str:
    """this one does a recursive expanding"""
    if level > maxLevel:
        return text
    regex = _blockRegex(begins[level], ends[level])
    Is = range(1, n + 1)

    def transform(block: str, i: int) -> str:
        return _substituteIndex(expandBlocksWithRange(block, i, level + 1), i)

    text = regex.sub(lambda match: _blockExpander(match, Is, transform), text)
    text = expandBlocksWithRange(text, n, level + 1)
    text = _substituteIndex(text, n)
    return text


def expandFileWithList(source: str, dest: str, Xs: List[str]) -> None:
    with open(source) as fp:
        text = fp.read()
    with open(dest, "w") as fp:
        fp.write(_header(source) + expandBlocksWithList(text, Xs))


"""
Expands parameters in file inPath to the size of n and save in file outPath.
If outPath contains $x then for each value i in range [1, n], an output file
with $x being replaced by i will be generated, and in each of these files the parameters
will be expanded to the size of i instead of n.
	"""


def expandFileWithRange(source: str, dest: str, n: int) -> None:
    with open(source) as fp:
        text = fp.read()
    if _symbolRegex("x").search(dest):
        for i in range(1, n + 1):
            path = _substituteSymbols(dest, {"x": i})
            with open(path, "w") as fp:
                fp.write(_header(source) + expandBlocksWithRange(text, i))
    else:
        with open(dest, "w") as fp:
            fp.write(_header(source) + expandBlocksWithRange(text, n))


def expandFile(dirname: str, inFileName: str, outFileName: str, n: int) -> None:
    expandFileWithRange(
        os.path.join(dirname, inFileName), os.path.join(dirname, outFileName), n
    )


def main(source: str, dest: str, Xs: List[str]) -> None:
    if len(Xs) == 1:
        try:
            n = int(Xs[0])
        except ValueError:
            pass
        else:
            return expandFileWithRange(source, dest, n)
    return expandFileWithList(source, dest, Xs)


if __name__ == "__main__":
    main(sys.argv[1], sys.argv[2], sys.argv[3:])

# EOF
