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

/** @defgroup BasicTypes
 */

#ifndef LASS_GUARDIAN_OF_INCLUSION_NUM_BASIC_TYPES_H
#define LASS_GUARDIAN_OF_INCLUSION_NUM_BASIC_TYPES_H

//#include "num_common.h"
#include "../meta/meta_assert.h"

#if defined(LASS_NUM_BASIC_TYPES_HAVE_STDINT_H)
#	include <stdint.h>
#endif

namespace lass
{
namespace num
{

#if defined(LASS_NUM_BASIC_TYPES_HAVE_STDINT_H)

	typedef int8_t Tint8;	/**< @ingroup BasicTypes */
	typedef uint8_t Tuint8;	/**< @ingroup BasicTypes */
	typedef int16_t Tint16;	/**< @ingroup BasicTypes */
	typedef uint16_t Tuint16;	/**< @ingroup BasicTypes */
	typedef int32_t Tint32;	/**< @ingroup BasicTypes */
	typedef uint32_t Tuint32;	/**< @ingroup BasicTypes */
	typedef int64_t Tint64;	/**< @ingroup BasicTypes */
	typedef uint64_t Tuint64;	/**< @ingroup BasicTypes */

#elif defined(LASS_NUM_BASIC_TYPES_HAVE_MSVC)

	typedef __int8 Tint8;	/**< @ingroup BasicTypes */
	typedef unsigned __int8 Tuint8;	/**< @ingroup BasicTypes */
	typedef __int16 Tint16;	/**< @ingroup BasicTypes */
	typedef unsigned __int16 Tuint16;	/**< @ingroup BasicTypes */
	typedef __int32 Tint32;	/**< @ingroup BasicTypes */
	typedef unsigned __int32 Tuint32;	/**< @ingroup BasicTypes */
	typedef __int64 Tint64;	/**< @ingroup BasicTypes */
	typedef unsigned __int64 Tuint64;	/**< @ingroup BasicTypes */

#else

	typedef signed char Tint8;	/**< @ingroup BasicTypes */
	typedef unsigned char Tuint8;	/**< @ingroup BasicTypes */
	typedef signed short Tint16;	/**< @ingroup BasicTypes */
	typedef unsigned short Tuint16;	/**< @ingroup BasicTypes */
	typedef signed int Tint32;	/**< @ingroup BasicTypes */
	typedef unsigned int Tuint32;	/**< @ingroup BasicTypes */
	typedef signed long long Tint64;	/**< @ingroup BasicTypes */
	typedef unsigned long long Tuint64;	/**< @ingroup BasicTypes */

#endif



typedef float Tfloat32;	/**< @ingroup BasicTypes */
typedef double Tfloat64;	/**< @ingroup BasicTypes */


/** @ingroup BasicTypes
 */
template <size_t iNumberOfBits> struct BasicType {};
template <> struct BasicType< 8> { typedef Tint8  Tint; typedef Tuint8  Tuint; };
template <> struct BasicType<16> { typedef Tint16 Tint; typedef Tuint16 Tuint; };
template <> struct BasicType<32> { typedef Tint32 Tint; typedef Tuint32 Tuint; typedef Tfloat32 Tfloat; };
template <> struct BasicType<64> { typedef Tint64 Tint; typedef Tuint64 Tuint; typedef Tfloat64 Tfloat; };



typedef BasicType<sizeof(void*) * lass::bitsPerByte>::Tint TintPtr;	/**< @ingroup BasicTypes */
typedef BasicType<sizeof(void*) * lass::bitsPerByte>::Tuint TuintPtr;	/**< @ingroup BasicTypes */

/** @ingroup BasicTypes
 */
template <typename T> struct DoublePrecision { };
template <> struct DoublePrecision<float> { typedef double Type; };
template <> struct DoublePrecision<double> { typedef long double Type; };
template <> struct DoublePrecision<long double> { typedef long double Type; };



// --- meta assertions -----------------------------------------------------------------------------

namespace impl
{

LASS_META_ASSERT(lass::bitsPerByte == 8, one_byte_is_not_8_bits);
LASS_META_ASSERT(sizeof(lass::num::Tint8) * lass::bitsPerByte == 8, Tint8_is_not_8_bits);
LASS_META_ASSERT(sizeof(lass::num::Tuint8) * lass::bitsPerByte == 8, Tuint8_is_not_8_bits);
LASS_META_ASSERT(sizeof(lass::num::Tint16) * lass::bitsPerByte == 16, Tint16_is_not_16_bits);
LASS_META_ASSERT(sizeof(lass::num::Tuint16) * lass::bitsPerByte == 16, Tuint16_is_not_16_bits);
LASS_META_ASSERT(sizeof(lass::num::Tint32) * lass::bitsPerByte == 32, Tint32_is_not_32_bits);
LASS_META_ASSERT(sizeof(lass::num::Tuint32) * lass::bitsPerByte == 32, Tuint32_is_not_32_bits);
LASS_META_ASSERT(sizeof(lass::num::Tint64) * lass::bitsPerByte == 64, Tint64_is_not_64_bits);
LASS_META_ASSERT(sizeof(lass::num::Tuint64) * lass::bitsPerByte == 64, Tuint64_is_not_64_bits);
LASS_META_ASSERT(sizeof(lass::num::Tfloat32) * lass::bitsPerByte == 32, Tfloat32_is_not_32_bits);
LASS_META_ASSERT(sizeof(lass::num::Tfloat64) * lass::bitsPerByte == 64, Tfloat64_is_not_64_bits);

}

}

}

#endif
