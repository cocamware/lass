set(sourcedir "${CMAKE_CURRENT_SOURCE_DIR}")


file(GLOB test_suite_HDRS "${sourcedir}/*.h")
file(GLOB test_suite_SRCS "${sourcedir}/*.cpp")


macro(list_remove_regex listvar)
	set(_to_be_removed)
	foreach(_path ${${listvar}})
		foreach(_regex ${ARGN})
			if("${_path}" MATCHES "${_regex}")
				list(APPEND _to_be_removed "${_path}")
			endif()
		endforeach()
	endforeach()
	if (_to_be_removed)
		list(REMOVE_ITEM ${listvar} ${_to_be_removed})
	endif()
endmacro()

list_remove_regex(test_suite_SRCS
	"ipc_child\\\\.cpp$" # that's the source code for the child process.
	"dynamic_child\\\\.cpp$" # for test_util_dynamic_library
	)

if(NOT TARGET Lass::lass_python)
	list_remove_regex(test_suite_SRCS
		"foo\\\\.cpp$"
		"bar\\\\.cpp$"
		"python[^/\\\\\\\\]*\\\\.cpp$"
		)
endif()

if(NOT LASS_HAVE_STD_FILESYSTEM)
	list(FILTER test_suite_SRCS EXCLUDE REGEX "test_python_export_traits_filesystem\\.cpp$")
endif()
if(NOT LASS_HAVE_STD_OPTIONAL)
	list(FILTER test_suite_SRCS EXCLUDE REGEX "test_python_export_traits_optional\\.cpp$")
endif()
if(NOT LASS_HAVE_STD_VARIANT)
	list(FILTER test_suite_SRCS EXCLUDE REGEX "test_python_export_traits_variant\\.cpp$")
endif()
if(LASS_HAVE_AARCH64)
	list(FILTER test_suite_SRCS EXCLUDE REGEX "test_util_atomic\\.cpp$")
endif()

set(infile "${sourcedir}/test_util_callback.cpp.in")
set(outfile "${sourcedir}/test_util_callback.cpp")
lass_param_expander("${infile}" "${outfile}" 3)
list(APPEND test_suite_SRCS "${outfile}")

foreach(fname ${test_suite_SRCS})
	if(${fname} MATCHES "/test_[a-zA-Z0-9_]+\\.cpp")
		get_filename_component(test_name "${fname}" NAME_WE)
		list(APPEND unit_tests "${test_name}")
	endif()
endforeach()
list(REMOVE_DUPLICATES unit_tests)
list(REMOVE_ITEM unit_tests "test_common")
list(SORT unit_tests)

set(infile "${sourcedir}/auto_test_suite.cpp.in")
set(outfile "${Lass_BINARY_DIR}/local/auto_test_suite.cpp")
lass_param_expander("${infile}" "${outfile}" ${unit_tests})
list(APPEND test_suite_SRCS "${outfile}")

set(infile "${sourcedir}/default_arguments.h.in")
set(outfile "${Lass_BINARY_DIR}/local/default_arguments.h")
configure_file("${infile}" "${outfile}")
list(APPEND test_suite_HDRS "${outfile}")

list(REMOVE_DUPLICATES test_suite_HDRS)
list(SORT test_suite_HDRS)
list(REMOVE_DUPLICATES test_suite_SRCS)
list(SORT test_suite_SRCS)


add_executable(
	test_ipc_child
	WIN32 # /SUBSYSTEM:WINDOWS
	"${sourcedir}/ipc_child.cpp"
	"${sourcedir}/ipc_protocol.h"
)
target_include_directories(test_ipc_child
	PRIVATE "${sourcedir}"
	PRIVATE "${Lass_BINARY_DIR}/local"
	)
target_link_libraries(test_ipc_child
	Lass::lass
	)


add_library(
	test_dynamic_child MODULE
	"${sourcedir}/dynamic_child.cpp"
)


