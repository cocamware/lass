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



/** @ingroup atomic
 */
template <typename T>
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
void atomicUnlock(volatile T& semaphore)
{
	atomicIncrement(semaphore);
}



#if LASS_COMPILER_TYPE == LASS_COMPILER_TYPE_MSVC
#	pragma warning(push)
#	pragma warning(disable: 4521) // multiple copy constructors specified
#	pragma warning(disable: 4522) // multiple assignment operators specified
#endif

#if LASS_ADDRESS_SIZE == 64
	// lock cmpxchg16b (or _InterlockedCompareExchange128) require the address to be 16-byte aligned
	// as this one is going to be used to atomicly compare-and-swap two TaggedPtrs, we need to use a bit of dynamic stack-alignment.
#	if LASS_COMPILER_TYPE == LASS_COMPILER_TYPE_MSVC
#		define LASS_TAGGED_PTR_ALIGN __declspec(align(16))
#	else
#		define LASS_TAGGED_PTR_ALIGN __attribute__ ((__aligned__ (16)))
#	endif
#elif defined(LASS_PROCESSOR_ARCHITECTURE_ARM)
#	define LASS_TAGGED_PTR_ALIGN __attribute__ ((__aligned__ (8)))
#else
	// Not for 32-bit. 
	// Technically, we don't need it. lock cmpxchg8b should be ok without. Though you may get a performance hit.
	// If we would use it though, msvc starts complaining about ebx being used in assembly blocks 
	// (it needs that to do the dynamic stack-alignment)
	// That's still okay, as we push and pop ebx, but msvc doesn't realize, so it keeps complaining.
	// And there's no easy way to silence it either. It's not enough to pragma warning disable it around the assembly code.
	// Let's avoid tons of warnings by simply not doing dynamic stack-alignment.
#	define LASS_TAGGED_PTR_ALIGN
#endif

/** Pointer with a tag for ABA salvation
 *  @ingroup atomic
 *  Some lock-free algorithms suffer from the ABA problem when acting on pointers.
 *  This can be solved (read: be make very unlikely) by adding a tag to the pointer.
 */
template <typename T>
class LASS_TAGGED_PTR_ALIGN TaggedPtr
{
public:
	typedef num::TuintPtr TTag;
	TaggedPtr(): ptr_(0), tag_(0) {}
	TaggedPtr(T* ptr, TTag tag): ptr_(ptr), tag_(tag) {}
	TaggedPtr(const TaggedPtr& other): ptr_(other.ptr_), tag_(other.tag_) {}
	TaggedPtr(const volatile TaggedPtr& other): ptr_(other.ptr_), tag_(other.tag_) {}
	TaggedPtr& operator=(const TaggedPtr& other) { ptr_ = other.ptr_; tag_ = other.tag_; return *this; }
	TaggedPtr& operator=(const volatile TaggedPtr& other) { ptr_ = other.ptr_; tag_ = other.tag_; return *this; }
	T* get() const { return ptr_; }
	TTag tag() const { return tag_; }
	bool operator==(const TaggedPtr& other) const { return ptr_ == other.ptr_ && tag_ == other.tag_; }
	bool operator==(const volatile TaggedPtr& other) const { return ptr_ == other.ptr_ && tag_ == other.tag_; }
	bool atomicCompareAndSwap(const TaggedPtr& expected, const TaggedPtr& fresh) volatile
	{
		return util::atomicCompareAndSwap(
			ptr_, expected.ptr_, expected.tag_, fresh.ptr_, fresh.tag_);
	}
private:
	T* ptr_;
	TTag tag_;
public:
	T* operator->() const { LASS_ASSERT(get()); return get(); }
	bool operator!() const { return get() == 0; }
	operator num::SafeBool() const { return get() ? num::safeTrue : num::safeFalse; }
};

#if LASS_COMPILER_TYPE == LASS_COMPILER_TYPE_MSVC
#	pragma warning(pop)
#endif

}
}

#endif

// EOF
