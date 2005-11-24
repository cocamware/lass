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

template <typename integral_type>
class integral_range_t
{
public:

	class const_iterator
	{
	public:
		typedef const integral_type value_type;
		typedef const integral_type* pointer;
		typedef const integral_type& reference;
		typedef std::size_t size_type;
		typedef std::ptrdiff_t difference_type;
		typedef std::random_access_iterator_tag iterator_category;
		const_iterator(): range_() {}
		pointer operator->() const { return range_.operator->(); }
		reference operator*() const { return range_.operator*(); }
		const_iterator& operator++() { ++range_; return *this; }
		const_iterator operator++(int) { const_iterator result(*this); ++(*this); return result; }
		bool operator==(const const_iterator& other) const { return range_ == other.range_; }
		bool operator!=(const const_iterator& other) const { return !(*this == other); }
	private:
		friend class integral_range_t<integral_type>;
		explicit const_iterator(const integral_range_t<integral_type>& range): range_(range) {}
		integral_range_t<integral_type> range_;
	};

	typedef const integral_type value_type;
	typedef const integral_type* const_pointer;
	typedef const integral_type& const_reference;
	typedef std::size_t size_type;
	typedef std::ptrdiff_t difference_type;
	typedef std::random_access_iterator_tag iterator_category;

	integral_range_t();
	integral_range_t(value_type last);
	integral_range_t(value_type first, value_type last);
	integral_range_t(value_type first, value_type last, value_type step);

	const_iterator begin() const;
	const_iterator end() const;

	const_pointer operator->() const;
	const_reference operator*() const;
	const_reference operator[](difference_type index) const;

	integral_range_t<integral_type>& operator++();
	integral_range_t<integral_type> operator++(int);

	const size_type size() const;
	const bool empty() const;
	const bool operator!() const;
	operator const num::SafeBool() const;

	void swap(integral_range_t<integral_type>& other);

private:

	template <typename J> friend const bool operator==(const integral_range_t<J>& a, const integral_range_t<J>& b);

	integral_type first_;
	integral_type step_;
	integral_type last_;
};

template <typename I> integral_range_t<I> integral_range(const I& last);
template <typename I> integral_range_t<I> integral_range(const I& first, const I& last);
template <typename I> integral_range_t<I> integral_range(const I& first, const I& last, const I& step);

template <typename I> const bool operator==(const integral_range_t<I>& a, const integral_range_t<I>& b);
template <typename I> const bool operator!=(const integral_range_t<I>& a, const integral_range_t<I>& b);

}

}

#include "integral_range.inl"

#endif

// EOF
