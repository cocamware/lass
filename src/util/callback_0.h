/** 
*   @author Bram de Greve (bramz@users.sourceforge.net)
*   @author Tom De Muer (tomdemuer@users.sourceforge.net)
*
*	Distributed under the terms of the GPL (GNU Public License)
*
* 	The LASS License:
*   
*	Copyright 2004 Bram de Greve and Tom De Muer
*
*   LASS is free software; you can redistribute it and/or modify
*   it under the terms of the GNU General Public License as published by
*   the Free Software Foundation; either version 2 of the License, or
*   (at your option) any later version.
*
*   This program is distributed in the hope that it will be useful,
*   but WITHOUT ANY WARRANTY; without even the implied warranty of
*   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
*   GNU General Public License for more details.
*
*   You should have received a copy of the GNU General Public License
*   along with this program; if not, write to the Free Software
*   Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
*


 */



/** @defgroup Callback Callback
 *  @brief library to wrap generalized callback functions in first class objects.
 *  @date 2002-2004
 *  @author Bram de Greve [BdG] (contact: bdegreve@mail.be)
 *
 *  The use of this library is to treat function calls as first class objects, so you can pass them
 *  around to be called at a later time.
 * 
 *  @code
 *  int fun(int a, int b) { return a + b; }
 *  // ...
 *  CallbackR2<int, int, int> cb(fun);
 *  // ...
 *  int result = cb(1, 2); // result will be 3.
 *  @endcode
 *
 *  Of course, the above example could be done just as easy with plain old C function pointers.
 *  That's true, but the advantage of this library will come forward when you want to call a
 *  method of a specific object instead.  You can tell what method must be called, and on what
 *  object.  Later, the caller can execute this without being aware this is a different case than
 *  above.  There's no way you can do this with plain old function pointers.
 *  
 *  @code
 *  class Foo
 *  {
 *      int m_;
 *  public:
 *      Foo(int m): m_(m) {}
 *      int bar(int a, b) const { return m * (a + b); }
 *  };
 *  // ...
 *  Foo foo(4);
 *  CallbackR2<int, int, int> cb2(&foo, Foo::bar);
 *  // ...
 *  int result2 = cb2(1, 2); // result2 will be 12.
 *  @endcode
 *
 *  Both constant as non-constant member functions are supported as dispatchers.
 *
 *  It is also possible to bind a function with a little different signature, but of which the
 *  arguments can be implicit converted to the right type.
 *
 *  @code
 *  float spam(float a, float b) { return a / b; }
 *  // ...
 *  CallbackR2<int, int, int> cb3(spam);
 *  // ...
 *  int result3 = cb3(5, 2); // result3 will be 2
 *  @endcode
 *
 *  Callbacks can also be in an empty state.  This is when the callback has no dispatcher, no 
 *  function call is bounded to it.  Callbacks will be empty when constructed by the default
 *  constructor, or when one of the arguments is a NULL pointer
 *
 *  @code
 *  CallbackR0<double> cb4;
 *  assert(cb4.empty());
 *  
 *  CallbackR1<bool, float> cb5(0); // NULL function pointer
 *  assert(cb5.empty());
 *
 *  CallbackR2<int, int, int> cb6(0, Foo::bar); // NULL ojbect pointer
 *  assert(cb6.empty());
 *  @endcode
 *
 *  When executing empty callbacks, we have to distinguish two situations: callbacks without
 *  return value, and callbacks with return value.  
 *  - In the former case, there's no danger.  When the callback is executed, it has no dispatcher to 
 *    call.  Hence, it won't do anything and it will immediately return.  
 *  - In the latter case, an exception will be thrown.  When the callback is executed, it has to 
 *    return a value.  Since there's no dispatcher to call that can return this value, we're in 
 *    troubles.  An Exception will be thrown to handle this error.
 *
 *  @code
 *  Callback2<int, const std::string&> cb7;
 *  assert(cb7.empty());
 *  cb7(4, "hello"); // OK, empty callback but no return value expected.
 *
 *  Callback3<std::string, int, int> cb8;
 *  assert(cb8.empty());
 *  try
 *  {
 *		std::string result8 = cb8(12, 34); // ERROR, an exception will be thrown.
 *  }
 *  catch(CallbackEmptyCall)
 *  {
 *		// you'll end up here on an empty call.
 *  }
 *
 *  std::string result8b = cb8 ? cb8(12, 34) : "bah"; // result8b will be "bah".
 *  @endcode
 * 
 *  @par Callbacks without return value:
 *
 *  - @ref lass::util::Callback0
 *  - @ref lass::util::Callback1
 *  - @ref lass::util::Callback2
 *  - @ref lass::util::Callback3
 *  - @ref lass::util::Callback4
 *  - @ref lass::util::Callback5
 *  - @ref lass::util::Callback6
 *  - @ref lass::util::Callback7
 *  - @ref lass::util::Callback8
 *  - @ref lass::util::Callback9
 *  - @ref lass::util::Callback10
 *
 *  @par Callbacks with return value:
 *
 *  - @ref lass::util::CallbackR0
 *  - @ref lass::util::CallbackR1
 *  - @ref lass::util::CallbackR2
 *  - @ref lass::util::CallbackR3
 *  - @ref lass::util::CallbackR4
 *  - @ref lass::util::CallbackR5
 *  - @ref lass::util::CallbackR6
 *  - @ref lass::util::CallbackR7
 *  - @ref lass::util::CallbackR8
 *  - @ref lass::util::CallbackR9
 *  - @ref lass::util::CallbackR10
 *
 *  @par Acknowledgements:
 *  I want to thank Altair, Jaap Suter & especially Dirk Gerrits (who has spent an entire afternoon
 *  on this to help me.  It was a waste of time for both of us, but we had a lot of fun, didn't we?)
 */



