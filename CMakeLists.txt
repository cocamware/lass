project(lass)
cmake_minimum_required(VERSION 2.6)
#list(APPEND CMAKE_MODULE_PATH ${CMAKE_SOURCE_DIR})

set(LASS_MAJOR_VERSION 1)
set(LASS_MINOR_VERSION 5)
set(LASS_PATCHLEVEL 4)
set(LASS_OUTPUT_NAME "lass-${LASS_MAJOR_VERSION}-${LASS_MINOR_VERSION}-${LASS_PATCHLEVEL}")
set(LASS_PYTHON_OUTPUT_NAME "lass-python-${LASS_MAJOR_VERSION}-${LASS_MINOR_VERSION}-${LASS_PATCHLEVEL}")

set(CMAKE_DEBUG_POSTFIX _d)


# --- SETTING A PREFIX IF NOT WAS FORCED BY USER ---
if(CMAKE_INSTALL_PREFIX_INITIALIZED_TO_DEFAULT)
	if(WIN32)
		set(CMAKE_INSTALL_PREFIX "$ENV{ProgramFiles}/Cocamware/Lass-${LASS_MAJOR_VERSION}.${LASS_MINOR_VERSION}" CACHE PATH "Install path prefix" FORCE)
	else()
		set(CMAKE_INSTALL_PREFIX "/opt/Cocamware/Lass-${LASS_MAJOR_VERSION}.${LASS_MINOR_VERSION}" CACHE PATH "Install path prefix" FORCE)
	endif(WIN32)
ENDIF(CMAKE_INSTALL_PREFIX_INITIALIZED_TO_DEFAULT)

# --- USER CONFIGURABLE STUFF ---

option(BUILD_SHARED_LIBS "build shared lass library" on) # The exact name of this option is important!
if(BUILD_SHARED_LIBS)
	set(LASS_SHARED_LIBRARY 1)
	set(LASS_PYTHON_SHARED_LIBRARY 1)
else()
	set(LASS_SHARED_LIBRARY 0)
	set(LASS_PYTHON_SHARED_LIBRARY 0)
endif()

option(BUILD_USING_PRECOMPILED_HEADERS "Build using precompiled headers" OFF)
option(BUILD_USING_STLPORT "Build lass with STLport" OFF)
if (BUILD_USING_STLPORT)
	find_package(STLPort 5.3)
	mark_as_advanced(CLEAR STLPORT_INSTALL_DIR STLPORT_INCLUDE_DIR STLPORT_LIBRARIES)
endif()
if (MSVC_IDE)
	option(BUILD_USING_MULTIPLE_PROCESSES OFF)
else()
	set(BUILD_USING_MULTIPLE_PROCESSES OFF)
endif ()

# --- CHECK THE ENVIRONMENT ----

include(Environment.cmake)

# --- SOURCE FILE GATHERING ---

set(lass_subdirectories config io num prim spat stde util meta)
set(lass_python_subdirectories python)

if (WIN32 AND LASS_SHARED_LIBRARY)
	set(lass_subdirectories ${lass_subdirectories} dll)
endif()

