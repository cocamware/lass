project(lass)
cmake_minimum_required(VERSION 2.6)
#list(APPEND CMAKE_MODULE_PATH ${CMAKE_SOURCE_DIR})

set(LASS_MAJOR_VERSION 1)
set(LASS_MINOR_VERSION 2)
set(LASS_PATCHLEVEL 0)
set(LASS_SOVERSION 5)

set(CMAKE_DEBUG_POSTFIX _d)
set(CMAKE_MINSIZEREL_POSTFIX _minsizerel)
set(CMAKE_RELWITHDEBINFO_POSTFIX _relwithdebinfo)

# --- SETTING A PREFIX IF NOT WAS FORCED BY USER ---
if(CMAKE_INSTALL_PREFIX_INITIALIZED_TO_DEFAULT)
	if(WIN32)
		set(CMAKE_INSTALL_PREFIX "$ENV{ProgramFiles}/Cocamware/Lass-${LASS_MAJOR_VERSION}.${LASS_MINOR_VERSION}.${LASS_PATCHLEVEL}" CACHE PATH "Install path prefix" FORCE)
	else()
		set(CMAKE_INSTALL_PREFIX "/opt/Cocamware/Lass-${LASS_MAJOR_VERSION}.${LASS_MINOR_VERSION}.${LASS_PATCHLEVEL}" CACHE PATH "Install path prefix" FORCE)
	endif(WIN32)
ENDIF(CMAKE_INSTALL_PREFIX_INITIALIZED_TO_DEFAULT)

# --- USER CONFIGURABLE STUFF ---

option(BUILD_SHARED_LIBS "build shared lass library" on) # The exact name of this option is important!
if(BUILD_SHARED_LIBS)
	set(LASS_SHARED_LIBRARY 1)
else()
	set(LASS_SHARED_LIBRARY 0)
endif()

option(BUILD_USING_PRECOMPILED_HEADERS "Build using precompiled headers" OFF)


# --- CHECK THE ENVIRONMENT ----

include(FindPythonLibs)
mark_as_advanced(CLEAR PYTHON_LIBRARY PYTHON_INCLUDE_PATH PYTHON_DEBUG_LIBRARY)
list(APPEND lass_INCLUDES ${PYTHON_INCLUDE_PATH})
if(PYTHON_DEBUG_LIBRARY)
	set(LASS_PYTHON_HAS_DEBUG_BUILD 1)
else()
	set(LASS_PYTHON_HAS_DEBUG_BUILD 0)
	set(PYTHON_DEBUG_LIBRARIES ${PYTHON_LIBRARIES})
endif()

include(FindPythonInterp)
mark_as_advanced(CLEAR PYTHON_EXECUTABLE)

include(CheckIncludeFile)
CHECK_INCLUDE_FILE("fcntl.h" LASS_HAVE_FCNTL_H)
CHECK_INCLUDE_FILE("limits.h" LASS_HAVE_LIMITS_H)
CHECK_INCLUDE_FILE("termios.h" LASS_HAVE_TERMIOS_H)
CHECK_INCLUDE_FILE("unistd.h" LASS_HAVE_UNISTD_H)
CHECK_INCLUDE_FILE("sys/ioctl.h" LASS_HAVE_SYS_IOCTL_H)
CHECK_INCLUDE_FILE("sys/mman.h" LASS_HAVE_SYS_MMAN_H)
CHECK_INCLUDE_FILE("sys/resource.h" LASS_HAVE_SYS_RESOURCE_H)
CHECK_INCLUDE_FILE("sys/socket.h" LASS_HAVE_SYS_SOCKET_H)
CHECK_INCLUDE_FILE("sys/stat.h" LASS_HAVE_SYS_STAT_H)

include(CheckLibraryExists)
CHECK_LIBRARY_EXISTS("rt" "clock_gettime" "" LASS_HAVE_LIBRT)
if (LASS_HAVE_LIBRT)
   list(APPEND lass_LIBS "rt")
