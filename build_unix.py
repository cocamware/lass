#! /usr/bin/python


import os
import string

include_directories = ['/usr/include/python2.3/', '/usr/local/lib/boost_1_31_0/']
subprojects = ['io', 'num', 'prim', 'util']

def makeConfigure():
    f=open('configure.ac','w+')
    f.write(r'''
AC_PREREQ(2.59)
AC_INIT(lass, 0.0, fubar@spam.com)
PACKAGE=lass
VERSION=0.0.0
AC_CONFIG_SRCDIR([src/lass_common.h])
AM_INIT_AUTOMAKE
AM_CONFIG_HEADER(config.h)
# Checks for programs.
AC_PROG_AWK
AC_PROG_CXX
AC_PROG_CC
AC_PROG_INSTALL
AC_PROG_LN_S
AC_PROG_RANLIB
AC_PROG_MAKE_SET
# Checks for libraries.
# Checks for header files.
# Checks for typedefs, structures, and compiler characteristics.
AC_HEADER_STDBOOL
AC_C_CONST
AC_C_INLINE
AC_TYPE_SIZE_T
AC_CHECK_TYPES([ptrdiff_t])
# Checks for library functions.
AC_FUNC_ERROR_AT_LINE
AC_HEADER_STDC
AC_TYPE_SIGNAL
AC_CHECK_FUNCS([atexit floor memset pow sqrt])
AC_OUTPUT(Makefile src/Makefile''')
    for p in subprojects:
        f.write(' src/%s/Makefile' % p)
    f.write(')\n') 



def makeMakeFile():
    f=open('Makefile.am','w+')
    f.write(r'''
AUTOMAKE_OPTIONS = foreign 1.4
SUBDIRS = src
    ''')



def makeSrcMakeFile():
    f=open('src/Makefile.am','w+')
    f.write(r'''# set the include path found by configure
INCLUDES= $(all_includes)
# the library search path.
#lass_LDFLAGS = $(all_libraries) 
SUBDIRS = ''')
    f.write(string.join(subprojects, ' '))
    f.close()



def getSources(map):
	filelist = os.listdir(map)
	files = []
	for f in filelist:
		if f.endswith('cpp'):
			files.append(f)
	return files



def makeMakeFileProject(directory):
	f = open('src/'+directory+'/Makefile.am','w+')
	f.write('INCLUDES= $(all_includes) %s\n' % string.join(['-I' + i for i in include_directories], ' '))
	f.write('lib_LIBRARIES = liblass'+directory+'.a\n')
	f.write('liblass'+directory+'_a_SOURCES = ')

	files = getSources('src/'+directory)
	filesStr = string.join(files,' ')
	try:
		implFiles = getSources('src/'+directory+'/impl')
		implFilesStr = string.join(['impl/'+x for x in implFiles],' ')		
	except OSError:
		implFilesStr = '' 
	f.write(filesStr+' '+implFilesStr+'\n')
	f.close()



os.chdir('src/util')
os.system('python pre_build.py')
os.chdir('../test')
os.system('python pre_build.py')
os.chdir('../..')
makeConfigure()
makeMakeFile()
makeSrcMakeFile()
for p in subprojects:
    makeMakeFileProject(p)
os.system('aclocal')
os.system('autoheader')
os.system('autoconf')
os.system('automake -a')
os.system('make distclean')
os.system('./configure')
os.system('make')
