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

#include "../../num/basic_types.h"

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

/** @internal 
 */
template <int byteSize> struct AtomicOperations;

template <>
struct AtomicOperations<1>
{
	template <typename T> inline 
	static T LASS_CALL compareAndSwap(T& dest, T expectedValue, T newValue)
	{
#if defined(LASS_HAVE_INLINE_ASSEMBLY_MSVC) && (LASS_ADDRESS_SIZE == 32)
		__asm 
		{
			mov al, expectedValue
			mov dl, newValue
			mov edi, dest
			lock cmpxchg [edi], dl
		}
		/* return eax */
#elif defined(LASS_HAVE_INLINE_ASSEMBLY_GCC)
		__asm__ __volatile__(
			"lock; cmpxchgb %2, %0;"
			: "=m"(dest), "=a"(expectedValue)
			: "q"(newValue), "1"(expectedValue), "m"(dest)
			: "cc", "memory");
		return expectedValue;
#else
#	error [LASS BUILD MSG] lass/util/impl/atomic_impl.h: missing implementation
#endif
	}

	template <typename T1, typename T2> inline 
	static bool LASS_CALL compareAndSwap(
			T1& dest1, T1 expected1, T2 expected2, T1 new1, T2 new2)
	{
#if defined(LASS_HAVE_INLINE_ASSEMBLY_MSVC) && (LASS_ADDRESS_SIZE == 32)
		__asm 
		{
			mov al, expected1
			mov dl, new1
			mov ah, expected2
			mov dh, new2
			mov edi, dest1
			lock cmpxchg [edi], dx
			mov eax, 0
			sete al
		}
		/* return eax */
#elif defined(LASS_HAVE_INLINE_ASSEMBLY_GCC)
		bool result;
		__asm__ __volatile__(
			"movb %4, %%ah;"
			"movb %5, %%dh;"
			"lock; cmpxchgw %%dx, %0;"
			"sete %1;"
			: "=m"(reinterpret_cast<num::Tuint16&>(dest1)), "=q"(result)
			: "a"(expected1), "d"(new1), "g"(expected2), "g"(new2), 
			  "m"(reinterpret_cast<num::Tuint16&>(dest1))
			: "cc", "memory");
		return result;		
#else
#	error [LASS BUILD MSG] lass/util/impl/atomic_impl.h: missing implementation
#endif
	}

	template <typename T> inline
	static void LASS_CALL increment(T& value)
	{
#if defined(LASS_HAVE_INLINE_ASSEMBLY_MSVC) && (LASS_ADDRESS_SIZE == 32)
		__asm 
		{
			mov edi, value
			lock inc byte ptr [edi]
		}
#elif defined(LASS_HAVE_INLINE_ASSEMBLY_GCC)
		__asm__ __volatile__(
			"lock; incb %0;"
			: "=m"(value)
			: "m"(value)
			: "cc", "memory");
#else
#	error [LASS BUILD MSG] lass/util/impl/atomic_impl.h: missing implementation
#endif		
	}

	template <typename T> inline
	static void LASS_CALL decrement(T& value)
	{
#if defined(LASS_HAVE_INLINE_ASSEMBLY_MSVC) && (LASS_ADDRESS_SIZE == 32)
		__asm 
		{
			mov edi, value
			lock dec byte ptr [edi]
		}
#elif defined(LASS_HAVE_INLINE_ASSEMBLY_GCC)
		__asm__ __volatile__(
			"lock; decb %0;"
			: "=m"(value)
			: "m"(value)
			: "cc", "memory");
#else
#	error [LASS BUILD MSG] lass/util/impl/atomic_impl.h: missing implementation
#endif
	}
};




// -------------------------------------------------------------------------------------------------

template <>
struct AtomicOperations<2>
{
	template <typename T> inline 
	static T LASS_CALL compareAndSwap(T& dest, T expectedValue, T newValue)
	{
#if defined(LASS_HAVE_INLINE_ASSEMBLY_MSVC) && (LASS_ADDRESS_SIZE == 32)
		__asm 
		{
			mov ax, expectedValue
			mov dx, newValue
			mov edi, dest
			lock cmpxchg [edi], dx
		}
		/* return eax */
#elif defined(LASS_HAVE_INLINE_ASSEMBLY_GCC)
		__asm__ __volatile__(
			"lock; cmpxchgw %2, %0;"
			: "=m"(dest), "=a"(expectedValue)
			: "q"(newValue), "1"(expectedValue), "m"(dest)
			: "cc", "memory");
		return expectedValue;
#else
#	error [LASS BUILD MSG] lass/util/impl/atomic_impl.h: missing implementation
#endif
	}

