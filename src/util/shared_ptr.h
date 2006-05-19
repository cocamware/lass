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



/** @class lass::util::SharedPtr
 *  @ingroup SmartPtr
 *  @brief Smart pointer with reference counting.
 *  @author Bram de Greve
 *  @date 2003
 *
 *  To be honest, this is nothing but a shallow copy of boost::shared_ptr, see http://www.boost.org.
 *  So, let's quote boost for they have the best explanation of what SharedPtr is (mind though:
 *  we don't have shared arrays and weak pointers (yet)):
 *
 *  <i>"The shared_ptr class template stores a pointer to a dynamically allocated object, typically
 *  with a C++ new-expression . The object pointed to is guaranteed to be deleted when the last
 *  shared_ptr pointing to it is destroyed or reset. See the example.
 *
 *  Every shared_ptr meets the CopyConstructible and Assignable requirements of the C++ Standard
 *  Library, and so can be used in standard library containers. Comparison operators are supplied
 *  so that shared_ptr works with the standard library's associative containers.
 *
 *  Normally, a shared_ptr cannot correctly hold a pointer to a dynamically allocated array. See
 *  shared_array for that usage.
 *
 *  Because the implementation uses reference counting, cycles of shared_ptr instances will not be
 *  reclaimed. For example, if main() holds a shared_ptr to A, which directly or indirectly holds
 *  a shared_ptr back to A, A's use count will be 2. Destruction of the original shared_ptr will
 *  leave A dangling with a use count of 1. Use weak_ptr to "break cycles."
 *
 *  The class template is parameterized on T, the type of the object pointed to. shared_ptr and most
 *  of its member functions place no requirements on T; it is allowed to be an incomplete type, or
 *  void. Member functions that do place additional requirements (constructors, reset) are
 *  explicitly documented below.
 *
 *  shared_ptr<T> can be implicitly converted to shared_ptr<U> whenever T* can be implicitly
 *  converted to U*. In particular, shared_ptr<T> is implicitly convertible to shared_ptr<T const>,
 *  to shared_ptr<U> where U is an accessible base of T, and to shared_ptr<void>."</i>
 *
 *  @par Thread safety
 *		The shared pointers are supposed to be thread safe concerning the reference count.
 *		This means that as long as no SharedPtr are shared between threads, they should be thread safe.
 *		In particular: if you have two threads with two shared pointers a and b, both pointing to the same 
 *		data, and both threads try to inc/decrease the reference counts at once, this will be done in an 
 *		atomic way.  
 *		However, if you share a single SharedPtr between two threads, then operations that affect the
 *		pointer itself (like a.swap(b)) are _NOT_ thread safe, since both the pointer and the reference
 *		counter have to be altered at the same time.
 */



#ifndef LASS_GUARDIAN_OF_INCLUSION_UTIL_SHARED_PTR_H
#define LASS_GUARDIAN_OF_INCLUSION_UTIL_SHARED_PTR_H

#include "util_common.h"
#include "smart_ptr_policies.h"
#include "../num/safe_bool.h"

namespace lass
{
namespace util
{

class DefaultCounter;

template
<
	typename T,
	template <typename, typename> class StoragePolicy = ObjectStorage,
	class CounterPolicy = DefaultCounter
>
class SharedPtr: public StoragePolicy<T, CounterPolicy>
{
public:

	typedef SharedPtr<T, StoragePolicy, CounterPolicy> TSelf;   /**< type of @c *this */

	typedef T TPointee;                                         /**< type of object pointed too. */
	typedef StoragePolicy<T, CounterPolicy> TStoragePolicy;     /**< type of used storage policy. */
	typedef CounterPolicy TCounterPolicy;                       /**< type of used counter policy. */

	typedef typename TStoragePolicy::TStorage TStorage;         /**< storage object of pointee */
	typedef typename TStoragePolicy::TPointer TPointer;         /**< pointer to pointee */
	typedef typename TStoragePolicy::TReference TReference;     /**< reference to pointee */
	typedef typename CounterPolicy::TCount TCount;              /**< counter type */

	/** Rebind smart pointer with this policies to another type.
	 */
	template <typename U>
	struct Rebind
	{
		typedef SharedPtr<U, StoragePolicy, CounterPolicy> Type;
	};




	/** Default constructor initializes to a default set by storage policy, usually NULL.
	 */
	SharedPtr():
		TStoragePolicy()
	{
	}

	/** Constructs from iPointee allocated in agreement with storage policy or be @c NULL.
	 *  The reference counting starts here.
	 */
	explicit SharedPtr(T* iPointee):
		TStoragePolicy(iPointee)
	{
		if (!isEmpty())
		{
			CounterPolicy::init(TStoragePolicy::storage());
		}
	}

	/** Copy constructor.
	 *  Copy another SharedPtr, and increase the reference count.
	 */
	SharedPtr(const TSelf& iOther):
		TStoragePolicy(static_cast<const TStoragePolicy&>(iOther))
	{
		if (!isEmpty())
		{
			CounterPolicy::increment(TStoragePolicy::storage());
		}
	}

	/** Constructs by stealing pointer from a std::auto_ptr.
	 *  The reference counting starts here.
	*/
	SharedPtr(std::auto_ptr<T> iOther):
		TStoragePolicy(iOther.release())
	{
		if (!isEmpty())
		{
			CounterPolicy::init(TStoragePolicy::storage());
		}
	}

