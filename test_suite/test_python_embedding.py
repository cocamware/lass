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
from contextlib import redirect_stdout

import embedding

print(sys.version)

POINTER_SIZE = struct.calcsize("P")  # size of a pointer in bytes


class TestDerived(unittest.TestCase):
    def testDerived(self):
        e = embedding.Bar()
        self.assertIsInstance(e, embedding.Bar)
        self.assertEqual(e.aMoreComplexFunction(1, 2), 3)
        d = embedding.DerivedBar()
        self.assertIsInstance(d, embedding.DerivedBar)
        self.assertEqual(d.aMoreComplexFunction(1, 2), 2)


class TestPyCObject(unittest.TestCase):
    def testNone(self):
        self.assertEqual(embedding.makeNullPointer(), None)
        self.assertTrue(embedding.testNullPointer(embedding.makeNullPointer()))

    def testPyCObject(self):
        self.assertTrue(
            str(type(embedding.makeSomePointer()))
            in ("<type 'PyCObject'>", "<class 'PyCObject'>", "<class 'PyCapsule'>")
        )
        self.assertTrue(embedding.testSomePointer(embedding.makeSomePointer()))


class TestConstMap(unittest.TestCase):
    def _testConstMap(self, mapping):
        self.assertEqual(len(mapping), 1)
        self.assertIn("spam", mapping)
        self.assertNotIn("foo", mapping)
        self.assertNotIn(123, mapping)
        self.assertEqual(mapping["spam"], "spam spam spam")
        self.assertListEqual(list(mapping.keys()), ["spam"])
        self.assertListEqual(list(mapping.values()), ["spam spam spam"])
        self.assertListEqual(list(mapping.items()), [("spam", "spam spam spam")])
        with self.assertRaises(TypeError):
            mapping["foo"] = "bar"
        with self.assertRaises(KeyError):
            _ = mapping["foo"]
        with self.assertRaises(KeyError):
            _ = mapping[123]
        self.assertEqual(mapping.get("foo"), None)
        self.assertEqual(mapping.get("foo", "bar"), "bar")
        with self.assertRaises(TypeError):
            mapping.clear()
        copy = mapping.copy()
        copy["foo"] = "bar"

    def testConstMap(self):
        bar = embedding.Bar()
        self._testConstMap(bar.constMap)
        bar.testConstMap(bar.constMap)


class TestMap(unittest.TestCase):
    def _testMap(self, mapping, refmap):
        mapping["test"] = "ok"
        with self.assertRaises(TypeError):
            mapping["test"] = 123
        with self.assertRaises(TypeError):
            mapping[123] = "ok"
        self.assertEqual(refmap["test"], "ok")
        self.assertIn("test", refmap)
        del mapping["test"]
        self.assertNotIn("test", refmap)
        with self.assertRaises(KeyError):
            del mapping["notakey"]
        with self.assertRaises(KeyError):
            del mapping[123]
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

    def testMap(self):
        bar = embedding.Bar()
        # self.assertIsInstance(bar.writeableMap, Mapping)  FIXME
        # self.assertIsInstance(bar.writeableMap, MutableMapping)  FIXME
        self._testMap(bar.writeableMap, bar.writeableMap)
        with self.assertRaises(TypeError):
            bar.writeableMap = 123
        with self.assertRaises(TypeError):
            bar.writeableMap = {1: 2, 3: 4}
        with self.assertRaises(TypeError):
            bar.writeableMap = ["123"]
        bar.testConstMap(bar.writeableMap)

    def testVectorMap(self):
        bar = embedding.Bar()
        # self.assertIsInstance(bar.writeableVectorMap, Mapping)  FIXME
        # self.assertIsInstance(bar.writeableVectorMap, MutableMapping)  FIXME
        self._testMap(bar.writeableVectorMap, bar.writeableVectorMap)


class TestConstSequence(unittest.TestCase):
    def _testConstSequence(self, seq):
        # Test that none of these operations are allowed on a const sequence
        with self.assertRaises(TypeError):
            seq[0:-1] = range(10)
        with self.assertRaises(TypeError):
            seq[3] = 5
        with self.assertRaises(TypeError):
            seq.clear()
        with self.assertRaises(TypeError):
            seq.reserve(5)
        with self.assertRaises(TypeError):
            seq.append(5)
        with self.assertRaises(TypeError):
            seq.pop()
        with self.assertRaises(TypeError):
            seq += [123]
        with self.assertRaises(TypeError):
            seq *= 2

    def testConstVector(self):
        self._testConstSequence(embedding.Bar().constVector)

    def testConstList(self):
        self._testConstSequence(embedding.Bar().constList)

    def testConstDeque(self):
        self._testConstSequence(embedding.Bar().constDeque)