	template <typename T1, typename T2> inline 
	static bool LASS_CALL compareAndSwap(
			T1& dest1, T1 expected1, T2 expected2, T1 new1, T2 new2)
	{
#if defined(LASS_HAVE_INLINE_ASSEMBLY_MSVC) && (LASS_ADDRESS_SIZE == 32)
		__asm 
		{
			mov ax, expected2
			mov dx, new2
			shl eax, 16
			shl edx, 16
			mov ax, expected1
			mov dx, new1
			mov edi, dest1
			lock cmpxchg [edi], edx
			mov eax, 0
			sete al
		}
		/* return eax */
#elif defined(LASS_HAVE_INLINE_ASSEMBLY_GCC)
		bool result;
		__asm__ __volatile__(
			"shll $16, %%eax;"
			"shll $16, %%edx;"
			"movw %4, %%ax;"
			"movw %5, %%dx;"
			"lock; cmpxchgl %%edx, %0;"
			"sete %1;"
			: "=m"(reinterpret_cast<num::Tuint32&>(dest1)), "=q"(result)
			: "a"(expected2), "d"(new2), "g"(expected1), "g"(new1),
			  "m"(reinterpret_cast<num::Tuint32&>(dest1))
			: "cc", "memory");
		return result;		
#else
#	error [LASS BUILD MSG] lass/util/impl/atomic_impl.h: missing implementation
#endif
	}

	template <typename T> inline
	static void LASS_CALL increment(T& value)
	{
#if defined(LASS_HAVE_INLINE_ASSEMBLY_MSVC) && (LASS_ADDRESS_SIZE == 32)
		__asm 
		{
			mov edi, value
			lock inc word ptr [edi]
		}
#elif defined(LASS_HAVE_INLINE_ASSEMBLY_GCC)
		__asm__ __volatile__(
			"lock; incw %0;"
			: "=m"(value)
			: "m"(value)
			: "cc", "memory");
#else
#	error [LASS BUILD MSG] lass/util/impl/atomic_impl.h: missing implementation
#endif		
	}

	template <typename T> inline
	static void LASS_CALL decrement(T& value)
	{
#if defined(LASS_HAVE_INLINE_ASSEMBLY_MSVC) && (LASS_ADDRESS_SIZE == 32)
		__asm 
		{
			mov edi, value
			lock dec word ptr [edi]
		}
#elif defined(LASS_HAVE_INLINE_ASSEMBLY_GCC)
		__asm__ __volatile__(
			"lock; decw %0;"
			: "=m"(value)
			: "m"(value)
			: "cc", "memory");
#else
#	error [LASS BUILD MSG] lass/util/impl/atomic_impl.h: missing implementation
#endif
	}
};



// -------------------------------------------------------------------------------------------------

template <>
struct AtomicOperations<4>
{
	template <typename T> inline 
	static T LASS_CALL compareAndSwap(T& dest, T expectedValue, T newValue)
	{
#if defined(LASS_HAVE_INLINE_ASSEMBLY_MSVC) && (LASS_ADDRESS_SIZE == 32)
		__asm 
		{
			mov eax, expectedValue
			mov edx, newValue
			mov edi, dest
			lock cmpxchg [edi], edx
		}
		/* return eax */
#elif defined(LASS_HAVE_INLINE_ASSEMBLY_GCC)
		__asm__ __volatile__(
			"lock; cmpxchgl %2, %0;"
			: "=m"(dest), "=a"(expectedValue)
			: "q"(newValue), "1"(expectedValue), "m"(dest)
			: "cc", "memory");
		return expectedValue;
#else
#	error [LASS BUILD MSG] lass/util/impl/atomic_impl.h: missing implementation
#endif
	}

	template <typename T1, typename T2> inline 
	static bool LASS_CALL compareAndSwap(
			T1& dest1, T1 expected1, T2 expected2, T1 new1, T2 new2)
	{
#if defined(LASS_HAVE_INLINE_ASSEMBLY_MSVC) && (LASS_ADDRESS_SIZE == 32)
		__asm 
		{
			mov eax, expected1
			mov edx, expected2
			mov ebx, new1
			mov ecx, new2
			mov edi, dest1
			lock cmpxchg8b [edi]
			mov eax, 0
			sete al
		}
		/* return eax */
#elif defined(LASS_HAVE_INLINE_ASSEMBLY_GCC)
		bool result;
		__asm__ __volatile__(
			"lock; cmpxchg8b %0;"
			"sete %1;"
			: "=m"(dest1), "=q"(result)
			: "a"(expected1), "d"(expected2), "b"(new1), "c"(new2), "m"(dest1)
			: "cc", "memory");
		return result;
#else
#	error [LASS BUILD MSG] lass/util/impl/atomic_impl.h: missing implementation
#endif
	}

