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
 *  @date 2004
 */

/** @class lass::util::SmartI
 *  @ingroup SmartPtr
 *  @brief smart pointer to COM interfaces
 *  @author Bram de Greve [Bramz]
 *
 *  @section interface_requirements Requirements for COM interface
 *  
 *  COM interface @a I must implement two methods @c AddRef() and @c Release():
 *  - @c AddRef() should increase reference count to interface
 *  - @c Release() should decrease reference count 
 */

#ifndef LASS_GUARDIAN_OF_INCLUSION_UTIL_SMART_I_H
#define LASS_GUARDIAN_OF_INCLUSION_UTIL_SMART_I_H

#include "util_common.h"
#include "../num/safe_bool.h"

namespace lass
{
namespace util
{

template <typename I>
class SmartI
{
public:


	typedef SmartI<I> TSelf;   /**< type of @c *this */

    typedef I TInterface;
    typedef I* TPointer;
    typedef I** TAddress;

    /** Rebind to another interface type.
     */
	template <typename I>
	struct Rebind
	{
		typedef SmartI<I> Type;
	};



	/** Default constructor initializes to NULL interface.
	 */
    SmartI(): 
        interface_(0) 
    {
    } 

	/** Constructor won't do an extra AddRef() on construction but will Release() on destruction.
	 */
    explicit SmartI(TInterface* iInterface): 
        interface_(iInterface) 
    {
    }

	/** Copy constructor performs AddRef() on interface.
	 */
    SmartI(const TSelf& iOther): 
        interface_(iOther.interface_) 
    { 
        safeAddRef(); 
    }

	/** Destructor performs Release() on interface.
     */
    ~SmartI() 
    { 
        safeRelease(); 
    }

    /** Reinitialize to NULL interface.
     *  Performs Release() on old interface.
	 */
    void reset()
	{
        TSelf temp;
        swap(temp);
    }

	/** Reinitialize to an interface, performs no AddRef on new interface, but will Release() it
     *  in destructor.
     *  Performs Release() on old interface.
     */
    void reset(TInterface* iInterface)
	{
		TSelf temp(iInterface);
		swap(temp);
	}

	/** assign the SmartI to another one, performing AddRef() on interface.
	 */
	TSelf& operator=(const TSelf& iOther)
	{
        TSelf temp(iOther);
        swap(temp);
        return *this;
	}

	/** return pointer to interface
	 */
	TPointer get() const 
    { 
        return interface_; 
    }

	/** access pointee as a pointer
	 */
	TPointer operator->() const
	{
		LASS_ASSERT(interface_);
		return interface_;
	}

	/** access address of interface pointer.
	 */
	TAddress address()
	{
		return &interface_;
	}

	/** return true if pointer doesn't own any interface.
	 *  @return this->get() == 0
	 */
	bool isEmpty() const
	{
		return interface_ == 0;
	}

	/** return true if pointer doesn't own any interface (same as isEmpty).
	 *  @return this->get() == 0.
	 */
	bool operator!() const
	{
		return interface_ == 0;
	}

	/** evaluates to true if pointer owns an interface (only in boolean context)
	 *  @return this->get() != 0.
	 *  @sa num::SafeBool
	 */
	operator num::SafeBool() const
	{
		return interface_ ? num::safeTrue : num::safeFalse;
	}

	/** exchange the pointees (and there reference counts) between two shared pointers
	 */
	void swap(TSelf& iOther)
	{
        std::swap(interface_, iOther.interface_);
	}

private:

    void safeAddRef()
    {
        if (interface_)
        {
            interface_->AddRef();
        }
    }

    void safeRelease()
    {
        if (interface_)
        {
            interface_->Release();
            interface_ = 0;
        }
    }

    TPointer interface_;
};

}

}

#endif

// EOF
