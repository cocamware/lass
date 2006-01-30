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



#include "num_common.h"

#include "num_traits.h"
#include "interval.h"
#include <limits>

#ifdef LASS_CHAR_IS_SIGNED
#	pragma LASS_NOTE("char is configured as signed char")
#endif

namespace lass
{
namespace num
{

/***********************************************************************
* float num trait
*/

/*  got rid of the template<> because VS2003 didn't like it although but we did
*   and at first and second glance the standard also likes it [TDM]
*/
const int   NumTraits<float>::memorySize = sizeof(float);
const float NumTraits<float>::one = 1.f;
const float NumTraits<float>::zero = 0.f;
const float NumTraits<float>::qNaN = std::numeric_limits<float>::quiet_NaN();
const float NumTraits<float>::sNaN = std::numeric_limits<float>::signaling_NaN();
const float NumTraits<float>::infinity = std::numeric_limits<float>::infinity();
const float NumTraits<float>::epsilon = 1.192092896e-07F;
const float NumTraits<float>::min = -3.402823466e+38F;
const float NumTraits<float>::max = 3.402823466e+38F;
const float NumTraits<float>::minStrictPositive = 1.175494351e-38F;
const float NumTraits<float>::pi = 3.1415926535897932384626433832795f;
const float NumTraits<float>::e = 2.7182818284590452353602874713527f;
const float NumTraits<float>::sqrt2 = 1.4142135623730950488016887242097f;
const float NumTraits<float>::sqrtPi = 1.7724538509055160272981674833411f;

// complex specialisation
const int   NumTraits<std::complex<float> >::memorySize = sizeof(std::complex<float>);
const std::complex<float>   NumTraits<std::complex<float> >::one = std::complex<float>(1.f, 0.f);
const std::complex<float>   NumTraits<std::complex<float> >::zero = std::complex<float>(0.f, 0.f);
const std::complex<float>   NumTraits<std::complex<float> >::pi = 3.1415926535897932384626433832795f;
const std::complex<float>   NumTraits<std::complex<float> >::e = 2.7182818284590452353602874713527f;
const std::complex<float>   NumTraits<std::complex<float> >::sqrt2 = 1.4142135623730950488016887242097f;
const std::complex<float>   NumTraits<std::complex<float> >::sqrtPi = 1.7724538509055160272981674833411f;

/***********************************************************************
* double num trait
*/
const int   NumTraits<double>::memorySize = sizeof(double);
const double    NumTraits<double>::one = 1.0;
const double NumTraits<double>::zero= 0.0;
const double    NumTraits<double>::qNaN = std::numeric_limits<double>::quiet_NaN();
const double    NumTraits<double>::sNaN = std::numeric_limits<double>::signaling_NaN();
const double NumTraits<double>::infinity = std::numeric_limits<double>::infinity();
const double NumTraits<double>::epsilon = 2.2204460492503131e-016;
const double    NumTraits<double>::min = -1.7976931348623158e+308;
const double    NumTraits<double>::max = 1.7976931348623158e+308;
const double    NumTraits<double>::minStrictPositive = 2.2250738585072014e-308;
const double    NumTraits<double>::pi = 3.1415926535897932384626433832795;
const double    NumTraits<double>::e = 2.7182818284590452353602874713527;
const double    NumTraits<double>::sqrt2 = 1.4142135623730950488016887242097;
const double    NumTraits<double>::sqrtPi = 1.7724538509055160272981674833411;

// complex specialisation
const int   NumTraits<std::complex<double> >::memorySize = sizeof(std::complex<double>);
const std::complex<double>  NumTraits<std::complex<double> >::one = std::complex<double>(1.0, 0.0);
const std::complex<double>  NumTraits<std::complex<double> >::zero = std::complex<double>(0.0, 0.0);
const std::complex<double>  NumTraits<std::complex<double> >::pi = 3.1415926535897932384626433832795;
const std::complex<double>  NumTraits<std::complex<double> >::e = 2.7182818284590452353602874713527;
const std::complex<double>  NumTraits<std::complex<double> >::sqrt2 = 1.4142135623730950488016887242097;
const std::complex<double>  NumTraits<std::complex<double> >::sqrtPi = 1.7724538509055160272981674833411;

/***********************************************************************
* unsigned/signed char/short/int/long num trait
*/

/** code generating macro for integral types */
#define LASS_NUM_TRAITS_INTEGRAL_TEMPLATE_SPEC( t_type, v_min, v_max ) \
const int   NumTraits<t_type>::memorySize = sizeof(t_type);\
const t_type NumTraits<t_type>::one = 1;\
const t_type NumTraits<t_type>::zero= 0;\
const t_type NumTraits<t_type>::epsilon = 1;\
const t_type NumTraits<t_type>::min = v_min;\
const t_type NumTraits<t_type>::max = v_max;\
const t_type NumTraits<t_type>::minStrictPositive = 1;\

LASS_NUM_TRAITS_INTEGRAL_TEMPLATE_SPEC( char, CHAR_MIN , CHAR_MAX )
LASS_NUM_TRAITS_INTEGRAL_TEMPLATE_SPEC( unsigned char, 0 , UCHAR_MAX)
LASS_NUM_TRAITS_INTEGRAL_TEMPLATE_SPEC( signed char, SCHAR_MIN , SCHAR_MAX )
LASS_NUM_TRAITS_INTEGRAL_TEMPLATE_SPEC( unsigned short, 0 , USHRT_MAX)
LASS_NUM_TRAITS_INTEGRAL_TEMPLATE_SPEC( signed short, SHRT_MIN , SHRT_MAX )
LASS_NUM_TRAITS_INTEGRAL_TEMPLATE_SPEC( unsigned int, 0 , UINT_MAX )
LASS_NUM_TRAITS_INTEGRAL_TEMPLATE_SPEC( signed int, INT_MIN , INT_MAX )
LASS_NUM_TRAITS_INTEGRAL_TEMPLATE_SPEC( unsigned long, 0 , ULONG_MAX )
LASS_NUM_TRAITS_INTEGRAL_TEMPLATE_SPEC( signed long, LONG_MIN , LONG_MAX )

}
}
