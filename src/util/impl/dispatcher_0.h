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

#ifndef LASS_GUARDIAN_OF_INCLUSION_UTIL_IMPL_DISPATCHER_0_H
#define LASS_GUARDIAN_OF_INCLUSION_UTIL_IMPL_DISPATCHER_0_H

#include "../small_object.h"

// --- NEW INTERFACES ----------------------------------------------------------

namespace lass
{
namespace util
{
namespace impl
{

/** abstract base class of all dispatchers for lass::util::Callback0.
 *  @author Bram de Greve [BdG]
 *  @date 2002-2003
 */
class Dispatcher0: public SmallObject<>
{
public:

    virtual void operator()() const = 0;
    virtual Dispatcher0* clone() const = 0;
	virtual bool isEquivalent(const Dispatcher0* iOther) const = 0;
};



/** Dispatcher for lass::util::Callback0 to a free function:
 *  @author Bram de Greve [BdG]
 *  @date 2002-2003
 */
class Dispatcher0Function: public Dispatcher0
{
private:

    typedef void (*TFunction) ();
    TFunction function_;

public:

    Dispatcher0Function(TFunction iFunction):
        function_(iFunction)
    {
    }

    void operator()() const
    {
        (*function_)(); 
    }

    Dispatcher0* clone() const
    {
        return new Dispatcher0Function(*this);
    }

	bool isEquivalent(const Dispatcher0* iOther) const
	{
		const Dispatcher0Function* other = dynamic_cast<const Dispatcher0Function*>(iOther);
		return other && function_ == other->function_;
	}
};



/** Dispatcher for lass::util::Callback0 to an object/method pair.
 *  @author Bram de Greve [BdG]
 *  @date 2002-2003
 */
template <class Object>
class Dispatcher0Method: public Dispatcher0
{
private:

    typedef void (Object::*TMethod) ();
    Object* object_;
    TMethod method_;

public:

    Dispatcher0Method(Object* iObject, TMethod iMethod): 
        object_(iObject), 
        method_(iMethod)
    {
    }

    void operator()() const
    {
        (object_->*method_)(); 
    }
    
    Dispatcher0* clone() const
    {
        return new Dispatcher0Method<Object>(*this);
    }

	bool isEquivalent(const Dispatcher0* iOther) const
	{
		const Dispatcher0Method* other = dynamic_cast<const Dispatcher0Method*>(iOther);
		return other && object_ == other->object_ && method_ == other->method_;
	}
};



/** Dispatcher for lass::util::Callback0 to an object/const method pair.
 *  @author Bram de Greve [BdG]
 *  @date 2002-2003
 */
template <class Object>
class Dispatcher0ConstMethod: public Dispatcher0
{
private:

    typedef void (Object::*TConstMethod) () const;
    Object* object_;
    TConstMethod method_;

public:

    Dispatcher0ConstMethod(Object* iObject, TConstMethod iMethod): 
        object_(iObject), 
        method_(iMethod)
    {
    }

    void operator()() const
    {
        (object_->*method_)(); 
    }
    
    Dispatcher0* clone() const
    {
        return new Dispatcher0ConstMethod<Object>(*this);
    }

	bool isEquivalent(const Dispatcher0* iOther) const
	{
		const Dispatcher0ConstMethod* other = dynamic_cast<const Dispatcher0ConstMethod*>(iOther);
		return other && object_ == other->object_ && method_ == other->method_;
	}
};



}

}

}

#endif // Guardian of Inclusion


// EOF
