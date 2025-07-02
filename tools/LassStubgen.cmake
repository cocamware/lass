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
      [OUTPUT_DIRECTORY <output-dir>]
      [PACKAGE <package>]
      [EXPORT <json-file>]
      [IMPORT <json-files>...]
      [WITH_SIGNATURES]
      )

  The ``android_add_test_data`` function is used to copy files and libraries
  needed to run project-specific tests. On the host operating system, this is
  done at build time. For on-device testing, the files are loaded onto the
  device by the manufactured test at run time.

  This function accepts the following named parameters:

  ``SOURCES <files>...``
    One or more C++ source files with Lass Python bindings.
    If not provided, all sources files in the target are used.
    Previously exported stub data can also be used as sources, in which case they are
    loaded as if they were parsed from source files directly, i.e. not as imported
    modules.
  ``OUTPUT_DIRECTORY <output-dir>``
    Absolute path where the *.pyi stub files are written to.
  ``PACKAGE <package-name>``
    Name of the package the stub files are part of.
  ``EXPORT <export-file>``
    Path to a json file containing the raw stub data.
  ``IMPORT <import-files>...``
    One or more paths to a json file containing exported stub data.
    This is used if a module depends on stub data from another module.
  ``PARSE_ONLY``
    Only parse the source files, do not generate any output. This is useful in
    combination with EXPORT to generate the stub data without writing any *.pyi files.
    The exported stub data can then be used as SOURCES for another target, or with
    the IMPORT option.
  ``SYSTEM_INCLUDE_DIRECTORIES``
    Include system directories for the target as ``-isystem`` compiler flags.
    If not specified, default system include directories will be added.
  ``WITH_SIGNATURES``
    Add C++ signatures as comments to *.pyi files.
  ``STUBGEN <stubgen-script>``
    Path to to custom stubgen script (advanced usage).

#]======================================================================]

set(LASS_STUBGEN "${CMAKE_CURRENT_LIST_DIR}/stubgen/lass_stubgen")

set(_Lass_stubgen_venv "${CMAKE_BINARY_DIR}/.venv-lass-stubgen")
set(_Lass_stubgen_requirement "${CMAKE_CURRENT_LIST_DIR}/stubgen/requirements.txt")
set(_Lass_stubgen_pythonpath "${CMAKE_CURRENT_LIST_DIR}/stubgen")


