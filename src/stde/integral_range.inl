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



#ifndef LASS_GUARDIAN_OF_INCLUSION_STDE_INTEGRAL_RANGE_INL
#define LASS_GUARDIAN_OF_INCLUSION_STDE_INTEGRAL_RANGE_INL

#include "stde_common.h"
#include "integral_range.h"

namespace lass
{
namespace stde
{

// --- const_integral_iterator ---------------------------------------------------------------------

template <typename I>
const_integral_iterator<I>::const_integral_iterator(value_type value, value_type step):
	value_(value),
	step_(step)
{
}
	


template <typename I> inline
typename const_integral_iterator<I>::pointer 
const_integral_iterator<I>::operator->() const
{
	return &value_;
}



template <typename I> inline
typename const_integral_iterator<I>::reference 
const_integral_iterator<I>::operator*() const
{
	return value_;
}



template <typename I> inline
typename const_integral_iterator<I>::value_type
const_integral_iterator<I>::operator[](difference_type n) const
{
	return value_ + n * step_;
}



template <typename I> inline
typename const_integral_iterator<I>::self_type&
const_integral_iterator<I>::operator++()
{
	value_ += step_;
	return *this;
}



template <typename I> inline
typename const_integral_iterator<I>::self_type
const_integral_iterator<I>::operator++(int)
{
	self_type temp(*this);
	++*this;
	return temp;
}



template <typename I> inline
typename const_integral_iterator<I>::self_type& 
const_integral_iterator<I>::operator--()
{
	value_ -= step_;
	return *this;
}



template <typename I> inline
typename const_integral_iterator<I>::self_type
const_integral_iterator<I>::operator--(int)
{
	self_type temp(*this);
	--*this;
	return temp;
}



template <typename I> inline
typename const_integral_iterator<I>::self_type& 
const_integral_iterator<I>::operator+=(difference_type n)
{
	value_ += n * step_;
	return *this;
}



template <typename I> inline
typename const_integral_iterator<I>::self_type& 
const_integral_iterator<I>::operator-=(difference_type n)
{
	value_ -= n * step_;
	return *this;
}



template <typename I> inline
typename const_integral_iterator<I>::self_type
const_integral_iterator<I>::operator+(difference_type n) const
{
	self_type temp(*this);
	temp += n;
	return temp;
}



template <typename I> inline
typename const_integral_iterator<I>::self_type
const_integral_iterator<I>::operator-(difference_type n) const
{
	self_type temp(*this);
	temp -= n;
	return temp;
}



template <typename I> inline
typename const_integral_iterator<I>::difference_type 
const_integral_iterator<I>::operator-(const self_type& other) const
{
	LASS_ASSERT(step_ == other.step_);
	LASS_ASSERT((value_ - other.value_) % step_ == 0);
	return (value_ - other.value_) / step_;
}



template <typename I> inline
bool operator==(const const_integral_iterator<I>& a, const const_integral_iterator<I>& b)
{
	LASS_ASSERT(a.step_ == b.step_);
	return a.value_ == b.value_;
}



template <typename I> inline
bool operator!=(const const_integral_iterator<I>& a, const const_integral_iterator<I>& b)
{
	return !(a == b);
}



template <typename I> inline
bool operator<(const const_integral_iterator<I>& a, const const_integral_iterator<I>& b)
{
	LASS_ASSERT(a.step_ == b.step_);
	return a.value_ < b.value_;
}



template <typename I> inline
bool operator>(const const_integral_iterator<I>& a, const const_integral_iterator<I>& b)
{
	return b < a;
}



template <typename I> inline
bool operator<=(const const_integral_iterator<I>& a, const const_integral_iterator<I>& b)
{
	return !(b < a);
}



template <typename I> inline
bool operator>=(const const_integral_iterator<I>& a, const const_integral_iterator<I>& b)
{
	return !(a < b);
}



// --- integral_range_t ----------------------------------------------------------------------------

template <typename I> inline
integral_range_t<I>::integral_range_t():
	first_(0),
	last_(0),
	step_(0)
{
}



template <typename I> inline
integral_range_t<I>::integral_range_t(value_type last):
	first_(0),
	last_(last),
	step_(last >= 0 ? 1 : -1)
{
}



template <typename I> inline
integral_range_t<I>::integral_range_t(value_type first, value_type last):
	first_(first),
	last_(last),
	step_(last >= first ? 1 : -1)
{
}



template <typename I> inline
integral_range_t<I>::integral_range_t(value_type first, value_type last, value_type step):
	first_(first),
	last_(last),
	step_(step)
{
	LASS_ASSERT(step != 0);
	value_type m = (last_ - first_) % step_;
	if (m != 0)
	{
		last_ += step_ - m;
	}
	LASS_ASSERT((last_ - first_) % step_ == 0);
}



template <typename I> inline
typename integral_range_t<I>::const_iterator
integral_range_t<I>::begin() const
{
	return const_iterator(first_, step_);
}



template <typename I> inline
typename integral_range_t<I>::const_iterator
integral_range_t<I>::end() const
{
	return const_iterator(last_, step_);
}



template <typename I> inline
typename integral_range_t<I>::const_reverse_iterator
integral_range_t<I>::rbegin() const
{
	return const_reverse_iterator(end());
}



template <typename I> inline
typename integral_range_t<I>::const_reverse_iterator
integral_range_t<I>::rend() const
{
	return const_reverse_iterator(begin());
}



template <typename I> inline
typename integral_range_t<I>::self_type&
integral_range_t<I>::operator++()
{
	LASS_ASSERT(first_ != last_);
	first_ += step_;
}



template <typename I> inline
typename integral_range_t<I>::self_type&
integral_range_t<I>::operator--()
{
	LASS_ASSERT(first_ != last_);
	first_ -= step_;
}



template <typename I> inline
typename integral_range_t<I>::self_type&
integral_range_t<I>::operator+=(difference_type n)
{
	LASS_ASSERT((step_ > 0) ? (first_ + n * step_ <= last_) : (first_ + n * step_ >= last_));
	first_ += n * step_;
}



template <typename I> inline
typename integral_range_t<I>::self_type&
integral_range_t<I>::operator-=(difference_type n)
{
	first_ -= n * step_;
}



template <typename I> inline
typename integral_range_t<I>::self_type
integral_range_t<I>::operator+(difference_type n) const
{
	self_type temp(*this);
	temp += n;
	return temp;
}



template <typename I> inline
typename integral_range_t<I>::self_type
integral_range_t<I>::operator-(difference_type n) const
{
	self_type temp(*this);
	temp -= n;
	return temp;
}



template <typename I> inline
const typename integral_range_t<I>::size_type
integral_range_t<I>::size() const
{
	LASS_ASSERT((last_ - first_) % step_ == 0);
	return static_cast<size_type>((last_ - first_) / step_);
}



template <typename I> inline
const bool integral_range_t<I>::empty() const
{
	return first_ == last_;
}



template <typename I> inline
const bool integral_range_t<I>::operator!() const
{
	return empty();
}



template <typename I> inline
integral_range_t<I>::operator const num::SafeBool() const
{
	return empty() ? num::safeFalse : num::safeTrue;
}



template <typename I>
void integral_range_t<I>::swap(self_type& other)
{
	std::swap(first_, other.first_);
	std::swap(last_, other.last_);
	std::swap(step_, other.step_);
}



template <typename I> inline
integral_range_t<I> integral_range(const I& last)
{
	return integral_range_t<I>(last);
}



template <typename I> inline
integral_range_t<I> integral_range(const I& first, const I& last)
{
	return integral_range_t<I>(first, last);
}



template <typename I> inline
integral_range_t<I> integral_range(const I& first, const I& last, const I& step)
{
	return integral_range_t<I>(first, last, step);
}



template <typename I> inline
bool operator==(const integral_range_t<I>& a, const integral_range_t<I>& b)
{
	LASS_ASSERT(a.step_ == b.step_ && a.last_ == b.last_);
	return a.first_ == b.first_;
}



template <typename I> inline
bool operator!=(const integral_range_t<I>& a, const integral_range_t<I>& b)
{
	return !(a == b);
}



template <typename I> inline
bool operator<(const integral_range_t<I>& a, const integral_range_t<I>& b)
{
	LASS_ASSERT(a.step_ == b.step_ && a.last_ == b.last_);
	return a.first_ < b.first_;
}



template <typename I> inline
bool operator>(const integral_range_t<I>& a, const integral_range_t<I>& b)
{
	return b < a;
}



template <typename I> inline
bool operator<=(const integral_range_t<I>& a, const integral_range_t<I>& b)
{
	return !(b < a);
}



template <typename I> inline
bool operator>=(const integral_range_t<I>& a, const integral_range_t<I>& b)
{
	return !(a < b);
}

}

}

#endif

// EOF
