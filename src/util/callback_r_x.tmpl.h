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
 *
 *  $g
 */



/** @class lass::util::CallbackR$x
 *  @ingroup Callback
 *  @brief callback with $x parameter$x and with returnvalue.
 *  @date 2002
 *  @author Bram de Greve [Bramz] (contact: bramz@users.sourceforge.net)
 *
 *  It's a single object that can hold a reference to a free function or an
 *  object/(const) method pair.  Once the callback is constructed, it works
 *  completely transparent.  All it shows to the client is a function that
 *  takes $x parameter(s) and gives a returnvalue.
 */



#ifndef LASS_GUARDIAN_OF_INCLUSION_UTIL_CALLBACK_R_$x_H
#define LASS_GUARDIAN_OF_INCLUSION_UTIL_CALLBACK_R_$x_H



// --- OLD INTERFACES ----------------------------------------------------------

#include "util_common.h"
#include "impl/dispatcher_r_$x.h"




// --- NEW INTERFACES ----------------------------------------------------------

namespace lass
{
namespace util
{


// THE CALLBACK:
// This is the actuall class you use to hold callbacks:

template<typename R, $(typename P$x)$>
class CallbackR$x
{
public:

	typedef SharedPtr<  impl::DispatcherR$x<R, $(P$x)$> > TDispatcher;

	// STRUCTORS

	/** Default constructor, construct empty callback.
	 */
	CallbackR$x()
	{
	}

	/** Construct function callback
	 */
	template<typename S, $(typename Q$x)$>
	explicit CallbackR$x(S (*iFunction)($(Q$x)$))
	{
		if (iFunction)
		{
			TDispatcher temp(new impl::DispatcherR$xFunction<R, $(P$x)$, S, $(Q$x)$>(iFunction));
			dispatcher_ = temp;
		}
	}

	/** Construct object/method callback.
	 */
	template<class Object, typename S, $(typename Q$x)$>
	CallbackR$x(Object* iObject, S (Object::*iMethod)($(Q$x)$))
	{
		if (iObject && iMethod)
		{
			TDispatcher temp(new impl::DispatcherR$xMethod<Object, R, $(P$x)$, S, $(Q$x)$>(iObject, iMethod));
			dispatcher_ = temp;
		}
	}

	/** Construct object/method callback with const method.
	 */
	template<class Object, typename S, $(typename Q$x)$>
	CallbackR$x(Object* iObject, S (Object::*iMethod)($(Q$x)$) const)
	{
		if (iObject && iMethod)
		{
			TDispatcher temp(new impl::DispatcherR$xConstMethod<Object, R, $(P$x)$, S, $(Q$x)$>(iObject, iMethod));
			dispatcher_ = temp;
		}
	}

	/** @internal
	 */
	CallbackR$x(const TDispatcher& iDispatcher):
		dispatcher_(iDispatcher)
	{
	}


	// OPERATORS

	/** THE operator.  Executes the callback.
	 */
	R operator()($(P$x iP$x)$) const
	{
		if (isEmpty())
		{
			LASS_THROW("You've tried to call an empty CallbackR$x.  Can't return a value.");
		}
		return (*dispatcher_)($(iP$x)$);
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
	void swap(CallbackR$x<R, $(P$x)$>& iOther)
	{
		dispatcher_.swap(iOther.dispatcher_);
	}

private:

	TDispatcher dispatcher_;
};



}

}

#define LASS_UTIL_CALLBACK_PYTHON_R$x
#ifdef LASS_GUARDIAN_OF_INCLUSION_UTIL_CALLBACK_PYTHON_H
#	include "callback_python.h"
#endif

#endif // Guardian of Inclusion

// EOF
