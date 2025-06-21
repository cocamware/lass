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
# Copyright (C) 2022-2025 the Initial Developer.
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

import datetime
import inspect
import io
import math
import os
import struct
import sys
import unittest
from collections.abc import Callable, Mapping, MutableMapping, MutableSequence, Sequence
from contextlib import redirect_stdout
from typing import TYPE_CHECKING, Any, Optional

import embedding

print(sys.version)

POINTER_SIZE = struct.calcsize("P")  # size of a pointer in bytes

if sys.version_info < (3, 9):
    SequenceFloat = Sequence
    MappingStrStr = Mapping
    MutableSequenceFloat = MutableSequence
    MutableMappingStrStr = MutableMapping
    CallableAnyAny = Callable
else:
    SequenceFloat = Sequence[float]
    MappingStrStr = Mapping[str, str]
    MutableSequenceFloat = MutableSequence[float]
    MutableMappingStrStr = MutableMapping[str, str]
    CallableAnyAny = Callable[[Any], Any]


class TestDerived(unittest.TestCase):
    def testDerived(self) -> None:
        e = embedding.Bar()
        self.assertIsInstance(e, embedding.Bar)
        self.assertEqual(e.aMoreComplexFunction(1, 2), 3)
        d = embedding.DerivedBar()
        self.assertIsInstance(d, embedding.DerivedBar)
        self.assertEqual(d.aMoreComplexFunction(1, 2), 2)


class TestPyCObject(unittest.TestCase):
    def testNone(self) -> None:
        self.assertEqual(embedding.makeNullPointer(), None)
        self.assertTrue(embedding.testNullPointer(embedding.makeNullPointer()))

    def testPyCObject(self) -> None:
        self.assertTrue(
            str(type(embedding.makeSomePointer()))
            in ("<type 'PyCObject'>", "<class 'PyCObject'>", "<class 'PyCapsule'>")
        )
        self.assertTrue(embedding.testSomePointer(embedding.makeSomePointer()))


class TestConstMap(unittest.TestCase):
    def _testConstMap(self, mapping: MappingStrStr) -> None:
        self.assertEqual(len(mapping), 1)
        self.assertIn("spam", mapping)
        self.assertNotIn("foo", mapping)
        self.assertNotIn(123, mapping)
        self.assertEqual(mapping["spam"], "spam spam spam")
        self.assertListEqual(list(mapping.keys()), ["spam"])
        self.assertListEqual(list(mapping.values()), ["spam spam spam"])
        self.assertListEqual(list(mapping.items()), [("spam", "spam spam spam")])
        with self.assertRaises(TypeError):
            mapping["foo"] = "bar"  # type: ignore[index]
        with self.assertRaises(KeyError):
            _ = mapping["foo"]
        with self.assertRaises(KeyError):
            _ = mapping[123]  # type: ignore[index]
        self.assertEqual(mapping.get("foo"), None)
        self.assertEqual(mapping.get("foo", "bar"), "bar")
        with self.assertRaises(TypeError):
            mapping.clear()  # type: ignore[attr-defined]
        copy = mapping.copy()  # type: ignore[attr-defined]
        copy["foo"] = "bar"

    def testConstMap(self) -> None:
        bar = embedding.Bar()
        self._testConstMap(bar.constMap)
        bar.testConstMap(bar.constMap)


class TestMap(unittest.TestCase):
    def _testMap(
        self, mapping: MutableMappingStrStr, refmap: MutableMappingStrStr
    ) -> None:
        mapping["test"] = "ok"
        with self.assertRaises(TypeError):
            mapping["test"] = 123  # type: ignore[assignment]
        with self.assertRaises(TypeError):
            mapping[123] = "ok"  # type: ignore[index]
        self.assertEqual(refmap["test"], "ok")
        self.assertIn("test", refmap)
        del mapping["test"]
        self.assertNotIn("test", refmap)
        with self.assertRaises(KeyError):
            del mapping["notakey"]
        with self.assertRaises(KeyError):
            del mapping[123]  # type: ignore[arg-type]
        mapping.clear()
        mapping["A"] = "a"
        self.assertEqual(repr(mapping), "{'A': 'a'}")
        mapping["B"] = "b"
        mapping["C"] = "c"
        self.assertEqual(len(refmap), 3)
        for a, b in zip(mapping, refmap.keys()):
            self.assertEqual(a, b)
        for a, b, (c, d) in zip(mapping.keys(), mapping.values(), mapping.items()):
            self.assertEqual(a, c)
            self.assertEqual(b, d)

    def testMap(self) -> None:
        bar = embedding.Bar()
        # self.assertIsInstance(bar.writeableMap, Mapping)  FIXME
        # self.assertIsInstance(bar.writeableMap, MutableMapping)  FIXME
        assert bar.writeableMap is not None
        self._testMap(bar.writeableMap, bar.writeableMap)  # type: ignore[arg-type]
        with self.assertRaises(TypeError):
            bar.writeableMap = 123  # type: ignore[assignment]
        with self.assertRaises(TypeError):
            bar.writeableMap = {1: 2, 3: 4}  # type: ignore[dict-item]
        with self.assertRaises(TypeError):
            bar.writeableMap = ["123"]  # type: ignore[assignment]
        bar.testConstMap(bar.writeableMap)

    def testVectorMap(self) -> None:
        bar = embedding.Bar()
        # self.assertIsInstance(bar.writeableVectorMap, Mapping)  FIXME
        # self.assertIsInstance(bar.writeableVectorMap, MutableMapping)  FIXME
        assert bar.writeableVectorMap is not None
        self._testMap(bar.writeableVectorMap, bar.writeableVectorMap)  # type: ignore[arg-type]


class TestConstSequence(unittest.TestCase):
    def _testConstSequence(self, seq: SequenceFloat) -> None:
        # Test that none of these operations are allowed on a const sequence
        with self.assertRaises(TypeError):
            seq[0:-1] = range(10)  # type: ignore[index]
        with self.assertRaises(TypeError):
            seq[3] = 5  # type: ignore[index]
        with self.assertRaises(TypeError):
            seq.clear()  # type: ignore[attr-defined]
        with self.assertRaises(TypeError):
            seq.reserve(5)  # type: ignore[attr-defined]
        with self.assertRaises(TypeError):
            seq.append(5)  # type: ignore[attr-defined]
        with self.assertRaises(TypeError):
            seq.pop()  # type: ignore[attr-defined]
        with self.assertRaises(TypeError):
            seq += [123]  # type: ignore[operator]
        with self.assertRaises(TypeError):
            seq *= 2  # type: ignore[assignment, operator]

    def testConstVector(self) -> None:
        self._testConstSequence(embedding.Bar().constVector)

    def testConstList(self) -> None:
        self._testConstSequence(embedding.Bar().constList)

    def testConstDeque(self) -> None:
        self._testConstSequence(embedding.Bar().constDeque)


