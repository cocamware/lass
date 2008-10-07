#! /usr/bin/env python

import os
import os.path

project = 'lass'
version = major_version, minor_version, patchlevel = 1, 1, 1
soversion = 1
src_dir = 'src'
libraries = ['config', 'io', 'meta', 'num', 'prim', 'spat', 'stde', 'util', 'dll']
tests = ['test']

pre_builds = []
N = 15
utilDir = 'util'
pre_builds += [
	(utilDir, 'object_factory.tmpl.h', 'object_factory.h', N),
	(utilDir, 'clone_factory.tmpl.h', 'clone_factory.h', N),
	(utilDir, 'callback.tmpl.h', 'callback.h', N),
	(utilDir, 'callback_x.tmpl.h', 'callback_$x.h', N),
	(utilDir, 'callback_r_x.tmpl.h', 'callback_r_$x.h', N),
	(utilDir, 'callback_python.tmpl.h', 'callback_python.h', N),
	(utilDir, 'pyobject_macros.tmpl.h', 'pyobject_macros.h', N),
	(utilDir, 'pyobject_call.tmpl.inl', 'pyobject_call.inl', N),
	(utilDir, 'py_tuple.tmpl.h', 'py_tuple.h', 20),
	(utilDir, 'thread_fun.tmpl.h', 'thread_fun.h', N),
	(utilDir, 'thread_fun.tmpl.inl', 'thread_fun.inl', N),
	(utilDir, 'bind.tmpl.h', 'bind.h', N),
	]
utilImplDir = utilDir + '/impl'
pre_builds += [
	(utilImplDir, 'dispatcher_x.tmpl.h', 'dispatcher_$x.h', N),
	(utilImplDir, 'dispatcher_r_x.tmpl.h', 'dispatcher_r_$x.h', N),
	]
metaDir = 'meta'
pre_builds += [
	(metaDir, 'is_member.tmpl.h', 'is_member.h', N),
	]
testDir = 'test'
pre_builds += [
	(testDir, 'test_util_callback.tmpl.inl', 'test_util_callback.inl', 3),
	]


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
		
def write_pre_build(dirname, source, dest, n):
	source_path = '${CMAKE_SOURCE_DIR}/%(dirname)s/%(source)s' % { 'dirname': dirname, 'source': source }
	dest_path = '${CMAKE_SOURCE_DIR}/%(dirname)s/%(dest)s' % { 'dirname': dirname, 'dest': dest }
	if '$x' in dest_path:
		output = ' '.join([dest_path.replace('$x', str(i+1)) for i in range(n)])
	else:
		output = dest_path
	if dirname.startswith('test'):
		target = 'test'
	else:
		target = 'lass'
		
	content = '''
add_custom_command(
	OUTPUT %(output)s
	DEPENDS %(source_path)s
	COMMAND ${PYTHON_EXECUTABLE}
	ARGS ${CMAKE_SOURCE_DIR}/../tools/param_expander.py %(source_path)s %(dest_path)s %(n)d
	)
set(%(target)s_PRE_BUILT_HDRS ${%(target)s_PRE_BUILT_HDRS} %(output)s)
''' % { 'output': output, 'source_path': source_path, 'dest_path': dest_path, 'n': n, 'target': target}
	
	return content


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
#set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY lib)
#set(CMAKE_LIBRARY_OUTPUT_DIRECTORY bin)
#set(CMAKE_RUNTIME_OUTPUT_DIRECTORY bin)
set(CMAKE_DEBUG_POSTFIX _d)
'''

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

include(FindPythonInterp)
mark_as_advanced(CLEAR PYTHON_EXECUTABLE)

include(CTest)

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
					
	for pre_build in pre_builds:
		content += write_pre_build(*pre_build)
	content += r'''
set(lass_HDRS ${lass_HDRS} ${lass_PRE_BUILT_HDRS})
set(test_HDRS ${test_HDRS} ${test_PRE_BUILT_HDRS})
'''

	content += r'''
add_library(lass ${lass_SRCS} ${lass_HDRS})
target_link_libraries(lass optimized ${PYTHON_LIBRARIES} debug ${PYTHON_DEBUG_LIBRARIES})
set_target_properties(lass PROPERTIES
	DEFINE_SYMBOL LASS_EXPORTS
	VERSION %(major_version)d.%(minor_version)d
	SOVERSION %(patchlevel)d)
''' % { 'sources': ' '.join(['${%s_SRCS} ${%s_HDRS}' % (lib, lib) for lib in library_dirs]), 'major_version':major_version, 'minor_version':minor_version, 'patchlevel':patchlevel }
		
	assert not os.path.join('test', 'impl') in source_files.keys() + header_files.keys()
	content += r'''
if(BUILD_TESTING)
	enable_testing()
	add_executable(test ${test_SRCS} ${test_HDRS})
	target_link_libraries(test lass optimized ${PYTHON_LIBRARIES} debug ${PYTHON_DEBUG_LIBRARIES})

	add_test(test ${PYTHON_EXECUTABLE} ${CMAKE_SOURCE_DIR}/test_driver.py ${CMAKE_BINARY_DIR}/test_work ${EXECUTABLE_OUTPUT_PATH}/test ${CMAKE_SOURCE_DIR}/test)
endif(BUILD_TESTING)
'''
	
	write_file(os.path.join(src_dir, 'CMakeLists.txt'), content)


if __name__ == "__main__":
	#import pre_build
	#pre_build.pre_build()
	write_cmakelists(libraries)
