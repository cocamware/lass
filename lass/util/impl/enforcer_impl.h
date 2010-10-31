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


#ifndef LASS_GUARDIAN_OF_INCLUSION_UTIL_IMPL_ENFORCER_IMPL_H
#define LASS_GUARDIAN_OF_INCLUSION_UTIL_IMPL_ENFORCER_IMPL_H

#include "../util_common.h"

namespace lass
{
namespace util
{
namespace impl
{

// Predicates

/** Predicate for enforcers using operator!, used by LASS_ENFORCE and LASS_ENFORCE_POINTER.
 *  @internal
 *
 *  the value @a t is evaluated by using the operator!.
 *  if this returns true, the predicate will return true and the enforcer will raise.
 *  if this is false, the predicate will return false and the program will continue
 *
 *  taken from:
 *  ALEXANDRESCU A. & MARGINEAN P. (2003), Enforcements. June 2003, C++ Experts Forum,
 *  http://www.cuj.com.
 *
 *  http://www.cuj.com/documents/s=8250/cujcexp2106alexandr
 *
 *  @pre type T must support bool operator!(const T&).
 */
struct TruePredicate
{
	template <typename T, typename C>
	static bool LASS_CALL wrong(const T& t, const C& /* closure */)
	{
		return !t;
	}
};



/** value must be equal to closure
 *  @internal
 *
 *  the value @a t is compared to the value of @a closure using operator==.
 *  If they are equal, the predicate returns fals and nothing happens.
 *  Otherwise, the predicate returns true and the enforcer will raise.
 *
 *  @pre T and C must be comparable using operator==(const T&, const C&).
 */
struct EqualPredicate
{
	template <typename T, typename C>
	static bool LASS_CALL wrong(const T& t, const C& closure)
	{
		return !(t == closure);
	}
};



/** value must be different than closure
 *  @internal
 *
 *  the value @a t is compared to the value of @a closure using operator==.
 *  If they are equal, the predicate returns true and the enforcer will raise.
 *  Otherwise, the predicate returns fals and nothing happens.
 *
 *  @pre T and C must be comparable using operator==(const T&, const C&).
 */
struct UnequalPredicate
{
	template <typename T, typename C>
	static bool LASS_CALL wrong(const T& t, const C& closure)
	{
		return t == closure;
	}
};



/** value must be greater than or equal to closure
 *  @internal
 *
 *  the value @a t is compared to the value of @a closure using operator<.
 *  If t is less, the predicate returns true and the enforcer will raise.
 *  Otherwise, the predicate returns false and nothing happens.
 *
 *  @pre T and C must be comparable using operator<(const T&, const C&).
 */
struct GreaterEqualPredicate
{
	template <typename T, typename C>
	static bool LASS_CALL wrong(const T& t, const C& closure)
	{
		return t < closure;
	}
};



/** value must be in range [0, closure)
 *  @internal
 *
 *  If t is out of range, the predicate returns true and the enforcer will raise.
 *  Otherwise, the predicate returns false and nothing happens.
 *
 *  @pre T and C must be comparable using operator<(const T&, const C&).
 */
struct IndexPredicate
{
	template <typename T, typename C>
	static bool LASS_CALL wrong(const T& t, const C& closure)
	{
		return t < C(0) || !(t < closure);
	}
	template <typename C>
	static bool LASS_CALL wrong(size_t t, const C& closure)
	{
		return !(t < closure);
	}
};



/** Predicate to enforce a stream to be a in good state.
 *  @internal
 *  @author Bram de Greve [Bramz].
 *
 *  Takes a pointer to a stream, and checks if that stream is in a good state (not eof,
 *  not failed, not bad).  If not, the enforcer will raise.
 */
struct StreamPredicate
{
	template <typename T, typename C>
	static bool LASS_CALL wrong(const T& stream, const C& /* closure */)
	{
		return stream.fail();
	}
};




#ifdef LASS_HAS_GETLASTERROR

/** Predicate for calls to the Windows API that return 0 on error _and_ have GetLastError() != 0.
 *  @internal
 *  @author Bram de Greve[Bramz]
 *
 *  This predicate checks if the return value of the function.  If it is zero, there possibly is
 *  an error.  It then also checks GetLastError().  If that one differs from zero, an error indeed happened.
 */
struct WinAPIPredicate
{
	template <typename T, typename C>
	static bool LASS_CALL wrong(const T& returnCode, const C& /* closure */)
	{
		return returnCode == 0 && lass_GetLastError() != 0;
	}
};

#endif



// --- Raisers -------------------------------------------------------------------------------------

/** Throw a runtime error
 *  @internal
 *
 *  taken from:
 *  ALEXANDRESCU A. & MARGINEAN P. (2003), Enforcements. June 2003, C++ Experts Forum,
 *  http://www.cuj.com.
 *
 *  http://www.cuj.com/documents/s=8250/cujcexp2106alexandr
 */
struct DefaultRaiser
{
	template <typename T, typename C>
	static void raise(const T&, const C&, const std::string& message, const char* locus)
	{
		if (message.empty())
		{
			LASS_THROW_EX(EnforceFailure, locus);
		}
		else
		{
			LASS_THROW_EX(EnforceFailure, locus << ":\n" << message);
		}
	}
};



inline void raiserAddMessage(std::ostream& stream, const std::string& message)
{
	if (!message.empty())
	{
		stream << ":\n" << message;
	}
}


/** Throws an run time exception for raising LASS_ENFORCE_ZERO
 *  @internal
 *  @author Bram de Greve.
 */
struct ZeroRaiser
{
	template <typename T, typename C>
	static void raise(const T& result, const C&, const std::string& message, const char* locus)
	{
		std::ostringstream buffer;
		buffer << "Expression " << locus << " resulted in a non-zero value '" << result << "'";
		raiserAddMessage(buffer, message);
		LASS_THROW_EX(EnforceFailure, buffer.str());
	}
};



/** Throws a run time exception for raisng LASS_ENFORCE_CLIB.
 *  @internal
 *  @author Bram de Greve
 *  retrieves an error code from errno.
 */
 struct ClibRaiser
 {
 	template <typename T, typename C>
	static void raise(const T&, const C&, const std::string& message, const char* locus)
	{
		//LASS_ASSERT(iRc == -1);
		const int errnum = lass_errno();
		std::ostringstream buffer;
		buffer << "Function call " << locus << " failed with: " << lass_strerror(errnum);			
		raiserAddMessage(buffer, message);
		LASS_THROW_EX(EnforceFailure, buffer.str());
	}
};



/** Prints warning to std::cerr for LASS_WARN_CLIB.
 *  @internal
 *  @author Bram de Greve
 *  retrieves an error code from errno.  rc should be -1.
 */
 struct ClibWarner
 {
 	template <typename T, typename C>
	static void raise(const T&, const C&, const std::string& message, const char* locus)
	{
		const int errnum = lass_errno();
		std::ostringstream buffer;
		std::cerr << "[LASS RUN MSG] UNDEFINED BEHAVIOUR: "
			<< "Function call " << locus << " failed with errno: ("
			<< errnum << ") " << lass_strerror(errnum);			
		raiserAddMessage(std::cerr, message);
		std::cerr << ".\n" << std::flush;
	}
};



/** Throws a run time exception for raisng LASS_ENFORCE_CLIB_RC.
 *  @internal
 *  @author Bram de Greve
 *  iRc contains error code and should not be zero.
 */
 struct ClibRcRaiser
 {
 	template <typename T, typename C>
	static void raise(const T& rc, const C&, const std::string& message, const char* locus)
	{
		//LASS_ASSERT(iRc != 0);
		std::ostringstream buffer;
		buffer << "Function call " << locus << " failed with return code: ("
			<< rc << ") " << lass_strerror(rc);			
		raiserAddMessage(buffer, message);
		LASS_THROW_EX(EnforceFailure, buffer.str());
	}
};



/** Throws a run time exception for raisng LASS_ENFORCE_CLIB_RC.
 *  @internal
 *  @author Bram de Greve
 *  iRc contains error code and should not be zero.
 */
 struct ClibRcWarner
 {
 	template <typename T, typename C>
	static void raise(const T& rc, const C&, const std::string& message, const char* locus)
	{
		std::cerr << "[LASS RUN MSG] UNDEFINED BEHAVIOUR: "
			<< "Function call " << locus << " failed with return code: ("
			<< rc << ") " << lass_strerror(rc);			
		raiserAddMessage(std::cerr, message);
		std::cerr << std::endl << std::flush;
	}
};



/** Throws an run time exception for raising LASS_ENFORCE_COM
 *  @internal
 *  @author Bram de Greve.
 */
struct ComRaiser
{
	template <typename T, typename C>
	static void raise(const T& hResult, const C&, const std::string& message, const char* locus)
	{
		std::ostringstream buffer;
		buffer << "Failure HRESULT '" << hResult << "' returned by " << locus;
		raiserAddMessage(buffer, message);
		LASS_THROW_EX(EnforceFailure, buffer.str());
	}
};



#ifdef LASS_HAS_GETLASTERROR

/** Throws an exception using GetLastError() and FormatMessage().
 *  @internal
 *  @author Bram de Greve
 *  @arg only available on Windows platform
 */
struct LastErrorRaiser
{
 	template <typename T, typename C>
	static void raise(const T&, const C&, const std::string& message, const char* locus)
	{
		std::ostringstream buffer;
		const unsigned lastError = lass_GetLastError();
		buffer << "Function call " << locus << " failed with last-error: ("
			<< lastError << ") " << lass_FormatMessage(lastError);
		raiserAddMessage(buffer, message);
		LASS_THROW_EX(EnforceFailure, buffer.str());
	}
};

/** Writes warning to std::cerr using GetLastError() and FormatMessage().
 *  @internal
 *  @author Bram de Greve
 *  @arg only available on Windows platform
 */
struct LastErrorWarner
{
 	template <typename T, typename C>
	static void raise(const T&, const C&, const std::string& message, const char* locus)
	{
		const unsigned lastError = lass_GetLastError();
		std::cerr << "Function call " << locus << " failed with last-error: ("
			<< lastError << ") " << lass_FormatMessage(lastError);
		raiserAddMessage(std::cerr, message);
		std::cerr << std::endl << std::flush;
	}
};

#endif


/** Throw a range error for LASS_ENFORCE_INDEX.
 *  @internal
 *  @author Bram de Greve.
 *  @pre type T must be streamable to a std::ostream.
 */
struct IndexRaiser
{
	template <typename T, typename C>
	static void raise(const T& index, const C& size, const std::string& message, const char* locus)
	{
		std::ostringstream buffer;
		buffer << "Value '" << index << "' is out of range [0, " << size << ") in '"
			<< locus << "'";
		raiserAddMessage(buffer, message);
		LASS_THROW_EX(EnforceFailure, buffer.str());
	}
};



// --- Other helper stuff --------------------------------------------------------------------------

/** Helper class of the enforce macro's.
 *  @internal
 *
 *  taken from:
 *  ALEXANDRESCU A. & MARGINEAN P. (2003), Enforcements. June 2003, C++ Experts Forum,
 *  http://www.cuj.com.
 *
 *  http://www.cuj.com/documents/s=8250/cujcexp2106alexandr
 */
template
<
	typename PredicateType,
	typename RaiserType,
	typename T,
	typename ClosureType
>
class Enforcer
{
public:
	Enforcer(T t, ClosureType closure, const char* locus):
		t_(t),
		closure_(closure),
		locus_(PredicateType::wrong(t, closure) ? locus : 0)
	{
		if (!locus)
		{
			std::cerr << "[LASS RUN MSG] UNDEFINED BEHAVIOUR WARNING: "
				"Enforcer did not get a locus!\n" << std::flush;
		}
	}

