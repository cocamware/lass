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

class LASS_DLL RemoteExceptionBase
{
public:
	virtual ~RemoteExceptionBase();
	void throwSelf() const;
	std::auto_ptr<RemoteExceptionBase> clone() const;
private:
	virtual void doThrowSelf() const = 0;
	virtual std::auto_ptr<RemoteExceptionBase> doClone() const = 0;
};



class LASS_DLL Exception: public virtual std::exception, public virtual RemoteExceptionBase
{
public:
	explicit Exception(const std::string& message = "no message", const std::string& location = "no location");
	~Exception() throw();
	const char* what() const throw();
	const std::string& message() const;
	const std::string& location() const;
private:
	std::string message_;
	std::string location_;
	void doThrowSelf() const;
	::std::auto_ptr<RemoteExceptionBase> doClone() const;
};



template <typename ExceptionType, typename ParentType = Exception>
class ExceptionMixin: public virtual ParentType
{
public:
	ExceptionMixin(const std::string& msg, const std::string& loc): ParentType(msg, loc) {}
private:
	virtual void doThrowSelf() const 
	{
		throw *static_cast<const ExceptionType*>(this); 
	}
	virtual ::std::auto_ptr<RemoteExceptionBase> doClone() const
	{ 
		return ::std::auto_ptr<RemoteExceptionBase>(new ExceptionType(*static_cast<const ExceptionType*>(this)));
	}
};



template <typename LocalException>
class RemoteExceptionWrapper:
	public virtual LocalException,
	public virtual RemoteExceptionBase
{
public:
	RemoteExceptionWrapper(const LocalException& e): LocalException(e) {}
	~RemoteExceptionWrapper() throw() {}
private:
	void doThrowSelf() const 
	{ 
		throw *this; 
	}
	std::auto_ptr<RemoteExceptionBase> doClone() const 
	{ 
		return std::auto_ptr<RemoteExceptionBase>(new RemoteExceptionWrapper<LocalException>(*this)); 
	}
};



class KeyError: public ExceptionMixin<KeyError>
{
public:
	KeyError(const std::string& msg, const std::string& loc): ExceptionMixin<KeyError>(msg, loc) {}
};


class ValueError: public ExceptionMixin<ValueError>
{
public:
	ValueError(const std::string& msg, const std::string& loc): ExceptionMixin<ValueError>(msg, loc) {}
};

class SingularityError: public ExceptionMixin<SingularityError>
{
public:
	SingularityError(const std::string& msg, const std::string& loc): ExceptionMixin<SingularityError>(msg, loc) {}
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
