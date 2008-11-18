set(sourcedir ${CMAKE_SOURCE_DIR}/src/test)

file(GLOB test_HDRS ${sourcedir}/*.h)
file(GLOB test_SRCS ${sourcedir}/*.cpp)

set(generator ${CMAKE_SOURCE_DIR}/tools/param_expander.py)
set(infile ${sourcedir}/test_util_callback.cpp.in)
set(outfile ${sourcedir}/test_util_callback.cpp)
add_custom_command(
	OUTPUT ${outfile}
	DEPENDS ${infile} ${generator}
	COMMAND ${PYTHON_EXECUTABLE}
	ARGS ${generator} ${infile} ${outfile} 3
	)

foreach(fname ${test_SRCS})
	if(${fname} MATCHES "/test_.*\\.cpp")
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
list(APPEND test_SRCS ${outfile})

include_directories(${sourcedir})

list(REMOVE_DUPLICATES test_SRCS)
add_executable(
	test_suite 
	${test_HDRS} 
	${test_SRCS}
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

if(BUILD_USING_PRECOMPILED_HEADERS)
	if(MSVC_IDE)
		set_target_properties(
			test_suite
			PROPERTIES
			COMPILE_FLAGS /Yutest_common.h
			)
		set_source_files_properties(
			${sourcedir}/test_common.cpp 
			PROPERTIES
			COMPILE_FLAGS /Yctest_common.h
			)
	else()
		#set(pchfile ${test_suite_BINARY_DIR}/local/test_common.pch)
		#add_custom_command(
		#	OUTPUT ${pchfile}
		#	DEPENDS ${hdrfile}
		#	COMMAND ...)
		#set_source_files_properties(
		#	${test_SRCS}
		#	PROPERTIES
		#	OBJECT_DEPENDS ${pchfile})
	endif()
endif()

foreach(test_name ${unit_tests})
	add_test(${test_name} test_suite --log=${test_name}.log --input-dir=${sourcedir} ${test_name})
endforeach()
