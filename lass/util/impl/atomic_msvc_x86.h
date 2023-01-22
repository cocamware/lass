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
 *	Copyright (C) 2004-2023 the Initial Developer.
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

#pragma warning(push)
#pragma warning(disable: 4035)

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
		__asm 
		{
			mov al, expectedValue
			mov dl, newValue
			mov edi, dest
			lock cmpxchg [edi], dl
			mov eax, 0
			sete al
		}
		/* return eax */
	}

	template <typename T1, typename T2> inline 
	static bool LASS_CALL compareAndSwap(
			volatile T1& dest1, T1 expected1, T2 expected2, T1 new1, T2 new2)
	{
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
	}

	template <typename T> inline
	static void LASS_CALL increment(volatile T& value)
	{
		__asm 
		{
			mov edi, value
			lock inc byte ptr [edi]
		}
	}

	template <typename T> inline
	static void LASS_CALL decrement(volatile T& value)
	{
		__asm 
		{
			mov edi, value
			lock dec byte ptr [edi]
		}
	}
};



template <>
struct AtomicOperations<2>
{
	template <typename T> inline 
	static bool LASS_CALL compareAndSwap(volatile T& dest, T expectedValue, T newValue)
	{
		__asm 
		{
			mov ax, expectedValue
			mov dx, newValue
			mov edi, dest
			lock cmpxchg [edi], dx
			mov eax, 0
			sete al
		}
		/* return eax */
	}

	template <typename T1, typename T2> inline 
	static bool LASS_CALL compareAndSwap(
			volatile T1& dest1, T1 expected1, T2 expected2, T1 new1, T2 new2)
	{
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
	}

	template <typename T> inline
	static void LASS_CALL increment(volatile T& value)
	{
		__asm 
		{
			mov edi, value
			lock inc word ptr [edi]
		}
	}

	template <typename T> inline
	static void LASS_CALL decrement(volatile T& value)
	{
		__asm 
		{
			mov edi, value
			lock dec word ptr [edi]
		}
	}
};



template <>
struct AtomicOperations<4>
{
	template <typename T> inline 
	static bool LASS_CALL compareAndSwap(volatile T& dest, T expectedValue, T newValue)
	{
		__asm 
		{
			mov eax, expectedValue
			mov edx, newValue
			mov edi, dest
			lock cmpxchg [edi], edx
			mov eax, 0
			sete al
		}
		/* return eax */
	}

	template <typename T1, typename T2> inline 
	static bool LASS_CALL compareAndSwap(
			volatile T1& dest1, T1 expected1, T2 expected2, T1 new1, T2 new2)
	{
		// If you ever read this, because MSVC starts complaining with warnings like:
		//
		// frame pointer register 'ebx' modified by inline assembly code,
		//
		// then know we're doing okay. 
		// We're pushing and popping ebx but msvc doesn't realize it. Poor thing.
		// Normally, you don't need to preserve ebx, except if you need dynamic stack-alignment
		// code, and you're not doing frame pointer optimzation. 
		//
		// Anyway, rest asure we heeded the warning, but we could not silence it. 
		// Not here anyway.
		//
		__asm 
		{
			push ebx
			mov eax, expected1
			mov edx, expected2
			mov ebx, new1
			mov ecx, new2
			mov edi, dest1
			lock cmpxchg8b [edi]
			mov eax, 0
			sete al
			pop ebx
		}
		/* return eax */
	}

	template <typename T> inline
	static void LASS_CALL increment(volatile T& value)
	{
		__asm 
		{
			mov edi, value
			lock inc dword ptr [edi]
		}
	}

	template <typename T> inline
	static void LASS_CALL decrement(volatile T& value)
	{
		__asm 
		{
			mov edi, value
			lock dec dword ptr [edi]
		}
	}
};



template <>
struct AtomicOperations<8>
{
	template <typename T> inline 
	static bool LASS_CALL compareAndSwap(volatile T& dest, T expectedValue, T newValue)
	{
		// If you ever read this, because MSVC starts complaining with warnings like:
		//
		// frame pointer register 'ebx' modified by inline assembly code,
		//
		// then know we're doing okay. 
		// We're pushing and popping ebx but msvc doesn't realize it. Poor thing.
		// Normally, you don't need to preserve ebx, except if you need dynamic stack-alignment
		// code, and you're not doing frame pointer optimzation. 
		//
		// Anyway, rest asure we heeded the warning, but we could not silence it. 
		// Not here anyway.
		//
		__asm 
		{
			push ebx
			lea esi, expectedValue
			lea edi, newValue
			mov eax, [esi]
			mov edx, 4[esi]
			mov ebx, [edi]
			mov ecx, 4[edi]
			mov edi, dest
			lock cmpxchg8b [edi]
			mov eax, 0
			sete al
			pop ebx
		}
		/* return edx:eax */
	}

	template <typename T> inline
	static void LASS_CALL increment(volatile T& value)
	{
		// not knowing any better, we emulate incq with cas loop
		T old, fresh;
		do
		{
			old = value;
			fresh = old + 1;
		}
		while (!AtomicOperations<8>::compareAndSwap(value, old, fresh));
	}

	template <typename T> inline
	static void LASS_CALL decrement(volatile T& value)
	{
		// not knowing any better, we emulate decq with cas loop
		T old, fresh;
		do
		{
			old = value;
			fresh = old - 1;
		}
		while (!AtomicOperations<8>::compareAndSwap(value, old, fresh));
	}
};

}
}
}

#pragma warning(pop)

// EOF
