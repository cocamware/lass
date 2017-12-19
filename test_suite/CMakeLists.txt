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
	)

if(NOT PYTHONLIBS_FOUND)
	list_remove_regex(test_suite_SRCS
		"foo\\\\.cpp$"
		"bar\\\\.cpp$"
		"python[^/\\\\\\\\]*\\\\.cpp$"
		"dynamic[^/\\\\\\\\]*\\\\.cpp$" # because it tries to load pylass
		)
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
if(PYTHONLIBS_FOUND)
	target_link_libraries(test_suite
		PRIVATE Lass::lass_python
	)
endif()
lass_add_precompiled_header(test_suite 
	test_common.h
	"${sourcedir}/test_common.cpp"
	)
target_compile_definitions(test_suite
	PRIVATE "TEST_IPC_CHILD=\"$<TARGET_FILE:test_ipc_child>\""
	)

foreach(test_name ${unit_tests})
	add_test(NAME "${test_name}" COMMAND test_suite "--log=${test_name}.log" "--input-dir=${sourcedir}" "${test_name}")
endforeach()
