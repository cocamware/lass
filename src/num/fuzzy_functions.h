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



#ifndef LASS_GUARDIAN_OF_INCLUSION_NUM_FUZZY_FUNCTIONS_H
#define LASS_GUARDIAN_OF_INCLUSION_NUM_FUZZY_FUNCTIONS_H

#include "num_common.h"
#include "num_traits.h"
#include "../util/call_traits.h"
#include "fn_sampled8.h"

namespace lass
{

namespace num
{
	template<class C>   inline void createUnit( C& oValue );
	template<class C>   inline void createZero( C& oValue );

#define NUM_LASS_GENERATE_FUZZY_FUNCTIONS_DECLARATIONS( TYPE )  \
	void createGaussian( TYPE& oValue, util::CallTraits< NumTraits<TYPE>::baseType >::TParam iMean,\
															   util::CallTraits< NumTraits<TYPE>::baseType >::TParam iStdDev);\
	void createTriangular( TYPE& oValue,  util::CallTraits< NumTraits<TYPE>::baseType >::TParam iLeft,\
																  util::CallTraits< NumTraits<TYPE>::baseType >::TParam iMid,\
																  util::CallTraits< NumTraits<TYPE>::baseType >::TParam iRight);\
	void createTrapezoidal( TYPE& oValue,     util::CallTraits< NumTraits<TYPE>::baseType >::TParam iA,\
																	  util::CallTraits< NumTraits<TYPE>::baseType >::TParam iB,\
																	  util::CallTraits< NumTraits<TYPE>::baseType >::TParam iC,\
																	  util::CallTraits< NumTraits<TYPE>::baseType >::TParam iD);\
	NumTraits<TYPE>::supportType    support( const TYPE& iValue );  \
	NumTraits<TYPE>::supportType    support( const TYPE& iValue, util::CallTraits< NumTraits<TYPE>::baseType >::TParam iAlpha );\
	NumTraits<TYPE>::baseType       memberShip( const TYPE& iValue, util::CallTraits< NumTraits<TYPE>::baseType >::TParam iX );\
	NumTraits<TYPE>::baseType       infinum( const TYPE& iValue );  \
	NumTraits<TYPE>::baseType       supinum( const TYPE& iValue );  \
	NumTraits<TYPE>::baseType       footprint( const TYPE& iValue ); \
	NumTraits<TYPE>::baseType       entropy( const TYPE& iValue ); \
	NumTraits<TYPE>::baseType       differentialEntropy( const TYPE& iValue ); \
	NumTraits<TYPE>::baseType       nonSpecificity( const TYPE& iValue ); \
	NumTraits<TYPE>::baseType       discord( const TYPE& iValue ); \
	NumTraits<TYPE>::baseType       bandwidth( const TYPE& iValue ); \
	NumTraits<TYPE>::baseType       bandwithRatio( const TYPE& iValue ); \
	NumTraits<TYPE>::baseType       maxMembership( const TYPE& iValue ); \
	NumTraits<TYPE>::baseType       centroid( const TYPE& iValue );

	#include "fuzzy_functions_default.inl"
	#include "fuzzy_functions_float.inl"
	#include "fuzzy_functions_double.inl"
	#include "fuzzy_functions_fn_sampled8.inl"
}
}

#endif
