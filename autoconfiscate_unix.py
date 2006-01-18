#! /usr/bin/python

library_name = "lass"
version = (1, 0, 0)
library_version = (1, 0, 0)

lib_dirs = ['config', 'frb', 'gis', 'io', 'meta', 'num', 'prim', 'spat', 'stde', 'util']
test_dirs = ['test']

debug_info_flags = '-g'
code_generation_flags = '-msse2 -march=pentium4 -mfpmath=sse'
warning_flags = '-Wall -Wextra -Wconversion -Wshadow -Wcast-qual -Wwrite-strings -Wold-style-casts -Wsign-promo'

import os
import os.path
import sys

python_version = sys.version[:3]
include_directories = ['%s/include/python%s' % (sys.prefix, python_version)]
library_directories = ['%s/lib/python%s/config' % (sys.prefix, python_version)]

version_string = "%s.%s" % (version[0], version[1])


def pre_build():
	current = os.getcwd()
	os.chdir('src/util')
	os.system(sys.executable + ' pre_build.py')
	os.chdir('../test')
	os.system(sys.executable + ' pre_build.py')
	os.chdir(current)


def check_extension(filename, extensions):
	return max([filename.endswith('.' + e) and not filename.endswith('.tmpl.' + e) for e in extensions])


def gather_sources(basedir, directories):
	ignore_list = []
	for s in directories:
		#ignore_list.append('%s/%s_common.cpp' % (s, s))
		if s != 'test':
			ignore_list.append('%s/%s.cpp' % (s, s))

	current = os.getcwd()
	os.chdir(basedir)
	
	def walker(sources, dirname, files):
		for f in files:
			sources.append(os.path.join(dirname, f))
	
	result = []
	for dirname in directories:
		os.path.walk(dirname, walker, result)
	result = [f for f in result if check_extension(f, ('h', 'inl', 'cpp')) and not f in ignore_list]
	
	os.chdir(current)
	#print result
	return result



def gather_headers(basedir, directories):
	current = os.getcwd()
	os.chdir(basedir)
	
	def walker(headers, dirname, files):
		filtered = [f for f in files if check_extension(f, ('h', 'inl'))]
		if filtered:
			headers[dirname] = filtered
	
	result = {'': ['lass_common.h']}
	os.path.walk('config', walker, result)
	for s in directories:
		os.path.walk(s, walker, result)	

	os.chdir(current)
	return result
	
	
def gather_extra_dist():
	extensions = ('py', 'tmpl.h', 'tmpl.inl', 'tmpl.cpp', 'sln', 'vcproj')
	def walker(extra_dist, dirname, files):
		d = dirname[2:]
		extra_dist += [os.path.join(d, f) for f in files if max([f.endswith('.'+e) for e in extensions])]
	
	result = []
	os.path.walk('.', walker, result)
	result += ['src/test/mt19937ar.out']
	result += ['docs/gpl.txt', 'docs/license.txt']
	result += ['docs/Simple-Color.xsl', 'docs/sloc.xml']
	result += ['docs/doxygen/footer.html', 'docs/doxygen/header.html', 'docs/doxygen/lass.doxygen', 'docs/doxygen/stylesheet.css']
	result += ['docs/codestyle/codestyle.tex']
	#result += gather_sources('test')
	#print result
	return result
	

def write_configure():
	print "- configure.ac"
	configure=open('configure.ac','w+')
	configure.write(r'''
AC_PREREQ(2.59)
AC_INIT(lass, %s.%s.%s, bramz@sourceforge.net)
AM_CONFIG_HEADER(lass_auto_config.h)
''' % version)

	configure.write(r'''
GENERIC_LIBRARY_NAME=lass

GENERIC_MAJOR_VERSION=%s
GENERIC_MINOR_VERSION=%s
GENERIC_MICRO_VERSION=%s
GENERIC_VERSION=$GENERIC_MAJOR_VERSION.$GENERIC_MINOR_VERSION.$GENERIC_MICRO_VERSION
AC_SUBST(GENERIC_VERSION)
''' % version)

	configure.write(r'''
#shared library versioning
GENERIC_LIBRARY_VERSION=%s:%s:%s
#                       | | |
#                +------+ | +---+
#                |        |     |
#             current:revision:age
#                |        |     |
#                |        |     +- increment if interfaces have been added
#                |        |        set to zero if interfaces have been removed
#                                  or changed
#                |        +- increment if source code has changed
#                |           set to zero if current is incremented
#                +- increment if interfaces have been added, removed or changed
''' % library_version)

	configure.write(r'''
dnl --------------------------------
dnl Package name and version number
dnl --------------------------------

AC_SUBST(GENERIC_LIBRARY_VERSION)

PACKAGE=$GENERIC_LIBRARY_NAME
AC_SUBST(GENERIC_LIBRARY_NAME)

GENERIC_VERSION=$GENERIC_MAJOR_VERSION.$GENERIC_MINOR_VERSION.$GENERIC_MICRO_VERSION
GENERIC_RELEASE=$GENERIC_MAJOR_VERSION.$GENERIC_MINOR_VERSION
AC_SUBST(GENERIC_RELEASE)
AC_SUBST(GENERIC_VERSION)

VERSION=$GENERIC_VERSION

AC_CONFIG_SRCDIR([src/lass_common.h])
AM_INIT_AUTOMAKE($PACKAGE, $VERSION, no-define)

# Checks for programs.
AC_PROG_CXX
AC_PROG_CXXCPP
AC_PROG_INSTALL
AC_PROG_LIBTOOL
AC_PROG_MAKE_SET
AM_SANITY_CHECK

# Checks for languages
AC_LANG_CPLUSPLUS

# Checks for libraries.

# Checks for header files.

# Checks for typedefs, structures, and compiler characteristics.
AC_HEADER_STDBOOL
#AC_C_CONST
#AC_C_INLINE
#AC_TYPE_SIZE_T
#AC_CHECK_TYPES([ptrdiff_t])

# Checks for library functions.
AC_FUNC_STRERROR_R

#AC_FUNC_ERROR_AT_LINE
#AC_HEADER_STDC
#AC_TYPE_SIGNAL
#AC_CHECK_FUNCS([atexit floor memset pow sqrt clock_gettime])


AC_OUTPUT(Makefile src/Makefile src/test/Makefile lass-1.0.0.pc)
''') 
	configure.close()


