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



/** @class lass::util::Callback$x
 *  @ingroup Callback
 *  @brief callback with $x parameter(s) but without returnvalue.
 *  @date 2002
 *  @author Bram de Greve [Bramz] (contact: bramz@users.sourceforge.net)
 *
 *  It's a single object that can hold a reference to a free function or an
 *  object/(const) method pair.  Once the callback is constructed, it works
 *  completely transparent.  All it shows to the client is a function that
 *  takes one parameter but gives no returnvalue.
 */



#ifndef LASS_GUARDIAN_OF_INCLUSION_UTIL_CALLBACK_$x_H
#define LASS_GUARDIAN_OF_INCLUSION_UTIL_CALLBACK_$x_H



// --- OLD INTERFACES ----------------------------------------------------------

#include "util_common.h"
#include "impl/dispatcher_$x.h"




// --- NEW INTERFACES ----------------------------------------------------------

namespace lass
{
namespace util
{


// THE CALLBACK:
// This is the actuall class you use to hold callbacks:

template
<
	$(typename P$x)$
>
class Callback$x
{
public:

	typedef Callback$x<$(P$x)$> TSelf;
	typedef SharedPtr< impl::Dispatcher$x<$(P$x)$> > TDispatcherPtr;

	// STRUCTORS

	/** Default constructor, construct empty callback.
	 */
	Callback$x()
	{
	}

	/** Construct function callback
	 */
	template <typename FunctionType>
	Callback$x(FunctionType iFunction)
	{
		dispatcher_ = TDispatcherPtr(new impl::Dispatcher$xFunction<$(P$x)$, FunctionType>(iFunction));
	}

	/** Construct object/method callback.
	 */
	template <typename ObjectPtr, typename Method>
	Callback$x(ObjectPtr iObject, Method iMethod)
	{
		dispatcher_ = TDispatcherPtr(new impl::Dispatcher$xMethod<$(P$x)$, ObjectPtr, Method>(iObject, iMethod));
	}

	/** copy constructor
	 */
	Callback$x(const TSelf& iOther):
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
	void operator()($(typename util::CallTraits<P$x>::TParam iP$x)$) const
	{
		if (!isEmpty())
		{
			dispatcher_->call($(iP$x)$);
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

private:

	TDispatcherPtr dispatcher_;
};



/** make a Callback$x from a function
 *  @relates Callback$x
 */
template <$(typename P$x)$> inline
Callback$x<$(P$x)$> makeCallback(void (*iFunction)($(P$x)$))
{
	return Callback$x<$(P$x)$>(iFunction);
}



/** make a Callback$x from a callback
 *  @relates Callback$x
 */
template <$(typename P$x)$> inline
const Callback$x<$(P$x)$>& makeCallback(const Callback$x<$(P$x)$>& iCallback)
{
	return iCallback;
}



/** make a Callback$x from a object and method
 *  @relates Callback$x
 */
template <typename ObjectPtr, typename Object, $(typename P$x)$> inline
Callback$x<$(P$x)$> makeCallback(ObjectPtr iObject, void (Object::*iMethod)($(P$x)$))
{
	return Callback$x<$(P$x)$>(iObject, iMethod);
}



/** make a Callback$x from a object and const method
 *  @relates Callback$x
 */
template <typename ObjectPtr, typename Object, $(typename P$x)$> inline
Callback$x<$(P$x)$> makeCallback(ObjectPtr iObject, void (Object::*iMethod)($(P$x)$) const)
{
	return Callback$x<$(P$x)$>(iObject, iMethod);
}



}

}

#define LASS_UTIL_CALLBACK_PYTHON_$x
#ifdef LASS_GUARDIAN_OF_INCLUSION_UTIL_CALLBACK_PYTHON_H
#	include "callback_python.h"
#endif

#endif // Guardian of Inclusion

// EOF
