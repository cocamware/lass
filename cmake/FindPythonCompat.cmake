#[=======================================================================[.rst:
FindPythonCompat
----------------

Find Python interpreter, compiler and development environment (include
directories and libraries).

This is a wrapper around the new ``FindPython`` module, available since
CMake 3.12. On older CMakes, it will attempt to mimic ``FindPython`` using the
older ``FindPythonInterp`` and ``FindPythonLibs`` modules.

The documentation of ``FindPython`` applies, but only a subset is 
supported. Below the differences are highlighted.

Only two components are supported (``Compiler`` is not supported):

* ``Interpreter``: search for Python interpreter.
* ``Development``: search for development artifacts (include directories and
  libraries).

Imported Targets
^^^^^^^^^^^^^^^^

Only the following targets are supported:

``Python::Interpreter``
  Python interpreter. Target defined if component ``Interpreter`` is found.
``Python::Python``
  Python library. Target defined if component ``Development`` is found.

Result Variables
^^^^^^^^^^^^^^^^

Only the following variables are guaranteed to be set in your project:

``Python_FOUND``
  System has the Python requested components.
``Python_Interpreter_FOUND``
  System has the Python interpreter.
``Python_EXECUTABLE``
  Path to the Python interpreter.
``Python_Development_FOUND``
  System has the Python development artifacts.
``Python_INCLUDE_DIRS``
  The Python include directories.
``Python_LIBRARIES``
  The Python libraries.
``Python_LIBRARY_DIRS``
  The Python library directories.
``Python_VERSION``
  Python version.
``Python_VERSION_MAJOR``
  Python major version.
``Python_VERSION_MINOR``
  Python minor version.
``Python_VERSION_PATCH``
  Python patch version.
#]=======================================================================]

