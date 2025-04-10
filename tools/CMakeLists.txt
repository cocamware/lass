if ("${Python_VERSION_MAJOR}.${Python_VERSION_MINOR}" VERSION_GREATER_EQUAL "3.10" AND CMAKE_SIZEOF_VOID_P EQUAL 8)
	
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
