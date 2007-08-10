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

#ifndef LASS_GUARDIAN_OF_INCLUSION_UTIL_SHARED_PTR_H
#define LASS_GUARDIAN_OF_INCLUSION_UTIL_SHARED_PTR_H

#include "util_common.h"
#include "smart_ptr_policies.h"
#include "../num/safe_bool.h"

namespace lass
{
namespace util
{

template
<
	typename T,
	template <typename, typename> class StoragePolicy = ObjectStorage,
	typename CounterPolicy = DefaultCounter
>
class SharedPtr: public StoragePolicy<T, CounterPolicy>
{
public:

	typedef T TPointee;
	typedef StoragePolicy<T, CounterPolicy> TStoragePolicy;
	typedef CounterPolicy TCounterPolicy;
	typedef typename TStoragePolicy::TStorage TStorage;
	typedef typename TStoragePolicy::TPointer TPointer;
	typedef typename TStoragePolicy::TReference TReference;
	typedef typename TCounterPolicy::TCount TCount;

	template <typename U> struct Rebind
	{
		typedef SharedPtr<T, StoragePolicy, CounterPolicy> Type;
	};

	SharedPtr():
		TStoragePolicy()
	{
	}
	explicit SharedPtr(TPointer p):
		TStoragePolicy(p)
	{
		if (!isEmpty())
		{
			CounterPolicy::init(TStoragePolicy::storage());
		}
	}
	template <typename U> SharedPtr(std::auto_ptr<U>& p):
		TStoragePolicy(p.get())
	{
		if (!isEmpty())
		{
			CounterPolicy::init(TStoragePolicy::storage());
		}
		p.release();
	}
	SharedPtr(const SharedPtr& other):
		TStoragePolicy(other)
	{
		if (!isEmpty())
		{
			CounterPolicy::increment(TStoragePolicy::storage());
		}
	}
	template <typename U> SharedPtr(const SharedPtr<U, StoragePolicy, CounterPolicy>& other):
		TStoragePolicy(other)
	{
		if (!isEmpty())
		{
			CounterPolicy::increment(TStoragePolicy::storage());
		}
	}

	~SharedPtr()
	{
		if (!isEmpty() && CounterPolicy::decrement(TStoragePolicy::storage()))
		{
			CounterPolicy::dispose(TStoragePolicy::storage());
			TStoragePolicy::dispose();
		}
	}

	SharedPtr& operator=(const SharedPtr& other)
	{
		SharedPtr temp(other);
		swap(temp);
		return *this;
	}

	void reset(TPointer p = 0)
	{
		SharedPtr temp(p);
		swap(temp);
	}
	template <typename U> void reset(std::auto_ptr<U>& p)
	{
		SharedPtr temp(p);
		swap(temp);
	}
	
	void swap(SharedPtr& other)
	{
		TStoragePolicy::swap(other);
	}

	TPointer get() const
	{
		return TStoragePolicy::pointer();
	}
	TPointer operator->() const
	{
		LASS_ASSERT(get());
		return get();
	}
	TReference operator*() const
	{
		LASS_ASSERT(get());
		return *get();
	}
	TReference operator[](size_t i) const
	{
		LASS_ASSERT(get());
		return TStoragePolicy::at(i);
	}

	const TCount count() const
	{
		return isEmpty() ? 0 : TCounterPolicy::count(TStoragePolicy::storage());
	}
	const bool isEmpty() const
	{
		return TStoragePolicy::isNull();
	}
	const bool operator!() const
	{
		return isEmpty();
	}
	operator num::SafeBool() const
	{
		return !isEmpty() ? num::safeTrue : num::safeFalse;
	}

	/** returns reference to storage pointer */
	const TStorage& storage() const
	{
		return TStoragePolicy::storage();
	}
	/** returns reference to storage pointer */
	TStorage& storage()
	{
		return TStoragePolicy::storage();
	}
};

}
}

#endif

// EOF
