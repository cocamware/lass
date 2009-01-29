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



/** @class lass::util::CallbackR$x
 *  @ingroup Callback
 *  @brief callback with $x parameter$x and with returnvalue.
 *  @date 2002
 *  @author Bram de Greve [Bramz] (contact: bram@cocamware.com)
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
#include "shared_ptr.h"
#include "callback_common.h"
#include "impl/dispatcher_r_$x.h"




// --- NEW INTERFACES ----------------------------------------------------------

namespace lass
{
namespace util
{


// THE CALLBACK:
// This is the actuall class you use to hold callbacks:

template
<
	typename R, 
	$(typename P$x)$
>
class CallbackR$x
{
public:

	typedef CallbackR$x<R, $(P$x)$> TSelf;
	typedef SharedPtr< impl::DispatcherR$x<R, $(P$x)$> > TDispatcherPtr;

	// STRUCTORS

	/** Default constructor, construct empty callback.
	 */
	CallbackR$x()
	{
	}

	/** Construct function callback
	 */
	template <typename FunctionType>
	CallbackR$x(FunctionType iFunction):
		dispatcher_(new impl::DispatcherR$xFunction<R, $(P$x)$, FunctionType>(iFunction))
	{
	}

	/** Construct object/method callback.
	 */
	template <typename ObjectPtr, typename Method>
	CallbackR$x(ObjectPtr iObject, Method iMethod):
		dispatcher_(new impl::DispatcherR$xMethod<R, $(P$x)$, ObjectPtr, Method>(iObject, iMethod))
	{
	}

	/** copy constructor
	 */
	CallbackR$x(const TSelf& iOther):
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
	R operator()($(typename util::CallTraits<P$x>::TParam iP$x)$) const
	{
		if (isEmpty())
		{
			LASS_THROW_EX(EmptyCallback, "You've tried to call an empty CallbackR$x.  Can't return a value.");
		}
		return dispatcher_->call($(iP$x)$);
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



/** make a CallbackR$x from a function
 *  @relates CallbackR$x
 */
template <typename R, $(typename P$x)$> inline
CallbackR$x<R, $(P$x)$> makeCallback(R (*iFunction)($(P$x)$))
{
	return CallbackR$x<R, $(P$x)$>(iFunction);
}



/** make a CallbackR$x from a function
 *  @relates CallbackR$x
 */
template <typename R, $(typename P$x)$> inline
const CallbackR$x<R, $(P$x)$>& makeCallback(const CallbackR$x<R, $(P$x)$>& iCallback)
{
	return iCallback;
}



/** make a CallbackR$x from a object and method
 *  @relates CallbackR$x
 */
template <typename ObjectPtr, typename Object, typename R, $(typename P$x)$> inline
CallbackR$x<R, $(P$x)$> makeCallback(ObjectPtr iObject, R (Object::*iMethod)($(P$x)$))
{
	return CallbackR$x<R, $(P$x)$>(iObject, iMethod);
}



/** make a CallbackR$x from a object and const method
 *  @relates CallbackR$x
 */
template <typename ObjectPtr, typename Object, typename R, $(typename P$x)$> inline
CallbackR$x<R, $(P$x)$> makeCallback(ObjectPtr iObject, R (Object::*iMethod)($(P$x)$) const)
{
	return CallbackR$x<R, $(P$x)$>(iObject, iMethod);
}



}

}

#define LASS_PRIM_HAVE_PY_EXPORT_TRAITS_CALLBACK_R$x
#ifdef LASS_GUARDIAN_OF_INCLUSION_UTIL_CALLBACK_PYTHON_H
#	include "callback_python.h"
#endif

#endif // Guardian of Inclusion

// EOF
