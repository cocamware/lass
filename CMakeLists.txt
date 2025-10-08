cmake_minimum_required(VERSION 3.16)
project(Lass VERSION 1.14.0)

include(cmake/Helpers.cmake)

set(CMAKE_DEBUG_POSTFIX _d)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${Lass_BINARY_DIR}/bin")
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "${Lass_BINARY_DIR}/lib")
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY "${Lass_BINARY_DIR}/lib")

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

if(CMAKE_CXX_STANDARD)
	if(CMAKE_CXX_STANDARD LESS 17)
		message(FATAL_ERROR "Lass requires C++17 or newer")
	endif()
else()
	set(CMAKE_CXX_STANDARD 17)
endif()

unset(SUNPRO)
if(CMAKE_C_COMPILER_ID MATCHES "SunPro")
	set(SUNPRO TRUE)
endif()


# --- SETTING A PREFIX IF NOT WAS FORCED BY USER ---
if(CMAKE_INSTALL_PREFIX_INITIALIZED_TO_DEFAULT)
	if(WIN32)
		set(CMAKE_INSTALL_PREFIX "$ENV{ProgramFiles}/Cocamware/Lass-${Lass_VERSION_MAJOR}.${Lass_VERSION_MINOR}" CACHE PATH "Install path prefix" FORCE)
	else()
		set(CMAKE_INSTALL_PREFIX "/opt/Cocamware/Lass-${Lass_VERSION_MAJOR}.${Lass_VERSION_MINOR}" CACHE PATH "Install path prefix" FORCE)
	endif()
endif()


# --- USER CONFIGURABLE STUFF ---

option(BUILD_SHARED_LIBS "build shared lass library" on) # The exact name of this option is important!
if(BUILD_SHARED_LIBS)
	set(LASS_SHARED_LIBRARY 1)
else()
	set(LASS_SHARED_LIBRARY 0)
endif()

option(BUILD_USING_PRECOMPILED_HEADERS "Build using precompiled headers" ON)

if (MSVC)
	option(BUILD_WITHOUT_ITERATOR_DEBUGGING "Build with _HAS_ITERATOR_DEBUGGING=0, otherwise as MSVC's default" OFF)
else()
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


if (Lass_WITH_STUBGEN)
	include(tools/LassStubgen.cmake)
	# Custum venv location, so we can reuse it across builds
	set(LASS_STUBGEN_VENV "${Lass_SOURCE_DIR}/${LASS_STUBGEN_VENV_NAME}")
endif()


# Private compiler options for all targets
# These typically deal with warnings
if(MSVC)
	add_compile_options("/W4")
	add_compile_options("/wd4251") # class 'X' needs to have dll-interface to be used by clients of class 'Y'
	add_compile_options("/wd4324") # 'structname': structure was padded due to alignment specifier
	if(BUILD_USING_MULTIPLE_PROCESSES)
		add_compile_options("/MP")
	endif()
	if (CMAKE_CXX_COMPILER_ID STREQUAL "Clang")
		add_compile_options("/clang:-Wno-unknown-pragmas")
		unset(CMAKE_INCLUDE_SYSTEM_FLAG_CXX)  # workaround for bug in intellisense
	endif()
elseif(SUNPRO)
	add_compile_options("-errtags=yes" "-erroff=wbadinit,wbadasg,badargtype2w,") 
else()
	add_compile_options("-Wall" "-Wextra" "-Wformat=2" "-Winit-self" "-Wconversion" "-Wno-unknown-pragmas")
	if (CMAKE_C_COMPILER_ID STREQUAL GNU)
		add_compile_options("-Wsuggest-override")
	endif()
endif()


# Always build Release with debug symbols as well
if(MSVC)
	add_compile_options("$<$<CONFIG:Release>:/Zi>")
	add_link_options("$<$<CONFIG:Release>:/DEBUG>" "$<$<CONFIG:Release>:/OPT:REF>"  "$<$<CONFIG:Release>:/OPT:ICF>")
else()
	add_compile_options("$<$<CONFIG:Release>:-g>")
endif()


if(Lass_HAVE_AVX)
	if(MSVC)
		add_compile_options("/arch:AVX")
	else()
		add_compile_options("-mavx")
	endif()
endif()


add_subdirectory(lass)
if(TARGET Lass::lass_python)
	add_subdirectory(pylass)
endif()

add_subdirectory(tools)

# --- TESTING ---

