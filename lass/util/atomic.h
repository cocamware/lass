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
 *	Copyright (C) 2004-2022 the Initial Developer.
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

#ifndef LASS_GUARDIAN_OF_INCLUSION_UTIL_ATOMIC_H
#define LASS_GUARDIAN_OF_INCLUSION_UTIL_ATOMIC_H

#include "util_common.h"
#include "impl/atomic_impl.h"

#include <atomic>

#ifdef LASS_PROCESSOR_ARCHITECTURE_x86
#	if LASS_COMPILER_TYPE == LASS_COMPILER_TYPE_MSVC
#		include <intrin.h>
#		define LASS_SPIN_PAUSE _mm_pause()
#	else
#		define LASS_SPIN_PAUSE __builtin_ia32_pause()
#	endif
#else
#	define LASS_SPIN_PAUSE
#endif

#ifdef __cpp_lib_hardware_interference_size
#define LASS_LOCK_FREE_ALIGNMENT std::hardware_destructive_interference_size
#else
#define LASS_LOCK_FREE_ALIGNMENT 64
#endif

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
 *
 *  @deprecated Use std::atomic
 */
template <typename T>
[[deprecated("Use std::atomic")]]
bool atomicCompareAndSwap(volatile T& dest, T expectedValue, T newValue)
{
	return impl::AtomicOperations< sizeof(T) >::compareAndSwap(dest, expectedValue, newValue);
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
 *
 *  @deprecated Use std::atomic
 */
template <typename T1, typename T2>
[[deprecated("Use std::atomic")]]
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
 *
 *  @deprecated Use std::atomic
 */
template <typename T>
[[deprecated("Use std::atomic")]]
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
 *
 *  @deprecated Use std::atomic
 */
template <typename T>
[[deprecated("Use std::atomic")]]
void atomicDecrement(volatile T& value)
{
	impl::AtomicOperations< sizeof(T) >::decrement(value);
}



/** @ingroup atomic
 *
 *  @deprecated Use std::atomic
 */
template <typename T>
[[deprecated("Use std::atomic")]]
void atomicLock(volatile T& semaphore)
{
	T current;
	do
	{
		current = semaphore;
	}
	while (current <= 0 || !atomicCompareAndSwap(semaphore, current, current - 1));
}



/** @ingroup atomic
 */
template <typename T>
void atomicLock(std::atomic<T>& semaphore)
{
	// the load on failure (or when current==0) may be relaxed because
	// memory order is not about the atomic variable itself. It's about
	// loads and stores on _other_ locations, and there are none of these
	// inside the loop. https://stackoverflow.com/q/58635264
	T current = semaphore.load(std::memory_order_acquire);
	do
	{
		while (!current)
		{
			LASS_SPIN_PAUSE;
			current = semaphore.load(std::memory_order_relaxed);
		}
		LASS_ASSERT(current >= 0);
	}
	while (!semaphore.compare_exchange_weak(current, current - 1,
		std::memory_order_release, std::memory_order_relaxed));
}



/** @ingroup atomic
 *
 *  @deprecated Use std::atomic
 */
template <typename T>
[[deprecated("Use std::atomic")]]
bool atomicTryLock(volatile T& semaphore)
{
	T current;
	do
	{
		current = semaphore;
		if (current <= 0)
		{
			return false;
		}
	}
	while (!atomicCompareAndSwap(semaphore, current, current - 1));
	return true;
}



/** @ingroup atomic
 */
template <typename T>
bool atomicTryLock(std::atomic<T>& semaphore)
{
	// the load on failure (or when current==0) may be relaxed because
	// memory order is not about the atomic variable itself. It's about
	// loads and stores on _other_ locations, and there are none of these
	// inside the loop. https://stackoverflow.com/q/58635264
	T current = semaphore.load(std::memory_order_acquire);
	do
	{
		LASS_ASSERT(current >= 0);
		if (!current)
		{
			return false;
		}
	}
	while (!semaphore.compare_exchange_weak(current, current - 1,
		std::memory_order_release, std::memory_order_relaxed));
	return true;
}



/** @ingroup atomic
 *
 *  @deprecated Use std::atomic
 */
template <typename T>
[[deprecated("Use std::atomic")]]
void atomicUnlock(volatile T& semaphore)
{
	atomicIncrement(semaphore);
}



/** @ingroup atomic
 */
template <typename T>
void atomicUnlock(std::atomic<T>& semaphore)
{
	semaphore.fetch_add(1, std::memory_order_acq_rel);
}


#if LASS_ADDRESS_SIZE == 64
#	if LASS_HAVE_STD_ATOMIC_DWCAS_LOCK_FREE
		// std::atomic is lock free for 16 byte PODs, and will use lock cmpxchg16b
		// (or _InterlockedCompareExchange128) for it's compare_exchange (DWCAS).
		// This requires TaggedPtr to be 16-byte aligned
#		define LASS_TAGGED_PTR_ALIGN alignas(16)
#	elif defined(LASS_PROCESSOR_ARCHITECTURE_x86)
		// std::atomic is *not* lock free for 16 byte PODs, because it will not use
		// lock cmpxchg16b, even though it may commonly exists for this architecture.
		// This is to remain ABI compatible with early 64-bit AMD processors that
		// lacked this instruction.
		// Therefore, we need to use the good old pointers-are-actually-only-48-bits
		// trick to pack a pointer and a 16-bit tag in the space of one 8 byte pointer,
		// for which std::atomic will be lock free.
#		define LASS_TAGGED_PTR_64_PACKED 1
#		define LASS_TAGGED_PTR_ALIGN alignas(8)
#	else
#		error not implemented yet
#	endif
#else
#	define LASS_TAGGED_PTR_ALIGN alignas(8)
#endif

/** Pointer with a tag for ABA salvation
 *  @ingroup atomic
 *  Some lock-free algorithms suffer from the ABA problem when acting on pointers.
 *  This can be solved (read: be made very unlikely) by adding a tag to the pointer.
 */
template <typename T>
class LASS_TAGGED_PTR_ALIGN TaggedPtr
{
#if LASS_TAGGED_PTR_64_PACKED

public:
	typedef num::Tuint16 TTag;
	TaggedPtr() = default;
	TaggedPtr(T * ptr, TTag tag): bits_((reinterpret_cast<num::Tint64>(ptr) << 16) | (tag & 0xffff)) {}
	T* get() const
	{
#	if defined(LASS_HAVE_INLINE_ASSEMBLY_GCC)
		T* ptr;
		__asm__ ("sarq $16, %0;" : "=q"(ptr) : "0"(bits_) : "cc");
		return ptr;
#	elif LASS_COMPILER_TYPE == LASS_COMPILER_TYPE_MSVC
		return reinterpret_cast<T*>(__ll_rshift(bits_, 16));
#	else
		return ((bits_ & 0xa000000000000000) == 0) ?
			reinterpret_cast<T*>(bits_ >> 16) :
			reinterpret_cast<T*>((bits_ >> 16) | 0xffff000000000000);
#	endif
	}
	TTag tag() const { return static_cast<TTag>(bits_ & 0xffff); }
	TTag nextTag() const { return static_cast<TTag>(static_cast<size_t>(tag() + 1) & 0xffff); }
	bool operator==(const TaggedPtr& other) const { return bits_ == other.bits_; }
private:
	num::Tint64 bits_ = 0;

#else

public:
	typedef num::TuintPtr TTag;
	TaggedPtr() = default;
	TaggedPtr(T* ptr, TTag tag) : ptr_(ptr), tag_(tag) {}
	T* get() const { return ptr_; }
	TTag tag() const { return tag_; }
	TTag nextTag() const { return tag_ + 1; }
	bool operator==(const TaggedPtr& other) const { return ptr_ == other.ptr_ && tag_ == other.tag_; }
private:
	T* ptr_ = nullptr;
	TTag tag_ = 0;

#endif

public:
	T* operator->() const { LASS_ASSERT(get()); return get(); }
	bool operator!() const { return get() == nullptr; }
	explicit operator bool() const { return get() != nullptr; }
	bool operator!=(const TaggedPtr& other) const { return !(&this == other); }
};


}
}

#endif

// EOF
