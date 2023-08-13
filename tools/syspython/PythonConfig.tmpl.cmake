set(Python_VERSION_MAJOR @{major})
set(Python_VERSION_MINOR @{minor})
set(Python_VERSION_PATCH @{minor})
set(Python_VERSION @{version})

set(Python_EXECUTABLE @{executable})
set(Python_INCLUDE_DIRS @{include_dirs})
set(Python_LIBRARY @{library})
set(Python_LIBRARIES "${Python_LIBRARY}")
set(Python_LIBRARY_DIRS @{library_dirs})
set(_Python_RUNTIME_LIBRARY @{runtime_library})
set(Python_RUNTIME_LIBRARY_DIRS @{runtime_library_dirs})
set(_Python_LINK_LIBRARIES @{link_libraries})
set(Python_STDLIB @{stdlib})
set(Python_STDARCH @{stdarch})
set(Python_SITELIB @{sitelib})
set(Python_SITEARCH @{sitearch})
set(Python_SOABI @{soabi})
set(_Python_POSTFIX @{postfix})
set(_Python_LIBRARY_TYPE @{library_type})

if(NOT Python_FIND_COMPONENTS)
    set(Python_FIND_COMPONENTS Interpreter)
    set(Python_FIND_REQUIRED_Interpreter TRUE)
endif()
if("Development" IN_LIST Python_FIND_COMPONENTS)
    list(APPEND Python_FIND_COMPONENTS "Development.Module" "Development.Embed")
endif()
list(REMOVE_DUPLICATES Python_FIND_COMPONENTS)
if(Python_FIND_REQUIRED_Development)
    set(Python_FIND_REQUIRED_Development.Module TRUE)
    set(Python_FIND_REQUIRED_Development.Embed TRUE)
endif()

set(Python_Interpreter_FOUND TRUE)
set(Python_Development_FOUND TRUE)
set(Python_Development.Module_FOUND TRUE)
set(Python_Development.Embed_FOUND TRUE)

if("Development.SABIModule" IN_LIST Python_FIND_COMPONENTS)
    set(_Python_SABI_LIBRARY @{sabi_library})
    set(Python_SABI_LIBRARIES "${_Python_SABI_LIBRARY}")
    set(Python_SABI_LIBRARY_DIRS @{sabi_library_dirs})
    set(_Python_RUNTIME_SABI_LIBRARY @{sabi_runtime_library})
    set(Python_RUNTIME_SABI_LIBRARY_DIRS @{sabi_runtime_library_dirs})
    set(Python_Development.SABIModule_FOUND @{sabi_found})
    set(Python_SOSABI @{sosabi})
endif()

foreach(_Python_COMPONENT ${Python_FIND_COMPONENTS})
    if(Python_FIND_REQUIRED_${_Python_COMPONENT} AND NOT Python_${_Python_COMPONENT}_FOUND)
        if(NOT Python_FIND_QUIETLY)
            message(STATUS "Could NOT find Python (missing: ${_Python_COMPONENT}) (found suitable version \"${Python_VERSION}\", via ${CMAKE_CURRENT_LIST_FILE})")
        endif()
        set(Python_FOUND FALSE)
        return()
    endif()
endforeach()



if(NOT TARGET Python::Interpreter)
    add_executable(Python::Interpreter IMPORTED)
    set_target_properties(Python::Interpreter
        PROPERTIES
            IMPORTED_LOCATION "${Python_EXECUTABLE}"
    )
endif()



if(NOT TARGET Python::Python)
    add_library(Python::Python "${_Python_LIBRARY_TYPE}" IMPORTED)
    set_target_properties(Python::Python
        PROPERTIES
            IMPORTED_LINK_INTERFACE_LANGUAGES "C"
            INTERFACE_INCLUDE_DIRECTORIES "${Python_INCLUDE_DIRS}"
    )
    if(_Python_RUNTIME_LIBRARY)
        set_target_properties(Python::Python
            PROPERTIES
                IMPORTED_IMPLIB "${Python_LIBRARY}"
                IMPORTED_LOCATION "${_Python_RUNTIME_LIBRARY}"
        )
    else()
        set_target_properties(Python::Python
            PROPERTIES
                IMPORTED_LOCATION "${Python_LIBRARY}"
        )
    endif()
    if (_Python_LINK_LIBRARIES)
        set_target_properties(Python::Python
            PROPERTIES
                INTERFACE_LINK_LIBRARIES "${_Python_LINK_LIBRARIES}"
        )
    endif()
endif()



if(NOT TARGET Python::Module)
    if(WIN32)
        add_library(Python::Module "${_Python_LIBRARY_TYPE}" IMPORTED)
        set_target_properties(Python::Module
            PROPERTIES
                IMPORTED_LINK_INTERFACE_LANGUAGES "C"
                INTERFACE_INCLUDE_DIRECTORIES "${Python_INCLUDE_DIRS}"
        )
        if(_Python_RUNTIME_LIBRARY)
            set_target_properties(Python::Module
                PROPERTIES
                    IMPORTED_IMPLIB "${Python_LIBRARY}"
                    IMPORTED_LOCATION "${_Python_RUNTIME_LIBRARY}"
            )
        else()
            set_target_properties(Python::Module
                PROPERTIES
                    IMPORTED_LOCATION "${Python_LIBRARY}"
            )
        endif()
        if (_Python_LINK_LIBRARIES)
            set_target_properties(Python::Module
                PROPERTIES
                    INTERFACE_LINK_LIBRARIES "${_Python_LINK_LIBRARIES}"
            )
        endif()
    else()
        add_library(Python::Module INTERFACE IMPORTED)
        set_target_properties(Python::Module
            PROPERTIES
                INTERFACE_INCLUDE_DIRECTORIES "${Python_INCLUDE_DIRS}"
        )
    endif()
