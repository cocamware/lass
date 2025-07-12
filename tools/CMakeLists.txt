if (Lass_WITH_STUBGEN)
	
	install(
		DIRECTORY "stubgen"
		DESTINATION share/Lass
		PATTERN "*.pyc" EXCLUDE
		PATTERN "__pycache__" EXCLUDE
	)

	install(
		FILES
			"LassStubgen.cmake"
			"LassClang.cmake"
			"_get_libclang_version.py"
		DESTINATION share/Lass
	)

endif()
