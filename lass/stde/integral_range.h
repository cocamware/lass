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

/** @class lass::stde::integral_range
 *  @brief integral range.
 *  @author Bram de Greve [Bramz]
 */

#ifndef LASS_GUARDIAN_OF_INCLUSION_STDE_INTEGRAL_RANGE_H
#define LASS_GUARDIAN_OF_INCLUSION_STDE_INTEGRAL_RANGE_H

#include "stde_common.h"
#include "../util/type_traits.h"

namespace lass
{
namespace stde
{

template <typename integral_type> class const_integral_iterator;

template <typename I> bool operator==(const const_integral_iterator<I>& a, const const_integral_iterator<I>& b);
template <typename I> bool operator!=(const const_integral_iterator<I>& a, const const_integral_iterator<I>& b);
template <typename I> bool operator< (const const_integral_iterator<I>& a, const const_integral_iterator<I>& b);
template <typename I> bool operator> (const const_integral_iterator<I>& a, const const_integral_iterator<I>& b);
template <typename I> bool operator<=(const const_integral_iterator<I>& a, const const_integral_iterator<I>& b);
template <typename I> bool operator>=(const const_integral_iterator<I>& a, const const_integral_iterator<I>& b);

template <typename integral_type>
class const_integral_iterator:
{
public:
	typedef const_integral_iterator<integral_type> self_type;
	typedef std::random_access_iterator_tag iterator_category;
	typedef integral_type value_type;
	typedef std::ptrdiff_t difference_type;
	typedef const integral_type* pointer;
	typedef const integral_type& reference;

	const_integral_iterator(value_type value, value_type step);

	pointer operator->() const;
	reference operator*() const;
	value_type operator[](difference_type n) const;

	self_type& operator++();
	self_type operator++(int);
	self_type& operator--();
	self_type operator--(int);

	self_type& operator+=(difference_type n);
	self_type& operator-=(difference_type n);
	self_type operator+(difference_type n) const;
	self_type operator-(difference_type n) const;

	difference_type operator-(const self_type& other) const;

private:

	friend bool operator== <integral_type>(const const_integral_iterator<integral_type>&, const const_integral_iterator<integral_type>& );
	friend bool operator<  <integral_type>(const const_integral_iterator<integral_type>&, const const_integral_iterator<integral_type>& );
	
	integral_type value_;
	integral_type step_;
};



template <typename integral_type> class integral_range_t;

template <typename I> bool operator==(const integral_range_t<I>& a, const integral_range_t<I>& b);
template <typename I> bool operator!=(const integral_range_t<I>& a, const integral_range_t<I>& b);
template <typename I> bool operator< (const integral_range_t<I>& a, const integral_range_t<I>& b);
template <typename I> bool operator> (const integral_range_t<I>& a, const integral_range_t<I>& b);
template <typename I> bool operator<=(const integral_range_t<I>& a, const integral_range_t<I>& b);
template <typename I> bool operator>=(const integral_range_t<I>& a, const integral_range_t<I>& b);

template <typename integral_type>
class integral_range_t
{
public:

	typedef integral_range_t<integral_type> self_type;

	typedef const_integral_iterator<integral_type> const_iterator;
	typedef std::reverse_iterator<const_iterator> const_reverse_iterator;

	typedef typename const_iterator::value_type value_type;
	typedef typename const_iterator::pointer const_pointer;
	typedef typename const_iterator::reference const_reference;
	typedef typename const_iterator::difference_type difference_type;
	typedef std::size_t size_type;

	integral_range_t();
	integral_range_t(value_type last);
	integral_range_t(value_type first, value_type last);
	integral_range_t(value_type first, value_type last, value_type step);

	const_iterator begin() const;
	const_iterator end() const;
	const_reverse_iterator rbegin() const;
	const_reverse_iterator rend() const;

	const_pointer operator->() const;
	const_reference operator*() const;
	value_type operator[](difference_type index) const;

	self_type& operator++();
	self_type operator++(int);
	self_type& operator--();
	self_type operator--(int);

	self_type& operator+=(difference_type n);
	self_type& operator-=(difference_type n);
	self_type operator+(difference_type n) const;
	self_type operator-(difference_type n) const;

	const size_type size() const;
	const bool empty() const;
	const bool operator!() const;
	explicit operator bool() const;

	void swap(self_type& other);

private:

	friend bool operator== <integral_type>(const integral_range_t<integral_type>& a, const integral_range_t<integral_type>& b);
	friend bool operator<  <integral_type>(const integral_range_t<integral_type>& a, const integral_range_t<integral_type>& b);

	integral_type first_;
	integral_type last_;
	integral_type step_;
};

template <typename I> integral_range_t<I> integral_range(const I& last);
template <typename I> integral_range_t<I> integral_range(const I& first, const I& last);
template <typename I> integral_range_t<I> integral_range(const I& first, const I& last, const I& step);

}

}

#include "integral_range.inl"

#endif

// EOF
