import embedding
#import code
#s = code.InteractiveConsole()
#s.interact()


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
    print "AUTOMATIC SETTER/GETTER ACCESSORS NOT SUPPORTED"
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
print test.primArguments(box, "y")

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
#print test.primArguments(box, "y")
print test.tester(box, "y")


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