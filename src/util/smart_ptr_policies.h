/** @file
 *  @author Bram de Greve (bramz@users.sourceforge.net)
 *  @author Tom De Muer (tomdemuer@users.sourceforge.net)
 *
 *  Distributed under the terms of the GPL (GNU Public License)
 *
 *  The LASS License:
 *
 *  Copyright 2004-2006 Bram de Greve and Tom De Muer
 *
 *  LASS is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */

/** @defgroup SmartPtr Smart Pointers
 *  @brief collection of smart pointers as ScopedPtr, SharedPtr, ...
 *  @author Bram de Greve [Bramz]
 *
 *  @section Pointers
 *
 *  - lass::util::ScopedPtr: a smart pointer for single ownership, deallocates on end of scope.
 *  - lass::util::SharedPtr: a smart pointer for shared ownership, keeping track of reference count.
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
 *      TStorage& storage();
 *      // return reference to storage object (usualy to a pointer)
 *
 *      const TStorage& storage() const;
 *      // return const-reference to storage object
 *
 *  protected:
 *
 *      StoragePolicy();
 *      // should initialize with default storage object (in most cases: NULL)
 *
 *      StoragePolicy(T* pointee);
 *      // should initialize to store given pointee.
 *
 *      TPointer pointer() const;
 *      // should return a pointer to the pointee
 *
 *      TReference at(size_t iIndex) const;
 *      // for arrays only: should return reference to iIndex'th object in array
 *      // (YOU CAN OMMIT THIS FOR NON-ARRAYS)
 *
 *      void dispose();
 *      // deallocate the pointee (e.g. by a delete or delete [])
 *
 *      bool isNull() const;
 *      // return true if storage contains equivalent of NULL pointer.
 *
 *      void swap(StoragePolicy<T>& other);
 *      // swap storage object with other policy.
 *  };
 *  @endcode
 *
 *  @subsection StorageModels implemenated models of storage policy
 *
 *  - lass::util::ObjectStorage: simple storage policy for objects allocated by @c new.
 *  - lass::util::ArrayStorage: simple storage policy for arrays allocated by @c new[].
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
 *      DefaultCounter(); // brings object in valid state without setting any counter.
 *
 *      template <typename TStorage> void init(TStorage& pointee);
 *      // initialize reference count one one.  This is called on acquring of object by first owner
 *      // (increment isn't called for first owner).
 *
 *      template <typename TStorage> void dispose(TStorage& pointee);
 *      // clean up the counter (not the pointee).  This is called when reference count drops to
 *      // zero (thus on release by last owner).
 *
 *      template <typename TStorage> void increment(TStorage& pointee);
 *      // increment the reference count by one.  This is called on acquiring of object by any owner
 *      // except the first (the first one calls init(TStorage&) instead).
 *
 *      template <typename TStorage> bool decrement(TStorage& pointee);
 *      // decrement the reference count by one and returns true if it drops below one.  This is
 *      // called on every release of the object by any owner.  It should return true if @e after
 *      // the decrement the reference count turns out the be less than one (i.e. there are no more
 *      // owners, this was the last one), to indicate the pointee should be deallocated now.  In
 *      // all other cases, it should return false @e after the (i.e. if the reference count is
 *      // still non-zero after the decrement).
 *
 *      template <typename TStorage> TCount count(TStorage& pointee) const;
 *      // return the reference count, i.e. number of owners of the pointee
 *
 *      void swap(DefaultCounter& other);
 *      // swap any reference counters with other policy.
 *  };
 *  @endcode
 *
 *  @subsection CounterModels implemenated models of counter policy
 *
 *  - lass::util::DefaultCounter: allocates extra counter on heap to keep track of reference count.
 *  - lass::util::IntrusiveCounter: uses a member of the object as reference count.
 */

#ifndef LASS_GUARDIAN_OF_INCLUSION_UTIL_SMART_PTR_POLICIES_H
#define LASS_GUARDIAN_OF_INCLUSION_UTIL_SMART_PTR_POLICIES_H

#include "util_common.h"
#include "atomic.h"
#include "../meta/empty_type.h"

