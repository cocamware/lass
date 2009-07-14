project(ex_exposingClass)
cmake_minimum_required(VERSION 2.6)
include(../../../FindLassForExamples.cmake)
include_directories(${lass_INCLUDE_DIRS})

set(ex_exposingClass_SRCS embedding/embedding.cpp)

add_library(embedding2 SHARED ${ex_exposingClass_SRCS})
if(WIN32)
	set_target_properties(
		embedding2 PROPERTIES 
		SUFFIX ".pyd"
		DEBUG_POSTFIX "_d"
	)
else()
	set_target_properties(
		embedding2 PROPERTIES 
		DEBUG_POSTFIX "_d"
	)
endif()
target_link_libraries(embedding2 lass)
