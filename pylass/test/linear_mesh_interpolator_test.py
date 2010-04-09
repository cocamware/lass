import lass

def loadFile(filename):
	f = open(filename)
	data = []
	for x in f.readlines():
		data.append(map(float,(x.split('\t'))))
	return data

boundingBox = ((0, 0), (100, 100))
valueOnBox = 0

interpolator = lass.LinearMeshInterpolator(boundingBox, valueOnBox)

positionA = (10, 10)
valueA = 100
interpolator.insertSite(positionA, valueA)

positionB = (20, 20)
valueB = 200
interpolator.insertSite(positionB, valueB)

positionC = (15, 15)

print positionA, interpolator.interpolate(positionA)
print positionB, interpolator.interpolate(positionB)
print positionC, interpolator.interpolate(positionC)
print boundingBox[0], interpolator.interpolate(boundingBox[0])