namespace lass
{
namespace util
{

// --- storage policies ----------------------------------------------------------------------------

/** @class ObjectStorage
 *  @ingroup SmartPtr
 *  @brief Default storage policy for single objects, implementation of StoragePolicy concept
 *  @author Bram de Greve [Bramz]
 *
 *  This policy is meant for simple pointees that are allocated in @c C++ by @c new or equivalent
 *  (i.e. stuff that should be deallocated by @c delete ).  The storage type @c TStorage used for
 *  this policy simply is an ordinary pointer to that pointee (@c T*)/.
 */
template 
<
	typename T,
	typename Cascade = meta::EmptyType
>
class ObjectStorage: public Cascade
{
public:

	typedef ObjectStorage<T, Cascade> TSelf;
	typedef T* TStorage;
	typedef T* TPointer;
	typedef T& TReference;

	TStorage& storage() { return storage_; }
	const TStorage& storage() const { return storage_; }

protected:

	ObjectStorage(): Cascade(), storage_(defaultStorage()) {}
	explicit ObjectStorage(T* pointee): Cascade(), storage_(pointee) {}
	ObjectStorage(const TSelf& other): Cascade(other), storage_(other.storage_) {}
	template <typename U> ObjectStorage(const ObjectStorage<U, Cascade>& other): Cascade(other), storage_(other.pointee) {} 

	TPointer pointer() const { return storage_; }

	void dispose()
	{
		delete storage_;
		storage_ = 0;
	}

	bool isNull() const { return !storage_; }
	void swap(TSelf& other) { Cascade::swap(other); std::swap(storage_, other.storage_); }

	static TStorage defaultStorage() { return 0; }

private:

	TStorage storage_;
};



/** @class ArrayStorage
 *  @ingroup SmartPtr
 *  @brief Default storage policy for arrays, implementation of StoragePolicy concept
 *  @author Bram de Greve [Bramz]
 *
 *  This policy is meant for simple arrays that are allocated in @c C++ by @c new[] or equivalent
 *  (i.e. stuff that should be deallocated by @c delete[] ).  The storage type @c TStorage used for
 *  this policy simply is an ordinary pointer to that array (@c T*)/.
 */
template 
<
	typename T,
	typename Cascade = meta::EmptyType
>
class ArrayStorage: public Cascade
{
public:

	typedef ArrayStorage<T, Cascade> TSelf;
	typedef T* TStorage;
	typedef T* TPointer;
	typedef T& TReference;

	TStorage& storage() { return storage_; }
	const TStorage& storage() const { return storage_; }

protected:

	ArrayStorage(): Cascade(), storage_(defaultStorage()) { }
	explicit ArrayStorage(T* pointee): Cascade(), storage_(pointee) { }
	ArrayStorage(const TSelf& other): Cascade(other), storage_(other.storage_) {}
	template <typename U> ArrayStorage(const ArrayStorage<U, Cascade>& other): Cascade(other), storage_(other.pointee) {} 

	TPointer pointer() const { return storage_; }
	TReference at(size_t index) const { return storage_[index]; }

	void dispose()
	{
		delete [] storage_;
		storage_ = 0;
	}

	bool isNull() const { return !storage_; }
	void swap(TSelf& other) { Cascade::swap(other); std::swap(storage_, other.storage_);  }

	static TStorage defaultStorage() { return 0; }

private:

	TStorage storage_;
};






// --- counter policies ----------------------------------------------------------------------------

namespace impl
{
	LASS_DLL void initHeapCounter(int*& ioCounter, int iInitialValue);
	LASS_DLL void disposeHeapCounter(int*& ioCounter);
}

/** @class DefaultCounter
 *  @ingroup SmartPtr
 *  @brief The default counter for the shared pointers, implementation of CounterPolicy concept.
 *  @author Bram de Greve [Bramz]
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

	DefaultCounter(): count_(0) {}

	template <typename TStorage> void init(TStorage& /*pointee*/)
	{
		impl::initHeapCounter(count_, 1);
		LASS_ASSERT(count_ && *count_ == 1);
	}

	template <typename TStorage> void dispose(TStorage& /*pointee*/)
	{
		LASS_ASSERT(count_ && *count_ == 0);
		impl::disposeHeapCounter(count_);
	}

