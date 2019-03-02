import sys
import unittest
def echo(*xs):
	sys.stdout.write(' '.join(map(str, xs)) + '\n')

echo(sys.version)
IS_PYTHON_3 = sys.version_info[0] >= 3


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
		self.assertRaises(TypeError, mapping.__delitem__, 123)
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
echo("test.int = ",test.int)

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
test.int = 5

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
		self.assertEqual(barA.int, 0)
		barB = embedding.Bar(5, "hello")
		self.assertEqual(barB.int, 5)
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


class TestTypes(unittest.TestCase):
	def testStdString(self):
		self.assertEqual(embedding.testStdString(""), "")
		self.assertEqual(embedding.testStdString("abc"), "abc")
		self.assertEqual(embedding.testStdString("\0"), "\0")
		self.assertEqual(embedding.testStdString("a\0b"), "a\0b")

	def testStdWstring(self):
		self.assertEqual(embedding.testStdWstring(u""), u"")
		self.assertEqual(embedding.testStdWstring(u"abc"), u"abc")
		self.assertEqual(embedding.testStdWstring(u"\0"), u"\0")
		self.assertEqual(embedding.testStdWstring(u"a\0b"), u"a\0b")


import sys
test = unittest.defaultTestLoader.loadTestsFromModule(sys.modules[__name__])
testRunner = unittest.TextTestRunner(verbosity = 2)
result = testRunner.run(test)
assert(result.wasSuccessful())
