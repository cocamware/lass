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
else()
	if (CMAKE_BUILD_TYPE)
		set(_configuration "${CMAKE_BUILD_TYPE}")
	else()
		set(_configuration "Release")
	endif()
endif()


if (MSVC_IDE)
	set(pylass_dist_files)
	foreach(_config ${CMAKE_CONFIGURATION_TYPES})
		get_target_property(_lass_loc lass LOCATION_${_config})
		get_target_property(_pylass_loc pylass LOCATION_${_config})
		set(pylass_dist_files "r'${_config}': [r'${_lass_loc}', r'${_pylass_loc}'],\n ${pylass_dist_files}")
	endforeach()
else()
	get_target_property(_lass_loc lass LOCATION)
	get_target_property(_pylass_loc pylass LOCATION)
	set(pylass_dist_files "r'${_configuration}': [r'${_lass_loc}', r'${_pylass_loc}']")
endif()


configure_file(
	"package.py.in"
	"${CMAKE_CURRENT_BINARY_DIR}/package.py"
	)


add_custom_target(
	BDIST_WININST
	COMMAND ${PYTHON_EXECUTABLE} "${CMAKE_CURRENT_BINARY_DIR}/package.py" "${_configuration}" "bdist_wininst"
	DEPENDS pylass
	SOURCES "package.py.in"
	)
