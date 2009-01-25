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
	TaggedPtr& operator=(const TaggedPtr& other) { bits_ = other.bits_; return *this; }
	TaggedPtr& operator=(const volatile TaggedPtr& other) { bits_ = other.bits_; return *this; }
	T* const get() const
	{
#	if defined(LASS_HAVE_INLINE_ASSEMBLY_GCC)
		T* ptr;
		__asm__ __volatile__("sarq $16, %0;" : "=q"(ptr) : "0"(bits_) : "cc");
		return ptr;
#	elif defined(LASS_UTIL_IMPL_ATOMIC_MSVC_X64)
		return reinterpret_cast<T*>(__ll_rshift(*reinterpret_cast<const volatile __int64*>(&bits_), 16));
#	else
		return (bits_ & 0xa000000000000000 == 0) ?
			reinterpret_cast<T*>(bits_ >> 16) :
			reinterpret_cast<T*>((bits_ >> 16) | 0xffff000000000000);
#	endif
	}
	const TTag tag() const { return static_cast<TTag>(bits_ & 0xffff); }
	const bool operator==(const TaggedPtr& other) const { return bits_ == other.bits_; }
	const bool operator==(const volatile TaggedPtr& other) const { return bits_ == other.bits_; }
	bool atomicCompareAndSwap(const TaggedPtr& expected, const TaggedPtr& fresh) volatile
	{
		return util::atomicCompareAndSwap(bits_, expected.bits_, fresh.bits_);
	}
private:
	num::Tuint64 bits_;
#elif LASS_ACTUAL_ADDRESS_SIZE == 32
	// We're in 32 bit space, so we use a 64 bit CAS to do tagging
	//
	typedef num::TuintPtr TTag;
	TaggedPtr(): ptr_(0), tag_(0) {}
	TaggedPtr(T* ptr, TTag tag): ptr_(ptr), tag_(tag) {}
	TaggedPtr(const TaggedPtr& other): ptr_(other.ptr_), tag_(other.tag_) {}
	TaggedPtr(const volatile TaggedPtr& other): ptr_(other.ptr_), tag_(other.tag_) {}
	TaggedPtr& operator=(const TaggedPtr& other) { ptr_ = other.ptr_; tag_ = other.tag_; return *this; }
	TaggedPtr& operator=(const volatile TaggedPtr& other) { ptr_ = other.ptr_; tag_ = other.tag_; return *this; }
	T* const get() const { return ptr_; }
	const TTag tag() const { return tag_; }
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
#else
#	error "not implemented yet [Bramz]"
#endif
public:
	T* const operator->() const { LASS_ASSERT(get()); return get(); }
	const bool operator!() const { return get() == 0; }
	operator num::SafeBool() const { return get() ? num::safeTrue : num::safeFalse; }
};

#if LASS_COMPILER_TYPE == LASS_COMPILER_TYPE_MSVC
#	pragma warning(pop)
#endif

}
}

#endif

// EOF
