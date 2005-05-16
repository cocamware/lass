import sys
import embedding

if 1:
	import code
	s = code.InteractiveConsole()
	s.interact()

numberOfErrors = 0

def reportError(description):
    global numberOfErrors
    numberOfErrors += 1
    if sys.exc_info()[0]:
        print "ERROR: %s: %s" % (description, sys.exc_info()[0])
    else:
        print "ERROR: %s" % description
    sys.exc_clear()

class TestClass:
    def __init__(self):
        self.member1 = 0
    def staticMethod(a,b):
        return a+b
    staticMethod = staticmethod(staticMethod)

    def normalFunction1(self,a):
        return a
    def normalFunction2(self,b):
        return a
    
#print dir( TestClass() )
#print embedding.listInfo( TestClass() )
#print TestClass().__class__

print "\n***\n"

# testing documentation
print "Module documentation :\n ",embedding.__doc__
print "Class documentation :\n ",embedding.Bar.__doc__

#print "dir of embedding.Bar : \n" , dir(embedding.Bar)
#print "__dict__ of Bar object :\n",embedding.Bar.__dict__

# querying the module
print "dir of bar module:\n",dir(embedding)

# object construction
test = embedding.Bar()
print "test.int = ",test.int 

print "\n***\n"

# object query
print "dir of Bar object returned :\n ",dir(test)
print "dir of embedding.Bar : \n" , dir(embedding.Bar)
print "__dict__ of Bar object :\n",embedding.Bar.__dict__



# the __members__ attribute is not (yet) supported
#print "members of Bar object : ",test.__members__

print "\n***\n"

# representation of a bar object
print test
print str(test)

# instantiation of methods/attributes
print test.aMoreComplexFunction(3.0,5.0)
print "aStaticMethod : ",embedding.Bar.aStaticMethod(5)
print "aStaticMethod : ",test.aStaticMethod(5)

#test.stringAttribute = "Written from within Python"

print "class name of test",test.__class__
print "class type of Bar is ", embedding.Bar.__class__
print "class name of test is ", embedding.Bar.__class__.__name__
print "dir of class type of test is ", dir(embedding.Bar.__class__)

print "\n***\n"

print test.testAutomaticFunctionExport(5,6.0)
print embedding.Bar.aStaticMethod(5)

print "\n***\n"

exceptionCaught = 0
test.int = 5

print "\n* Test automatic and cool setter/getter accessors"
try:
    print "cool member", test.cool
    test.cool = 6
    print "cool member", test.cool
except:
    reportError("cool setter/getter accessors not supported")
print "\n"

try:
    embedding.Bar.aStaticMethod("crash here")
    test.int = "bogus"
except:
    print 
    exceptionCaught = 1
    import sys
    print "Exception caught was"
    print sys.exc_type, sys.exc_value

if not exceptionCaught:
    raise exception("Test Failed")

import gc

# testing reference counting
for i in xrange(100):
    test = embedding.Bar()
    gc.collect()

print "\n***\n"

temp = dir(test)

# more complex arguments
test.complexArguments('a string')
box = ((10, 20, 30), (100, 200, 300))
matrix = ((1, 2, 3, 4), [5, 6, 7, 8], (9, 10, 11, 12), (13, 14, 15, 16))
print test.primArguments(box, "y", matrix)

# testing custom built objects as arguments

footest = embedding.PythonFoo(6, "world!")

print "foo member of bar = ", test.foo
test.foo = footest
print "foo member of bar = ", test.foo


test.publicInt = 1
print "publicint of bar object",test.publicInt

# testing overloaded functions
# print test.complexArguments('a string')
print test.tester('a string')
#print test.primArguments(box, "y", matrix)
print test.tester(box, "y", matrix)



print "\n* Testing class constants"
try:
    print "Bar.CONST:\n", embedding.Bar.CONST
except:
    reportError("Bar seems to have troubles with CONST")
else:
    try:
        Bar.CONST = 6
    except:
        print "Can't change Bar.CONST, which is nice"
    else:
        reportError("Can change Bar.CONST, which I shouldn't be able to")
print "\n"



print "\n* Testing inner classes"
try:
    innerClass = embedding.Bar.InnerClass("the answer is 42")
except:
    reportError("Can't create innerclass")
else:
    try:
        sayings = innerClass.talkTo("Arthur")
    except:
        reportError("Couldn't talk to Arthur")
    print sayings
    if sayings != "Arthur, the answer is 42.\n":
        reportError("innerClass said the wrong things")
print "\n"



print "\n* Testing qualified and overloaded methods"
test.overloaded(5)
test.overloaded('hello!')
print "\n"



print "\n* Testing overloaded constructor"
barA = embedding.Bar()
print barA
barB = embedding.Bar(5, "hello")
print barB
print "\n"

print "\n* Testing shadow objects"
shadowB = embedding.Bacon()
print "shadowB.who():", shadowB.who()
shadowB2 = embedding.Ham()
print "shadowB2.who():", shadowB2.who()
shadowC = embedding.Eggs(3)
print "shadowC.who():", shadowC.who()
print "shadowC.number:", shadowC.number
shadowC.number = 4
print "shadowC.number:", shadowC.number
spam = embedding.makeSpam("Bacon")
print "spam.who():", spam.who()
print "spam.address:", spam.address
embedding.spamToCppByReference(spam)
embedding.spamToCppByConstReference(spam)
embedding.spamToCppByPointer(spam)
embedding.spamToCppByCopy(spam)
print "\n"

print "\n* Testing callbacks"
print "Callback0:"
def callback0():
	print "... is called succesfully"
embedding.call0(callback0)
print "Callback1:"
def callback1(iA):
	print iA
embedding.call1(callback1)
print "CallbackR0:"
def callbackR0():
	return "hi!"
embedding.callR0(callbackR0)
print "CallbackR2:"
embedding.callR2(lambda x, y: x * y)

print "\n"
