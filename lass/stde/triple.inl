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
 *	Copyright (C) 2004-2011 the Initial Developer.
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
	*  conversions are necessary.
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

// EOF
