/**	@file
 *	@author Bram de Greve (bramz@users.sourceforge.net)
 *	@author Tom De Muer (tomdemuer@users.sourceforge.net)
 *
 *	Distributed under the terms of the GPL (GNU Public License)
 *
 * 	The LASS License:
 *
 *	Copyright 2004 Bram de Greve and Tom De Muer
 *
 *	LASS is free software; you can redistribute it and/or modify
 *	it under the terms of the GNU General Public License as published by
 *	the Free Software Foundation; either version 2 of the License, or
 *	(at your option) any later version.
 *
 *	This program is distributed in the hope that it will be useful,
 *	but WITHOUT ANY WARRANTY; without even the implied warranty of
 *	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *	GNU General Public License for more details.
 *
 *	You should have received a copy of the GNU General Public License
 *	along with this program; if not, write to the Free Software
 *	Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */



#ifndef LASS_GUARDIAN_OF_INCLUSION_UTIL_BIT_MANIP_INL
#define LASS_GUARDIAN_OF_INCLUSION_UTIL_BIT_MANIP_INL
#pragma once

#include "bit_manip.h"



namespace lass
{

namespace util
{

/** Set a bit high.
 *  @ingroup BitManip
 *  @author Bram de Greve [BdG]
 *  @param a_bits the bitfield of which a bit must be set high.
 *  @param a_bit the index of the bit to be set high (0 => 0x1, 1 => 0x2, ...)
 */
template<typename T>
void setBit(T& a_bits, unsigned a_bit)
{
    a_bits |= (1 << a_bit);
}



/** set a bit low.
 *  @ingroup BitManip
 *  @author Bram de Greve [BdG]
 *  @param a_bits the bitfield of which a bit must be set low.
 *  @param a_bit the index of the bit to be set low (0 => 0x1, 1 => 0x2, ...)
 */
template<typename T>
void clearBit(T& a_bits, unsigned a_bit)
{
    a_bits &= ~(1 << a_bit);
}



/** flip state of a bit (low->high, high->low).
 *  @ingroup BitManip
 *  @author Bram de Greve [BdG]
 *  @param a_bits the bitfield of which a bit must be flipped.
 *  @param a_bit the index of the bit to be flipped (0 => 0x1, 1 => 0x2, ...)
 */
template<typename T>
void flipBit(T& a_bits, unsigned a_bit)
{
    a_bits ^= (1 << a_bit);
}



/** set a bit high if (and only if) a condition is fullfilled.
 *  @ingroup BitManip
 *  @author Bram de Greve [BdG]
 *  @param a_bits the bitfield of which a bit must be set high.
 *  @param a_bit the index of the bit to be set high (0 => 0x1, 1 => 0x2, ...)
 *  @param a_condition true if the bit must be set high, false if not.
 */
template<typename T>
void setBitIf(T& a_bits, unsigned a_bit, bool a_condition)
{
    setMasked(a_bits, a_condition ? (1 << a_bit) : 0);
}



/** set a bit low if (and only if) a condition is fullfilled is true.
 *  @ingroup BitManip
 *  @author Bram de Greve [BdG]
 *  @param a_bits the bitfield of which a bit must be set low.
 *  @param a_bit the index of the bit to be set low (0 => 0x1, 1 => 0x2, ...)
 *  @param a_condition true if the bit must be set low, false if not.
 */
template<typename T>
void clearBitIf(T& a_bits, unsigned a_bit, bool a_condition)
{
    clearMasked(a_bits, a_condition ? (1 << a_bit) : 0);
}



/** flip a bit if (and only if) a condition is fullfilled (low->high, high->low).
 *  @ingroup BitManip
 *  @author Bram de Greve [BdG]
 *  @param a_bits the bitfield of which a bit must be flipped.
 *  @param a_bit the index of the bit to be flipped (0 => 0x1, 1 => 0x2, ...)
 *  @param a_condition true if the bit must be flipped, false if not.
 */
template<typename T>
void flipBitIf(T& a_bits, unsigned a_bit, bool a_condition)
{
    flipMasked(a_bits, a_condition ? (1 << a_bit) : 0);
}



/** set a bit to a given state.
 *  @ingroup BitManip
 *  @author Bram de Greve [BdG]
 *  @param a_bits the bitfield of which a bit must be set.
 *  @param a_bit the index of the bit to be set (0 => 0x1, 1 => 0x2, ...)
 *  @param a_state the state to which the bit must be set: true for high, false for low.
 */
template<typename T>
void setBitTo(T& a_bits, unsigned a_bit, bool a_state)
{
    clearBit(a_bits, a_bit);
    setBitIf(a_bits, a_bit, a_state);
}



/** return true if a bit is set high.
 *  @ingroup BitManip
 *  @author Bram de Greve [BdG]
 *  @param a_bits the bitfield of which a bit must be checked.
 *  @param a_bit the index of the bit to be checked (0 => 0x1, 1 => 0x2, ...)
 */
template<typename T>
bool checkBit(T a_bits, unsigned a_bit)
{
    return a_bits & (1 << a_bit);
}




/** Set masked bits high.
 *  @ingroup BitManip
 *  @author Bram de Greve [BdG]
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
 *  @author Bram de Greve [BdG]
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
 *  @author Bram de Greve [BdG]
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
 *  @author Bram de Greve [BdG]
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
    setBit(a_bits, a_condition ? a_mask : 0);
}



/** Set masked bits low if (and only if) a condition is fullfilled.
 *  @ingroup BitManip
 *  @author Bram de Greve [BdG]
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
    clearBit(a_bits, a_condition ? a_mask : 0);
}



/** Flip the masked bits if (and only if) a condition is fullfilled.
 *  @ingroup BitManip
 *  @author Bram de Greve [BdG]
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
    flipBit(a_bits, a_condition ? a_mask : 0);
}



/** Set the masked bits to a given state if (and only if) a condition is fullfilled.
 *  @ingroup BitManip
 *  @author Bram de Greve [BdG]
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
    clearBit(a_bits, a_mask);
    setBitIf(a_bits, a_mask, a_state);
}



/** Check the masked bits and return true if they are ALL set.
 *  @ingroup BitManip
 *  @author Bram de Greve [BdG]
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
 *  @author Bram de Greve [BdG]
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


}

}

#endif

// --- END OF FILE ------------------------------------------------------------------------------
