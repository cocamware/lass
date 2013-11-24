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

#include <intrin.h>
#pragma intrinsic(_InterlockedCompareExchange)
#pragma intrinsic(_InterlockedCompareExchange16)
#pragma intrinsic(_InterlockedCompareExchange64)
#if LASS_COMPILER_VERSION >= 1500
#	pragma intrinsic(_InterlockedCompareExchange128)
#endif
#pragma intrinsic(_InterlockedIncrement)
#pragma intrinsic(_InterlockedIncrement16)
#pragma intrinsic(_InterlockedIncrement64)
#pragma intrinsic(_InterlockedDecrement)
#pragma intrinsic(_InterlockedDecrement16)
#pragma intrinsic(_InterlockedDecrement64)
#pragma intrinsic(__ll_rshift)

extern "C"
{
	bool lass_cas8(volatile lass::num::Tuint8*, lass::num::Tuint8, lass::num::Tuint8);
	void lass_inc8(volatile lass::num::Tuint8*);
	void lass_dec8(volatile lass::num::Tuint8*);
	bool lass_dcas8(volatile lass::num::Tuint8*, lass::num::Tuint8, lass::num::Tuint8, lass::num::Tuint8, lass::num::Tuint8);
	bool lass_dcas16(volatile lass::num::Tuint16*, lass::num::Tuint16, lass::num::Tuint16, lass::num::Tuint16, lass::num::Tuint16);
	bool lass_dcas32(volatile lass::num::Tuint32*, lass::num::Tuint32, lass::num::Tuint32, lass::num::Tuint32, lass::num::Tuint32);
	bool lass_dcas64(volatile lass::num::Tuint64*, lass::num::Tuint64, lass::num::Tuint64, lass::num::Tuint64, lass::num::Tuint64);
}

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
		return lass_cas8(reinterpret_cast<volatile num::Tuint8*>(&dest), 
			*reinterpret_cast<num::Tuint8*>(&newValue), *reinterpret_cast<num::Tuint8*>(&expectedValue));
	}

	template <typename T1, typename T2> inline 
	static bool LASS_CALL compareAndSwap(
			volatile T1& dest1, T1 expected1, T2 expected2, T1 new1, T2 new2)
	{
		return lass_dcas8(
			reinterpret_cast<volatile num::Tuint8*>(&dest1), 
			*reinterpret_cast<num::Tuint8*>(&new1), *reinterpret_cast<num::Tuint8*>(&new2), 
			*reinterpret_cast<num::Tuint8*>(&expected1), *reinterpret_cast<num::Tuint8*>(&expected2));
	}

	template <typename T> inline
	static void LASS_CALL increment(volatile T& value)
	{
		lass_inc8(reinterpret_cast<volatile num::Tuint8*>(&value));
	}

	template <typename T> inline
	static void LASS_CALL decrement(volatile T& value)
	{
		lass_dec8(reinterpret_cast<volatile num::Tuint8*>(&value));
	}
};



template <>
struct AtomicOperations<2>
{
	template <typename T> inline 
	static bool LASS_CALL compareAndSwap(volatile T& dest, T expectedValue, T newValue)
	{
		const short expected = *reinterpret_cast<short*>(&expectedValue);
		return _InterlockedCompareExchange16(reinterpret_cast<volatile short*>(&dest), 
			*reinterpret_cast<short*>(&newValue), expected) == expected;
	}

	template <typename T1, typename T2> inline 
	static bool LASS_CALL compareAndSwap(
			volatile T1& dest1, T1 expected1, T2 expected2, T1 new1, T2 new2)
	{
		return lass_dcas16(
			reinterpret_cast<volatile num::Tuint16*>(&dest1), 
			*reinterpret_cast<num::Tuint16*>(&new1), *reinterpret_cast<num::Tuint16*>(&new2), 
			*reinterpret_cast<num::Tuint16*>(&expected1), *reinterpret_cast<num::Tuint16*>(&expected2));
	}

	template <typename T> inline
	static void LASS_CALL increment(volatile T& value)
	{
		_InterlockedIncrement16(reinterpret_cast<volatile short*>(&value));
	}

	template <typename T> inline
	static void LASS_CALL decrement(volatile T& value)
	{
		_InterlockedDecrement16(reinterpret_cast<volatile short*>(&value));
	}
};



template <>
struct AtomicOperations<4>
{
	template <typename T> inline 
	static bool LASS_CALL compareAndSwap(volatile T& dest, T expectedValue, T newValue)
	{
		const long expected = *reinterpret_cast<long*>(&expectedValue);
		return _InterlockedCompareExchange(reinterpret_cast<volatile long*>(&dest),
			*reinterpret_cast<long*>(&newValue), expected) == expected;
	}

	template <typename T1, typename T2> inline 
	static bool LASS_CALL compareAndSwap(
			volatile T1& dest1, T1 expected1, T2 expected2, T1 new1, T2 new2)
	{
		return lass_dcas32(
			reinterpret_cast<volatile num::Tuint32*>(&dest1), 
			*reinterpret_cast<num::Tuint32*>(&new1), *reinterpret_cast<num::Tuint32*>(&new2), 
			*reinterpret_cast<num::Tuint32*>(&expected1), *reinterpret_cast<num::Tuint32*>(&expected2));
	}

	template <typename T> inline
	static void LASS_CALL increment(volatile T& value)
	{
		_InterlockedIncrement(reinterpret_cast<volatile long*>(&value));
	}

	template <typename T> inline
	static void LASS_CALL decrement(volatile T& value)
	{
		_InterlockedDecrement(reinterpret_cast<volatile long*>(&value));
	}
};



template <>
struct AtomicOperations<8>
{
	template <typename T> inline 
	static bool LASS_CALL compareAndSwap(volatile T& dest, T expectedValue, T newValue)
	{
		const __int64 expected = *reinterpret_cast<__int64*>(&expectedValue);
		return _InterlockedCompareExchange64(reinterpret_cast<volatile __int64*>(&dest), 
			*reinterpret_cast<__int64*>(&newValue), expected) == expected;
	}

	template <typename T1, typename T2> inline 
	static bool LASS_CALL compareAndSwap(
			volatile T1& dest1, T1 expected1, T2 expected2, T1 new1, T2 new2)
	{
		LASS_ASSERT((reinterpret_cast<num::Tuint64>(&dest1) & 0xf) == 0); // dest needs to be 16-byte aligned.
#if LASS_COMPILER_VERSION >= 1500
		__int64 expected[2] = { *reinterpret_cast<__int64*>(&expected1), *reinterpret_cast<__int64*>(&expected2) };
		return _InterlockedCompareExchange128(
			reinterpret_cast<volatile __int64*>(&dest1), 
			*reinterpret_cast<__int64*>(&new2), *reinterpret_cast<__int64*>(&new1), 
			expected) != 0;
#else
		return lass_dcas64(
			reinterpret_cast<volatile num::Tuint64*>(&dest1), 
			*reinterpret_cast<num::Tuint64*>(&new1), *reinterpret_cast<num::Tuint64*>(&new2), 
			*reinterpret_cast<num::Tuint64*>(&expected1), *reinterpret_cast<num::Tuint64*>(&expected2));
#endif
	}
	
	template <typename T> inline
	static void LASS_CALL increment(volatile T& value)
	{
		_InterlockedIncrement64(reinterpret_cast<volatile __int64*>(&value));
	}

	template <typename T> inline
	static void LASS_CALL decrement(volatile T& value)
	{
		_InterlockedDecrement64(reinterpret_cast<volatile __int64*>(&value));
	}
};



}
}
}

// EOF
