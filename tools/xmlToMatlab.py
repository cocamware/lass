#############################################
#
# Lass Geometric XML to Matlab convertor
# Author : Tom De Muer
#
# Released under the Lass License
#
#############################################

from xml.sax import make_parser, ContentHandler, SAXException, SAXReaderNotAvailable, SAXParseException
try:
    make_parser()
except SAXReaderNotAvailable:
    raise ImportError("no XML parsers available")

import xml
import os
import math
from test.test_support import findfile


pointStructs = ['Point2D','Point3D','Point2DH','Point3DH']
vectorStructs = ['Vector2D','Vector3D']
polygonStructs = ['SimplePolygon2D','SimplePolygon3D','Triangle2D','Triangle3D']

x = 0
y = 1
z = 2
fill = 0

tesselateX = 8
tesselateY = 8

def translate(p,t):
	return map(lambda x,y:x+y,p,t)

def scale(p,s):
	return map(lambda x:x*s,p)


def tesselateCircle(scale=1.0):
	ps=[]
	for i in range(tesselateX):
		phi = 2.0*math.pi*(float(i)+0.5)/float(tesselateX)
		ps.append( [scale*math.cos(phi),scale*math.sin(phi)] )
	return ps

def tesselateUnitSphere():
	ps=[]
	cs=[]
	for i in range(tesselateY):
		x=2.0*((float(i)+0.5)/float(tesselateY)-0.5)
		c = tesselateCircle(math.sin(math.acos(x)))
		for p in c:
			p.append(x)
		cs.append(c)
	for i in range(1,tesselateY):
		cU = cs[i-1]
		cL = cs[i]
		for j in range(tesselateX):
			p=[ cL[j-1],cL[j],cU[j],cU[j-1] ]
			ps.append(p)
	ps.append(cs[0])
	ps.append(cs[len(cs)-1])
	return ps	# a list of polygons
		
	
def tesselateSphere(center,radius):
	ps=tesselateUnitSphere()
	tps=[]
	for p in ps:
		poly=[]
		for point in p:
			sp=scale(point,radius)
			sp=translate(sp,center)
			poly.append(sp)
		tps.append(poly)
	return tps
			
			

def transpose(aoarray):
	nrArrays = len(aoarray)
	lorArray = len(aoarray[0])
	r = []
	for i in range(lorArray):
		t=nrArrays*[0]
		r.append(t)
	for i in range(lorArray):
		for j in range(nrArrays):
			r[i][j]=aoarray[j][i]
	return r


def bracketLessStr(array):
	s=''
	for a in array:
		s+=','+str(a)
	return s[1:]


def writeMatlabPoint(data):
	print 'line(',bracketLessStr(data),",'marker','o');"


def writeMatlabLineSegment(data):
	print 'line(',bracketLessStr(transpose([data[0],data[1]])),');'


def writeMatlabSimplePolygon(data):
	if fill:
		print 'patch(',bracketLessStr(transpose(data)),');'
	else:
		data.append(data[0])	# close the polygon
		print 'line(',bracketLessStr(transpose(data)),');'


def writeMatlabAabb2D(data):
	m = data[0]
	M = data[1]
	p = [ [m[x],m[y]],[M[x],m[y]],[M[x],M[y]],[m[x],M[y]] ]
	writeMatlabSimplePolygon(p)


def writeMatlabAabb3D(data):
	m = data[0]
	M = data[1]
	ps = []
	ps.append( [ [m[x],m[y],M[z]],[M[x],m[y],M[z]],[M[x],M[y],M[z]],[m[x],M[y],M[z]] ] )
	ps.append( [ [m[x],m[y],m[z]],[m[x],M[y],m[z]],[M[x],M[y],m[z]],[M[x],m[y],m[z]] ] )

	ps.append( [ [m[x],m[y],m[z]],[M[x],m[y],m[z]],[M[x],m[y],M[z]],[m[x],m[y],M[z]] ] )
	ps.append( [ [m[x],M[y],m[z]],[m[x],M[y],M[z]],[M[x],M[y],M[z]],[M[x],M[y],m[z]] ] )

	ps.append( [ [m[x],m[y],m[z]],[m[x],m[y],M[z]],[m[x],M[y],M[z]],[m[x],M[y],m[z]] ] )
	ps.append( [ [M[x],m[y],m[z]],[M[x],M[y],m[z]],[M[x],M[y],M[z]],[M[x],m[y],M[z]] ] )
	
	for p in ps:
		writeMatlabSimplePolygon(p)


def writeMatlabSphere(data):
	ps=tesselateSphere(data[0],data[1])
	for p in ps:
		writeMatlabSimplePolygon(p)


def writeMatlab(type,data):
	if type=='Aabb2D':
		writeMatlabAabb2D(data)
	if type=='Aabb3D':
		writeMatlabAabb3D(data)
	if type=='Point':
		writeMatlabPoint(data)
	if type=='LineSegment':
		writeMatlabLineSegment(data)
	if type=='SimplePolygon':
		writeMatlabSimplePolygon(data)
	if type=='Sphere':
		writeMatlabSphere(data)


