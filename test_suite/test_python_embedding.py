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
import math
import sys
import unittest
import inspect

def echo(*xs):
	sys.stdout.write(' '.join(map(str, xs)) + '\n')

echo(sys.version)


numberOfErrors = 0
def reportError(description):
    global numberOfErrors
    numberOfErrors += 1
    if sys.exc_info()[0]:
        echo("ERROR: %s: %s" % (description, sys.exc_info()[0]))
    else:
        echo("ERROR: %s" % description)
    sys.exc_clear()

if 0:
	import code
	s = code.InteractiveConsole()
	s.interact()


import embedding
barC = embedding.Bar()


echo("Testing print functionality", barC)


class TestClass:
    def __init__(self):
        self.member1 = 777
    def staticMethod(a,b):
        return a+b
    staticMethod = staticmethod(staticMethod)

    def normalFunction1(self,a):
        return a
    def normalFunction2(self):
        return self.member1


class TestDerived(unittest.TestCase):
	def testDerived(self):
		e = embedding.Bar()
		self.assertTrue(isinstance(e, embedding.Bar))
		self.assertEqual(e.aMoreComplexFunction(1,2), 3)
		d = embedding.DerivedBar()
		self.assertTrue(isinstance(d, embedding.DerivedBar))
		self.assertEqual(d.aMoreComplexFunction(1,2), 2)


class TestPyCObject(unittest.TestCase):
	def testNone(self):
		self.assertEqual(embedding.makeNullPointer(), None)
		self.assertTrue(embedding.testNullPointer(embedding.makeNullPointer()))
	def testPyCObject(self):
		self.assertTrue(str(type(embedding.makeSomePointer())) in ("<type 'PyCObject'>", "<class 'PyCObject'>", "<class 'PyCapsule'>"))
		self.assertTrue(embedding.testSomePointer(embedding.makeSomePointer()))

echo("\n***\n")

barC = embedding.Bar()



class TestConstMap(unittest.TestCase):
	def _testConstMap(self, mapping):
		self.assertEqual(len(mapping), 1)
		self.assertTrue("spam" in mapping)
		self.assertFalse("foo" in mapping)
		self.assertFalse(123 in mapping)
		self.assertEqual(mapping["spam"], "spam spam spam")
		self.assertEqual(list(mapping.keys()), ["spam"])
		self.assertEqual(list(mapping.values()), ["spam spam spam"])
		self.assertEqual(list(mapping.items()), [("spam", "spam spam spam")])
		self.assertRaises(TypeError, mapping.__setitem__, mapping, "foo", "bar")
		self.assertRaises(KeyError, mapping.__getitem__, "foo")
		self.assertRaises(KeyError, mapping.__getitem__, 123)
		self.assertEqual(mapping.get("foo"), None)
		self.assertEqual(mapping.get("foo", "bar"), "bar")
		def clear(m): m.clear()
		self.assertRaises(TypeError, clear, mapping)
		copy = mapping.copy()
		copy['foo'] = 'bar'
	def testConstMap(self):
		bar = embedding.Bar()
		self._testConstMap(bar.constMap)
		bar.testConstMap(bar.constMap)
		

class TestMap(unittest.TestCase):
	def _testMap(self, mapping, refmap):
		mapping['test'] = 'ok'
		self.assertRaises(TypeError, mapping.__setitem__, "test", 123)
		self.assertRaises(TypeError, mapping.__setitem__, 123, "ok")
		self.assertEqual(refmap['test'], 'ok')
		self.assertTrue('test' in refmap)
		del mapping['test']
		self.assertRaises(KeyError, mapping.__delitem__, "notakey")
		self.assertRaises(KeyError, mapping.__delitem__, 123)
		self.assertTrue('test' not in refmap)
		mapping.clear()
		mapping['A'] = 'a'
		self.assertEqual(repr(mapping), "{'A': 'a'}")
		mapping['B'] = 'b'
		mapping['C'] = 'c'
		self.assertEqual(len(refmap), 3)
		for a, b in zip(mapping, refmap.keys()):
			self.assertEqual(a, b)
		for a, b, (c, d) in zip(mapping.keys(), mapping.values(), mapping.items()):
			self.assertEqual(a, c)
			self.assertEqual(b, d)
	def testMap(self):
		bar = embedding.Bar()
		self._testMap(bar.writeableMap, bar.writeableMap)
		self.assertRaises(TypeError, setattr, bar, "writeableMap", 123)
		self.assertRaises(TypeError, setattr, bar, "writeableMap", {1:2, 3:4})
		self.assertRaises(TypeError, setattr, bar, "writeableMap", ["123"])
		bar.testConstMap(bar.writeableMap)
	def testVectorMap(self):
		bar = embedding.Bar()
		self._testMap(bar.writeableVectorMap, bar.writeableVectorMap)
		
