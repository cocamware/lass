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

	typedef signed __int8 Tint8;	/**< @ingroup BasicTypes */
	typedef unsigned __int8 Tuint8;	/**< @ingroup BasicTypes */
	typedef signed __int16 Tint16;	/**< @ingroup BasicTypes */
	typedef unsigned __int16 Tuint16;	/**< @ingroup BasicTypes */
	typedef signed __int32 Tint32;	/**< @ingroup BasicTypes */
	typedef unsigned __int32 Tuint32;	/**< @ingroup BasicTypes */
	typedef signed __int64 Tint64;	/**< @ingroup BasicTypes */
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
LASS_META_ASSERT(sizeof(lass::num::Tint8) * lass::bitsPerByte == 8, Tint8_must_be_8_bits_);
LASS_META_ASSERT(sizeof(lass::num::Tuint8) * lass::bitsPerByte == 8, Tuint8_must_be_8_bits);
LASS_META_ASSERT(sizeof(lass::num::Tint16) * lass::bitsPerByte == 16, Tint16_must_be_16_bits);
LASS_META_ASSERT(sizeof(lass::num::Tuint16) * lass::bitsPerByte == 16, Tuint16_must_be_16_bits);
LASS_META_ASSERT(sizeof(lass::num::Tint32) * lass::bitsPerByte == 32, Tint32_must_be_32_bits);
LASS_META_ASSERT(sizeof(lass::num::Tuint32) * lass::bitsPerByte == 32, Tuint32_must_be_32_bits);
LASS_META_ASSERT(sizeof(lass::num::Tint64) * lass::bitsPerByte == 64, Tint64_must_be_64_bits);
LASS_META_ASSERT(sizeof(lass::num::Tuint64) * lass::bitsPerByte == 64, Tuint64_must_be_64_bits);
LASS_META_ASSERT(sizeof(lass::num::Tfloat32) * lass::bitsPerByte == 32, Tfloat32_must_be_32_bits);
LASS_META_ASSERT(sizeof(lass::num::Tfloat64) * lass::bitsPerByte == 64, Tfloat64_must_be_64_bits);
LASS_META_ASSERT(sizeof(lass::num::TintPtr) == sizeof(void*), TintPtr_must_be_size_of_pointer);
LASS_META_ASSERT(sizeof(lass::num::TuintPtr) == sizeof(void*), TuintPtr_must_be_size_of_pointer);

}

}

}

#endif
