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

namespace lass
{
namespace util
{
namespace impl
{

template <>
struct AtomicOperations<1>
{
	template <typename T> inline 
	static bool LASS_CALL compareAndSwap(volatile T& dest, T expectedValue, T newValue)
	{
		bool result;
		__asm__ __volatile__(
			"lock; cmpxchgb %2, %0;"
			"sete %1;"
			: "=m"(dest), "=q"(result)
			: "q"(newValue), "a"(expectedValue), "m"(dest)
			: "cc", "memory");
		return result;
	}

	template <typename T1, typename T2> inline 
	static bool LASS_CALL compareAndSwap(
			volatile T1& dest1, T1 expected1, T2 expected2, T1 new1, T2 new2)
	{
		bool result;
#if defined(__PIC__) && LASS_ADDRESS_SIZE == 32
		__asm__ __volatile__(
			"pushl %%ebx;"
			"movb %4, %%ah;"
			"movb %%cl, %%dh;"
			"lock; cmpxchgw %%dx, %0;"
			"sete %1;"
			"popl %%ebx;"
			: "=m"(dest1), "=q"(result)
			: "a"(expected1), "d"(expected2), "m"(new1), "c"(new2), "m"(dest1)
			: "cc", "memory");
#else
		__asm__ __volatile__(
			"movb %%bl, %%ah;"
			"movb %%cl, %%dh;"
			"lock; cmpxchgw %%dx, %0;"
			"sete %1;"
			: "=m"(reinterpret_cast<volatile num::Tuint16&>(dest1)), "=q"(result)
			: "a"(expected1), "d"(new1), "b"(expected2), "c"(new2), 
			  "m"(reinterpret_cast<volatile num::Tuint16&>(dest1))
			: "cc", "memory");
#endif
		return result;
	}

	template <typename T> inline
	static void LASS_CALL increment(volatile T& value)
	{
		__asm__ __volatile__(
			"lock; incb %0;"
			: "=m"(value)
			: "m"(value)
			: "cc", "memory");
	}

	template <typename T> inline
	static void LASS_CALL decrement(volatile T& value)
	{
		__asm__ __volatile__(
			"lock; decb %0;"
			: "=m"(value)
			: "m"(value)
			: "cc", "memory");
	}
};



template <>
struct AtomicOperations<2>
{
	template <typename T> inline 
	static bool LASS_CALL compareAndSwap(volatile T& dest, T expectedValue, T newValue)
	{
		bool result;
		__asm__ __volatile__(
			"lock; cmpxchgw %2, %0;"
			"sete %1;"
			: "=m"(dest), "=q"(result)
			: "q"(newValue), "a"(expectedValue), "m"(dest)
			: "cc", "memory");
		return result;
	}

	template <typename T1, typename T2> inline 
	static bool LASS_CALL compareAndSwap(
			volatile T1& dest1, T1 expected1, T2 expected2, T1 new1, T2 new2)
	{
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
	}

	template <typename T> inline
	static void LASS_CALL increment(volatile T& value)
	{
		__asm__ __volatile__(
			"lock; incw %0;"
			: "=m"(value)
			: "m"(value)
			: "cc", "memory");
	}

	template <typename T> inline
	static void LASS_CALL decrement(volatile T& value)
	{
		__asm__ __volatile__(
			"lock; decw %0;"
			: "=m"(value)
			: "m"(value)
			: "cc", "memory");
	}
};



template <>
struct AtomicOperations<4>
{
	template <typename T> inline 
	static bool LASS_CALL compareAndSwap(volatile T& dest, T expectedValue, T newValue)
	{
		bool result;
		__asm__ __volatile__(
			"lock; cmpxchgl %2, %0;"
			"sete %1;"
			: "=m"(dest), "=q"(result)
			: "q"(newValue), "a"(expectedValue), "m"(dest)
			: "cc", "memory");
		return result;
	}

	template <typename T1, typename T2> inline 
	static bool LASS_CALL compareAndSwap(
			volatile T1& dest1, T1 expected1, T2 expected2, T1 new1, T2 new2)
	{
		bool result;

		// cmpxchg8b and PIC mode don't play nice.  Push ebx before use!
		// see http://www.technovelty.org/code/arch/pic-cas.html
		//
#if defined(__PIC__) && LASS_ADDRESS_SIZE == 32
		__asm__ __volatile__(
			"pushl %%ebx;"
			"movl %4,%%ebx;"
			"lock; cmpxchg8b %0;"
			"sete %1;"
			"popl %%ebx;"
			: "=m"(dest1), "=q"(result)
			: "a"(expected1), "d"(expected2), "m"(new1), "c"(new2), "m"(dest1)
			: "cc", "memory");
#else
		__asm__ __volatile__(
			"lock; cmpxchg8b %0;"
			"sete %1;"
			: "=m"(dest1), "=q"(result)
			: "a"(expected1), "d"(expected2), "b"(new1), "c"(new2), "m"(dest1)
			: "cc", "memory");
#endif
		return result;
	}