	template <typename T> inline
	static void LASS_CALL increment(T& value)
	{
#if defined(LASS_HAVE_INLINE_ASSEMBLY_MSVC) && (LASS_ADDRESS_SIZE == 32)
		__asm 
		{
			mov edi, value
			lock inc dword ptr [edi]
		}
#elif defined(LASS_HAVE_INLINE_ASSEMBLY_GCC)
		__asm__ __volatile__(
			"lock; incl %0;"
			: "=m"(value)
			: "m"(value)
			: "cc", "memory");
#else
#	error [LASS BUILD MSG] lass/util/impl/atomic_impl.h: missing implementation
#endif		
	}

	template <typename T> inline
	static void LASS_CALL decrement(T& value)
	{
#if defined(LASS_HAVE_INLINE_ASSEMBLY_MSVC) && (LASS_ADDRESS_SIZE == 32)
		__asm 
		{
			mov edi, value
			lock dec dword ptr [edi]
		}
#elif defined(LASS_HAVE_INLINE_ASSEMBLY_GCC)
		__asm__ __volatile__(
			"lock; decl %0;"
			: "=m"(value)
			: "m"(value)
			: "cc", "memory");
#else
#	error [LASS BUILD MSG] lass/util/impl/atomic_impl.h: missing implementation
#endif
	}
};



// -------------------------------------------------------------------------------------------------

template <>
struct AtomicOperations<8>
{
	template <typename T> inline 
	static T LASS_CALL compareAndSwap(T& dest, T expectedValue, T newValue)
	{
#if defined(LASS_HAVE_INLINE_ASSEMBLY_MSVC) && (LASS_ADDRESS_SIZE == 32)
		__asm 
		{
			lea esi, expectedValue
			lea edi, newValue
			mov eax, [esi]
			mov edx, 4[esi]
			mov ebx, [edi]
			mov ecx, 4[edi]
			mov edi, dest
			lock cmpxchg8b [edi]
		}
		/* return edx:eax */
#elif defined(LASS_HAVE_INLINE_ASSEMBLY_GCC) && (LASS_ADDRESS_SIZE == 32)
		__asm__ __volatile__(
			"lock; cmpxchg8b %0;"
			: "=m"(dest), "=A"(expectedValue)
			: "m"(dest),
			  "a"(reinterpret_cast<num::Tuint32*>(&expectedValue)[0]), 
			  "d"(reinterpret_cast<num::Tuint32*>(&expectedValue)[1]),	
			  "b"(reinterpret_cast<num::Tuint32*>(&newValue)[0]), 
			  "c"(reinterpret_cast<num::Tuint32*>(&newValue)[1])	
			: "cc", "memory");
		return expectedValue;
#elif defined(LASS_HAVE_INLINE_ASSEMBLY_GCC) && (LASS_ADDRESS_SIZE == 64)
		__asm__ __volatile__(
			"lock; cmpxchgq %2, %0;"
			: "=m"(dest), "=a"(expectedValue)
			: "q"(newValue), "1"(expectedValue), "m"(dest)
			: "cc", "memory");
		return expectedValue;
#else
#	error [LASS BUILD MSG] lass/util/impl/atomic_impl.h: missing implementation
#endif
	}

	// we assume we don't have a cmpxchg16b
	//
	//template <typename T1, typename T2> inline 
	//static bool LASS_CALL compareAndSwap(
	//		T1& dest1, T1 expected1, T2 expected2, T1 new1, T2 new2);
	
	template <typename T> inline
	static void LASS_CALL increment(T& value)
	{
#if (LASS_ADDRESS_SIZE == 32)
		// not knowing any better, we emulate incq with cas loop
		T old, fresh;
		do
		{
			old = value;
			fresh = old + 1;
		}
		while (!atomicCompareAndSwap(value, old, fresh));		
#elif (LASS_ADDRESS_SIZE == 64) && defined(LASS_HAVE_INLINE_ASSEMBLY_GCC)
		__asm__ __volatile__(
			"lock; incq %0;"
			: "=m"(value)
			: "m"(value)
			: "cc", "memory");
#else
#	error [LASS BUILD MSG] lass/util/impl/atomic_impl.h: missing implementation
#endif		
	}

	template <typename T> inline
	static void LASS_CALL decrement(T& value)
	{
#if (LASS_ADDRESS_SIZE == 32)
		// not knowing any better, we emulate decq with cas loop
		T old, fresh;
		do
		{
			old = value;
			fresh = old - 1;
		}
		while (!atomicCompareAndSwap(value, old, fresh));		
#elif (LASS_ADDRESS_SIZE == 64) && defined(LASS_HAVE_INLINE_ASSEMBLY_GCC)
		__asm__ __volatile__(
			"lock; decq %0;"
			: "=m"(value)
			: "m"(value)
			: "cc", "memory");
#else
#	error [LASS BUILD MSG] lass/util/impl/atomic_impl.h: missing implementation
#endif
	}
};



}
}
}

// EOF