file(GLOB lass_SRCS lass/*.cpp)
file(GLOB lass_HDRS lass/*.h lass/*.inl)
foreach(subdir ${lass_subdirectories})
	if(NOT subdir STREQUAL "config")
		file(GLOB temp_SRCS lass/${subdir}/*.cpp)
		file(GLOB temp_impl_SRCS lass/${subdir}/impl/*.cpp)
	endif()
	file(GLOB temp_HDRS lass/${subdir}/*.h lass/${subdir}/*.inl)
	file(GLOB temp_impl_HDRS lass/${subdir}/impl/*.h lass/${subdir}/impl/*.inl)
	list(APPEND lass_SRCS ${temp_SRCS} ${temp_impl_SRCS})
	list(APPEND lass_HDRS ${temp_HDRS} ${temp_impl_HDRS})
	source_group("Source Files\\${subdir}" FILES ${temp_SRCS})
	source_group("Source Files\\${subdir}_impl" FILES ${temp_impl_SRCS})
	source_group("Header Files\\${subdir}" FILES ${temp_HDRS})
	source_group("Header Files\\${subdir}_impl" FILES ${temp_impl_HDRS})
endforeach()

file(GLOB lass_python_SRCS lass/*.cpp)
file(GLOB lass_python_HDRS lass/*.h lass/*.inl)
foreach(subdir ${lass_python_subdirectories})
	if(NOT subdir STREQUAL "config")
		file(GLOB temp_SRCS lass/${subdir}/*.cpp)
		file(GLOB temp_impl_SRCS lass/${subdir}/impl/*.cpp)
	endif()
	file(GLOB temp_HDRS lass/${subdir}/*.h lass/${subdir}/*.inl)
	file(GLOB temp_impl_HDRS lass/${subdir}/impl/*.h lass/${subdir}/impl/*.inl)
	list(APPEND lass_python_SRCS ${temp_SRCS} ${temp_impl_SRCS})
	list(APPEND lass_python_HDRS ${temp_HDRS} ${temp_impl_HDRS})
	source_group("Source Files\\${subdir}" FILES ${temp_SRCS})
	source_group("Source Files\\${subdir}_impl" FILES ${temp_impl_SRCS})
	source_group("Header Files\\${subdir}" FILES ${temp_HDRS})
	source_group("Header Files\\${subdir}_impl" FILES ${temp_impl_HDRS})
endforeach()


# --- PREBUILD STUFF ---

# These go in the source tree, as they are environment agnostic
# This is perhaps not the best way of doing it, but otherwise the include paths will become a huge mess ...

macro(param_expander inpath outpath n)
	set(generator "${CMAKE_CURRENT_SOURCE_DIR}/tools/param_expander.py")
	add_custom_command(
		OUTPUT ${outpath}
		DEPENDS ${inpath} ${generator}
		COMMAND ${PYTHON_EXECUTABLE}
		ARGS ${generator} ${inpath} ${outpath} ${n})
endmacro()

macro(prebuild dirname infile outfile n)
	set(inpath "${CMAKE_CURRENT_SOURCE_DIR}/${dirname}/${infile}")
	set(outpath "${CMAKE_CURRENT_SOURCE_DIR}/${dirname}/${outfile}")
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
prebuild(lass/meta is_member.tmpl.h is_member.h ${N})
prebuild(lass/python callback_python.tmpl.h callback_python.h ${N})
prebuild(lass/python pyobject_macros.tmpl.h pyobject_macros.h ${N})
prebuild(lass/python pyobject_call.tmpl.inl pyobject_call.inl ${N})
prebuild(lass/python pycallback_export_traits.tmpl.inl pycallback_export_traits.inl ${N})
prebuild(lass/python py_tuple.tmpl.h py_tuple.h 20)
prebuild(lass/util object_factory.tmpl.h object_factory.h ${N})
prebuild(lass/util clone_factory.tmpl.h clone_factory.h ${N})
prebuild(lass/util callback.tmpl.h callback.h ${N})
prebuild(lass/util multi_callback.tmpl.h multi_callback.h ${N})
prebuild(lass/util callback_x.tmpl.h "callback_$x.h" ${N})
prebuild(lass/util multi_callback_x.tmpl.h "multi_callback_$x.h" ${N})
prebuild(lass/util callback_r_x.tmpl.h "callback_r_$x.h" ${N})
prebuild(lass/util thread_fun.tmpl.h thread_fun.h ${N})
prebuild(lass/util thread_fun.tmpl.inl thread_fun.inl ${N})
prebuild(lass/util bind.tmpl.h bind.h ${N})
prebuild(lass/util/impl dispatcher_x.tmpl.h "dispatcher_$x.h" ${N})
prebuild(lass/util/impl dispatcher_r_x.tmpl.h "dispatcher_r_$x.h" ${N})
prebuild(lass/python bulk_add_integer.tmpl.inl "bulk_add_integer.inl" ${N})


list(APPEND lass_HDRS ${lass_PRE_BUILT_HDRS})
list(REMOVE_DUPLICATES lass_HDRS)

# --- PRECOMPILED HEADERS ---

macro(add_precompiled_header target hdrfile srcpath target_SRCS)
	if(BUILD_USING_PRECOMPILED_HEADERS)
		if(MSVC_IDE)
			set(pchpath "${CMAKE_CURRENT_BINARY_DIR}/${target}.dir/${CMAKE_CFG_INTDIR}/${hdrfile}.pch")
		else()
			set(pchpath "${CMAKE_CURRENT_BINARY_DIR}/CMakeFiles/${target}.dir/${hdrfile}.pch")
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
	set(lass_C_FLAGS "-fno-strict-aliasing -Wall -Wextra -Wformat=2 -Winit-self -Wconversion")
endif()
	
# --- THE LIBRARY ----

include_directories(
	${lass_INCLUDES}
	${config_dir}
	)
if(NOT BUILD_USING_PRECOMPILED_HEADERS)
	include_directories("${CMAKE_CURRENT_SOURCE_DIR}/lass") # for lass_common.h
endif()

if(CMAKE_CL_64)
    # CMake doesn't really support adding assembly files to visual studio projects (yet)
    # Work around it by adding the obj file as 'source file' and have it generated
    set(sourcedir ${CMAKE_CURRENT_SOURCE_DIR}/lass/util/impl)
    set(fname atomic_msvc_x64)
    set(sourcepath ${sourcedir}/${fname}.asm)
    set(defpath ${sourcedir}/${fname}.def)
    set(masm ml64.exe)
    if(MSVC_IDE)
        set(objpath ${CMAKE_CURRENT_BINARY_DIR}/lass.dir/${CMAKE_CFG_INTDIR}/${fname}.obj)
    else()
        set(objpath ${CMAKE_CURRENT_BINARY_DIR}/CMakeFiles/lass.dir/${fname}.obj)
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

set(_cflags "${lass_C_FLAGS}") 
if (BUILD_USING_MULTIPLE_PROCESSES)
	set(_cflags "${_cflags} /MP")
endif()

add_library(lass 
	${lass_SRCS} 
	${lass_HDRS})
target_link_libraries(lass
	${lass_LIBS})
set_target_properties(lass PROPERTIES
	DEFINE_SYMBOL LASS_EXPORTS 
	COMPILE_FLAGS ${_cflags}
	SOVERSION ${LASS_PATCHLEVEL}
	OUTPUT_NAME "${LASS_OUTPUT_NAME}"
	)
add_precompiled_header(
	lass 
	lass_common.h 
	${lass_SOURCE_DIR}/lass/lass_common.cpp
	${lass_SRCS})

add_library(lass_python
	${lass_python_SRCS} 
	${lass_python_HDRS})
target_link_libraries(lass_python
	lass
	optimized ${PYTHON_LIBRARIES} 
	debug ${PYTHON_DEBUG_LIBRARIES})
set_target_properties(lass_python PROPERTIES
	DEFINE_SYMBOL LASS_PYTHON_EXPORTS 
	COMPILE_FLAGS ${_cflags}
	SOVERSION ${LASS_PATCHLEVEL}
	OUTPUT_NAME "${LASS_PYTHON_OUTPUT_NAME}"
	)
#add_precompiled_header(
#	lass 
#	lass_common.h 
#	${lass_SOURCE_DIR}/lass/lass_common.cpp
#	${lass_python_SRCS})


add_subdirectory(pylass)

# --- TESTING ---

set(DART_TESTING_TIMEOUT 240 CACHE STRING "Maximum time allowed before CTest will kill the test.")
include(CTest)
if(BUILD_TESTING)
	enable_testing()
	include(test_suite/CMakeLists.txt)
endif()
	
# --- INSTALLING ---
	
install(
	TARGETS lass lass_python
	EXPORT LassExports
	RUNTIME DESTINATION bin
	LIBRARY DESTINATION lib
	ARCHIVE DESTINATION lib)
	
install(
	EXPORT LassExports
	DESTINATION lib
	)

install(
	DIRECTORY lass/
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
	DESTINATION include/lass/config
	)

foreach(_config_file LassConfig.cmake LassConfig.py LassPythonConfig.cmake LassPythonConfig.py)    
    configure_file(
        "${lass_SOURCE_DIR}/${_config_file}.in"
        "${lass_BINARY_DIR}/${_config_file}"
        @ONLY
        )
    install(
        FILES "${lass_BINARY_DIR}/${_config_file}"
        DESTINATION lib
        )
endforeach()

# Export build tree targets, so that you can build against the build-tree through LassConfig.cmake
export(
	TARGETS lass lass_python pylass 
	FILE ${lass_BINARY_DIR}/LassBuildExports.cmake
	)

export(
	TARGETS lass lass_python pylass 
	FILE ${lass_BINARY_DIR}/LassPythonBuildExports.cmake
	)


	

	
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