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