class TestConstSequence(unittest.TestCase):
	def _testConstSequence(self, seq):
		self.assertRaises(TypeError, seq.__setitem__, slice(0, -1), range(10))
		self.assertRaises(TypeError, seq.__setitem__, 3, 5)
		self.assertRaises(TypeError, seq.clear)
		self.assertRaises(TypeError, seq.reserve, 5)
		self.assertRaises(TypeError, seq.append, 5)
		self.assertRaises(TypeError, seq.pop)
		self.assertRaises(TypeError, seq.__iadd__, [123])
		self.assertRaises(TypeError, seq.__imul__, 2)
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
		self.assertRaises(TypeError, seq.reserve, "123")
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
		self.assertRaises(TypeError, seq.append, "123")
	def _testRepr(self, seq, refseq):
		self.assertEqual(repr(seq), "[0.0, 1.0, 2.0, 3.0, 4.0, 5.0, 6.0, 7.0, 8.0, 9.0]")
	def _testPop(self, seq, refseq):
		self.assertEqual(seq.pop(), 9)
		self.assertEqual(len(refseq), 9)
		for i in range(5):
			self.assertEqual(seq.pop(3), 3 + i)
		self.assertRaises(IndexError, seq.pop, 10)
		self.assertEqual(len(refseq), 4)
		for a, b in zip(refseq, [0, 1, 2, 8]):
			self.assertEqual(a, b)
	def _testContains(self, seq, refseq):
		self.assertIn(8, seq)
		self.assertNotIn(-123456, seq)
		self.assertNotIn("123", seq)
	def _testGetItem(self, seq, refseq):
		n = len(seq)
		self.assertRaises(IndexError, lambda a, i: a[i], seq, n)
		self.assertRaises(IndexError, lambda a, i: a[i], seq, -n-1)
		self.assertEqual(seq[n - 1], refseq[-1])
		self.assertEqual(seq[-n], refseq[0])
	def _testSetItem(self, seq, refseq):
		seq[3] = 5
		self.assertEqual(refseq[3], 5)
		self.assertRaises(TypeError, seq.__setitem__, 3, "foo")
		seq.append(2)
		del seq[len(seq) - 1]
		seq.append(2)
		del seq[-1]
	def _testGetSlice(self, seq, refseq):
		self.assertEqual(seq[:], [0, 1, 2, 5])
		self.assertEqual(seq[:], [0, 1, 2, 5])
		self.assertEqual(seq[:2], [0, 1])
		self.assertEqual(seq[2:], [2, 5])
		self.assertEqual(seq[50:], [])
		self.assertEqual(seq[-2:], [2, 5])
		self.assertEqual(seq[-100:42], [0, 1, 2, 5])
		self.assertEqual(seq[2:1], [])
		self.assertEqual(seq[1::2], [1, 5])
	def _testSetSlice(self, seq, refseq):
		seq[1:3] = [10, 11, 12]
		self.assertEqual(refseq[:], [0, 10, 11, 12, 5])
		seq[:] = []
		self.assertEqual(refseq[:], [])
		seq[:] = range(10)
		self.assertEqual(refseq[:], list(range(10)))
		def set_bad_slice(seq):
			seq[2:9:2] = [1, 2]
		self.assertRaises(ValueError, set_bad_slice, seq)
		seq[2:8:2] = [20, 40, 60]
		self.assertEqual(refseq[:], [0, 1, 20, 3, 40, 5, 60, 7, 8, 9])
		del seq[3:9:2]
		self.assertEqual(refseq[:], [0, 1, 20, 40, 60, 8, 9])
		self.assertRaises(TypeError, seq.__setitem__, slice(2, 8, 2), ["20", "40", "60"])
		seq[2:5] = seq
		self.assertEqual(refseq[:], [0, 1, 0, 1, 20, 40, 60, 8, 9, 8, 9])
		del seq[:2]
		del seq[-2:]
		self.assertEqual(refseq[:], [0, 1, 20, 40, 60, 8, 9])
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
		self._testSequence(bar.writeableVector, bar.writeableVector)
		self.assertRaises(TypeError, setattr, bar, "writeableVector", 123)
		self.assertRaises(TypeError, setattr, bar, "writeableVector", {1:2, 3:4})
		self.assertRaises(TypeError, setattr, bar, "writeableVector", ["123"])
	def testWriteableList(self):
		bar = embedding.Bar()
		self._testSequence(bar.writeableList, bar.writeableList)
	def testWriteableDeque(self):
		bar = embedding.Bar()
		self._testSequence(bar.writeableDeque, bar.writeableDeque)
	def testWriteableStaticVector(self):
		bar = embedding.Bar()
		self._testSequence(bar.writeableStaticVector, bar.writeableStaticVector)
		

