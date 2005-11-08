/** @file
 *  @author Bram de Greve (bramz@users.sourceforge.net)
 *  @author Tom De Muer (tomdemuer@users.sourceforge.net)
 *
 *  Distributed under the terms of the GPL (GNU Public License)
 *
 *  The LASS License:
 *
 *  Copyright 2004 Bram de Greve and Tom De Muer
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

#ifndef LASS_GUARDIAN_OF_INCLUSION_NUM_DB_INL
#define LASS_GUARDIAN_OF_INCLUSION_NUM_DB_INL

#include "num_traits.h"

namespace lass
{
	namespace num
	{
		#define LASS_NUM_REFERENCE_PRESSURE_P0      20.e-6
		#define LASS_NUM_REFERENCE_INTENSITY_I0     1.e-12
		#define LASS_NUM_REFERENCE_POWER_W0         1.e-12

		template< typename T >  T   p2dB( const T& iValue )
		{
			if (iValue>=T(0))
			{
				return T(20)*lass::num::log(iValue/T(LASS_NUM_REFERENCE_PRESSURE_P0))/lass::num::log(T(10));
			}
			if (!isNaN(iValue))
			{
				LASS_THROW( "Negative argument '" << iValue << "'" );
			}
			return iValue; // nan
		}

		template< typename T >  T   W2dB( const T& iValue )
		{
			if (iValue>=T(0))
			{
				return T(10)*lass::num::log(iValue/T(LASS_NUM_REFERENCE_POWER_W0))/lass::num::log(T(10));
			}
			if (!isNaN(iValue))
			{
				LASS_THROW( "Negative argument '" << iValue << "'" );
			}
			return iValue; // nan
		}

		template< typename T >  T   I2dB( const T& iValue )
		{
			if (iValue>=T(0))
			{
				return T(10)*lass::num::log(iValue/T(LASS_NUM_REFERENCE_INTENSITY_I0))/lass::num::log(T(10));
			}
			if (!isNaN(iValue))
			{
				LASS_THROW( "Negative argument '" << iValue << "'" );
			}
			return iValue; // nan
		}

		template< typename T >  T   dB2p( const T& iValue )
		{
			const T log10by20 = lass::num::log(T(10)) / T(20);
			return iValue == -NumTraits<T>::infinity ?
				NumTraits<T>::zero :
				T(LASS_NUM_REFERENCE_PRESSURE_P0) * lass::num::exp(iValue * log10by20);
		}

		template< typename T >  T   dB2W( const T& iValue )
		{
			const T log10by10 = lass::num::log(T(10)) / T(10);
			return iValue == -NumTraits<T>::infinity ?
				NumTraits<T>::zero :
				T(LASS_NUM_REFERENCE_POWER_W0) * lass::num::exp(iValue * log10by10);
		}

		template< typename T >  T   dB2I( const T& iValue )
		{
			const T log10by10 = lass::num::log(T(10)) / T(10);
			return iValue == -NumTraits<T>::infinity ?
				NumTraits<T>::zero :
				T(LASS_NUM_REFERENCE_INTENSITY_I0) * lass::num::exp(iValue * log10by10);
		}
	}
}

#endif

// EOF
