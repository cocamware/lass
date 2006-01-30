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



#ifndef LASS_GUARDIAN_OF_INCLUSION_STDE_TRIPLE_INL
#define LASS_GUARDIAN_OF_INCLUSION_STDE_TRIPLE_INL

#include "stde_common.h"
#include "triple.h"

namespace lass
{
namespace stde
{

// --- public --------------------------------------------------------------------------------------


/** default constructor
 *  The default constructor creates a vlaue triple with triples that are initialized by the
 *  default constructor of their type.  Because of language rules, an explicit call of a default
 *  constructor also initializes fundamental data types such as @c int.
 */
template <typename T1, typename T2, typename T3>
triple<T1, T2, T3>::triple():
	first(T1()),
	second(T2()),
	third(T3())
{
}



/** constructor for three values
 */
template <typename T1, typename T2, typename T3>
triple<T1, T2, T3>::triple(typename util::CallTraits<T1>::TParam a,
						   typename util::CallTraits<T2>::TParam b,
						   typename util::CallTraits<T3>::TParam c):
	first(a),
	second(b),
	third(c)
{
}



/** copy constructor with implicit conversions
	*  This template version of the copy constructor provided here is ussed when implicit
	*  conversions are necessary.  If an object of type Triple gets copied, the normal implicitly
	*  generated default copy constructor is called, which is nice.
	*/
template <typename T1, typename T2, typename T3>
template <typename U1, typename U2, typename U3>
triple<T1, T2, T3>::triple(const triple<U1, U2, U3>& other):
	first(other.first),
	second(other.second),
	third(other.third)
{
}



// --- protected -----------------------------------------------------------------------------------



// --- private -------------------------------------------------------------------------------------



// --- free ----------------------------------------------------------------------------------------

/** two triples are identical if all elements (first, second, third) are identical.
 *  @relates triple
 */
template <typename T1, typename T2, typename T3, typename U1, typename U2, typename U3>
bool operator==(const triple<T1, T2, T3>& x, const triple<U1, U2, U3>& y)
{
	return x.first == y.first && x.second == y.second && x.third == y.third;
}

/** return true if x is "less" than y.
 *  @relates triple
 *
 *  As for std::pair, the "less than" comparison is implemented for Triples in a similar way.
 *  The first value has higher priority than the second and third, and the second value has higher
 *  priority than the third.  Thus, if the first values of two pairs differ, the result of their
 *  comparison is used as the result of the comparison of the whole triples.  If the first values
 *  are equal, the comparison of the second values yields the result if they differ.  If the
 *  second values are equal too, the comparison of the third values finally yields the result.
 */
template <typename T1, typename T2, typename T3, typename U1, typename U2, typename U3>
bool operator<(const triple<T1, T2, T3>& x, const triple<U1, U2, U3>& y)
{
	return x.first < y.first || (!(y.first < x.first) &&
		(x.second < y.second || (!(y.second < x.second) && x.third < y.third)));
}

/** equivalent to !(x == y), see operator<
 *  @relates triple
 */
template <typename T1, typename T2, typename T3, typename U1, typename U2, typename U3>
bool operator!=(const triple<T1, T2, T3>& x, const triple<U1, U2, U3>& y)
{
	return !(x == y);
}

/** equivalent to y < x, see operator<
 *  @relates triple
 */
template <typename T1, typename T2, typename T3, typename U1, typename U2, typename U3>
bool operator>(const triple<T1, T2, T3>& x, const triple<U1, U2, U3>& y)
{
	return y < x;
}

/** equivalent to !(y < x), see operator<
 *  @relates triple
 */
template <typename T1, typename T2, typename T3, typename U1, typename U2, typename U3>
bool operator<=(const triple<T1, T2, T3>& x, const triple<U1, U2, U3>& y)
{
	return !(y < x);
}

/** equivalent to !(x < y), see operator<
 *  @relates triple
 */
template <typename T1, typename T2, typename T3, typename U1, typename U2, typename U3>
bool operator>=(const triple<T1, T2, T3>& x, const triple<U1, U2, U3>& y)
{
	return !(x < y);
}



/*
 *  @relates triple
 */
template <typename T1, typename T2, typename T3>
std::ostream& operator<<(std::ostream& stream, const triple<T1, T2, T3>& x)
{
	return stream << "(" << x.first << ", " << x.second << ", " << x.third << ")";
}



/** convenience function to create a lass::stde::triple.
 *  @relates triple
 *
 *  The make_triple template function enables you to create a value triple without writing the
 *  types explicitly.  In particular the make_triple function makes it convenient to pass three
 *  values of a triple directly to a function that requires a triple as its argument.
 *  It works even when the types do not match exactly becuase the template copy constructor
 *  provides implicit type conversion.
 *
 *  using make_triple should cost no runtime.  The compiler should always optimize any implied
 *  overhead.
 */
template <typename T1, typename T2, typename T3>
triple<T1, T2, T3> make_triple(const T1& a, const T2& b, const T3& c)
{
	return triple<T1, T2, T3>(a, b, c);
}



}

}

#endif

// EOF