class TestWriteableSequence(unittest.TestCase):
    def _testClear(self, seq, refseq):
        seq.clear()
        self.assertEqual(len(refseq), 0)

    def _testReserve(self, seq):
        with self.assertRaises(TypeError):
            seq.reserve("123")
        try:
            seq.reserve(5)
        except Exception as err:
            self.fail("seq.reserve raised exception: {!s}".format(err))

    def _testAppend(self, seq, refseq):
        for i in range(10):
            seq.append(i)
        self.assertEqual(len(refseq), 10)
        for i in range(10):
            self.assertEqual(refseq[i], i)
        with self.assertRaises(TypeError):
            seq.append("123")

    def _testRepr(self, seq, refseq):
        self.assertEqual(
            repr(seq), "[0.0, 1.0, 2.0, 3.0, 4.0, 5.0, 6.0, 7.0, 8.0, 9.0]"
        )

    def _testPop(self, seq, refseq):
        self.assertEqual(seq.pop(), 9)
        self.assertEqual(len(refseq), 9)
        for i in range(5):
            self.assertEqual(seq.pop(3), 3 + i)
        with self.assertRaises(IndexError):
            seq.pop(10)
        self.assertEqual(len(refseq), 4)
        for a, b in zip(refseq, [0, 1, 2, 8]):
            self.assertEqual(a, b)

    def _testContains(self, seq, _refseq):
        self.assertIn(8, seq)
        self.assertNotIn(-123456, seq)
        self.assertNotIn("123", seq)

    def _testGetItem(self, seq, refseq):
        with self.assertRaises(IndexError):
            seq[len(seq)]  # Index beyond end of sequence
        with self.assertRaises(IndexError):
            seq[-len(seq) - 1]  # Index before start of sequence
        self.assertEqual(seq[-1], refseq[-1])  # Last element
        self.assertEqual(seq[0], refseq[0])  # First element

    def _testSetItem(self, seq, refseq):
        seq[3] = 5
        self.assertEqual(refseq[3], 5)
        with self.assertRaises(TypeError):
            seq[3] = "foo"
        seq.append(2)
        del seq[len(seq) - 1]
        seq.append(2)
        del seq[-1]

    def _testGetSlice(self, seq, _refseq):
        self.assertListEqual(seq[:], [0, 1, 2, 5])
        self.assertListEqual(seq[:], [0, 1, 2, 5])
        self.assertListEqual(seq[:2], [0, 1])
        self.assertListEqual(seq[2:], [2, 5])
        self.assertListEqual(seq[50:], [])
        self.assertListEqual(seq[-2:], [2, 5])
        self.assertListEqual(seq[-100:42], [0, 1, 2, 5])
        self.assertListEqual(seq[2:1], [])
        self.assertListEqual(seq[1::2], [1, 5])

    def _testSetSlice(self, seq, refseq):
        seq[1:3] = [10, 11, 12]
        self.assertListEqual(refseq[:], [0, 10, 11, 12, 5])
        seq[:] = []
        self.assertListEqual(refseq[:], [])
        seq[:] = range(10)
        self.assertListEqual(refseq[:], list(range(10)))
        with self.assertRaises(ValueError):
            seq[2:9:2] = [1, 2]  # Wrong length for extended slice assignment
        seq[2:8:2] = [20, 40, 60]
        self.assertListEqual(refseq[:], [0, 1, 20, 3, 40, 5, 60, 7, 8, 9])
        del seq[3:9:2]
        self.assertListEqual(refseq[:], [0, 1, 20, 40, 60, 8, 9])
        with self.assertRaises(TypeError):
            seq[2:8:2] = ["20", "40", "60"]
        seq[2:5] = seq
        self.assertListEqual(refseq[:], [0, 1, 0, 1, 20, 40, 60, 8, 9, 8, 9])
        del seq[:2]
        del seq[-2:]
        self.assertListEqual(refseq[:], [0, 1, 20, 40, 60, 8, 9])

    def _testRepeat(self, seq, refseq):
        n = len(seq)
        seq *= 2
        self.assertEqual(len(refseq), 2 * n)
        b = seq * 2
        self.assertEqual(len(refseq), 2 * n)
        for i in range(n):
            self.assertEqual(seq[i], seq[n + i])
        self.assertEqual(len(b), 4 * n)

    def _testConcat(self, seq, refseq):
        n = len(seq)
        seq += refseq
        self.assertEqual(len(refseq), 2 * n)
        b = seq + refseq
        self.assertEqual(len(refseq), 2 * n)
        for i in range(n):
            self.assertEqual(seq[i], seq[n + i])
        self.assertEqual(len(b), 4 * n)

    def _testSequence(self, seq, refseq):
        self._testClear(seq, refseq)
        self._testReserve(seq)
        self._testAppend(seq, refseq)
        self._testRepr(seq, refseq)
        self._testPop(seq, refseq)
        self._testContains(seq, refseq)
        self._testGetItem(seq, refseq)
        self._testSetItem(seq, refseq)
        self._testGetSlice(seq, refseq)
        self._testSetSlice(seq, refseq)
        self._testRepeat(seq, refseq)
        self._testConcat(seq, refseq)

    def testWriteableVector(self):
        bar = embedding.Bar()
        # self.assertIsInstance(bar.writeableVector, Sequence)  FIXME
        # self.assertIsInstance(bar.writeableVector, MutableSequence)  FIXME
        self._testSequence(bar.writeableVector, bar.writeableVector)
        with self.assertRaises(TypeError):
            bar.writeableVector = 123
        with self.assertRaises(TypeError):
            bar.writeableVector = {1: 2, 3: 4}
        with self.assertRaises(TypeError):
            bar.writeableVector = ["123"]
        bar.writeableVector = range(3)
        # self.assertEqual(bar.writeableVector, [0, 1, 2])  FIXME
        self.assertListEqual(list(bar.writeableVector), [0, 1, 2])

    def testWriteableList(self):
        bar = embedding.Bar()
        self._testSequence(bar.writeableList, bar.writeableList)

    def testWriteableDeque(self):
        bar = embedding.Bar()
        self._testSequence(bar.writeableDeque, bar.writeableDeque)

    def testWriteableStaticVector(self):
        bar = embedding.Bar()
        self._testSequence(bar.writeableStaticVector, bar.writeableStaticVector)


