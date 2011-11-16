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

		template< typename T >  T   p2dB( const T& pressure )
		{
			if (pressure>=T(0))
			{
				return T(LASS_NUM_DB_20_OVER_LN10) * num::log(pressure / T(LASS_NUM_REFERENCE_PRESSURE_P0));
			}
			if (!isNaN(pressure))
			{
				LASS_THROW( "Negative argument '" << pressure << "'" );
			}
			return pressure; // nan
		}

		template< typename T >  T   W2dB( const T& power )
		{
			if (power>=T(0))
			{
				return T(LASS_NUM_DB_10_OVER_LN10) * num::log(power / T(LASS_NUM_REFERENCE_POWER_W0));
			}
			if (!isNaN(power))
			{
				LASS_THROW( "Negative argument '" << power << "'" );
			}
			return power; // nan
		}

		template< typename T >  T   I2dB( const T& intensity )
		{
			if (intensity>=T(0))
			{
				return T(LASS_NUM_DB_10_OVER_LN10) * num::log(intensity / T(LASS_NUM_REFERENCE_INTENSITY_I0));
			}
			if (!isNaN(intensity))
			{
				LASS_THROW( "Negative argument '" << intensity << "'" );
			}
			return intensity; // nan
		}

		template< typename T >  T   dB2p( const T& decibels )
		{
			return decibels == -NumTraits<T>::infinity ?
				NumTraits<T>::zero :
				T(LASS_NUM_REFERENCE_PRESSURE_P0) * num::exp(decibels * T(LASS_NUM_DB_LN10_OVER_20));
		}

		template< typename T >  T   dB2W( const T& decibels )
		{
			return decibels == -NumTraits<T>::infinity ?
				NumTraits<T>::zero :
				T(LASS_NUM_REFERENCE_POWER_W0) * num::exp(decibels * T(LASS_NUM_DB_LN10_OVER_10));
		}

		template< typename T >  T   dB2I( const T& decibels )
		{
			return decibels == -NumTraits<T>::infinity ?
				NumTraits<T>::zero :
				T(LASS_NUM_REFERENCE_INTENSITY_I0) * num::exp(decibels * T(LASS_NUM_DB_LN10_OVER_10));
		}
	}
}

#endif

// EOF
