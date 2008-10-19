file(GLOB
	test_sources
	test_*.cpp)

set(testdir ${CMAKE_SOURCE_DIR}/test)
set(generator ${testdir}/auto_suite_map.py)
set(inpath ${testdir}/auto_suite_map.cpp.in)
set(outpath ${CMAKE_BINARY_DIR}/test/auto_suite_map.cpp)
add_custom_command(
	OUTPUT ${outpath}
	DEPENDS ${inpath} ${generator}
	COMMAND ${PYTHON_EXECUTABLE}
	ARGS ${generator} -i ${inpath} -o ${outpath} -t ${testdir})

include_directories(${CMAKE_SOURCE_DIR}/test)
add_executable(test test.cpp unit_test.cpp ${outpath} ${test_sources})
target_link_libraries(test lass optimized ${PYTHON_LIBRARIES} debug ${PYTHON_DEBUG_LIBRARIES})

foreach(test_source ${test_sources})
	get_filename_component(test_name ${test_source} NAME_WE)
	add_test(${test_name} test --output=${test_name}.log ${test_name})
endforeach()