endif()
CHECK_LIBRARY_EXISTS("dl" "dlopen" "" LASS_HAVE_LIBDL)
if (LASS_HAVE_LIBDL)
   list(APPEND lass_LIBS "dl")
endif()
CHECK_LIBRARY_EXISTS("util" "openpty" "" LASS_HAVE_LIBUTIL)
if (LASS_HAVE_LIBUTIL)
   list(APPEND lass_LIBS "util")
endif()

include(CheckFunctionExists)
CHECK_FUNCTION_EXISTS("strerror_r" LASS_HAVE_FUNC_STRERROR_R)
if(LASS_HAVE_FUNC_STRERROR_R)
	set(testdir "${lass_BINARY_DIR}/local")
	try_compile(
		LASS_HAVE_STRERROR_R_CHAR_P
		"${lass_BINARY_DIR}/local"
		"${lass_SOURCE_DIR}/src/config/check_strerror_r_char_p.cpp"
		)
endif()

# --- SOURCE FILE GATHERING ---
set(lass_subdirectories config io num prim spat stde util meta)

if (WIN32 AND LASS_SHARED_LIBRARY)
	set(lass_subdirectories ${lass_subdirectories} dll)
endif()

file(GLOB lass_SRCS src/*.cpp)
file(GLOB lass_HDRS src/*.h src/*.inl)
foreach(subdir ${lass_subdirectories})
	if(NOT subdir STREQUAL "config")
		file(GLOB_RECURSE temp_SRCS src/${subdir}/*.cpp)
	endif()	       
	file(GLOB_RECURSE temp_HDRS src/${subdir}/*.h src/${subdir}/*.inl)
	list(APPEND lass_SRCS ${temp_SRCS})
	list(APPEND lass_HDRS ${temp_HDRS})
endforeach()

# --- LOCAL CONFIGURATION FILE ---

# This header goes in the build directory as it contains build specific information

set(config_dir ${lass_BINARY_DIR}/local)
set(config_file ${config_dir}/local_config.h)
configure_file(${lass_SOURCE_DIR}/src/config/local_config.h.in ${config_file})
list(APPEND lass_HDRS ${config_file})
list(APPEND lass_INCLUDES ${config_dir})

# --- PREBUILD STUFF ---

# These go in the source tree, as they are environment agnostic
# This is perhaps not the best way of doing it, but otherwise the include paths will become a huge mess ...

macro(param_expander inpath outpath n)
	set(generator ${CMAKE_SOURCE_DIR}/tools/param_expander.py)
	add_custom_command(
		OUTPUT ${outpath}
		DEPENDS ${inpath} ${generator}
		COMMAND ${PYTHON_EXECUTABLE}
		ARGS ${generator} ${inpath} ${outpath} ${n})
endmacro()

macro(prebuild dirname infile outfile n)
	set(inpath ${CMAKE_SOURCE_DIR}/${dirname}/${infile})
	set(outpath ${CMAKE_SOURCE_DIR}/${dirname}/${outfile})
	if(${outpath} MATCHES "\\$x")
		foreach(i RANGE 1 ${n})
			string(REPLACE "$x" ${i} temp ${outpath})
			param_expander(${inpath} ${temp} ${i})
			list(APPEND lass_PRE_BUILT_HDRS ${temp})
		endforeach()
	else()
		param_expander(${inpath} ${outpath} ${n})
		list(APPEND lass_PRE_BUILT_HDRS ${outpath})
	endif()
endmacro()

set(N 15)
prebuild(src/util object_factory.tmpl.h object_factory.h ${N})
prebuild(src/util clone_factory.tmpl.h clone_factory.h ${N})
prebuild(src/util callback.tmpl.h callback.h ${N})
prebuild(src/util callback_x.tmpl.h "callback_$x.h" ${N})
prebuild(src/util callback_r_x.tmpl.h "callback_r_$x.h" ${N})
prebuild(src/util callback_python.tmpl.h callback_python.h ${N})
prebuild(src/util pyobject_macros.tmpl.h pyobject_macros.h ${N})
prebuild(src/util pyobject_call.tmpl.inl pyobject_call.inl ${N})
prebuild(src/util py_tuple.tmpl.h py_tuple.h 20)
prebuild(src/util thread_fun.tmpl.h thread_fun.h ${N})
prebuild(src/util thread_fun.tmpl.inl thread_fun.inl ${N})
prebuild(src/util bind.tmpl.h bind.h ${N})
prebuild(src/util/impl dispatcher_x.tmpl.h "dispatcher_$x.h" ${N})
prebuild(src/util/impl dispatcher_r_x.tmpl.h "dispatcher_r_$x.h" ${N})
prebuild(src/meta is_member.tmpl.h is_member.h ${N})

list(APPEND lass_HDRS ${lass_PRE_BUILT_HDRS})
list(REMOVE_DUPLICATES lass_HDRS)

# --- PRECOMPILED HEADERS ---

macro(add_precompiled_header target hdrfile srcpath target_SRCS)
	if(BUILD_USING_PRECOMPILED_HEADERS)
		if(MSVC_IDE)
			set(pchpath ${lass_BINARY_DIR}/${target}.dir/${CMAKE_CFG_INTDIR}/${hdrfile}.pch)
		else()
			set(pchpath ${lass_BINARY_DIR}/CMakeFiles/${target}.dir/${hdrfile}.pch)
		endif()
		if(MSVC)
			get_target_property(cflags ${target} COMPILE_FLAGS)
			message("${clags}")
			set_target_properties(
				${target}
				PROPERTIES
				COMPILE_FLAGS /Fp"${pchpath}"
				COMPILE_FLAGS /Yu"${hdrfile}"
				COMPILE_DEFINITIONS BUILD_USING_PRECOMPILED_HEADERS
				)
			set_source_files_properties(
				${srcpath} 
				PROPERTIES
				COMPILE_FLAGS /Yc"${hdrfile}"
				OBJECT_OUTPUTS ${pchpath})
			if (NOT MSVC_IDE)
				set(other_srcs ${target_SRCS})
				list(REMOVE_ITEM other_srcs ${srcpath})
				set_source_files_properties(
					${other_srcs}
					PROPERTIES
					OBJECT_DEPENDS "${pchpath}")
			endif()
		else()
			add_custom_command(
				OUTPUT ${pchpath}
				DEPENDS ${hdrfile}
				COMMAND ...)
			set_source_files_properties(
				${test_SRCS}
				PROPERTIES
				OBJECT_DEPENDS ${pchpath})
		endif()
	endif()
endmacro()

if(MSVC)
	set(lass_C_FLAGS "/EHsc")
else()
	set(lass_C_FLAGS "-fno-strict-aliasing")
endif()
	
# --- THE LIBRARY ----

include_directories(${lass_INCLUDES})
if(NOT BUILD_USING_PRECOMPILED_HEADERS)
	include_directories(${lass_SOURCE_DIR}/src) # for lass_common.h
endif()

if(CMAKE_CL_64)
    # CMake doesn't really support adding assembly files to visual studio projects (yet)
    # Work around it by adding the obj file as 'source file' and have it generated
    set(sourcedir ${CMAKE_SOURCE_DIR}/src/util/impl)
    set(fname atomic_msvc_x64)
    set(sourcepath ${sourcedir}/${fname}.asm)
    set(defpath ${sourcedir}/${fname}.def)
    set(masm ml64.exe)
    if(MSVC_IDE)
        set(objpath ${CMAKE_BINARY_DIR}/lass.dir/${CMAKE_CFG_INTDIR}/${fname}.obj)
    else()
        set(objpath ${CMAKE_BINARY_DIR}/local/${fname}.obj)
    endif()
    add_custom_command(
        OUTPUT ${objpath}
        DEPENDS ${sourcepath}
        COMMAND ${masm}
        ARGS /c "/Fo${objpath}" "${sourcepath}")
    list(APPEND lass_SRCS ${objpath} ${defpath})
    
    # It should have been as easy as
    #enable_language(ASM_MASM)
    #set_source_files_properties(
    #    ${sourcepath}
    #    LANGUAGE ASM_MASM)
    #list(APPEND lass_SRCS ${sourcepath} ${defpath})
endif()

add_library(lass 
	${lass_SRCS} 
	${lass_HDRS})
target_link_libraries(lass
	${lass_LIBS}
	optimized ${PYTHON_LIBRARIES} 
	debug ${PYTHON_DEBUG_LIBRARIES})
set_target_properties(lass PROPERTIES
	DEFINE_SYMBOL LASS_EXPORTS
	COMPILE_FLAGS ${lass_C_FLAGS}
	VERSION ${LASS_MAJOR_VERSION}.${LASS_MINOR_VERSION}
	SOVERSION ${LASS_SOVERSION})
add_precompiled_header(
	lass 
	lass_common.h 
	${lass_SOURCE_DIR}/src/lass_common.cpp
	${lass_SRCS})

	
# --- TESTING ---

set(DART_TESTING_TIMEOUT 240 CACHE STRING "Maximum time allowed before CTest will kill the test.")
include(CTest)
if(BUILD_TESTING)
	enable_testing()
	include(src/test/CMakeLists.txt)
endif()
	
# --- INSTALLING ---
	
install(
	TARGETS lass
	RUNTIME DESTINATION bin
	LIBRARY DESTINATION lib
	ARCHIVE DESTINATION lib)

install(
	DIRECTORY src/
	DESTINATION include/lass
	USE_SOURCE_PERMISSIONS
	REGEX "\\.svn" EXCLUDE
	PATTERN "*.tmpl.h" EXCLUDE
	PATTERN "*.tmpl.inl" EXCLUDE
	PATTERN "*.cmake" EXCLUDE
	PATTERN "*.cpp" EXCLUDE
	PATTERN "*.asm" EXCLUDE
	PATTERN "*.txt" EXCLUDE
	PATTERN "*.py" EXCLUDE
	PATTERN "*.suo" EXCLUDE
	PATTERN "*.ncb" EXCLUDE
	)
	
install(
	FILES ${config_file}
	DESTINATION include/lass/config)
	
# --- PACKAGING ---
if (WIN32)
	set (CPACK_PACKAGE_INSTALL_DIRECTORY "Cocamware/Lass-${LASS_MAJOR_VERSION}.${LASS_MINOR_VERSION}.${LASS_PATCHLEVEL}")
endif(WIN32)
set(CPACK_PACKAGE_NAME "Lass")
set(CPACK_PACKAGE_VENDOR "Cocamware")
set(CPACK_PACKAGE_DESCRIPTION_SUMMARY "Lass - Library of Assembled Sources")
set(CPACK_PACKAGE_VERSION "${LASS_MAJOR_VERSION}.${LASS_MINOR_VERSION}.${LASS_PATCHLEVEL}")
set(CPACK_PACKAGE_VERSION_MAJOR "${LASS_MAJOR_VERSION}")
set(CPACK_PACKAGE_VERSION_MINOR "${LASS_MINOR_VERSION}")
set(CPACK_PACKAGE_VERSION_PATCH "${LASS_PATCHLEVEL}")
set(CPACK_RESOURCE_FILE_LICENSE "${lass_SOURCE_DIR}/docs/license.txt")
set(CPACK_NSIS_DISPLAY_NAME "${CPACK_PACKAGE_INSTALL_DIRECTORY} Lass")
set(CPACK_NSIS_HELP_LINK "http:\\\\\\\\lass.cocamware.com")
set(CPACK_NSIS_URL_INFO_ABOUT "http:\\\\\\\\lass.cocamware.com")
set(CPACK_NSIS_CONTACT "liblass@cocamware.com")
include(CPack)
