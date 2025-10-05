/**	@file
 *	@author Bram de Greve (bram@cocamware.com)
 *	@author Tom De Muer (tom@cocamware.com)
 *
 *	*** BEGIN LICENSE INFORMATION ***
 *	
 *	The contents of this file are subject to the Common Public Attribution License 
 *	Version 1.0 (the "License"); you may not use this file except in compliance with 
 *	the License. You may obtain a copy of the License at 
 *	http://lass.sourceforge.net/cpal-license. The License is based on the 
 *	Mozilla Public License Version 1.1 but Sections 14 and 15 have been added to cover 
 *	use of software over a computer network and provide for limited attribution for 
 *	the Original Developer. In addition, Exhibit A has been modified to be consistent 
 *	with Exhibit B.
 *	
 *	Software distributed under the License is distributed on an "AS IS" basis, WITHOUT 
 *	WARRANTY OF ANY KIND, either express or implied. See the License for the specific 
 *	language governing rights and limitations under the License.
 *	
 *	The Original Code is LASS - Library of Assembled Shared Sources.
 *	
 *	The Initial Developer of the Original Code is Bram de Greve and Tom De Muer.
 *	The Original Developer is the Initial Developer.
 *	
 *	All portions of the code written by the Initial Developer are:
 *	Copyright (C) 2004-2025 the Initial Developer.
 *	All Rights Reserved.
 *	
 *	Contributor(s):
 *
 *	Alternatively, the contents of this file may be used under the terms of the 
 *	GNU General Public License Version 2 or later (the GPL), in which case the 
 *	provisions of GPL are applicable instead of those above.  If you wish to allow use
 *	of your version of this file only under the terms of the GPL and not to allow 
 *	others to use your version of this file under the CPAL, indicate your decision by 
 *	deleting the provisions above and replace them with the notice and other 
 *	provisions required by the GPL License. If you do not delete the provisions above,
 *	a recipient may use your version of this file under either the CPAL or the GPL.
 *	
 *	*** END LICENSE INFORMATION ***
 */

#ifndef LASS_GUARDIAN_OF_INCLUSION_PYTHON_PYTHON_COMMON_H
#define LASS_GUARDIAN_OF_INCLUSION_PYTHON_PYTHON_COMMON_H

#include "../lass_common.h"

/** @namespace lass::python
 *  @brief Comprehensive C++ to Python binding library
 *  
 *  The lass::python namespace provides a comprehensive Python binding system that allows
 *  easy integration between C++ code and Python scripts. Lass Python bindings support
 *  both native Python-aware classes and shadow classes for existing C++ types.
 *
 *  **Key Components:**
 *
 *  - @ref ModuleDefinition "ModuleDefinition": Macros to create and configure Python modules
 *  - @ref ClassDefinition "ClassDefinition": Macros to export C++ classes as Python types
 *  - PyObjectPlus: Base class for Python-aware C++ objects
 *  - Shadow Classes: Wrapper system for existing non-Python-aware C++ classes
 *  - @ref PyExportTraits "PyExportTraits": Type conversion system between C++ and Python objects
 *  - Overload Resolution: Automatic dispatcher generation for overloaded functions/methods
 *
 *  **Class Export Approaches:**
 *
 *  1. **Native Python Classes**: C++ classes that inherit from PyObjectPlus
 *     and are designed to be Python-compatible from the start
 *
 *  2. **Shadow Classes**: Wrapper classes for existing C++ types using
 *     PY_SHADOW_CLASS macros - the shadow inherits from PyObjectPlus and
 *     wraps the original type
 *
 *  **Binding Features:**
 *
 *  - Multiple constructor overloads with automatic dispatching
 *  - Method overloading mixing member functions and free functions
 *  - Property binding with getter/setter pairs or direct member access
 *  - Python special methods (`__add__`, `__str__`, `__eq__`, etc.)
 *  - Static methods and class constants
 *  - Nested classes and enumerations
 *  - Automatic type conversion and argument validation
 *  - Exception mapping between C++ and Python
 */
namespace lass::python
{

/** @defgroup Python Python bindings
 *  @copydoc lass::python
 */

}

#ifdef LASS_PYTHON_DLL
#	undef LASS_PYTHON_DLL
#endif
#if LASS_SHARED_LIBRARY // yes, it's the same flag.
#	if defined(LASS_PYTHON_EXPORTS) || defined (lass_python_EXPORTS)
#		define LASS_PYTHON_DLL LASS_DLL_EXPORT
#	else
#		define LASS_PYTHON_DLL LASS_DLL_IMPORT
#	endif
#else 
#	define LASS_PYTHON_DLL LASS_DLL_EXPORT
#endif

// Python.h is a bit blunt in (re)defining _POSIX_C_SOURCE causing a nice warning.
// Undefing it before including Python.h will suppress that warning.
// Remove this once Python plays nice again. 
// [Bramz]
#if defined(LASS_HAVE_PYTHON_POSIX_C_SOURCE) && defined(_POSIX_C_SOURCE)
#	undef _POSIX_C_SOURCE
#endif
#if defined(LASS_HAVE_PYTHON_FILE_OFFSET_BITS) && defined(_FILE_OFFSET_BITS)
#	undef _FILE_OFFSET_BITS
#endif
#if defined(LASS_HAVE_PYTHON_XOPEN_SOURCE) && defined(_XOPEN_SOURCE)
#	undef _XOPEN_SOURCE
#endif

// Python 3.12 yields following warning in unicodeobject.h:
// warning C4100: '_unused_op': unreferenced formal parameter
#if LASS_COMPILER_TYPE == LASS_COMPILER_TYPE_MSVC
#	pragma warning(push)
#	pragma warning(disable: 4100) // unreferenced formal parameter
#endif

// Python >= 3.2 defines `PyType_Spec` in `object.h` which has a member called
// `slots`. When combining this with Qt, `slots` may already be defined as a
// macro. It is sufficient to temporarily undefine `slots` since this member
// is not being used on the client side.
// `push_macro` is supported by MSVC, GCC and clang.
#pragma push_macro("slots")
#undef slots

#if defined(_DEBUG) && LASS_PYTHON_HAS_DEBUG_BUILD == 0
#	undef _DEBUG
#	include "Python.h"
#	define _DEBUG
#else
#	include "Python.h"
#endif

#pragma pop_macro("slots")

#if LASS_COMPILER_TYPE == LASS_COMPILER_TYPE_MSVC
#	pragma warning(pop)
#endif

// oh r'ly? Because of some issue lost to ancient history, pyport.h will
// redefine following functions on FreeBSD > 500039 (and Apple?),
// causing really havoc with the C++ code. 
// Undefine! Hopefully it's not to late yet.
#ifdef _PY_PORT_CTYPE_UTF8_ISSUE
#	undef isalnum
#	undef isalpha
#	undef islower
#	undef isspace
#	undef isupper
#	undef tolower
#	undef toupper
#endif

#endif
