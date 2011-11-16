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

#include <sys/atomic.h>

namespace lass
{
namespace util
{
namespace impl
{

template <typename UintSingle, typename UintDouble, typename T1, typename T2>
UintDouble packPair(T1 a, T2 b)
{
	LASS_META_ASSERT(sizeof(UintDouble) == 2 * sizeof(UintSingle), UintDouble_must_be_twice_as_wide_as_UintSingle);
	LASS_META_ASSERT(sizeof(T1) == sizeof(UintSingle), T1_must_have_same_wide_as_UintSingle);
	LASS_META_ASSERT(sizeof(T2) == sizeof(UintSingle), T2_must_have_same_wide_as_UintSingle);
	enum { shift = bitsPerByte * sizeof(UintSingle) };
	const UintDouble r1 = *reinterpret_cast<UintSingle*>(&a);
	const UintDouble r2 = *reinterpret_cast<UintSingle*>(&b);
#if LASS_HAVE_BIG_ENDIAN
	return (r1 << shift) | r2;
#else
	return (r2 << shift) | r1;
#endif
}

template <>
struct AtomicOperations<1>
{
	template <typename T> inline 
	static T LASS_CALL compareAndSwap(volatile T& dest, T expectedValue, T newValue)
	{
		return atomic_cas_8(
			reinterpret_cast<volatile uint8_t*>(&dest),
			*reinterpret_cast<uint8_t*>(&expectedValue),
			*reinterpret_cast<uint8_t*>(&newValue));
	}

	template <typename T1, typename T2> inline 
	static bool LASS_CALL compareAndSwap(
			volatile T1& dest1, T1 expected1, T2 expected2, T1 new1, T2 new2)
	{
		const uint16_t expected = packPair<uint8_t, uint16_t>(expected1, expected2);
		return atomic_cas_16(
			reinterpret_cast<volatile uint16_t*>(&dest1),
			expected, 
			packPair<uint8_t, uint16_t>(new1, new2)) == expected;
	}

	template <typename T> inline
	static void LASS_CALL increment(volatile T& value)
	{
		atomic_inc_8(reinterpret_cast<volatile uint8_t*>(&value));
	}

	template <typename T> inline
	static void LASS_CALL decrement(volatile T& value)
	{
		atomic_dec_8(reinterpret_cast<volatile uint8_t*>(&value));
	}
};



template <>
struct AtomicOperations<2>
{
	template <typename T> inline 
	static T LASS_CALL compareAndSwap(volatile T& dest, T expectedValue, T newValue)
	{
		return atomic_cas_16(
			reinterpret_cast<volatile uint16_t*>(&dest),
			*reinterpret_cast<uint16_t*>(&expectedValue), 
			*reinterpret_cast<uint16_t*>(&newValue));
	}

	template <typename T1, typename T2> inline 
	static bool LASS_CALL compareAndSwap(
			volatile T1& dest1, T1 expected1, T2 expected2, T1 new1, T2 new2)
	{
		const uint32_t expected = packPair<uint16_t, uint32_t>(expected1, expected2);
		return atomic_cas_32(
			reinterpret_cast<volatile uint32_t*>(&dest1),
			expected, 
			packPair<uint16_t, uint32_t>(new1, new2)) == expected;
	}

	template <typename T> inline
	static void LASS_CALL increment(volatile T& value)
	{
		atomic_inc_16(reinterpret_cast<volatile uint16_t*>(&value));
	}

	template <typename T> inline
	static void LASS_CALL decrement(volatile T& value)
	{
		atomic_dec_16(reinterpret_cast<volatile uint16_t*>(&value));
	}
};



template <>
struct AtomicOperations<4>
{
	template <typename T> inline 
	static T LASS_CALL compareAndSwap(volatile T& dest, T expectedValue, T newValue)
	{
		return atomic_cas_32(
			reinterpret_cast<volatile uint32_t*>(&dest),
			*reinterpret_cast<uint32_t*>(&expectedValue), 
			*reinterpret_cast<uint32_t*>(&newValue));
	}

	template <typename T1, typename T2> inline 
	static bool LASS_CALL compareAndSwap(
			volatile T1& dest1, T1 expected1, T2 expected2, T1 new1, T2 new2)
	{
		const uint64_t expected = packPair<uint32_t, uint64_t>(expected1, expected2);
		return atomic_cas_64(
			reinterpret_cast<volatile uint64_t*>(&dest1),
			expected, 
			packPair<uint32_t, uint64_t>(new1, new2)) == expected;
	}

	template <typename T> inline
	static void LASS_CALL increment(volatile T& value)
	{
		atomic_inc_32(reinterpret_cast<volatile uint32_t*>(&value));
	}

	template <typename T> inline
	static void LASS_CALL decrement(volatile T& value)
	{
		atomic_dec_32(reinterpret_cast<volatile uint32_t*>(&value));
	}
};



template <>
struct AtomicOperations<8>
{
	template <typename T> inline 
	static T LASS_CALL compareAndSwap(volatile T& dest, T expectedValue, T newValue)
	{
		return atomic_cas_64(
			reinterpret_cast<volatile uint64_t*>(&dest),
			*reinterpret_cast<uint64_t*>(&expectedValue),
			*reinterpret_cast<uint64_t*>(&newValue));
	}
	
	template <typename T> inline
	static void LASS_CALL increment(volatile T& value)
	{
		atomic_inc_64(reinterpret_cast<volatile uint64_t*>(&value));
	}

	template <typename T> inline
	static void LASS_CALL decrement(volatile T& value)
	{
		atomic_dec_64(reinterpret_cast<volatile uint64_t*>(&value));
	}
};

}
}
}

// EOF