class TestDocstrings(unittest.TestCase):
    def testDocstrings(self):
        self.assertEqual(embedding.__doc__, "Documentation for module embedding")
        self.assertEqual(embedding.PythonFoo.__doc__, "Documentation for class Foo.")
        self.assertEqual(embedding.Bar.tester.__doc__, "tester doc")
        self.assertEqual(embedding.Bar.InnerClass.__doc__, "InnerClass of Bar")
        self.assertEqual(embedding.ClassB.properImagine.__doc__, "blabla")


class TestMethods(unittest.TestCase):
    def testMethods(self):
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
        self.assertEqual(bar.primArguments(box2, "y", matrix2), answer)

    def testFreeMethods(self):
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
    def testClassName(self):
        bar = embedding.Bar()
        self.assertEqual(bar.__class__, embedding.Bar)
        self.assertEqual(embedding.Bar.__class__, type)
        self.assertEqual(embedding.Bar.__name__, "Bar")
        self.assertEqual(embedding.Bar.__module__, "embedding")
        self.assertEqual(embedding.Bar.__qualname__, "Bar")

    def testInnerClassName(self):
        innerClass = embedding.Bar.InnerClass("the answer is 42")
        self.assertEqual(innerClass.__class__, embedding.Bar.InnerClass)
        self.assertEqual(embedding.Bar.InnerClass.__class__, type)
        self.assertEqual(embedding.Bar.InnerClass.__name__, "InnerClass")
        # self.assertEqual(embedding.Bar.InnerClass.__module__, "embedding")  FIXME
        # self.assertEqual(embedding.Bar.InnerClass.__qualname__, "Bar.InnerClass")  FIXME

    def testEnumClassName(self):
        color = embedding.Color.BLUE
        self.assertEqual(color.__class__, embedding.Color)
        self.assertEqual(embedding.Color.__name__, "Color")
        self.assertEqual(embedding.Color.__module__, "embedding")
        self.assertEqual(embedding.Color.__qualname__, "Color")

    def testClassEnumName(self):
        shape = embedding.Bar.Shape.CIRCLE
        self.assertEqual(shape.__class__, embedding.Bar.Shape)
        self.assertEqual(embedding.Bar.Shape.__name__, "Shape")
        # self.assertEqual(embedding.Bar.Shape.__module__, "embedding")  FIXME
        # self.assertEqual(embedding.Bar.Shape.__qualname__, "Bar.Shape")  FIXME


class TestTypeSafety(unittest.TestCase):
    def testTypeSafety(self):
        embedding.Bar.aStaticMethod(3.14)
        with self.assertRaises(TypeError):
            embedding.Bar.aStaticMethod("crash here")

        bar = embedding.Bar()
        bar.myInt = 123
        with self.assertRaises(TypeError):
            bar.myInt = "bogus"


