project(lass)
cmake_minimum_required(VERSION 2.6)
#list(APPEND CMAKE_MODULE_PATH ${CMAKE_SOURCE_DIR})

set(LASS_MAJOR_VERSION 1)
set(LASS_MINOR_VERSION 1)
set(LASS_PATCHLEVEL 1)
set(LASS_SOVERSION 4)

set(CMAKE_DEBUG_POSTFIX _d)

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
	set(testdir "${CMAKE_BINARY_DIR}/config")
	try_compile(
		LASS_HAVE_STRERROR_R_CHAR_P
		"${CMAKE_BINARY_DIR}/config"
		"${CMAKE_SOURCE_DIR}/config/check_strerror_r_char_p.cpp"
		)
endif()

set(lass_subdirectories config io num prim spat stde util)
if (WIN32 AND LASS_SHARED_LIBRARY)
	set(lass_subdirectories ${lass_subdirectories} dll)
endif()

file(GLOB lass_SRCS *.cpp)
file(GLOB lass_HDRS *.h *.inl)
foreach(subdir ${lass_subdirectories})
	if(NOT subdir STREQUAL "config")
		file(GLOB_RECURSE temp_SRCS ${subdir}/*.cpp)
	endif()	       
	file(GLOB_RECURSE temp_HDRS ${subdir}/*.h ${subdir}/*.inl)
	list(APPEND lass_SRCS ${temp_SRCS})
	list(APPEND lass_HDRS ${temp_HDRS})
endforeach()

configure_file(${lass_SOURCE_DIR}/config/local_config.h.in ${lass_BINARY_DIR}/config/local_config.h)
include_directories(${lass_BINARY_DIR}/config)
list(APPEND lass_HDRS ${lass_BINARY_DIR}/config/local_config.h)

macro(prebuild dirname infile outfile n)
        if(${outfile} MATCHES "\\$x")
                foreach(i RANGE 1 ${n})
                        string(REPLACE "$x" ${i} temp ${outfile})
                        set(inpath ${CMAKE_SOURCE_DIR}/${dirname}/${infile})
                        set(outpath ${CMAKE_SOURCE_DIR}/${dirname}/${temp})
                        add_custom_command(
                                OUTPUT ${outpath}
                                DEPENDS ${inpath}
                                COMMAND ${PYTHON_EXECUTABLE}
                                ARGS ${CMAKE_SOURCE_DIR}/../tools/param_expander.py ${inpath} ${outpath} ${i})
                        list(APPEND lass_PRE_BUILT_HDRS ${outpath})
                endforeach()
        else()
                set(inpath ${CMAKE_SOURCE_DIR}/${dirname}/${infile})
                set(outpath ${CMAKE_SOURCE_DIR}/${dirname}/${outfile})
                add_custom_command(
                        OUTPUT ${outpath}
                        DEPENDS ${inpath}
                        COMMAND ${PYTHON_EXECUTABLE}
                        ARGS ${CMAKE_SOURCE_DIR}/../tools/param_expander.py ${inpath} ${outpath} ${n})
                list(APPEND lass_PRE_BUILT_HDRS ${outpath})
        endif()
endmacro()

set(N 15)
prebuild(util object_factory.tmpl.h object_factory.h ${N})
prebuild(util clone_factory.tmpl.h clone_factory.h ${N})
prebuild(util callback.tmpl.h callback.h ${N})
prebuild(util callback_x.tmpl.h "callback_$x.h" ${N})
prebuild(util callback_r_x.tmpl.h "callback_r_$x.h" ${N})
prebuild(util callback_python.tmpl.h callback_python.h ${N})
prebuild(util pyobject_macros.tmpl.h pyobject_macros.h ${N})
prebuild(util pyobject_call.tmpl.inl pyobject_call.inl ${N})
prebuild(util py_tuple.tmpl.h py_tuple.h 20)
prebuild(util thread_fun.tmpl.h thread_fun.h ${N})
prebuild(util thread_fun.tmpl.inl thread_fun.inl ${N})
prebuild(util bind.tmpl.h bind.h ${N})
prebuild(util/impl dispatcher_x.tmpl.h "dispatcher_$x.h" ${N})
prebuild(util/impl dispatcher_r_x.tmpl.h "dispatcher_r_$x.h" ${N})
prebuild(meta is_member.tmpl.h is_member.h ${N})

list(APPEND lass_HDRS ${lass_PRE_BUILT_HDRS})
list(REMOVE_DUPLICATES lass_HDRS)

if(CMAKE_CL_64)
    # CMake doesn't really support adding assembly files to visual studio projects (yet)
    # Work around it by adding the obj file as 'source file' and have it generated
    set(sourcedir ${CMAKE_SOURCE_DIR}/util/impl)
    set(fname atomic_msvc_x64)
    set(sourcepath ${sourcedir}/${fname}.asm)
    set(defpath ${sourcedir}/${fname}.def)
    set(masm ml64.exe)
    if(MSVC_IDE)
        set(objpath ${CMAKE_BINARY_DIR}/lass.dir/${CMAKE_CFG_INTDIR}/${fname}.obj)
    else()
        set(objpath ${CMAKE_BINARY_DIR}/CMakeFiles/lass.dir/util/impl/${fname}.obj)
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
	VERSION ${LASS_MAJOR_VERSION}.${LASS_MINOR_VERSION}
	SOVERSION ${LASS_SOVERSION})
	
install(
	TARGETS lass
	RUNTIME DESTINATION bin
	LIBRARY DESTINATION lib
	ARCHIVE DESTINATION lib)

install(
	DIRECTORY ./
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
	FILES ${lass_BINARY_DIR}/config/local_config.h
	DESTINATION include/lass/config)
	
set (CPACK_PACKAGE_VERSION_MAJOR "${LASS_MAJOR_VERSION}")
set (CPACK_PACKAGE_VERSION_MINOR "${LASS_MINOR_VERSION}")
set (CPACK_PACKAGE_VERSION_PATCH "${LASS_PATCHLEVEL}")
set (CPACK_RESOURCE_FILE_LICENSE "${lass_SOURCE_DIR}/../docs/license.txt")
include(CPack)
	
include(CTest)
if(BUILD_TESTING)
	enable_testing()
	include(test/CMakeLists.txt)
endif()
