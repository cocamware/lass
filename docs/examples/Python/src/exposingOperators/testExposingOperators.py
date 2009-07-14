import embedding5

print dir(embedding5.SpecialNumber)

n = embedding5.SpecialNumber(1)
m = embedding5.SpecialNumber(2)
print n+m
n += m
n += 7.0
c= n+3.0
print c

print "Press enter to continue"
raw_input()