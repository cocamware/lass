file(GLOB pylass_HDRS *.h *.inl)
file(GLOB pylass_SRCS *.cpp)

if (LASS_PYTHON_HAS_DEBUG_BUILD)
    set(_pylass_debug_postfix "_d")
else()
    set(_pylass_debug_postfix`)
endif()

add_library(pylass MODULE
	${pylass_HDRS}
	${pylass_SRCS}
	)
add_library(Lass::pylass ALIAS pylass)
target_link_libraries(pylass
	PUBLIC Lass::lass_python
	)
set_target_properties(pylass
	PROPERTIES 
	OUTPUT_NAME "lass"
	PREFIX ""
	DEBUG_POSTFIX "${_pylass_debug_postfix}"
	)
if (WIN32)
	set_target_properties(pylass
		PROPERTIES 
		SUFFIX ".pyd"
		)
endif()
lass_add_precompiled_header(pylass
	pylass_common.h
	pylass_common.cpp
	)

if (WIN32)
	set(_destination "bin")
else()
	set(_destination "lib")
endif()

install(
	TARGETS pylass
	EXPORT LassExports
	LIBRARY DESTINATION "${_destination}"
	)

if(MSVC)
	install(
		FILES $<TARGET_PDB_FILE:pylass>
		DESTINATION "${_destination}"
		OPTIONAL
		)
endif()

if (WIN32)
    # bdist_wininst only works on windows
    # do we need binary distributions on other platforms?  I don't think so.
    if (MSVC_IDE)
        set(_target_name BDIST_WININST)
    else()
        set(_target_name bdist_wininst)
    endif()
   
    # configuring needed to bake in version.
    configure_file(
        "package.py.in"
        "${CMAKE_CURRENT_BINARY_DIR}/package.py"
        )
        
    # Using generator expressions $<TARGET_FILE:pylass> instead of getting the LOCATION property of the target.
    # The latter is deprecated in CMake 3.x (and gives a warning) because it's apparently not conclusive,
    # since it can only be known for sure at generation time.
    # This should also work with CMake 2.8, at least with 2.8.12. I haven't tested earlier versions.
    add_custom_target(
        ${_target_name}
        COMMAND "${PYTHON_EXECUTABLE}" "${CMAKE_CURRENT_BINARY_DIR}/package.py" "$<CONFIGURATION>" "$<TARGET_FILE:pylass>" "$<TARGET_FILE:lass>" "$<TARGET_FILE:lass_python>"
        DEPENDS pylass
        SOURCES "package.py.in"
        )
endif()