class TestWriteableSequence(unittest.TestCase):
    def _testClear(self, seq: MutableSequenceFloat, refseq: SequenceFloat) -> None:
        seq.clear()
        self.assertEqual(len(refseq), 0)

    def _testReserve(self, seq: MutableSequenceFloat) -> None:
        with self.assertRaises(TypeError):
            seq.reserve("123")  # type: ignore[attr-defined]
        try:
            seq.reserve(5)  # type: ignore[attr-defined]
        except Exception as err:
            self.fail("seq.reserve raised exception: {!s}".format(err))

    def _testAppend(self, seq: MutableSequenceFloat, refseq: SequenceFloat) -> None:
        for i in range(10):
            seq.append(i)
        self.assertEqual(len(refseq), 10)
        for i in range(10):
            self.assertEqual(refseq[i], i)
        with self.assertRaises(TypeError):
            seq.append("123")  # type: ignore[arg-type]

    def _testRepr(self, seq: MutableSequenceFloat) -> None:
        self.assertEqual(
            repr(seq), "[0.0, 1.0, 2.0, 3.0, 4.0, 5.0, 6.0, 7.0, 8.0, 9.0]"
        )

    def _testPop(self, seq: MutableSequenceFloat, refseq: SequenceFloat) -> None:
        self.assertEqual(seq.pop(), 9)
        self.assertEqual(len(refseq), 9)
        for i in range(5):
            self.assertEqual(seq.pop(3), 3 + i)
        with self.assertRaises(IndexError):
            seq.pop(10)
        self.assertEqual(len(refseq), 4)
        for a, b in zip(refseq, [0, 1, 2, 8]):
            self.assertEqual(a, b)

    def _testContains(self, seq: MutableSequenceFloat) -> None:
        self.assertIn(8, seq)
        self.assertNotIn(-123456, seq)
        self.assertNotIn("123", seq)

    def _testGetItem(self, seq: MutableSequenceFloat, refseq: SequenceFloat) -> None:
        with self.assertRaises(IndexError):
            seq[len(seq)]  # Index beyond end of sequence
        with self.assertRaises(IndexError):
            seq[-len(seq) - 1]  # Index before start of sequence
        self.assertEqual(seq[-1], refseq[-1])  # Last element
        self.assertEqual(seq[0], refseq[0])  # First element

    def _testSetItem(self, seq: MutableSequenceFloat, refseq: SequenceFloat) -> None:
        seq[3] = 5
        self.assertEqual(refseq[3], 5)
        with self.assertRaises(TypeError):
            seq[3] = "foo"  # type: ignore[call-overload]
        seq.append(2)
        del seq[len(seq) - 1]
        seq.append(2)
        del seq[-1]

    def _testGetSlice(self, seq: MutableSequenceFloat) -> None:
        self.assertSequenceEqual(seq[:], [0, 1, 2, 5])
        self.assertSequenceEqual(seq[:], [0, 1, 2, 5])
        self.assertSequenceEqual(seq[:2], [0, 1])
        self.assertSequenceEqual(seq[2:], [2, 5])
        self.assertSequenceEqual(seq[50:], [])
        self.assertSequenceEqual(seq[-2:], [2, 5])
        self.assertSequenceEqual(seq[-100:42], [0, 1, 2, 5])
        self.assertSequenceEqual(seq[2:1], [])
        self.assertSequenceEqual(seq[1::2], [1, 5])

    def _testSetSlice(self, seq: MutableSequenceFloat, refseq: SequenceFloat) -> None:
        seq[1:3] = [10, 11, 12]
        self.assertSequenceEqual(refseq[:], [0, 10, 11, 12, 5])
        seq[:] = []
        self.assertSequenceEqual(refseq[:], [])
        seq[:] = range(10)
        self.assertSequenceEqual(refseq[:], list(range(10)))
        with self.assertRaises(ValueError):
            seq[2:9:2] = [1, 2]  # Wrong length for extended slice assignment
        seq[2:8:2] = [20, 40, 60]
        self.assertSequenceEqual(refseq[:], [0, 1, 20, 3, 40, 5, 60, 7, 8, 9])
        del seq[3:9:2]
        self.assertSequenceEqual(refseq[:], [0, 1, 20, 40, 60, 8, 9])
        with self.assertRaises(TypeError):
            seq[2:8:2] = ["20", "40", "60"]  # type: ignore[list-item]
        seq[2:5] = seq
        self.assertSequenceEqual(refseq[:], [0, 1, 0, 1, 20, 40, 60, 8, 9, 8, 9])
        del seq[:2]
        del seq[-2:]
        self.assertSequenceEqual(refseq[:], [0, 1, 20, 40, 60, 8, 9])

    def _testRepeat(self, seq: MutableSequenceFloat, refseq: SequenceFloat) -> None:
        n = len(seq)
        seq *= 2  # type: ignore[operator,assignment]
        self.assertEqual(len(refseq), 2 * n)
        b: MutableSequenceFloat = seq * 2  # type: ignore[operator,assignment]
        self.assertEqual(len(refseq), 2 * n)
        for i in range(n):
            self.assertEqual(seq[i], seq[n + i])
        self.assertEqual(len(b), 4 * n)

    def _testConcat(self, seq: MutableSequenceFloat, refseq: SequenceFloat) -> None:
        n = len(seq)
        seq += refseq
        self.assertEqual(len(refseq), 2 * n)
        b: MutableSequenceFloat = seq + refseq  # type: ignore[operator]
        self.assertEqual(len(refseq), 2 * n)
        for i in range(n):
            self.assertEqual(seq[i], seq[n + i])
        self.assertEqual(len(b), 4 * n)

    def _testSequence(self, seq: MutableSequenceFloat, refseq: SequenceFloat) -> None:
        self._testClear(seq, refseq)
        self._testReserve(seq)
        self._testAppend(seq, refseq)
        self._testRepr(seq)
        self._testPop(seq, refseq)
        self._testContains(seq)
        self._testGetItem(seq, refseq)
        self._testSetItem(seq, refseq)
        self._testGetSlice(seq)
        self._testSetSlice(seq, refseq)
        self._testRepeat(seq, refseq)
        self._testConcat(seq, refseq)

    def testWriteableVector(self) -> None:
        bar = embedding.Bar()
        # self.assertIsInstance(bar.writeableVector, Sequence)  FIXME
        # self.assertIsInstance(bar.writeableVector, MutableSequence)  FIXME
        assert bar.writeableVector is not None
        self._testSequence(bar.writeableVector, bar.writeableVector)  # type: ignore[arg-type]
        with self.assertRaises(TypeError):
            bar.writeableVector = 123  # type: ignore[assignment]
        with self.assertRaises(TypeError):
            bar.writeableVector = {1: 2, 3: 4}  # type: ignore[assignment]
        with self.assertRaises(TypeError):
            bar.writeableVector = ["123"]  # type: ignore[list-item]
        bar.writeableVector = range(3)
        # self.assertEqual(bar.writeableVector, [0, 1, 2])  FIXME
        self.assertListEqual(list(bar.writeableVector), [0, 1, 2])

    def testWriteableList(self) -> None:
        bar = embedding.Bar()
        assert bar.writeableList is not None
        self._testSequence(bar.writeableList, bar.writeableList)  # type: ignore[arg-type]

    def testWriteableDeque(self) -> None:
        bar = embedding.Bar()
        assert bar.writeableDeque is not None
        self._testSequence(bar.writeableDeque, bar.writeableDeque)  # type: ignore[arg-type]

    def testWriteableStaticVector(self) -> None:
        bar = embedding.Bar()
        assert bar.writeableStaticVector is not None
        self._testSequence(bar.writeableStaticVector, bar.writeableStaticVector)  # type: ignore[arg-type]


