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



#ifndef LASS_GUARDIAN_OF_INCLUSION_CONFIG_COMPILER_INTEL_H
#define LASS_GUARDIAN_OF_INCLUSION_CONFIG_COMPILER_INTEL_H

#if defined(__INTEL_COMPILER)
#	define LASS_COMPILER_VERSION __INTEL_COMPILER

#elif defined(__ICL)
#	define LASS_COMPILER_VERSION __ICL

#elif defined(__ICC)
#	define LASS_COMPILER_VERSION __ICC

#elif defined(__ECC)
#	define LASS_COMPILER_VERSION __ECC

#else
#	define LASS_COMPILER_VERSION -1

#endif


#define LASS_COMPILER "Intel C++ version " LASS_STRINGIFY(LASS_COMPILER_VERSION)
#define LASS_LIB_COMPILER "intel" LASS_STRINGIFY(LASS_COMPILER_VERSION)

// a little hack for intel 7.1
//
#if defined(__INTEL_COMPILER) && __INTEL_COMPILER == 710
#	undef LASS_COMPILER_VERSION
#	define LASS_COMPILER_VERSION 700
#endif

#if !defined(_CHAR_UNSIGNED)
#	define LASS_CHAR_IS_SIGNED
#endif

// use __int8 and friends
#define LASS_NUM_BASIC_TYPES_HAVE_MSVC

// use _isnan from MSVC <float.h>
#define LASS_NUM_NUM_TRAITS_HAVE_MSVC_FLOAT_H

#define LASS_DLL_IMPORT __declspec( dllimport )
#define LASS_DLL_EXPORT __declspec( dllexport )
#define LASS_CALL __cdecl
#define LASS_NO_INLINE __declspec( noinline )

#endif
