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
#echo("values=",barC.writeableMap)
#echo("values=",barC.writeableMap.values())

echo("Testing print functionality", barC)

#class SubType(embedding.Bar):
#    def __init__(self):
#        pass
#
#class DerivedBar(SubType):
#    def __init__(self):
#        self.test = 5
#        print "test ",self.test
#    def doSomething(self):
#        print "did something"
#print dir(embedding.Bar)
#b = DerivedBar()
#print type(b)
#print dir(b)
#b.doSomething()
#print b.test




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


echo("Testing derived classes behavior")
e = embedding.Bar()
echo(e.__class__)
echo(e.aMoreComplexFunction(1,2))
assert(e.aMoreComplexFunction(1,2)==3)
e = embedding.DerivedBar()
echo(e.__class__)
assert str(e.__class__)=="<%s 'embedding.DerivedBar'>" % ('type', 'class')[IS_PYTHON_3], str(e.__class__)
e = embedding.testPolymorphism()
echo(e.aMoreComplexFunction(1,2))
assert(e.aMoreComplexFunction(1,2)==2)


echo("\n***\n")

barC = embedding.Bar()
#barC.writeableMap['test'] = 'ok'
#echo("barC['test']=",barC.writeableMap['test'])
#if barC.writeableMap['test'] != 'ok':
#    reportError("Map is not writeable")
#else:
#    echo("Succesfully wrote to map")
#echo(barC.writeableMap)		# test the repr and str
#del barC.writeableMap['test']
#exceptionCaught = 1
#try:
#	echo(barC.writeableMap['test'])	# this should fail, because of the previous del
#	exceptionCaught = 0
#except:
#	pass
#if not exceptionCaught:
#	reportError("Could not remove item from map")
#echo(barC.writeableMap)
#for i in range(3):
#	barC.writeableMap[str(i)] = str(i+1)
#echo("values=",barC.writeableMap.values())
#echo("keys=",barC.writeableMap.keys())
#
# subsequent test fails, probably need a partial sequence-protocol in the mapping
# protocol embedded
# for x in barC.writeableMap:
	# echo("map iterator",x)

try:
    barC.constMap['test'] = 'notok'
    reportError("Could write to const map")
except:
    pass
	
class TestConstSequence(unittest.TestCase):
	def _testConstSequence(self, seq):
		def assign(a, b):
			a[:] = b
		def set_item(a, i, b):
			a[i] = b
		self.assertRaises(TypeError, assign, seq, range(10))
		self.assertRaises(TypeError, set_item, seq, 3, 5)
		self.assertRaises(TypeError, lambda a: a.clear(), seq)
		self.assertRaises(TypeError, lambda a, n: a.reserve(n), seq, 5)
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
	def _testAppend(self, seq, refseq):
		for i in range(10):
			seq.append(i)
		self.assertEqual(len(refseq), 10)
		for i in range(10):
			self.assertEqual(refseq[i], i)
	def _testPop(self, seq, refseq):
		self.assertEqual(seq.pop(), 9)
		self.assertEqual(len(refseq), 9)
		for i in range(5):
			self.assertEqual(seq.pop(3), 3 + i)
		self.assertEqual(len(refseq), 4)
		for a, b in zip(refseq, [0, 1, 2, 8]):
			self.assertEqual(a, b)
	def _testContains(self, seq, refseq):
		self.assert_(8 in seq)
		self.failIf(-123456 in seq)
	def _testGetItem(self, seq, refseq):
		n = len(seq)
		self.assertRaises(IndexError, lambda a, i: a[i], seq, n)
		self.assertRaises(IndexError, lambda a, i: a[i], seq, -n-1)
		self.assertEqual(seq[n - 1], refseq[-1])
		self.assertEqual(seq[-n], refseq[0])
	def _testSetItem(self, seq, refseq):
		seq[3] = 5
		self.assertEqual(refseq[3], 5)
	def _testGetSlice(self, seq, refseq):
		self.assertEqual(seq[:], [0, 1, 2, 5])
		self.assertEqual(seq[:2], [0, 1])
		self.assertEqual(seq[2:], [2, 5])
		self.assertEqual(seq[50:], [])
		self.assertEqual(seq[-2:], [2, 5])
		self.assertEqual(seq[-100:42], [0, 1, 2, 5])
		self.assertEqual(seq[2:1], [])
	def _testSetSlice(self, seq, refseq):
		seq[1:3] = [10, 11, 12]
		self.assertEqual(seq[:], [0, 10, 11, 12, 5])
		seq[:] = []
		self.assertEqual(seq[:], [])
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
		self._testAppend(seq, refseq)
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
	def testWriteableList(self):
		bar = embedding.Bar()
		self._testSequence(bar.writeableList, bar.writeableList)
	def testWriteableDeque(self):
		bar = embedding.Bar()
		self._testSequence(bar.writeableDeque, bar.writeableDeque)
		
