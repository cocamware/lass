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



/** @class lass::util::ScopedPtr
 *  @ingroup SmartPtr
 *  @brief Smart pointer with simple sole ownership of single objects, non-copyable.
 *  @author Bram de Greve
 *  @date 2003
 *
 *  To be honest, this is nothing but a copy of boost::scoped_ptr, see http://www.boost.org.
 *  So, let's quote boost for they have the best explanation of what ScopedPtr is:
 *
 *  <i>"The scoped_ptr class template stores a pointer to a dynamically allocated object. 
 *  (Dynamically allocated objects are allocated with the C++ new expression.) The object 
 *  pointed to is guaranteed to be deleted, either on destruction of the scoped_ptr, or via 
 *  an explicit reset. See the example.
 *  
 *  The scoped_ptr template is a simple solution for simple needs. It supplies a basic 
 *  "resource acquisition is initialization" facility, without shared-ownership or 
 *  transfer-of-ownership semantics. Both its name and enforcement of semantics (by being 
 *  noncopyable) signal its intent to retain ownership solely within the current scope. 
 *  Because it is noncopyable, it is safer than shared_ptr or std::auto_ptr for pointers 
 *  which should not be copied.
 *
 *  Because scoped_ptr is simple, in its usual implementation every operation is as fast as 
 *  for a built-in pointer and it has no more space overhead that a built-in pointer.
 *
 *  scoped_ptr cannot be used in C++ Standard Library containers. Use shared_ptr if you need
 *  a smart pointer that can.
 *
 *  scoped_ptr cannot correctly hold a pointer to a dynamically allocated array. See 
 *  scoped_array for that usage.
 *
 *  The class template is parameterized on T, the type of the object pointed to. T must meet
 *  the smart pointer common requirements."</i>
 */



#ifndef LASS_GUARDIAN_OF_INCLUSION_UTIL_SCOPED_PTR_H
#define LASS_GUARDIAN_OF_INCLUSION_UTIL_SCOPED_PTR_H



#include "util_common.h"
#include "smart_ptr_policies.h"
#include "non_copyable.h"
#include "../num/safe_bool.h"


namespace lass
{

namespace util
{

template 
<
    typename T,
    template <typename> class StoragePolicy = ObjectStorage
>
class ScopedPtr: public StoragePolicy<T>, public NonCopyable
{
public:

	typedef ScopedPtr<T, StoragePolicy> TSelf;                  /**< type of @c *this */

	typedef T TPointee;                                         /**< type of object pointed too. */
	typedef StoragePolicy<T> TStoragePolicy;                    /**< type of used storage policy. */

	typedef typename StoragePolicy<T>::TStorage TStorage;       /**< storage object of pointee */
	typedef typename StoragePolicy<T>::TPointer TPointer;       /**< pointer to pointee */
	typedef typename StoragePolicy<T>::TReference TReference;   /**< reference to pointee */

    /** Rebind smart pointer with this policies to another type.
     */
	template <typename U>
	struct Rebind
	{
		typedef ScopedPtr<U, StoragePolicy> Type;
	};


	/** Default constructor initializes to a default set by storage policy, usually NULL.
	 */
	ScopedPtr() throw(): 
		StoragePolicy<T>()
	{
	}

	/** Constructs from iPointee allocated in agreement with storage policy or be @c NULL.
	 */
	explicit ScopedPtr(T* iPointee) throw(): 
		StoragePolicy<T>(iPointee)
	{
	}

	/** Constructs by stealing pointer from a std::auto_ptr.
	 */
	ScopedPtr(std::auto_ptr<T> iOther) throw():
		StoragePolicy<T>(iOther.release())
	{
	}

	/** Destructor always disposes pointee.
	 */
	~ScopedPtr() throw()
	{
		StoragePolicy<T>::dispose();
	}

    /** reset the scoped pointer to default by disposing old pointee and setting to default. 
     */
    void reset() throw()
    {
        TSelf temp;
        swap(temp);
    }

	/** reset the scoped pointer by disposing old pointee and taking new one.
	 */
	void reset(T* iPointee) throw()
	{
		TSelf temp(iPointee);
		swap(temp);
	}

    /** reset the scoped pointer by disposing old pointee and taking new one from std::auto_ptr.
     */
    void reset(std::auto_ptr<T> iOther) throw()
    {
        TSelf temp(iOther);
        swap(temp);
    }

	/** return pointer to pointee.
	 */
	TPointer get() const throw()
	{
		return StoragePolicy<T>::pointer();
	}

	/** access pointee as a pointer.
	 */
	TPointer operator->() const
	{
		LASS_ASSERT(StoragePolicy<T>::pointer());
		return StoragePolicy<T>::pointer();
	}

	/** access pointee as a reference.
	 */
	TReference operator*() const
	{
		LASS_ASSERT(StoragePolicy<T>::pointer());
		return *StoragePolicy<T>::pointer();
	}

	/** access iIndex'th object in pointee.
     *
	 *  This obviously only works if the pointee is an array.  For this kind of pointees you should
	 *  use a @c StoragePolicy like ArrayStorage which implements @c at(size_t) (ObjectStorage 
	 *  doesn't implement that one), and which uses @c delete[] to deallocate the pointee.
	 */
	TReference operator[](size_t iIndex) const
	{
		LASS_ASSERT(StoragePolicy<T>::pointer());
		return StoragePolicy<T>::at(iIndex);
	}

	/** access by reference the storage object that stores the link to the pointee.
	 *  This will usually return a reference to a pointer, @c T*&.
     *
	 *  @warning You can screw up things seriously if you change the storage object.  
     *           DON'T DO THIS UNLESS YOU'RE ABSOLUTELY SURE YOU KNOW WHAT YOU'RE DOING!
	 */
	TStorage& storage() throw()
	{
		return StoragePolicy<T>::storage();
	}

	/** return by reference the storage object that stores the link to the pointee.
	 *  This will usually return a constant reference to a pointer, const T*&.
	 */
	const TStorage& storage() const throw()
	{
		return StoragePolicy<T>::storage();
	}

	/** return true if pointer doesn't own any object.
	 *  @return this->get() == 0.
	 */
	bool isEmpty() const throw()
	{
		return StoragePolicy<T>::isNull();
	}

	/** return true if pointer doesn't own any object (same as isEmpty).
	 *  @return this->get() == 0.
	 */
	bool operator!() const throw()
	{
		return isEmpty();
	}

	/** evaluates to true if pointer owns an object (only in boolean context).
	 *  @return this->get() != 0.
	 *  @sa num::SafeBool
	 */
	operator num::SafeBool() const throw()
	{
		return isEmpty() ? num::safeFalse : num::safeTrue;
	}

	/** exchange the pointees between two shared pointers
	 */
	void swap(TSelf& iOther) throw()
	{
		StoragePolicy<T>::swap(static_cast<StoragePolicy<T>&>(iOther));
	}

};



}

}




#endif




