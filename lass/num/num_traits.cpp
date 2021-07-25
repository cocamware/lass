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
 *	Copyright (C) 2004-2021 the Initial Developer.
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
constexpr size_t NumTraits<float>::memorySize;
constexpr size_t NumTraits<float>::mantisseSize;
constexpr float NumTraits<float>::one;
constexpr float NumTraits<float>::zero;
constexpr float NumTraits<float>::qNaN;
constexpr float NumTraits<float>::sNaN;
constexpr float NumTraits<float>::infinity;
constexpr float NumTraits<float>::epsilon;
constexpr float NumTraits<float>::min;
constexpr float NumTraits<float>::max ;
constexpr float NumTraits<float>::minStrictPositive;
constexpr float NumTraits<float>::pi;
constexpr float NumTraits<float>::e;
constexpr float NumTraits<float>::sqrt2 ;
constexpr float NumTraits<float>::sqrtPi;

// complex specialisation
template <> struct LASS_DLL NumTraits< std::complex<float> >;
constexpr size_t NumTraits< std::complex<float> >::memorySize;
constexpr size_t NumTraits< std::complex<float> >::mantisseSize;
constexpr std::complex<float> NumTraits< std::complex<float> >::one;
constexpr std::complex<float> NumTraits< std::complex<float> >::zero;
constexpr std::complex<float> NumTraits< std::complex<float> >::pi;
constexpr std::complex<float> NumTraits< std::complex<float> >::e;
constexpr std::complex<float> NumTraits< std::complex<float> >::sqrt2;
constexpr std::complex<float> NumTraits< std::complex<float> >::sqrtPi;

/***********************************************************************
* double num trait
*/
template <> struct LASS_DLL NumTraits<double>;
constexpr size_t NumTraits<double>::memorySize;
constexpr size_t NumTraits<double>::mantisseSize;
constexpr double NumTraits<double>::one;
constexpr double NumTraits<double>::zero;
constexpr double NumTraits<double>::qNaN;
constexpr double NumTraits<double>::sNaN;
constexpr double NumTraits<double>::infinity;
constexpr double NumTraits<double>::epsilon;
constexpr double NumTraits<double>::min;
constexpr double NumTraits<double>::max;
constexpr double NumTraits<double>::minStrictPositive;
constexpr double NumTraits<double>::pi;
constexpr double NumTraits<double>::e;
constexpr double NumTraits<double>::sqrt2;
constexpr double NumTraits<double>::sqrtPi;

// complex specialisation
template <> struct LASS_DLL NumTraits< std::complex<double> >;
constexpr size_t NumTraits< std::complex<double> >::memorySize;
constexpr size_t NumTraits< std::complex<double> >::mantisseSize;
constexpr std::complex<double> NumTraits< std::complex<double> >::one;
constexpr std::complex<double> NumTraits< std::complex<double> >::zero;
constexpr std::complex<double> NumTraits< std::complex<double> >::pi;
constexpr std::complex<double> NumTraits< std::complex<double> >::e;
constexpr std::complex<double> NumTraits< std::complex<double> >::sqrt2;
constexpr std::complex<double> NumTraits< std::complex<double> >::sqrtPi;

/***********************************************************************
* long double num trait
*/
template <> struct LASS_DLL NumTraits<long double>;
constexpr size_t NumTraits<long double>::memorySize;
constexpr size_t NumTraits<long double>::mantisseSize;
constexpr long double NumTraits<long double>::one;
constexpr long double NumTraits<long double>::zero;
constexpr long double NumTraits<long double>::qNaN;
constexpr long double NumTraits<long double>::sNaN;
constexpr long double NumTraits<long double>::infinity;
constexpr long double NumTraits<long double>::epsilon;
constexpr long double NumTraits<long double>::min;
constexpr long double NumTraits<long double>::max;
constexpr long double NumTraits<long double>::minStrictPositive;
constexpr long double NumTraits<long double>::pi;
constexpr long double NumTraits<long double>::e;
constexpr long double NumTraits<long double>::sqrt2;
constexpr long double NumTraits<long double>::sqrtPi;

// complex specialisation
template <> struct LASS_DLL NumTraits< std::complex<long double> >;
constexpr size_t NumTraits< std::complex<long double> >::memorySize;
constexpr size_t NumTraits< std::complex<long double> >::mantisseSize;
constexpr std::complex<long double> NumTraits< std::complex<long double> >::one;
constexpr std::complex<long double> NumTraits< std::complex<long double> >::zero;
constexpr std::complex<long double> NumTraits< std::complex<long double> >::pi;
constexpr std::complex<long double> NumTraits< std::complex<long double> >::e;
constexpr std::complex<long double> NumTraits< std::complex<long double> >::sqrt2;
constexpr std::complex<long double> NumTraits< std::complex<long double> >::sqrtPi;


/***********************************************************************
* unsigned/signed char/short/int/long num trait
*/

/** code generating macro for integral types */
#define LASS_NUM_TRAITS_INTEGRAL_TEMPLATE_SPEC( t_type ) \
template <> struct LASS_DLL NumTraits<t_type>;\
constexpr size_t NumTraits<t_type>::memorySize;\
constexpr size_t NumTraits<t_type>::mantisseSize;\
constexpr t_type NumTraits<t_type>::one ;\
constexpr t_type NumTraits<t_type>::zero;\
constexpr t_type NumTraits<t_type>::epsilon;\
constexpr t_type NumTraits<t_type>::min;\
constexpr t_type NumTraits<t_type>::max ;\
constexpr t_type NumTraits<t_type>::minStrictPositive;

LASS_NUM_TRAITS_INTEGRAL_TEMPLATE_SPEC(char)
LASS_NUM_TRAITS_INTEGRAL_TEMPLATE_SPEC(unsigned char)
LASS_NUM_TRAITS_INTEGRAL_TEMPLATE_SPEC(signed char)
LASS_NUM_TRAITS_INTEGRAL_TEMPLATE_SPEC(unsigned short)
LASS_NUM_TRAITS_INTEGRAL_TEMPLATE_SPEC(signed short)
LASS_NUM_TRAITS_INTEGRAL_TEMPLATE_SPEC(unsigned int)
LASS_NUM_TRAITS_INTEGRAL_TEMPLATE_SPEC(signed int)
LASS_NUM_TRAITS_INTEGRAL_TEMPLATE_SPEC(unsigned long)
LASS_NUM_TRAITS_INTEGRAL_TEMPLATE_SPEC(signed long)
#if LASS_HAVE_LONG_LONG
	LASS_NUM_TRAITS_INTEGRAL_TEMPLATE_SPEC(unsigned long long)
	LASS_NUM_TRAITS_INTEGRAL_TEMPLATE_SPEC(signed long long)
#elif LASS_PLATFORM_TYPE == LASS_PLATFORM_TYPE_WIN32
	LASS_NUM_TRAITS_INTEGRAL_TEMPLATE_SPEC(unsigned __int64)
	LASS_NUM_TRAITS_INTEGRAL_TEMPLATE_SPEC(signed __int64)
#endif
	}
}