def testSequence(seq):


	if not IS_PYTHON_3:
		seq[0:4] = range(10)
		assert(len(seq)==4*l+6)
		for i,v in enumerate(seq):
			echo (i,v)


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



echo("\n* Testing __call__ operator")
try:
	result = test(4)
	echo(result)
	assert(result == 8)
except:
	reportError("Bar seems to have troubles with __call__")
try:
	result = test("hallo")
	echo(result)
	assert(result == "HALLO")
except:
	reportError("Bar seems to have troubles with __call__")
try:
	result = test(3.5)
	echo(result)
	assert(result == 1.75)
except:
	reportError("Bar seems to have troubles with __call__")
echo("\n")

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

if 0:
	echo("\n* Testing special functions and operator overloads")
	echo("Sequence protocol testing")
	echo("Testing len function")
	echo(len(c))
	echo("been here")
	assert(c[1]==1)
	echo("been here")
	assert(len(c)==5)
	echo("been here")
	c[2]=(3.0,5)
	echo("been here")
	c[2]=(70,5)
	echo("been here")
	try:
		# this overload should not be accepted
		c[2]=(70,5,2)
		assert(False)
	except:
		pass
	echo("been here")

	echo("\n* Full Sequence and Iterator protocol testing")
	c = embedding.ClassSeq()
	testSequence(c)
	c.clear()
	[c.append(i) for i in range(10)]
	assert(len(c)==10)
	for v in c:
		echo(v)

	echo("\n* Full Map and Iterator protocol testing")
	c = embedding.ClassMap()
	c["test"] = 1
	c["test2"] = 2
	for v in c:
		echo(v)

	
echo("\n* Testing qualified and overloaded methods")
test.overloaded(5)
test.overloaded('hello!')
echo("\n")

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
		self.assert_(not embedding.spamToCppByCopy(spam, spam))
		self.assert_(embedding.spamToCppByConstReference(spam, spam))
		self.assert_(embedding.spamToCppByReference(spam, spam))

class TestCallbacks(unittest.TestCase):
	def setUp(self):
		self.isCalled = False
		self.arg = None
	def testCallback0(self):
		def callback():
			self.isCalled = True
		embedding.call0(callback)
		self.assert_(self.isCalled)
	def testCallback1(self):
		hi = "hi!"
		def callback(x):
			self.isCalled = True
			self.arg = x
		embedding.call1(callback, hi)
		self.assert_(self.isCalled)
		self.assertEqual(self.arg, hi)
	def testCallbackR0(self):
		hi = "hi!"
		def callback():
			self.isCalled = True
			return hi
		result = embedding.callR0(callback)
		self.assert_(self.isCalled)
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
		self.assert_(self.isCalled)
		
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


import sys
test = unittest.defaultTestLoader.loadTestsFromModule(sys.modules[__name__])
testRunner = unittest.TextTestRunner(verbosity = 2)
result = testRunner.run(test)
assert(result.wasSuccessful())
