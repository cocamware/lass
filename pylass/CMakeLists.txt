file(GLOB pylass_HDRS *.h *.inl)
file(GLOB pylass_SRCS *.cpp)

add_library(pylass MODULE 
	${pylass_HDRS} 
	${pylass_SRCS}
	)
target_link_libraries(pylass 
	lass
	lass_python
	)
set_target_properties(
	pylass
	PROPERTIES 
	OUTPUT_NAME "lass"
	PREFIX ""
	)
if (WIN32)
	set_target_properties(
		pylass
		PROPERTIES 
		SUFFIX ".pyd"
		)
endif()
add_precompiled_header(
	pylass 
	pylass_common.h
	pylass_common.cpp
	${pylass_SRCS}
	)

install(
	TARGETS pylass
	EXPORT LassExports
	RUNTIME DESTINATION bin
	LIBRARY DESTINATION lib
	ARCHIVE DESTINATION lib
	)

if (MSVC_IDE)
	set(_configuration "$(ConfigurationName)")
	set(_configurations ${CMAKE_CONFIGURATION_TYPES})
else()
	if (CMAKE_BUILD_TYPE)
		set(_configuration "${CMAKE_BUILD_TYPE}")
	else()
		set(_configuration "Release")
	endif()
	set(_configurations ${_configuration})
endif()


set(pylass_dist_files)
foreach(_config ${_configurations})
	get_target_property(_pylass_loc pylass LOCATION_${_config})
	get_target_property(_lass_loc lass LOCATION_${_config})
	get_target_property(_lass_python_loc lass_python LOCATION_${_config})
	set(pylass_dist_files "r'${_config}': [r'${_pylass_loc}', r'${_lass_loc}', r'${_lass_python_loc}'],\n ${pylass_dist_files}")
endforeach()


configure_file(
	"package.py.in"
	"${CMAKE_CURRENT_BINARY_DIR}/package.py"
	)


if (MSVC_IDE)
	set(_target_name BDIST_WININST)
else()
	set(_target_name bdist_wininst)
endif()
add_custom_target(
	${_target_name}
	COMMAND ${PYTHON_EXECUTABLE} "${CMAKE_CURRENT_BINARY_DIR}/package.py" "${_configuration}" "bdist_wininst"
	DEPENDS pylass
	SOURCES "package.py.in"
	)
