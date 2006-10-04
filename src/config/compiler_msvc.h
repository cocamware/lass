/** @file
 *  @author Bram de Greve (bramz@users.sourceforge.net)
 *  @author Tom De Muer (tomdemuer@users.sourceforge.net)
 *
 *  Distributed under the terms of the GPL (GNU Public License)
 *
 *  The LASS License:
 *
 *  Copyright 2004-2006 Bram de Greve and Tom De Muer
 *
 *  LASS is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */



#ifndef LASS_GUARDIAN_OF_INCLUSION_CONFIG_COMPILER_MSVC_H
#define LASS_GUARDIAN_OF_INCLUSION_CONFIG_COMPILER_MSVC_H

// identify the compiler
//
#define LASS_COMPILER_VERSION _MSC_VER

#if LASS_COMPILER_VERSION == 1200
#	define LASS_COMPILER "Microsoft Visual C++ version 6.0"
#	define LASS_LIB_COMPILER "vc6"

#elif LASS_COMPILER_VERSION == 1300
#	define LASS_COMPILER "Microsoft Visual C++ 2002"
#	define LASS_LIB_COMPILER "vc7"

#elif LASS_COMPILER_VERSION == 1310
#	define LASS_COMPILER "Microsoft Visual C++ 2003"
#	define LASS_LIB_COMPILER "vc71"

#elif LASS_COMPILER_VERSION == 1400
#	define LASS_COMPILER "Microsoft Visual C++ 2005"
#	define LASS_LIB_COMPILER "vc8"

#else
#	define LASS_COMPILER "Microsoft Visual C++"
#	define LASS_LIB_COMPILER "vc"
#endif



// warnings and errors
//
#include "pragmas_msvc.h"
#pragma warning(4: 4061)    // enumerate 'identifier' in switch of enum 'enumeration' is not
							// explicitly handled by a case label
#pragma warning(4: 4251)    // class needs to have dll-interface to be used by clients of class 'type2'
#pragma warning(4: 4505)    // unreferenced local function removed
#pragma warning(4: 4710)    // inline function was not inlined
#pragma warning(error: 4035)	// no return value

// broken compilers ...
//
#if LASS_COMPILER_VERSION <= 1300 // 1300 == 7.0
#	define LASS_NO_TEMPLATE_PARTIAL_SPECIALIZATION
#endif

#if !defined(_CHAR_UNSIGNED)
#	define LASS_CHAR_IS_SIGNED
#endif

// use __int8, __int16, ...
#define LASS_NUM_BASIC_TYPES_HAVE_MSVC

// use _isnan from <float.h>
#define LASS_NUM_NUM_TRAITS_HAVE_MSVC_FLOAT_H



#define LASS_DLL_IMPORT __declspec( dllimport )
#define LASS_DLL_EXPORT __declspec( dllexport )
#define LASS_CALL __cdecl
#define LASS_NO_INLINE __declspec( noinline )



#endif
