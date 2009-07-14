project(ex_exposingOperators)
cmake_minimum_required(VERSION 2.6)
include(../../../FindLassForExamples.cmake)
include_directories(${lass_INCLUDE_DIRS})

set(ex_exposingOperators_SRCS embedding/embedding.cpp)

add_library(embedding5 SHARED ${ex_exposingOperators_SRCS})
if(WIN32)
	set_target_properties(
		embedding5 PROPERTIES 
		SUFFIX ".pyd"
		DEBUG_POSTFIX "_d"
	)
else()
	set_target_properties(
		embedding5 PROPERTIES 
		DEBUG_POSTFIX "_d"
	)
endif()
target_link_libraries(embedding5 lass)
