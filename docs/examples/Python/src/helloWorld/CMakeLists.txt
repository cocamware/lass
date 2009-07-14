project(ex_helloworld)
cmake_minimum_required(VERSION 2.6)
include(../../../FindLassForExamples.cmake)
include_directories(${lass_INCLUDE_DIRS})

set(ex_helloworld_SRCS embedding/embedding.cpp)

add_library(embedding1 SHARED ${ex_helloworld_SRCS})
if(WIN32)
	set_target_properties(
		embedding1 PROPERTIES 
		SUFFIX ".pyd"
		DEBUG_POSTFIX "_d"
	)
else()
	set_target_properties(
		embedding1 PROPERTIES 
		DEBUG_POSTFIX "_d"
	)
endif()
target_link_libraries(embedding1 lass)
