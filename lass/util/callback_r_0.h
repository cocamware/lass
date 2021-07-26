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
 *	Copyright (C) 2004-2020 the Initial Developer.
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



/** @class lass::util::CallbackR0
 *  @ingroup Callback
 *  @brief callback with 0 parameters but with returnvalue.
 *  @date 2002
 *  @author Bram de Greve [Bramz] (contact: bram@cocamware.com)
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
		dispatcher_(make(iFunction, meta::Wrap<typename meta::IsDerived<Function, impl::DispatcherR0<R> >::Type>()))
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
	TSelf& operator=(const TSelf& iOther)
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
	explicit operator bool() const
	{
		return !dispatcher_.isEmpty();
	}

	/** Swaps the dispatcher of this callback with the dispatcher of another.
	 */
	void swap(TSelf& iOther)
	{
		dispatcher_.swap(iOther.dispatcher_);
	}

	/** return true if two callbacks call the same function/method,
	 *  NEEDS RTTI!
	 */
	bool operator==(const TSelf& iOther) const
	{
		if (dispatcher_ == iOther.dispatcher_)
		{
			return true;
		}
		return dispatcher_ && dispatcher_->isEquivalent(iOther.dispatcher_.get());
	}

private:

	template <typename Function>
	static TDispatcherPtr make(Function iFunction, meta::Wrap<meta::False>)
	{
		return TDispatcherPtr(new impl::DispatcherR0Function<R, Function>(iFunction));
	}

	template <typename Dispatcher>
	static TDispatcherPtr make(Dispatcher iDispatcher, meta::Wrap<meta::True>)
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

#define LASS_PRIM_HAVE_PY_EXPORT_TRAITS_CALLBACK_R0
#ifdef LASS_GUARDIAN_OF_INCLUSION_UTIL_CALLBACK_PYTHON_H
#	include "../python/callback_python.h"
#endif

#endif // Guardian of Inclusion

// EOF
