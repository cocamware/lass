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

/** @defgroup BitManip BitManip
 *  @brief A set of simple bit manipulation functions.
 *  @author Bram de Greve
 *  @date 2003
 *
 *  This module groups simple bit manipulation functions you can use on bitfields.  These bitfields 
 *  can be of any type for which the bit operators like &=, |=, ... are defined.  Typically, you'll 
 *  use @c int, @c char, @c unsigned, ...
 *
 *  The functions are split in two groups.  The @e bit routines (with the word @c Bit in their 
 *  names :) and the @e mask routines (with the word @c Mask).  The groups are very alike, except in
 *  the way you address the bits to affect.  With the @e bit group you will address one single bit 
 *  at one time, and you will indicate it by its index in the bit field (i.e. bit 0 is 0x01, bit 1 
 *  is 0x02, bit 2 is 0x04, ...).  With the @e mask group, you can address multiple bits at once by 
 *  setting these bits in the mask (e.g. to address bits 0 and 2, you'll use 0x05 as mask).  These
 *  mask must be of the same type of the bitfield.
 *
 *  With the routines you can set bits to 1; clear bits to 0; flip bits (0 becomes 1, 1 becomes 0);
 *  set, clear or flip bits if a condition is fullfilled; set a bit to a given value (0 or 1);
 *  check if a bit is set.  For all these operations you'll find a function in both the @c bit group
 *  as in the @c mask group, except for the last operation: to check if a bit is set.  In the @c bit
 *  group you'll find one function checkBit which will return true if the addressed bit is set.  
 *  In the @c mask group you'll find two: checkMaskedAll and checkMaskedSome.  the former only
 *  returns true if @e all bits addressed by the mask are set (an @e and operation), the latter 
 *  already returns true if at least @e some of the addressed bits are set (an @e or operation).
 */


#ifndef LASS_GUARDIAN_OF_INCLUSION_UTIL_BIT_MANIP_H
#define LASS_GUARDIAN_OF_INCLUSION_UTIL_BIT_MANIP_H
#pragma once

#include "util_common.h"



namespace lass
{

namespace util
{

template<typename T> inline void setBit(T& a_bits, unsigned a_bit);
template<typename T> inline void clearBit(T& a_bits, unsigned a_bit);
template<typename T> inline void flipBit(T& a_bits, unsigned a_bit);
template<typename T> inline void setBitIf(T& a_bits, unsigned a_bit, bool a_condition);
template<typename T> inline void clearBitIf(T& a_bits, unsigned a_bit, bool a_condition);
template<typename T> inline void flipBitIf(T& a_bits, unsigned a_bit, bool a_condition);
template<typename T> inline void setBitTo(T& a_bits, unsigned a_bit, bool a_state);
template<typename T> inline bool checkBit(T a_bits, unsigned a_bit);

template<typename T> inline void setMasked(T& a_bits, const T& a_mask);
template<typename T> inline void clearMasked(T& a_bits, const T& a_mask);
template<typename T> inline void flipMasked(T& a_bits, const T& a_mask);
template<typename T> inline void setMaskedIf(T& a_bits, const T& a_mask, bool a_condition);
template<typename T> inline void clearMaskedIf(T& a_bits, const T& a_mask, bool a_condition);
template<typename T> inline void flipMaskedIf(T& a_bits, const T& a_mask, bool a_condition);
template<typename T> inline void setMaskedTo(T& a_bits, const T& a_mask, bool a_state);
template<typename T> inline bool checkMaskedAll(T a_bits, const T& a_mask);
template<typename T> inline bool checkMaskedSome(T a_bits, const T& a_mask);

}

}

#include "bit_manip.inl"

#endif