class TestDocstrings(unittest.TestCase):
    def testDocstrings(self) -> None:
        self.assertEqual(embedding.__doc__, "Documentation for module embedding")
        self.assertEqual(embedding.PythonFoo.__doc__, "Documentation for class Foo.")
        self.assertEqual(embedding.Bar.tester.__doc__, "tester doc")
        self.assertEqual(embedding.Bar.InnerClass.__doc__, "InnerClass of Bar")
        self.assertEqual(embedding.ClassB.properImagine.__doc__, "blabla")


class TestMethods(unittest.TestCase):
    def testMethods(self) -> None:
        bar = embedding.Bar()
        self.assertEqual(bar.aMoreComplexFunction(3.0, 5.0), 8.0)
        self.assertEqual(
            bar.testAutomaticFunctionExport(5, 6.0),
            "C++ testAutomaticFunctionExport : 11",
        )
        self.assertEqual(
            bar.complexArguments("a string"), "complexArguments: 'a string'"
        )
        box = ((10, 20, 30), (100, 200, 300))
        matrix = ((1, 2, 3, 4), (5, 6, 7, 8), (9, 10, 11, 12), (13, 14, 15, 16))
        answer = """size of box along y axis is 180
transformation: ((1, 2, 3, 4), (5, 6, 7, 8), (9, 10, 11, 12), (13, 14, 15, 16))
"""
        self.assertEqual(bar.primArguments(box, "y", matrix), answer)

        # matrix2 is not exactly the right type, but it should work
        box2 = [[10, 20, 30], [100, 200, 300]]
        matrix2 = [[1, 2, 3, 4], [5, 6, 7, 8], [9, 10, 11, 12], [13, 14, 15, 16]]
        self.assertEqual(bar.primArguments(box2, "y", matrix2), answer)  # type: ignore[arg-type]

    def testFreeMethods(self) -> None:
        bar = embedding.Bar()
        # bar has a virtual function table, whose vptr sits before PyObject base
        address = id(bar) - POINTER_SIZE
        num_hexdigits = 2 * POINTER_SIZE
        if os.name == "nt":
            str_address = f"{'0' * num_hexdigits}{address:X}"[-num_hexdigits:]
        else:
            str_address = f"0x{address:x}"
        self.assertEqual(
            bar.freeMethodA("been here"),
            f"freeMethodA(const Bar&, const std::string&): {str_address} been here",
        )
        self.assertEqual(
            bar.freeMethodB("done that"),
            f"freeMethodB(Bar*, const std::string&): {str_address} done that",
        )


class TestClassName(unittest.TestCase):
    def testClassName(self) -> None:
        bar = embedding.Bar()
        self.assertEqual(bar.__class__, embedding.Bar)
        self.assertEqual(embedding.Bar.__class__, type)
        self.assertEqual(embedding.Bar.__name__, "Bar")
        self.assertEqual(embedding.Bar.__module__, "embedding")
        self.assertEqual(embedding.Bar.__qualname__, "Bar")

    def testInnerClassName(self) -> None:
        innerClass = embedding.Bar.InnerClass("the answer is 42")
        self.assertEqual(innerClass.__class__, embedding.Bar.InnerClass)
        self.assertEqual(embedding.Bar.InnerClass.__class__, type)
        self.assertEqual(embedding.Bar.InnerClass.__name__, "InnerClass")
        # self.assertEqual(embedding.Bar.InnerClass.__module__, "embedding")  FIXME
        # self.assertEqual(embedding.Bar.InnerClass.__qualname__, "Bar.InnerClass")  FIXME

    def testEnumClassName(self) -> None:
        color = embedding.Color.BLUE
        self.assertEqual(color.__class__, embedding.Color)
        self.assertEqual(embedding.Color.__name__, "Color")
        self.assertEqual(embedding.Color.__module__, "embedding")
        self.assertEqual(embedding.Color.__qualname__, "Color")

    def testClassEnumName(self) -> None:
        shape = embedding.Bar.Shape.CIRCLE
        self.assertEqual(shape.__class__, embedding.Bar.Shape)
        self.assertEqual(embedding.Bar.Shape.__name__, "Shape")
        # self.assertEqual(embedding.Bar.Shape.__module__, "embedding")  FIXME
        # self.assertEqual(embedding.Bar.Shape.__qualname__, "Bar.Shape")  FIXME


class TestTypeSafety(unittest.TestCase):
    def testTypeSafety(self) -> None:
        embedding.Bar.aStaticMethod(3.14)
        with self.assertRaises(TypeError):
            embedding.Bar.aStaticMethod("crash here")  # type: ignore[arg-type]

        bar = embedding.Bar()
        bar.myInt = 123
        with self.assertRaises(TypeError):
            bar.myInt = "bogus"  # type: ignore[assignment]


class TestProperties(unittest.TestCase):
    def testGetSetter(self) -> None:
        bar = embedding.Bar()
        self.assertEqual(bar.myInt, 0)
        bar.myInt = 5
        self.assertEqual(bar.myInt, 5)
        with self.assertRaises(TypeError):
            bar.myInt = "foo"  # type: ignore[assignment]
        with self.assertRaises(TypeError):
            bar.myInt = 3.14  # type: ignore[assignment]

    def testObjectProperty(self) -> None:
        bar = embedding.Bar()
        self.assertIs(bar.foo, None)
        foo = embedding.PythonFoo(6, "world!")
        bar.foo = foo
        self.assertIs(bar.foo, foo)
        bar.foo = None
        self.assertIs(bar.foo, None)

    def testSetterByRef(self) -> None:
        bar = embedding.Bar()
        self.assertEqual(bar.cool, 1)
        bar.cool = 2.5
        self.assertEqual(bar.cool, 2.5)
        with self.assertRaises(TypeError):
            bar.cool = "foo"  # type: ignore[assignment]

    def testPublicProperty(self) -> None:
        bar = embedding.Bar()
        self.assertEqual(bar.publicInt, 42)
        bar.publicInt = 60
        self.assertEqual(bar.publicInt, 60)
        with self.assertRaises(TypeError):
            bar.publicInt = "foo"  # type: ignore[assignment]
        with self.assertRaises(TypeError):
            bar.publicInt = 3.14  # type: ignore[assignment]

    def testFreeProperty(self) -> None:
        b = embedding.ClassB()
        self.assertEqual(b.properImagine, 1)
        b.properImagine = 2
        self.assertEqual(b.properImagine, 2)
        with self.assertRaises(TypeError):
            b.properImagine = "foo"  # type: ignore[assignment]
        with self.assertRaises(TypeError):
            b.properImagine = 3.14  # type: ignore[assignment]

    def testFreePropertyConst(self) -> None:
        b = embedding.ClassB()
        self.assertEqual(b.properImagineRO, 1)
        with self.assertRaises(AttributeError):
            b.properImagineRO = 2  # type: ignore[misc]
        with self.assertRaises(AttributeError):
            b.properImagineRO = "foo"  # type: ignore[assignment,misc]
        with self.assertRaises(AttributeError):
            b.properImagineRO = 3.14  # type: ignore[assignment,misc]


