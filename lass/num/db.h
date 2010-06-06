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
 *	Copyright (C) 2004-2010 the Initial Developer.
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