barC.writeableVector = range(3)

	
# testing documentation
echo("Module documentation :\n ",embedding.__doc__)
echo("Class documentation :\n ",embedding.Bar.__doc__)

#print "dir of embedding.Bar : \n" , dir(embedding.Bar)
#print "__dict__ of Bar object :\n",embedding.Bar.__dict__

# querying the module
echo("dir of bar module:\n",dir(embedding))

# object construction
test = embedding.Bar()
echo("test.int = ",test.myInt)

echo("\n***\n")

# object query
echo("dir of Bar object returned :\n ",dir(test))
echo("dir of embedding.Bar : \n" , dir(embedding.Bar))
echo("__dict__ of Bar object :\n",embedding.Bar.__dict__)



# the __members__ attribute is not (yet) supported
#print "members of Bar object : ",test.__members__

echo("\n***\n")

# representation of a bar object
echo(test)
echo(str(test))

# instantiation of methods/attributes
echo(test.aMoreComplexFunction(3.0,5.0))
echo("aStaticMethod : ",embedding.Bar.aStaticMethod(5))
echo("aStaticMethod : ",test.aStaticMethod(5))

#test.stringAttribute = "Written from within Python"

echo("class name of test",test.__class__)
echo("class type of Bar is ", embedding.Bar.__class__)
echo("class name of test is ", embedding.Bar.__class__.__name__)
echo("dir of class type of test is ", dir(embedding.Bar.__class__))

echo("\n***\n")

echo(test.testAutomaticFunctionExport(5,6.0))
echo(embedding.Bar.aStaticMethod(5))

echo("\n***\n")

exceptionCaught = 0
test.myInt = 5

#echo("\n* Test automatic and cool setter/getter accessors")
#try:
#    echo("cool member", test.cool)
#    test.cool = 6
#    echo("cool member", test.cool)
#except:
#    reportError("cool setter/getter accessors not supported")
#echo("\n")

class TestTypeSafety(unittest.TestCase):
	def testTypeSafety(self):
		self.assertRaises(TypeError, embedding.Bar.aStaticMethod, "crash here")
		def setMember(bar, x):
			bar = embedding.Bar()
			bar.int = x
		self.assertRaises(TypeError, setMember, "bogus")

import gc



echo("\n***\n")

temp = dir(test)

# more complex arguments
test.complexArguments('a string')
box = ((10, 20, 30), (100, 200, 300))
matrix = ((1, 2, 3, 4), [5, 6, 7, 8], (9, 10, 11, 12), (13, 14, 15, 16))
echo(test.primArguments(box, "y", matrix))

# testing custom built objects as arguments

footest = embedding.PythonFoo(6, "world!")

echo("foo member of bar = ", test.foo)
test.foo = footest
echo("foo member of bar = ", test.foo)


test.publicInt = 1
echo("publicint of bar object",test.publicInt)

# testing overloaded functions
# print test.complexArguments('a string')
echo(test.tester('a string'))
#print test.primArguments(box, "y", matrix)
echo(test.tester(box, "y", matrix))

class TestSignatures(unittest.TestCase):
	def testCString(self):
		self.assertEqual(embedding.testCStringSupport("Hello World!"), "Hello World!")

echo("\n* Testing free methods")
try:
	test.freeMethodA("been here")
except:
	reportError("Bar seems to have troubles with freeMethodA")
try:
	test.freeMethodB("done that")
except:
	reportError("Bar seems to have troubles with freeMethodB")
echo("\n")


