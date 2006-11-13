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
#include <climits>
#include <cfloat>

#ifdef LASS_CHAR_IS_SIGNED
#	pragma LASS_NOTE("char is configured as signed char")
#endif

// http://pi.lacim.uqam.ca/eng/table_en.html
#define LASS_NUM_PI			3.141592653589793238462643383279502884197169399375105820974944592307816406286208998628034825342117067
#define LASS_NUM_E			2.718281828459045235360287471352662497757247093699959574966967627724076630353547594571382178525166427
#define LASS_NUM_SQRT_2		1.414213562373095048801688724209698078569671875376948073176679737990732478462107038850387534327641572
#define LASS_NUM_SQRT_PI	1.772453850905516027298167483341145

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
template <> struct LASS_DLL NumTraits<float>;
const int   NumTraits<float>::memorySize = sizeof(float);
const int   NumTraits<float>::mantisseSize = std::numeric_limits<float>::digits;			
const float NumTraits<float>::one = 1.f;
const float NumTraits<float>::zero = 0.f;
const float NumTraits<float>::qNaN = std::numeric_limits<float>::quiet_NaN();
const float NumTraits<float>::sNaN = std::numeric_limits<float>::signaling_NaN();
const float NumTraits<float>::infinity = std::numeric_limits<float>::infinity();
const float NumTraits<float>::epsilon = FLT_EPSILON;
const float NumTraits<float>::min = -FLT_MAX;
const float NumTraits<float>::max = FLT_MAX;
const float NumTraits<float>::minStrictPositive = FLT_MIN;
const float NumTraits<float>::pi = float(LASS_NUM_PI);
const float NumTraits<float>::e = float(LASS_NUM_E);
const float NumTraits<float>::sqrt2 = float(LASS_NUM_SQRT_2);
const float NumTraits<float>::sqrtPi = float(LASS_NUM_SQRT_PI);

// complex specialisation
template <> struct LASS_DLL NumTraits< std::complex<float> >;
const int   NumTraits<std::complex<float> >::memorySize = sizeof(std::complex<float>);
const std::complex<float>   NumTraits<std::complex<float> >::one = std::complex<float>(1.f, 0.f);
const std::complex<float>   NumTraits<std::complex<float> >::zero = std::complex<float>(0.f, 0.f);
const std::complex<float>   NumTraits<std::complex<float> >::pi = float(LASS_NUM_PI);
const std::complex<float>   NumTraits<std::complex<float> >::e = float(LASS_NUM_E);
const std::complex<float>   NumTraits<std::complex<float> >::sqrt2 = float(LASS_NUM_SQRT_2);
const std::complex<float>   NumTraits<std::complex<float> >::sqrtPi = float(LASS_NUM_SQRT_PI);

/***********************************************************************
* double num trait
*/
template <> struct LASS_DLL NumTraits<double>;
const int   NumTraits<double>::memorySize = sizeof(double);
const int   NumTraits<double>::mantisseSize = std::numeric_limits<double>::digits;
const double    NumTraits<double>::one = 1.0;
const double NumTraits<double>::zero= 0.0;
const double    NumTraits<double>::qNaN = std::numeric_limits<double>::quiet_NaN();
const double    NumTraits<double>::sNaN = std::numeric_limits<double>::signaling_NaN();
const double NumTraits<double>::infinity = std::numeric_limits<double>::infinity();
const double NumTraits<double>::epsilon = DBL_EPSILON;
const double    NumTraits<double>::min = -DBL_MAX;
const double    NumTraits<double>::max = DBL_MAX;
const double    NumTraits<double>::minStrictPositive = DBL_MIN;
const double    NumTraits<double>::pi = LASS_NUM_PI;
const double    NumTraits<double>::e = LASS_NUM_E;
const double    NumTraits<double>::sqrt2 = LASS_NUM_SQRT_2;
const double    NumTraits<double>::sqrtPi = LASS_NUM_SQRT_PI;

// complex specialisation
template <> struct LASS_DLL NumTraits< std::complex<double> >;
const int   NumTraits<std::complex<double> >::memorySize = sizeof(std::complex<double>);
const std::complex<double>  NumTraits<std::complex<double> >::one = std::complex<double>(1.0, 0.0);
const std::complex<double>  NumTraits<std::complex<double> >::zero = std::complex<double>(0.0, 0.0);
const std::complex<double>  NumTraits<std::complex<double> >::pi = LASS_NUM_PI;
const std::complex<double>  NumTraits<std::complex<double> >::e = LASS_NUM_E;
const std::complex<double>  NumTraits<std::complex<double> >::sqrt2 = LASS_NUM_SQRT_2;
const std::complex<double>  NumTraits<std::complex<double> >::sqrtPi = LASS_NUM_SQRT_PI;

/***********************************************************************
* long double num trait
*/
template <> struct LASS_DLL NumTraits<long double>;
const int   NumTraits<long double>::memorySize = sizeof(long double);
const int   NumTraits<long double>::mantisseSize = std::numeric_limits<long double>::digits;
const long double    NumTraits<long double>::one = 1.0;
const long double NumTraits<long double>::zero= 0.0;
const long double    NumTraits<long double>::qNaN = std::numeric_limits<long double>::quiet_NaN();
const long double    NumTraits<long double>::sNaN = std::numeric_limits<long double>::signaling_NaN();
const long double NumTraits<long double>::infinity = std::numeric_limits<long double>::infinity();
const long double NumTraits<long double>::epsilon = LDBL_EPSILON;
const long double    NumTraits<long double>::min = -LDBL_MAX;
const long double    NumTraits<long double>::max = LDBL_MAX;
const long double    NumTraits<long double>::minStrictPositive = LDBL_MIN;
const long double    NumTraits<long double>::pi = LASS_NUM_PI;
const long double    NumTraits<long double>::e = LASS_NUM_E;
const long double    NumTraits<long double>::sqrt2 = LASS_NUM_SQRT_2;
const long double    NumTraits<long double>::sqrtPi = LASS_NUM_SQRT_PI;

// complex specialisation
template <> struct LASS_DLL NumTraits< std::complex<long double> >;
const int   NumTraits<std::complex<long double> >::memorySize = sizeof(std::complex<long double>);
const std::complex<long double>  NumTraits<std::complex<long double> >::one = std::complex<long double>(1.0, 0.0);
const std::complex<long double>  NumTraits<std::complex<long double> >::zero = std::complex<long double>(0.0, 0.0);
const std::complex<long double>  NumTraits<std::complex<long double> >::pi = LASS_NUM_PI;
const std::complex<long double>  NumTraits<std::complex<long double> >::e = LASS_NUM_E;
const std::complex<long double>  NumTraits<std::complex<long double> >::sqrt2 = LASS_NUM_SQRT_2;
const std::complex<long double>  NumTraits<std::complex<long double> >::sqrtPi = LASS_NUM_SQRT_PI;

/***********************************************************************
* unsigned/signed char/short/int/long num trait
*/

/** code generating macro for integral types */
#define LASS_NUM_TRAITS_INTEGRAL_TEMPLATE_SPEC( t_type, v_min, v_max ) \
template <> struct LASS_DLL NumTraits<t_type>;\
const int   NumTraits<t_type>::memorySize = sizeof(t_type);\
const int   NumTraits<t_type>::mantisseSize = 0;\
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
