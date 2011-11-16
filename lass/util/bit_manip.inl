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



#ifndef LASS_GUARDIAN_OF_INCLUSION_UTIL_BIT_MANIP_INL
#define LASS_GUARDIAN_OF_INCLUSION_UTIL_BIT_MANIP_INL


#include "bit_manip.h"



namespace lass
{

namespace util
{

/** Set a bit high.
 *  @ingroup BitManip
 *  @author Bram de Greve [Bramz]
 *  @param a_bits the bitfield of which a bit must be set high.
 *  @param a_bit the index of the bit to be set high (0 => 0x1, 1 => 0x2, ...)
 */
template<typename T>
void setBit(T& a_bits, size_t a_bit)
{
	a_bits |= (T(1) << a_bit);
}



/** set a bit low.
 *  @ingroup BitManip
 *  @author Bram de Greve [Bramz]
 *  @param a_bits the bitfield of which a bit must be set low.
 *  @param a_bit the index of the bit to be set low (0 => 0x1, 1 => 0x2, ...)
 */
template<typename T>
void clearBit(T& a_bits, size_t a_bit)
{
	a_bits &= ~(T(1) << a_bit);
}



/** flip state of a bit (low->high, high->low).
 *  @ingroup BitManip
 *  @author Bram de Greve [Bramz]
 *  @param a_bits the bitfield of which a bit must be flipped.
 *  @param a_bit the index of the bit to be flipped (0 => 0x1, 1 => 0x2, ...)
 */
template<typename T>
void flipBit(T& a_bits, size_t a_bit)
{
	a_bits ^= (T(1) << a_bit);
}



/** set a bit high if (and only if) a condition is fullfilled.
 *  @ingroup BitManip
 *  @author Bram de Greve [Bramz]
 *  @param a_bits the bitfield of which a bit must be set high.
 *  @param a_bit the index of the bit to be set high (0 => 0x1, 1 => 0x2, ...)
 *  @param a_condition true if the bit must be set high, false if not.
 */
template<typename T>
void setBitIf(T& a_bits, size_t a_bit, bool a_condition)
{
	setMasked(a_bits, a_condition ? (T(1) << a_bit) : T(0));
}



/** set a bit low if (and only if) a condition is fullfilled is true.
 *  @ingroup BitManip
 *  @author Bram de Greve [Bramz]
 *  @param a_bits the bitfield of which a bit must be set low.
 *  @param a_bit the index of the bit to be set low (0 => 0x1, 1 => 0x2, ...)
 *  @param a_condition true if the bit must be set low, false if not.
 */
template<typename T>
void clearBitIf(T& a_bits, size_t a_bit, bool a_condition)
{
	clearMasked(a_bits, a_condition ? (T(1) << a_bit) : T(0));
}



/** flip a bit if (and only if) a condition is fullfilled (low->high, high->low).
 *  @ingroup BitManip
 *  @author Bram de Greve [Bramz]
 *  @param a_bits the bitfield of which a bit must be flipped.
 *  @param a_bit the index of the bit to be flipped (0 => 0x1, 1 => 0x2, ...)
 *  @param a_condition true if the bit must be flipped, false if not.
 */
template<typename T>
void flipBitIf(T& a_bits, size_t a_bit, bool a_condition)
{
	flipMasked(a_bits, a_condition ? (T(1) << a_bit) : T(0));
}



/** set a bit to a given state.
 *  @ingroup BitManip
 *  @author Bram de Greve [Bramz]
 *  @param a_bits the bitfield of which a bit must be set.
 *  @param a_bit the index of the bit to be set (0 => 0x1, 1 => 0x2, ...)
 *  @param a_state the state to which the bit must be set: true for high, false for low.
 */
template<typename T>
void setBitTo(T& a_bits, size_t a_bit, bool a_state)
{
	clearBit(a_bits, a_bit);
	setBitIf(a_bits, a_bit, a_state);
}



/** return true if a bit is set high.
 *  @ingroup BitManip
 *  @author Bram de Greve [Bramz]
 *  @param a_bits the bitfield of which a bit must be checked.
 *  @param a_bit the index of the bit to be checked (0 => 0x1, 1 => 0x2, ...)
 */
template<typename T>
bool checkBit(T a_bits, size_t a_bit)
{
	return (a_bits & (T(1) << a_bit)) != 0;
}




/** Set masked bits high.
 *  @ingroup BitManip
 *  @author Bram de Greve [Bramz]
 *  @param a_bits the bitfield of which some bits must be set high.
 *  @param a_mask mask indicating the bits that must be set high.
 *                The high bits in the mask indicate the bits in the bitfield to be set,
 *                the low bits in the mask indicate the bits in the bitfield to be left
 *                untouched.
 */
template<typename T>
void setMasked(T& a_bits, const T& a_mask)
{
	a_bits |= a_mask;
}



/** Set masked bits low.
 *  @ingroup BitManip
 *  @author Bram de Greve [Bramz]
 *  @param a_bits the bitfield of which some bits must be set low.
 *  @param a_mask mask indicating the bits that must be set low.
 *                The high bits in the mask indicate the bits in the bitfield to be set,
 *                the low bits in the mask indicate the bits in the bitfield to be left
 *                untouched.
 */
template<typename T>
void clearMasked(T& a_bits, const T& a_mask)
{
	a_bits &= ~a_mask;
}



/** flip masked bits.
 *  @ingroup BitManip
 *  @author Bram de Greve [Bramz]
 *  @param a_bits the bitfield of which some bits must be flipped.
 *  @param a_mask mask indicating the bits that must be flipped.
 *                The high bits in the mask indicate the bits in the bitfield to be flipped,
 *                the low bits in the mask indicate the bits in the bitfield to be left
 *                untouched.
 */
template<typename T>
void flipMasked(T& a_bits, const T& a_mask)
{
	a_bits ^= a_mask;
}



/** Set masked bits high if (and only if) a condition is fullfilled.
 *  @ingroup BitManip
 *  @author Bram de Greve [Bramz]
 *  @param a_bits the bitfield of which some bits must be set high.
 *  @param a_mask mask indicating the bits that must be set high.
 *                The high bits in the mask indicate the bits in the bitfield to be set,
 *                the low bits in the mask indicate the bits in the bitfield to be left
 *                untouched.
 *  @param a_condition true if the bits must be set, false if not.
 */
template<typename T>
void setMaskedIf(T& a_bits, const T& a_mask, bool a_condition)
{
	setMasked(a_bits, a_condition ? a_mask : T(0));
}



/** Set masked bits low if (and only if) a condition is fullfilled.
 *  @ingroup BitManip
 *  @author Bram de Greve [Bramz]
 *  @param a_bits the bitfield of which some bits must be set low.
 *  @param a_mask mask indicating the bits that must be set low.
 *                The high bits in the mask indicate the bits in the bitfield to be set,
 *                the low bits in the mask indicate the bits in the bitfield to be left
 *                untouched.
 *  @param a_condition true if the bits must be set, false if not.
 */
template<typename T>
void clearMaskedIf(T& a_bits, const T& a_mask, bool a_condition)
{
	clearMasked(a_bits, a_condition ? a_mask : T(0));
}



/** Flip the masked bits if (and only if) a condition is fullfilled.
 *  @ingroup BitManip
 *  @author Bram de Greve [Bramz]
 *  @param a_bits the bitfield of which some bits must be flipped.
 *  @param a_mask mask indicating the bits that must be flipped.
 *                The high bits in the mask indicate the bits in the bitfield to be flipped,
 *                the low bits in the mask indicate the bits in the bitfield to be left
 *                untouched.
 *  @param a_condition true if the bits must be flipped, false if not.
 */
template<typename T>
void flipMaskedIf(T& a_bits, const T& a_mask, bool a_condition)
{
	flipMasked(a_bits, a_condition ? a_mask : T(0));
}



/** Set the masked bits to a given state if (and only if) a condition is fullfilled.
 *  @ingroup BitManip
 *  @author Bram de Greve [Bramz]
 *  @param a_bits the bitfield of which some bits must be set.
 *  @param a_mask mask indicating the bits that must be set.
 *                The high bits in the mask indicate the bits in the bitfield to be set,
 *                the low bits in the mask indicate the bits in the bitfield to be left
 *                untouched.
 *  @param a_state the state to which the bits must be set: true for high, false for low.
 */
template<typename T>
void setMaskedTo(T& a_bits, const T& a_mask, bool a_state)
{
	clearMasked(a_bits, a_mask);
	setMaskedIf(a_bits, a_mask, a_state);
}



/** Check the masked bits and return true if they are ALL set.
 *  @ingroup BitManip
 *  @author Bram de Greve [Bramz]
 *  @param a_bits the bitfield of which some bits must be checked.
 *  @param a_mask mask indicating the bits that must be checked.
 *                The high bits in the mask indicate the bits in the bitfield to be checked,
 *                the low bits in the mask indicate the bits in the bitfield to be left
 *                untouched.
 *  @return true if ALL masked bits are set, false if not.  So it also returns if only
 *          a few of the masked bits are set, but not all of them.
 */
template<typename T>
bool checkMaskedAll(T a_bits, const T& a_mask)
{
	return (a_bits & a_mask) == a_mask;
}



/** Check the masked bits and return true if at least one is set.
 *  @ingroup BitManip
 *  @author Bram de Greve [Bramz]
 *  @param a_bits the bitfield of which some bits must be checked.
 *  @param a_mask mask indicating the bits that must be checked.
 *                The high bits in the mask indicate the bits in the bitfield to be checked,
 *                the low bits in the mask indicate the bits in the bitfield to be left
 *                untouched.
 *  @return true if at least one masked bits is set, false if not.
 */
template<typename T>
bool checkMaskedSome(T a_bits, const T& a_mask)
{
	return (a_bits & a_mask) != 0;
}



namespace impl
{
	/** lookup table of number of bits in a byte
	 *  @ingroup BitManip
	 *  @internal
	 */
	const size_t bitsInByte[256] = 
	{
		0, 1, 1, 2, 1, 2, 2, 3, 1, 2, 2, 3, 2, 3, 3, 4,
		1, 2, 2, 3, 2, 3, 3, 4, 2, 3, 3, 4, 3, 4, 4, 5,
		1, 2, 2, 3, 2, 3, 3, 4, 2, 3, 3, 4, 3, 4, 4, 5,
		2, 3, 3, 4, 3, 4, 4, 5, 3, 4, 4, 5, 4, 5, 5, 6,
		1, 2, 2, 3, 2, 3, 3, 4, 2, 3, 3, 4, 3, 4, 4, 5,
		2, 3, 3, 4, 3, 4, 4, 5, 3, 4, 4, 5, 4, 5, 5, 6,
		2, 3, 3, 4, 3, 4, 4, 5, 3, 4, 4, 5, 4, 5, 5, 6,
		3, 4, 4, 5, 4, 5, 5, 6, 4, 5, 5, 6, 5, 6, 6, 7,
		1, 2, 2, 3, 2, 3, 3, 4, 2, 3, 3, 4, 3, 4, 4, 5,
		2, 3, 3, 4, 3, 4, 4, 5, 3, 4, 4, 5, 4, 5, 5, 6,
		2, 3, 3, 4, 3, 4, 4, 5, 3, 4, 4, 5, 4, 5, 5, 6,
		3, 4, 4, 5, 4, 5, 5, 6, 4, 5, 5, 6, 5, 6, 6, 7,
		2, 3, 3, 4, 3, 4, 4, 5, 3, 4, 4, 5, 4, 5, 5, 6,
		3, 4, 4, 5, 4, 5, 5, 6, 4, 5, 5, 6, 5, 6, 6, 7,
		3, 4, 4, 5, 4, 5, 5, 6, 4, 5, 5, 6, 5, 6, 6, 7,
		4, 5, 5, 6, 5, 6, 6, 7, 5, 6, 6, 7, 6, 7, 7, 8
	};

