project(ex_exposingClassHierarchies)
cmake_minimum_required(VERSION 2.6)
include(../../../FindLassForExamples.cmake)
include_directories(${LassPython_INCLUDE_DIRS})

set(ex_exposingClassHierarchies_SRCS embedding/embedding.cpp)

add_library(embedding3 SHARED ${ex_exposingClassHierarchies_SRCS})
if(WIN32)
	set_target_properties(
		embedding3 PROPERTIES 
		SUFFIX ".pyd"
		DEBUG_POSTFIX "_d"
	)
else()
	set_target_properties(
		embedding3 PROPERTIES 
		DEBUG_POSTFIX "_d"
	)
endif()
target_link_libraries(embedding3 lass_python)