class TestProperties(unittest.TestCase):
    def testGetSetter(self):
        bar = embedding.Bar()
        self.assertEqual(bar.myInt, 0)
        bar.myInt = 5
        self.assertEqual(bar.myInt, 5)
        with self.assertRaises(TypeError):
            bar.myInt = "foo"
        with self.assertRaises(TypeError):
            bar.myInt = 3.14

    def testObjectProperty(self):
        bar = embedding.Bar()
        self.assertIs(bar.foo, None)
        foo = embedding.PythonFoo(6, "world!")
        bar.foo = foo
        self.assertIs(bar.foo, foo)
        bar.foo = None
        self.assertIs(bar.foo, None)

    def testPublicProperty(self):
        bar = embedding.Bar()
        self.assertEqual(bar.publicInt, 42)
        bar.publicInt = 60
        self.assertEqual(bar.publicInt, 60)
        with self.assertRaises(TypeError):
            bar.publicInt = "foo"
        with self.assertRaises(TypeError):
            bar.publicInt = 3.14

    def testFreeProperty(self):
        b = embedding.ClassB()
        self.assertEqual(b.properImagine, 1)
        b.properImagine = 2
        self.assertEqual(b.properImagine, 2)
        with self.assertRaises(TypeError):
            b.properImagine = "foo"
        with self.assertRaises(TypeError):
            b.properImagine = 3.14

    def testFreePropertyConst(self):
        b = embedding.ClassB()
        self.assertEqual(b.properImagineRO, 1)
        with self.assertRaises(AttributeError):
            b.properImagineRO = 2
        with self.assertRaises(AttributeError):
            b.properImagineRO = "foo"
        with self.assertRaises(AttributeError):
            b.properImagineRO = 3.14


class TestSignatures(unittest.TestCase):
    def testCString(self):
        self.assertEqual(embedding.testCStringSupport("Hello World!"), "Hello World!")


class TestOperators(unittest.TestCase):
    def testCallOperator(self):
        bar = embedding.Bar()
        self.assertEqual(bar(4), 8)
        self.assertEqual(bar("hallo"), "HALLO")
        self.assertEqual(bar(3.5), 1.75)

    def testStrOperator(self):
        bar = embedding.Bar()
        self.assertEqual(str(bar), "A free __str__ representation of Bar")
        self.assertEqual(repr(bar), "A free __repr__ representation of Bar")

    def testNumericOperators(self):
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


class TestStaticMembers(unittest.TestCase):
    def testClassConstants(self):
        self.assertEqual(embedding.Bar.CONST, 5)
        with self.assertRaises(TypeError):
            embedding.Bar.CONST = 6

    def testInnerClasses(self):
        innerClass = embedding.Bar.InnerClass("the answer is 42")
        self.assertEqual(innerClass.talkTo("Arthur"), "Arthur, the answer is 42.\n")
        self.assertEqual(inspect.getdoc(embedding.Bar.InnerClass), "InnerClass of Bar")

    def testStaticMethods(self):
        self.assertEqual(embedding.Bar.aStaticMethod(3.14), 3)
        bar = embedding.Bar()
        self.assertEqual(bar.aStaticMethod(5.1), 5)


class TestOverloading(unittest.TestCase):
    def testOverloadedFunctions(self):
        self.assertEqual(embedding.overloaded(5), 10)
        self.assertEqual(embedding.overloaded("hello!"), "HELLO!")
        self.assertEqual(embedding.overloaded(0.3 + 0.4j), 0.5)

    def testOverloadedMethods(self):
        bar = embedding.Bar()
        self.assertEqual(bar.tester("a string"), "complexArguments: 'a string'")
        box = ((10, 20, 30), (100, 200, 300))
        matrix = ((1, 2, 3, 4), (5, 6, 7, 8), (9, 10, 11, 12), (13, 14, 15, 16))
        answer = """size of box along y axis is 180
transformation: ((1, 2, 3, 4), (5, 6, 7, 8), (9, 10, 11, 12), (13, 14, 15, 16))
"""
        self.assertEqual(bar.tester(box, "y", matrix), answer)
        matrix2 = ((1, 2, 3, 4), [5, 6, 7, 8], (9, 10, 11, 12), (13, 14, 15, 16))
        self.assertEqual(bar.tester(box, "y", matrix2), answer)


class TestSpecialFunctionsAndOperators(unittest.TestCase):
    def testSequenceProtocol(self):
        c = embedding.ClassB()
        self.assertEqual(len(c), 5)
        self.assertEqual(c[1], 1)