def write_makefile(extra_dist):
	print "- Makefile.am"
	makefile=open('Makefile.am','w+')
	makefile.write(r'''
AUTOMAKE_OPTIONS = 1.7
SUBDIRS = src

pkgconfigdir = $(libdir)/pkgconfig
pkgconfig_DATA = lass-1.0.0.pc

EXTRA_DIST=%s
''' % ' '.join(extra_dist))

	makefile.write(r'''
clean-local:
	-rm -rf ./src/test/*.log
	-rm -rf ./src/test/*.txt
	-rm -rf ./src/test/*.xml
	-rm -rf ./src/test/*.m
''')
	makefile.close()



def write_src_makefile(sources_list, headers_dict):
	print "- src/Makefile.am"
	makefile=open('src/Makefile.am','w+')
	
	includes = ' '.join(['-I' + i for i in include_directories])
	makefile.write(r'''
SUBDIRS=. test
INCLUDES= $(all_includes) %s
''' % includes)

	sources = ' '.join(sources_list)
	makefile.write(r'''
lib_LTLIBRARIES = liblass-1.0.0.la
liblass_1_0_0_la_SOURCES = %s
''' % sources)

	makefile.write(r'''
liblass_1_0_0_la_CPPFLAGS = %s %s %s
''' % (debug_info_flags, code_generation_flags, warning_flags))
	
	library_dirs = ' '.join(['-L' + i for i in library_directories])
	makefile.write(r'''
liblass_1_0_0_la_LDFLAGS = %s -lpython%s -lpthread -version-info $(GENERIC_LIBRARY_VERSION) -release $(GENERIC_RELEASE)
''' % (library_dirs, python_version))

	for subdir in headers_dict.keys():
		incdir = subdir
		if incdir != '':
			incdir = '/' + incdir
		incname = incdir.replace('/', '_')
		incfiles = ' '.join([os.path.join(subdir, f) for f in headers_dict[subdir]])
		makefile.write('include_lass%sdir = $(includedir)/lass-1.0.0/lass%s\n' % (incname, incdir))
		makefile.write('include_lass%s_HEADERS = %s\n' % (incname, incfiles))
		
	makefile.close()

	
	
def write_src_test_makefile(test_sources_list):
	print "- src/test/Makefile.am"
	makefile=open('src/test/Makefile.am','w+')
	
	includes = ' '.join(['-I' + i for i in include_directories])
	makefile.write(r'''
INCLUDES= $(all_includes) %s
''' % includes)
	
	test_sources = ' '.join(test_sources_list)
	makefile.write(r'''
# test
check_PROGRAMS = lass_test
lass_test_SOURCES = %s
''' % test_sources)

	makefile.write(r'''
lass_test_CPPFLAGS = %s %s %s
''' % (debug_info_flags, code_generation_flags, warning_flags))

	makefile.write(r'''
lass_test_LDADD = ../liblass-1.0.0.la

TESTS = lass_test
''')



def write_pc_file():
	print "- lass-1.0.0.pc.in"
	pc_file = open('lass-1.0.0.pc.in', 'w')
	pc_file.write(r'''
prefix=@prefix@
exec_prefix=@exec_prefix@
libdir=@libdir@
includedir=@includedir@

Name: LASS
Description: Library of Assembled Shared Sources (http://liar.sourceforge.net)
Requires: 
Version: @VERSION@
Libs: -L${libdir} -llass-1.0.0
Cflags: -I${includedir}/lass-1.0.0 -I${libdir}/lass-1.0.0/include
''')



def autogen():
	print "- libtoolize"
        os.system('libtoolize --force')
	print "- aclocal"
	os.system('aclocal')
	print "- autoconf"
	os.system('autoconf')
	print "- autoheader"
	os.system('autoheader')
	print "- automake"
	os.system('automake -a')
	#os.system('make distclean')
	#os.system('./configure')
	#os.system('make')



print "* GENERATING SOURCE FILES"
pre_build()

print "\n* GENERATING MAKE FILES"
write_configure()
extra_dist = gather_extra_dist()
write_makefile(extra_dist)
sources = gather_sources('src', lib_dirs)
headers = gather_headers('src', lib_dirs)
write_src_makefile(sources, headers)
test_sources = gather_sources('src/test', '.')
write_src_test_makefile(test_sources)
write_pc_file()

print "\n* AUTOGEN"
autogen()
