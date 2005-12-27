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


#ifndef LASS_GUARDIAN_OF_INCLUSION_NUM_TRAITS_H
#define LASS_GUARDIAN_OF_INCLUSION_NUM_TRAITS_H

#include "num_common.h"
#include "../util/call_traits.h"

#ifdef LASS_ISNAN_MSVC_FLOAT_H
#	include <float.h>
#endif

namespace lass
{
namespace num
{

template<class C>
struct LASS_DLL NumTraits
{
	typedef C   selfType;               /**< our own type */
	typedef C   baseType;               /**< is the base type where selfType is based on */
	typedef C   intervalType;           /**< type of the support, useful for distributions */

	enum
	{
		isDistribution = 0,	/**< true for distribution like types */
		isIntegral = 0,		/**< true for integral types, ie supporting ++ and -- fi */
		isNative = 0,       /**< true for the native types, ie char, int , ... */
		isSigned = 0,		/**< true for signed types */
		hasInfinity = 0,    /**< true for types having infinity */
		hasNaN = 0,			/**< true for types have the concept of Not a Number */
	};

	static const int   memorySize;      /**< memory footprint */
	static const std::string name() { return "unknown"; }   /**< name of the selfType */

	static const C  one;                /**< definition of one */
	static const C  zero;               /**< definition of zero */

	/* some constants */
	static const C  sNaN;               /**< signaling Nan, see http://research.microsoft.com/~hollasch/cgindex/coding/ieeefloat.html */
	static const C  qNaN;               /**< quiet NaN, see http://research.microsoft.com/~hollasch/cgindex/coding/ieeefloat.html */
	static const C  epsilon;            /**< the smallest positive value such that one + epsilon != one*/
	static const C infinity;            /**< infinity */
	static const C min;                 /**< the smallest value representable */
	static const C  max;                /**< the largest positive value */
	static const C minStrictPositive;   /**< the smallest strictly positive value */
	static const C pi;
	static const C e;
	static const C sqrt2;
	static const C sqrtPi;
};

template<class C> inline
bool isNaN( const C& iV )
{
#ifdef LASS_ISNAN_MSVC_FLOAT_H
	return NumTraits<C>::hasNaN && _isnan(static_cast<double>(iV)) != 0;
#else
	return iV != iV;
#endif
}

/** return true if iV equals minus or plus Infinity
 */
template<class C>
bool isInf( const C& iV )
{
	return NumTraits<C>::hasInfinity &&
		(iV == NumTraits<C>::infinity || iV == -NumTraits<C>::infinity);
}

#define LASS_NUM_DECLARE_FLOATING_TRAITS( ttype, tname ) \
template<> \
struct NumTraits<ttype>\
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
	};\
	static const int   memorySize;\
	static const std::string name() { return tname ; }\
	static const ttype one;\
	static const ttype zero;\
	static const ttype infinity;\
	static const ttype qNaN;\
	static const ttype sNaN;\
	static const ttype epsilon;\
	static const ttype min;\
	static const ttype max;\
	static const ttype minStrictPositive;\
	static const ttype pi;\
	static const ttype e;\
	static const ttype sqrt2;\
	static const ttype sqrtPi;\
};

#define LASS_NUM_DECLARE_COMPLEX_FLOATING_TRAITS( ttype, tname ) \
template<> \
struct NumTraits< ttype >\
{\
	typedef ttype   selfType;\
	typedef ttype::value_type   baseType;\
	typedef ttype   intervalType;\
	enum\
	{\
		isDistribution = false,\
		isIntegral = false,\
		isNative = false,\
		isSigned = true,\
		hasInfinity = false,\
		hasNaN = true,\
	};\
	static const int   memorySize;\
	static const std::string name() { return tname ; }\
	static const ttype one;\
	static const ttype zero;\
	static const ttype pi;\
	static const ttype e;\
	static const ttype sqrt2;\
	static const ttype sqrtPi;\
};

LASS_NUM_DECLARE_FLOATING_TRAITS( float, "float" )
LASS_NUM_DECLARE_COMPLEX_FLOATING_TRAITS( std::complex< float > , "complex<float>" )

LASS_NUM_DECLARE_FLOATING_TRAITS( double , "double" )
LASS_NUM_DECLARE_COMPLEX_FLOATING_TRAITS( std::complex< double > , "complex<double>" )

template<>
struct NumTraits<char>
{
	typedef char selfType;
	typedef char baseType;
	typedef float   intervalType;
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
		hasNaN = 0
	};
	static const int   memorySize;
	static const std::string name() { return LASS_STRINGIFY(char); }
	static const selfType one;
	static const selfType zero;
	static const selfType epsilon;
	static const selfType min;
	static const selfType max;
	static const selfType minStrictPositive;
};

#define LASS_NUM_DECLARE_INTEGRAL_TRAITS( sign, type, is_signed ) \
template<> \
struct NumTraits<sign type> \
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
		hasNaN = 0\
	};\
	static const int   memorySize;\
	static const std::string name() { return LASS_STRINGIFY(sign type); }\
	static const selfType one;\
	static const selfType zero;\
	static const selfType epsilon;\
	static const selfType min;\
	static const selfType max;\
	static const selfType minStrictPositive;\
};

LASS_NUM_DECLARE_INTEGRAL_TRAITS( signed, char, 1 )
LASS_NUM_DECLARE_INTEGRAL_TRAITS( unsigned, char, 0 )
LASS_NUM_DECLARE_INTEGRAL_TRAITS( signed, short, 1 )
LASS_NUM_DECLARE_INTEGRAL_TRAITS( unsigned, short, 0 )
LASS_NUM_DECLARE_INTEGRAL_TRAITS( signed, int, 1 )
LASS_NUM_DECLARE_INTEGRAL_TRAITS( unsigned, int, 0 )
LASS_NUM_DECLARE_INTEGRAL_TRAITS( signed, long, 1 )
LASS_NUM_DECLARE_INTEGRAL_TRAITS( unsigned, long, 0 )

}

}

// we didn't include basic_ops.h in num_common.h when including num_traits.h,
// because basic_ops.h needs NumTraits to be defined first.  So now it's time
// to include basic_ops.h anyway [Bramz]
//
#include "basic_ops.h"

#endif