	/** countBits helper 
	 *  @ingroup BitManip
	 *  @internal
	 */
	template <size_t numBytes> struct BitCounter;

	template <>
	struct BitCounter<1>
	{
		template <typename T> static size_t count(T bits)
		{
			return bitsInByte[(num::Tuint8) bits];
		}
	};

	template <>
	struct BitCounter<2>
	{
		template <typename T> static size_t count(T bits)
		{
			const num::Tuint16 x = (num::Tuint16) bits;
			return impl::bitsInByte[x >> 8] + impl::bitsInByte[x & 0xff];
		}
	};

	template <>
	struct BitCounter<4>
	{
		template <typename T> static size_t count(T bits)
		{
			const num::Tuint32 x = (num::Tuint32) bits;
			return impl::bitsInByte[x >> 24] + impl::bitsInByte[(x >> 16) & 0xff] + 
				impl::bitsInByte[(x >> 8) & 0xff] + impl::bitsInByte[x & 0xff];
		}
	};

	template <>
	struct BitCounter<8>
	{
		template <typename T> static size_t count(T bits)
		{
			const num::Tuint64 x = (num::Tuint64) bits;
			return impl::bitsInByte[x >> 56] + impl::bitsInByte[(x >> 48) & 0xff] +
				impl::bitsInByte[(x >> 40) & 0xff] + impl::bitsInByte[(x >> 32) & 0xff] +
				impl::bitsInByte[(x >> 24) & 0xff] + impl::bitsInByte[(x >> 16) & 0xff] +
				impl::bitsInByte[(x >> 8) & 0xff] + impl::bitsInByte[x & 0xff];
		}
	};
}

/** returns number of set bits in @a bits
 *  @ingroup BitManip
 */
template <typename T> inline size_t countBits(T bits)
{
	return impl::BitCounter<sizeof(T)>::count(bits);
}

}

}

#endif

// --- END OF FILE ------------------------------------------------------------------------------