class TestTuples(unittest.TestCase):
    def assertTupleAlmostEqual(self, first, second, places=7, msg=None):
        msg = msg or "%r != %r within %r places" % (first, second, places)
        self.assertEqual(len(first), len(second), msg)
        for a, b in zip(first, second):
            self.assertAlmostEqual(a, b, places=places, msg=msg)

    def testVariableLength(self):
        bar = embedding.Bar()
        self.assertTupleAlmostEqual(bar.rgba((0.1, 0.2, 0.3)), (0.1, 0.2, 0.3, 1.0))
        self.assertTupleAlmostEqual(
            bar.rgba([0.1, 0.2, 0.3, 0.4]), (0.1, 0.2, 0.3, 0.4)
        )
        with self.assertRaises(TypeError):
            bar.rgba([])
        with self.assertRaises(TypeError):
            bar.rgba((0.1, 0.2))
        with self.assertRaises(TypeError):
            bar.rgba((0.1, 0.2, 0.3, 0.4, 0.5))


class TestConstructors(unittest.TestCase):
    def testOverloadedConstructors(self):
        barA = embedding.Bar()
        self.assertEqual(barA.myInt, 0)

        barB = embedding.Bar(5, "hello")
        self.assertEqual(barB.myInt, 5)

    def testFreeConstructors(self):
        classB = embedding.ClassB(5)
        self.assertEqual(classB.properImagine, 5)


class TestShadowHierarchy(unittest.TestCase):
    def testBacon(self):
        bacon = embedding.Bacon()
        self.assertEqual(bacon.virtualWho(), "Bacon")
        self.assertEqual(bacon.overridenWho(), "Bacon")
        self.assertEqual(bacon.__class__, embedding.Bacon)
        self.assertEqual(bacon.__class__.__bases__, (embedding.Ham,))
        sup = super(bacon.__class__, bacon)
        self.assertEqual(sup.virtualWho(), "Bacon")
        self.assertEqual(sup.overridenWho(), "Ham")

    def testHam(self):
        ham = embedding.Ham()
        self.assertEqual(ham.virtualWho(), "Ham")
        self.assertEqual(ham.overridenWho(), "Ham")

    def testEggs(self):
        eggs = embedding.Eggs(3)
        self.assertEqual(eggs.virtualWho(), "3 Eggs")
        self.assertEqual(eggs.overridenWho(), "3 Eggs")
        self.assertEqual(eggs.number, 3)
        eggs.number = 4
        self.assertEqual(eggs.number, 4)

    def testSpam(self):
        spam = embedding.makeSpam("Bacon")
        self.assertEqual(spam.virtualWho(), "Bacon")
        self.assertEqual(spam.overridenWho(), "Bacon")
        self.assertFalse(embedding.spamToCppByCopy(spam, spam))
        self.assertTrue(embedding.spamToCppByConstReference(spam, spam))
        self.assertTrue(embedding.spamToCppByReference(spam, spam))


class TestCallbacks(unittest.TestCase):
    def setUp(self):
        self.isCalled = False
        self.arg = None

    def testCallback0(self):
        def callback():
            self.isCalled = True

        embedding.call0(callback)
        self.assertTrue(self.isCalled)

    def testCallback1(self):
        hi = "hi!"

        def callback(x):
            self.isCalled = True
            self.arg = x

        embedding.call1(callback, hi)
        self.assertTrue(self.isCalled)
        self.assertEqual(self.arg, hi)

    def testCallbackR0(self):
        hi = "hi!"

        def callback():
            self.isCalled = True
            return hi

        result = embedding.callR0(callback)
        self.assertTrue(self.isCalled)
        self.assertEqual(result, hi)

    def testCallbackR2(self):
        a, b = 5, 6
        c = embedding.callR2(lambda x, y: x * y, a, b)
        self.assertEqual(c, a * b)

    def testCallbackException(self):
        def callback():
            self.isCalled = True
            d = {"a": 1}
            return d["b"]  # raises a KeyError

        with self.assertRaises(KeyError):
            embedding.call0(callback)
        self.assertTrue(self.isCalled)


class TestRichCompare(unittest.TestCase):
    def setUp(self):
        self.a = embedding.Bar(5, "A")
        self.b = embedding.Bar(5, "B")
        self.c = embedding.Bar(6, "C")

    def testEqual(self):
        a, b, c = self.a, self.b, self.c
        self.assertTrue(a == b)
        self.assertFalse(a == c)
        self.assertFalse(a == None)  # noqa: E711, none-comparison

    def testLess(self):
        a, b, c = self.a, self.b, self.c
        self.assertTrue(a < c)
        self.assertFalse(a < b)
        self.assertFalse(c < a)
        self.assertFalse(a < None)

    def testNotEqual(self):
        a, b, c = self.a, self.b, self.c
        self.assertFalse(a != b)
        self.assertTrue(a != c)
        self.assertTrue(a != None)  # noqa: E711, none-comparison

    def testGreater(self):
        a, b, c = self.a, self.b, self.c
        self.assertTrue(c > a)
        self.assertFalse(b > a)
        self.assertFalse(a > c)
        self.assertFalse(a > None)

    def testLessEqual(self):
        a, b, c = self.a, self.b, self.c
        self.assertTrue(a <= b)
        self.assertTrue(a <= c)
        self.assertFalse(c <= a)
        self.assertFalse(a <= None)

    def testGreaterEqual(self):
        a, b, c = self.a, self.b, self.c
        self.assertTrue(c >= a)
        self.assertTrue(a >= b)
        self.assertFalse(a >= c)
        self.assertFalse(a >= None)


