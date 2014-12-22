function(LASS_EVAL)
    foreach(arg ${ARGN})
        message(STATUS "${arg}: ${${arg}}")
    endforeach()
endfunction()


function(LASS_PARAM_EXPANDER inpath outpath)
    # LASS_PARAM_EXPANDER(<inpath> <outpath> <N>)
    # LASS_PARAM_EXPANDER(<inpath> <outpath> <value1> [<value2> ...])
    set(generator "${lass_SOURCE_DIR}/tools/param_expander.py")
    add_custom_command(
        OUTPUT "${outpath}"
        DEPENDS "${inpath}" "${generator}"
        COMMAND "${PYTHON_EXECUTABLE}"
        ARGS "${generator}" "${inpath}" "${outpath}" ${ARGN})
endfunction()


function(LASS_PREBUILD hdrs dirname infile outfile)
    # LASS_PREBUILD(<hdrs> <dirname> <infile> <outfile> <N>)
    # LASS_PREBUILD(<hdrs> <dirname> <infile> <outfile> <value1> [<value2> ...])
    set(inpath "${CMAKE_CURRENT_SOURCE_DIR}/${dirname}/${infile}")
    set(outpath "${CMAKE_CURRENT_SOURCE_DIR}/${dirname}/${outfile}")
    set(tmphdrs ${${hdrs}})
    if(${outpath} MATCHES "\\$x")
        foreach(i RANGE 1 ${ARGN})
            string(REPLACE "$x" "${i}" outpath_i "${outpath}")
            LASS_PARAM_EXPANDER("${inpath}" "${outpath_i}" "${i}")
            list(APPEND tmphdrs "${outpath_i}")
        endforeach()
    else()
        LASS_PARAM_EXPANDER("${inpath}" "${outpath}" ${ARGN})
        list(APPEND tmphdrs "${outpath}")
    endif()
    set(${hdrs} ${tmphdrs} PARENT_SCOPE)
endfunction()


function(LASS_ADD_PRECOMPILED_HEADER target hdrfile srcpath)
    if(NOT BUILD_USING_PRECOMPILED_HEADERS)
        return()
    endif()
    
    if(MSVC_IDE)
        set(pchpath "${CMAKE_CURRENT_BINARY_DIR}/${target}.dir/${CMAKE_CFG_INTDIR}/${hdrfile}.pch")
    else()
        set(pchpath "${CMAKE_CURRENT_BINARY_DIR}/CMakeFiles/${target}.dir/${hdrfile}.pch")
    endif()
    if(MSVC)
        set_property(
            TARGET "${target}"
            APPEND_STRING
            PROPERTY COMPILE_FLAGS " /Fp\"${pchpath}\" /Yu\"${hdrfile}\""
        )
        set_property(
            SOURCE "${srcpath}"
            APPEND_STRING
            PROPERTY COMPILE_FLAGS " /Yc\"${hdrfile}\""
        )
        if (NOT MSVC_IDE)
            get_target_property(sources "${target}" SOURCES)
            list(REMOVE_ITEM sources "${srcpath}")
            set_property(
                SOURCE ${sources}
                APPEND
                PROPERTY OBJECT_DEPENDS "${pchpath}"
            )
        endif()            
    else()
        #add_custom_command(
        #    OUTPUT ${pchpath}
        #    DEPENDS ${hdrfile}
        #    COMMAND ...)
        #set_source_files_properties(
        #    ${test_SRCS}
        #    PROPERTIES
        #    OBJECT_DEPENDS ${_pchpath})
    endif()
endfunction()


function(LASS_ADD_FLAGS_ONCE var export)
    # LASS_SET_FLAGS_ONCE(<var> [EXPORT|NOEXPORT] <flag> [<flag> ...])
    # var must be a cached var
    
    _lass_enforce_cached_var("${var}")
    set(flags_already_set "lass_${var}_set_flags")
            
    # argument processing
    set(flags ${ARGN})
    if(export STREQUAL "EXPORT")
        set(is_export TRUE)
    elseif(export STREQUAL "NOEXPORT")
        set(is_export FALSE)
    else() 
        # neither EXPORT or NOEXPORT, so it's a real flag. default to NOEXPORT and add it to flags list        
        set(is_export FALSE)
        list(INSERT flags 0 "${export}")
    endif()
    
    foreach(flag ${flags})
        _lass_get_flag_key("${flag}" key)
        
        _lass_mark_key_as_processed("${var}" "${key}" was_processed_before)
        if(NOT was_processed_before)
            # flag has not been seen before, process it
            
            _lass_find_flag_by_key("${var}" "${key}" is_set)
            if(NOT is_set)
                LASS_SET_CACHE_VALUE("${var}" " ${flag}" APPEND_STRING)
            endif()
        endif()
        
        if (is_export)
            _lass_mark_key_for_export("${var}" "${key}")
        endif()
    endforeach()
endfunction()


