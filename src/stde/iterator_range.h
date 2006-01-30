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

/** @class lass::stde::iterator_range
 *  @brief iterator range.
 *  @author Bram de Greve [Bramz]
 */

#ifndef LASS_GUARDIAN_OF_INCLUSION_STDE_ITERATOR_RANGE_H
#define LASS_GUARDIAN_OF_INCLUSION_STDE_ITERATOR_RANGE_H

#include "stde_common.h"
#include "../num/safe_bool.h"

namespace lass
{
namespace stde
{

template <typename iterator_type>
class iterator_range
{
public:

	typedef iterator_type iterator;
	typedef typename iterator_type::pointer pointer;
	typedef typename iterator_type::reference reference;
	typedef typename iterator_type::difference_type difference_type;
	typedef typename iterator_type::iterator_category iterator_category;

	iterator_range();
	iterator_range(iterator first, iterator last);

	iterator begin() const;
	iterator end() const;

	iterator operator->() const;
	reference operator*() const;
	reference operator[](difference_type index) const;

	iterator_range& operator++();
	iterator_range operator++(int);

	const difference_type size() const;
	const bool empty() const;
	const bool operator!() const;
	operator const num::SafeBool() const;

	void swap(iterator_range& other);

private:

	iterator first_;
	iterator last_;
};

template <typename I> iterator_range<I> range(const I& a, const I& b);

template <typename I> const bool operator==(const iterator_range<I>& a, const iterator_range<I>& b);
template <typename I> const bool operator!=(const iterator_range<I>& a, const iterator_range<I>& b);

}

}

#include "iterator_range.inl"

#endif

// EOF
