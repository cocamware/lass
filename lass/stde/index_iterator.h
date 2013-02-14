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
 *	Copyright (C) 2004-2012 the Initial Developer.
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

#ifndef LASS_GUARDIAN_OF_INCLUSION_STDE_TRANSFORM_ITERATOR_H
#define LASS_GUARDIAN_OF_INCLUSION_STDE_TRANSFORM_ITERATOR_H

#include "stde_common.h"

namespace lass
{
namespace stde
{

/** iterator adaptor to access members.
 *
 *  A prime example is to iterate over the keys or values in a @c  std::map.  
 *  The value_type of the map's iterator is a @c std::pair of key and value.  
 *  @c first_accessor_t is an accessor that will access the first element of
 *  the pair, and combined with @c index_iterator_t it allows you to 
 *  directly iterate over just the keys of the map.  
 *  A helper function @c first_iterator is provided to easily adapt a
 *  map's iterator:
 *
 *  @code
 *  // prints spam, ham, eggs
 *	std::map<std::string, int> map;
 *	map["spam"] = 1;
 *	map["ham"] = 2;
 *	map["eggs"] = 3;
 *	std::copy(
 *		first_iterator(map.begin()), 
 *		first_iterator(map.end()), 
 *		std::ostream_iterator<std::string>(std::cout, ", "));
 *	@endcode
 *
 *  @tparam Iterator the iterator type of the original sequence.  In the 
 *		example above, this would be @c std::map<std::string, int>::iterator
 *  @tparam Accessor functor type that takes an Iterator as argument and
 *		returns a reference to the accessed member.  Accessor shall also
 *		define @c value_type, @c reference and @c pointer.  An example is 
 *		@c first_accessor_t
 */
template 
<
	typename ContainerType,
	typename ValueType = typename ContainerType::value_type,
	typename PointerType = typename ContainerType::pointer,
	typename ReferenceType = typename ContainerType::reference,
	typename SizeType = size_t,
	typename DifferenceType = ptrdiff_t
>
class index_iterator_t: 
	public std::iterator
	<
		std::random_access_iterator_tag,
		ValueType,
		DifferenceType,
		PointerType,
		ReferenceType
	>
{
public:
	typedef ContainerType container_type;
	typedef SizeType size_type;
	typedef ValueType value_type;
	typedef DifferenceType difference_type;
	typedef PointerType pointer;
	typedef ReferenceType reference;

	index_iterator_t(ContainerType& container, size_type index): container_(&container), index_(static_cast<difference_type>(index)) {}

	pointer operator->() const { return &(*container_)[static_cast<size_type>(index_)]; }
	reference operator*() const { return (*container_)[static_cast<size_type>(index_)]; }

	index_iterator_t& operator++() { ++index_; return *this; }
	index_iterator_t operator++(int) { return index_iterator_t(container_, index_++); }
	index_iterator_t& operator--() { --index_; return *this; }
	index_iterator_t operator--(int) { return index_iterator_t(container_, index_--); }

	index_iterator_t& operator+=(difference_type n) { index_ += n; return *this; }
	index_iterator_t operator+(difference_type n) { return index_iterator_t(container_, index_ + n); }
	index_iterator_t& operator-=(difference_type n) { index_ -= n; return *this; }
	index_iterator_t operator-(difference_type n) { return index_iterator_t(container_, index_ - n); }

	reference operator[](difference_type n) const { return (*container_)[static_cast<size_type>(index_ + n)]; }

	container_type* container() const { return container_; }
	size_type index() const { return static_cast<size_type>(index_); }

public:
	container_type* container_;
	difference_type index_;
};

/** @relates index_iterator_t */
template <typename C, typename V, typename R, typename P, typename S, typename D> 
typename index_iterator_t<C, V, R, P, S, D>::difference_type operator-(const index_iterator_t<C, V, R, P, S, D>& a, const index_iterator_t<C, V, R, P, S, D>& b) 
{ 
	return a.index() - b.index(); 
}
/** @relates index_iterator_t */
template <typename C, typename V, typename R, typename P, typename S, typename D> 
bool operator==(const index_iterator_t<C, V, R, P, S, D>& a, const index_iterator_t<C, V, R, P, S, D>& b) 
{ 
	return a.container() == b.container() && a.index() == b.index();
}
/** @relates index_iterator_t */
template <typename C, typename V, typename R, typename P, typename S, typename D> 
bool operator!=(const index_iterator_t<C, V, R, P, S, D>& a, const index_iterator_t<C, V, R, P, S, D>& b) 
{ 
	return !(a == b); 
}
/** @relates index_iterator_t */
template <typename C, typename V, typename R, typename P, typename S, typename D> 
bool operator<(const index_iterator_t<C, V, R, P, S, D>& a, const index_iterator_t<C, V, R, P, S, D>& b) 
{ 
	return a.index() < b.index();
}
/** @relates index_iterator_t */
template <typename C, typename V, typename R, typename P, typename S, typename D> 
bool operator>(const index_iterator_t<C, V, R, P, S, D>& a, const index_iterator_t<C, V, R, P, S, D>& b) 
{ 
	return a.index() > b.index(); 
}
/** @relates index_iterator_t */
template <typename C, typename V, typename R, typename P, typename S, typename D> 
bool operator<=(const index_iterator_t<C, V, R, P, S, D>& a, const index_iterator_t<C, V, R, P, S, D>& b) 
{ 
	return a.index() <= b.index(); 
}
/** @relates index_iterator_t */
template <typename C, typename V, typename R, typename P, typename S, typename D> 
bool operator>=(const index_iterator_t<C, V, R, P, S, D>& a, const index_iterator_t<C, V, R, P, S, D>& b) 
{ 
	return a.index() >= b.index(); 
}


}
}

#endif

// EOF

