project(ex_exposingContainers)
cmake_minimum_required(VERSION 2.6)
include(../../../FindLassForExamples.cmake)
include_directories(${LassPython_INCLUDE_DIRS})

set(ex_exposingContainers_SRCS embedding/embedding.cpp)

add_library(embedding4 SHARED ${ex_exposingContainers_SRCS})
if(WIN32)
	set_target_properties(
		embedding4 PROPERTIES 
		SUFFIX ".pyd"
		DEBUG_POSTFIX "_d"
	)
else()
	set_target_properties(
		embedding4 PROPERTIES 
		DEBUG_POSTFIX "_d"
	)
endif()
target_link_libraries(embedding4 lass_python)
