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
 *	Copyright (C) 2004-2010 the Initial Developer.
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



/** @namespace lass
 *  @brief Library for Assembled Shared Sources
 */



/** @mainpage lass: Library of Assembled Shared Sources
 *  @author Bram de Greve (bram@www.cocamware.com)
 *  @author Tom De Muer (tom@www.cocamware.com)
 *
 *  - lass::io: streams, binary streams, vrmlstreams, ...
 *  - lass::meta: a meta programming library for your cunning plans.
 *  - lass::num: numeric primitives and functions
 *  - lass::prim: geometric primitives
 *  - lass::python: creating links back and forth between C++ and Python
 *  - lass::spat: spatial subdivisions, quadtrees, octrees, meshes in 2D and 3D, triangulators, ...
 *  - lass::stde: std extensions often Python inspired in their nature
 *  - lass::util: general utility, debug facilities, ...
 *
 *  Distributed under the terms of CPAL
 *
 *  The LASS License:
 *
 *	The contents of this file are subject to the Common Public Attribution License 
 *	Version 1.0 (the "License"); you may not use this file except in compliance with 
 *	the License. You may obtain a copy of the License at 
 *	http://lass.cocamware.com/cpal-license. The License is based on the 
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
 *	Copyright (C) 2004-2010 the Initial Developer.
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
 */


#ifndef LASS_GUARDIAN_OF_INCLUSION_LASS_COMMON_H
#define LASS_GUARDIAN_OF_INCLUSION_LASS_COMMON_H

#include "config/config.h"

#define LASS_HEX_VERSION (LASS_MAJOR_VERSION << 16 | LASS_MINOR_VERSION << 8 | LASS_PATCHLEVEL)
#define LASS_VERSION LASS_STRINGIFY(LASS_MAJOR_VERSION) "." LASS_STRINGIFY(LASS_MINOR_VERSION) "." LASS_STRINGIFY(LASS_PATCHLEVEL)

#ifdef LASS_UTIL_THREAD_HAVE_POSIX
#	ifndef _REENTRANT
#		define _REENTRANT
#	endif
#endif


// --- Check if we have RTTI and warn if we don't ---

// If you have the Intel C++ Compiler 7.1 installed, it screws up the part of MSVC's IDE
// were you can enable RTTI.  Therefore, you must add /GR to the commandline explicitely
// http://tinyurl.com/8usvw
#if !defined(_CPPRTTI) && LASS_COMPILER_TYPE == LASS_COMPILER_TYPE_MSVC
#	pragma message("[LASS BUILD MSG] RTTI not enabled ... Add /GR to commandline options if you need it.")
#endif



// --- Here we'll define some stuff on the name of libraries lass creates ---

/** @def LASS_LIB_DEBUG
 *  @brief part of the library name that indicates if it is a release or debug build.
 */
#ifdef _DEBUG
#	define LASS_LIB_DEBUG "_d"
#else
#	define LASS_LIB_DEBUG ""
#endif

#if LASS_COMPILER_TYPE == LASS_COMPILER_TYPE_MSVC && LASS_COMPILER_VERSION >= 1400
	// currently we only do this for vc8, probably this will stay so ...
#	if defined(LASS_BUILD_DLL) || defined(LASS_USE_DLL)
		// currently we only do this for the DLL edition, probably this will stay so ...
#		ifdef _M_IX86_FP
#			if _M_IX86_FP == 2
#				define LASS_LIB_ARCH "_sse2"
#			endif
#		endif
#	endif
#endif
#ifndef LASS_LIB_ARCH
#	define LASS_LIB_ARCH ""
#endif



/** @def LASS_LIB_PREFIX
 *  @brief prefix of all library names that will be made
 */
//#define LASS_LIB_PREFIX "lass_"

/** @def LASS_LIB_SUFFIX
 *  @brief suffix of all library names that will be made, indicates platform, compiler and build.
 */
//#define LASS_LIB_SUFFIX "_" LASS_LIB_PLATFORM "_" LASS_LIB_COMPILER LASS_LIB_ARCH LASS_LIB_DEBUG ".lib"

/** @def LASS_DLL
 *  @brief DLL interface: import or export symbols?  or neither?
 */
#ifdef LASS_DLL
#	undef LASS_DLL
#endif
#if LASS_SHARED_LIBRARY
#	if defined(LASS_EXPORTS) || defined (lass_EXPORTS)
#		define LASS_DLL LASS_DLL_EXPORT
#	else
#		define LASS_DLL LASS_DLL_IMPORT
#	endif
#else 
#	define LASS_DLL LASS_DLL_EXPORT
#endif



// --- frequently used STL mumbo jumbo ---

#include <typeinfo>
#include <exception>
#include <stdexcept>

#include <utility>
#include <functional>
#include <memory>

#include <string>

#include <locale>

#include <vector>
#include <list>
#include <deque>
#include <map>
#include <set>

#include <iterator>

#include <algorithm>

#include <complex>
#include <numeric>

#include <iostream>
#include <fstream>
#include <sstream>
#include <iomanip>



// --- common lass stuff ---

#include "util/common_macros.h"
#include "util/call_traits.h"
#include "util/exception.h"
#include "util/enforcer.h"
#include "util/string_cast.h"
#include "io/io_fwd.h"
#include "io/proxy_man.h"
#include "stde/extended_io.h"

#endif
