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



#include "num_common.h"

#include "num_traits.h"
#include "interval.h"
#include <limits>

namespace lass
{
namespace num
{

/*********************************************************************** 
* float num trait
*/

/*  got rid of the template<> because VS2003 didn't like it although but we did 
*	and at first and second glance the standard also likes it [TDM]
*/
const int	NumTraits<float>::memorySize = sizeof(float);
const bool	NumTraits<float>::isIntegral = false;
const bool	NumTraits<float>::isNative = true;
const bool	NumTraits<float>::isSigned = true;
const bool	NumTraits<float>::hasInfinity = true;
const bool	NumTraits<float>::hasNaN = true;

const float	NumTraits<float>::one = 1.f;
const float	NumTraits<float>::zero = 0.f;

const float	NumTraits<float>::qNaN = std::numeric_limits<float>::quiet_NaN();
const float	NumTraits<float>::sNaN = std::numeric_limits<float>::signaling_NaN();
const float	NumTraits<float>::infinity = std::numeric_limits<float>::infinity();
const float NumTraits<float>::epsilon = 1.192092896e-07F;
const float	NumTraits<float>::min = -3.402823466e+38F;
const float	NumTraits<float>::max = 3.402823466e+38F;
const float	NumTraits<float>::minStrictPositive = 1.175494351e-38F;
const float	NumTraits<float>::pi = 3.1415926535897932384626433832795f;
const float	NumTraits<float>::e = 2.7182818284590452353602874713527f;
const float	NumTraits<float>::sqrt2 = 1.4142135623730950488016887242097f;
const float	NumTraits<float>::sqrtPi = 1.7724538509055160272981674833411f;
bool isNaN( float iV )
{
	return (iV!=iV);
}
// complex specialisation
const int	NumTraits<std::complex<float> >::memorySize = sizeof(std::complex<float>);
const bool	NumTraits<std::complex<float> >::isIntegral = false;
const bool	NumTraits<std::complex<float> >::isNative = false;
const bool	NumTraits<std::complex<float> >::isSigned = true;
const bool	NumTraits<std::complex<float> >::hasInfinity = false;
const bool	NumTraits<std::complex<float> >::hasNaN = false;

const std::complex<float>	NumTraits<std::complex<float> >::one = NumTraits<float>::one;
const std::complex<float>	NumTraits<std::complex<float> >::zero =  NumTraits<float>::zero;

const std::complex<float>	NumTraits<std::complex<float> >::pi = 3.1415926535897932384626433832795f;
const std::complex<float>	NumTraits<std::complex<float> >::e = 2.7182818284590452353602874713527f;
const std::complex<float>	NumTraits<std::complex<float> >::sqrt2 = 1.4142135623730950488016887242097f;
const std::complex<float>	NumTraits<std::complex<float> >::sqrtPi = 1.7724538509055160272981674833411f;
/*********************************************************************** 
* double num trait
*/
const int	NumTraits<double>::memorySize = sizeof(double);
const bool	NumTraits<double>::isIntegral = false;
const bool	NumTraits<double>::isNative = true;
const bool	NumTraits<double>::isSigned = true;
const bool	NumTraits<double>::hasInfinity = true;
const bool	NumTraits<double>::hasNaN = true;

const double	NumTraits<double>::one = 1.0;
const double NumTraits<double>::zero= 0.0;

const double	NumTraits<double>::qNaN = std::numeric_limits<double>::quiet_NaN();
const double	NumTraits<double>::sNaN = std::numeric_limits<double>::signaling_NaN();
const double NumTraits<double>::infinity = std::numeric_limits<double>::infinity();
const double NumTraits<double>::epsilon = 2.2204460492503131e-016;
const double	NumTraits<double>::min = -1.7976931348623158e+308;
const double	NumTraits<double>::max = 1.7976931348623158e+308;
const double	NumTraits<double>::minStrictPositive = 2.2250738585072014e-308;
const double	NumTraits<double>::pi = 3.1415926535897932384626433832795;
const double	NumTraits<double>::e = 2.7182818284590452353602874713527;
const double	NumTraits<double>::sqrt2 = 1.4142135623730950488016887242097;
const double	NumTraits<double>::sqrtPi = 1.7724538509055160272981674833411;
bool isNaN( double iV )
{
	return (iV!=iV);
}
// complex specialisation
const int	NumTraits<std::complex<double> >::memorySize = sizeof(std::complex<double>);
const bool	NumTraits<std::complex<double> >::isIntegral = false;
const bool	NumTraits<std::complex<double> >::isNative = false;
const bool	NumTraits<std::complex<double> >::isSigned = true;
const bool	NumTraits<std::complex<double> >::hasInfinity = false;
const bool	NumTraits<std::complex<double> >::hasNaN = false;

const std::complex<double>	NumTraits<std::complex<double> >::one = NumTraits<double>::one;
const std::complex<double>	NumTraits<std::complex<double> >::zero =  NumTraits<double>::zero;

const std::complex<double>	NumTraits<std::complex<double> >::pi = 3.1415926535897932384626433832795;
const std::complex<double>	NumTraits<std::complex<double> >::e = 2.7182818284590452353602874713527;
const std::complex<double>	NumTraits<std::complex<double> >::sqrt2 = 1.4142135623730950488016887242097;
const std::complex<double>	NumTraits<std::complex<double> >::sqrtPi = 1.7724538509055160272981674833411;
/** code generating macro for integral types */
#define LASS_NUM_TRAITS_INTEGRAL_TEMPLATE_SPEC( __LASS_type, __LASS_isSigned, __LASS_min, __LASS_max ) \
const int	NumTraits<__LASS_type>::memorySize = sizeof(signed char);\
const bool	NumTraits<__LASS_type>::isIntegral = false;\
const bool	NumTraits<__LASS_type>::isNative = true;\
const bool	NumTraits<__LASS_type>::isSigned = __LASS_isSigned;\
const bool	NumTraits<__LASS_type>::hasInfinity = false;\
const bool	NumTraits<__LASS_type>::hasNaN = false;\
const __LASS_type NumTraits<__LASS_type>::one = 1;\
const __LASS_type NumTraits<__LASS_type>::zero= 0;\
const __LASS_type NumTraits<__LASS_type>::epsilon = 1;\
const __LASS_type NumTraits<__LASS_type>::min = __LASS_min;\
const __LASS_type NumTraits<__LASS_type>::max = __LASS_max;\
const __LASS_type NumTraits<__LASS_type>::minStrictPositive = 1;


/*********************************************************************** 
* unsigned/signed char/short/int/long num trait
*/
#if defined(_MSC_VER)
LASS_NUM_TRAITS_INTEGRAL_TEMPLATE_SPEC( unsigned char, true, 0 , UCHAR_MAX)
LASS_NUM_TRAITS_INTEGRAL_TEMPLATE_SPEC( signed char, true, SCHAR_MIN , SCHAR_MAX )
LASS_NUM_TRAITS_INTEGRAL_TEMPLATE_SPEC( unsigned short, true, 0 , USHRT_MAX)
LASS_NUM_TRAITS_INTEGRAL_TEMPLATE_SPEC( signed short, true, SHRT_MIN , SHRT_MAX )
LASS_NUM_TRAITS_INTEGRAL_TEMPLATE_SPEC( unsigned int, true, 0 , UINT_MAX )
LASS_NUM_TRAITS_INTEGRAL_TEMPLATE_SPEC( signed int, true, INT_MIN , INT_MAX )
LASS_NUM_TRAITS_INTEGRAL_TEMPLATE_SPEC( unsigned long, true, 0 , ULONG_MAX )
LASS_NUM_TRAITS_INTEGRAL_TEMPLATE_SPEC( signed long, true, LONG_MIN , LONG_MAX )
#else
LASS_NUM_TRAITS_INTEGRAL_TEMPLATE_SPEC( signed char, true, -128, 127 )
LASS_NUM_TRAITS_INTEGRAL_TEMPLATE_SPEC( unsigned char, false, 0, 255 )
LASS_NUM_TRAITS_INTEGRAL_TEMPLATE_SPEC( unsigned short, true, 0 , 0xffff)
LASS_NUM_TRAITS_INTEGRAL_TEMPLATE_SPEC( signed short, true, (-32768) , 32767 )
LASS_NUM_TRAITS_INTEGRAL_TEMPLATE_SPEC( unsigned int, true, 0 , 0xffffffff )
LASS_NUM_TRAITS_INTEGRAL_TEMPLATE_SPEC( signed int, true, (-2147483647 - 1) , 2147483647 )
LASS_NUM_TRAITS_INTEGRAL_TEMPLATE_SPEC( unsigned long, true, 0 , 0xffffffff )
LASS_NUM_TRAITS_INTEGRAL_TEMPLATE_SPEC( signed long, true, (-2147483647 - 1) , 2147483647 )
#endif

#ifdef LASS_CHAR_IS_SIGNED
#pragma LASS_NOTE("char is configured as signed char")
LASS_NUM_TRAITS_INTEGRAL_TEMPLATE_SPEC( char, false, -128, 127 )
#else
#pragma LASS_NOTE("char is configured as unsigned char")
LASS_NUM_TRAITS_INTEGRAL_TEMPLATE_SPEC( char, false, 0, 255 )
#endif
}
}