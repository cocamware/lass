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

#ifndef LASS_GUARDIAN_OF_INCLUSION_UTIL_ATOMIC_H
#define LASS_GUARDIAN_OF_INCLUSION_UTIL_ATOMIC_H

#include "util_common.h"
#include "impl/atomic_impl.h"
#include "../num/safe_bool.h"

/** @defgroup atomic
 *  @brief atomic operations and tools for lock-free algorithms
 */

namespace lass
{
namespace util
{

/** @ingroup atomic
 *  
 *  Performs the following pseudocode in an atomic way (no other threads can intervene):
 *  @code
 *      if (dest != expectedValue) return false;
 *      dest = newValue;
 *      return true;
 *  @endcode
 */
template <typename T> inline 
bool atomicCompareAndSwap(volatile T& dest, T expectedValue, T newValue)
{
	return impl::AtomicOperations< sizeof(T) >::compareAndSwap(dest, expectedValue, newValue) 
		== expectedValue;
}



/** @ingroup atomic
 *  
 *  Performs the following pseudocode in an atomic way (no other threads can intervene):
 *  @code
 *      if (dest1 != expectedValue1 || *(T2*)(&dest1 + sizeof(T1)) != expected2) return false;
 *      dest1 = new1;
 *      dest2 = new2;
 *      return true;
 *  @endcode
 *
 *  Does not exist for 64-bit types (there's no 128-bit CAS).
 */
template <typename T1, typename T2> inline 
bool atomicCompareAndSwap(volatile T1& dest1, T1 expected1, T2 expected2, T1 new1, T2 new2)
{
	LASS_META_ASSERT(sizeof(T1) == sizeof(T2), T1_and_T2_must_be_of_same_size);
	return impl::AtomicOperations< sizeof(T1) >::compareAndSwap(
		dest1, expected1, expected2, new1, new2);
}



/** @ingroup atomic
 *  
 *  Performs the following pseudocode in an atomic way (no other threads can intervene):
 *  @code
 *      ++value;
 *  @endcode
 */
template <typename T> inline
void atomicIncrement(volatile T& value)
{
	impl::AtomicOperations< sizeof(T) >::increment(value);
}



/** @ingroup atomic
 *  
 *  Performs the following pseudocode in an atomic way (no other threads can intervene):
 *  @code
 *      --value;
 *  @endcode
 */
template <typename T> inline
void atomicDecrement(volatile T& value)
{
	impl::AtomicOperations< sizeof(T) >::decrement(value);
}



#if LASS_COMPILER_TYPE == LASS_COMPILER_TYPE_MSVC
#	pragma warning(push)
#	pragma warning(disable: 4521) // multiple copy constructors specified
#	pragma warning(disable: 4522) // multiple assignment operators specified
#endif

/** Pointer with a tag for ABA salvation
 *  @ingroup atomic
 *  Some lock-free algorithms suffer from the ABA problem when acting on pointers.
 *  This can be solved (read: be make very unlikely) by adding a tag to the pointer.
 */
template <typename T>
class TaggedPtr
{
public:
#if LASS_ACTUAL_ADDRESS_SIZE == 48
	// We're in 64 bit space, but we don't have 128 bit CAS to do tagging ... Help!
	// Luckely, only the least significant 48 bits of a pointer are really used.
	// So, we have 16 bits we can fiddle with.  Should be enough for a counting tag.
	// Well, theoretically it isn't enough (no number of bits is ever enough to 
	// completely remove the possibility of a wrap-over, but at least it decreases
	// the odds somewhat ;)
	//
	// We store the the pointer in the most significant part for two reasons:
	//	- order of the pointers is somewhat preserved
	//	- we can use SAR to restore the sign bit.
	//
	typedef num::Tuint16 TTag;
	TaggedPtr(): bits_(0) {}
	TaggedPtr(T* ptr, TTag tag): bits_((reinterpret_cast<num::Tuint64>(ptr) << 16) | tag) {}
	TaggedPtr(const TaggedPtr& other): bits_(other.bits_) {}
	TaggedPtr(const volatile TaggedPtr& other): bits_(other.bits_) {}
	volatile TaggedPtr& operator=(const TaggedPtr& other) volatile { bits_ = other.bits_; return *this; }
	volatile TaggedPtr& operator=(const volatile TaggedPtr& other) volatile { bits_ = other.bits_; return *this; }
	T* const get() const volatile
	{
#	if defined(LASS_HAVE_INLINE_ASSEMBLY_GCC)
		T* ptr;
		__asm__ __volatile__("sarq $16, %0;" : "=q"(ptr) : "0"(bits_) : "cc");
		return ptr;
#	elif defined(LASS_HAVE_INLINE_ASSEMBLY_MSVC)
#		error Not implemented yet ...
#	else
		return (bits_ & 0xa000000000000000 == 0) ?
			reinterpret_cast<T*>(bits_ >> 16) :
			reinterpret_cast<T*>((bits_ >> 16) | 0xffff000000000000);
#	endif
	}
	const TTag tag() const volatile { return static_cast<TTag>(bits_ & 0xffff); }
	const bool operator==(const volatile TaggedPtr& other) const volatile { return bits_ == other.bits_; }
	bool atomicCompareAndSwap(const TaggedPtr& expected, const TaggedPtr& fresh) volatile
	{
		return util::atomicCompareAndSwap(bits_, expected.bits_, fresh.bits_);
	}
private:
	num::Tuint64 bits_;
#else
	// We're in 32 bit space, so we use a 64 bit CAS to do tagging
	//
	typedef num::TuintPtr TTag;
	TaggedPtr(): ptr_(0), tag_(0) {}
	TaggedPtr(T* ptr, TTag tag): ptr_(ptr), tag_(tag) {}
	TaggedPtr(const TaggedPtr& other): ptr_(other.ptr_), tag_(other.tag_) {}
	TaggedPtr(const volatile TaggedPtr& other): ptr_(other.ptr_), tag_(other.tag_) {}
	volatile TaggedPtr& operator=(const TaggedPtr& other) volatile { ptr_ = other.ptr_; tag_ = other.tag_; return *this; }
	volatile TaggedPtr& operator=(const volatile TaggedPtr& other) volatile { ptr_ = other.ptr_; tag_ = other.tag_; return *this; }
	T* const get() const volatile { return ptr_; }
	const TTag tag() const volatile { return tag_; }
	bool operator==(const volatile TaggedPtr& other) const volatile { return ptr_ == other.ptr_ && tag_ == other.tag_; }
	bool atomicCompareAndSwap(const TaggedPtr& expected, const TaggedPtr& fresh) volatile
	{
		return util::atomicCompareAndSwap(
			ptr_, expected.ptr_, expected.tag_, fresh.ptr_, fresh.tag_);
	}
private:
	T* ptr_;
	TTag tag_;
#endif
public:
	T* const operator->() const volatile { LASS_ASSERT(get()); return get(); }
	const bool operator!() const volatile { return get() == 0; }
	operator num::SafeBool() const volatile { return get() ? num::safeTrue : num::safeFalse; }
};

#if LASS_COMPILER_TYPE == LASS_COMPILER_TYPE_MSVC
#	pragma warning(pop)
#endif

}
}

#endif

// EOF
