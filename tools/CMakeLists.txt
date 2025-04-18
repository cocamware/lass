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
		DESTINATION share/Lass
	)

endif()