if(CMAKE_VERSION VERSION_LESS 3.12)

    # CMake < 3.12 doesn't have FindPython.cmake yet.  Mimic its behaviour
    # using FindPythonInterp.cmake and FindPythonLibs.cmake

    unset(_PythonCompat_ARGS)
    if(PythonCompat_FIND_VERSION)
        list(APPEND _PythonCompat_ARGS ${PythonCompat_FIND_VERSION})
    endif()
    if(PythonCompat_FIND_VERSION_EXACT)
        list(APPEND _PythonCompat_ARGS EXACT)
    endif()
    if(PythonCompat_FIND_QUIETLY)
        list(APPEND _PythonCompat_ARGS QUIET)
    endif()

    if(NOT PythonCompat_FIND_COMPONENTS)
        set(PythonCompat_FIND_COMPONENTS Interpreter)
        set(PythonCompat_FIND_REQUIRED_Interpreter TRUE)
    endif()

    set(_PythonCompat_REQUIRED_VARS)

    if(DEFINED PythonCompat_FIND_REQUIRED_Interpreter)
        if(Python_EXECUTABLE AND NOT PYTHON_EXECUTABLE)
            set(PYTHON_EXECUTABLE ${Python_EXECUTABLE} CACHE FILEPATH
                "Path to a program." FORCE)
        endif()

        find_package(PythonInterp ${_PythonCompat_ARGS})

        set(Python_Interpreter_FOUND ${PYTHONINTERP_FOUND})
        set(Python_EXECUTABLE ${PYTHON_EXECUTABLE})
        set(Python_VERSION ${PYTHON_VERSION_STRING})
        set(Python_VERSION_MAJOR ${PYTHON_VERSION_MAJOR})
        set(Python_VERSION_MINOR ${PYTHON_VERSION_MINOR})
        set(Python_VERSION_PATCH ${PYTHON_VERSION_PATCH})

        if(NOT TARGET Python::Interpreter AND PYTHONINTERP_FOUND)
            add_executable (Python::Interpreter IMPORTED)
            set_property(TARGET Python::Interpreter PROPERTY
                IMPORTED_LOCATION "${PYTHON_EXECUTABLE}")
        endif()

        if(PythonCompat_FIND_REQUIRED_Interpreter)
            list(APPEND _PythonCompat_REQUIRED_VARS PYTHON_EXECUTABLE)
        endif()
    endif()

    if(PYTHONINTERP_FOUND AND PYTHON_VERSION_STRING)
        # if we have an interpreter, reconstruct the ARGS to find an EXACT
        # match for the libraries
        set(_PythonCompat_ARGS "${PYTHON_VERSION_STRING}" EXACT)
        if(PythonCompat_FIND_QUIETLY)
            list(APPEND _PythonCompat_ARGS QUIET)
        endif()
    endif()

    if(DEFINED PythonCompat_FIND_REQUIRED_Development)
        find_package(PythonLibs ${_PythonCompat_ARGS})

        set(Python_Development_FOUND ${PYTHONLIBS_FOUND})
        set(Python_INCLUDE_DIRS ${PYTHON_INCLUDE_DIRS})
        set(Python_LIBRARIES ${PYTHON_LIBRARIES})
        #set(Python_LIBRARY_DIRS ${PYTHON_EXECUTABLE})
        #set(Python_RUNTIME_LIBRARY_DIRS ${PYTHON_EXECUTABLE})

        set(Python_INCLUDE_DIR ${PYTHON_INCLUDE_DIR})
        set(Python_LIBRARY_RELEASE ${PYTHON_LIBRARY_RELEASE})
        set(Python_LIBRARY_DEBUG ${PYTHON_DEBUG_LIBRARY})

        if(NOT TARGET Python::Python AND PYTHONLIBS_FOUND)
            if(PYTHON_LIBRARY MATCHES "${CMAKE_SHARED_LIBRARY_SUFFIX}$" OR
                PYTHON_DEBUG_LIBRARY MATCHES "${CMAKE_SHARED_LIBRARY_SUFFIX}$")
                set(_PythonCompat_LIBRARY_TYPE SHARED)
            else()
                set(_PythonCompat_LIBRARY_TYPE UNKNOWN)
            endif()
            add_library(Python::Python "${_PythonCompat_LIBRARY_TYPE}" IMPORTED)
            set_property(TARGET Python::Python PROPERTY
                INTERFACE_INCLUDE_DIRECTORIES "${PYTHON_INCLUDE_DIRS}")
            if(PYTHON_DEBUG_LIBRARY)
                set_property(TARGET Python::Python APPEND PROPERTY
                    IMPORTED_CONFIGURATIONS RELEASE)
                set_target_properties(Python::Python PROPERTIES
                    IMPORTED_LINK_INTERFACE_LANGUAGES_RELEASE "C"
                    IMPORTED_LOCATION_RELEASE "${PYTHON_LIBRARY_RELEASE}")
                set_property(TARGET Python::Python APPEND PROPERTY
                    IMPORTED_CONFIGURATIONS DEBUG)
                set_target_properties(Python::Python PROPERTIES
                    IMPORTED_LINK_INTERFACE_LANGUAGES_DEBUG "C"
                    IMPORTED_LOCATION_DEBUG "${PYTHON_DEBUG_LIBRARY}")
            else()
                set_target_properties(Python::Python PROPERTIES
                    IMPORTED_LINK_INTERFACE_LANGUAGES "C"
                    IMPORTED_LOCATION "${PYTHON_LIBRARY}")
            endif()
        endif()

        if(PythonCompat_FIND_REQUIRED_Development)
            list(APPEND _PythonCompat_REQUIRED_VARS PYTHON_LIBRARIES
                                                    PYTHON_INCLUDE_DIRS)
        endif()
    endif()

    include(FindPackageHandleStandardArgs)
    set(Python_FIND_VERSION ${PythonCompat_FIND_VERSION})
    set(Python_FIND_VERSION_COUNT ${PythonCompat_FIND_VERSION_COUNT})
    set(Python_FIND_VERSION_EXACT ${PythonCompat_FIND_VERSION_EXACT})
    set(Python_FIND_QUIETLY ${PythonCompat_FIND_QUIETLY})
    set(Python_FIND_COMPONENTS ${PythonCompat_FIND_COMPONENTS})
    set(_PythonCompat_VERSION ${Python_VERSION})
    find_package_handle_standard_args(Python
        FOUND_VAR Python_FOUND
        REQUIRED_VARS ${_PythonCompat_REQUIRED_VARS}
        VERSION_VAR _PythonCompat_VERSION
        HANDLE_COMPONENTS)

else()

    # Cmake 3.12 has FindPython.cmake, so we can simply forward to it

    unset(_PythonCompat_ARGS)
    if(PythonCompat_FIND_VERSION)
        list(APPEND _PythonCompat_ARGS ${PythonCompat_FIND_VERSION})
    endif()
    if(PythonCompat_FIND_VERSION_EXACT)
        list(APPEND _PythonCompat_ARGS EXACT)
    endif()
    if(PythonCompat_FIND_QUIETLY)
        list(APPEND _PythonCompat_ARGS QUIET)
    endif()

    unset(_PythonCompat_COMPONENTS)
    unset(_PythonCompat_OPTIONAL_COMPONENTS)
    foreach(_PythonCompat_COMPONENT ${PythonCompat_FIND_COMPONENTS})
        if(PythonCompat_FIND_REQUIRED_${_PythonCompat_COMPONENT})
            list(APPEND _PythonCompat_COMPONENTS "${_PythonCompat_COMPONENT}")
        else()
            list(APPEND _PythonCompat_OPTIONAL_COMPONENTS 
                "${_PythonCompat_COMPONENT}")
        endif()
    endforeach()

    find_package(Python ${_PythonCompat_ARGS}
        COMPONENTS ${_PythonCompat_COMPONENTS}
        OPTIONAL_COMPONENTS ${_PythonCompat_OPTIONAL_COMPONENTS})

endif()

set(PythonCompat_FOUND ${Python_FOUND})