add_executable(test_suite
	${test_suite_HDRS} 
	${test_suite_SRCS}
	)
target_include_directories(test_suite
	PRIVATE "${sourcedir}"
	PRIVATE "${Lass_BINARY_DIR}/local"
	)
target_link_libraries(test_suite
	PRIVATE Lass::lass
	)
if(TARGET Lass::lass_python)
	target_link_libraries(test_suite
		PRIVATE Lass::lass_python Python::Python
	)
	if(MSVC_IDE)
		set_property(TARGET test_suite PROPERTY
			VS_DEBUGGER_ENVIRONMENT "PATH=$<TARGET_FILE_DIR:Python::Python>;%PATH%"
		)
	endif()
	target_compile_definitions(test_suite
		PRIVATE "TEST_PYTHONHOME=\"${Lass_PYTHON_BASE_PREFIX}\""
	)
	if(NOT WIN32)
		target_link_options(test_suite
			PRIVATE -rdynamic
		)
	endif()
endif()
lass_add_precompiled_header(test_suite 
	test_common.h
	"${sourcedir}/test_common.cpp"
	)
target_compile_definitions(test_suite
	PRIVATE "TEST_IPC_CHILD=\"$<TARGET_FILE:test_ipc_child>\"" # for test_io_ipc
	PRIVATE "TEST_DYNAMIC_CHILD=\"$<TARGET_FILE:test_dynamic_child>\"" # for test_util_dynamic_library
	)

foreach(test_name ${unit_tests})
	add_test(NAME "${test_name}" COMMAND test_suite "--log=${test_name}.log" "--input-dir=${sourcedir}" "${test_name}")
endforeach()

if (WIN32 AND TARGET Lass::lass_python)
	# Add directory of Python DLL to PATH environment variable so the tests
	# can actually run. Works best with CMake 3.12 and newer, since it also
	# searches the runtime libraries.
	#
	# the string replacements are required to avoid ; and \ being interpreted
	# differently than part of the PATH variable.
	#
	set(_path "$<TARGET_FILE_DIR:Python::Python>;$ENV{PATH}")
	string(REPLACE "\\" "/" _path "${_path}")
	string(REPLACE ";" "\\;" _path "${_path}")
	set_tests_properties(${unit_tests}
		PROPERTIES
		ENVIRONMENT "PATH=${_path}"
		)
endif()

if (Lass_WITH_STUBGEN)
	Lass_generate_stubs(test_suite
		SOURCES
			python_embedding.cpp
			python_shadow.cpp
			foo.cpp
			bar.cpp
		OUTPUT_DIRECTORY
			"${sourcedir}"
	)

	if (WIN32)
		set(_venv_python "${_Lass_stubgen_venv}/Scripts/python.exe")
	else()
		set(_venv_python "${_Lass_stubgen_venv}/bin/python")
	endif()
	set(_requirements "${CMAKE_CURRENT_SOURCE_DIR}/requirements.txt")
	set(_constraints "${CMAKE_CURRENT_SOURCE_DIR}/requirements.txt")
	set(_stamp_file "${_Lass_stubgen_venv}/.test_suite.stamp")
	if ("${_requirements}" IS_NEWER_THAN "${_stamp_file}" OR "${_constraints}" IS_NEWER_THAN "${_stamp_file}")
		_Lass_checked_process("${_venv_python}" -m pip install --requirement "${_requirements}" --constraint "${_constraints}")
		file(TOUCH "${_stamp_file}")
	endif()

	# Add a test for the generated stubs by testing mypy and ruff on test_python_embedding.py
	add_test(NAME "test_python_embedding_mypy" COMMAND "${_venv_python}" -m mypy "${CMAKE_CURRENT_SOURCE_DIR}/test_python_embedding.py")
	add_test(NAME "test_python_embedding_ruff" COMMAND "${_venv_python}" -m ruff check "${CMAKE_CURRENT_SOURCE_DIR}/test_python_embedding.py")
endif()