	template <typename TStorage> void increment(TStorage& /*pointee*/)
	{
		TCount oldCount = 0, newCount = 0;
		do
		{
			LASS_ASSERT(count_);
			oldCount = *count_;
			LASS_ASSERT(oldCount > 0);
			newCount = oldCount + 1;
		}
		while (!atomicCompareAndSwap(*count_, oldCount, newCount));
	}

	template <typename TStorage> bool decrement(TStorage& /*pointee*/)
	{
		TCount oldCount = 0, newCount = 0;
		do
		{
			LASS_ASSERT(count_);
			oldCount = *count_;
			LASS_ASSERT(oldCount > 0);
			newCount = oldCount - 1;
		}
		while (!atomicCompareAndSwap(*count_, oldCount, newCount));
		return newCount == 0;
	}

	template <typename TStorage> TCount count(TStorage& /*pointee*/) const
	{
		LASS_ASSERT(count_ && *count_ > 0);
		return *count_;
	}

	void swap(DefaultCounter& other) { std::swap(count_, other.count_); }

private:

	TCount* count_;
};




/** @class IntrusiveCounter
 *  @brief implementation of CounterPolicy concept, using a counter in the managed object itself.
 *  @ingroup SmartPtr
 *  @author Bram de Greve [Bramz]
 *
 *  This comes from "C++ Templates, The Complete Guide" by David Vandevoorde and
 *  Nicolai M. Josuttis.  See their @c MemberReferenceCount policy.
 *
 *  The DefaultCounter policy puts the reference counter outside the managed object.  That's great in
 *  most cases, because that way, the managed type doesn't have to be designed to be used with our
 *  SharedPtr.  However, in some cases it is more interesting to put the reference counter @e inside
 *  the managed object.  You can use the IntrusiveCounter policy to use such a reference counter with
 *  our SharedPtr.
 *
 *  @code
 *  struct Foo
 *  {
 *      std::string blablabla;
 *      int referenceCount;
 *  };
 *
 *  typedef lass::util::SharedPtr
 *  <
 *      Foo,
 *      lass::util::ObjectStorage,
 *      lass::util::IntrusiveCounter
 *      <
 *          Foo,                        // the managed type (containing the counter)
 *          int,                        // type of the counter
 *          &Foo::referenceCount        // pointer to the counter
 *      >
 *  >
 *  TFooPtr;
 *
 *  TFooPtr foo(new Foo);
 *  @endcode
 */
template
<
	typename T,
	typename CounterType,
	CounterType T::* referenceCounter
>
class IntrusiveCounter
{
public:

	typedef IntrusiveCounter<T, CounterType, referenceCounter> TSelf;
	typedef CounterType TCount;

protected:

	template <typename TStorage> void init(TStorage& pointee)
	{
		LASS_ASSERT(pointee);
		(pointee->*referenceCounter) = 1;
	}

	template <typename TStorage> void dispose(TStorage& pointee)
	{
		LASS_ASSERT(pointee && (pointee->*referenceCounter) == 0);
	}

	template <typename TStorage> void increment(TStorage& pointee)
	{
		LASS_ASSERT(pointee);
		TCount oldCount = 0, newCount = 0;
		do
		{
			oldCount = pointee->*referenceCounter;
			LASS_ASSERT(oldCount > 0);
			newCount = oldCount + 1;
		}
		while (!atomicCompareAndSwap(pointee->*referenceCounter, oldCount, newCount));
	}

	template <typename TStorage> bool decrement(TStorage& pointee)
	{
		LASS_ASSERT(pointee);
		TCount oldCount = 0, newCount = 0;
		do
		{
			oldCount = pointee->*referenceCounter;
			LASS_ASSERT(oldCount > 0);
			newCount = oldCount - 1;
		}
		while (!atomicCompareAndSwap(pointee->*referenceCounter, oldCount, newCount));
		return newCount == 0;
	}

	template <typename TStorage> TCount count(TStorage& pointee) const
	{
		LASS_ASSERT(pointee && (pointee->*referenceCounter) > 0);
		return pointee->*referenceCounter;
	}

	void swap(TSelf& /*other*/)
	{
	}
};



}

}

#endif

// EOF
