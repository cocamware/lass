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
 *	Copyright (C) 2004-2011 the Initial Developer.
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

#ifndef LASS_GUARDIAN_OF_INCLUSION_UTIL_SCOPED_PTR_H
#define LASS_GUARDIAN_OF_INCLUSION_UTIL_SCOPED_PTR_H

#include "util_common.h"
#include "non_copyable.h"
#include "smart_ptr_policies.h"
#include "../num/safe_bool.h"

#if LASS_COMPILER_TYPE == LASS_COMPILER_TYPE_MSVC
#	pragma warning(push)
#	pragma warning(disable: 4996) // 'deprecated-declaration': deprecation-message (or "was declared deprecated")
#else
#	pragma GCC diagnostic push
#	pragma GCC diagnostic ignored "-Wdeprecated-declarations"
#endif

namespace lass
{
namespace util
{

template
<
	typename T,
	template <typename, typename> class StoragePolicy = ObjectStorage
>
class LASS_DEPRECATED("use std::unique_ptr instead") ScopedPtr: public StoragePolicy<T, NonCopyable>
{
public:

	typedef T TPointee;
	typedef StoragePolicy<T, NonCopyable> TStoragePolicy;
	typedef typename TStoragePolicy::TStorage TStorage;
	typedef typename TStoragePolicy::TPointer TPointer;
	typedef typename TStoragePolicy::TReference TReference;

	template <typename U> struct Rebind
	{
		typedef ScopedPtr<U, StoragePolicy> Type;
	};

	ScopedPtr(): 
		TStoragePolicy() 
	{
	}
	explicit ScopedPtr(TPointer p): 
		TStoragePolicy(p) 
	{
	}
	~ScopedPtr() 
	{ 
		TStoragePolicy::dispose(); 
	}

	void reset(TPointer p = 0)
	{
		ScopedPtr temp(p);
		swap(temp);
	}

	template <typename U> ScopedPtr(std::unique_ptr<U> p):
		TStoragePolicy(p.get()) 
	{ 
		p.release(); 
	}
	template <typename U> void reset(std::unique_ptr<U> p)
	{
		ScopedPtr temp(std::move(p));
		swap(temp);
	}

	void swap(ScopedPtr& other)
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

	bool isEmpty() const
	{
		return TStoragePolicy::isNull();
	}
	bool operator!() const
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

#if LASS_COMPILER_TYPE == LASS_COMPILER_TYPE_MSVC
#	pragma warning(pop)
#else
#	pragma GCC diagnostic pop
#endif

}
}

#endif

// EOF
