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

#define LASS_UTIL_EXCEPTION_PRIVATE_IMPL(t_exception)\
		virtual void doThrowSelf() const { throw *this; }\
		virtual ::std::auto_ptr< ::lass::util::experimental::RemoteExceptionBase > doClone() const \
		{ \
			return ::std::auto_ptr< ::lass::util::experimental::RemoteExceptionBase >(new t_exception(*this)); \
		}\
	/**/

namespace lass
{
namespace util
{
namespace experimental
{
	class RemoteExceptionBase
	{
	public:
		virtual ~RemoteExceptionBase() {}
		void throwSelf() const { doThrowSelf(); }
		std::auto_ptr<RemoteExceptionBase> clone() const
		{
			std::auto_ptr<RemoteExceptionBase> copy = doClone();
			if (typeid(*copy) != typeid(*this))
			{
				std::cerr << "[LASS RUN MSG] UNDEFINED BEHAVIOUR WARNING: Cloned exception has been sliced from '"
					<< typeid(*this).name() << "' to '" << typeid(*copy).name() << "'." << std::endl;
			}
			return copy;
		}
	private:
		virtual void doThrowSelf() const = 0;
		virtual std::auto_ptr<RemoteExceptionBase> doClone() const = 0;
	};

	template <typename LocalException>
	class RemoteExceptionWrapper:
		public LocalException,
		public RemoteExceptionBase
	{
	public:
		RemoteExceptionWrapper(const LocalException& e): LocalException(e) {}
		~RemoteExceptionWrapper() throw() {}
	private:
		void doThrowSelf() const { throw *this; }
		std::auto_ptr<RemoteExceptionBase> doClone() const 
		{ 
			return std::auto_ptr<RemoteExceptionBase>(new RemoteExceptionWrapper<LocalException>(*this)); 
		}
	};
}



class Exception: 
	public std::exception,
	public experimental::RemoteExceptionBase
{
public:

	Exception(const std::string& message, const std::string& location):
		std::exception(),
		message_(message),
		location_(location)
	{
	}

	explicit Exception(const std::string& message):
		std::exception(),
		message_(message),
		location_("no location")
	{
	}

	Exception():
		std::exception(),
		message_("no message"),
		location_("no location")
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
	const std::string& message() const
	{
		return message_;
	}

	/** return string with location info where exception is thrown
	 */
	const std::string& location() const
	{
		return location_;
	}


private:

	std::string message_;
	std::string location_;

	LASS_UTIL_EXCEPTION_PRIVATE_IMPL(Exception)
};

}

}

/** @relates lass::util::Exception
 *  
 *  To throw an exception like util::Exception that accepts a message and a location string, you 
 *  can supply those strings yourself, ore you can use this macro for your convenience.
 *
 *  This macro will fill in the location by itself so that you only have to worry about the message.  
 *  And as an extra, the message is streamed, so you can use the regular @c operator<< to 
 *  concatenate variables in the message.
 *
 *  @code
 *  int foo = -5;
 *  if (foo < 0)
 *  {
 *      LASS_THROW_EX(util::Exception, "foo '" << foo << "' is less than zero");
 *  }
 *  @endcode
 */
#define LASS_THROW_EX(t_exception, s_message)\
	do\
	{\
		::std::ostringstream lassUtilExceptionImplBuffer;\
		lassUtilExceptionImplBuffer << s_message;\
		throw t_exception(lassUtilExceptionImplBuffer.str(), LASS_PRETTY_FUNCTION);\
	}\
	while (false)



/** LASS_THROW_EX for ::lass::util::Exception
 *  @relates lass::Util::Exception
 *
 *  LASS_THROW(something) is identical to LASS_THROW_EX(util::Exception, something)
 */
#define LASS_THROW(s_message) LASS_THROW_EX(::lass::util::Exception, s_message)



/** @relates lass::Util::Exception
 *
 *  catches all exceptions and prints an undefined behaviour warning to std::cerr
 */
#define LASS_CATCH_TO_WARNING\
	catch (const ::std::exception& error)\
	{\
		std::cerr << "[LASS RUN MSG] UNDEFINED BEHAVIOUR WARNING: Exception caught in "\
			<< LASS_PRETTY_FUNCTION << ":\n" << error.what() << std::endl;\
	}\
	catch (...)\
	{\
		std::cerr << "[LASS RUN MSG] UNDEFINED BEHAVIOUR WARNING: Unknown exception caught in "\
			<< LASS_PRETTY_FUNCTION << std::endl;\
	}\
	/**/

#endif

// EOF
