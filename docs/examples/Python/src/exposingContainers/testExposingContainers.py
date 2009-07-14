import sys
sys.path.append(".")

import embedding4

print embedding4.aListOfFoos()
print embedding4.aConstListOfFoos()
print embedding4.aMapOfFoos()
print embedding4.doSum(range(10))

print "Press enter to continue"
raw_input()