project(lass)
cmake_minimum_required(VERSION 2.8)
#list(APPEND CMAKE_MODULE_PATH ${CMAKE_SOURCE_DIR})

set(LASS_MAJOR_VERSION 1)
set(LASS_MINOR_VERSION 7)
set(LASS_PATCHLEVEL 0)
set(LASS_OUTPUT_NAME "lass-${LASS_MAJOR_VERSION}-${LASS_MINOR_VERSION}-${LASS_PATCHLEVEL}")
set(LASS_PYTHON_OUTPUT_NAME "lass-python-${LASS_MAJOR_VERSION}-${LASS_MINOR_VERSION}-${LASS_PATCHLEVEL}")

set(CMAKE_DEBUG_POSTFIX _d)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${lass_BINARY_DIR}/bin")
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "${lass_BINARY_DIR}/bin")
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY "${lass_BINARY_DIR}/lib")

unset(SUNPRO)
if("${CMAKE_C_COMPILER_ID}" MATCHES "SunPro")
	set(SUNPRO TRUE)
endif()
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
else()
	set(LASS_SHARED_LIBRARY 0)
endif()

if (MSVC)
	option(BUILD_USING_PRECOMPILED_HEADERS "Build using precompiled headers" ON)
	option(BUILD_WITHOUT_ITERATOR_DEBUGGING "Build with _HAS_ITERATOR_DEBUGGING=0, otherwise as MSVC's default" OFF)
	mark_as_advanced(BUILD_WITHOUT_ITERATOR_DEBUGGING)
else()
	set(BUILD_USING_PRECOMPILED_HEADERS OFF)
	set(BUILD_WITHOUT_ITERATOR_DEBUGGING OFF)
endif()

if (MSVC_IDE AND NOT MSVC_VERSION LESS 1400)
	if (MSVC_VERSION LESS 1500)
		set(_default_mp OFF)
	else()
		set(_default_mp ON)
	endif()
	option(BUILD_USING_MULTIPLE_PROCESSES "Enable parallel compilation within one project (/MP)" ${_default_mp})
else()
	set(BUILD_USING_MULTIPLE_PROCESSES OFF)
endif ()

# --- CHECK THE ENVIRONMENT ----

include(cmake/Environment.cmake)

# --- MACROS ---

macro(eval)
	foreach(arg ${ARGN})
		message(STATUS "${arg}: ${${arg}}")
	endforeach()
endmacro()

macro(param_expander inpath outpath n)
	set(generator "${lass_SOURCE_DIR}/tools/param_expander.py")
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

macro(add_precompiled_header target hdrfile srcpath target_SRCS)
	if(BUILD_USING_PRECOMPILED_HEADERS)
		if(MSVC_IDE)
			set(_pchpath "${CMAKE_CURRENT_BINARY_DIR}/${target}.dir/${CMAKE_CFG_INTDIR}/${hdrfile}.pch")
		else()
			set(_pchpath "${CMAKE_CURRENT_BINARY_DIR}/CMakeFiles/${target}.dir/${hdrfile}.pch")
		endif()
		if(MSVC)
			get_target_property(_cflags ${target} COMPILE_FLAGS)
			if(NOT _cflags)
				set(_cflags "") # enforce to empty string
			endif()
			set_target_properties(
				${target}
				PROPERTIES
				COMPILE_FLAGS "${_cflags} /Fp\"${_pchpath}\" /Yu\"${hdrfile}\""
				COMPILE_DEFINITIONS BUILD_USING_PRECOMPILED_HEADERS
				)
			get_source_file_property(_cflags ${srcpath} COMPILE_FLAGS)
			if(NOT _cflags)
				set(_cflags "") # enforce to empty string
			endif()
			set_source_files_properties(
				${srcpath} 
				PROPERTIES
				COMPILE_FLAGS "${_cflags} /Yc\"${hdrfile}\""
				OBJECT_OUTPUTS ${_pchpath})
			if (NOT MSVC_IDE)
				set(other_srcs ${target_SRCS})
				list(REMOVE_ITEM other_srcs ${srcpath})
				set_source_files_properties(
					${other_srcs}
					PROPERTIES
					OBJECT_DEPENDS "${_pchpath}")
			endif()
		else()
			#add_custom_command(
			#	OUTPUT ${_pchpath}
			#	DEPENDS ${hdrfile}
			#	COMMAND ...)
			#set_source_files_properties(
			#	${test_SRCS}
			#	PROPERTIES
			#	OBJECT_DEPENDS ${_pchpath})
		endif()
	endif()
endmacro()

if(MSVC)
	set(lass_C_FLAGS "/EHsc")
	if (BUILD_USING_MULTIPLE_PROCESSES)
		set(lass_C_FLAGS "${lass_C_FLAGS} /MP")
	endif()
elseif(SUNPRO)
	set(lass_C_FLAGS "-errtags=yes -erroff=wbadinit,wbadasg,badargtype2w,")
else()
	set(lass_C_FLAGS "-fno-strict-aliasing -Wall -Wextra -Wformat=2 -Winit-self -Wconversion -Wno-unknown-pragmas")
endif()

if (BUILD_WITHOUT_ITERATOR_DEBUGGING)
	add_definitions(/D_HAS_ITERATOR_DEBUGGING=0)
endif()

include_directories(
	${lass_INCLUDES}
	"${config_dir}"
	"${CMAKE_CURRENT_SOURCE_DIR}"
	"${CMAKE_CURRENT_SOURCE_DIR}/lass" # for lass_common.h
	)

add_subdirectory(lass)
add_subdirectory(pylass)

# --- TESTING ---

set(DART_TESTING_TIMEOUT 240 CACHE STRING "Maximum time allowed before CTest will kill the test.")
include(CTest)
if(BUILD_TESTING)
	enable_testing()
	add_subdirectory(test_suite)
endif()
	
# --- INSTALLING ---
	
	
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

foreach(_config_file LassConfig.cmake LassConfig.py)    
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
