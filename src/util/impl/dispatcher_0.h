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

#ifndef LASS_GUARDIAN_OF_INCLUSION_UTIL_IMPL_DISPATCHER_0_H
#define LASS_GUARDIAN_OF_INCLUSION_UTIL_IMPL_DISPATCHER_0_H

#include "dispatcher_allocator.h"

// --- NEW INTERFACES ----------------------------------------------------------

namespace lass
{
namespace util
{
namespace impl
{

/** abstract base class of all dispatchers for lass::util::Callback0.
 *  @internal
 *  @sa Callback0
 *  @author Bram de Greve [Bramz]
 */
class Dispatcher0: public TDispatcherAllocatorBase
{
public:

	Dispatcher0() {}
	virtual ~Dispatcher0() {}
	void call() const { doCall(); }
	bool isEquivalent(const Dispatcher0* iOther) const { return doIsEquivalent(iOther); }

protected:

	Dispatcher0(const Dispatcher0& iOther) {}

private:

	virtual void doCall() const = 0;
	virtual bool doIsEquivalent(const Dispatcher0* iOther) const = 0;
	
	Dispatcher0& operator=(const Dispatcher0& iOther);
};



/** Dispatcher for lass::util::Callback0 to a fuction or equivalent callable entity.
 *  @internal
 *  @sa Callback0
 *  @author Bram de Greve [Bramz]
 */
template 
<
	typename Function
>
class Dispatcher0Function: public Dispatcher0
{
public:

	typedef Dispatcher0Function<Function> TSelf;
	typedef Function TFunction;

	Dispatcher0Function(typename CallTraits<TFunction>::TParam iFunction):
		function_(iFunction)
	{
	}

private:

	void doCall() const
	{
		function_();
	}

	bool doIsEquivalent(const Dispatcher0* iOther) const
	{
		const TSelf* other = dynamic_cast<const TSelf*>(iOther);
		return other && function_ == other->function_;
	}

	TFunction function_;
};



/** Dispatcher for lass::util::Callback0 to an object/method pair.
 *  @internal
 *  @sa Callback0
 *  @author Bram de Greve [Bramz]
 */
template 
<
	typename ObjectPtr,
	typename Method
>
class Dispatcher0Method: public Dispatcher0
{
public:

	typedef Dispatcher0Method<ObjectPtr, Method> TSelf;
	typedef ObjectPtr TObjectPtr;
	typedef Method TMethod;

	Dispatcher0Method(typename CallTraits<TObjectPtr>::TParam iObject, 
					  typename CallTraits<TMethod>::TParam iMethod):
		object_(iObject),
		method_(iMethod)
	{
	}

private:

	void doCall() const
	{
		((*object_).*method_)();
	}

	bool doIsEquivalent(const Dispatcher0* iOther) const
	{
		const TSelf* other = dynamic_cast<const TSelf*>(iOther);
		return other && object_ == other->object_ && method_ == other->method_;
	}

	TObjectPtr object_;
	TMethod method_;
};



}

}

}

#endif // Guardian of Inclusion


// EOF
