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



#include "num_common.h"
#include "interval.h"

namespace lass
{
namespace num
{

#ifdef LASS_NO_TEMPLATE_PARTIAL_SPECIALIZATION

// partial specialization for float

template<> const int    NumTraits<interval<float> >::memorySize = sizeof(interval<float>);
template<> const bool   NumTraits<interval<float> >::isIntegral = false;
template<> const bool   NumTraits<interval<float> >::isNative = false;
template<> const bool   NumTraits<interval<float> >::isSigned = true;
template<> const bool   NumTraits<interval<float> >::hasInfinity = false;
template<> const std::string    NumTraits<interval<float> >::name() { return std::string("interval<")+NumTraits<baseType>::name()+ std::string(">"); }

template<> const interval<float>    NumTraits<interval<float> >::one = interval<float>( NumTraits<float>::one );
template<> const interval<float>    NumTraits<interval<float> >::zero = interval<float>( NumTraits<float>::zero );

template<> const interval<float>    NumTraits<interval<float> >::epsilon = interval<float>( NumTraits<float>::epsilon );
template<> const interval<float>    NumTraits<interval<float> >::min = interval<float>( NumTraits<float>::min );
template<> const interval<float>    NumTraits<interval<float> >::max = interval<float>( NumTraits<float>::max );
template<> const interval<float>    NumTraits<interval<float> >::minStrictPositive = interval<float>( NumTraits<float>::minStrictPositive );
template<> const interval<float>    NumTraits<interval<float> >::pi = interval<float>( NumTraits<float>::pi );
template<> const interval<float>    NumTraits<interval<float> >::e = interval<float>( NumTraits<float>::e );
template<> const interval<float>    NumTraits<interval<float> >::sqrt2 = interval<float>( NumTraits<float>::sqrt2 );
template<> const interval<float>    NumTraits<interval<float> >::sqrtPi = interval<float>( NumTraits<float>::sqrtPi );


// partial specialization for double

template<> const int    NumTraits<interval<double> >::memorySize = sizeof(interval<double>);
template<> const bool   NumTraits<interval<double> >::isIntegral = false;
template<> const bool   NumTraits<interval<double> >::isNative = false;
template<> const bool   NumTraits<interval<double> >::isSigned = true;
template<> const bool   NumTraits<interval<double> >::hasInfinity = false;
template<> const std::string    NumTraits<interval<double> >::name() { return std::string("interval<")+NumTraits<baseType>::name()+ std::string(">"); }

template<> const interval<double>   NumTraits<interval<double> >::one = interval<double>( NumTraits<double>::one );
template<> const interval<double>   NumTraits<interval<double> >::zero = interval<double>( NumTraits<double>::zero );

template<> const interval<double>   NumTraits<interval<double> >::epsilon = interval<double>( NumTraits<double>::epsilon );
template<> const interval<double>   NumTraits<interval<double> >::min = interval<double>( NumTraits<double>::min );
template<> const interval<double>   NumTraits<interval<double> >::max = interval<double>( NumTraits<double>::max );
template<> const interval<double>   NumTraits<interval<double> >::minStrictPositive = interval<double>( NumTraits<double>::minStrictPositive );
template<> const interval<double>   NumTraits<interval<double> >::pi = interval<double>( NumTraits<double>::pi );
template<> const interval<double>   NumTraits<interval<double> >::e = interval<double>( NumTraits<double>::e );
template<> const interval<double>   NumTraits<interval<double> >::sqrt2 = interval<double>( NumTraits<double>::sqrt2 );
template<> const interval<double>   NumTraits<interval<double> >::sqrtPi = interval<double>( NumTraits<double>::sqrtPi );

#endif


}
}