class TestSignatures(unittest.TestCase):
    def testCString(self) -> None:
        self.assertEqual(embedding.testCStringSupport("Hello World!"), "Hello World!")


class TestOperators(unittest.TestCase):
    def testCallOperator(self) -> None:
        bar = embedding.Bar()
        self.assertEqual(bar(4), 8)
        self.assertEqual(bar("hallo"), "HALLO")
        self.assertEqual(bar(3.5), 1.75)

    def testStrOperator(self) -> None:
        bar = embedding.Bar()
        self.assertEqual(str(bar), "A free __str__ representation of Bar")
        self.assertEqual(repr(bar), "A free __repr__ representation of Bar")

    def testNumericOperators(self) -> None:
        a = embedding.Cyclic(0, 3)
        self.assertEqual(a.value, 0)
        self.assertEqual(a.period, 3)
        b = a + 1
        self.assertEqual(b.value, 1)
        self.assertEqual(b.period, 3)
        c = a + 5
        self.assertEqual(c.value, 2)
        self.assertEqual(c.period, 3)
        d = 2 + a
        self.assertEqual(d.value, 2)
        self.assertEqual(d.period, 3)
        e = c**2
        self.assertEqual(e.value, 1)
        self.assertEqual(e.period, 3)
        f = c**3
        self.assertEqual(f.value, 2)
        self.assertEqual(f.period, 3)
        g = pow(c, 3, 10)
        self.assertEqual(g.value, 8)
        self.assertEqual(g.period, 10)


def expectedFailureIf(condition: bool) -> CallableAnyAny:
    """Decorator to mark a test as expected failure if the condition is True."""
    if condition:
        return unittest.expectedFailure

    def decorator(test_item: unittest.TestCase) -> unittest.TestCase:
        return test_item

    return decorator


class TestStaticMembers(unittest.TestCase):
    def testClassConstants(self) -> None:
        self.assertEqual(embedding.Bar.CONST, 5)
        with self.assertRaises(TypeError):
            embedding.Bar.CONST = 6  # type: ignore[misc]

    def testInnerClasses(self) -> None:
        innerClass = embedding.Bar.InnerClass("the answer is 42")
        self.assertEqual(innerClass.talkTo("Arthur"), "Arthur, the answer is 42.\n")
        self.assertEqual(inspect.getdoc(embedding.Bar.InnerClass), "InnerClass of Bar")

    def testStaticMethods(self) -> None:
        self.assertEqual(embedding.Bar.aStaticMethod(3.14), 3)
        bar = embedding.Bar()
        self.assertEqual(bar.aStaticMethod(5.1), 5)


class TestOverloading(unittest.TestCase):
    def testOverloadedFunctions(self) -> None:
        self.assertEqual(embedding.overloaded(5), 10)
        self.assertEqual(embedding.overloaded("hello!"), "HELLO!")
        self.assertEqual(embedding.overloaded(0.3 + 0.4j), 0.5)

    def testOverloadedMethods(self) -> None:
        bar = embedding.Bar()
        self.assertEqual(bar.tester("a string"), "complexArguments: 'a string'")
        box = ((10, 20, 30), (100, 200, 300))
        matrix = ((1, 2, 3, 4), (5, 6, 7, 8), (9, 10, 11, 12), (13, 14, 15, 16))
        answer = """size of box along y axis is 180
transformation: ((1, 2, 3, 4), (5, 6, 7, 8), (9, 10, 11, 12), (13, 14, 15, 16))
"""
        self.assertEqual(bar.tester(box, "y", matrix), answer)
        matrix2 = ((1, 2, 3, 4), [5, 6, 7, 8], (9, 10, 11, 12), (13, 14, 15, 16))
        self.assertEqual(bar.tester(box, "y", matrix2), answer)  # type: ignore[call-overload]


class TestSpecialFunctionsAndOperators(unittest.TestCase):
    def testSequenceProtocol(self) -> None:
        c = embedding.ClassB()
        self.assertEqual(len(c), 5)
        self.assertEqual(c[1], 1)


class TestTuples(unittest.TestCase):
    def assertSequenceAlmostEqual(
        self,
        first: SequenceFloat,
        second: SequenceFloat,
        places: int = 7,
        msg: Optional[str] = None,
    ) -> None:
        msg = msg or "%r != %r within %r places" % (first, second, places)
        self.assertEqual(len(first), len(second), msg)
        for a, b in zip(first, second):
            self.assertAlmostEqual(a, b, places=places, msg=msg)

    def testVariableLength(self) -> None:
        bar = embedding.Bar()
        self.assertSequenceAlmostEqual(bar.rgba((0.1, 0.2, 0.3)), (0.1, 0.2, 0.3, 1.0))  # type: ignore[arg-type]
        self.assertSequenceAlmostEqual(
            bar.rgba([0.1, 0.2, 0.3, 0.4]),  # type: ignore[arg-type]
            (0.1, 0.2, 0.3, 0.4),
        )
        with self.assertRaises(TypeError):
            bar.rgba([])  # type: ignore[arg-type]
        with self.assertRaises(TypeError):
            bar.rgba((0.1, 0.2))  # type: ignore[arg-type]
        with self.assertRaises(TypeError):
            bar.rgba((0.1, 0.2, 0.3, 0.4, 0.5))  # type: ignore[arg-type]


class TestConstructors(unittest.TestCase):
    def testOverloadedConstructors(self) -> None:
        barA = embedding.Bar()
        self.assertEqual(barA.myInt, 0)

        barB = embedding.Bar(5, "hello")
        self.assertEqual(barB.myInt, 5)

    def testFreeConstructors(self) -> None:
        classB = embedding.ClassB(5)
        self.assertEqual(classB.properImagine, 5)


class TestShadowHierarchy(unittest.TestCase):
    def testBacon(self) -> None:
        bacon = embedding.Bacon()
        self.assertEqual(bacon.virtualWho(), "Bacon")
        self.assertEqual(bacon.overridenWho(), "Bacon")
        self.assertEqual(bacon.__class__, embedding.Bacon)
        self.assertEqual(bacon.__class__.__bases__, (embedding.Ham,))
        sup: embedding.Ham = super(bacon.__class__, bacon)  # type: ignore[assignment]
        self.assertEqual(sup.virtualWho(), "Bacon")
        self.assertEqual(sup.overridenWho(), "Ham")

    def testHam(self) -> None:
        ham = embedding.Ham()
        self.assertEqual(ham.virtualWho(), "Ham")
        self.assertEqual(ham.overridenWho(), "Ham")

    def testEggs(self) -> None:
        eggs = embedding.Eggs(3)
        self.assertEqual(eggs.virtualWho(), "3 Eggs")
        self.assertEqual(eggs.overridenWho(), "3 Eggs")
        self.assertEqual(eggs.number, 3)
        eggs.number = 4
        self.assertEqual(eggs.number, 4)

    def testSpam(self) -> None:
        spam = embedding.makeSpam("Bacon")
        self.assertIsNotNone(spam)
        assert spam is not None
        self.assertEqual(spam.virtualWho(), "Bacon")
        self.assertEqual(spam.overridenWho(), "Bacon")
        self.assertFalse(embedding.spamToCppByCopy(spam, spam))
        self.assertTrue(embedding.spamToCppByConstReference(spam, spam))
        self.assertTrue(embedding.spamToCppByReference(spam, spam))


