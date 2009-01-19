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


#include "util_common.h"
#include "../num/basic_types.h"

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

template<typename T> inline const size_t countBits(T bits);

/*
inline const unsigned countBits(num::Tuint8 x);
inline const unsigned countBits(num::Tint8 x);
inline const unsigned countBits(num::Tuint16 x);
inline const unsigned countBits(num::Tint16 x);
inline const unsigned countBits(num::Tuint32 x);
inline const unsigned countBits(num::Tint32 x);
inline const unsigned countBits(num::Tuint64 x);
inline const unsigned countBits(num::Tint64 x);
*/

}

}

#include "bit_manip.inl"

#endif
