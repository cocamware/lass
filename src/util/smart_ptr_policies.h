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

/** @defgroup SmartPtr Smart Pointers
 *  @brief collection of smart pointers as ScopedPtr, SharedPtr, ...
 *  @author Bram de Greve [BdG]
 *
 *  @section Pointers
 *
 *  - lass::util::ScopedPtr: a smart pointer for single ownership, deallocates on end of scope.
 *  - lass::util::SharedPtr: a smart pointer for shared ownership, keeping track of reference count.
 *  - lass::util::ScopedArray: deprecated, use @c ScopedPtr<T,ArrayStorage> now.
 *  - lass::util::SmartI: a smart pointer to COM interfaces.
 *
 *  @section StoragePolicy
 *
 *  Storage polices control how the pointees are stored in the smart pointer and how they should
 *  be deallocated.  Two default storage policies are provided for objects allocated by @c new
 *  and arrays allocated by @c new[], but it should be possible to write special storage policies
 *  to embed other @e smart pointers in our lass::util smart pointer classes.
 *
 *  @subsection StorageConcept concept of storage policy
 *
 *  All storage policies should implement the following interface.  Some functions like 
 *  @c at(size_t) should only be implemented for array-like pointees.  So they can safely be
 *  ommited if pointee does not act like an array.
 *
 *  @code
 *  template <typename T>
 *  class StoragePolicy
 *  {
 *  public:
 *  
 *      typedef ... TStorage;   // the type of the storage_ object (usually T*)
 *      typedef ... TPointer;   // return type of pointer() or operator-> (usually T*)
 *      typedef ... TReference; // return type of operator* or operator[] (usually T&)
 *  
 *      TStorage& storage() throw();
 *      // return reference to storage object (usualy to a pointer)
 *
 *      const TStorage& storage() const throw(); 
 *      // return const-reference to storage object
 *  
 *  protected:
 *
 *      StoragePolicy() throw();  
 *      // should initialize with default storage object (in most cases: NULL)
 *
 *      StoragePolicy(T* iPointee) throw();
 *      // should initialize to store given pointee.
 *  
 *      TPointer pointer() const throw(); 
 *      // should return a pointer to the pointee
 *
 *      TReference at(size_t iIndex) const; 
 *      // for arrays only: should return reference to iIndex'th object in array 
 *      // (YOU CAN OMMIT THIS FOR NON-ARRAYS)
 *  
 *      void dispose() throw(); 
 *      // deallocate the pointee (e.g. by a delete or delete [])
 *
 *      bool isNull() const throw(); 
 *      // return true if storage contains equivalent of NULL pointer.
 *
 *      void swap(StoragePolicy<T>& iOther) throw(); 
 *      // swap storage object with other policy.
 *  };
 *  @endcode
 *
 *  @subsection StorageModels implemenated models of storage policy
 *
 *  - lass::util::ObjectStorage: simple storage policy for objects allocated by @c new.
 *  - lass::util::ArrayStorage: simple storage policy for arrays allocated by @c new[].
 *  - lass::util::ComStorage: storage policy for COM interfaces.
 *
 *  @section CounterPolicy
 *
 *  Counter policies control how reference counting should be done.  In contrary to the storage
 *  policies, this is only used by the SharedPtr.
 *
 *  @subsection CounterConcept concept of counter policy
 *
 *  All counter policies should implement the following interface.  The template parameter 
 *  @c TStorage is of the same type as @c TStorage in the used storage policy.  It will useually
 *  be a pointer to the pointee.
 *
 *  @code 
 *  class CounterPolicy
 *  {
 *  public:
 *  
 *      typedef ... TCount; // type of reference counter variable (should be an integer)
 *  
 *  protected:
 *  
 *      DefaultCounter() throw(); // brings object in valid state without setting any counter.
 *
 *      template <typename TStorage> void init(TStorage& iPointee); 
 *      // initialize reference count one one.  This is called on acquring of object by first owner
 *      // (increment isn't called for first owner). 
 *  
 *      template <typename TStorage> void dispose(TStorage& iPointee) throw();
 *      // clean up the counter (not the pointee).  This is called when reference count drops to 
 *      // zero (thus on release by last owner).
 *      
 *      template <typename TStorage> void increment(TStorage& iPointee) throw();
 *      // increment the reference count by one.  This is called on acquiring of object by any owner
 *      // except the first (the first one calls init(TStorage&) instead).
 *  
 *      template <typename TStorage> bool decrement(TStorage& iPointee) throw();
 *      // decrement the reference count by one and returns true if it drops below one.  This is 
 *      // called on every release of the object by any owner.  It should return true if @e after 
 *      // the decrement the reference count turns out the be less than one (i.e. there are no more
 *      // owners, this was the last one), to indicate the pointee should be deallocated now.  In
 *      // all other cases, it should return false @e after the (i.e. if the reference count is
 *      // still non-zero after the decrement).
 *      
 *      template <typename TStorage> TCount count(TStorage& iPointee) const throw();
 *      // return the reference count, i.e. number of owners of the pointee
 *
 *      void swap(DefaultCounter& iOther) throw();
 *      // swap any reference counters with other policy.
 *  };
 *  @endcode
 *
 *  @subsection CounterModels implemenated models of counter policy
 *
 *  - lass::util::DefaultCounter: allocates extra counter on heap to keep track of reference count.
 *  - lass::util::ComCounter: counter policy for COM interfaces, to be used with ComStorage
 */

