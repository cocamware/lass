#! /usr/bin/python

lass_name = "lass"
lass_version = (1, 0, 0)
library_version = (2, 0, 2)
#                  |  |  |
#            +-----+  |  +--+
#            |        |     |
#         current:revision:age
#            |        |     |
#            |        |     +- increment if interfaces have been added
#            |        |        set to zero if interfaces have been removed
#            |        |        or changed
#            |        +- increment if source code has changed
#            |           set to zero if current is incremented
#            +- increment if interfaces have been added, removed or changed

source_dirs = ['config', 'frb', 'gis', 'io', 'meta', 'num', 'prim', 'spat', 'stde', 'util']
test_dirs = ['test']

debug_info_flags = '-g'
code_generation_flags = ''#'-msse2 -march=pentium4 -mfpmath=sse'
warning_flags = '-Wall'# -Wextra'#-Wconversion -Wshadow -Wcast-qual -Wwrite-strings -Wold-style-casts -Wsign-promo'
libs = '-lpthread -lrt -ldl -lutil'

import os
import os.path
import sys

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
	result += ['src/test/mt19937ar.out', 'src/test/spline.in', 'src/test/spline_linear.pattern', 'src/test/spline_cubic.pattern']
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
AC_PREREQ(2.57)
AC_INIT(%s, %s, bramz@sourceforge.net)
''' % (lass_name, "%s.%s.%s" % lass_version))

	configure.write(r'''
LASS_NAME=%s
AC_SUBST(LASS_NAME)
PACKAGE=$LASS_NAME
''' % lass_name)

	configure.write(r'''
LASS_MAJOR_VERSION=%s
LASS_MINOR_VERSION=%s
LASS_PATCH_VERSION=%s
LASS_VERSION=$LASS_MAJOR_VERSION.$LASS_MINOR_VERSION.$LASS_MICRO_VERSION
LASS_RELEASE=$LASS_MAJOR_VERSION.$LASS_MINOR_VERSION
LASS_RELEASE_NAME=$LASS_NAME-$LASS_RELEASE
AC_SUBST(LASS_RELEASE)
AC_SUBST(LASS_VERSION)
AC_SUBST(LASS_RELEASE_NAME)
VERSION=$LASS_VERSION
''' % lass_version)

	configure.write(r'''
LASS_LIBRARY_VERSION=%s:%s:%s
AC_SUBST(LASS_LIBRARY_VERSION)
''' % library_version)

	configure.write(r'''
AC_CONFIG_SRCDIR([src/lass_common.h])
AM_CONFIG_HEADER(lass_auto_config.h)
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
AC_HEADER_STDBOOL
AC_CHECK_HEADERS([limits.h termios.h sys/ioctl.h sys/resource.h sys/socket.h])

# Checks for typedefs, structures, and compiler characteristics.

# Checks for library functions.
AC_FUNC_STRERROR_R
AC_CHECK_FUNCS([atexit clock_gettime])

AC_MSG_CHECKING(for Python)
LASS_PY_PREFIX=`python -c 'import sys; print sys.prefix'`
LASS_PY_EXEC_PREFIX=`python -c 'import sys; print sys.exec_prefix'`
LASS_PY_VERSION=`python -c 'import sys; print sys.version[[:3]]'`
LASS_PY_INCLUDES="-I${LASS_PY_PREFIX}/include/python${LASS_PY_VERSION} -I${LASS_PY_EXEC_PREFIX}/include/python${LASS_PY_VERSION}"
LASS_PY_LDFLAGS="-L${LASS_PY_PREFIX}/lib/python${LASS_PY_VERSION}/config -lpython${LASS_PY_VERSION}"
AC_SUBST(LASS_PY_PREFIX)
AC_SUBST(LASS_PY_EXEC_PREFIX)
AC_SUBST(LASS_PY_VERSION)
AC_SUBST(LASS_PY_INCLUDES)
AC_SUBST(LASS_PY_LDFLAGS)
AC_MSG_RESULT(${LASS_PY_VERSION})
''')

	release_name = "%s-%s.%s" % (lass_name, lass_version[0], lass_version[1])
	configure.write(r'''
AC_OUTPUT([
	Makefile 
	src/Makefile 
	src/test/Makefile 
	%s.pc
])
''' % release_name) 
	configure.close()


def write_makefile(extra_dist):
	print "- Makefile.am"
	makefile=open('Makefile.am','w+')
	makefile.write(r'''
AUTOMAKE_OPTIONS = 1.6
SUBDIRS = src

pkgconfigdir = $(libdir)/pkgconfig
pkgconfig_DATA = $(LASS_RELEASE_NAME).pc

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
	
	makefile.write(r'''
SUBDIRS=. test
INCLUDES= $(all_includes) $(LASS_PY_INCLUDES)
''')

	release_name = "%s-%s.%s" % (lass_name, lass_version[0], lass_version[1])
	makefile.write(r'''
lib_LTLIBRARIES = lib%s.la
''' % release_name)

	simple_release_name = "%s_%s_%s" % (lass_name, lass_version[0], lass_version[1])
	sources = ' '.join(sources_list)
	makefile.write(r'''
lib%s_la_SOURCES = %s
''' % (simple_release_name, sources))

	makefile.write(r'''
lib%s_la_CPPFLAGS = %s %s %s
''' % (simple_release_name, debug_info_flags, code_generation_flags, warning_flags))
	
	makefile.write(r'''
lib%s_la_LDFLAGS = $(LASS_PY_LDFLAGS) %s -version-info $(LASS_LIBRARY_VERSION) -release $(LASS_RELEASE)
''' % (simple_release_name, libs))

	for subdir in headers_dict.keys():
		incdir = subdir
		if incdir != '':
			incdir = '/' + incdir
		incname = incdir.replace('/', '_')
		incfiles = ' '.join([os.path.join(subdir, f) for f in headers_dict[subdir]])
		makefile.write('include_lass%sdir = $(includedir)/$(LASS_RELEASE_NAME)/lass%s\n' % (incname, incdir))
		makefile.write('include_lass%s_HEADERS = %s\n' % (incname, incfiles))
		
	makefile.close()

	
	
def write_src_test_makefile(test_sources_list):
	print "- src/test/Makefile.am"
	makefile=open('src/test/Makefile.am','w+')
	
	makefile.write(r'''
INCLUDES= $(all_includes) $(LASS_PY_INCLUDES)
''')
	
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
lass_test_LDADD = ../lib$(LASS_RELEASE_NAME).la

TESTS = lass_test
''')



def write_pc_file():
	file_name = "%s-%s.%s.pc.in" % (lass_name, lass_version[0], lass_version[1])
	print "- %s" % file_name
	pc_file = open(file_name, 'w')
	pc_file.write(r'''
prefix=@prefix@
exec_prefix=@exec_prefix@
libdir=@libdir@
includedir=@includedir@
lass_release_name=@LASS_RELEASE_NAME@
lass_py_ldflags=@LASS_PY_LDFLAGS@

Name: LASS
Description: Library of Assembled Shared Sources (http://liar.sourceforge.net)
Requires: 
Version: @VERSION@
Libs: -L${libdir} -l${lass_release_name} ${lass_py_ldflags} %s
Cflags: -I${includedir}/${lass_release_name} -I${libdir}/${lass_release_name}/include
''' % libs)



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
sources = gather_sources('src', source_dirs)
headers = gather_headers('src', source_dirs)
write_src_makefile(sources, headers)
test_sources = gather_sources('src/test', '.')
write_src_test_makefile(test_sources)
write_pc_file()

print "\n* AUTOGEN"
autogen()
