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
target_link_libraries(lass
	${lass_LIBS}
	)
set_target_properties(lass PROPERTIES
	DEFINE_SYMBOL LASS_EXPORTS 
	SOVERSION "${LASS_PATCHLEVEL}"
	OUTPUT_NAME "${LASS_OUTPUT_NAME}"
	)
lass_add_precompiled_header(lass 
	lass_common.h 
	"${lass_SOURCE_DIR}/lass/lass_common.cpp"
	)
	
install(
	TARGETS lass
	EXPORT LassExports
	RUNTIME DESTINATION bin
	LIBRARY DESTINATION lib
	ARCHIVE DESTINATION lib
	)

# --- LASS_PYTHON ---

if(PYTHONLIBS_FOUND)
	file(GLOB lass_python_SRCS python/*.cpp)
	file(GLOB lass_python_HDRS python/*.h python/*.inl)

	set(N 15)
	lass_prebuild(lass_python_HDRS python callback_python.tmpl.h callback_python.h "${N}")
	lass_prebuild(lass_python_HDRS python pyobject_macros.tmpl.h pyobject_macros.h "${N}")
	lass_prebuild(lass_python_HDRS python pyobject_call.tmpl.inl pyobject_call.inl "${N}")
	lass_prebuild(lass_python_HDRS python pycallback_export_traits.tmpl.inl pycallback_export_traits.inl "${N}")
	lass_prebuild(lass_python_HDRS python py_tuple.tmpl.h py_tuple.h 20)
	lass_prebuild(lass_python_HDRS python bulk_add_integer.tmpl.inl bulk_add_integer.inl "${N}")

	add_library(lass_python
		${lass_python_SRCS} 
		${lass_python_HDRS}
		)
	target_link_libraries(lass_python
		lass
		${PYTHON_LIBRARIES} 
		)
	set_target_properties(lass_python PROPERTIES
		DEFINE_SYMBOL LASS_PYTHON_EXPORTS 
		SOVERSION "${LASS_PATCHLEVEL}"
		OUTPUT_NAME "${LASS_PYTHON_OUTPUT_NAME}"
		)
	lass_add_precompiled_header(lass_python 
		python_common.h 
		"${lass_SOURCE_DIR}/lass/python/python_common.cpp"
		)
	install(
		TARGETS lass_python
		EXPORT LassExports
		RUNTIME DESTINATION bin
		LIBRARY DESTINATION lib
		ARCHIVE DESTINATION lib
		)
endif()
