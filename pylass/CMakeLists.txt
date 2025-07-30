file(GLOB pylass_HDRS *.h *.inl)
file(GLOB pylass_SRCS *.cpp)

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
	DEBUG_POSTFIX "${LASS_PYTHON_DEBUG_POSTFIX}"
	)
if (WIN32)
	set_target_properties(pylass
		PROPERTIES 
		SUFFIX ".pyd"
		)
endif()
lass_add_precompiled_header(pylass
	pylass_common.h
	"${Lass_SOURCE_DIR}/pylass/pylass_common.cpp"
	)

if (WIN32)
	set(_destination "bin")
else()
	set(_destination "lib")
endif()
set_target_properties(pylass
	PROPERTIES 
	LIBRARY_OUTPUT_DIRECTORY "${Lass_BINARY_DIR}/${_destination}"
	)
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

if (Lass_WITH_STUBGEN)
	Lass_generate_stubs(pylass)
	install(
		FILES "$<TARGET_FILE_DIR:pylass>/lass.pyi"
		DESTINATION "${_destination}"
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
