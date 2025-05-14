#[======================================================================[.rst:
LassStubgen
------------------------

.. versionadded:: 3.7

Automatically generate Python *.pyi stub files from Lass Python bindings.

Module Functions
^^^^^^^^^^^^^^^^

.. command:: Lass_generate_stubs

  .. code-block:: cmake

    Lass_generate_stubs(<target-name>
      [SOURCES <sources>...]
      [EXTRA_TARGETS <target-names>...]
      [OUTPUT_DIRECTORY <output-dir>]
      [PACKAGE <package>]
      [EXPORT <json-file>]
      [IMPORT <json-files>...]
      [WITH_SIGNATURES]
      [NUM_THREADS <num-threads>]
      )

  The ``android_add_test_data`` function is used to copy files and libraries
  needed to run project-specific tests. On the host operating system, this is
  done at build time. For on-device testing, the files are loaded onto the
  device by the manufactured test at run time.

  This function accepts the following named parameters:

  ``SOURCES <files>...``
    One or more C++ source files with Lass Python bindings.
    If not provided, all sources files in the target are used.
  ``EXTRA_TARGETS <target-names>...``
    Extra targets from which to parse source files.
  ``OUTPUT_DIRECTORY <output-dir>``
    Absolute path where the *.pyi stub files are written to.
  ``PACKAGE <package-name>``
    Name of the package the stub files are part of.
  ``EXPORT <export-name>``
    Path to a json file containing the raw stub data.
  ``IMPORT <import-name>``
    Path to a json file containing exported stub data.
    This is used if a module depends on stub data from another module.
  ``WITH_SIGNATURES``
    Add C++ signatures as comments to *.pyi files.
  ``MAX_THREADS <max-threads>``
    Parse source files in parallel. If ``<max-threads>`` is 0, then the
    CPU count is used instead. By default, this is 0.

#]======================================================================]

set(LASS_STUBGEN "${CMAKE_CURRENT_LIST_DIR}/stubgen/lass_stubgen")

set(_Lass_venv_stubgen "${CMAKE_BINARY_DIR}/.venv-lass-stubgen")
set(_Lass_venv_stubgen_requirement "${CMAKE_CURRENT_LIST_DIR}/stubgen/requirements.txt")


function(Lass_generate_stubs target)
	set(_prefix)
	set(_options WITH_SIGNATURES)
	set(_one_value_keywords OUTPUT_DIRECTORY PACKAGE EXPORT MAX_THREADS DEBUG_CONNECT)
	set(_multi_value_keywords SOURCES EXTRA_TARGETS IMPORT)
	cmake_parse_arguments("${_prefix}" "${_options}" "${_one_value_keywords}" "${_multi_value_keywords}" ${ARGN})

	set(cxx_standard "--arg=-std=c++$<TARGET_PROPERTY:${target},CXX_STANDARD>")

	set(defines)
	set(includes)
	set(object_files)
	set(sources)
	foreach(tgt "${target}" ${_EXTRA_TARGETS})
		if (NOT TARGET "${tgt}")
			message(FATAL_ERROR "Target ${tgt} does not exist.")
		endif()
	
		set(_defines "$<TARGET_PROPERTY:${tgt},COMPILE_DEFINITIONS>")
		set(_defines "$<$<BOOL:${_defines}>:-D$<JOIN:${_defines},;-D>>")
		list(APPEND defines "${_defines}")

		set(_define_symbol "$<TARGET_PROPERTY:${tgt},DEFINE_SYMBOL>")
		set(_define_symbol "$<IF:$<BOOL:${define_symbol}>,${define_symbol},${tgt}_EXPORTS>") # default to target_EXPORTS
		list(APPEND defines "-D${_define_symbol}")

		set(_includes "$<TARGET_PROPERTY:${tgt},INCLUDE_DIRECTORIES>")
		set(_includes "$<$<BOOL:${_includes}>:-I$<JOIN:${_includes},;-I>>")
		list(APPEND includes "${_includes}")

		if(CMAKE_CXX_IMPLICIT_INCLUDE_DIRECTORIES)
			set(_includes ${CMAKE_CXX_IMPLICIT_INCLUDE_DIRECTORIES})
			set(_includes "$<$<BOOL:${_includes}>:-isystem;$<JOIN:${_includes},;-isystem;>>")
			list(APPEND includes "${_includes}")
		endif()

		set(_object_files "$<TARGET_OBJECTS:${tgt}>")
		set(_object_files "$<$<BOOL:${_object_files}>:--obj=$<JOIN:${_object_files},;--obj=>>")
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
		set(_package "--package=${_PACKAGE}")
	else()
		set(_package)
	endif()
	if(_EXPORT)
		set(_export "--export=${_EXPORT}")
	else()
		set(_export)
	endif()
	set(_imports)
	foreach(import ${_IMPORT})
		set(_imports "--import=${import}")
	endforeach()
	if(_WITH_SIGNATURES)
		set(_with_signatures "--with-signatures")
	else()
		set(_with_signatures)
	endif()
	if(_MAX_THREADS)
		set(_max_threads "--num-threads=${_MAX_THREADS}")
	else()
		set(_max_threads)
	endif()

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

	if(_DEBUG_CONNECT)
		set(_debug_connect "--debug-connect=${_DEBUG_CONNECT}")
		_Lass_checked_process("${_venv_python}" -m pip install --upgrade debugpy)
	else()
		set(_debug_connect)
	endif()

	set(_args
		${_SOURCES}
		"${_max_threads}"
		"${_debug_connect}"
		"${_package}"
		"${_imports}"
		"${_export}"
		"--output-dir=${_OUTPUT_DIRECTORY}"
		"${_with_signatures}"
		"--cache-dir=${CMAKE_BINARY_DIR}/.lass_stubgen_cache"
		"${cxx_standard}"
		${defines}
		${includes}
		${object_files}
	)
	if (MSVC_IDE)
		set(_args_file "${CMAKE_CURRENT_BINARY_DIR}/${target}.dir/$<CONFIG>/lass-stubgen-args.txt")
	else()
		set(_args_file "${CMAKE_CURRENT_BINARY_DIR}/CMakeFiles/${target}.dir/lass-stubgen-args.txt")
	endif()
	file(GENERATE OUTPUT "${_args_file}"
		CONTENT "$<JOIN:${_args},\n>\n"
	)

	# Add POST_BUILD command to target
	add_custom_command(TARGET "${target}" POST_BUILD
		COMMAND "${_venv_python}"
		ARGS "-B" "${LASS_STUBGEN}" "@${_args_file}"
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