set(DART_TESTING_TIMEOUT 240 CACHE STRING "Maximum time allowed before CTest will kill the test.")
include(CTest)
if(BUILD_TESTING)
	list(APPEND CMAKE_CTEST_ARGUMENTS "--output-on-failure")
	enable_testing()
	add_subdirectory(test_suite)
	if(MSVC_IDE)
		set_property (DIRECTORY PROPERTY VS_STARTUP_PROJECT test_suite)
	endif()
endif()
	
# --- INSTALLING ---
	

install(
	EXPORT LassExports
	NAMESPACE Lass::
	DESTINATION share/Lass
	)


install(
	DIRECTORY lass/
	DESTINATION include/lass
	USE_SOURCE_PERMISSIONS
	FILES_MATCHING
	PATTERN "*.h"
	PATTERN "*.inl"
	PATTERN "*.tmpl.h" EXCLUDE
	PATTERN "*.tmpl.inl" EXCLUDE
	PATTERN "*.dir" EXCLUDE
	PATTERN "CMakeFiles" EXCLUDE
	)

install(
	FILES "${Lass_BINARY_DIR}/local/local_config.h"
	DESTINATION include/lass/config
	)

# LassConfig.cmake and friends
configure_file(
	"${Lass_SOURCE_DIR}/LassConfig.cmake.in"
	"${Lass_BINARY_DIR}/LassConfig.cmake"
	@ONLY
	)

include(CMakePackageConfigHelpers)
write_basic_package_version_file(
	"${Lass_BINARY_DIR}/LassConfigVersion.cmake"
	VERSION "${Lass_VERSION}"
	COMPATIBILITY AnyNewerVersion
	)

configure_file(
	"${Lass_SOURCE_DIR}/cmake/FindPythonCompat.cmake"
	"${Lass_BINARY_DIR}/FindPythonCompat.cmake"
	COPYONLY
	)

install(
	FILES
		"${Lass_BINARY_DIR}/LassConfig.cmake"
		"${Lass_BINARY_DIR}/LassConfigVersion.cmake"
		"${Lass_BINARY_DIR}/FindPythonCompat.cmake"
	DESTINATION share/Lass
	)


# Config file for Python-based build systems
configure_file(
	"${Lass_SOURCE_DIR}/LassConfig.py.in"
	"${Lass_BINARY_DIR}/tmp/LassConfig.py.tmp"
	@ONLY
	)

file(
	GENERATE
	OUTPUT "${Lass_BINARY_DIR}/LassConfig.py"
	INPUT "${Lass_BINARY_DIR}/tmp/LassConfig.py.tmp"
	)

install(
	FILES "${Lass_BINARY_DIR}/LassConfig.py"
	DESTINATION share/Lass
	)


# Export build tree targets, so that you can build against the build-tree through LassConfig.cmake
export(
	TARGETS lass
	NAMESPACE Lass::
	FILE ${Lass_BINARY_DIR}/LassBuildExports.cmake
	)
if(Python_Development_FOUND)
	export(
		TARGETS lass_python pylass
		NAMESPACE Lass::
		APPEND FILE ${Lass_BINARY_DIR}/LassBuildExports.cmake
		)
endif()


# --- PACKAGING ---
if (WIN32)
	set (CPACK_PACKAGE_INSTALL_DIRECTORY "Cocamware/Lass-${Lass_VERSION_MAJOR}.${Lass_VERSION_MINOR}")
endif(WIN32)
set(CPACK_PACKAGE_NAME "Lass")
set(CPACK_PACKAGE_VENDOR "Cocamware")
set(CPACK_PACKAGE_DESCRIPTION_SUMMARY "Lass - Library of Assembled Sources")
set(CPACK_PACKAGE_VERSION "${Lass_VERSION}")
set(CPACK_PACKAGE_VERSION_MAJOR "${Lass_VERSION_MAJOR}")
set(CPACK_PACKAGE_VERSION_MINOR "${Lass_VERSION_MINOR}")
set(CPACK_PACKAGE_VERSION_PATCH "${Lass_VERSION_PATCH}")
set(CPACK_RESOURCE_FILE_LICENSE "${Lass_SOURCE_DIR}/docs/license.txt")
set(CPACK_NSIS_DISPLAY_NAME "${CPACK_PACKAGE_INSTALL_DIRECTORY} Lass")
set(CPACK_NSIS_HELP_LINK "https:\\\\\\\\lass.cocamware.com")
set(CPACK_NSIS_URL_INFO_ABOUT "https:\\\\\\\\lass.cocamware.com")
set(CPACK_NSIS_CONTACT "info@cocamware.com")
include(CPack)
