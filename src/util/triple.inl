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



#ifndef LASS_GUARDIAN_OF_INCLUSION_UTIL_TRIPLE_INL
#define LASS_GUARDIAN_OF_INCLUSION_UTIL_TRIPLE_INL

#include "util_common.h"
#include "triple.h"

namespace lass
{
namespace util
{

// --- public --------------------------------------------------------------------------------------


/** default constructor
 *  The default constructor creates a vlaue triple with triples that are initialized by the 
 *  default constructor of their type.  Because of language rules, an explicit call of a default
 *  constructor also initializes fundamental data types such as int.
 */
template <typename T1, typename T2, typename T3>
Triple<T1, T2, T3>::Triple():
    first(T1()),
    second(T2()),
    third(T3())
{
}



/** constructor for three values
 */
template <typename T1, typename T2, typename T3>
Triple<T1, T2, T3>::Triple(typename CallTraits<T1>::TParam iA, 
                           typename CallTraits<T2>::TParam iB, 
                           typename CallTraits<T3>::TParam iC):
    first(iA),
    second(iB),
    third(iC)
{
}



// --- protected -----------------------------------------------------------------------------------



// --- private -------------------------------------------------------------------------------------



// --- free ----------------------------------------------------------------------------------------

/** two triples are identical if all elements (first, second, third) are identical.
 *  @relates Triple
 */
template <typename T1, typename T2, typename T3, typename U1, typename U2, typename U3>
bool operator==(const Triple<T1, T2, T3>& iA, const Triple<U1, U2, U3>& iB)
{
    return iA.first == iB.first && iA.second == iB.second && iA.third == iB.third;
}

/** return true if iA is "less" than iB.
 *  @relates Triple
 * 
 *  As for std::pair, the "less than" comparison is implemented for Triples in a similar way.
 *  The first value has higher priority than the second and third, and the second value has higher
 *  priority than the third.  Thus, if the first values of two pairs differ, the result of their 
 *  comparison is used as the result of the comparison of the whole triples.  If the first values
 *  are equal, the comparison of the second values yields the result if they differ.  If the 
 *  second values are equal too, the comparison of the third values finally yields the result.
 */
template <typename T1, typename T2, typename T3, typename U1, typename U2, typename U3>
bool operator<(const Triple<T1, T2, T3>& iA, const Triple<U1, U2, U3>& iB)
{
    return iA.first < iB.first || 
            (!(iB.first < iA.first) && (iA.second < iB.second ||
                                        (!(iB.second < iA.second) && iA.third < iB.third)));
}

/** equivalent to !(iA == iB), see operator<
 *  @relates Triple
 */
template <typename T1, typename T2, typename T3, typename U1, typename U2, typename U3>
bool operator!=(const Triple<T1, T2, T3>& iA, const Triple<U1, U2, U3>& iB)
{
    return !(iA == iB);
}

/** equivalent to iB < iA, see operator<
 *  @relates Triple
 */
template <typename T1, typename T2, typename T3, typename U1, typename U2, typename U3>
bool operator>(const Triple<T1, T2, T3>& iA, const Triple<U1, U2, U3>& iB)
{
    return iB < iA;
}

/** equivalent to !(iB < iA), see operator<
 *  @relates Triple
 */
template <typename T1, typename T2, typename T3, typename U1, typename U2, typename U3>
bool operator<=(const Triple<T1, T2, T3>& iA, const Triple<U1, U2, U3>& iB)
{
    return !(iB < iA);
}

/** equivalent to !(iA < iB), see operator<
 *  @relates Triple
 */
template <typename T1, typename T2, typename T3, typename U1, typename U2, typename U3>
bool operator>=(const Triple<T1, T2, T3>& iA, const Triple<U1, U2, U3>& iB)
{
    return !(iA < iB);
}



/*
 *  @relates Triple
 */
template <typename T1, typename T2, typename T3>
std::ostream& operator<<(std::ostream& ioS, const Triple<T1, T2, T3>& iTriple)
{
    ioS << "(" << iTriple.first << ", " << iTriple.second << ", " << iTriple.third << ")";
    return ioS;
}



/** convenience function to create a lass::util::Triple.
 *  @relates Triple
 *
 *  The makeTriple template function enables you to create a value triple without writing the 
 *  types explicitly.  In particular the makeTriple function makes it convenient to pass three
 *  values of a Triple directly to a function that requires a Triple as its argument.
 *  It works even when the types do not match exactly becuase the template copy constructor
 *  provides implicit type conversion.
 *
 *  using makeTriple should cost no runtime.  The compiler should always optimize any implied
 *  overhead.
 */
template <typename T1, typename T2, typename T3>
Triple<T1, T2, T3> makeTriple(const T1& iA, const T2& iB, const T3& iC)
{
    return Triple<T1, T2, T3>(iA, iB, iC);
}



}

}

#endif

// EOF
