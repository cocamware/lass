/**	@file
 *	@author Bram de Greve (bramz@users.sourceforge.net)
 *	@author Tom De Muer (tomdemuer@users.sourceforge.net)
 *
 *	Distributed under the terms of the GPL (GNU Public License)
 *
 * 	The LASS License:
 *
 *	Copyright 2004 Bram de Greve and Tom De Muer
 *
 *	LASS is free software; you can redistribute it and/or modify
 *	it under the terms of the GNU General Public License as published by
 *	the Free Software Foundation; either version 2 of the License, or
 *	(at your option) any later version.
 *
 *	This program is distributed in the hope that it will be useful,
 *	but WITHOUT ANY WARRANTY; without even the implied warranty of
 *	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *	GNU General Public License for more details.
 *
 *	You should have received a copy of the GNU General Public License
 *	along with this program; if not, write to the Free Software
 *	Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */



/** @class lass::util::Exception
 *  @brief type of all exceptions in lass
 *  @author Bram de Greve [Bramz]
 *
 *  @sa LASS_THROW
 */

#ifndef LASS_GUARDIAN_OF_INCLUSION_UTIL_EXCEPTION_H
#define LASS_GUARDIAN_OF_INCLUSION_UTIL_EXCEPTION_H

#include "util_common.h"
#include <exception>
#include <string>
#include <sstream>

namespace lass
{
namespace util
{

class Exception: public std::exception
{
public:

	Exception(const std::string& iMessage, const std::string& iLocation):
		std::exception(),
		message_(iMessage),
		location_(iLocation)
	{
	}
	
	~Exception() throw() {}

	/** return message in STL style
	 */
	virtual const char* what() const throw()
	{ 
		return message_.c_str(); 
	}

	/** return string with error message
	 */
	const std::string& message() 
	{ 
		return message_; 
	}

	/** return string with location info where exception is thrown
	 */
	const std::string& location() 
	{ 
		return location_; 
	}

private:

	std::string message_;
	std::string location_;
};



}

}

/** a nice wrapper around util::Exception to throw an exception.
 *  @sa lass::Util::Exception
 *
 *  To throw an exception like util::Exception, you could fill in the message and location yourself,
 *  or ... you could use this macro.  This macro will fill in the location by itself so that you only
 *  have to worry about the message.  And as an extra, the message is streamed, so you can use the regular
 *  @c operator<< to concatenate variables in the message
 *
 *  @code
 *  int foo = -5;
 *  if (foo < 0)
 *  {
 *		LASS_THROW("foo '" << foo << "' is less than zero");
 *  }
 *  @endcode
 */
#define LASS_THROW(iMessage)\
	do\
	{\
		std::ostringstream message;\
		message << iMessage;\
		std::ostringstream location;\
		location << LASS_PRETTY_FUNCTION;\
		throw lass::util::Exception(message.str(), location.str());\
	}\
	while (false)

#endif

// EOF
