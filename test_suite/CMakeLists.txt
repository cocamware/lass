set(sourcedir ${lass_SOURCE_DIR}/test_suite)

file(GLOB test_suite_HDRS ${sourcedir}/*.h)
file(GLOB test_suite_SRCS ${sourcedir}/*.cpp)

set(generator ${lass_SOURCE_DIR}/tools/param_expander.py)
set(infile ${sourcedir}/test_util_callback.cpp.in)
set(outfile ${sourcedir}/test_util_callback.cpp)
add_custom_command(
	OUTPUT ${outfile}
	DEPENDS ${infile} ${generator}
	COMMAND ${PYTHON_EXECUTABLE}
	ARGS ${generator} ${infile} ${outfile} 3
	)

foreach(fname ${test_suite_SRCS})
	if(${fname} MATCHES "/test_[a-zA-Z0-9_]+\\.cpp")
		get_filename_component(test_name ${fname} NAME_WE)
		list(APPEND unit_tests ${test_name})
	endif()
endforeach()
list(REMOVE_DUPLICATES unit_tests)
list(REMOVE_ITEM unit_tests "test_common")

set(infile ${sourcedir}/auto_test_suite.cpp.in)
set(outfile ${lass_BINARY_DIR}/local/auto_test_suite.cpp)
add_custom_command(
	OUTPUT ${outfile}
	DEPENDS ${infile} ${generator}
	COMMAND ${PYTHON_EXECUTABLE}
	ARGS ${generator} ${infile} ${outfile} ${unit_tests}
	)
list(APPEND test_suite_SRCS ${outfile})

include_directories(${sourcedir})

list(REMOVE_DUPLICATES test_suite_SRCS)
add_executable(
	test_suite 
	${test_suite_HDRS} 
	${test_suite_SRCS}
	)
target_link_libraries(
	test_suite 
	lass 
	${lass_LIBS}
	optimized ${PYTHON_LIBRARIES} 
	debug ${PYTHON_DEBUG_LIBRARIES}
	)
set_target_properties(
	test_suite PROPERTIES
        COMPILE_FLAGS ${lass_C_FLAGS}
	)
add_precompiled_header(
	test_suite 
	test_common.h 
	${sourcedir}/test_common.cpp
	${test_suite_SRCS}
	)

foreach(test_name ${unit_tests})
	add_test(${test_name} test_suite --log=${test_name}.log --input-dir=${sourcedir} ${test_name})
endforeach()
