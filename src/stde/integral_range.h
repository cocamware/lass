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

/** @class lass::stde::integral_range
 *  @brief integral range.
 *  @author Bram de Greve [Bramz]
 */

#ifndef LASS_GUARDIAN_OF_INCLUSION_STDE_INTEGRAL_RANGE_H
#define LASS_GUARDIAN_OF_INCLUSION_STDE_INTEGRAL_RANGE_H

#include "stde_common.h"
#include "../num/safe_bool.h"

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
	public std::iterator<std::random_access_iterator_tag, const integral_type>
{
public:
	typedef const_integral_iterator<integral_type> self_type;
	typedef std::iterator<std::random_access_iterator_tag, const integral_type> iterator_type;
	typedef typename iterator_type::value_type value_type;
	typedef typename iterator_type::pointer pointer;
	typedef typename iterator_type::reference reference;
	typedef typename iterator_type::difference_type difference_type;

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
	operator const num::SafeBool() const;

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
