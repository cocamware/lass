import os
import string

def createSrcMakeFile():
    f=open('src/Makefile.am','w+')
    f.write(r'''# set the include path found by configure
INCLUDES= $(all_includes)

# the library search path.
#lass_LDFLAGS = $(all_libraries) 
SUBDIRS = num prim util io test ''')
    f.close()
    

def getSources(map):
	filelist = os.listdir(map)
	files = []
	for f in filelist:
		if f.endswith('cpp'):
			files.append(f)
	return files

def makeMakeFile(directory):
	f = open('src/'+directory+'/Makefile.am','w+')
	f.write('INCLUDES= $(all_includes) -I/usr/include/python2.3/ -I/usr/local/lib/boost_1_31_0/\n')
	f.write('lib_LIBRARIES = liblass'+directory+'.a\n')
	f.write('liblass'+directory+'_a_SOURCES = ')
	files = getSources('src/'+directory)
	filesStr = string.join(files,' ')
	f.write(filesStr+'\n')
	f.close()


os.chdir('src/util')
os.system('python pre_build.py')
os.chdir('../test')
os.system('python pre_build.py')
os.chdir('../..')
createSrcMakeFile()
makeMakeFile('util')
makeMakeFile('io')
makeMakeFile('num')
makeMakeFile('prim')
makeMakeFile('test')
os.system('aclocal')
os.system('autoheader')
os.system('autoconf')
os.system('automake -a')
os.system('make distclean')
os.system('./configure')
os.system('make CPPFLAGS+=-w')


