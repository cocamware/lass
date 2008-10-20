file(GLOB test_HDRS *.h)
file(GLOB test_SRCS *.cpp)

set(sourcedir ${CMAKE_SOURCE_DIR}/test)

set(generator ${CMAKE_SOURCE_DIR}/../tools/param_expander.py)
set(infile ${sourcedir}/test_util_callback.cpp.in)
set(outfile ${sourcedir}/test_util_callback.cpp)
add_custom_command(
	OUTPUT ${outfile}
	DEPENDS ${infile} ${generator}
	COMMAND ${PYTHON_EXECUTABLE}
	ARGS ${generator} ${infile} ${outfile} 3)
list(REMOVE_ITEM test_SRCS ${outfile})
list(APPEND test_SRCS ${outfile})

foreach(fname ${test_SRCS})
	if(${fname} MATCHES "/test_.*\\.cpp")
		get_filename_component(test_name ${fname} NAME_WE)
		list(APPEND unit_tests ${test_name})
	endif()
endforeach()
list(REMOVE_ITEM unit_tests "test_common")

set(infile ${CMAKE_SOURCE_DIR}/test/auto_test_suite.cpp.in)
set(outfile ${CMAKE_SOURCE_DIR}/test/auto_test_suite.cpp)
add_custom_command(
	OUTPUT ${outfile}
	DEPENDS ${infile} ${generator}
	COMMAND ${PYTHON_EXECUTABLE}
	ARGS ${generator} ${infile} ${outfile} ${unit_tests})
list(REMOVE_ITEM test_SRCS ${outfile})
list(APPEND test_SRCS ${outfile})

include_directories(${CMAKE_SOURCE_DIR}/test)

add_executable(test_suite ${test_HDRS} ${test_SRCS})
target_link_libraries(test_suite lass optimized ${PYTHON_LIBRARIES} debug ${PYTHON_DEBUG_LIBRARIES})

if(MSVC)
	option(USE_PRECOMPILED_HEADERS "Build using precompiled headers" ON)
	if(USE_PRECOMPILED_HEADERS)
		set_target_properties(
			test_suite
			PROPERTIES
			COMPILE_FLAGS /Yutest_common.h)
		set_source_files_properties(
			${CMAKE_SOURCE_DIR}/test/test_common.cpp 
			PROPERTIES
			COMPILE_FLAGS /Yctest_common.h)
	endif()
endif()

foreach(test_name ${unit_tests})
	add_test(${test_name} test_suite --output=${test_name}.log ${test_name})
endforeach()