class TestCallbacks(unittest.TestCase):
    def setUp(self) -> None:
        self.isCalled = False
        self.arg: Optional[str] = None

    def testCallback0(self) -> None:
        def callback() -> None:
            self.isCalled = True

        embedding.call0(callback)
        self.assertTrue(self.isCalled)

    def testCallback1(self) -> None:
        hi = "hi!"

        def callback(x: str) -> None:
            self.isCalled = True
            self.arg = x

        embedding.call1(callback, hi)
        self.assertTrue(self.isCalled)
        self.assertEqual(self.arg, hi)

    def testCallbackR0(self) -> None:
        hi = "hi!"

        def callback() -> str:
            self.isCalled = True
            return hi

        result = embedding.callR0(callback)
        self.assertTrue(self.isCalled)
        self.assertEqual(result, hi)

    def testCallbackR2(self) -> None:
        a, b = 5, 6
        c = embedding.callR2(lambda x, y: x * y, a, b)
        self.assertEqual(c, a * b)

    def testCallbackException(self) -> None:
        def callback() -> None:
            self.isCalled = True
            d = {"a": 1}
            assert d["b"]  # raises a KeyError

        with self.assertRaises(KeyError):
            embedding.call0(callback)
        self.assertTrue(self.isCalled)


class TestRichCompare(unittest.TestCase):
    def setUp(self) -> None:
        self.a = embedding.Bar(5, "A")
        self.b = embedding.Bar(5, "B")
        self.c = embedding.Bar(6, "C")

    def testEqual(self) -> None:
        a, b, c = self.a, self.b, self.c
        self.assertTrue(a == b)
        self.assertFalse(a == c)
        self.assertFalse(a == None)  # noqa: E711, none-comparison

    def testLess(self) -> None:
        a, b, c = self.a, self.b, self.c
        self.assertTrue(a < c)
        self.assertFalse(a < b)
        self.assertFalse(c < a)
        self.assertFalse(a < None)

    def testNotEqual(self) -> None:
        a, b, c = self.a, self.b, self.c
        self.assertFalse(a != b)
        self.assertTrue(a != c)
        self.assertTrue(a != None)  # noqa: E711, none-comparison

    def testGreater(self) -> None:
        a, b, c = self.a, self.b, self.c
        self.assertTrue(c > a)
        self.assertFalse(b > a)
        self.assertFalse(a > c)
        self.assertFalse(a > None)

    def testLessEqual(self) -> None:
        a, b, c = self.a, self.b, self.c
        self.assertTrue(a <= b)
        self.assertTrue(a <= c)
        self.assertFalse(c <= a)
        self.assertFalse(a <= None)

    def testGreaterEqual(self) -> None:
        a, b, c = self.a, self.b, self.c
        self.assertTrue(c >= a)
        self.assertTrue(a >= b)
        self.assertFalse(a >= c)
        self.assertFalse(a >= None)


dummyCounter: int = 0


def dummyCallback0() -> None:
    global dummyCounter
    dummyCounter += 1


def dummyCallback1(adder: int) -> None:
    global dummyCounter
    dummyCounter += adder


class TestMultiCallback(unittest.TestCase):
    def testConstruction(self) -> None:
        global dummyCounter
        a = embedding.TestCallback()
        # can we grab the object
        cb = a.callback
        # can we add a function to the multi-dispatching callback
        cb.add(dummyCallback0)  # type: ignore[attr-defined]
        # calling a wrapped callback
        cb.call()  # type: ignore[attr-defined]
        self.assertEqual(dummyCounter, 1)
        cb.add(dummyCallback0)  # type: ignore[attr-defined]
        cb.call()  # type: ignore[attr-defined]
        self.assertEqual(dummyCounter, 3)
        cb.reset()  # type: ignore[attr-defined]
        cb()
        self.assertEqual(dummyCounter, 3)
        for i in range(10):
            cb.add(dummyCallback0)  # type: ignore[attr-defined]
        cb()
        self.assertEqual(dummyCounter, 13)
        # checking ref counting
        cb()
        self.assertEqual(dummyCounter, 23)
        # can we pass it as argument and call it in c++
        a.acceptAsArgument(cb)
        self.assertEqual(dummyCounter, 33)
        # can we plainly assign a function without using add, mimicking
        # the behavior of a plain callback
        a.callback = dummyCallback0
        # dropping the call() function a directly calling the () operator
        a.callback()
        self.assertEqual(dummyCounter, 34)


class TestStream(unittest.TestCase):
    def testSysStdout(self) -> None:
        with redirect_stdout(io.StringIO()) as buffer:
            embedding.writeStdout("Hello\n")
            embedding.writeStdout("World\n")
        self.assertEqual(buffer.getvalue(), "Hello\nWorld\n")


class MyFloat:
    def __init__(self, value: Any):
        self.value = value

    def __float__(self) -> float:
        return float(self.value)


LASS_HAVE_STD_U8STRING: bool = getattr(embedding, "LASS_HAVE_STD_U8STRING", False)

skipIfNoU8string = unittest.skipIf(
    not LASS_HAVE_STD_U8STRING, "std::u8string not supported"
)

if TYPE_CHECKING:
    # Trick the type checker that we have testStdU8string
    testStdU8string = embedding.testStdString
    testStdU8stringView = embedding.testStdStringView
    testConstChar8Ptr = embedding.testConstCharPtr
elif LASS_HAVE_STD_U8STRING:
    testStdU8string = embedding.testStdU8string
    testStdU8stringView = embedding.testStdU8stringView
    testConstChar8Ptr = embedding.testConstChar8Ptr


