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
	template <typename U> ObjectStorage(const ObjectStorage<U, Cascade>& other): Cascade(other), storage_(other.storage()) {} 

	TPointer pointer() const { return storage_; }

	void dispose()
	{
		delete storage_;
		storage_ = 0;
	}

	bool isNull() const { return !storage_; }
	void swap(TSelf& other) { Cascade::swap(other); std::swap(storage_, other.storage_); }

	template <typename U> const ObjectStorage<U, Cascade> staticCast() const
	{
		return ObjectStorage<U, Cascade>(static_cast<U*>(storage_), *this);
	}

	template <typename U> const ObjectStorage<U, Cascade> dynamicCast() const
	{
		if (U* p = dynamic_cast<U*>(storage_))
		{		
			return ObjectStorage<U, Cascade>(p, *this);
		}
		return ObjectStorage<U, Cascade>();
	}

	template <typename U> const ObjectStorage<U, Cascade> constCast() const
	{
		return ObjectStorage<U, Cascade>(const_cast<U*>(storage_), *this);
	}

	static TStorage defaultStorage() { return 0; }

private:

	template <typename U, typename C> friend class ObjectStorage;
	template <typename U, template <typename, typename> class S, typename C> friend class SharedPtr;

	ObjectStorage(T* pointee, const Cascade& cascade): Cascade(cascade), storage_(pointee) {}
	
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
	template <typename U> ArrayStorage(const ArrayStorage<U, Cascade>& other): Cascade(other), storage_(other.storage()) {} 

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
	LASS_DLL void initHeapCounter(volatile size_t*& ioCounter, size_t iInitialValue);
	LASS_DLL void disposeHeapCounter(volatile size_t*& ioCounter);
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

	typedef size_t TCount;

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
		LASS_ASSERT(count_ && *count_ > 0);
		atomicIncrement(*count_);
	}

	template <typename TStorage> bool decrement(TStorage& /*pointee*/)
	{
		LASS_ASSERT(count_);
		TCount oldCount = 0, newCount = 0;
		do
		{
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

	volatile TCount* count_;
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
