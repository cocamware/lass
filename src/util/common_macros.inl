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


 */



#ifndef LASS_GUARDIAN_OF_INCLUSION_UTIL_COMMON_MACROS_INL
#define LASS_GUARDIAN_OF_INCLUSION_UTIL_COMMON_MACROS_INL

#include "common_macros.h"
#include "../io/io_fwd.h"


/** Throw an std::run_time_error with additional info on who has thrown it.
 */
#define LASS_THROW_IMPL(iMessage)\
	do\
	{\
		std::ostringstream message;\
		message << LASS_PRETTY_FUNCTION << ": RUNTIME ERROR: " << iMessage << "\n";\
        LASS_LOG(message.str());\
		throw std::runtime_error(message.str());\
	}\
	while (false)

/** Throw a given exception rather that a runtime error filled with given test.
 */
#define LASS_THROW_EXCEPTION_IMPL(iException)\
    do\
    {\
        /*LASS_LOG(iException.what());*/\
        throw iException;\
    }\
    while (false)

/** The usual assert
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



/** Print a warning to the cerr proxy stream
 */
#define LASS_WARNING_IMPL(iMessage)\
	do\
	{\
		LASS_CERR << "\n" << LASS_PRETTY_FUNCTION << ": WARNING: " << iMessage << "\n";\
	}\
	while (false)



/** Evaluates an expression and prints it to the clog proxy stream
 */
#define LASS_EVAL_IMPL(iExpression)\
	do\
	{\
		LASS_CLOG << "\n" << LASS_PRETTY_FUNCTION << ": " << LASS_STRINGIFY(iExpression) << " == '"\
			<< (iExpression) << "'\n";\
	}\
	while (false)



/** Prints a not to the clog proxy stream
 */
#define LASS_LOG_IMPL(iMessage)\
	do\
	{\
		LASS_CLOG << "\n" << LASS_PRETTY_FUNCTION << ": " << iMessage << "\n";\
	}\
	while (false)


#endif