	/** Destructor only disposes pointee if reference count drops to zero.
	 *
	 *  Decreases the reference count.  If the reference count reaches 0, disposes the pointee by
	 *  its storage policy.
	 */
	~SharedPtr()
	{
		if (!isEmpty())
		{
			if (CounterPolicy::decrement(TStoragePolicy::storage()))
			{
				CounterPolicy::dispose(TStoragePolicy::storage());
				TStoragePolicy::dispose();
			}
		}
	}

	/** Reset the shared pointer to the default value by the storage policy.
	 *
	 *  Decreases the reference count and if it drops to zero, disposes the pointee.  Then it sets
	 *  itself to the default of the storage policy (usually this is the NULL pointer).
	 *
	 *  @warning Thread unsafe on individual SharedPtr level
	 */
	void reset()
	{
		TSelf temp;
		swap(temp);
	}

	/** Reset the shared pointer to another naked pointer.
	 *
	 *  Decreases the reference count and if it drops to zero, disposes the pointee.  Then it takes
	 *  iPointee as new pointee and it restarts reference counting.
	 *
	 *  @warning Thread unsafe on individual SharedPtr level
	 */
	void reset(T* iPointee)
	{
		TSelf temp(iPointee);
		swap(temp);
	}

	/** Reset the shared pointer to pointer of a std::auto_ptr.
	 *
	 *  Decreases the reference count and if it drops to zero, disposes the pointee.  Then it steals
	 *  the pointer of iOther as new pointee and it restarts reference counting.
	 *
	 *  @warning Thread unsafe on individual SharedPtr level
	 */
	void reset(std::auto_ptr<T> iOther)
	{
		TSelf temp(iOther);
		swap(temp);
	}

	/** assign the shared pointer with another one.
	 *
	 *  Decreases the reference count and if it drops to zero, disposes the pointee.  Then it copies
	 *  storage and counter from @c iOther and in increases the reference count.
	 *
	 *  Protects against self-assignment (i.e. if you do 'foo = bar' under the condition that
	 *  'foo.get() == bar.get()', then nothing happens :)
	 *
	 *  @warning Thread unsafe on individual SharedPtr level
	 */
	TSelf& operator=(const TSelf& iOther)
	{
		if (TStoragePolicy::storage() != static_cast<const TStoragePolicy&>(iOther).storage())
		{
			TSelf temp(iOther);
			swap(temp);
		}
		return *this;
	}

	/** return pointer to pointee
	 */
	TPointer get() const
	{
		return TStoragePolicy::pointer();
	}

	/** access pointee as a pointer
	 */
	TPointer operator->() const
	{
		LASS_ASSERT(TStoragePolicy::pointer());
		return TStoragePolicy::pointer();
	}

	/** access pointee as a reference
	 */
	TReference operator*() const
	{
		LASS_ASSERT(TStoragePolicy::pointer());
		return *TStoragePolicy::pointer();
	}

	/** access iIndex'th object in pointee.
	 *
	 *  This obviously only works if the pointee is an array.  For this kind of pointees you should
	 *  use a @c StoragePolicy like ArrayStorage which implements @c at(size_t) (ObjectStorage
	 *  doesn't implement that one), and which uses @c delete[] to deallocate the pointee.
	 */
	TReference operator[](size_t iIndex) const
	{
		LASS_ASSERT(TStoragePolicy::pointer());
		return TStoragePolicy::at(iIndex);
	}

	/** access by reference the storage object that stores the link to the pointee.
	 *
	 *  This will usually return a reference to a pointer (@c T*&).
	 *  @warning You can screw up things seriously if you change the storage object.
	 *           DON'T DO THIS UNLESS YOU'RE ABSOLUTELY SURE YOU KNOW WHAT YOU'RE DOING!
	 */
	TStorage& storage()
	{
		return TStoragePolicy::storage();
	}

	/** return by reference the storage object that stores the link to the pointee.
	 *  This will usually return a constant reference to a pointer (const T*&).
	 */
	const TStorage& storage() const
	{
		return TStoragePolicy::storage();
	}

	/** return number of owners of this pointee.
	 *  @return 0 if pointer is empty (get() == 0), the number of owners otherwise.
	 */
	TCount count() const
	{
		return !isEmpty() ? CounterPolicy::count(TStoragePolicy::storage()) : 0;
	}

	/** return true if this is the only owner of this pointee
	 *  an empty pointer is never unique.
	 *  @return true if this->count() == 1.
	 */
	bool isUnique() const
	{
		return count() == 1;
	}

	/** return true if pointer doesn't own any object.
	 *  @return this->get() == 0
	 */
	bool isEmpty() const
	{
		return TStoragePolicy::isNull();
	}

	/** return true if pointer doesn't own any object (same as isEmpty).
	 *  @return this->get() == 0.
	 */
	bool operator!() const
	{
		return isEmpty();
	}

	/** evaluates to true if pointer owns an object (only in boolean context)
	 *  @return this->get() != 0.
	 *  @sa num::SafeBool
	 */
	operator num::SafeBool() const
	{
		return isEmpty() ? num::safeFalse : num::safeTrue;
	}

	/** exchange the pointees (and there reference counts) between two shared pointers
	 *
	 *  @warning Thread unsafe on individual SharedPtr level
	 */
	void swap(TSelf& iOther)
	{
		TStoragePolicy::swap(static_cast<TStoragePolicy&>(iOther));
		CounterPolicy::swap(static_cast<CounterPolicy&>(iOther));
	}
};



}

}

#endif
