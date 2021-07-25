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


#ifndef LASS_GUARDIAN_OF_INCLUSION_NUM_TRAITS_H
#define LASS_GUARDIAN_OF_INCLUSION_NUM_TRAITS_H

#include "num_common.h"

#if LASS_HAVE_STD_ISNAN
#	include <cmath>
#elif LASS_NUM_NUM_TRAITS_HAVE_MSVC_FLOAT_H
#	include <float.h>
#else
#	include <math.h>
#endif

namespace lass
{
namespace num
{

template<class C>
struct LASS_DLL NumTraits
{
	//typedef C   selfType;               /**< our own type */
	//typedef C   baseType;               /**< is the base type where selfType is based on */
	//typedef C   intervalType;           /**< type of the support, useful for distributions */

	//enum
	//{
	//	isDistribution = 0,	/**< true for distribution like types */
	//	isIntegral = 0,		/**< true for integral types, ie supporting ++ and -- fi */
	//	isNative = 0,       /**< true for the native types, ie char, int , ... */
	//	isSigned = 0,		/**< true for signed types */
	//	hasInfinity = 0,    /**< true for types having infinity */
	//	hasNaN = 0,			/**< true for types have the concept of Not a Number */
	//	isFloatingPoint = 0 /**< true for floating point types */
	//};

	//static const size_t   memorySize;      /**< memory footprint */
	//static const size_t   mantisseSize;	/**< number of bits used for mantisse, useful for error analysis */
	//static const std::string name() { return "unknown"; }   /**< name of the selfType */

	//static const C  one;                /**< definition of one */
	//static const C  zero;               /**< definition of zero */

