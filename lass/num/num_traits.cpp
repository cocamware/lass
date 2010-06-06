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



#include "lass_common.h"
#include "num_traits.h"
#include "interval.h"
#include <climits>
#include <cfloat>

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
const size_t   NumTraits<float>::memorySize = sizeof(float);
const size_t   NumTraits<float>::mantisseSize = std::numeric_limits<float>::digits;			
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
const size_t   NumTraits<std::complex<float> >::memorySize = sizeof(std::complex<float>);
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
const size_t   NumTraits<double>::memorySize = sizeof(double);
const size_t   NumTraits<double>::mantisseSize = std::numeric_limits<double>::digits;
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
const size_t   NumTraits<std::complex<double> >::memorySize = sizeof(std::complex<double>);
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
const size_t   NumTraits<long double>::memorySize = sizeof(long double);
const size_t   NumTraits<long double>::mantisseSize = std::numeric_limits<long double>::digits;
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
const size_t   NumTraits<std::complex<long double> >::memorySize = sizeof(std::complex<long double>);
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
const size_t   NumTraits<t_type>::memorySize = sizeof(t_type);\
const size_t   NumTraits<t_type>::mantisseSize = 0;\
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
#if LASS_HAVE_LONG_LONG
	LASS_NUM_TRAITS_INTEGRAL_TEMPLATE_SPEC( unsigned long long, 0 , ULLONG_MAX )
	LASS_NUM_TRAITS_INTEGRAL_TEMPLATE_SPEC( signed long long, LLONG_MIN, LLONG_MAX )
#elif LASS_PLATFORM_TYPE == LASS_PLATFORM_TYPE_WIN32
	LASS_NUM_TRAITS_INTEGRAL_TEMPLATE_SPEC( unsigned __int64, 0 , ULLONG_MAX )
	LASS_NUM_TRAITS_INTEGRAL_TEMPLATE_SPEC( signed __int64, LLONG_MIN, LLONG_MAX )
#endif

}
}
