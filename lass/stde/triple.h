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

/** @class lass::stde::triple
 *  @brief extension of @c std::pair to three values, trait three values as a single unit.
 *  @author Bram de Greve [Bramz]
 *
 *  we'd like to refer to the C++ standard library for info on @c std::pair.  @c triple is an
 *  extension of this to three values.  As @c std::pair, triple is declared as a struct,
 *  the default constructor will initialize all elements with their default constructor,
 *  and implicit conversions between triples of different type are allowed, provided that
 *  the conversions between the element types are possible.
 *
 *  As for @c std::pair, the "less than" comparison is implemented for @c triple in a similar way.
 *  The first value has higher priority than the second and third, and the second value has higher
 *  priority than the third.  Thus, if the first values of two pairs differ, the result of their
 *  comparison is used as the result of the comparison of the whole triples.  If the first values
 *  are equal, the comparison of the second values yields the result if they differ.  If the
 *  second values are equal too, the comparison of the third values finally yields the result.
 */

#ifndef LASS_GUARDIAN_OF_INCLUSION_STDE_TRIPLE_H
#define LASS_GUARDIAN_OF_INCLUSION_STDE_TRIPLE_H

#include "stde_common.h"

namespace lass
{
namespace stde
{

template <typename T1, typename T2, typename T3>
struct triple
{
	typedef T1 first_type;
	typedef T2 second_type;
	typedef T3 third_type;

	T1 first;
	T2 second;
	T3 third;

	triple();
	triple(typename util::CallTraits<T1>::TParam a, typename util::CallTraits<T2>::TParam b,
		typename util::CallTraits<T3>::TParam c);
	template <typename U1, typename U2, typename U3> triple(const triple<U1, U2, U3>& other);
};

template <typename T1, typename T2, typename T3, typename U1, typename U2, typename U3>
bool operator==(const triple<T1, T2, T3>& x, const triple<U1, U2, U3>& y);

template <typename T1, typename T2, typename T3, typename U1, typename U2, typename U3>
bool operator!=(const triple<T1, T2, T3>& x, const triple<U1, U2, U3>& y);

template <typename T1, typename T2, typename T3, typename U1, typename U2, typename U3>
bool operator<(const triple<T1, T2, T3>& x, const triple<U1, U2, U3>& y);

template <typename T1, typename T2, typename T3, typename U1, typename U2, typename U3>
bool operator>(const triple<T1, T2, T3>& x, const triple<U1, U2, U3>& y);

template <typename T1, typename T2, typename T3, typename U1, typename U2, typename U3>
bool operator<=(const triple<T1, T2, T3>& x, const triple<U1, U2, U3>& y);

template <typename T1, typename T2, typename T3, typename U1, typename U2, typename U3>
bool operator>=(const triple<T1, T2, T3>& x, const triple<U1, U2, U3>& y);



template <typename T1, typename T2, typename T3>
std::ostream& operator<<(std::ostream& stream, const triple<T1, T2, T3>& x);



template <typename T1, typename T2, typename T3>
triple<T1, T2, T3> make_triple(const T1& a, const T2& b, const T3& c);

}

}

#include "triple.inl"

#endif

// EOF