#ifndef LASS_GUARDIAN_OF_INCLUSION_UTIL_SMART_PTR_POLICIES_H
#define LASS_GUARDIAN_OF_INCLUSION_UTIL_SMART_PTR_POLICIES_H

#include "util_common.h"

namespace lass
{
namespace util
{

// --- storage policies ----------------------------------------------------------------------------

/** @class ObjectStorage
 *  @ingroup SmartPtr
 *  @brief Default storage policy for single objects, implementation of StoragePolicy concept
 *  @author Bram de Greve [BdG]
 *
 *  This policy is meant for simple pointees that are allocated in @c C++ by @c new or equivalent
 *  (i.e. stuff that should be deallocated by @c delete ).  The storage type @c TStorage used for
 *  this policy simply is an ordinary pointer to that pointee (@c T*)/.
 */
template <typename T>
class ObjectStorage
{
public:

	typedef T* TStorage;
	typedef T* TPointer;
	typedef T& TReference;

	TStorage& storage() throw() { return storage_; }
	const TStorage& storage() const throw() { return storage_; }

protected:

	ObjectStorage() throw(): storage_(defaultStorage()) {}
	ObjectStorage(T* iPointee) throw(): storage_(iPointee) {}

	TPointer pointer() const throw() { return storage_; }

	void dispose() throw() 
	{
		delete storage_; 
		storage_ = 0; 
	}

	bool isNull() const throw() { return !storage_;	}
	void swap(ObjectStorage<T>& iOther) throw() { std::swap(storage_, iOther.storage_);	}

	static TStorage defaultStorage() throw() { return 0; }

private:

	TStorage storage_;
};



/** @class ArrayStorage
 *  @ingroup SmartPtr
 *  @brief Default storage policy for arrays, implementation of StoragePolicy concept
 *  @author Bram de Greve [BdG]
 *
 *  This policy is meant for simple arrays that are allocated in @c C++ by @c new[] or equivalent
 *  (i.e. stuff that should be deallocated by @c delete[] ).  The storage type @c TStorage used for
 *  this policy simply is an ordinary pointer to that array (@c T*)/.
 */
template <typename T>
class ArrayStorage
{
public:

	typedef T* TStorage;
	typedef T* TPointer;
	typedef T& TReference;

	TStorage& storage() throw() { return storage_; }
	const TStorage& storage() const throw() { return storage_; }

protected:

	ArrayStorage() throw(): storage_(defaultStorage()) { }
	ArrayStorage(T* iPointee) throw(): storage_(iPointee) { }

	TPointer pointer() const throw() { return storage_; }
	TReference at(size_t iIndex) const { return storage_[iIndex]; }

	void dispose() throw()
	{
		delete [] storage_; 
		storage_ = 0; 
	}

	bool isNull() const throw() { return !storage_; }
    void swap(ArrayStorage<T>& iOther) throw() { std::swap(storage_, iOther.storage_);	}

	static TStorage defaultStorage() throw() { return 0; }

private:

	TStorage storage_;
};



// --- counter policies ----------------------------------------------------------------------------

/** @class DefaultCounter
 *  @ingroup SmartPtr
 *  @brief The default counter for the shared pointers, implementation of CounterPolicy concept.
 *  @author Bram de Greve [BdG]
 *
 *  The default counter will use an self-owned integer as counter object.  It will allocate this on
 *  the heap and share it between all smart pointers that own the same pointee (duh!).  Of course
 *  it will be so nice to clean it up by itself too.
 */
class DefaultCounter
{
public:

	typedef int TCount;

protected:

	DefaultCounter() throw(): count_(0) {}

	template <typename TStorage> void init(TStorage& iPointee)
	{
		LASS_ASSERT(!count_);
		count_ = new TCount;
		LASS_ASSERT(count_);
		*count_ = 1;
	}

	template <typename TStorage> void dispose(TStorage& iPointee) throw()
	{
		LASS_ASSERT(count_);
		delete count_;
		count_ = 0;
	}
	
	template <typename TStorage> void increment(TStorage& iPointee) throw()
	{
		LASS_ASSERT(count_);
		++*count_;
	}

	template <typename TStorage> bool decrement(TStorage& iPointee) throw()
	{
		LASS_ASSERT(count_);
		--*count_;
		return *count_ < 1;
	}
	
	template <typename TStorage> TCount count(TStorage& iPointee) const throw()
	{
		LASS_ASSERT(count_);
		return *count_;
	}
	
	void swap(DefaultCounter& iOther) throw() {	std::swap(count_, iOther.count_); }

private:

	TCount* count_;
};



}

}

#endif

// EOF
