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



/** @class lass::util::CallbackR0
 *  @ingroup Callback
 *  @brief callback with 0 parameters but with returnvalue.
 *  @date 2002
 *  @author Bram de Greve [BdG] (contact: bdegreve@mail.be)
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

    // STRUCTORS
    
    /** Default constructor, construct empty callback.
     */
    CallbackR0(): 
        dispatcher_(0) 
    {
    }

    /** Construct function callback
     */
    template<typename S>
	explicit CallbackR0(S (*iFunction)()):
		dispatcher_(0)
    {
        if (iFunction)
        {
			dispatcher_ = new impl::DispatcherR0Function<R, S>(iFunction);
        }
    }
    
    /** Construct object/method callback.
     */
    template<class Object, typename S>
    CallbackR0(Object* iObject, S (Object::*iMethod)()):
		dispatcher_(0)
    {
        if (iObject && iMethod) 
        {
			dispatcher_ = new impl::DispatcherR0Method<Object, R, S>(iObject, iMethod);
		}
    }

    /** Construct object/method callback with const method.
     */
    template<class Object, typename S>
    CallbackR0(Object* iObject, S (Object::*iMethod)() const):
		dispatcher_(0)
    {
        if (iObject && iMethod) 
        {
			dispatcher_ = new impl::DispatcherR0ConstMethod<Object, R, S>(iObject, iMethod);
		}
    }

    /** Copy Constructor.
     */
    CallbackR0(const CallbackR0<R>& iOther):
        dispatcher_(iOther.isEmpty() ? 0 : iOther.dispatcher_->clone())
    {
    }

    /** Destructor
     */
    ~CallbackR0()
    {
        delete dispatcher_;
		dispatcher_ = 0;
    }
    
    
    // OPERATORS

    /** Assignment operator.
     */
    const CallbackR0& operator=(const CallbackR0<R>& iOther)
    {
        CallbackR0<R> temp(iOther);
		swap(temp);
        return *this;
    }           
    
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
        CallbackR0<R> temp;
		swap(temp);
    }

    /** Reset to function callback
     */
    template<typename S>
    void reset(S (*iFunction)())
    {
        CallbackR0<R> temp(iFunction);
        swap(temp);
    }
    
    /** Reset to object/method callback.
     */
    template<class Object, typename S>
    void reset(Object* iObject, S (Object::*iMethod)())
    {
        CallbackR0<R> temp(iObject, iMethod);
        swap(temp);
    }

    /** Reset to object/method callback with const method.
     */
    template<class Object, typename S>
    void reset(Object* iObject, S (Object::*iMethod)() const)
    {
        CallbackR0<R> temp(iObject, iMethod);
        swap(temp);
    }

    /** Reset to other callback.
     */
    void reset(const CallbackR0<R>& iOther)
    {
        CallbackR0<R> temp(iOther);
        swap(temp);
    }

    /** Returns true if no callback dispatcher is assigned to this object.
     */
    bool isEmpty() const
    {
        return dispatcher_ == 0;
    }

    /** Swaps the dispatcher of this callback with the dispatcher of another.
     */
    void swap(CallbackR0<R>& iOther)
    {
        std::swap(dispatcher_, iOther.dispatcher_);
    }

private:

    impl::DispatcherR0<R>* dispatcher_;
};



}

}

#endif // Guardian of Inclusion

// EOF
