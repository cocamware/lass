/** @file
 *  @author Bram de Greve (bramz@users.sourceforge.net)
 *  @author Tom De Muer (tomdemuer@users.sourceforge.net)
 *
 *  Distributed under the terms of the GPL (GNU Public License)
 *
 *  The LASS License:
 *
 *  Copyright 2004 Bram de Greve and Tom De Muer
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
#include "impl/dispatcher_r_0.h"




// --- NEW INTERFACES ----------------------------------------------------------

namespace lass
{
namespace util
{


// THE CALLBACK:
// This is the actuall class you use to hold callbacks:

template<typename R>
class CallbackR0
{
public:

	typedef SharedPtr< impl::DispatcherR0<R> > TDispatcher;

	// STRUCTORS

	/** Default constructor, construct empty callback.
	 */
	CallbackR0()
	{
	}

	/** Construct function callback
	 */
	template<typename S>
	explicit CallbackR0(S (*iFunction)())
	{
		if (iFunction)
		{
			TDispatcher temp(new impl::DispatcherR0Function<R, S>(iFunction));
			dispatcher_ = temp;
		}
	}

	/** Construct object/method callback.
	 */
	template<class Object, typename S>
	CallbackR0(Object* iObject, S (Object::*iMethod)())
	{
		if (iObject && iMethod)
		{
			TDispatcher temp(new impl::DispatcherR0Method<Object, R, S>(iObject, iMethod));
			dispatcher_ = temp;
		}
	}

	/** Construct object/method callback with const method.
	 */
	template<class Object, typename S>
	CallbackR0(Object* iObject, S (Object::*iMethod)() const)
	{
		if (iObject && iMethod)
		{
			TDispatcher temp(new impl::DispatcherR0ConstMethod<Object, R, S>(iObject, iMethod));
			dispatcher_ = temp;
		}
	}


	/** @internal
	 */
	CallbackR0(const TDispatcher& iDispatcher):
		dispatcher_(iDispatcher)
	{
	}


	// OPERATORS

	/** THE operator.  Executes the callback.
	 */
	R operator()() const
	{
		if (isEmpty())
		{
			LASS_THROW("You've tried to call an empty CallbackR0.  Can't return a value.");
		}
		return (*dispatcher_)();
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

	/** Swaps the dispatcher of this callback with the dispatcher of another.
	 */
	void swap(CallbackR0<R>& iOther)
	{
		dispatcher_.swap(iOther.dispatcher_);
	}

	/** return true if two callbacks call the same function/method,
	 *  NEEDS RTTI!
	 */
	bool operator==(const CallbackR0<R>& iOther) const
	{
		return dispatcher_->isEquivalent(iOther.dispatcher_.get());
	}

private:

	TDispatcher dispatcher_;
};



/** return true if two callbacks are different
 *  @relates CallbackR0
 */
template <typename R>
bool operator!=(const CallbackR0<R>& iA, const CallbackR0<R>& iB)
{
	return !(iA == iB);
}



}

}

#define LASS_UTIL_CALLBACK_PYTHON_R0
#ifdef LASS_GUARDIAN_OF_INCLUSION_UTIL_CALLBACK_PYTHON_H
#	include "callback_python.h"
#endif

#endif // Guardian of Inclusion

// EOF