dummyCounter = 0


def dummyCallback0():
    global dummyCounter
    dummyCounter += 1
    return


def dummyCallback1(adder):
    global dummyCounter
    dummyCounter += adder
    return


class TestMultiCallback(unittest.TestCase):
    def testConstruction(self):
        global dummyCounter
        a = embedding.TestCallback()
        # can we grab the object
        cb = a.callback
        # can we add a function to the multi-dispatching callback
        cb.add(dummyCallback0)
        # calling a wrapped callback
        cb.call()
        self.assertEqual(dummyCounter, 1)
        cb.add(dummyCallback0)
        cb.call()
        self.assertEqual(dummyCounter, 3)
        cb.reset()
        cb()
        self.assertEqual(dummyCounter, 3)
        for i in range(10):
            cb.add(dummyCallback0)
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
    def testSysStdout(self):
        with redirect_stdout(io.StringIO()) as buffer:
            embedding.writeStdout("Hello\n")
            embedding.writeStdout("World\n")
        self.assertEqual(buffer.getvalue(), "Hello\nWorld\n")


class MyFloat:
    def __init__(self, value):
        self.value = value

    def __float__(self):
        return float(self.value)


skipIfNoU8string = unittest.skipIf(
    not getattr(embedding, "LASS_HAVE_STD_U8STRING", False),
    "std::u8string not supported",
)


