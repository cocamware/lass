if (NOT WIN32)
	add_executable(double_double_gen
		double_double_gen.cpp
		)
	target_compile_definitions(double_double_gen
		PRIVATE "LASS_SOURCE_DIR=\"${Lass_SOURCE_DIR}\""
	)
	set_property(TARGET double_double_gen
		PROPERTY CXX_EXTENSIONS ON
		)
	target_link_libraries(double_double_gen
		PRIVATE Lass::lass quadmath
		)
endif()