	/* some constants */
	//static const C  sNaN;               /**< signaling Nan, see http://research.microsoft.com/~hollasch/cgindex/coding/ieeefloat.html */
	//static const C  qNaN;               /**< quiet NaN, see http://research.microsoft.com/~hollasch/cgindex/coding/ieeefloat.html */
	//static const C  epsilon;            /**< the smallest positive value such that one + epsilon != one*/
	//static const C infinity;            /**< infinity */
	//static const C min;                 /**< the smallest value representable */
	//static const C  max;                /**< the largest positive value */
	//static const C minStrictPositive;   /**< the smallest strictly positive value */
	//static const C pi;
	//static const C e;
	//static const C sqrt2;
	//static const C sqrtPi;
};


template<class C> inline
bool isNaN( C iV )
{
    return iV != iV;
}

#if LASS_HAVE_STD_ISNAN
inline bool isNaN( float iV ) { return std::isnan(iV); }
inline bool isNaN( double iV ) { return std::isnan(iV); }
inline bool isNaN( long double iV ) { return std::isnan(iV); }
#elif LASS_NUM_NUM_TRAITS_HAVE_MSVC_FLOAT_H
inline bool isNaN( float iV ) { return _isnan(static_cast<double>(iV)) != 0; }
inline bool isNaN( double iV ) { return _isnan(iV) != 0; }
inline bool isNaN( long double iV ) { return _isnan(static_cast<double>(iV)) != 0; }
#else
inline bool isNaN( float iV ) { return isnan(iV) != 0; }
inline bool isNaN( double iV ) { return isnan(iV) != 0; }
inline bool isNaN( long double iV ) { return isnan(static_cast<double>(iV)) != 0; }
#endif

template<class C> inline
bool isNaN( std::complex<C> iV )
{
    return isNaN(iV.real()) || isNaN(iV.imag());
}


/** return true if iV equals minus or plus Infinity
 */
template<class C>
bool isInf( const C& iV )
{
	return NumTraits<C>::hasInfinity &&
		(iV == NumTraits<C>::infinity || iV == -NumTraits<C>::infinity);
}

inline bool isInf(float iV) { return std::isinf(iV); }
inline bool isInf(double iV) { return std::isinf(iV); }
inline bool isInf(long double iV) { return std::isinf(iV); }

#define LASS_NUM_PI			3.141592653589793238462643383279502884197169399375105820974944592307816406286208998628034825342117067
#define LASS_NUM_E			2.718281828459045235360287471352662497757247093699959574966967627724076630353547594571382178525166427
#define LASS_NUM_SQRT_2		1.414213562373095048801688724209698078569671875376948073176679737990732478462107038850387534327641572
#define LASS_NUM_SQRT_PI	1.772453850905516027298167483341145

#define LASS_NUM_DECLARE_FLOATING_TRAITS( ttype, tname ) \
template<> \
struct LASS_DLL NumTraits<ttype>\
{\
	typedef ttype   selfType;\
	typedef ttype   baseType;\
	typedef ttype   intervalType;\
	enum\
	{\
		isDistribution = false,\
		isIntegral = false,\
		isNative = true,\
		isSigned = true,\
		hasInfinity = true,\
		hasNaN = true,\
		isFloatingPoint = true\
	};\
	static constexpr size_t memorySize = sizeof(ttype);\
	static constexpr size_t mantisseSize = std::numeric_limits<ttype>::digits;\
	static const std::string name() { return tname ; }\
	static constexpr ttype one = static_cast<ttype>(1);\
	static constexpr ttype zero = static_cast<ttype>(0);\
	static constexpr ttype infinity = std::numeric_limits<ttype>::infinity();\
	static constexpr ttype qNaN = std::numeric_limits<ttype>::quiet_NaN();\
	static constexpr ttype sNaN = std::numeric_limits<ttype>::signaling_NaN();\
	static constexpr ttype epsilon= std::numeric_limits<ttype>::epsilon();\
	static constexpr ttype min = -std::numeric_limits<ttype>::max();\
	static constexpr ttype max = std::numeric_limits<ttype>::max();\
	static constexpr ttype minStrictPositive = std::numeric_limits<ttype>::min();\
	static constexpr ttype pi = static_cast<ttype>(LASS_NUM_PI);\
	static constexpr ttype e = static_cast<ttype>(LASS_NUM_E);\
	static constexpr ttype sqrt2 = static_cast<ttype>(LASS_NUM_SQRT_2);\
	static constexpr ttype sqrtPi = static_cast<ttype>(LASS_NUM_SQRT_PI);\
};

#define LASS_NUM_DECLARE_COMPLEX_FLOATING_TRAITS( ttype, tname ) \
template<> \
struct LASS_DLL NumTraits< ttype >\
{\
	typedef ttype selfType;\
	typedef ttype::value_type baseType;\
	typedef ttype intervalType;\
	enum\
	{\
		isDistribution = false,\
		isIntegral = false,\
		isNative = false,\
		isSigned = true,\
		hasInfinity = false,\
		hasNaN = false,\
		isFloatingPoint = NumTraits< baseType >::isFloatingPoint\
	};\
	static constexpr size_t memorySize = sizeof(ttype);\
	static constexpr size_t mantisseSize = NumTraits< baseType >::mantisseSize;\
	static const std::string name() { return tname ; }\
	static constexpr ttype one = ttype( NumTraits< baseType >::one );\
	static constexpr ttype zero = ttype( NumTraits< baseType >::zero );\
	static constexpr ttype pi = ttype( NumTraits< baseType >::pi );\
	static constexpr ttype e = ttype( NumTraits< baseType >::e );\
	static constexpr ttype sqrt2 = ttype( NumTraits< baseType >::sqrt2 );\
	static constexpr ttype sqrtPi = ttype( NumTraits< baseType >::sqrtPi );\
};

LASS_NUM_DECLARE_FLOATING_TRAITS( float, "float" )
LASS_NUM_DECLARE_COMPLEX_FLOATING_TRAITS( std::complex< float > , "complex<float>" )

LASS_NUM_DECLARE_FLOATING_TRAITS( double , "double" )
LASS_NUM_DECLARE_COMPLEX_FLOATING_TRAITS( std::complex< double > , "complex<double>" )

LASS_NUM_DECLARE_FLOATING_TRAITS( long double, "long double" )
LASS_NUM_DECLARE_COMPLEX_FLOATING_TRAITS( std::complex< long double > , "complex<long double>" )

template<>
struct LASS_DLL NumTraits<char>
{
	typedef char selfType;
	typedef char baseType;
	typedef float intervalType;
	typedef signed char signedType;
	typedef unsigned char unsignedType;
	enum
	{
		isDistribution = 0,
		isIntegral = 1,
		isNative = 1,
#ifdef LASS_CHAR_IS_SIGNED
		isSigned = 1,
#else
		isSigned = 0,
#endif
		hasInfinity = 0,
		hasNaN = 0,
		isFloatingPoint = 0
	};
	static constexpr size_t memorySize = sizeof(char);
	static constexpr size_t mantisseSize = 0;
	static const std::string name() { return LASS_STRINGIFY(char); }
	static constexpr selfType one = 1;
	static constexpr selfType zero = 0;
	static constexpr selfType epsilon = 1;
	static constexpr selfType min = std::numeric_limits<char>::min();
	static constexpr selfType max = std::numeric_limits<char>::max();
	static constexpr selfType minStrictPositive = 1;
};

#define LASS_NUM_DECLARE_INTEGRAL_TRAITS( sign, type, is_signed ) \
template<> \
struct LASS_DLL NumTraits<sign type> \
{\
	typedef sign type selfType;\
	typedef sign type baseType;\
	typedef float   intervalType;\
	typedef signed type signedType;\
	typedef unsigned type unsignedType;\
	enum\
	{\
		isDistribution = 0,\
		isIntegral = 1,\
		isNative = 1,\
		isSigned = is_signed,\
		hasInfinity = 0,\
		hasNaN = 0,\
		isFloatingPoint = 0\
	};\
	static constexpr size_t memorySize = sizeof(sign type);\
	static constexpr size_t mantisseSize = 0;\
	static const std::string name() { return LASS_STRINGIFY(sign type); }\
	static constexpr selfType one = 1;\
	static constexpr selfType zero = 0;\
	static constexpr selfType epsilon = 1;\
	static constexpr selfType min = std::numeric_limits<sign type>::min();\
	static constexpr selfType max = std::numeric_limits<sign type>::max();\
	static constexpr selfType minStrictPositive = 1;\
};

LASS_NUM_DECLARE_INTEGRAL_TRAITS( signed, char, 1 )
LASS_NUM_DECLARE_INTEGRAL_TRAITS( unsigned, char, 0 )
LASS_NUM_DECLARE_INTEGRAL_TRAITS( signed, short, 1 )
LASS_NUM_DECLARE_INTEGRAL_TRAITS( unsigned, short, 0 )
LASS_NUM_DECLARE_INTEGRAL_TRAITS( signed, int, 1 )
LASS_NUM_DECLARE_INTEGRAL_TRAITS( unsigned, int, 0 )
LASS_NUM_DECLARE_INTEGRAL_TRAITS( signed, long, 1 )
LASS_NUM_DECLARE_INTEGRAL_TRAITS( unsigned, long, 0 )
#if LASS_HAVE_LONG_LONG
	LASS_NUM_DECLARE_INTEGRAL_TRAITS( signed, long long, 1 )
	LASS_NUM_DECLARE_INTEGRAL_TRAITS( unsigned, long long, 0 )
#elif LASS_PLATFORM_TYPE == LASS_PLATFORM_TYPE_WIN32
	LASS_NUM_DECLARE_INTEGRAL_TRAITS( signed, __int64, 1 )
	LASS_NUM_DECLARE_INTEGRAL_TRAITS( unsigned, __int64, 0 )
#endif

}

}

// we didn't include basic_ops.h in num_common.h when including num_traits.h,
// because basic_ops.h needs NumTraits to be defined first.  So now it's time
// to include basic_ops.h anyway [Bramz]
//
#include "basic_ops.h"

#endif
