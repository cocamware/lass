function(_lass_install_library target)
	install(
		TARGETS "${target}"
		EXPORT LassExports
		RUNTIME DESTINATION bin
		LIBRARY DESTINATION lib
		ARCHIVE DESTINATION lib
		)
	if(MSVC)
		if (BUILD_SHARED_LIBS)
			install(
				FILES "$<TARGET_PDB_FILE:${target}>"
				DESTINATION bin
				OPTIONAL
				)
		else()
			install(
				FILES "$<PATH:REPLACE_EXTENSION,$<TARGET_FILE:${target}>,.pdb>"
				DESTINATION lib
				OPTIONAL
				)
		endif()
	endif()
endfunction()

# --- LASS ---

set(lass_subdirectories config io num prim spat stde util meta)
if (WIN32 AND LASS_SHARED_LIBRARY)
	set(lass_subdirectories ${lass_subdirectories} dll)
endif()

file(GLOB lass_SRCS *.cpp)
file(GLOB lass_HDRS *.h *.inl)
foreach(subdir ${lass_subdirectories})
	file(GLOB subdir_SRCS "${subdir}/*.cpp")
	file(GLOB subdir_impl_SRCS "${subdir}/impl/*.cpp")
	file(GLOB subdir_HDRS "${subdir}/*.h" "${subdir}/*.inl")
	file(GLOB subdir_impl_HDRS "${subdir}/impl/*.h" "${subdir}/impl/*.inl")
	list(APPEND lass_SRCS ${subdir_SRCS} ${subdir_impl_SRCS})
	list(APPEND lass_HDRS ${subdir_HDRS} ${subdir_impl_HDRS})
	source_group("Source Files\\${subdir}" FILES ${subdir_SRCS})
	source_group("Source Files\\${subdir}_impl" FILES ${subdir_impl_SRCS})
	source_group("Header Files\\${subdir}" FILES ${subdir_HDRS})
	source_group("Header Files\\${subdir}_impl" FILES ${subdir_impl_HDRS})
endforeach()

# These go in the source tree, as they are environment agnostic
# This is perhaps not the best way of doing it, but otherwise the include paths will become a huge mess ...
set(N 15)
lass_prebuild(lass_HDRS meta is_member.tmpl.h is_member.h "${N}")
lass_prebuild(lass_HDRS util object_factory.tmpl.h object_factory.h "${N}")
lass_prebuild(lass_HDRS util clone_factory.tmpl.h clone_factory.h "${N}")
lass_prebuild(lass_HDRS util callback.tmpl.h callback.h "${N}")
lass_prebuild(lass_HDRS util multi_callback.tmpl.h multi_callback.h "${N}")
lass_prebuild(lass_HDRS util callback_x.tmpl.h "callback_$x.h" "${N}")
lass_prebuild(lass_HDRS util multi_callback_x.tmpl.h "multi_callback_$x.h" "${N}")
lass_prebuild(lass_HDRS util callback_r_x.tmpl.h "callback_r_$x.h" "${N}")
lass_prebuild(lass_HDRS util thread_fun.tmpl.h thread_fun.h "${N}")
lass_prebuild(lass_HDRS util thread_fun.tmpl.inl thread_fun.inl "${N}")
lass_prebuild(lass_HDRS util bind.tmpl.h bind.h "${N}")
lass_prebuild(lass_HDRS util/impl dispatcher_x.tmpl.h "dispatcher_$x.h" "${N}")
lass_prebuild(lass_HDRS util/impl dispatcher_r_x.tmpl.h "dispatcher_r_$x.h" "${N}")

list(REMOVE_DUPLICATES lass_HDRS)

