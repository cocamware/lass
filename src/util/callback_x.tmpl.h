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
#include "shared_ptr.h"
#include "impl/dispatcher_$x.h"
#include "callback_common.h"




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
	Callback$x(FunctionType iFunction):
		dispatcher_(new impl::Dispatcher$xFunction<$(P$x)$, FunctionType>(iFunction))
	{
	}

	/** Construct object/method callback.
	 */
	template <typename ObjectPtr, typename Method>
	Callback$x(ObjectPtr iObject, Method iMethod):
		dispatcher_(new impl::Dispatcher$xMethod<$(P$x)$, ObjectPtr, Method>(iObject, iMethod))
	{
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

#define LASS_PRIM_HAVE_PY_EXPORT_TRAITS_CALLBACK_$x
#ifdef LASS_GUARDIAN_OF_INCLUSION_UTIL_CALLBACK_PYTHON_H
#	include "callback_python.h"
#endif

#endif // Guardian of Inclusion

// EOF