class TestTypes(unittest.TestCase):
    def testStdString(self) -> None:
        self.assertEqual(embedding.testStdString(""), "")
        self.assertEqual(embedding.testStdString("abc"), "abc")
        self.assertEqual(embedding.testStdString("𐌷𐌴𐌻𐌻𐍉"), "𐌷𐌴𐌻𐌻𐍉")
        self.assertEqual(embedding.testStdString("\0"), "\0")
        self.assertEqual(embedding.testStdString("a\0b"), "a\0b")

    def testStdWstring(self) -> None:
        self.assertEqual(embedding.testStdWstring(""), "")
        self.assertEqual(embedding.testStdWstring("abc"), "abc")
        self.assertEqual(embedding.testStdWstring("𐌷𐌴𐌻𐌻𐍉"), "𐌷𐌴𐌻𐌻𐍉")
        self.assertEqual(embedding.testStdWstring("\0"), "\0")
        self.assertEqual(embedding.testStdWstring("a\0b"), "a\0b")

    @skipIfNoU8string
    def testStdU8string(self) -> None:
        self.assertEqual(testStdU8string(""), "")
        self.assertEqual(testStdU8string("abc"), "abc")
        self.assertEqual(testStdU8string("𐌷𐌴𐌻𐌻𐍉"), "𐌷𐌴𐌻𐌻𐍉")
        self.assertEqual(testStdU8string("\0"), "\0")
        self.assertEqual(testStdU8string("a\0b"), "a\0b")

    def testStdU16string(self) -> None:
        self.assertEqual(embedding.testStdU16string(""), "")
        self.assertEqual(embedding.testStdU16string("abc"), "abc")
        self.assertEqual(embedding.testStdU16string("𐌷𐌴𐌻𐌻𐍉"), "𐌷𐌴𐌻𐌻𐍉")
        self.assertEqual(embedding.testStdU16string("\0"), "\0")
        self.assertEqual(embedding.testStdU16string("a\0b"), "a\0b")

    def testStdU32string(self) -> None:
        self.assertEqual(embedding.testStdU32string(""), "")
        self.assertEqual(embedding.testStdU32string("abc"), "abc")
        self.assertEqual(embedding.testStdU32string("𐌷𐌴𐌻𐌻𐍉"), "𐌷𐌴𐌻𐌻𐍉")
        self.assertEqual(embedding.testStdU32string("\0"), "\0")
        self.assertEqual(embedding.testStdU32string("a\0b"), "a\0b")

    def testStdStringView(self) -> None:
        self.assertEqual(embedding.testStdStringView(""), "")
        self.assertEqual(embedding.testStdStringView("abc"), "abc")
        self.assertEqual(embedding.testStdStringView("𐌷𐌴𐌻𐌻𐍉"), "𐌷𐌴𐌻𐌻𐍉")
        self.assertEqual(embedding.testStdStringView("\0"), "\0")
        self.assertEqual(embedding.testStdStringView("a\0b"), "a\0b")

    def testStdWstringView(self) -> None:
        self.assertEqual(embedding.testStdWstringView(""), "")
        self.assertEqual(embedding.testStdWstringView("abc"), "abc")
        self.assertEqual(embedding.testStdWstringView("𐌷𐌴𐌻𐌻𐍉"), "𐌷𐌴𐌻𐌻𐍉")
        self.assertEqual(embedding.testStdWstringView("\0"), "\0")
        self.assertEqual(embedding.testStdWstringView("a\0b"), "a\0b")

    @skipIfNoU8string
    def testStdU8stringView(self) -> None:
        self.assertEqual(testStdU8stringView(""), "")
        self.assertEqual(testStdU8stringView("abc"), "abc")
        self.assertEqual(testStdU8stringView("𐌷𐌴𐌻𐌻𐍉"), "𐌷𐌴𐌻𐌻𐍉")
        self.assertEqual(testStdU8stringView("\0"), "\0")
        self.assertEqual(testStdU8stringView("a\0b"), "a\0b")

    def testStdU16stringView(self) -> None:
        self.assertEqual(embedding.testStdU16stringView(""), "")
        self.assertEqual(embedding.testStdU16stringView("abc"), "abc")
        self.assertEqual(embedding.testStdU16stringView("𐌷𐌴𐌻𐌻𐍉"), "𐌷𐌴𐌻𐌻𐍉")
        self.assertEqual(embedding.testStdU16stringView("\0"), "\0")
        self.assertEqual(embedding.testStdU16stringView("a\0b"), "a\0b")

    def testStdU32stringView(self) -> None:
        self.assertEqual(embedding.testStdU32stringView(""), "")
        self.assertEqual(embedding.testStdU32stringView("abc"), "abc")
        self.assertEqual(embedding.testStdU32stringView("𐌷𐌴𐌻𐌻𐍉"), "𐌷𐌴𐌻𐌻𐍉")
        self.assertEqual(embedding.testStdU32stringView("\0"), "\0")
        self.assertEqual(embedding.testStdU32stringView("a\0b"), "a\0b")

    def testConstCharPtr(self) -> None:
        self.assertEqual(embedding.testConstCharPtr(""), "")
        self.assertEqual(embedding.testConstCharPtr("abc"), "abc")
        self.assertEqual(embedding.testConstCharPtr("𐌷𐌴𐌻𐌻𐍉"), "𐌷𐌴𐌻𐌻𐍉")

    def testConstWcharPtr(self) -> None:
        self.assertEqual(embedding.testConstWcharPtr(""), "")
        self.assertEqual(embedding.testConstWcharPtr("abc"), "abc")
        self.assertEqual(embedding.testConstWcharPtr("𐌷𐌴𐌻𐌻𐍉"), "𐌷𐌴𐌻𐌻𐍉")

    @skipIfNoU8string
    def testConstChar8Ptr(self) -> None:
        self.assertEqual(testConstChar8Ptr(""), "")
        self.assertEqual(testConstChar8Ptr("abc"), "abc")
        self.assertEqual(testConstChar8Ptr("𐌷𐌴𐌻𐌻𐍉"), "𐌷𐌴𐌻𐌻𐍉")

    def testConstChar16Ptr(self) -> None:
        self.assertEqual(embedding.testConstChar16Ptr(""), "")
        self.assertEqual(embedding.testConstChar16Ptr("abc"), "abc")
        self.assertEqual(embedding.testConstChar16Ptr("𐌷𐌴𐌻𐌻𐍉"), "𐌷𐌴𐌻𐌻𐍉")

    def testConstChar32Ptr(self) -> None:
        self.assertEqual(embedding.testConstChar32Ptr(""), "")
        self.assertEqual(embedding.testConstChar32Ptr("abc"), "abc")
        self.assertEqual(embedding.testConstChar32Ptr("𐌷𐌴𐌻𐌻𐍉"), "𐌷𐌴𐌻𐌻𐍉")

    def testFloatSingle(self) -> None:
        useOldExportTraits = getattr(embedding, "LASS_USE_OLD_EXPORTRAITS_FLOAT", False)
        # float to single conversion is not always exact
        inf = float("inf")
        self.assertEqual(embedding.testFloatSingle(0.0), 0.0)
        self.assertEqual(embedding.testFloatSingle(1.0), 1.0)
        self.assertEqual(embedding.testFloatSingle(-1.0), -1.0)
        self.assertEqual(embedding.testFloatSingle(1.5), 1.5)
        self.assertEqual(embedding.testFloatSingle(1e-10), 1.000000013351432e-10)
        self.assertEqual(embedding.testFloatSingle(1e10), 1e10)
        self.assertEqual(embedding.testFloatSingle(1.0000000001), 1.0)
        if useOldExportTraits:
            with self.assertRaises(TypeError):
                embedding.testFloatSingle(inf)
            with self.assertRaises(TypeError):
                embedding.testFloatSingle(-inf)
            with self.assertRaises(TypeError):
                embedding.testFloatSingle(1e40)
            with self.assertRaises(TypeError):
                embedding.testFloatSingle(1e400)
        else:
            self.assertEqual(embedding.testFloatSingle(inf), inf)
            self.assertEqual(embedding.testFloatSingle(-inf), -inf)
            self.assertEqual(embedding.testFloatSingle(1e40), inf)
            self.assertEqual(embedding.testFloatSingle(1e400), inf)
        self.assertEqual(embedding.testFloatSingle(1e-50), 0.0)
        self.assertEqual(embedding.testFloatSingle(1e-500), 0.0)
        self.assertTrue(math.isnan(embedding.testFloatSingle(float("nan"))))

        # integer to single conversion isn't always exact, but it follows the same rules as float(x)
        self.assertEqual(float(0), 0.0)
        self.assertEqual(embedding.testFloatSingle(0), 0.0)
        self.assertEqual(float(1234567890), 1234567890.0)
        self.assertEqual(embedding.testFloatSingle(1234567890), 1234567936.0)  # !!!
        self.assertEqual(float(12345678901234567890), 1.2345678901234567e19)
        self.assertEqual(
            embedding.testFloatSingle(12345678901234567890), 1.2345679395506094e19
        )  # !!!
        with self.assertRaises(OverflowError):
            float(2**1024)
        if useOldExportTraits:
            with self.assertRaises(TypeError):
                embedding.testFloatSingle(2**1024)  # so not inf...
        else:
            with self.assertRaises(OverflowError):
                embedding.testFloatSingle(2**1024)  # so not inf...
        if useOldExportTraits:
            with self.assertRaises(TypeError):
                embedding.testFloatSingle(MyFloat(1.5))  # type: ignore[arg-type]
        else:
            # Custom objects should be converted to float first
            self.assertEqual(embedding.testFloatSingle(MyFloat(1.5)), 1.5)  # type: ignore[arg-type]
            self.assertEqual(embedding.testFloatSingle(MyFloat("inf")), inf)  # type: ignore[arg-type]
            self.assertEqual(embedding.testFloatSingle(MyFloat(1e40)), inf)  # type: ignore[arg-type]
            self.assertTrue(math.isnan(embedding.testFloatSingle(MyFloat("nan"))))  # type: ignore[arg-type]
            with self.assertRaises(OverflowError):
                float(MyFloat(2**1024))

    def testFloatDouble(self) -> None:
        useOldExportTraits = getattr(embedding, "LASS_USE_OLD_EXPORTRAITS_FLOAT", False)
        # float to double conversion should be exact
        inf = float("inf")
        self.assertEqual(embedding.testFloatDouble(0.0), 0.0)
        self.assertEqual(embedding.testFloatDouble(1.0), 1.0)
        self.assertEqual(embedding.testFloatDouble(-1.0), -1.0)
        self.assertEqual(embedding.testFloatDouble(1.5), 1.5)
        self.assertEqual(embedding.testFloatDouble(1e-10), 1e-10)
        self.assertEqual(embedding.testFloatDouble(1e10), 1e10)
        self.assertEqual(embedding.testFloatDouble(1.0000000001), 1.0000000001)
        self.assertEqual(embedding.testFloatDouble(inf), inf)
        self.assertEqual(embedding.testFloatDouble(-inf), -inf)
        self.assertEqual(embedding.testFloatDouble(1e40), 1e40)
        self.assertEqual(embedding.testFloatDouble(1e400), inf)
        self.assertEqual(embedding.testFloatDouble(1e-50), 1e-50)
        self.assertEqual(embedding.testFloatDouble(1e-500), 0)
        self.assertTrue(math.isnan(embedding.testFloatDouble(float("nan"))))

        # integer to double conversion isn't always exact, but it follows the same rules as float(x)
        self.assertEqual(float(0), 0.0)
        self.assertEqual(embedding.testFloatDouble(0), 0.0)
        self.assertEqual(float(1234567890), 1234567890.0)
        self.assertEqual(embedding.testFloatDouble(1234567890), 1234567890.0)
        self.assertEqual(float(12345678901234567890), 1.2345678901234567e19)
        self.assertEqual(
            embedding.testFloatDouble(12345678901234567890), 1.2345678901234567e19
        )
        self.assertRaises(OverflowError, float, 2**1024)
        if useOldExportTraits:
            self.assertRaises(
                TypeError, embedding.testFloatDouble, 2**1024
            )  # so not inf...
        else:
            self.assertRaises(
                OverflowError, embedding.testFloatDouble, 2**1024
            )  # so not inf...

        if useOldExportTraits:
            with self.assertRaises(TypeError):
                embedding.testFloatDouble(MyFloat(1.5))  # type: ignore[arg-type]
        else:
            # Custom objects should be converted to float first
            self.assertEqual(embedding.testFloatDouble(MyFloat(1.5)), 1.5)  # type: ignore[arg-type]
            self.assertEqual(embedding.testFloatDouble(MyFloat("inf")), inf)  # type: ignore[arg-type]
            self.assertEqual(embedding.testFloatDouble(MyFloat(1e400)), inf)  # type: ignore[arg-type]
            self.assertTrue(math.isnan(embedding.testFloatDouble(MyFloat("nan"))))  # type: ignore[arg-type]
            with self.assertRaises(OverflowError):
                float(MyFloat(2**1024))

    def testStdTuple(self) -> None:
        x = (True, "Hello")
        y = embedding.testTuple(x)
        self.assertIsInstance(y, tuple)
        self.assertEqual(len(y), 2)
        self.assertIsInstance(y[0], bool)
        self.assertIsInstance(y[1], str)
        self.assertEqual(y, x)

    def testStdVariant(self) -> None:
        self.assertEqual(embedding.testVariant(42), 42)
        self.assertEqual(embedding.testVariant("Hello"), "Hello")
        with self.assertRaises(TypeError):
            embedding.testVariant(3.14)  # type: ignore[arg-type]


