/**	@file
 *	@author Bram de Greve (bramz@users.sourceforge.net)
 *	@author Tom De Muer (tomdemuer@users.sourceforge.net)
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
 *	Copyright (C) 2004-2007 the Initial Developer.
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

/** @class lass::util::Callback0
 *  @ingroup Callback
 *  @brief callback with 0 parameter(s) and without returnvalue.
 *  @date 2002
 *  @author Bram de Greve [Bramz] (contact: bramz@users.sourceforge.net)
 *
 *  It's a single object that can hold a reference to a free function or an
 *  object/(const) method pair.  Once the callback is constructed, it works
 *  completely transparent.  All it shows to the client is a function that
 *  takes one parameter but gives no returnvalue.
 */



#ifndef LASS_GUARDIAN_OF_INCLUSION_UTIL_CALLBACK_0_H
#define LASS_GUARDIAN_OF_INCLUSION_UTIL_CALLBACK_0_H



// --- OLD INTERFACES ----------------------------------------------------------

#include "util_common.h"
#include "callback_common.h"
#include "shared_ptr.h"
#include "impl/dispatcher_0.h"
#include "../meta/is_derived.h"
#include "../meta/wrap.h"



// --- NEW INTERFACES ----------------------------------------------------------

namespace lass
{
namespace util
{

// THE CALLBACK:
// This is the actuall class you use to hold callbacks:

class Callback0
{
public:

	typedef Callback0 TSelf;
	typedef SharedPtr<impl::Dispatcher0> TDispatcherPtr;

	// STRUCTORS

	/** Default constructor, construct empty callback.
	 */
	Callback0()
	{
	}

	/** callback to function or other callable entity.
	 */
	template <typename Function>
	Callback0(Function iFunction):
		dispatcher_(make(iFunction, meta::Wrap<typename meta::IsDerived<Function, impl::Dispatcher0>::Type>()))
	{
	}

	/** callback to method of object
	 */
	template <typename ObjectPtr, typename Method>
	Callback0(ObjectPtr iObject, Method iMethod):
		dispatcher_(new impl::Dispatcher0Method<ObjectPtr, Method>(iObject, iMethod))
	{
	}

	/** copy constructor
	 */
	Callback0(const Callback0& iOther):
		dispatcher_(iOther.dispatcher_)
	{
	}



	// OPERATORS

	/** assignment operator (also usable for conversion)
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
	void operator()() const
	{
		if (!isEmpty())
		{
			dispatcher_->call();
		}
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

	/** return true if two callbacks call the same function/method,
	 *  NEEDS RTTI!
	 */
	bool operator==(const TSelf& iOther) const
	{
		return dispatcher_->isEquivalent(iOther.dispatcher_.get());
	}

private:

	template <typename Function>
	static TDispatcherPtr make(Function iFunction, meta::Wrap<meta::False>)
	{
		return TDispatcherPtr(new impl::Dispatcher0Function<Function>(iFunction));
	}

	template <typename Dispatcher>
	static TDispatcherPtr make(Dispatcher iDispatcher, meta::Wrap<meta::True>)
	{
		return TDispatcherPtr(new Dispatcher(iDispatcher));
	}

	TDispatcherPtr dispatcher_;
};



/** return true if two callbacks are different
 *  @relates Callback0
 */
inline bool operator!=(const Callback0& iA, const Callback0& iB)
{
	return !(iA == iB);
}



/** make a Callback0 from a function
 *  @relates Callback0
 */
inline Callback0 makeCallback(void (*iFunction)())
{
	return Callback0(iFunction);
}



/** convencie function, make callback from callback
 */
inline const Callback0& makeCallback(const Callback0& iCallback)
{
	return iCallback;
}


/** make a Callback0 from a object and method
 *  @relates Callback0
 */
template <typename ObjectPtr, typename Object> inline
Callback0 makeCallback(ObjectPtr iObject, void (Object::*iMethod)())
{
	return Callback0(iObject, iMethod);
}



/** make a Callback0 from a object and const method
 *  @relates Callback0
 */
template <typename ObjectPtr, typename Object> inline
Callback0 makeCallback(ObjectPtr iObject, void (Object::*iMethod)() const)
{
	return Callback0(iObject, iMethod);
}



}

}

#include "callback_python.inl"

#endif

// EOF