	template <typename T> inline
	static void LASS_CALL increment(volatile T& value)
	{
		__asm__ __volatile__(
			"lock; incl %0;"
			: "=m"(value)
			: "m"(value)
			: "cc", "memory");
	}

	template <typename T> inline
	static void LASS_CALL decrement(volatile T& value)
	{
		__asm__ __volatile__(
			"lock; decl %0;"
			: "=m"(value)
			: "m"(value)
			: "cc", "memory");
	}
};



template <>
struct AtomicOperations<8>
{
	template <typename T> inline 
	static bool LASS_CALL compareAndSwap(volatile T& dest, T expectedValue, T newValue)
	{
		bool result;
#if LASS_ADDRESS_SIZE == 32
		// cmpxchg8b and PIC mode don't play nice.  Push ebx before use!
		// see http://www.technovelty.org/code/arch/pic-cas.html
		//
#	ifdef __PIC__
		__asm__ __volatile__(
			"pushl %%ebx;"
			"movl (%%ecx),%%ebx;"
			"movl 4(%%ecx),%%ecx;"
			"lock; cmpxchg8b %0;"
			"sete %1;"
			"pop %%ebx;"
			: "=m"(dest), "=q"(result)
			: "m"(dest),
			  "a"(reinterpret_cast<volatile num::Tuint32*>((void*)&expectedValue)[0]), 
			  "d"(reinterpret_cast<volatile num::Tuint32*>((void*)&expectedValue)[1]),	
			  "c"(&newValue)
			: "cc", "memory");
#	else
		__asm__ __volatile__(
			"lock; cmpxchg8b %0;"
			"sete %1;"
			: "=m"(dest), "=q"(result)
			: "m"(dest),
			  "a"(reinterpret_cast<volatile num::Tuint32*>((void*)&expectedValue)[0]), 
			  "d"(reinterpret_cast<volatile num::Tuint32*>((void*)&expectedValue)[1]),	
			  "b"(reinterpret_cast<volatile num::Tuint32*>((void*)&newValue)[0]), 
			  "c"(reinterpret_cast<volatile num::Tuint32*>((void*)&newValue)[1])	
			: "cc", "memory");
#	endif
#else
		__asm__ __volatile__(
			"lock; cmpxchgq %2, %0;"
			"sete %1;"
			: "=m"(dest), "=q"(result)
			: "q"(newValue), "a"(expectedValue), "m"(dest)
			: "cc", "memory");
#endif
		return result;
	}

#if LASS_ADDRESS_SIZE != 32
	template <typename T1, typename T2> inline 
	static bool LASS_CALL compareAndSwap(
			volatile T1& dest1, T1 expected1, T2 expected2, T1 new1, T2 new2)
	{
		LASS_ASSERT((reinterpret_cast<num::Tuint64>(&dest1) & 0xf) == 0); // dest needs to be 16-byte aligned.
		bool result;
		__asm__ __volatile__(
			"lock; cmpxchg16b %0;"
			"sete %1;"
			: "=m"(dest1), "=q"(result)
			: "a"(expected1), "d"(expected2), "b"(new1), "c"(new2), "m"(dest1)
			: "cc", "memory");
		return result;
	}
#endif
	
	template <typename T> inline
	static void LASS_CALL increment(volatile T& value)
	{
#if LASS_ADDRESS_SIZE == 32
		// not knowing any better, we emulate incq with cas loop
		T old, fresh;
		do
		{
			old = value;
			fresh = old + 1;
		}
		while (!atomicCompareAndSwap(value, old, fresh));		
#else
		__asm__ __volatile__(
			"lock; incq %0;"
			: "=m"(value)
			: "m"(value)
			: "cc", "memory");
#endif		
	}

	template <typename T> inline
	static void LASS_CALL decrement(volatile T& value)
	{
#if LASS_ADDRESS_SIZE == 32
		// not knowing any better, we emulate decq with cas loop
		T old, fresh;
		do
		{
			old = value;
			fresh = old - 1;
		}
		while (!atomicCompareAndSwap(value, old, fresh));
#else
		__asm__ __volatile__(
			"lock; decq %0;"
			: "=m"(value)
			: "m"(value)
			: "cc", "memory");
#endif
	}
};

}
}
}

// EOF
