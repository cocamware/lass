/** 
*   @author Bram de Greve (bramz@users.sourceforge.net)
*   @author Tom De Muer (tomdemuer@users.sourceforge.net)
*
*	Distributed under the terms of the GPL (GNU Public License)
*
* 	The LASS License:
*   
*	Copyright 2004 Bram de Greve and Tom De Muer
*
*   LASS is free software; you can redistribute it and/or modify
*   it under the terms of the GNU General Public License as published by
*   the Free Software Foundation; either version 2 of the License, or
*   (at your option) any later version.
*
*   This program is distributed in the hope that it will be useful,
*   but WITHOUT ANY WARRANTY; without even the implied warranty of
*   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
*   GNU General Public License for more details.
*
*   You should have received a copy of the GNU General Public License
*   along with this program; if not, write to the Free Software
*   Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
*


 *  @date 2004
 */

/** @class lass::util::Triple
 *  @brief extension of std::pair to three values, trait three values as a single unit.
 *  @author Bram de Greve [BdG]
 *
 *  we'd like to refer to the C++ standard library for info on std::pair.  Triple is an 
 *  extension of this to three values.  As std::pair, Triple is declared as a struct,
 *  the default constructor will initialize all elements with their default constructor,
 *  and implicit conversions between Triples of different type are allowed, provided that
 *  the conversions between the element types are possible.
 *
 *  As for std::pair, the "less than" comparison is implemented for Triple in a similar way.
 *  The first value has higher priority than the second and third, and the second value has higher
 *  priority than the third.  Thus, if the first values of two pairs differ, the result of their 
 *  comparison is used as the result of the comparison of the whole triples.  If the first values
 *  are equal, the comparison of the second values yields the result if they differ.  If the 
 *  second values are equal too, the comparison of the third values finally yields the result.
 */

#ifndef LASS_GUARDIAN_OF_INCLUSION_UTIL_TRIPLE_H
#define LASS_GUARDIAN_OF_INCLUSION_UTIL_TRIPLE_H

#include "util_common.h"

namespace lass
{
namespace util
{

template <typename T1, typename T2, typename T3>
struct Triple
{
    // typenames for the values
    typedef T1 first_type;
    typedef T2 second_type;
    typedef T3 third_type;

    // member
    T1 first;
    T2 second;
    T3 third;

    Triple();
    Triple(typename CallTraits<T1>::TParam iA, 
		   typename CallTraits<T2>::TParam iB, 
		   typename CallTraits<T3>::TParam iC);

    /** copy constructor with implicit conversions
     *  This template version of the copy constructor provided here is ussed when implicit 
     *  conversions are necessary.  If an object of type Triple gets copied, the normal implicitly
     *  generated default copy constructor is called, which is nice.
     */
    template <typename U1, typename U2, typename U3>
    Triple(const Triple<U1, U2, U3>& iOther):
        first(iOther.first),
        second(iOther.second),
        third(iOther.third)
    {
    }
};

template <typename T1, typename T2, typename T3, typename U1, typename U2, typename U3>
bool operator==(const Triple<T1, T2, T3>& iA, const Triple<U1, U2, U3>& iB);

template <typename T1, typename T2, typename T3, typename U1, typename U2, typename U3>
bool operator!=(const Triple<T1, T2, T3>& iA, const Triple<U1, U2, U3>& iB);

template <typename T1, typename T2, typename T3, typename U1, typename U2, typename U3>
bool operator<(const Triple<T1, T2, T3>& iA, const Triple<U1, U2, U3>& iB);

template <typename T1, typename T2, typename T3, typename U1, typename U2, typename U3>
bool operator>(const Triple<T1, T2, T3>& iA, const Triple<U1, U2, U3>& iB);

template <typename T1, typename T2, typename T3, typename U1, typename U2, typename U3>
bool operator<=(const Triple<T1, T2, T3>& iA, const Triple<U1, U2, U3>& iB);

template <typename T1, typename T2, typename T3, typename U1, typename U2, typename U3>
bool operator>=(const Triple<T1, T2, T3>& iA, const Triple<U1, U2, U3>& iB);



template <typename T1, typename T2, typename T3>
std::ostream& operator<<(std::ostream& ioS, const Triple<T1, T2, T3>& iTriple);



template <typename T1, typename T2, typename T3>
Triple<T1, T2, T3> makeTriple(const T1& iA, const T2& iB, const T3& iC);

}

}

#include "triple.inl"

#endif

// EOF