class TestOperators(unittest.TestCase):
	def testCallOperator(self):
		test = embedding.Bar()
		self.assertEqual(test(4), 8)
		self.assertEqual(test("hallo"), "HALLO")
		self.assertEqual(test(3.5), 1.75)
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
		e = c ** 2
		self.assertEqual(e.value, 1)
		self.assertEqual(e.period, 3)
		f = c ** 3
		self.assertEqual(f.value, 2)
		self.assertEqual(f.period, 3)
		g = pow(c, 3, 10)
		self.assertEqual(g.value, 8)
		self.assertEqual(g.period, 10)

class TestStaticMembers(unittest.TestCase):
	def setBarCONST(self, x):
		embedding.Bar.CONST = x
	def testClassConstants(self):
		self.assertEqual(embedding.Bar.CONST, 5)
		self.assertRaises(TypeError, self.setBarCONST, 6)
	def testInnerClasses(self):
		innerClass = embedding.Bar.InnerClass("the answer is 42")
		self.assertEqual(innerClass.talkTo("Arthur"), "Arthur, the answer is 42.\n")
		self.assertEqual(inspect.getdoc(embedding.Bar.InnerClass), "InnerClass of Bar")
	def testStaticMethods(self):
		self.assertEqual(embedding.Bar.aStaticMethod(3.14), 3)

echo("\n* Testing qualified and overloaded functions")
embedding.overloaded(5)
embedding.overloaded('hello!')
embedding.overloaded(1 + 2j)
echo("\n")

class TestSpecialFunctionsAndOperators(unittest.TestCase):
	def testSequenceProtocol(self):
		c = embedding.ClassB()
		self.assertEqual(len(c), 5)
		self.assertEqual(c[1], 1)


echo("\n* Testing qualified and overloaded methods")
test.overloaded(5)
test.overloaded('hello!')
echo("\n")

class TestTuples(unittest.TestCase):
	def assertTupleAlmostEqual(self, first, second, places=7, msg=None):
		msg = msg or '%r != %r within %r places' % (first, second, places)
		self.assertEqual(len(first), len(second), msg)
		for a, b in zip(first, second):
			self.assertAlmostEqual(a, b, places=places, msg=msg)
	def testVariableLength(self):
		bar = embedding.Bar()
		self.assertTupleAlmostEqual(bar.rgba((0.1, 0.2, 0.3)), (0.1, 0.2, 0.3, 1.0))
		self.assertTupleAlmostEqual(bar.rgba([0.1, 0.2, 0.3, 0.4]), (0.1, 0.2, 0.3, 0.4))
		self.assertRaises(TypeError, bar.rgba, [])
		self.assertRaises(TypeError, bar.rgba, (0.1, 0.2))
		self.assertRaises(TypeError, bar.rgba, (0.1, 0.2, 0.3, 0.4, 0.5))

class TestConstructors(unittest.TestCase):
	def testOverloadedConstructors(self):
		barA = embedding.Bar()
		self.assertEqual(barA.myInt, 0)
		barB = embedding.Bar(5, "hello")
		self.assertEqual(barB.myInt, 5)
	def testFreeConstructors(self):
		classB = embedding.ClassB(5)

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
		self.assertTrue(not embedding.spamToCppByCopy(spam, spam))
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
			d = {'a':1}
			return d['b'] # raises a KeyError
		self.assertRaises(KeyError, embedding.call0, callback)
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
		self.assertFalse(a == None)
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
		self.assertTrue(a != None)
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
		import sys
		try:
			from StringIO import StringIO
		except ImportError:
			from io import StringIO
		old = sys.stdout
		try:
			sys.stdout = StringIO()
			embedding.writeStdout("Hello\n")
			embedding.writeStdout("World\n")
			self.assertEqual(sys.stdout.getvalue(), "Hello\nWorld\n")
		finally:
			sys.stdout = old


class MyFloat:
	def __init__(self, value):
		self.value = value
	def __float__(self):
		return float(self.value)
	

skipIfNoU8string = unittest.skipIf(
	not getattr(embedding, "LASS_HAVE_STD_U8STRING", False), 
	"std::u8string not supported"
)

