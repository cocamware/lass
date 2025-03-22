set(LASS_STUBGEN "${CMAKE_CURRENT_LIST_DIR}/stubgen.py")

set(_Lass_venv_stubgen "${CMAKE_BINARY_DIR}/.venv-lass-stubgen")
set(_Lass_venv_stubgen_requirement "${CMAKE_CURRENT_LIST_DIR}/requirements.txt")


function(Lass_generate_stubs target)
	set(_prefix)
	set(_options)
	set(_one_value_keywords OUTPUT_DIRECTORY EXPORT PACKAGE)
	set(_multi_value_keywords SOURCES EXTRA_TARGETS IMPORT)
	cmake_parse_arguments("${_prefix}" "${_options}" "${_one_value_keywords}" "${_multi_value_keywords}" ${ARGN})

	set(cxx_standard "-std=c++$<TARGET_PROPERTY:${target},CXX_STANDARD>")

	set(defines)
	set(includes)
	set(object_files)
	set(sources)
	foreach(tgt "${target}" ${_EXTRA_TARGETS})
		if (NOT TARGET "${tgt}")
			message(FATAL_ERROR "Target ${tgt} does not exist.")
		endif()
	
		set(_defines "$<TARGET_PROPERTY:${tgt},COMPILE_DEFINITIONS>")
		set(_defines "$<$<BOOL:${_defines}>:-I$<JOIN:${_defines},;-D>>")
		list(APPEND defines "${_defines}")

		set(_define_symbol "$<TARGET_PROPERTY:${tgt},DEFINE_SYMBOL>")
		set(_define_symbol "$<IF:$<BOOL:${define_symbol}>,${define_symbol},${tgt}_EXPORTS>") # default to target_EXPORTS
		list(APPEND defines "-D${_define_symbol}")

		set(_includes "$<TARGET_PROPERTY:${tgt},INCLUDE_DIRECTORIES>")
		set(_includes "$<$<BOOL:${_includes}>:-I$<JOIN:${_includes},;-I>>")
		list(APPEND includes "${_includes}")

		set(_object_files "$<TARGET_OBJECTS:${tgt}>")
		set(_object_files "$<$<BOOL:${_object_files}>:-I$<JOIN:${_object_files},;--obj=>>")
		list(APPEND object_files "${_object_files}")
	endforeach()

	if(NOT _SOURCES)
		foreach(tgt "${target}" ${_EXTRA_TARGETS})
			get_target_property(sources "${tgt}" SOURCES)
			foreach(source ${sources})
				get_source_file_property(language "${source}" LANGUAGE)
				get_source_file_property(header_only "${source}" HEADER_FILE_ONLY)
				if (language STREQUAL "CXX" AND NOT header_only)
					list(APPEND _SOURCES "${source}")
				endif()
			endforeach()
		endforeach()
	endif()

	if(NOT _OUTPUT_DIRECTORY)
		set(_OUTPUT_DIRECTORY "$<TARGET_FILE_DIR:${target}>")
	endif()
	if(_PACKAGE)
		set(_package "--package" "${_PACKAGE}")
	else()
		set(_package)
	endif()
	if(_EXPORT)
		set(_export "--export" "${_EXPORT}")
	else()
		set(_export)
	endif()
	set(_imports)
	foreach(import ${_IMPORT})
		set(_imports "--import" "${import}")
	endforeach()


	# Create virtual environment to run stubgen
	if (WIN32)
		set(_venv_python "${_Lass_venv_stubgen}/Scripts/python.exe")
	else()
		set(_venv_python "${_Lass_venv_stubgen}/bin/python")
	endif()
	_Lass_get_base_prefix("${Python_EXECUTABLE}" _base_prefix)
	_Lass_get_base_prefix("${_venv_python}" _venv_base_prefix)
	if (NOT (_base_prefix STREQUAL _venv_base_prefix))
		message(STATUS "Creating venv for Lass stubgen: ${_Lass_venv_stubgen}...")
		_Lass_checked_process("${Python_EXECUTABLE}" -m venv "--clear" "${_Lass_venv_stubgen}"
		)
	endif()
	set(_stamp_file "${_Lass_venv_stubgen}/.stamp")
	if ("${_Lass_venv_stubgen_requirement}" IS_NEWER_THAN "${_stamp_file}")
		message(STATUS "Updating venv for Lass stubgen: ${_Lass_venv_stubgen}...")
		_Lass_checked_process("${_venv_python}" -m pip install --upgrade pip)
		_Lass_checked_process("${_venv_python}" -m pip install --requirement "${_Lass_venv_stubgen_requirement}")
		file(TOUCH "${_stamp_file}")
	endif()


	# Add POST_BUILD command to target
	add_custom_command(TARGET "${target}" POST_BUILD
		COMMAND "${_venv_python}"
			ARGS
				"${LASS_STUBGEN}"
				"${_package}"
				"${_imports}"
				"${_export}"
				"--output-dir" "${_OUTPUT_DIRECTORY}"
				"${cxx_standard}"
				"${defines}"
				"${includes}"
				"${object_files}"
				"${_SOURCES}"
		COMMAND_EXPAND_LISTS
		VERBATIM
		WORKING_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}"
		COMMENT "Generating Python stubs for ${target}..."
	)
endfunction()


function(_Lass_get_base_prefix python_executable base_prefix)
	execute_process(COMMAND "${python_executable}" -c "import sys; sys.stdout.write(sys.base_prefix.replace('\\\\', '/'))"
		OUTPUT_VARIABLE _base_prefix
		RESULT_VARIABLE _result
		OUTPUT_STRIP_TRAILING_WHITESPACE
	)
	if (_result)
		set(${base_prefix} "" PARENT_SCOPE)
	else()
		set(${base_prefix} "${_base_prefix}" PARENT_SCOPE)
	endif()
endfunction()


function(_Lass_checked_process)
	execute_process(
		COMMAND ${ARGV}
		RESULT_VARIABLE _result
	)
	if (_result)
		message(FATAL_ERROR "Command failed: ${_result}")
	endif()
endfunction()
