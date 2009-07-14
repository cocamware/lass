import sys
sys.path.append(".")

import embedding3

print dir(embedding3)
print dir(embedding3.Foo)

c = embedding3.factoryObject(3)
print type(c)
b = embedding3.factoryObject(2)
print type(b)
a = embedding3.factoryObject(1)
print type(a)

"""
Output should be:

<type 'embedding3.FooBarred'>
<type 'embedding3.Bar'>
<type 'embedding3.Foo'>
"""

print "Press enter to continue"
raw_input()