def parseCh(tag,ch):
	if tag in ['Point2D','Point3D','Vector2D','Vector3D']:
		return tuple(map(float,ch.split()))
	if tag=='Point2DH' or tag=='Point3DH':
		temp = map(float,ch.split())
		dim = len(temp)-1
		if dim==2:
			return (temp[0]/temp[dim],temp[1]/temp[dim])
		else:
			return (temp[0]/temp[dim],temp[1]/temp[dim],temp[2]/temp[dim])


class CoordinateHandler(ContentHandler):
	def __init__(self):
		self.name = None
		self.point = None
		self.vector = None
	def startElement(self, name, attrs):
		self.name = name
	def endElement(self, name): 
		if self.point!=None:
			writeMatlab('Point',self.point)
		if self.vector!=None:
			writeMatlab('Vector',self.vector)
	def characters (self, ch): 
		if self.name in pointStructs:
			self.point = parseCh(self.name,ch)
		if self.name in vectorStructs:
			self.vector = parseCh(self.name,ch)

class AabbHandler(ContentHandler):
	def __init__(self):
		self.min = None
		self.max = None
	def startElement(self, name, attrs):
		self.lastTag=name
	def endElement(self, name):
		if name==self.lastTag:
			self.lastTag=None
		if name=='min':
			self.min=self.lastPoint
		if name=='max':
			self.max=self.lastPoint
		if name in ['Aabb2D','Aabb3D']:
			writeMatlab(name,(self.min,self.max))
	def characters (self, ch):
		if self.lastTag in pointStructs:
			self.lastPoint=parseCh(self.lastTag,ch)


class LineHandler(ContentHandler):
	def __init__(self):
		pass


class LineSegmentHandler(ContentHandler):
	def __init__(self):
		self.tail = None
		self.head = None
	def startElement(self, name, attrs):
		self.lastTag=name
	def endElement(self, name):
		if name==self.lastTag:
			self.lastTag=None
		if name=='tail':
			self.tail=self.lastPoint
		if name=='head':
			self.head=self.lastPoint
		if name in ['LineSegment2D','LineSegment3D']:
			writeMatlab('LineSegment',(self.tail,self.head))
	def characters (self, ch):
		if self.lastTag in pointStructs:
			self.lastPoint=parseCh(self.lastTag,ch)

class SphereHandler(ContentHandler):
	def __init__(self):
		self.center = None
		self.radius = None
	def startElement(self, name, attrs):
		self.lastTag=name
	def endElement(self, name):
		if name==self.lastTag:
			self.lastTag=None
		if name=='center':
			self.center=self.lastPoint
		if name=='radius':
			self.radius=self.lastValue
		if name in ['Sphere3D']:
			writeMatlab('Sphere',(self.center,self.radius))
	def characters(self, ch):
		if self.lastTag in pointStructs:
			self.lastPoint=parseCh(self.lastTag,ch)
		if self.lastTag=='radius':
			self.lastValue=float(ch)


class SimplePolygonHandler(ContentHandler):
	def __init__(self):
		self.vertices = []
	def startElement(self, name, attrs):
		self.lastTag=name
	def endElement(self, name):
		if name==self.lastTag:
			self.lastTag=None
		if name=='vertex':
			self.vertices.append(self.lastPoint)
		if name in polygonStructs:
			writeMatlab('SimplePolygon',(self.vertices))
	def characters (self, ch):
		if self.lastTag in pointStructs:
			self.lastPoint=parseCh(self.lastTag,ch)

class RayHandler(ContentHandler):
	def __init__(self):
		pass


class GeometryHandler(ContentHandler):
	def __init__(self):
		self.handler = None
		self.handlerName = None
	def startElement(self, name, attrs):
		if self.handler==None:
			if name in polygonStructs:
				self.handler=SimplePolygonHandler()
				self.handlerName=name
			if name in ['LineSegment2D','LineSegment3D']:
				self.handler=LineSegmentHandler()
				self.handlerName=name
			if name in ['Ray2D','Ray3D']:
				self.handler=RayHandler()
				self.handlerName=name
			if name in ['Line2D','Line3D']:
				self.handler=LineHandler()
				self.handlerName=name
			if name in pointStructs or name in vectorStructs:
				self.handler=CoordinateHandler()
				self.handlerName=name
			if name in ['Aabb2D','Aabb3D']:
				self.handler=AabbHandler()
				self.handlerName=name
			if name in ['Sphere3D']:
				self.handler=SphereHandler()
				self.handlerName=name
		if self.handler!=None:
			self.handler.startElement(name,attrs)
	def endElement(self,name): 
		if self.handler!=None:
			self.handler.endElement(name)
		if self.handlerName==name:
			self.handler=None
	def characters (self, ch): 
		if self.handler!=None:
			self.handler.characters(ch)
		

def convertToMatlab(xmlFile):
	parser = xml.sax.make_parser()
	geoHandler = GeometryHandler()
	parser.setContentHandler(geoHandler)
	parser.parse(open(findfile(xmlFile)))

print "% Matlab code generated with LASS xml converter"

import sys

argc = len(sys.argv)

if argc==2:
	convertToMatlab(sys.argv[1])
