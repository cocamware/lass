import os
from os.path import join, getsize,splitext,sep
import string

for root, dirs, files in os.walk('html'):
	for name in files:
		if splitext(name)[1].lower()==".html":
			text=string.join(file(join(root,name)).readlines(),"")
			if text.find("$relpath$")>=0:
				depth = join(root,name).count(sep)
				newRelPath = ""
				if depth>1:
					newRelPath = string.join([".."]*(depth-1),sep)
				newtext = text.replace("$relpath$",newRelPath)
				print "Patching file at depth level ", depth , "(",join(root,name),")"
				file(join(root,name),"w").write(newtext)
