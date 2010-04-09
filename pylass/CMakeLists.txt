project(pylass)
cmake_minimum_required(VERSION 2.6)

set(CMAKE_DEBUG_POSTFIX _d)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${pylass_BINARY_DIR}/bin")
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "${pylass_BINARY_DIR}/bin")

find_package(Lass REQUIRED)
include_directories(${Lass_INCLUDE_DIRS})
foreach(_config RELEASE DEBUG MINSIZEREL RELWITHDEBINFO)
	get_target_property(lass_location_${_config} lass IMPORTED_LOCATION_${_config})
	install(
		FILES "${lass_location_${_config}}"
		DESTINATION .
		CONFIGURATIONS ${_config}
		)
endforeach()

set (CMAKE_INSTALL_DEBUG_LIBRARIES 1)
set (CMAKE_INSTALL_SYSTEM_RUNTIME_LIBS_SKIP 1)
include(InstallRequiredSystemLibraries)
if (CMAKE_INSTALL_SYSTEM_RUNTIME_LIBS)
	install(
		PROGRAMS ${CMAKE_INSTALL_SYSTEM_RUNTIME_LIBS}
		DESTINATION .
		)
endif()
if (CMAKE_INSTALL_SYSTEM_RUNTIME_LIBS_DEBUG)
	install(
		PROGRAMS ${CMAKE_INSTALL_SYSTEM_RUNTIME_LIBS_DEBUG}
		DESTINATION .
		)
endif()

if (CMAKE_INSTALL_PREFIX_INITIALIZED_TO_DEFAULT)
	if (Lass_PYTHON_EXECUTABLE)
		execute_process(
			COMMAND "${Lass_PYTHON_EXECUTABLE}" -c "from distutils import sysconfig; print sysconfig.get_python_lib(True)" 
			OUTPUT_VARIABLE _python_site_packages
			OUTPUT_STRIP_TRAILING_WHITESPACE
		)
		set (CMAKE_INSTALL_PREFIX "${_python_site_packages}" CACHE PATH "liar install prefix" FORCE)
	endif()
endif ()

file(GLOB pylass_HDRS *.h *.inl)
file(GLOB pylass_SRCS *.cpp)

add_library(pylass MODULE 
	${pylass_HDRS} 
	${pylass_SRCS}
	)
target_link_libraries(pylass 
	lass
	)
if (WIN32)
	set_target_properties(
		pylass
		PROPERTIES 
		OUTPUT_NAME "lass"
		SUFFIX ".pyd"
		)
endif()
install(
	TARGETS pylass
	DESTINATION .
	)
	
configure_file(
	"package.py.in"
	"${pylass_BINARY_DIR}/package.py"
	)

if (MSVC_IDE)
	set(_configuration "$(ConfigurationName)")
	set(_bindir "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/$(OutDir)")
else()
	if (CMAKE_BUILD_TYPE)
		set(_configuration "${CMAKE_BUILD_TYPE}")
	else()
		set(_configuration "Release")
	endif()
	set(_bindir "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}")
endif()

add_custom_target(
	BDIST
	COMMAND ${Lass_PYTHON_EXECUTABLE} "${pylass_BINARY_DIR}/package.py" "${_configuration}" "${_bindir}"  "bdist_wininst" "--target-version=2.6"
	DEPENDS pylass
	SOURCES "package.py.in"
	)