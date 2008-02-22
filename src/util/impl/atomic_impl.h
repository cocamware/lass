/**	@file
 *	@author Bram de Greve (bramz@users.sourceforge.net)
 *	@author Tom De Muer (tomdemuer@users.sourceforge.net)
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
 *	Copyright (C) 2004-2007 the Initial Developer.
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

/** @ingroup atomic
 *  @internal 
 */
template <int byteSize> struct AtomicOperations;

/** @ingroup atomic
 *  @internal 
 */
template <>
struct AtomicOperations<1>
{
	template <typename T> inline 
	static T LASS_CALL compareAndSwap(volatile T& dest, T expectedValue, T newValue)
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
			volatile T1& dest1, T1 expected1, T2 expected2, T1 new1, T2 new2)
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
			: "=m"(reinterpret_cast<volatile num::Tuint16&>(dest1)), "=q"(result)
			: "a"(expected1), "d"(new1), "g"(expected2), "g"(new2), 
			  "m"(reinterpret_cast<volatile num::Tuint16&>(dest1))
			: "cc", "memory");
		return result;		
#else
#	error [LASS BUILD MSG] lass/util/impl/atomic_impl.h: missing implementation
#endif
	}

	template <typename T> inline
	static void LASS_CALL increment(volatile T& value)
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
	static void LASS_CALL decrement(volatile T& value)
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

/** @ingroup atomic
 *  @internal 
 */
template <>
struct AtomicOperations<2>
{
	template <typename T> inline 
	static T LASS_CALL compareAndSwap(volatile T& dest, T expectedValue, T newValue)
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
			volatile T1& dest1, T1 expected1, T2 expected2, T1 new1, T2 new2)
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
			: "=m"(reinterpret_cast<volatile num::Tuint32&>(dest1)), "=q"(result)
			: "a"(expected2), "d"(new2), "g"(expected1), "g"(new1),
			  "m"(reinterpret_cast<volatile num::Tuint32&>(dest1))
			: "cc", "memory");
		return result;		
#else
#	error [LASS BUILD MSG] lass/util/impl/atomic_impl.h: missing implementation
#endif
	}

	template <typename T> inline
	static void LASS_CALL increment(volatile T& value)
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
	static void LASS_CALL decrement(volatile T& value)
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

/** @ingroup atomic
 *  @internal 
 */
template <>
struct AtomicOperations<4>
{
	template <typename T> inline 
	static T LASS_CALL compareAndSwap(volatile T& dest, T expectedValue, T newValue)
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
			volatile T1& dest1, T1 expected1, T2 expected2, T1 new1, T2 new2)
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
	static void LASS_CALL increment(volatile T& value)
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
	static void LASS_CALL decrement(volatile T& value)
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

/** @ingroup atomic
 *  @internal 
 */
template <>
struct AtomicOperations<8>
{
	template <typename T> inline 
	static T LASS_CALL compareAndSwap(volatile T& dest, T expectedValue, T newValue)
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
			  "a"(reinterpret_cast<volatile num::Tuint32*>(&expectedValue)[0]), 
			  "d"(reinterpret_cast<volatile num::Tuint32*>(&expectedValue)[1]),	
			  "b"(reinterpret_cast<volatile num::Tuint32*>(&newValue)[0]), 
			  "c"(reinterpret_cast<volatile num::Tuint32*>(&newValue)[1])	
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
	static void LASS_CALL increment(volatile T& value)
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
	static void LASS_CALL decrement(volatile T& value)
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

#if LASS_COMPILER_TYPE == LASS_COMPILER_TYPE_MSVC
#	pragma warning(pop)
#endif

// EOF