class TestTypes(unittest.TestCase):
	def testStdString(self):
		self.assertEqual(embedding.testStdString(""), "")
		self.assertEqual(embedding.testStdString("abc"), "abc")
		self.assertEqual(embedding.testStdString("𐌷𐌴𐌻𐌻𐍉"), "𐌷𐌴𐌻𐌻𐍉")
		self.assertEqual(embedding.testStdString("\0"), "\0")
		self.assertEqual(embedding.testStdString("a\0b"), "a\0b")

	def testStdWstring(self):
		self.assertEqual(embedding.testStdWstring(u""), u"")
		self.assertEqual(embedding.testStdWstring(u"abc"), u"abc")
		self.assertEqual(embedding.testStdWstring("𐌷𐌴𐌻𐌻𐍉"), "𐌷𐌴𐌻𐌻𐍉")
		self.assertEqual(embedding.testStdWstring(u"\0"), u"\0")
		self.assertEqual(embedding.testStdWstring(u"a\0b"), u"a\0b")

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
			self.assertRaises(TypeError, embedding.testFloatSingle, inf)
			self.assertRaises(TypeError, embedding.testFloatSingle, -inf)
			self.assertRaises(TypeError, embedding.testFloatSingle, 1e40)
			self.assertRaises(TypeError, embedding.testFloatSingle, 1e400)
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
		self.assertEqual(embedding.testFloatSingle(1234567890), 1234567936.0) # !!!
		self.assertEqual(float(12345678901234567890), 1.2345678901234567e+19)
		self.assertEqual(embedding.testFloatSingle(12345678901234567890), 1.2345679395506094e+19) # !!!
		self.assertRaises(OverflowError, float, 2**1024)
		if useOldExportTraits:
			self.assertRaises(TypeError, embedding.testFloatSingle, 2**1024)  # so not inf...
		else:
			self.assertRaises(OverflowError, embedding.testFloatSingle, 2**1024)  # so not inf...

		if useOldExportTraits:
			self.assertRaises(TypeError, embedding.testFloatSingle, MyFloat(1.5))
		else:
			# custom objects should be converted to float first
			self.assertEqual(embedding.testFloatSingle(MyFloat(1.5)), 1.5)
			self.assertEqual(embedding.testFloatSingle(MyFloat("inf")), inf)
			self.assertEqual(embedding.testFloatSingle(MyFloat(1e40)), inf)
			self.assertTrue(math.isnan(embedding.testFloatSingle(MyFloat("nan"))))
			self.assertRaises(OverflowError, float, MyFloat(2**1024))

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
		self.assertEqual(float(12345678901234567890), 1.2345678901234567e+19)
		self.assertEqual(embedding.testFloatDouble(12345678901234567890), 1.2345678901234567e+19)
		self.assertRaises(OverflowError, float, 2**1024)
		if useOldExportTraits:
			self.assertRaises(TypeError, embedding.testFloatDouble, 2**1024)  # so not inf...
		else:
			self.assertRaises(OverflowError, embedding.testFloatDouble, 2**1024)  # so not inf...

		if useOldExportTraits:
			self.assertRaises(TypeError, embedding.testFloatDouble, MyFloat(1.5))
		else:
			# custom objects should be converted to float first
			self.assertEqual(embedding.testFloatDouble(MyFloat(1.5)), 1.5)
			self.assertEqual(embedding.testFloatDouble(MyFloat("inf")), inf)
			self.assertEqual(embedding.testFloatDouble(MyFloat(1e400)), inf)
			self.assertTrue(math.isnan(embedding.testFloatDouble(MyFloat("nan"))))
			self.assertRaises(OverflowError, float, MyFloat(2**1024))

class TestBar(unittest.TestCase):
	def testMapProtocol(self):
		bar = embedding.Bar()
		self.assertEqual(len(bar), 0)
		self.assertRaises(KeyError, bar.__getitem__, "foo")
		self.assertRaises(TypeError, bar.__getitem__, 1) # should become keyerror?
		bar["foo"] = "spam"
		bar["baz"] = 42
		self.assertEqual(len(bar), 2)
		self.assertEqual(bar["foo"], "spam")
		self.assertEqual(bar["baz"], "42")
		self.assertTrue("foo" in bar)
		self.assertTrue("baz" in bar)
		self.assertFalse("buzz" in bar)
		self.assertRaises(KeyError, bar.__delitem__, "buzz")
		self.assertRaises(IndexError, bar.__delitem__, 42)
		self.assertRaises(TypeError, bar.__delitem__, (1,2)) # should become KeyError?
		del bar["foo"]
		self.assertEqual(len(bar), 1)
		self.assertFalse("foo" in bar)


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
		self.assertEqual(inspect.getdoc(embedding.InjectedClass), "Class injected into module")



import sys
test = unittest.defaultTestLoader.loadTestsFromModule(sys.modules[__name__])
testRunner = unittest.TextTestRunner(verbosity = 2)
result = testRunner.run(test)
assert(result.wasSuccessful())