class TestTypes(unittest.TestCase):
    def testStdString(self):
        self.assertEqual(embedding.testStdString(""), "")
        self.assertEqual(embedding.testStdString("abc"), "abc")
        self.assertEqual(embedding.testStdString("𐌷𐌴𐌻𐌻𐍉"), "𐌷𐌴𐌻𐌻𐍉")
        self.assertEqual(embedding.testStdString("\0"), "\0")
        self.assertEqual(embedding.testStdString("a\0b"), "a\0b")

    def testStdWstring(self):
        self.assertEqual(embedding.testStdWstring(""), "")
        self.assertEqual(embedding.testStdWstring("abc"), "abc")
        self.assertEqual(embedding.testStdWstring("𐌷𐌴𐌻𐌻𐍉"), "𐌷𐌴𐌻𐌻𐍉")
        self.assertEqual(embedding.testStdWstring("\0"), "\0")
        self.assertEqual(embedding.testStdWstring("a\0b"), "a\0b")

    @skipIfNoU8string
    def testStdU8string(self):
        self.assertEqual(embedding.testStdU8string(""), "")
        self.assertEqual(embedding.testStdU8string("abc"), "abc")
        self.assertEqual(embedding.testStdU8string("𐌷𐌴𐌻𐌻𐍉"), "𐌷𐌴𐌻𐌻𐍉")
        self.assertEqual(embedding.testStdU8string("\0"), "\0")
        self.assertEqual(embedding.testStdU8string("a\0b"), "a\0b")

    def testStdU16string(self):
        self.assertEqual(embedding.testStdU16string(""), "")
        self.assertEqual(embedding.testStdU16string("abc"), "abc")
        self.assertEqual(embedding.testStdU16string("𐌷𐌴𐌻𐌻𐍉"), "𐌷𐌴𐌻𐌻𐍉")
        self.assertEqual(embedding.testStdU16string("\0"), "\0")
        self.assertEqual(embedding.testStdU16string("a\0b"), "a\0b")

    def testStdU32string(self):
        self.assertEqual(embedding.testStdU32string(""), "")
        self.assertEqual(embedding.testStdU32string("abc"), "abc")
        self.assertEqual(embedding.testStdU32string("𐌷𐌴𐌻𐌻𐍉"), "𐌷𐌴𐌻𐌻𐍉")
        self.assertEqual(embedding.testStdU32string("\0"), "\0")
        self.assertEqual(embedding.testStdU32string("a\0b"), "a\0b")

    def testStdStringView(self):
        self.assertEqual(embedding.testStdStringView(""), "")
        self.assertEqual(embedding.testStdStringView("abc"), "abc")
        self.assertEqual(embedding.testStdStringView("𐌷𐌴𐌻𐌻𐍉"), "𐌷𐌴𐌻𐌻𐍉")
        self.assertEqual(embedding.testStdStringView("\0"), "\0")
        self.assertEqual(embedding.testStdStringView("a\0b"), "a\0b")

    def testStdWstringView(self):
        self.assertEqual(embedding.testStdWstringView(""), "")
        self.assertEqual(embedding.testStdWstringView("abc"), "abc")
        self.assertEqual(embedding.testStdWstringView("𐌷𐌴𐌻𐌻𐍉"), "𐌷𐌴𐌻𐌻𐍉")
        self.assertEqual(embedding.testStdWstringView("\0"), "\0")
        self.assertEqual(embedding.testStdWstringView("a\0b"), "a\0b")

    @skipIfNoU8string
    def testStdU8stringView(self):
        self.assertEqual(embedding.testStdU8stringView(""), "")
        self.assertEqual(embedding.testStdU8stringView("abc"), "abc")
        self.assertEqual(embedding.testStdU8stringView("𐌷𐌴𐌻𐌻𐍉"), "𐌷𐌴𐌻𐌻𐍉")
        self.assertEqual(embedding.testStdU8stringView("\0"), "\0")
        self.assertEqual(embedding.testStdU8stringView("a\0b"), "a\0b")

    def testStdU16stringView(self):
        self.assertEqual(embedding.testStdU16stringView(""), "")
        self.assertEqual(embedding.testStdU16stringView("abc"), "abc")
        self.assertEqual(embedding.testStdU16stringView("𐌷𐌴𐌻𐌻𐍉"), "𐌷𐌴𐌻𐌻𐍉")
        self.assertEqual(embedding.testStdU16stringView("\0"), "\0")
        self.assertEqual(embedding.testStdU16stringView("a\0b"), "a\0b")

    def testStdU32stringView(self):
        self.assertEqual(embedding.testStdU32stringView(""), "")
        self.assertEqual(embedding.testStdU32stringView("abc"), "abc")
        self.assertEqual(embedding.testStdU32stringView("𐌷𐌴𐌻𐌻𐍉"), "𐌷𐌴𐌻𐌻𐍉")
        self.assertEqual(embedding.testStdU32stringView("\0"), "\0")
        self.assertEqual(embedding.testStdU32stringView("a\0b"), "a\0b")

    def testConstCharPtr(self):
        self.assertEqual(embedding.testConstCharPtr(""), "")
        self.assertEqual(embedding.testConstCharPtr("abc"), "abc")
        self.assertEqual(embedding.testConstCharPtr("𐌷𐌴𐌻𐌻𐍉"), "𐌷𐌴𐌻𐌻𐍉")

    def testConstWcharPtr(self):
        self.assertEqual(embedding.testConstWcharPtr(""), "")
        self.assertEqual(embedding.testConstWcharPtr("abc"), "abc")
        self.assertEqual(embedding.testConstWcharPtr("𐌷𐌴𐌻𐌻𐍉"), "𐌷𐌴𐌻𐌻𐍉")

    @skipIfNoU8string
    def testConstChar8Ptr(self):
        self.assertEqual(embedding.testConstChar8Ptr(""), "")
        self.assertEqual(embedding.testConstChar8Ptr("abc"), "abc")
        self.assertEqual(embedding.testConstChar8Ptr("𐌷𐌴𐌻𐌻𐍉"), "𐌷𐌴𐌻𐌻𐍉")

    def testConstChar16Ptr(self):
        self.assertEqual(embedding.testConstChar16Ptr(""), "")
        self.assertEqual(embedding.testConstChar16Ptr("abc"), "abc")
        self.assertEqual(embedding.testConstChar16Ptr("𐌷𐌴𐌻𐌻𐍉"), "𐌷𐌴𐌻𐌻𐍉")

    def testConstChar32Ptr(self):
        self.assertEqual(embedding.testConstChar32Ptr(""), "")
        self.assertEqual(embedding.testConstChar32Ptr("abc"), "abc")
        self.assertEqual(embedding.testConstChar32Ptr("𐌷𐌴𐌻𐌻𐍉"), "𐌷𐌴𐌻𐌻𐍉")

    def testFloatSingle(self):
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
                embedding.testFloatSingle(MyFloat(1.5))
        else:
            # Custom objects should be converted to float first
            self.assertEqual(embedding.testFloatSingle(MyFloat(1.5)), 1.5)
            self.assertEqual(embedding.testFloatSingle(MyFloat("inf")), inf)
            self.assertEqual(embedding.testFloatSingle(MyFloat(1e40)), inf)
            self.assertTrue(math.isnan(embedding.testFloatSingle(MyFloat("nan"))))
            with self.assertRaises(OverflowError):
                float(MyFloat(2**1024))

    def testFloatDouble(self):
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
                embedding.testFloatDouble(MyFloat(1.5))
        else:
            # Custom objects should be converted to float first
            self.assertEqual(embedding.testFloatDouble(MyFloat(1.5)), 1.5)
            self.assertEqual(embedding.testFloatDouble(MyFloat("inf")), inf)
            self.assertEqual(embedding.testFloatDouble(MyFloat(1e400)), inf)
            self.assertTrue(math.isnan(embedding.testFloatDouble(MyFloat("nan"))))
            with self.assertRaises(OverflowError):
                float(MyFloat(2**1024))