class TestBar(unittest.TestCase):
    def testMapProtocol(self) -> None:
        bar = embedding.Bar()
        self.assertEqual(len(bar), 0)
        with self.assertRaises(KeyError):
            _ = bar["foo"]
        with self.assertRaises(TypeError):
            _ = bar[1]  # type: ignore[index]  # should become KeyError?
        bar["foo"] = "spam"
        bar["baz"] = 42
        self.assertEqual(len(bar), 2)
        self.assertEqual(bar["foo"], "spam")
        self.assertEqual(bar["baz"], "42")
        self.assertIn("foo", bar)
        self.assertIn("baz", bar)
        self.assertNotIn("buzz", bar)
        with self.assertRaises(KeyError):
            del bar["buzz"]
        with self.assertRaises(IndexError):
            del bar[42]
        with self.assertRaises(TypeError):
            del bar[(1, 2)]  # type: ignore[call-overload] # should become KeyError?
        del bar["foo"]
        self.assertEqual(len(bar), 1)
        self.assertNotIn("foo", bar)


class TestStdSharedObject(unittest.TestCase):
    @unittest.skipUnless(hasattr(embedding, "StdSharedObject"), "needs std::shared_ptr")
    def testStdSharedObject(self) -> None:
        StdSharedObject = embedding.StdSharedObject
        self.assertEqual(StdSharedObject.constructed(), 0)
        self.assertEqual(StdSharedObject.destructed(), 0)
        self.assertEqual(StdSharedObject.deleted(), 0)

        shared = StdSharedObject.makeShared()
        self.assertIsNotNone(shared)
        assert shared is not None
        self.assertEqual(StdSharedObject.constructed(), 1)
        self.assertEqual(shared.method(), 1)
        self.assertEqual(StdSharedObject.destructed(), 0)
        self.assertEqual(StdSharedObject.deleted(), 0)

        del shared
        self.assertEqual(StdSharedObject.constructed(), 1)
        self.assertEqual(StdSharedObject.destructed(), 1)
        self.assertEqual(StdSharedObject.deleted(), 1)

        unique = StdSharedObject.makeUnique()
        self.assertIsNotNone(unique)
        assert unique is not None
        self.assertEqual(StdSharedObject.constructed(), 2)
        self.assertEqual(unique.method(), 2)
        self.assertEqual(StdSharedObject.destructed(), 1)
        self.assertEqual(StdSharedObject.deleted(), 1)

        del unique
        self.assertEqual(StdSharedObject.constructed(), 2)
        self.assertEqual(StdSharedObject.destructed(), 2)
        self.assertEqual(StdSharedObject.deleted(), 2)