endif()



if(Python_Development.SABIModule_FOUND AND NOT TARGET Python::SABIModule)
    if(WIN32)
        add_library(Python::SABIModule "${_Python_LIBRARY_TYPE}" IMPORTED)
        set_target_properties(Python::SABIModule
            PROPERTIES
                IMPORTED_LINK_INTERFACE_LANGUAGES "C"
                INTERFACE_INCLUDE_DIRECTORIES "${Python_INCLUDE_DIRS}"
        )
        if(_Python_RUNTIME_SABI_LIBRARY)
            set_target_properties(Python::SABIModule
                PROPERTIES
                    IMPORTED_IMPLIB "${_Python_SABI_LIBRARY}"
                    IMPORTED_LOCATION "${_Python_RUNTIME_SABI_LIBRARY}"
            )
        else()
            set_target_properties(Python::SABIModule
                PROPERTIES
                    IMPORTED_LOCATION "${_Python_SABI_LIBRARY}"
            )
        endif()
        if (_Python_LINK_LIBRARIES)
            set_target_properties(Python::SABIModule
                PROPERTIES
                    INTERFACE_LINK_LIBRARIES "${_Python_LINK_LIBRARIES}"
            )
        endif()
    else()
        add_library(Python::SABIModule INTERFACE IMPORTED)
        set_target_properties(Python::SABIModule
            PROPERTIES
                INTERFACE_INCLUDE_DIRECTORIES "${Python_INCLUDE_DIRS}"
        )
    endif()
endif()



function(Python_add_library _name)
    set(_prefix)
    set(_options STATIC SHARED MODULE WITH_SOABI)
    set(_one_value_keywords USE_SABI)
    set(_multi_value_keywords)
    cmake_parse_arguments("${_prefix}" "${_options}" "${_one_value_keywords}" "${_multi_value_keywords}" ${ARGN})

    if(_STATIC)
        if(_SHARED)
            message(SEND_ERROR "Both STATIC and SHARED are set.")
            return()
        endif()
        if(_MODULE)
            message(SEND_ERROR "Both STATIC and MODULE are set.")
            return()
        endif()
        set(_type STATIC)
    elseif(_SHARED)
        if(_MODULE)
            message(SEND_ERROR "Both SHARED and MODULE are set.")
            return()
        endif()
        set(_type SHARED)
    else()
        set(_type MODULE)
        if(_USE_SABI)
            if(NOT TARGET Python::SABIModule)
                message(SEND_ERROR "USE_SABI is set but Python::SABIModule is not found.")
                return()
            endif()
            if(NOT _USE_SABI MATCHES [[^3(\.([0-9]+))$]])
                message(SEND_ERROR "USE_SABI is not of form 3[.<minor>].")
                return()
            endif()
            if (CMAKE_MATCH_2 AND CMAKE_MATCH_2 GREATER_EQUAL 2)
                math(EXPR _minor "${CMAKE_MATCH_2}" OUTPUT_FORMAT HEXADECIMAL)
                string(REPLACE "0x" "" _minor "${_minor}")
                if(CMAKE_MATCH_2 LESS 16)
                    set(_minor "0${_minor}")
                endif()
                set(_Py_LIMITED_API "0x03${_minor}0000")
            else()
                set(_Py_LIMITED_API "3")
            endif()
        endif()
    endif()

    add_library("${_name}" "${_type}" ${_UNPARSED_ARGUMENTS})
    if(_type STREQUAL MODULE)
        if(_USE_SABI)
            target_link_libraries("${_name}" PRIVATE Python::SABIModule)
            target_compile_definitions("${_name}" PRIVATE "Py_LIMITED_API=${_Py_LIMITED_API}")
        else()
            target_link_libraries("${_name}" PRIVATE Python::Module)
        endif()
        set_target_properties("${name}" PROPERTIES POSTFIX "${_Python_POSTFIX}")
        if(WIN32)
            set_target_properties("${_name}" PROPERTIES SUFFIX ".pyd")
        else()
            set_target_properties("${_name}" PROPERTIES PREFIX "")
        endif()
        if(_WITH_SOABI)
            get_target_property(_suffix "${_name}" SUFFIX)
            if(NOT _suffix)
                set(_suffix "${CMAKE_SHARED_MODULE_SUFFIX}")
            endif()
            if(_USE_SABI)
                if(Python_SOSABI)
                    set(_suffix ".${Python_SOSABI}${_suffix}")
                endif()
            else()
                if(Python_SOABI)
                    set(_suffix ".${Python_SOABI}${_suffix}")
                endif()
            endif()
            set_target_properties("${_name}" PROPERTIES SUFFIX "${_suffix}")
        endif()
    else()
        if(_WITH_SOABI)
            message(SEND_ERROR "You must only use WITH_SOABI with MODULE.")
            return()
        endif()
        if(_USE_SABI)
            message(SEND_ERROR "You must only use USE_SABI with MODULE.")
            return()
        endif()
        target_link_libraries("${_name}" PRIVATE Python::Python)
    endif()
endfunction()



if(NOT Python_FIND_QUIETLY)
    message(STATUS "Found Python: ${Python_EXECUTABLE} (found version \"${Python_VERSION}\", via ${CMAKE_CURRENT_LIST_FILE})")
endif()