class TestBar(unittest.TestCase):
    def testMapProtocol(self):
        bar = embedding.Bar()
        self.assertEqual(len(bar), 0)
        with self.assertRaises(KeyError):
            _ = bar["foo"]
        with self.assertRaises(TypeError):
            _ = bar[1]  # should become KeyError?
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
            del bar[(1, 2)]  # should become KeyError?
        del bar["foo"]
        self.assertEqual(len(bar), 1)
        self.assertNotIn("foo", bar)


class TestStdSharedObject(unittest.TestCase):
    @unittest.skipUnless(hasattr(embedding, "StdSharedObject"), "needs std::shared_ptr")
    def testStdSharedObject(self):
        StdSharedObject = embedding.StdSharedObject
        self.assertEqual(StdSharedObject.constructed(), 0)
        self.assertEqual(StdSharedObject.destructed(), 0)
        self.assertEqual(StdSharedObject.deleted(), 0)

        shared = StdSharedObject.makeShared()
        self.assertEqual(StdSharedObject.constructed(), 1)
        self.assertEqual(shared.method(), 1)
        self.assertEqual(StdSharedObject.destructed(), 0)
        self.assertEqual(StdSharedObject.deleted(), 0)

        del shared
        self.assertEqual(StdSharedObject.constructed(), 1)
        self.assertEqual(StdSharedObject.destructed(), 1)
        self.assertEqual(StdSharedObject.deleted(), 1)

        unique = StdSharedObject.makeUnique()
        self.assertEqual(StdSharedObject.constructed(), 2)
        self.assertEqual(unique.method(), 2)
        self.assertEqual(StdSharedObject.destructed(), 1)
        self.assertEqual(StdSharedObject.deleted(), 1)

        del unique
        self.assertEqual(StdSharedObject.constructed(), 2)
        self.assertEqual(StdSharedObject.destructed(), 2)
        self.assertEqual(StdSharedObject.deleted(), 2)


class TestEnum(unittest.TestCase):
    def testIntEnum(self):
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
            _ = Color("RED")
        self.assertIs(embedding.passColor(Color.GREEN), Color.GREEN)
        self.assertIs(embedding.passColor(3), Color.BLUE)
        with self.assertRaises(ValueError):
            _ = embedding.passColor(123)
        with self.assertRaises(ValueError):
            _ = embedding.badColor()
        self.assertEqual(list(Color), [Color.RED, Color.GREEN, Color.BLUE])

    def testStrEnum(self):
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
            _ = Shape(2)
        with self.assertRaises(ValueError):
            _ = Shape("notashape")
        self.assertIs(embedding.Bar.passShape(Shape.SQUARE), Shape.SQUARE)
        self.assertIs(embedding.Bar.passShape("triangle"), Shape.TRIANGLE)
        with self.assertRaises(ValueError):
            _ = embedding.Bar.passShape("notashape")
        self.assertIs(embedding.Bar.getShape(), Shape.SQUARE)
        self.assertTrue(embedding.Bar.isTriangle(Shape.TRIANGLE))
        self.assertFalse(embedding.Bar.isTriangle("square"))
        with self.assertRaises(ValueError):
            _ = embedding.Bar.isTriangle("notashape")
        with self.assertRaises(ValueError):
            _ = embedding.Bar.isTriangle(2)
        with self.assertRaises(ValueError):
            _ = embedding.Bar.badShape()
        self.assertEqual(list(Shape), [Shape.CIRCLE, Shape.SQUARE, Shape.TRIANGLE])


class TestDatetime(unittest.TestCase):
    def testSystemClock(self):
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
        self.assertEqual(embedding.testSystemClock(dt7.date()), dt7)


class TestInjectedClass(unittest.TestCase):
    def testInjectedClass(self):
        self.assertEqual(
            inspect.getdoc(embedding.InjectedClass), "Class injected into module"
        )


test = unittest.defaultTestLoader.loadTestsFromModule(sys.modules[__name__])
testRunner = unittest.TextTestRunner(verbosity=2)
result = testRunner.run(test)
assert result.wasSuccessful()
