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
 *
 *  the value iObj is taken and evaluated by using the operator!.  
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
struct DefaultPredicate
{
    template <typename T> 
    static bool LASS_CALL wrong(const T& iObj) 
	{ 
		return !iObj; 
	}
};



/** Predicate to enforce a stream to be a in good state.
 *  @author Bram de Greve [BdG].
 *
 *  Takes a pointer to a stream, and checks if that stream is in a good state (not eof,
 *  not failed, not bad).  If not, the enforcer will raise.
 */
struct StreamPredicate
{
    template <typename T> 
    static bool LASS_CALL wrong(const T& iStream) 
	{ 
		return iStream.fail(); 
	}
};



/** Predicate for enforcers for handles, used by LASS_ENFORCE_HANDLE.
 *  @author Bram de Greve [BdG].
 *
 *  checks iObj against -1.  Handles are assumed to return -1 as error code.  If a handle equals
 *  -1, then something is wrong and the predicate will return true (the enforce will raise).
 *  For any other value of the handle, the enforcer will not raise and the program will continue.
 * 
 *  @pre type T must be comparable (operator==) to a integer constant.
 */
struct HandlePredicate
{
	template <typename T>
	static bool LASS_CALL wrong(const T& iHandle)
	{
		return iHandle == -1;
	}
};



/** Predicate for index checking, used by LASS_ENFORCE_INDEX
 *  @author Bram de Greve[BdG]
 *  
 *  This predicate checks if an value iIndex fits in the half-open range [0, size), determined
 *  by the template parameter.  It's ideal for checking valid indices in arrays etc., hence the
 *  name.  It returns true if the index is out of the range, so the enforcer will raise.
 *
 *  @pre type T must be comparable with operator< against 0 and size_t.
 */
template <size_t size>
struct IndexPredicate
{
    template <typename T>
    static bool LASS_CALL wrong(const T& iIndex)
    {
        return iIndex < 0 || !(iIndex < size);
    }
};



/** Predicate for calls to COM interfaces, used by LASS_ENFORCE_COM
 *  @author Bram de Greve[BdG]
 *  
 *  This predicate checks if the HRESULT return value of a COM call is non-negative (0 or more).
 *  It returns true if the return value is negative, so the enforcer will raise.
 */
struct ComPredicate
{
    template <typename T>
    static bool LASS_CALL wrong(const T& iHResult)
    {
        return iHResult < 0;
    }
};



// --- Raisers -------------------------------------------------------------------------------------

/** Throw a runtime error
 *
 *  taken from:
 *  ALEXANDRESCU A. & MARGINEAN P. (2003), Enforcements. June 2003, C++ Experts Forum,
 *  http://www.cuj.com.
 *
 *  http://www.cuj.com/documents/s=8250/cujcexp2106alexandr
 */
struct DefaultRaiser
{
    template <typename T> 
	static void raise(const T&, const std::string& iMessage, const char* iLocus)
    {
        if (iMessage.empty())
        {
            LASS_THROW(iLocus);
        }
        else
        {
            LASS_THROW(iLocus << ":\n" << iMessage); 
        }
    }
};



/** Throw a range error for LASS_ENFORCE_INDEX.
 *  @author Bram de Greve.
 *  @pre type T must be streamable to a std::ostream.
 */
template <size_t size>
struct IndexRaiser
{
    template <typename T> 
    static void raise(const T& iIndex, const std::string& iMessage,
                      const char* iLocus)
    {
        std::ostringstream locus;
        locus << "Value '" << iIndex << "' out of range [0, " << size << ") in '" << iLocus << "'";

        if (iMessage.empty())
        {
            LASS_THROW(locus.str());
        }
        else
        {
            LASS_THROW(locus.str() << ":\n" << iMessage); 
        }
    }
};



/** Throws an run time exception for raising LASS_ENFORCE_COM
 *  @author Bram de Greve.
 */
struct ComRaiser
{
    template <typename T> 
    static void raise(const T& iHResult, const std::string& iMessage, const char* iLocus)
    {
        std::ostringstream locus;
        locus << "Failure HRESULT '" << iHResult << "' returned by " << iLocus;

        if (iMessage.empty())
        {
            LASS_THROW(locus.str());
        }
        else
        {
            LASS_THROW(locus.str() << ":\n" << iMessage); 
        }
    }
};



// --- Other helper stuff --------------------------------------------------------------------------

/** Helper class of the enforce macro's.
 *
 *  taken from:
 *  ALEXANDRESCU A. & MARGINEAN P. (2003), Enforcements. June 2003, C++ Experts Forum,
 *  http://www.cuj.com.
 *
 *  http://www.cuj.com/documents/s=8250/cujcexp2106alexandr
 */
template
<
    typename Ref, 
    typename PredicateType, 
    typename RaiserType
>
class Enforcer
{
public:

    Enforcer(Ref iT, const char* iLocus): 
        t_(iT), 
        locus_(PredicateType::wrong(iT) ? iLocus : 0)
    {
    }

    Ref operator*() const
    {
        if (locus_) 
        {
            RaiserType::raise(t_, msg_, locus_);
        }
        return t_;
    }

    template <class MsgType>
    Enforcer& operator()(const MsgType& iMsg)
    {
        if (locus_) 
        {
            // Here we have time; no need to be super-efficient
            std::ostringstream ss;
            ss << iMsg;
            msg_ += ss.str();
        }
        return *this;
    }

private:

    Ref t_;
    std::string msg_;
    const char* const locus_;
};



/** helper function to create enforcers
 *  @relates lass::util::impl::Enforcer
 *
 *  taken from:
 *  ALEXANDRESCU A. & MARGINEAN P. (2003), Enforcements. June 2003, C++ Experts Forum,
 *  http://www.cuj.com.
 *
 *  http://www.cuj.com/documents/s=8250/cujcexp2106alexandr
 */
template <class P, class R, typename T>
inline Enforcer<T&, P, R> makeEnforcer(T& iT, const char* iLocus)
{
    return Enforcer<T&, P, R>(iT, iLocus);
}



/** helper function to create enforcers
 *  @relates lass::util::impl::Enforcer
 *
 *  taken from:
 *  ALEXANDRESCU A. & MARGINEAN P. (2003), Enforcements. June 2003, C++ Experts Forum,
 *  http://www.cuj.com.
 *
 *  http://www.cuj.com/documents/s=8250/cujcexp2106alexandr
 */
template <class P, class R, typename T>
inline Enforcer<const T&, P, R> makeEnforcer(const T& iT, const char* iLocus)
{
    return Enforcer<const T&, P, R>(iT, iLocus);
}



}

}

}

#endif

// EOF
