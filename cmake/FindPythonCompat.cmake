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
  Python library for embedding. Target defined if component ``Development`` is
  found.
``Python::Module``
  Python library for modules. Target defined if component ``Development`` is
  found. On Windows, this is the same as ``Python::Python``. Otherwise, it only
  defines the ``INTERFACE_INCLUDE_DIRECTORIES``, as modules don't need to 
  link to the Python library because the interpreter already provides the
  symbols.

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

message(DEPRECATION "PythonCompat is deprecated. Directly use find_package(Python) instead.")

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

set(PythonCompat_FOUND "${Python_FOUND}")