class TestEnum(unittest.TestCase):
    def testIntEnum(self) -> None:
        Color = embedding.Color
        self.assertIsInstance(Color.RED, Color)
        self.assertIsInstance(Color.RED, int)
        self.assertEqual(Color.RED.value, 1)
        self.assertEqual(Color.GREEN.value, 2)
        self.assertEqual(Color.BLUE.value, 3)
        self.assertEqual(Color.RED, 1)
        self.assertEqual(Color.GREEN, 2)
        self.assertEqual(Color.BLUE, 3)
        self.assertEqual(str(Color.RED), "1")
        self.assertIs(Color(2), Color.GREEN)
        with self.assertRaises(ValueError):
            _ = Color(123)
        with self.assertRaises(ValueError):
            _ = Color("RED")  # type: ignore[arg-type]
        self.assertIs(embedding.passColor(Color.GREEN), Color.GREEN)
        self.assertIs(embedding.passColor(3), Color.BLUE)  # type: ignore[arg-type]
        with self.assertRaises(ValueError):
            _ = embedding.passColor(123)  # type: ignore[arg-type]
        with self.assertRaises(ValueError):
            _ = embedding.badColor()
        self.assertEqual(list(Color), [Color.RED, Color.GREEN, Color.BLUE])

    def testStrEnum(self) -> None:
        Shape = embedding.Bar.Shape
        self.assertIsInstance(Shape.CIRCLE, Shape)
        self.assertIsInstance(Shape.CIRCLE, str)
        self.assertEqual(Shape.CIRCLE.value, "circle")
        self.assertEqual(Shape.SQUARE.value, "square")
        self.assertEqual(Shape.TRIANGLE.value, "triangle")
        self.assertEqual(Shape.CIRCLE, "circle")
        self.assertEqual(Shape.SQUARE, "square")
        self.assertEqual(Shape.TRIANGLE, "triangle")
        self.assertEqual(str(Shape.SQUARE), "square")
        self.assertIs(Shape("triangle"), Shape.TRIANGLE)
        with self.assertRaises(ValueError):
            if not TYPE_CHECKING:
                # This is a arg-type error on Python 3.11 and later, but not on earlier
                # versions. Can't fix it for both, so avoid type checking it entirely.
                _ = Shape(2)  # type: ignore[arg-type]
        with self.assertRaises(ValueError):
            _ = Shape("notashape")
        self.assertIs(embedding.Bar.passShape(Shape.SQUARE), Shape.SQUARE)
        self.assertIs(embedding.Bar.passShape("triangle"), Shape.TRIANGLE)  # type: ignore[arg-type]
        with self.assertRaises(ValueError):
            _ = embedding.Bar.passShape("notashape")  # type: ignore[arg-type]
        self.assertIs(embedding.Bar.getShape(), Shape.SQUARE)
        self.assertTrue(embedding.Bar.isTriangle(Shape.TRIANGLE))
        self.assertFalse(embedding.Bar.isTriangle("square"))  # type: ignore[arg-type]
        with self.assertRaises(ValueError):
            _ = embedding.Bar.isTriangle("notashape")  # type: ignore[arg-type]
        with self.assertRaises(ValueError):
            _ = embedding.Bar.isTriangle(2)  # type: ignore[arg-type]
        with self.assertRaises(ValueError):
            _ = embedding.Bar.badShape()
        self.assertEqual(list(Shape), [Shape.CIRCLE, Shape.SQUARE, Shape.TRIANGLE])


class TestDatetime(unittest.TestCase):
    def testSystemClock(self) -> None:
        dt = datetime.datetime.fromtimestamp(0)
        self.assertEqual(embedding.testSystemClock(dt), dt)
        dt2 = datetime.datetime.fromtimestamp(0, tz=datetime.timezone.utc)
        self.assertEqual(embedding.testSystemClock(dt2), dt)
        ist = datetime.timezone(datetime.timedelta(hours=5, minutes=30), name="IST")
        dt3 = datetime.datetime.fromtimestamp(0, tz=ist)
        self.assertNotEqual(dt3.hour, dt2.hour)
        self.assertEqual(embedding.testSystemClock(dt3), dt)

        dt4 = datetime.datetime.now()
        self.assertEqual(embedding.testSystemClock(dt4), dt4)
        dt5 = dt4.astimezone(datetime.timezone.utc)
        self.assertEqual(embedding.testSystemClock(dt5), dt4)
        dt6 = dt4.astimezone(ist)
        self.assertNotEqual(dt6.hour, dt5.hour)
        self.assertEqual(embedding.testSystemClock(dt6), dt4)

        dt7 = datetime.datetime(2022, 10, 17)
        self.assertEqual(embedding.testSystemClock(dt7.date()), dt7)  # type: ignore[arg-type]


class TestInjectedClass(unittest.TestCase):
    def testInjectedClass(self) -> None:
        self.assertEqual(
            inspect.getdoc(embedding.InjectedClass), "Class injected into module"
        )


class TestModuleConstants(unittest.TestCase):
    def testModuleConstants(self) -> None:
        self.assertEqual(embedding.emIsThis, 3)
        self.assertEqual(embedding.emAnEnum, 7)
        self.assertEqual(embedding.STRING_CONSTANT, "string constant")
        self.assertEqual(embedding.INTEGER_CONSTANT, 42)
        self.assertEqual(embedding.INJECTED_INTEGER_CONSTANT, 99)
        self.assertEqual(embedding.INJECTED_STRING_CONSTANT, "spam and eggs")
        # self.assertIsInstance(embedding.aVectorObject, Sequence)  FIXME
        self.assertEqual(embedding.FUNCIONAL_CASTED_RED, int(embedding.Color.RED))
        self.assertEqual(embedding.INJECTED_ENUM_VALUE, 42)
        self.assertEqual(embedding.INJECTED_INT_ENUM_VALUE, 42)


test = unittest.defaultTestLoader.loadTestsFromModule(sys.modules[__name__])
testRunner = unittest.TextTestRunner(verbosity=2)
result = testRunner.run(test)
assert result.wasSuccessful()
