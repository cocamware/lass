project(lass)
cmake_minimum_required(VERSION 2.8)
#list(APPEND CMAKE_MODULE_PATH ${CMAKE_SOURCE_DIR})

include(cmake/Helpers.cmake)

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
if(CMAKE_C_COMPILER_ID MATCHES "SunPro")
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
	lass_link_option_to_flag(BUILD_WITHOUT_ITERATOR_DEBUGGING CMAKE_CXX_FLAGS "/D_HAS_ITERATOR_DEBUGGING=0" EXPORT)
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
	lass_link_option_to_flag(BUILD_USING_MULTIPLE_PROCESSES CMAKE_CXX_FLAGS "/MP")
else()
	set(BUILD_USING_MULTIPLE_PROCESSES OFF)
endif ()

# If BUILD_SIMD_ALIGNED is enabled, it sets LASS_SIMD_ALIGNMENT to 16 (16 bytes is the common SIMD alignment I know of so far).
# Eventually LASS_SIMD_ALIGN is conditionally defined to LASS_ALIGN(LASS_SIMD_ALIGNMENT).
# And this is finally used to conditionally align types like Vector2D, Vector3D, ... ColorRGBA, Transformation3D.
# 
# Enabling this can allow the compiler to generate more optimized SSE code, but obviously comes at the cost of extra storage.
# So, it's not beneficial for all applications, which is why I leave it disabled as default.
# 
option(BUILD_SIMD_ALIGNED "Align some vector-like structures on 16-byte boundaries for better SSE2 code generation" OFF)
if (BUILD_SIMD_ALIGNED)
	set(LASS_SIMD_ALIGNMENT 16)
else()
	set(LASS_SIMD_ALIGNMENT 0)
endif()

# --- CHECK THE ENVIRONMENT ----

include(cmake/Environment.cmake)



if(MSVC)
	lass_add_flags_once(CMAKE_CXX_FLAGS EXPORT "/EHsc") 
elseif(SUNPRO)
	lass_add_flags_once(CMAKE_CXX_FLAGS NOEXPORT "-errtags=yes" "-erroff=wbadinit,wbadasg,badargtype2w,") 
else()
	lass_add_flags_once(CMAKE_CXX_FLAGS EXPORT "-fno-strict-aliasing")
	lass_add_flags_once(CMAKE_CXX_FLAGS NOEXPORT "-Wall" "-Wextra" "-Wformat=2" "-Winit-self" "-Wconversion" "-Wno-unknown-pragmas")
endif()


include_directories(
	${lass_INCLUDES}
	"${config_dir}"
	"${CMAKE_CURRENT_SOURCE_DIR}"
	"${CMAKE_CURRENT_SOURCE_DIR}/lass" # for lass_common.h
	)

add_subdirectory(lass)
if(PYTHONLIBS_FOUND)
	add_subdirectory(pylass)
endif()

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

# we export the flags from CMAKE_CXX_FLAGS that we know cause binary compatibility issues
# and that projects linking to LASS will also need to have.
# It's put in the config files as Lass_CXX_FLAGS
lass_gather_exported_flags(CMAKE_CXX_FLAGS Lass_EXTRA_CXX_FLAGS)

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
	TARGETS lass
	FILE ${lass_BINARY_DIR}/LassBuildExports.cmake
	)
if(PYTHONLIBS_FOUND)
	export(
		TARGETS lass_python pylass
		APPEND FILE ${lass_BINARY_DIR}/LassBuildExports.cmake
		)
endif()



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
