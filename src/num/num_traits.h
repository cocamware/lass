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



#ifndef LASS_GUARDIAN_OF_INCLUSION_NUM_TRAITS_H
#define LASS_GUARDIAN_OF_INCLUSION_NUM_TRAITS_H

#include "num_common.h"
#include "../util/call_traits.h"

namespace lass
{
namespace num
{

template<class C>
struct NumTraits
{
	typedef C	selfType;				/**< our own type */
	typedef C	baseType;				/**< is the base type where selfType is based on */
	typedef C	intervalType;			/**< type of the support, useful for distributions */
	
	static const bool  isDistribution;	/**< true for distribution like types */
	static const bool  isIntegral;		/**< true for integral types, ie supporting ++ and -- fi */
	static const bool  isNative;		/**< true for the native types, ie char, int , ... */
	static const bool  isSigned;		/**< true for signed types */
	static const bool  hasInfinity;		/**< true for types having infinity */
	static const bool  hasNaN;			/**< true for types have the concept of Not a Number */
	static const int   memorySize;		/**< memory footprint */
	static const std::string name() { return "unknown"; }	/**< name of the selfType */

	static const C	one;				/**< definition of one */
	static const C	zero;				/**< definition of zero */

	/* some constants */
	static const C	sNaN;				/**< signaling Nan, see http://research.microsoft.com/~hollasch/cgindex/coding/ieeefloat.html */
	static const C	qNaN;				/**< quiet NaN, see http://research.microsoft.com/~hollasch/cgindex/coding/ieeefloat.html */
	static const C	epsilon;			/**< the smallest positive value such that one + epsilon != one*/
	static const C infinity;			/**< infinity */
	static const C min;					/**< the smallest value representable */
	static const C	max;				/**< the largest positive value */
	static const C minStrictPositive;	/**< the smallest strictly positive value */
	static const C pi;					
	static const C e;
	static const C sqrt2;
	static const C sqrtPi;
};

template<class C>
bool isNaN( const C& iV )
{
	return iV!=iV;
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
	typedef ttype	selfType;\
	typedef ttype	baseType;\
	typedef ttype	intervalType;\
	static const bool  isDistribution;\
	static const bool  isIntegral;\
	static const bool  isNative;\
	static const bool  isSigned;\
	static const bool  hasInfinity;\
	static const bool  hasNaN;\
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
	typedef ttype	selfType;\
	typedef ttype::value_type	baseType;\
	typedef ttype	intervalType;\
	static const bool  isDistribution;\
	static const bool  isIntegral;\
	static const bool  isNative;\
	static const bool  isSigned;\
	static const bool  hasInfinity;\
	static const bool  hasNaN;\
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

bool isNaN( float iV );

LASS_NUM_DECLARE_FLOATING_TRAITS( double , "double" )
LASS_NUM_DECLARE_COMPLEX_FLOATING_TRAITS( std::complex< double > , "complex<double>" )

bool isNaN( double iV );


#define LASS_NUM_DECLARE_INTEGRAL_TRAITS( type, tname ) \
template<> \
struct NumTraits<type> \
{\
	typedef type selfType;\
	typedef type baseType;\
	typedef float	intervalType;\
	static const bool  isDistribution;\
	static const bool  isIntegral;\
	static const bool  isNative;\
	static const bool  isSigned;\
	static const bool  hasInfinity;\
	static const bool  hasNaN;\
	static const int   memorySize;\
	static const std::string name() { return tname ; }\
	static const type one;\
	static const type zero;\
	static const type epsilon;\
	static const type min;\
	static const type max;\
	static const type minStrictPositive;\
};

LASS_NUM_DECLARE_INTEGRAL_TRAITS( char, "char" ) 
LASS_NUM_DECLARE_INTEGRAL_TRAITS( signed char, "signed char" ) 
LASS_NUM_DECLARE_INTEGRAL_TRAITS( unsigned char, "unsigned char" ) 
LASS_NUM_DECLARE_INTEGRAL_TRAITS( signed short, "signed short" ) 
LASS_NUM_DECLARE_INTEGRAL_TRAITS( unsigned short, "unsigned short" ) 
LASS_NUM_DECLARE_INTEGRAL_TRAITS( signed int, "signed int" ) 
LASS_NUM_DECLARE_INTEGRAL_TRAITS( unsigned int, "unsigned int" ) 
LASS_NUM_DECLARE_INTEGRAL_TRAITS( signed long, "signed long" ) 
LASS_NUM_DECLARE_INTEGRAL_TRAITS( unsigned long, "unsigned long" ) 


}

}

#endif
