#! /usr/bin/env python

import os
import os.path

project = 'lass'
version = (1, 1, 1)
src_dir = 'src'
libraries = ['config', 'io', 'meta', 'num', 'prim', 'spat', 'stde', 'util', 'dll']
tests = ['test']

class Vars(object):
	def __init__(self):
		pass
	def vars(self):
		return self.__dict__

def gather_files(dirname, extensions, exclude_extensions = []):
	def endswith_one_of(path, suffixes):
		return suffixes and max([path.endswith(suffix) for suffix in suffixes])
			
	paths = []
	full_dirname = os.path.join(src_dir, dirname)
	if os.path.isdir(full_dirname):
		for fname in os.listdir(os.path.join(src_dir, dirname)):
			full_path = os.path.join(full_dirname, fname)
			if os.path.isfile(full_path) and endswith_one_of(full_path, extensions) and not endswith_one_of(full_path, exclude_extensions):
				path = os.path.join(dirname, fname)
				path = path.replace('\\', '/')
				paths.append(path)
	
	return paths

def gather_target_files(dirname):
	source_files = gather_files(dirname, ['.cpp'])
	header_files = gather_files(dirname, ['.h', '.inl'])#, exclude_extensions = ['.tmpl.h', '.tmpl.inl']) 
	return source_files, header_files

def write_file(path, content):
	print path,
	try:
		old_content = file(path).read()
	except IOError:
		old_content = None
	if content != old_content:
		file(path, 'w').write(content)
		print 'updated'
	else:
		print 'still up to date'

def write_cmakelists(library_dirs):
	content = r'''project(lass)
cmake_minimum_required(VERSION 2.6)
set(CMAKE_USE_RELATIVE_PATHS TRUE)
	'''
	
	content += r'''
set(LASS_MAJOR_VERSION %d)
set(LASS_MINOR_VERSION %d)
set(LASS_PATCHLEVEL %d)
''' % version

	content += r'''
option(BUILD_SHARED_LIBS "build shared lass library" on)
if(BUILD_SHARED_LIBS)
	set(LASS_SHARED_LIBRARY 1)
else(BUILD_SHARED_LIBS)
	set(LASS_SHARED_LIBRARY 0)
endif(BUILD_SHARED_LIBS)

include(FindPythonLibs)
mark_as_advanced(CLEAR PYTHON_LIBRARY PYTHON_INCLUDE_PATH PYTHON_DEBUG_LIBRARY)
include_directories(BEFORE ${PYTHON_INCLUDE_PATH})
if(PYTHON_DEBUG_LIBRARY)
	set(LASS_PYTHON_HAS_DEBUG_BUILD 1)
else(PYTHON_DEBUG_LIBRARY)
	set(LASS_PYTHON_HAS_DEBUG_BUILD 0)
	set(PYTHON_DEBUG_LIBRARIES ${PYTHON_LIBRARIES})
endif(PYTHON_DEBUG_LIBRARY)

configure_file(${lass_SOURCE_DIR}/config/local_config.h.in ${lass_SOURCE_DIR}/config/local_config.h)

set(lass_SRCS)
set(lass_HDRS lass_common.h)
'''
	
	source_files = {}
	header_files = {}
	for lib in library_dirs + ['test']:
		for sub_dir in lib, os.path.join(lib, 'impl'):
			key = sub_dir.replace('\\', '_').lower()
			sources, headers = gather_target_files(sub_dir)
			if sources or headers:
				if key.startswith('dll'):
					content += 'if(WIN32 AND LASS_SHARED_LIBRARY)\n'
				for files, suffix, top_group in (sources, 'SRCS', 'Source Files'), (headers, 'HDRS', 'Header Files'):
					if files:
						content += 'set(%(key)s_%(suffix)s %(files)s)\n' % { 'key':key, 'suffix':suffix, 'files':' '.join(files) }
						if not key.startswith('test'):
							content += 'source_group("%(top_group)s\\\\%(key)s" FILES ${%(key)s_%(suffix)s})\n' % { 'top_group':top_group, 'key':key, 'suffix':suffix }
							content += 'set(lass_%(suffix)s ${lass_%(suffix)s} ${%(key)s_%(suffix)s})\n' % { 'key':key, 'suffix':suffix }
				if key.startswith('dll'):
					content += 'endif(WIN32 AND LASS_SHARED_LIBRARY)\n'
	
	content += r'''
add_library(lass ${lass_SRCS} ${lass_HDRS})
target_link_libraries(lass optimized ${PYTHON_LIBRARIES} debug ${PYTHON_DEBUG_LIBRARIES})
set_property(TARGET lass PROPERTY DEFINE_SYMBOL LASS_EXPORTS)
''' % { 'sources': ' '.join(['${%s_SRCS} ${%s_HDRS}' % (lib, lib) for lib in library_dirs]) }
		
	assert not os.path.join('test', 'impl') in source_files.keys() + header_files.keys()
	content += r'''
add_executable(test ${test_SRCS} ${test_HDRS})
target_link_libraries(test lass optimized ${PYTHON_LIBRARIES} debug ${PYTHON_DEBUG_LIBRARIES})

set_property(TARGET lass test PROPERTY DEBUG_POSTFIX _d)

enable_testing()
add_test(test test)
'''
	
	write_file(os.path.join(src_dir, 'CMakeLists.txt'), content)


if __name__ == "__main__":
	import pre_build
	pre_build.pre_build()
	write_cmakelists(libraries)
