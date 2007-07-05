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



/** @class lass::util::CallbackR0
 *  @ingroup Callback
 *  @brief callback with 0 parameters but with returnvalue.
 *  @date 2002
 *  @author Bram de Greve [Bramz] (contact: bramz@users.sourceforge.net)
 *
 *  It's a single object that can hold a reference to a free function or an
 *  object/(const) method pair.  Once the callback is constructed, it works
 *  completely transparent.  All it shows to the client is a function that
 *  takes 0 parameter(s) and gives a returnvalue.
 */



#ifndef LASS_GUARDIAN_OF_INCLUSION_UTIL_CALLBACK_R_0_H
#define LASS_GUARDIAN_OF_INCLUSION_UTIL_CALLBACK_R_0_H



// --- OLD INTERFACES ----------------------------------------------------------

#include "util_common.h"
#include "shared_ptr.h"
#include "callback_common.h"
#include "impl/dispatcher_r_0.h"




// --- NEW INTERFACES ----------------------------------------------------------

namespace lass
{
namespace util
{


// THE CALLBACK:
// This is the actuall class you use to hold callbacks:

template
<
	typename R
>
class CallbackR0
{
public:

	typedef CallbackR0<R> TSelf;
	typedef SharedPtr< impl::DispatcherR0<R> > TDispatcherPtr;

	// STRUCTORS

	/** Default constructor, construct empty callback.
	 */
	CallbackR0()
	{
	}

	/** Construct function callback
	 */
	template <typename Function>
	CallbackR0(Function iFunction):
		dispatcher_(make(iFunction, meta::Type2Type<typename meta::IsDerivedType<Function, impl::DispatcherR0<R> >::Type>()))
	{
	}

	/** Construct object/method callback.
	 */
	template <typename ObjectPtr, typename Method>
	CallbackR0(ObjectPtr iObject, Method iMethod):
		dispatcher_(new impl::DispatcherR0Method<R, ObjectPtr, Method>(iObject, iMethod))
	{
	}

	/** copy constructor
	 */
	CallbackR0(const TSelf& iOther):
		dispatcher_(iOther.dispatcher_)
	{
	}



	// OPERATORS

	/** assignment operator
	 */
	template <typename Other>
	TSelf& operator=(const Other& iOther)
	{
		TSelf temp(iOther);
		swap(temp);
		return *this;
	}

	/** THE operator.  Executes the callback.
	 */
	R operator()() const
	{
		if (isEmpty())
		{
			LASS_THROW_EX(EmptyCallback, "You've tried to call an empty CallbackR0.  Can't return a value.");
		}
		return dispatcher_->call();
	}


	// METHODS

	/** Reset to empty callback.
	 */
	void reset()
	{
		dispatcher_.reset();
	}

	/** Returns true if no callback dispatcher is assigned to this object.
	 */
	bool isEmpty() const
	{
		return dispatcher_.isEmpty();
	}

	/** return this->isEmpty()
	 */
	bool operator!() const
	{
		return dispatcher_.isEmpty(); 
	}

	/** return !this->isEmpty())
	 */
	operator num::SafeBool() const
	{
		return dispatcher_.isEmpty() ? num::safeFalse : num::safeTrue;
	}

	/** Swaps the dispatcher of this callback with the dispatcher of another.
	 */
	void swap(TSelf& iOther)
	{
		dispatcher_.swap(iOther.dispatcher_);
	}

private:

	template <typename Function>
	static TDispatcherPtr make(Function iFunction, meta::Type2Type<meta::False>)
	{
		return TDispatcherPtr(new impl::DispatcherR0Function<R, Function>(iFunction));
	}

	template <typename Dispatcher>
	static TDispatcherPtr make(Dispatcher iDispatcher, meta::Type2Type<meta::True>)
	{
		return TDispatcherPtr(new Dispatcher(iDispatcher));
	}

	TDispatcherPtr dispatcher_;
};



/** return true if two callbacks are different
 *  @relates CallbackR0
 */
template <typename R>
bool operator!=(const CallbackR0<R>& iA, const CallbackR0<R>& iB)
{
	return !(iA == iB);
}



/** make a CallbackR0 from a function
 *  @relates CallbackR0
 */
template <typename R> inline
CallbackR0<R> makeCallback(R (*iFunction)())
{
	return Callback0(iFunction);
}



/** make a CallbackR0 from a callback
 *  @relates CallbackR0
 */
template <typename R> inline
const CallbackR0<R>& makeCallback(const CallbackR0<R>& iCallback)
{
	return iCallback;
}



/** make a CallbackR0 from a object and method
 *  @relates CallbackR0
 */
template <typename ObjectPtr, typename Object, typename R> inline
CallbackR0<R> makeCallback(ObjectPtr iObject, R (Object::*iMethod)())
{
	return Callback0(iObject, iMethod);
}



/** make a CallbackR0 from a object and const method
 *  @relates CallbackR0
 */
template <typename ObjectPtr, typename Object, typename R> inline
CallbackR0<R> makeCallback(ObjectPtr iObject, R (Object::*iMethod)() const)
{
	return Callback0(iObject, iMethod);
}



}

}

#include "callback_python.inl"

#endif // Guardian of Inclusion

// EOF
