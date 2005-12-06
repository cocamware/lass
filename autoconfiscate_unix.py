#! /usr/bin/python


import os
import os.path
import sys

include_directories = ['%s/include/python%s' % (sys.prefix, sys.version[:3])]

lib_dirs = ['config', 'frb', 'gis', 'io', 'meta', 'num', 'prim', 'spat', 'stde', 'util']
test_dirs = ['test']




def pre_build():
	current = os.getcwd()
	os.chdir('src/util')
	os.system(sys.executable + ' pre_build.py')
	os.chdir('../test')
	os.system(sys.executable + ' pre_build.py')
	os.chdir(current)


def check_extension(filename, extension):
	return filename.endswith('.' + extension) and not filename.endswith('.tmpl.' + extension)


def gather_sources(directories):
	ignore_list = []
	for s in directories:
		ignore_list.append('%s/%s_common.cpp' % (s, s))
		if s != 'test':
			ignore_list.append('%s/%s.cpp' % (s, s))

	current = os.getcwd()
	os.chdir('src')
	
	def walker(sources, dirname, files):
		for f in files:
			sources.append(os.path.join(dirname, f))
	
	result = []
	for dirname in directories:
		os.path.walk(dirname, walker, result)
	result = [f for f in result if check_extension(f, 'cpp') and not f in ignore_list]
	
	os.chdir(current)
	print result
	return result



def gather_headers(directories):
	current = os.getcwd()
	os.chdir('src')
	
	def walker(headers, dirname, files):
		filtered = [f for f in files if check_extension(f, 'h') or check_extension(f, 'inl')]
		if filtered:
			headers[dirname] = filtered
	
	result = {'': ['lass_common.h']}
	os.path.walk('config', walker, result)
	for s in directories:
		os.path.walk(s, walker, result)
	
	os.chdir(current)
	return result



def writeConfigure():
	configure=open('configure.ac','w+')
	configure.write(r'''
AC_PREREQ(2.59)
AC_INIT(lass, 0.1, bramz@sourceforge.net)
PACKAGE=lass
VERSION=0.0.0
AC_CONFIG_SRCDIR([src/lass_common.h])
AM_INIT_AUTOMAKE
AM_CONFIG_HEADER(config.h)

# Checks for programs.
AC_PROG_CXX
AC_PROG_INSTALL
AC_PROG_LIBTOOL
AC_PROG_MAKE_SET

# Checks for languages
AC_LANG_CPLUSPLUS

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
AC_CHECK_FUNCS([atexit floor memset pow sqrt clock_gettime])
AC_OUTPUT(Makefile src/Makefile)
''') 
	configure.close()


def writeMakeFile():
	makefile=open('Makefile.am','w+')
	makefile.write(r'''
AUTOMAKE_OPTIONS = foreign 1.4
SUBDIRS = src
''')
	makefile.close()



def writeSrcMakeFile(sources_list, headers_dict, test_sources_list):
	includes = ' '.join(['-I' + i for i in include_directories])
	sources = ' '.join(sources_list)
	test_sources = ' '.join(test_sources_list)

	makefile=open('src/Makefile.am','w+')
	makefile.write(r'''
# set the include path found by configure
INCLUDES= $(all_includes) %s

# the librart
lib_LTLIBRARIES = liblass.la
liblass_la_SOURCES = %s
liblass_la_CPPFLAGS = -g -m32 -Wall
liblass_la_LDFLAGS = -lpython2.3 -lrt
''' % (includes, sources)
)

	for subdir in headers_dict.keys():
		incdir = subdir
		if incdir != '':
			incdir = '/' + incdir
		incname = incdir.replace('/', '_')
		incfiles = ' '.join([os.path.join(subdir, f) for f in headers_dict[subdir]])
		makefile.write('include_lass%sdir = $(includedir)/lass%s\n' % (incname, incdir))
		makefile.write('include_lass%s_HEADERS = %s\n' % (incname, incfiles))

	makefile.write(r'''
# test
noinst_PROGRAMS = testlass
testlass_SOURCES = %s
testlass_CPPFLAGS = -g -m32 -Wall
testlass_LDADD = liblass.la
''' % test_sources)

	makefile.close()



def build():
	print "LIBTOOLIZE"
        os.system('libtoolize --force')
	print "ACLOCAL"
	os.system('aclocal')
	print "AUTOCONF"
	os.system('autoconf')
	print "AUTOHEADER"
	os.system('autoheader')
	print "AUTOMAKE"
	os.system('automake -a')
	#os.system('make distclean')
	#os.system('./configure')
	#os.system('make')




pre_build()
sources = gather_sources(lib_dirs)
headers = gather_headers(lib_dirs)
test_sources = gather_sources(test_dirs)
writeConfigure()
writeMakeFile()
writeSrcMakeFile(sources, headers, test_sources)
build()
