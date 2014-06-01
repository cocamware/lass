# --- LASS ---

set(lass_subdirectories config io num prim spat stde util meta)
if (WIN32 AND LASS_SHARED_LIBRARY)
	set(lass_subdirectories ${lass_subdirectories} dll)
endif()

file(GLOB lass_SRCS *.cpp)
file(GLOB lass_HDRS *.h *.inl)
foreach(subdir ${lass_subdirectories})
	if(NOT subdir STREQUAL "config")
		file(GLOB temp_SRCS ${subdir}/*.cpp)
		file(GLOB temp_impl_SRCS ${subdir}/impl/*.cpp)
	endif()
	file(GLOB temp_HDRS ${subdir}/*.h ${subdir}/*.inl)
	file(GLOB temp_impl_HDRS ${subdir}/impl/*.h ${subdir}/impl/*.inl)
	list(APPEND lass_SRCS ${temp_SRCS} ${temp_impl_SRCS})
	list(APPEND lass_HDRS ${temp_HDRS} ${temp_impl_HDRS})
	source_group("Source Files\\${subdir}" FILES ${temp_SRCS})
	source_group("Source Files\\${subdir}_impl" FILES ${temp_impl_SRCS})
	source_group("Header Files\\${subdir}" FILES ${temp_HDRS})
	source_group("Header Files\\${subdir}_impl" FILES ${temp_impl_HDRS})
endforeach()

# These go in the source tree, as they are environment agnostic
# This is perhaps not the best way of doing it, but otherwise the include paths will become a huge mess ...
set(N 15)
prebuild(meta is_member.tmpl.h is_member.h ${N})
prebuild(util object_factory.tmpl.h object_factory.h ${N})
prebuild(util clone_factory.tmpl.h clone_factory.h ${N})
prebuild(util callback.tmpl.h callback.h ${N})
prebuild(util multi_callback.tmpl.h multi_callback.h ${N})
prebuild(util callback_x.tmpl.h "callback_$x.h" ${N})
prebuild(util multi_callback_x.tmpl.h "multi_callback_$x.h" ${N})
prebuild(util callback_r_x.tmpl.h "callback_r_$x.h" ${N})
prebuild(util thread_fun.tmpl.h thread_fun.h ${N})
prebuild(util thread_fun.tmpl.inl thread_fun.inl ${N})
prebuild(util bind.tmpl.h bind.h ${N})
prebuild(util/impl dispatcher_x.tmpl.h "dispatcher_$x.h" ${N})
prebuild(util/impl dispatcher_r_x.tmpl.h "dispatcher_r_$x.h" ${N})

list(APPEND lass_HDRS ${lass_PRE_BUILT_HDRS})
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
	COMPILE_FLAGS "${lass_C_FLAGS}"
	SOVERSION "${LASS_PATCHLEVEL}"
	OUTPUT_NAME "${LASS_OUTPUT_NAME}"
	)
add_precompiled_header(lass 
	lass_common.h 
	"${lass_SOURCE_DIR}/lass/lass_common.cpp"
	${lass_SRCS}
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
	set(lass_python_subdirectories python)

	file(GLOB lass_python_SRCS)
	file(GLOB lass_python_HDRS)
	foreach(subdir ${lass_python_subdirectories})
		if(NOT subdir STREQUAL "config")
			file(GLOB temp_SRCS ${subdir}/*.cpp)
			file(GLOB temp_impl_SRCS ${subdir}/impl/*.cpp)
		endif()
		file(GLOB temp_HDRS ${subdir}/*.h ${subdir}/*.inl)
		file(GLOB temp_impl_HDRS ${subdir}/impl/*.h ${subdir}/impl/*.inl)
		list(APPEND lass_python_SRCS ${temp_SRCS} ${temp_impl_SRCS})
		list(APPEND lass_python_HDRS ${temp_HDRS} ${temp_impl_HDRS})
		source_group("Source Files\\${subdir}" FILES ${temp_SRCS})
		source_group("Source Files\\${subdir}_impl" FILES ${temp_impl_SRCS})
		source_group("Header Files\\${subdir}" FILES ${temp_HDRS})
		source_group("Header Files\\${subdir}_impl" FILES ${temp_impl_HDRS})
	endforeach()

	prebuild(python callback_python.tmpl.h callback_python.h ${N})
	prebuild(python pyobject_macros.tmpl.h pyobject_macros.h ${N})
	prebuild(python pyobject_call.tmpl.inl pyobject_call.inl ${N})
	prebuild(python pycallback_export_traits.tmpl.inl pycallback_export_traits.inl ${N})
	prebuild(python py_tuple.tmpl.h py_tuple.h 20)
	prebuild(python bulk_add_integer.tmpl.inl "bulk_add_integer.inl" ${N})

	add_library(lass_python
		${lass_python_SRCS} 
		${lass_python_HDRS}
		)
	target_link_libraries(lass_python
		lass
		optimized ${PYTHON_LIBRARIES} 
		debug ${PYTHON_DEBUG_LIBRARIES}
		)
	set_target_properties(lass_python PROPERTIES
		DEFINE_SYMBOL LASS_PYTHON_EXPORTS 
		COMPILE_FLAGS "${lass_C_FLAGS}"
		SOVERSION "${LASS_PATCHLEVEL}"
		OUTPUT_NAME "${LASS_PYTHON_OUTPUT_NAME}"
		)
	add_precompiled_header(lass_python 
		python_common.h 
		${lass_SOURCE_DIR}/lass/python/python_common.cpp
		${lass_python_SRCS}
		)
	install(
		TARGETS lass_python
		EXPORT LassExports
		RUNTIME DESTINATION bin
		LIBRARY DESTINATION lib
		ARCHIVE DESTINATION lib
		)
endif()
