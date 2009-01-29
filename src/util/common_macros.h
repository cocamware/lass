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
 *	Copyright (C) 2004-2009 the Initial Developer.
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


/** @defgroup CommonMacros Common Macros
 *  @author Bramz
 *  @brief usefull macros to use all over the place.
 */



#ifndef LASS_GUARDIAN_OF_INCLUSION_UTIL_MACRO_H
#define LASS_GUARDIAN_OF_INCLUSION_UTIL_MACRO_H


#define LASS_WRAP(x)					x

// first of all, the usual stuff on stringifying, concatenating and breakpoint preprocessor stuff.

/** convert to string at preprocessor time.
 *  @ingroup CommonMacros
 */
#define LASS_STRINGIFY(iA)              LASS_STRINGIFY_IMPL(iA)
#define LASS_STRINGIFY_IMPL(iA)         #iA

/** concatenate two words at preprocessor time.
 *  @ingroup CommonMacros
 */
#define LASS_CONCATENATE( iA, iB )      LASS_CONCATENATE_IMPL(iA, iB)
#define LASS_CONCATENATE_IMPL( iA, iB ) iA##iB

/** concatenate three words at preprocessor time.
 *  @ingroup CommonMacros
 */
#define LASS_CONCATENATE_3( iA, iB, iC )    LASS_CONCATENATE_3_IMPL(iA, iB, iC)
#define LASS_CONCATENATE_3_IMPL( iA, iB, iC ) iA##iB##iC

/** user breakpoint
 *  @ingroup CommonMacros
 */
#if LASS_COMPILER_TYPE == LASS_COMPILER_TYPE_GCC
#	define LASS_BREAK_HERE	__asm__("int3")
#else
#	if defined(_WIN64)
#		define LASS_BREAK_HERE __debugbreak();
#	else
#		define LASS_BREAK_HERE	__asm { int 3 }
#	endif
#endif

/** create a unique name based on a prefix and line number
 *  @ingroup CommonMacros
 */
#define LASS_UNIQUENAME( iPrefix ) LASS_CONCATENATE(iPrefix,LASS_LINE)



// second, the name of the game, where we are.

/** current file
 *  @ingroup CommonMacros
 */
#define LASS_FILE                       __FILE__

/** current line (stringified!)
 *  @ingroup CommonMacros
 */
#define LASS_LINE                       __LINE__

#define LASS_HERE                       LASS_FILE "(" LASS_STRINGIFY(LASS_LINE) ")"

/** current function
 *  @ingroup CommonMacros
 */
#ifdef __FUNCTION__
#	define LASS_FUNCTION			__FUNCTION__
#	define LASS_PRETTY_FUNCTION		LASS_FUNCTION
#else
#	define LASS_FUNCTION			"??"
#	define LASS_PRETTY_FUNCTION		LASS_HERE
#endif




// debug jumbo jambo

#include "common_macros.inl"

#if !defined(NDEBUG)
#	define LASS_ASSERT( iExpression )           LASS_ASSERT_IMPL( iExpression )
#	define LASS_ASSERT_UNREACHABLE				LASS_ASSERT_UNREACHABLE_IMPL
#	define LASS_WARNING( iMessage )             LASS_WARNING_IMPL( iMessage )
#	define LASS_WARNING_ONCE_EX( iMessage, iUniqueName ) LASS_WARNING_ONCE_IMPL( iMessage, LASS_UNIQUENAME(iUniqueName) )
#	define LASS_WARNING_ONCE( iMessage )        LASS_WARNING_ONCE_EX( iMessage, lassWarnOnce )
#	define LASS_EVAL( iExpression )             LASS_EVAL_IMPL( iExpression )
#	define LASS_LOG( iMessage )                 LASS_LOG_IMPL( iMessage )
#else
#	define LASS_ASSERT( iExpression )
#	define LASS_ASSERT_UNREACHABLE
#	define LASS_WARNING( iMessage )             LASS_WARNING_IMPL( iMessage )
#	define LASS_WARNING_ONCE_EX( iMessage, iUniqueName ) LASS_WARNING_ONCE_IMPL( iMessage, LASS_UNIQUENAME(iUniqueName) )
#	define LASS_WARNING_ONCE( iMessage )        LASS_WARNING_ONCE_EX( iMessage, lassWarnOnce )
#	define LASS_EVAL( iExpression )             LASS_EVAL_IMPL( iExpression )
#	define LASS_LOG( iMessage )                 LASS_LOG_IMPL( iMessage )
#endif

/** Executes code before the main has actually begun.  The macro relies on line numbers, so
 *  only one LASS_EXECUTE_BEFORE_MAIN per line!
 *  @ingroup CommonMacros
 */
#define LASS_EXECUTE_BEFORE_MAIN( x ) LASS_EXECUTE_BEFORE_MAIN_EX(lassDummyName, x)

/** Same as LASS_EXECUTE_BEFORE_MAIN but with an additional parameter that let's you specify the
 *  used prefix.   Mmmh, let's use this as implementation of LASS_EXECUTE_BEFORE_MAIN :)
 *  @ingroup CommonMacros
 */
#define LASS_EXECUTE_BEFORE_MAIN_EX( iPrefix, x ) \
	namespace \
	{ \
		inline bool LASS_CONCATENATE( LASS_UNIQUENAME(iPrefix),func ) () \
		{ \
			x \
			return true; \
		} \
		const bool LASS_CONCATENATE( LASS_UNIQUENAME(iPrefix),var ) = \
			LASS_CONCATENATE( LASS_UNIQUENAME(iPrefix),func ) (); \
	}


// now some pragma's you can use to leave messages behind

/** pragma for leaving notes in build window
 *  @ingroup CommonMacros
 */
#define LASS_NOTE( iMessage )\
	message(LASS_FILE "(" LASS_STRINGIFY(LASS_LINE) "):\n[LASS BUILD MSG] " iMessage)


/** pragma for leaving todo notes in build window
 *  @ingroup CommonMacros
 */
#define LASS_TODO( iMessage )\
	message(LASS_FILE "(" LASS_STRINGIFY(LASS_LINE) "):\n[LASS BUILD MSG] *** TODO: " iMessage " ***")

/** pragma for leaving fixme notes in build window
 *  @ingroup CommonMacros
 */
#define LASS_FIXME( iMessage )\
	message(LASS_FILE "(" LASS_STRINGIFY(LASS_LINE) "):\n[LASS BUILD MSG] *** FIXME: " iMessage " ***")


// some other usefull macros

/** @def LASS_UNUSED
 *  tell the compiler that some variable may stay unused.
 *  @ingroup CommonMacros
 *  Some compilers throw warnings at us if a variable stays unused.
 *  Often this is a good thing, but sometimes that's exactly what we want.
 *  This macro will attempt to avoid the warning (as long as the 
 *  compiler suports such an attempt).
 *
 *  @code
 *  int LASS_UNUSED(foobar) = 0;
 *  @endcode
 */
#if LASS_COMPILER_TYPE == LASS_COMPILER_TYPE_GCC
#	define LASS_UNUSED(x) x __attribute__((unused))
#else
#	define LASS_UNUSED(x) x
#endif

#endif
