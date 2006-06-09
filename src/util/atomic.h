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
#include "../num/basic_types.h"

#if LASS_COMPILER_TYPE == LASS_COMPILER_TYPE_MSVC || LASS_COMPILER_TYPE == LASS_COMPILER_TYPE_INTEL
#	if defined(_M_IA64)
#		define LASS_UTIL_ATOMIC_UNSUPPORTED
#	else
#		define LASS_UTIL_ATOMIC_MSVC
#		define LASS_UTIL_ATOMIC_32
#	endif
#elif LASS_COMPILER_TYPE == LASS_COMPILER_TYPE_GCC
#	if defined(_LP64) || defined(__LP64__)
#		define LASS_UTIL_ATOMIC_GCC
#		define LASS_UTIL_ATOMIC_64
#	else
#		define LASS_UTIL_ATOMIC_UNSUPPORTED
#	endif
#else
#	define LASS_UTIL_ATOMIC_UNSUPPORTED
#endif

#ifdef LASS_UTIL_ATOMIC_UNSUPPORTED
#	error [LASS BUILD MSG] lass/util/atomic.h not yet supported for your compiler!
#endif

#if LASS_COMPILER_TYPE == LASS_COMPILER_TYPE_MSVC
#	pragma warning(push)
#	pragma warning(disable: 4035)
#endif

namespace lass
{
namespace util
{

namespace impl
{

template <int byteSize> struct AtomicOperations;

template <>
struct AtomicOperations<1>
{
	template <typename T> inline 
	static T LASS_CALL compareAndSwap(T& dest, T expectedValue, T newValue)
	{
#if defined(LASS_UTIL_ATOMIC_MSVC) and defined(LASS_UTIL_ATOMIC_32)
		T* const addr = &dest;
		__asm 
		{
			mov cl, newValue
			mov al, expectedValue
			mov edx, addr
			lock cmpxchg [edx], cl
		}
		/* return eax */
#elif defined(LASS_UTIL_ATOMIC_GCC)
		__asm__ __volatile__(
			"lock; cmpxchgb %2, %0;"
			: "=m"(dest), "=a"(expectedValue)
			: "q"(newValue), "1"(expectedValue));
		return expectedValue;
#else
#	error [LASS BUILD MSG] lass/util/atomic.h: missing implementation
#endif
	}

	template <typename T> inline
	static void LASS_CALL increment(T& value)
	{
#if defined(LASS_UTIL_ATOMIC_GCC)
		__asm__ __volatile__(
			"lock; incb %0;"
			: "=m"(value)
			: "m"(value)
			: "memory");
#else
#	error [LASS BUILD MSG] lass/util/atomic.h: missing implementation
#endif		
	}

	template <typename T> inline
	static void LASS_CALL decrement(T& value)
	{
#if defined(LASS_UTIL_ATOMIC_GCC)
		__asm__ __volatile__(
			"lock; decb %0;"
			: "=m"(value)
			: "m"(value)
			: "memory");
#else
#	error [LASS BUILD MSG] lass/util/atomic.h: missing implementation
#endif
	}
};




template <>
struct AtomicOperations<2>
{
	template <typename T> inline 
	static T LASS_CALL compareAndSwap(T& dest, T expectedValue, T newValue)
	{
#if defined(LASS_UTIL_ATOMIC_MSVC) and defined(LASS_UTIL_ATOMIC_32)
		T* const addr = &dest;
		__asm 
		{
			mov cx, newValue
			mov ax, expectedValue
			mov edx, addr
			lock cmpxchg [edx], cx
		}
		/* return eax */
#elif defined(LASS_UTIL_ATOMIC_GCC)
		__asm__ __volatile__(
			"lock; cmpxchgw %2, %0;"
			: "=m"(dest), "=a"(expectedValue)
			: "q"(newValue), "1"(expectedValue));
		return expectedValue;
#else
#	error [LASS BUILD MSG] lass/util/atomic.h: missing implementation
#endif
	}

	template <typename T> inline
	static void LASS_CALL increment(T& value)
	{
#if defined(LASS_UTIL_ATOMIC_GCC)
		__asm__ __volatile__(
			"lock; incw %0;"
			: "=m"(value)
			: "m"(value)
			: "memory");
#else
#	error [LASS BUILD MSG] lass/util/atomic.h: missing implementation
#endif		
	}

	template <typename T> inline
	static void LASS_CALL decrement(T& value)
	{
#if defined(LASS_UTIL_ATOMIC_GCC)
		__asm__ __volatile__(
			"lock; decw %0;"
			: "=m"(value)
			: "m"(value)
			: "memory");
#else
#	error [LASS BUILD MSG] lass/util/atomic.h: missing implementation
#endif
	}
};




template <>
struct AtomicOperations<4>
{
	template <typename T> inline 
	static T LASS_CALL compareAndSwap(T& dest, T expectedValue, T newValue)
	{
#if defined(LASS_UTIL_ATOMIC_MSVC) and defined(LASS_UTIL_ATOMIC_32)
		T* const addr = &dest;
		__asm 
		{
			mov ecx, newValue
			mov eax, expectedValue
			mov edx, addr
			lock cmpxchg [edx], ecx
		}
		/* return eax */
#elif defined(LASS_UTIL_ATOMIC_GCC)
		__asm__ __volatile__(
			"lock; cmpxchgl %2, %0;"
			: "=m"(dest), "=a"(expectedValue)
			: "q"(newValue), "1"(expectedValue));
		return expectedValue;
#else
#	error [LASS BUILD MSG] lass/util/atomic.h: missing implementation
#endif
	}

	template <typename T> inline
	static void LASS_CALL increment(T& value)
	{
#if defined(LASS_UTIL_ATOMIC_MSVC) and defined(LASS_UTIL_ATOMIC_32)
		T* const addr = &value;
		__asm 
		{
			mov edx, addr
			lock inc [edx]
		}
#elif defined(LASS_UTIL_ATOMIC_GCC)
		__asm__ __volatile__(
			"lock; incl %0;"
			: "=m"(value)
			: "m"(value)
			: "memory");
#else
#	error [LASS BUILD MSG] lass/util/atomic.h: missing implementation
#endif		
	}

	template <typename T> inline
	static void LASS_CALL decrement(T& value)
	{
#if defined(LASS_UTIL_ATOMIC_MSVC) and defined(LASS_UTIL_ATOMIC_32)
		T* const addr = &value;
		__asm 
		{
			mov edx, addr
			lock dec [edx]
		}
#elif defined(LASS_UTIL_ATOMIC_GCC)
		__asm__ __volatile__(
			"lock; decl %0;"
			: "=m"(value)
			: "m"(value)
			: "memory");
#else
#	error [LASS BUILD MSG] lass/util/atomic.h: missing implementation
#endif
	}
};

}



template <typename T> inline 
bool atomicCompareAndSwap(T& dest, T expectedValue, T newValue)
{
	return impl::AtomicOperations< sizeof(T) >::compareAndSwap(dest, expectedValue, newValue) 
		== expectedValue;
}

template <typename T> inline
void atomicIncrement(T& value)
{
	impl::AtomicOperations< sizeof(T) >::increment(value);
}

template <typename T> inline
void atomicDecrement(T& value)
{
	impl::AtomicOperations< sizeof(T) >::decrement(value);
}
}

}

#if LASS_COMPILER_TYPE == LASS_COMPILER_TYPE_MSVC
#	pragma warning(pop)
#	pragma warning(disable: 4035)
#endif

#endif

// EOF