function (LASS_LINK_OPTION_TO_FLAG optionvar flagsvar flag)
    # LASS_LINK_OPTION_TO_FLAG(<optionvar> <flagsvar> <flag> [EXPORT])
    
    if (NOT DEFINED "${flagsvar}")
        message(FATAL_ERROR "Cannot use _link_flag_to_option to set a flag in non-existing variable ${flagsvar}")
    endif ()
    if (NOT DEFINED "${optionvar}")
        message(FATAL_ERROR "Cannot use _link_flag_to_option to link a flag with non-existing option ${optionvar}")
    endif ()
      
    _lass_get_flag_key("${flag}" key)

    if(ARGV3 STREQUAL "EXPORT")
        _lass_mark_key_for_export("${flagsvar}" "${key}")
    endif()
    
    get_property(previous CACHE "${optionvar}_previous" PROPERTY VALUE)
    string(COMPARE NOTEQUAL "${${optionvar}}" "${previous}" option_has_changed)      
    if (option_has_changed)
    
        # update flagsvar to match optionvar    
        set(value "${${flagsvar}}")    
        string(FIND "${value}" "${flag}" index)
    
        if (${optionvar} AND index EQUAL -1)
            message(STATUS "setting ${flag} in ${flagsvar}")
            set(value "${value} ${flag}")
        elseif (NOT ${optionvar} AND index GREATER -1)
            message(STATUS "unsetting ${flag} in ${flagsvar}")
            string(REPLACE " ${flag}" "" value "${value}") # try to eat a single space too.
            string(REPLACE "${flag}" "" value "${value}")
        else()
            message(STATUS "${flag} is already (un)set in ${flagsvar}")
            return()
        endif()
        
        LASS_SET_CACHE_VALUE("${flagsvar}" "${value}")
    else()
        # update optionvar to match flagsvar
        string(FIND "${${flagsvar}}" "${flag}" index)
        if(index GREATER -1)
            LASS_SET_CACHE_VALUE("${optionvar}" ON)
        else()
            LASS_SET_CACHE_VALUE("${optionvar}" OFF)
        endif()
    endif()
    
    set("${optionvar}_previous" "${${optionvar}}" CACHE INTERNAL "" FORCE)
    
endfunction()


function(LASS_GATHER_EXPORTED_FLAGS var out)
    # gathers the flags in ${var} that have been set by one of the functions
    # above, and which have been marked for export
    set(tmp "")
    foreach(key ${lass_${var}_exports})
        _lass_find_flag_by_key("${var}" "${key}" flag)
        if(flag)
            set(tmp "${tmp} ${flag}")
        endif()
    endforeach()
    string(STRIP "${tmp}" tmp)
    set(${out} "${tmp}" PARENT_SCOPE)
endfunction()


function(LASS_SET_CACHE_VALUE var value)
    # LASS_SET_CACHE_VALUE(<var> <value> [APPEND|APPEND_STRING])
    # sets the value of an _existing_ variable <var> to <value>
    #
    # Rationale: 
    #   when using the set command to set existing cached variable, this does not succeed without the FORCE option
    #   using the FORCE option also requires you to give all to other options like <type> and <docstring>.
    #   And that is inconvenient.
    #
    #   This function will set the variable for you without needing to give all the extra option,     
    _lass_enforce_cached_var("${var}")
    
    if(ARGV2 STREQUAL "APPEND" OR ARGV2 STREQUAL "APPEND_STRING")
        set_property(CACHE "${var}" "${ARGV2}" PROPERTY VALUE "${value}")
    else()
        set_property(CACHE "${var}" PROPERTY VALUE "${value}")
    endif()    
endfunction()


function(_lass_get_flag_key flag out)
    # return the prefix of a flag, without the value. By this key, it'll be determined
    # if a flag is already set or not (possibly of course with a different value).
    # Mostly this is what's in front of =
    # .e.g for /D_HAS_ITERATOR_DEBUGGING=0 or -Wformat=2, this is /D_HAS_ITERATOR_DEBUGGING or -Wformat
    string(FIND "${flag}" "=" index)
    if(index GREATER -1)
        string(SUBSTRING "${flag}" 0 "${index}" key)
        set("${out}" "${key}" PARENT_SCOPE)
    else()
        set("${out}" "${flag}" PARENT_SCOPE)
    endif()
endfunction()


function(_lass_find_flag_by_key var key out)
    if (WIN32)
        separate_arguments(args WINDOWS_COMMAND "${${var}}")
    else()
        separate_arguments(args UNIX_COMMAND "${${var}}")
    endif()
    foreach(arg ${args})
        string(FIND "${arg}" "${key}" index)
        if(index EQUAL 0)
            set("${out}" "${arg}" PARENT_SCOPE)
            return()
        endif()
    endforeach()
    set("${out}" "NOTFOUND" PARENT_SCOPE)
endfunction()


function(_lass_mark_key_for_export var key)
    set(exports "lass_${var}_exports")
    
    list(FIND "${exports}" "${key}" index)
    if (index GREATER -1)
        return() # already marked
    endif()
    
    set("${exports}" ${${exports}} "${key}" CACHE INTERNAL "")
endfunction()


function(_lass_mark_key_as_processed var key was_processed_before)
    set(_processed_flags "_lass_processed_flags_${var}")
    # check if it was processed before.
    list(FIND ${_processed_flags} "${key}" index)
    if(index EQUAL -1)
        set(was_processed_before FALSE PARENT_SCOPE)
        set("${_processed_flags}" ${${_processed_flags}} "${key}" CACHE INTERNAL "") # it is now ...
    else()
        set(was_processed_before TRUE PARENT_SCOPE)
    endif()      
endfunction()


function(_lass_enforce_cached_var var)
    get_property(is_cached CACHE "${var}" PROPERTY VALUE SET)
    if (NOT is_cached)
        message(FATAL_ERROR "${var} is not a cached variable")
    endif()
endfunction()