import os.path
import re
import string

startDirectory = "d:/users/tom/internal_libraries/lassdev/lass/src"
fileTypes = ['*.h','*.cpp','*.inl']

res=[re.compile("\.h\Z"), re.compile("\.cpp\Z"), re.compile("\.inl\Z")]
headerBegin = re.compile('/\*\* @file')
headerEnd = re.compile('@author')
headerEnd2= re.compile('\*/')
lineEnd = re.compile('$')
newHeader = ''

f = open('../docs/license.txt','r')
newLines = f.readlines()
newHeader = string.join(newLines,'') +'\n'
newPartialHeader = string.join(newLines[:len(newLines)-1],'')+'\n'
f.close()

def processSpaces(filename):
	f = open(filename,'r')
	text = f.readlines()
	f.close()
	f = open(filename,'w')
	for l in text:
		if l[0]==' ':
			f.write(l[1:])
		else:
			f.write(l)
	f.close()
	
def processHeaders(filename):
	# clean up the old headers
	f = open(filename,'r')
	text = string.join(f.readlines(),'')
	f.close()
	hb = headerBegin.search(text)
	usePartial=0
	if hb!=None and hb.start()<5:
		usePartial=1
		hbt = hb.start()
		he = headerEnd.search(text)
		if he==None:
			he = headerEnd2.search(text)
			usePartial=0
			hbe = he.end()
		else:
			hbe = he.start()
			linee = text[he.start():].find('\n')
			if linee!=-1:
				hbe += linee
			else:
				raise "could not extract header"
	if usePartial:
		newText = newPartialHeader + text[hbe:]
	else:
		newText = newHeader+text
	f = open(filename,"w")
	f.write(newText)
	f.close()
		

def callback(arg,dirname,names):
	for n in names:
		for r in res:
			if r.search(n)!=None:
				try:
					print dirname+'\\'+n
					processSpaces(dirname+'\\'+n)
					break
				except:
					print "exception"
					print dirname+'\\'+n

os.path.walk(startDirectory,callback,None)
