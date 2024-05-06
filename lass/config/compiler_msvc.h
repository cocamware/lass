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
 *	Copyright (C) 2004-2024 the Initial Developer.
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



#ifndef LASS_GUARDIAN_OF_INCLUSION_CONFIG_COMPILER_MSVC_H
#define LASS_GUARDIAN_OF_INCLUSION_CONFIG_COMPILER_MSVC_H

// identify the compiler
//
#define LASS_COMPILER_VERSION _MSC_VER

#if LASS_COMPILER_VERSION < 1920
#	error This Microsoft Visual C++ version is no longer supported

#elif LASS_COMPILER_VERSION < 1930
#	define LASS_COMPILER "Microsoft Visual C++ 2019"
#	define LASS_LIB_COMPILER "vc142"

#elif LASS_COMPILER_VERSION < 1940
#	define LASS_COMPILER "Microsoft Visual C++ 2022"
#	define LASS_LIB_COMPILER "vc143"

#else
#	define LASS_COMPILER "Microsoft Visual C++"
#	define LASS_LIB_COMPILER "vc"
#endif



#if !defined(_CHAR_UNSIGNED)
#	define LASS_CHAR_IS_SIGNED 1
#endif

// use MSVC style inline assembly
#if !defined(_WIN64)
#	define LASS_HAVE_INLINE_ASSEMBLY_MSVC 1
#endif

// use __int8, __int16, ...
#define LASS_NUM_BASIC_TYPES_HAVE_MSVC 1

#if defined(_WIN64)
#	define LASS_SIZE_T_IS_NOT_ULONG 1
#endif

// use _isnan from <float.h>
#define LASS_NUM_NUM_TRAITS_HAVE_MSVC_FLOAT_H 1



#define LASS_DLL_IMPORT __declspec( dllimport )
#define LASS_DLL_EXPORT __declspec( dllexport )
#define LASS_CALL __cdecl
#define LASS_NO_INLINE __declspec( noinline )
#define LASS_PER_THREAD __declspec( thread )
#define LASS_ALIGN(n) __declspec( align(n) )
#define LASS_ALIGNED(x, n) LASS_ALIGN(n) x


#endif
