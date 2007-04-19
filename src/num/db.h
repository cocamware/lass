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

/** @defgroup dB
 *  @brief collection of conversion functions for dB
 */

#ifndef LASS_GUARDIAN_OF_INCLUSION_NUM_DB_H
#define LASS_GUARDIAN_OF_INCLUSION_NUM_DB_H

#include "num_common.h"
#include "num_traits.h"

#define LASS_NUM_DB_10_OVER_LN10 4.3429448190325182765112891891661
#define LASS_NUM_DB_20_OVER_LN10 8.6858896380650365530225783783321
#define LASS_NUM_DB_LN10_OVER_10 0.23025850929940456840179914546844
#define LASS_NUM_DB_LN10_OVER_20 0.11512925464970228420089957273422

namespace lass
{
	namespace num
	{
		/** power to decibels: y = 10 * log10(x)
		 *  @ingroup dB
		 */
		template <typename T> inline T pow2dB(const T& power)
		{
			return T(LASS_NUM_DB_10_OVER_LN10) * num::log(power);
		}

		/** amplitude to decibels: y = 20 * log10(x)
		 *	@ingroup dB
		 */
		template <typename T> inline T amp2dB(const T& amplitude)
		{
			return T(LASS_NUM_DB_20_OVER_LN10) * num::log(amplitude);
		}

		/** decibels to power: y = num::pow(10, x / 10)
		 *  @ingroup dB
		 */
		template <typename T> inline T dB2pow(const T& decibels)
		{
			return num::exp(T(LASS_NUM_DB_LN10_OVER_10) * decibels);
		}

		/** decibels to amplitude: y = num::pow(10, x / 20)
		 *  @ingroup dB
		 */
		template <typename T> inline T dB2amp(const T& decibels)
		{
			return num::exp(T(LASS_NUM_DB_LN10_OVER_20) * decibels);
		}

		/** Converts an absolute acoustical pressure into decibels.
		 *  @ingroup dB
		 */
		template< typename T >  T   p2dB( const T& iValue );

		/** Converts a absolute acoustical power into decibels.
		 *  @ingroup dB
		 */
		template< typename T >  T   W2dB( const T& iValue );

		/** Converts an intensity into decibels.
		 *  @ingroup dB.
		 *  The intensity represents the intensity in all directions expressed in Watts/m^2,
		 *  referenced to 10^-12.
		 */
		template< typename T >  T   I2dB( const T& iValue );

		/** Converts decibels into a pressure .
		 *  @ingroup dB
		 */
		template< typename T >  T   dB2p( const T& iValue );

		/** Converts decibels into a power.
		 *  @ingroup dB
		 */
		template< typename T >  T   dB2W( const T& iValue );

		/** Converts decibels into an intensity.
		 *  @ingroup dB
		 *  The intensity represents the intensity in all directions expressed in Watts/m^2,
		 *  referenced to 10^-12
		 */
		template< typename T >  T   dB2I( const T& iValue );
	}
}

#include "db.inl"

#endif