	T operator*() const
	{
		if (locus_)
		{
			RaiserType::raise(t_, closure_, msg_, locus_);
		}
		return t_;
	}

	template <class MsgType>
	Enforcer& operator()(const MsgType& msg)
	{
		if (locus_)
		{
			// Here we have time; no need to be super-efficient
			std::ostringstream ss;
			ss << msg;
			msg_ += ss.str();
		}
		return *this;
	}

private:

	T t_;
	ClosureType closure_;
	std::string msg_;
	const char* const locus_;
};

#if LASS_COMPILER_TYPE == LASS_COMPILER_TYPE_GCC && LASS_COMPILER_VERSION < 40000

template <typename PredicateType, typename RaiserType>
struct EnforcerMaker
{
	template <typename T, typename C> inline
	static Enforcer<PredicateType, RaiserType, T&, const C&> make(
			T& t, const C& closure, const char* locus)
	{
		return Enforcer<PredicateType, RaiserType, T&, const C&>(
			t, closure, locus);
	}

	template <typename T, typename C> inline
	static Enforcer<PredicateType, RaiserType, const T&, const C&> make(
			const T& t, const C& closure, const char* locus)
	{
		return Enforcer<PredicateType, RaiserType, const T&, const C&>(
			t, closure, locus);
	}
};

#define LASS_UTIL_IMPL_MAKE_ENFORCER(predicate, raiser, t, closure, locus)\
	::lass::util::impl::EnforcerMaker< predicate, raiser >::make(t, closure, locus)

#else

/** helper function to create enforcers
 *  @internal
 *  @relates lass::util::impl::Enforcer
 *
 *  taken from:
 *  ALEXANDRESCU A. & MARGINEAN P. (2003), Enforcements. June 2003, C++ Experts Forum,
 *  http://www.cuj.com.
 *
 *  http://www.cuj.com/documents/s=8250/cujcexp2106alexandr
 */
template <typename PredicateType, typename RaiserType, typename T, typename C>
inline Enforcer<PredicateType, RaiserType, T&, const C&> makeEnforcer(
		T& t, const C& closure, const char* locus)
{
	return Enforcer<PredicateType, RaiserType, T&, const C&>(
		t, closure, locus);
}

template <typename PredicateType, typename RaiserType, typename T, typename C>
inline Enforcer<PredicateType, RaiserType, const T&, const C&> makeEnforcer(
		const T& t, const C& closure, const char* locus)
{
	return Enforcer<PredicateType, RaiserType, const T&, const C&>(
		t, closure, locus);
}

#define LASS_UTIL_IMPL_MAKE_ENFORCER(predicate, raiser, t, closure, locus)\
	::lass::util::impl::makeEnforcer< predicate, raiser >(t, closure, locus)

#endif

}

}

}

#endif

// EOF
