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



#ifndef LASS_GUARDIAN_OF_INCLUSION_UTIL_COMMON_MACROS_INL
#define LASS_GUARDIAN_OF_INCLUSION_UTIL_COMMON_MACROS_INL

#include "common_macros.h"
#include "../io/io_fwd.h"

/** The usual assert
 *  @internal
 */
#define LASS_ASSERT_IMPL(iExpression)\
	do\
	{\
		if (!(iExpression))\
		{\
			LASS_CERR << "\n" << LASS_PRETTY_FUNCTION << ": ASSERT FAILURE: '"\
				<< LASS_STRINGIFY(iExpression) << "'\n";\
			LASS_BREAK_HERE;\
		}\
	}\
	while (false)


/** an assert that always fails (thus should be in unreachable code)
 *  @internal
 */
#define LASS_ASSERT_UNREACHABLE_IMPL\
	do\
	{\
		LASS_CERR << "\n" << LASS_PRETTY_FUNCTION \
			<< ": ASSERT FAILURE: Unreachable code reached.\n";\
		LASS_BREAK_HERE;\
	}\
	while (false)



/** @internal
 */
#define LASS_WARNING_IMPL_ACTION(iMessage)\
	LASS_CERR << "\n" << LASS_PRETTY_FUNCTION << ": WARNING: " << iMessage << "\n"


/** Print a warning to the cerr proxy stream
 *  @internal
 */
#define LASS_WARNING_IMPL(iMessage)\
	do\
	{\
		LASS_WARNING_IMPL_ACTION(iMessage);\
	}\
	while (false)



/** Print a warning to the cerr proxy stream, but only once per run
 *  @internal
 */
#define LASS_WARNING_ONCE_IMPL(iMessage, iUniqueName)\
	do\
	{\
		static bool iUniqueName = false;\
		if (!iUniqueName)\
		{\
			LASS_WARNING_IMPL_ACTION(iMessage);\
			iUniqueName = true;\
		}\
	}\
	while (false)



/** Evaluates an expression and prints it to the clog proxy stream
 *  @internal
 */
#define LASS_EVAL_IMPL(iExpression)\
	do\
	{\
		LASS_CLOG << "\n" << LASS_PRETTY_FUNCTION << ": " << LASS_STRINGIFY(iExpression) << " == '"\
			<< (iExpression) << "'\n";\
	}\
	while (false)



/** Prints a not to the clog proxy stream
 *  @internal
 */
#define LASS_LOG_IMPL(iMessage)\
	do\
	{\
		LASS_CLOG << "\n" << LASS_PRETTY_FUNCTION << ": " << iMessage << "\n";\
	}\
	while (false)


#endif
