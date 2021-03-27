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
 *	Copyright (C) 2004-2011 the Initial Developer.
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

/** @namespace lass::util
 *  @brief general utility, debug facilities, ...
 *
 *  The util namespace contains general utilities, debug facilities, etc. used by our lass
 *  software.  The following stuff is provided:
 *
 *	- Allocator: library of custom allocator building blocks
 *	- Atomic: atomic operations on integers
 *	- Bind: bind function and arguments to nullary callback
 *  - @ref BitManip: a set of bit  manipulation routines
 *  - @ref Callback: library to wrap callback functions in first class objects.
 *  - CallTraits: defines @e best types for arguments, references, etc.
 *  - Clock: an class to measure time.
 *  - CloneFactory: implements Alexandrescu's clone factory pattern.
 *  - @ref CommonMacros: a set of common macros for your convenience
 *  - Dictionary: a bidirectional dictonary wrapper around a std::map.
 *  - empty.h: an empty header for those moments you want to include absolutely @e nothing.
 *  - @ref Enforcers: release-time counterpart of assertions to ease condition verifying
 *	- Environment: accessing environment variables
 *  - Exception: the exception class used by LASS
 *  - IdGenerator: Generates unique ID's of the type you want (well, not @e all types).
 *  - NonCopyable: boost::non_copyable
 *  - ObjectFactory: implements Alexandrescu's object factory.
 *  - @ref Process: concerning process settings.
 *  - ProgressIndicator: a class to print a progress message.
 *  - @ref Python: Python binding library
 *  - Singleton: implements the singleton idiom in a non-intrusive way, very easy.
 *  - SmallObject: base class for small objects that need a specialized allocator.
 *  - @ref SmartPtr: library of configurable smart pointers like ScopedPtr and SharedPtr
 *  - @ref stringCast: casts a value to a type by using string streams, to cast to/from strings.
 *  - @ref Threading: basic multithreading library
 *  - @ref ThreadFun: Uses the Bind library to run existing functions in threads
 *	- ThreadPool: producer/consumer pattern with multithreading ...
 *  - @ref VisitorPattern: implements Alexandrescu's visitor pattern.
 */



/** @namespace lass::python::impl
 *  @internal
 */

#ifndef LASS_GUARDIAN_OF_INCLUSION_PYTHON_PYTHON_COMMON_H
#define LASS_GUARDIAN_OF_INCLUSION_PYTHON_PYTHON_COMMON_H

#include "../lass_common.h"

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