/** @class lass::util::Callback0
 *  @ingroup Callback
 *  @brief callback with 0 parameter(s) and without returnvalue.
 *  @date 2002
 *  @author Bram de Greve [BdG] (contact: bdegreve@mail.be)
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
#include "impl/dispatcher_0.h"




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

    // STRUCTORS
    
    /** Default constructor, construct empty callback.
     */
    Callback0(): 
        dispatcher_(0) 
    {
    }

    /** Construct function callback
     */
	explicit Callback0(void (*iFunction)()):
		dispatcher_(0)
    {
        if (iFunction)
        {
			dispatcher_ = new impl::Dispatcher0Function(iFunction);
        }
    }
    
    /** Construct object/method callback.
     */
	template <class Object>
    Callback0(Object* iObject, void (Object::*iMethod)()):
		dispatcher_(0)
    {
        if (iObject && iMethod) 
        {
			dispatcher_ = new impl::Dispatcher0Method<Object>(iObject, iMethod);
		}
    }

    /** Construct object/method callback with const method.
     */
	template <class Object>
    Callback0(Object* iObject, void (Object::*iMethod)() const):
		dispatcher_(0)
    {
        if (iObject && iMethod) 
        {
			dispatcher_ = new impl::Dispatcher0ConstMethod<Object>(iObject, iMethod);
		}
    }

    /** Copy Constructor.
     */
    Callback0(const Callback0& iOther):
        dispatcher_(iOther.isEmpty() ? 0 : iOther.dispatcher_->clone())
    {
    }

    /** Destructor
     */
    ~Callback0()
    {
        delete dispatcher_;
		dispatcher_ = 0;
    }
    
    
    // OPERATORS

    /** Assignment operator.
     */
    const Callback0& operator=(const Callback0& iOther)
    {
        Callback0 temp(iOther);
		swap(temp);
        return *this;
    }           
    
    /** THE operator.  Executes the callback.
     */
    void operator()() const 
    {
        if (!isEmpty()) 
        {
			(*dispatcher_)(); 
        }
    }


    // METHODS
    
    /** Reset to empty callback.
     */
    void reset() 
    {
        Callback0 temp;
		swap(temp);
    }

    /** Reset to function callback
     */
    void reset(void (*iFunction)())
    {
        Callback0 temp(iFunction);
        swap(temp);
    }
    
    /** Reset to object/method callback.
     */
	template <class Object>
    void reset(Object* iObject, void (Object::*iMethod)())
    {
        Callback0 temp(iObject, iMethod);
        swap(temp);
    }

    /** Reset to object/method callback with const method.
     */
 	template <class Object>
    void reset(Object* iObject, void (Object::*iMethod)() const)
    {
        Callback0 temp(iObject, iMethod);
        swap(temp);
    }

    /** Reset to other callback.
     */
    void reset(const Callback0& iOther)
    {
        Callback0 temp(iOther);
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
    void swap(Callback0& iOther)
    {
        std::swap(dispatcher_, iOther.dispatcher_);
    }

	/** return true if two callbacks are equivalent
	 */
	bool operator==(const Callback0& iOther) const
	{
		return dispatcher_->isEquivalent(iOther.dispatcher_);
	}

private:

    impl::Dispatcher0* dispatcher_;
};



/** return true if two callbacks are different
 *  @relates Callback0
 */
inline bool operator!=(const Callback0& iA, const Callback0& iB)
{
	return !(iA == iB);
}



}

}

#endif // Guardian of Inclusion

// EOF