if(CMAKE_CL_64)
    # CMake doesn't really support adding assembly files to visual studio projects (yet)
    # Work around it by adding the obj file as 'source file' and have it generated
    set(sourcedir ${CMAKE_CURRENT_SOURCE_DIR}/util/impl)
    set(fname atomic_msvc_x64)
    set(sourcepath ${sourcedir}/${fname}.asm)
    set(defpath ${sourcedir}/${fname}.def)
    set(masm ml64.exe)
    if(MSVC_IDE)
        set(objpath ${CMAKE_CURRENT_BINARY_DIR}/lass.dir/${CMAKE_CFG_INTDIR}/${fname}.obj)
    else()
        set(objpath ${CMAKE_CURRENT_BINARY_DIR}/CMakeFiles/lass.dir/${fname}.obj)
    endif()
    add_custom_command(
        OUTPUT ${objpath}
        DEPENDS ${sourcepath}
        COMMAND ${masm}
        ARGS /c "/Fo${objpath}" "${sourcepath}")
    list(APPEND lass_SRCS ${objpath} ${defpath})
    
    # It should have been as easy as
    #enable_language(ASM_MASM)
    #set_source_files_properties(
    #    ${sourcepath}
    #    LANGUAGE ASM_MASM)
    #list(APPEND lass_SRCS ${sourcepath} ${defpath})
endif()

add_library(lass
	${lass_SRCS}
	${lass_HDRS}
	)

add_library(Lass::lass ALIAS lass)

target_include_directories(lass
	PUBLIC $<BUILD_INTERFACE:${Lass_SOURCE_DIR}>
	PUBLIC $<BUILD_INTERFACE:${Lass_BINARY_DIR}/local>
	PUBLIC $<INSTALL_INTERFACE:include>
	PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}" # for pre-compiled header
	)

target_link_libraries(lass
	PUBLIC ${lass_LIBS}
	)

set_target_properties(lass PROPERTIES
	DEFINE_SYMBOL LASS_EXPORTS 
	SOVERSION "${Lass_VERSION_PATCH}"
	OUTPUT_NAME "lass-${Lass_VERSION_MAJOR}-${Lass_VERSION_MINOR}"
	)

# public compiler options that client should use too
if(MSVC)
	target_compile_options(lass
		PUBLIC "/EHsc" "/GR"
	)
	if(BUILD_WITHOUT_ITERATOR_DEBUGGING)
		target_compile_options(lass
			PUBLIC "/D_HAS_ITERATOR_DEBUGGING=0"
		)
	endif()
elseif(SUNPRO)
	# pass
else()
	target_compile_options(lass
		PUBLIC "-fno-strict-aliasing"
	)
endif()

target_compile_features(lass
	PUBLIC
		cxx_std_${CMAKE_CXX_STANDARD}
)

lass_add_precompiled_header(lass 
	lass_common.h 
	"${Lass_SOURCE_DIR}/lass/lass_common.cpp"
	)
	
_lass_install_library(lass)


# --- LASS_PYTHON ---

if(Python_Development_FOUND)
	file(GLOB lass_python_SRCS python/*.cpp)
	file(GLOB lass_python_HDRS python/*.h python/*.inl)

	set(N 15)
	lass_prebuild(lass_python_HDRS python callback_python.tmpl.h callback_python.h "${N}")
	lass_prebuild(lass_python_HDRS python pyobject_macros.tmpl.h pyobject_macros.h "${N}")
	lass_prebuild(lass_python_HDRS python pyobject_call.tmpl.inl pyobject_call.inl "${N}")
	lass_prebuild(lass_python_HDRS python pycallback_export_traits.tmpl.inl pycallback_export_traits.inl "${N}")
	lass_prebuild(lass_python_HDRS python bulk_add_integer.tmpl.inl bulk_add_integer.inl "${N}")

	add_library(lass_python
		${lass_python_SRCS}
		${lass_python_HDRS}
	)
	add_library(Lass::lass_python ALIAS lass_python)
	target_include_directories(lass_python
		PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}" # for pre-compiled header
 		)
	target_link_libraries(lass_python
		PUBLIC Lass::lass Python::Module # also adds the include dirs
		)
	set_target_properties(lass_python PROPERTIES
		DEFINE_SYMBOL LASS_PYTHON_EXPORTS 
		SOVERSION "${Lass_VERSION_PATCH}"
		OUTPUT_NAME "lass-python-${Lass_VERSION_MAJOR}-${Lass_VERSION_MINOR}"
		)
	lass_add_precompiled_header(lass_python 
		python/python_common.h
		"${Lass_SOURCE_DIR}/lass/python/python_common.cpp"
		)
	_lass_install_library(lass_python)
endif()
