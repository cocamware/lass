import embedding2
print dir(embedding2)
print dir(embedding2.Foo)

f = embedding2.Foo()
f = embedding2.Foo(1)
f = embedding2.Foo(1,2)
f.publicMember = 3
print f.publicMember
print f.getMember()
f.setMember(5)
print f.accessorMember
f.accessorMember = 7

import sys
try:
	f.readOnlyPublicMember = 5
except:
	print sys.exc_info()

print f.overloaded(7,6)
print f.overloaded(7,6,5)
print f.overloaded(7,6,5,4)

print "Press enter to continue"
raw_input()