/** 
*   @author Bram de Greve (bramz@users.sourceforge.net)
*   @author Tom De Muer (tomdemuer@users.sourceforge.net)
*
*	Distributed under the terms of the GPL (GNU Public License)
*
* 	The LASS License:
*   
*	Copyright 2004 Bram de Greve and Tom De Muer
*
*   LASS is free software; you can redistribute it and/or modify
*   it under the terms of the GNU General Public License as published by
*   the Free Software Foundation; either version 2 of the License, or
*   (at your option) any later version.
*
*   This program is distributed in the hope that it will be useful,
*   but WITHOUT ANY WARRANTY; without even the implied warranty of
*   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
*   GNU General Public License for more details.
*
*   You should have received a copy of the GNU General Public License
*   along with this program; if not, write to the Free Software
*   Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
*


 *
 *  This whole config thing is somehow "borowed" of boost.  http://www.boost.org.
 *
 *  @warning from now, 'to boost' is a verb meaning "using Boost". :)
 */



#ifndef LASS_GUARDIAN_OF_INCLUSION_CONFIG_CONFIG_H
#define LASS_GUARDIAN_OF_INCLUSION_CONFIG_CONFIG_H

/** @def #define LASS_BOOST_ALWAYS
 *  @brief Define this if you have Boost (www.boost.org) around, and you always want to fall back 
 *         on it whenever possible.
 *
 *  This is when you always want to use the boost implementations of lass::util::CallTraits<>, ...
 *  Obviously, you'll have to have boost installed.
 *
 *  @warning if you neither define LASS_BOOST_ALWAYS or LASS_BOOST_IF_BROKEN, so stuff might
 *           not want to compile, or you'll have bad performance on some stuff.
 */
//#define LASS_BOOST_ALWAYS

/** @def LASS_BOOST_IF_BROKEN
 *  @brief Define this if you want to fall back on Boost (www.boost.org) for broken compilers only.
 *
 *  If you define this, we'll try to use our own implentations of lass::util::CallTraits<>, etc., but only
 *  if the compiler is willing to play it nicely.  If not, we'll fall back on boost.
 *
 *  So, if you have a nice compiler (like the intel one :), you won't need to have boost around to 
 *  compile.  But if you have a broken compiler, you'll have to have boost if you want this to work.
 *
 *  @warning if you neither define LASS_BOOST_ALWAYS or LASS_BOOST_IF_BROKEN, some stuff might
 *           not want to compile, or you'll have bad performance on some stuff.
 */
#define LASS_BOOST_IF_BROKEN

/** @def LASS_CHAR_IS_SIGNED
 *  Define this if you want the "char" type to be the "equivalent" of the "signed char".
 *  This is implementation dependent and can be set in the compiler options.  The default
 *  assumes the "unsigned char" and hence there is no need to define the "LASS_CHAR_IS_SIGNED".  
 */
//#define LASS_CHAR_IS_SIGNED



// go check compiler and platform support
//
#include "compilers.h"
#include "platforms.h"



#define LASS_DLL_IMPORT   __declspec( dllimport )
#define LASS_DLL_EXPORT   __declspec( dllexport )

// experimental
#define LASS_CALL __cdecl
#define LASS_DLL
//

#define LASS_NO_INLINE   __declspec( noinline )
 


// debug mode of STL port or not?
//
/*
#ifdef _DEBUG
#	define _STLP_DEBUG 1
#else
#	undef _STLP_DEBUG
#endif
/**/


// windows specific defines
#ifdef WIN32
#	define NOMINMAX
#endif



#endif