function(Lass_generate_stubs target)
	if (CMAKE_VERSION VERSION_LESS 3.21)
		# missing support for DEPFILE
		message(FATAL_ERROR "Lass_generate_stubs requires CMake 3.21 or higher.")
	endif()

	cmake_policy(PUSH)
	if (CMAKE_VERSION VERSION_GREATER_EQUAL 3.27)
		# Visual Studio Generators build custom commands in parallel.
		cmake_policy(SET CMP0147 NEW) 
	endif()

	set(_prefix)
	set(_options WITH_SIGNATURES PARSE_ONLY)
	set(_one_value_keywords OUTPUT_DIRECTORY PACKAGE EXPORT DEBUG_CONNECT STUBGEN)
	set(_multi_value_keywords SOURCES IMPORT SYSTEM_INCLUDE_DIRECTORIES)
	cmake_parse_arguments("${_prefix}" "${_options}" "${_one_value_keywords}" "${_multi_value_keywords}" ${ARGN})

	set(cxx_standard "--arg=-std=c++$<TARGET_PROPERTY:${target},CXX_STANDARD>")

	set(defines)
	set(includes)
	set(object_files)
	set(sources)

	if(NOT _SOURCES)
		get_target_property(sources "${target}" SOURCES)
		foreach(source ${sources})
			get_source_file_property(language "${source}" LANGUAGE)
			get_source_file_property(header_only "${source}" HEADER_FILE_ONLY)
			if (language STREQUAL "CXX" AND NOT header_only)
				list(APPEND _SOURCES "${source}")
			endif()
		endforeach()
	endif()

	if (MSVC_IDE)
		set(_stubs_dir "${CMAKE_CURRENT_BINARY_DIR}/${target}_stubs.dir/$<CONFIG>")
	else()
		set(_stubs_dir "${CMAKE_CURRENT_BINARY_DIR}/CMakeFiles/${target}_stubs.dir")
	endif()

	if(NOT _OUTPUT_DIRECTORY)
		set(_OUTPUT_DIRECTORY "$<TARGET_FILE_DIR:${target}>")
	endif()
	if(_PACKAGE)
		set(_package "--package=${_PACKAGE}")
	else()
		set(_package)
	endif()
	if(NOT _EXPORT)
		set(_EXPORT "${_stubs_dir}/${target}.stubdata.json")
	endif()
	set(_imports)
	foreach(import ${_IMPORT})
		set(_imports "--import=${import}")
	endforeach()
	if(NOT _SYSTEM_INCLUDE_DIRECTORIES)
		set(_SYSTEM_INCLUDE_DIRECTORIES ${CMAKE_CXX_IMPLICIT_INCLUDE_DIRECTORIES})
	endif()
	if(_WITH_SIGNATURES)
		set(_with_signatures "--with-signatures")
	else()
		set(_with_signatures)
	endif()
	if (NOT _STUBGEN)
		set(_STUBGEN "${LASS_STUBGEN}")
	endif()

	# Create virtual environment to run stubgen
	if (WIN32)
		set(_venv_python "${_Lass_stubgen_venv}/Scripts/python.exe")
	else()
		set(_venv_python "${_Lass_stubgen_venv}/bin/python")
	endif()
	_Lass_get_base_prefix("${Python_EXECUTABLE}" _base_prefix)
	_Lass_get_base_prefix("${_venv_python}" _venv_base_prefix)
	if (NOT (_base_prefix STREQUAL _venv_base_prefix))
		message(STATUS "Creating venv for Lass stubgen: ${_Lass_stubgen_venv}...")
		_Lass_checked_process("${Python_EXECUTABLE}" -m venv "--clear" "${_Lass_stubgen_venv}"
		)
	endif()
	set(_stamp_file "${_Lass_stubgen_venv}/.stamp")
	if ("${_Lass_stubgen_requirement}" IS_NEWER_THAN "${_stamp_file}")
		message(STATUS "Updating venv for Lass stubgen: ${_Lass_stubgen_venv}...")
		_Lass_checked_process("${_venv_python}" -m pip install --upgrade pip)
		_Lass_checked_process("${_venv_python}" -m pip install --requirement "${_Lass_stubgen_requirement}")
		file(TOUCH "${_stamp_file}")
	endif()

	set (_preamble "${CMAKE_COMMAND}" -E env "PYTHONPATH=${_Lass_stubgen_pythonpath}")
	if (CMAKE_VERSION VERSION_GREATER_EQUAL 3.24)
		list(APPEND _preamble "--")
	endif()

	if(_DEBUG_CONNECT)
		set(_debug_connect "--debug-connect=${_DEBUG_CONNECT}")
		_Lass_checked_process("${_venv_python}" -m pip install --upgrade debugpy)
	else()
		set(_debug_connect)
	endif()

	# Get target properties
	set(_defines "$<TARGET_PROPERTY:${target},COMPILE_DEFINITIONS>")
	set(defines "$<$<BOOL:${_defines}>:-D$<JOIN:${_defines},;-D>>")

	set(_define_symbol "$<TARGET_PROPERTY:${target},DEFINE_SYMBOL>")
	set(define_symbol "$<IF:$<BOOL:${define_symbol}>,${define_symbol},${target}_EXPORTS>") # default to target_EXPORTS
	list(APPEND defines "-D${define_symbol}")

	set(_includes "$<TARGET_PROPERTY:${target},INCLUDE_DIRECTORIES>")
	set(includes "$<$<BOOL:${_includes}>:-I$<JOIN:${_includes},;-I>>")

	if(_SYSTEM_INCLUDE_DIRECTORIES)
		set(system_includes "$<$<BOOL:${_SYSTEM_INCLUDE_DIRECTORIES}>:-isystem;$<JOIN:${_SYSTEM_INCLUDE_DIRECTORIES},;-isystem;>>")
	else()
		set(system_includes)
	endif()

	# Scan the dependencies of target, and see which ones also have stubs
	# add them as imports to this stubgen command
	set(upstream_dependencies "${target}")
	get_target_property(deps "${target}" LINK_LIBRARIES)
	foreach (dep ${deps})
		if (NOT TARGET "${dep}")
			continue()
		endif()
		get_target_property(dep_stubs "${dep}" LASS_STUBGEN_TARGET)
		if (NOT TARGET "${dep_stubs}")
			continue()
		endif()
		get_target_property(dep_export "${dep_stubs}" LASS_STUBGEN_EXPORT)
		list(APPEND upstream_dependencies "${dep_stubs}")
		list(APPEND _imports "--import=${dep_export}")
	endforeach()

	# Setup the precompiled header file, if any
	get_target_property(pch "${target}" PRECOMPILE_HEADERS)
	if (pch)
		if(MSVC_IDE)
  			set(src_file "${CMAKE_CURRENT_BINARY_DIR}/CMakeFiles/${target}.dir/$<CONFIG>/cmake_pch.hxx")
		else()
  			set(src_file "${CMAKE_CURRENT_BINARY_DIR}/CMakeFiles/${target}.dir/cmake_pch.hxx")
		endif()
		set(pch_path "${_stubs_dir}/cmake_pch.hxx.pch")
		set(pch_path_arg "--pch-path=${pch_path}")
		set(depfile "${pch_path}.d")

		set(args
			"${src_file}"
			${includes}
			${system_includes}
			${defines}
			"${cxx_standard}"
			"${_package}"
			"${pch_path_arg}"
			"--save-pch"
			"--depfile=${depfile}"
			"--parse-only"
			"--num-threads=1"
			"--quiet"
			"${_debug_connect}"
		)
		set(args_file "${pch_path}.txt")
		file(GENERATE OUTPUT "${args_file}" CONTENT "$<JOIN:${args},\n>\n")
		add_custom_command(
			OUTPUT "${pch_path}" "${depfile}"
			COMMAND ${_preamble}
					"${_venv_python}"
					"-B"
					"${_STUBGEN}"
					"@${args_file}"
			DEPENDS "${src_file}" "${args_file}" "${upstream_dependencies}"
			DEPFILE "${depfile}"
			COMMENT "Precompiling ${src_file}"
			WORKING_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}"
			COMMAND_EXPAND_LISTS
			VERBATIM
		)
		set(upstream_dependencies "${pch_path}")
		if (MSVC)
			# MSBuild can't handle very well parallel execution of custom commands
			# that all depend on the same PCH file. Create an intermediate target
			# as workaround.
			set(pch_target "${target}_stubs_pch")
			add_custom_target("${pch_target}" ALL DEPENDS
				"${pch_path}"
			)
			set_target_properties("${pch_target}" PROPERTIES
				FOLDER "LassStubgen"
			)
			list(APPEND upstream_dependencies "${pch_target}")
		endif()
	else()
		set(pch_path_arg)
	endif()
	
	set(intermediate_stubdata)
	foreach (src_file ${_SOURCES})
		# Get relative path to make output names unique
		file(REAL_PATH "${src_file}" abs_path)
		file(RELATIVE_PATH rel_path "${CMAKE_CURRENT_SOURCE_DIR}" "${abs_path}")
		# Make safe name for output file
		# See also https://gitlab.kitware.com/cmake/cmake/-/blob/747f4713dd1d14f831440820da0ebfdbecff91eb/Source/cmLocalGenerator.cxx#L4062-4081
		string(REGEX REPLACE "^/+" "" safe_name "${rel_path}")
		string(REPLACE ":" "_" safe_name "${safe_name}")
		string(REPLACE "../" "__/" safe_name "${safe_name}")
		# string(REPLACE " " "_" safe_name "${safe_name}")
		set(output_json "${_stubs_dir}/${safe_name}.stubdata.json")
		set(depfile "${output_json}.d")
		list(APPEND intermediate_stubdata "${output_json}")
		set(args
			"${src_file}"
			${includes}
			${system_includes}
			${defines}
			"${cxx_standard}"
			"${_package}"
			"--export=${output_json}"
			"--depfile=${depfile}"
			"${pch_path_arg}"
			"--parse-only"
			"--num-threads=1"
			"--quiet"
			"${_debug_connect}"
		)
		set(args_file "${output_json}.txt")
		file(GENERATE OUTPUT "${args_file}" CONTENT "$<JOIN:${args},\n>\n")

		# Add custom command for this source file
		add_custom_command(
			OUTPUT "${output_json}" "${depfile}"
			COMMAND ${_preamble}
					"${_venv_python}"
					"-B"
					"${_STUBGEN}"
					"@${args_file}"
			DEPENDS "${src_file}" "${args_file}" "${upstream_dependencies}"
			DEPFILE "${depfile}"
			COMMENT "Extracting Python stubs: ${src_file}"
			WORKING_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}"
			COMMAND_EXPAND_LISTS
			VERBATIM
		)
	endforeach()


	# Generate the final stubgen command, using all the parsed stubdata as source
	# We don't know what *.pyi files will be generated, so we can't use them as output.
	# Instead, we use the export file as output, and let stubgen handle the rest.
	# The export file will also be used as import for dependent targets.
	set(args
		${intermediate_stubdata}
		"${_package}"
		"${_imports}"
		"--export=${_EXPORT}"
		"--output-dir=${_OUTPUT_DIRECTORY}"
		"${_with_signatures}"
		"--num-threads=1"
		"${_debug_connect}"
	)
	if (_PARSE_ONLY)
		list(APPEND args "--parse-only")
	endif()

	set(args_file "${_stubs_dir}/${target}.stubdata.json.txt")
	file(GENERATE OUTPUT "${args_file}" CONTENT "$<JOIN:${args},\n>\n")
	set(stubs_target "${target}_stubs")
	add_custom_target("${stubs_target}" ALL
		COMMAND
			${_preamble}
			"${_venv_python}"
			"-B"
			"${_STUBGEN}"
			"@${args_file}"
		DEPENDS ${intermediate_stubdata} ${_IMPORT} 
		BYPRODUCTS "${_EXPORT}"
		COMMENT "Generating Python stubs for ${target}..."
		WORKING_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}"
		COMMAND_EXPAND_LISTS
		VERBATIM
	)

	# Set custom properties so we can find the stubgen target later
	set_target_properties("${stubs_target}" PROPERTIES
		LASS_STUBGEN_EXPORT "${_EXPORT}"
		FOLDER "LassStubgen"
	)
	set_target_properties("${target}" PROPERTIES
		LASS_STUBGEN_TARGET "${stubs_target}"
	)

	cmake_policy(POP)
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
