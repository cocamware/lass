set(_Lass_get_libclang_version_py "${CMAKE_CURRENT_LIST_DIR}/_get_libclang_version.py")

function(Lass_find_libclang libclang_library libclang_version)
	if (Lass_LIBCLANG_LIBRARY AND _Lass_LIBCLANG_VERSION)
		set(${libclang_library} "${Lass_LIBCLANG_LIBRARY}" PARENT_SCOPE)
		set(${libclang_version} "${_Lass_LIBCLANG_VERSION}" PARENT_SCOPE)
		return()
	endif()

	if (NOT Python_EXECUTABLE)
		message(FATAL_ERROR "Python_EXECUTABLE not found. Please find_package(Python) first.")
	endif()

	if (NOT Lass_LIBCLANG_LIBRARY)
		if (MSVC)
			# Figure out the VC/Tools directory of the visual studio installation 
			get_filename_component(_vc_tools_dir "${CMAKE_CXX_COMPILER}" DIRECTORY)
			while(NOT _vc_tools_dir MATCHES ".+/VC/Tools$")
				get_filename_component(_parent "${_vc_tools_dir}" DIRECTORY)
				if (_parent STREQUAL _vc_tools_dir)
					set(_vc_tools_dir)
					break()
				endif()
				set(_vc_tools_dir "${_parent}")
			endwhile()
			if (_vc_tools_dir)
				execute_process(COMMAND "${Python_EXECUTABLE}" -c 
						"import struct, sys; sys.stdout.write('x64/bin' if struct.calcsize('P') == 8 else 'bin')"
					OUTPUT_VARIABLE _bin_dir
					RESULT_VARIABLE _exitcode
					OUTPUT_STRIP_TRAILING_WHITESPACE
				)
				if (_exitcode)
					set(_llvm_bin_dir)
				else()
					set(_llvm_bin_dir "${_vc_tools_dir}/Llvm/${_bin_dir}")
				endif()
			endif()
			if (_llvm_bin_dir)
				find_file(Lass_LIBCLANG_LIBRARY
					NAMES libclang.dll
					PATHS ${_llvm_bin_dir}
					NO_DEFAULT_PATH
				)
			endif()

		else()
			if(CMAKE_CXX_COMPILER_ID STREQUAL "Clang")
				set(_clang "${CMAKE_CXX_COMPILER}")
			else()
				set(_clang "clang")
			endif()

			# find the resource directory of the Clang compiler
			execute_process(
				COMMAND "${_clang}" --print-resource-dir
				OUTPUT_VARIABLE _resource_dir
				RESULT_VARIABLE _exitcode
				OUTPUT_STRIP_TRAILING_WHITESPACE
			)
			if (_exitcode)
				set(_resource_dir)
			endif()

			if (_resource_dir)
				get_filename_component(_version "${_resource_dir}" NAME)
				get_filename_component(_lib_dir "${_resource_dir}/../.." ABSOLUTE)
				find_library(Lass_LIBCLANG_LIBRARY
					NAMES
						libclang.so
						libclang.so.1
						"libclang-${_version}.so"
						"libclang-${_version}.so.1"
					PATHS
						"${_lib_dir}"
					NO_DEFAULT_PATH
				)
			endif()
		endif()
	endif()

	if (Lass_LIBCLANG_LIBRARY)
		execute_process(
			COMMAND "${Python_EXECUTABLE}" "${_Lass_get_libclang_version_py}" "${Lass_LIBCLANG_LIBRARY}"
			OUTPUT_VARIABLE _version
			RESULT_VARIABLE _exitcode
			OUTPUT_STRIP_TRAILING_WHITESPACE
		)
		if (_exitcode)
			message(WARNING "Failed to determine version of ${Lass_LIBCLANG_LIBRARY}.")
			set(_version "_Lass_LIBCLANG_VERSION-NOTFOUND")
		endif()
		set(_Lass_LIBCLANG_VERSION "${_version}" CACHE INTERNAL "Version of libclang" FORCE)
	endif()

	if (Lass_LIBCLANG_LIBRARY AND _Lass_LIBCLANG_VERSION)
		message(STATUS "Looking for libclang - found: ${_Lass_LIBCLANG_VERSION}")
		set(${libclang_library} "${Lass_LIBCLANG_LIBRARY}" PARENT_SCOPE)
		set(${libclang_version} "${_Lass_LIBCLANG_VERSION}" PARENT_SCOPE)
	else()
		message(STATUS "Looking for libclang - not found")
		set(${libclang_library} "" PARENT_SCOPE)
		set(${libclang_version} "" PARENT_SCOPE)
	endif()
endfunction()
