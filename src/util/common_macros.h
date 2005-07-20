/** @file
 *  @author Bram de Greve (bramz@users.sourceforge.net)
 *  @author Tom De Muer (tomdemuer@users.sourceforge.net)
 *
 *  Distributed under the terms of the GPL (GNU Public License)
 *
 *  The LASS License:
 *
 *  Copyright 2004 Bram de Greve and Tom De Muer
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


/** @defgroup CommonMacros Common Macros
 *  @author Bramz
 *  @brief usefull macros to use all over the place.
 */



#ifndef LASS_GUARDIAN_OF_INCLUSION_UTIL_MACRO_H
#define LASS_GUARDIAN_OF_INCLUSION_UTIL_MACRO_H



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
#define LASS_BREAK_HERE                 __asm { int 3 }

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

/** current function
 *  @ingroup CommonMacros
 */
#ifdef __FUNCTION__
#define LASS_FUNCTION                   __FUNCTION__
#define LASS_PRETTY_FUNCTION            LASS_FUNCTION
#else
#define LASS_FUNCTION                   "??"
#define LASS_PRETTY_FUNCTION            LASS_FILE << "(" << LASS_LINE << ")"
#endif

#define LASS_HERE                       LASS_FILE "(" LASS_STRINGIFY(LASS_LINE) ")"



// debug jumbo jambo

#include "common_macros.inl"

#ifdef _DEBUG
//# define LASS_THROW( iMessage )               LASS_THROW_IMPL( iMessage )
#	define LASS_THROW_EXCEPTION( iException )   LASS_THROW_EXCEPTION_IMPL( iException )
#	define LASS_ASSERT( iExpression )           LASS_ASSERT_IMPL( iExpression )
#	define LASS_ASSERT_UNREACHABLE				LASS_ASSERT_UNREACHABLE_IMPL
#	define LASS_WARNING( iMessage )             LASS_WARNING_IMPL( iMessage )
#	define LASS_EVAL( iExpression )             LASS_EVAL_IMPL( iExpression )
#	define LASS_LOG( iMessage )                 LASS_LOG_IMPL( iMessage )
#else
//# define LASS_THROW( iMessage )               LASS_THROW_IMPL( iMessage )
#	define LASS_THROW_EXCEPTION( iException )   LASS_THROW_EXCEPTION_IMPL( iException )
#	define LASS_ASSERT( iExpression )
#	define LASS_ASSERT_UNREACHABLE
#	define LASS_WARNING( iMessage )             LASS_WARNING_IMPL( iMessage )
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
	namespace { inline bool LASS_CONCATENATE( LASS_UNIQUENAME(iPrefix),func ) () { \
	x \
	return true; \
	}; \
	const bool LASS_CONCATENATE( LASS_UNIQUENAME(iPrefix),var ) = LASS_CONCATENATE( LASS_UNIQUENAME(iPrefix),func ) (); }


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



#endif
