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

// --- public --------------------------------------------------------------------------------------

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
	step_(1)
{
}



template <typename I> inline
integral_range_t<I>::integral_range_t(value_type first, value_type last): 
	first_(first), 
	last_(last),
	step_(1)
{
}



template <typename I> inline
integral_range_t<I>::integral_range_t(value_type first, value_type last, value_type step): 
	first_(first), 
	last_(last),
	step_(step)
{
}



template <typename I> inline
typename integral_range_t<I>::const_iterator 
integral_range_t<I>::begin() const 
{ 
	return const_iterator(*this);
}



template <typename I> inline
typename integral_range_t<I>::const_iterator 
integral_range_t<I>::end() const 
{ 
	return const_iterator(integral_range_t<I>(last_, last_, step_)); 
}



template <typename I> inline
typename integral_range_t<I>::const_pointer 
integral_range_t<I>::operator->() const 
{ 
	LASS_ASSERT(!this->empty()); 
	return &first_; 
}



template <typename I> inline
typename integral_range_t<I>::const_reference 
integral_range_t<I>::operator*() const 
{ 
	LASS_ASSERT(!this->empty()); 
	return first_; 
}



template <typename I> inline
typename integral_range_t<I>::const_reference 
integral_range_t<I>::operator[](difference_type index) const
{ 
	return first_ + index * step_;
}



template <typename I> inline
integral_range_t<I>& 
integral_range_t<I>::operator++() 
{ 
	LASS_ASSERT(!empty());
	first_ += step_;
	if (step_ > 0)
	{
		first_ == std::min(first_, last_);
	}
	else
	{
		first_ == std::max(first_, last_);
	}
	return *this; 
}



template <typename I> inline
integral_range_t<I>
integral_range_t<I>::operator++(int) 
{ 
	integral_range_t<integral_type> result(*this); 
	this->operator++(); 
	return result; 
}



template <typename I> inline
const typename integral_range_t<I>::size_type 
integral_range_t<I>::size() const 
{
	return static_cast<size_type>(last_ - first_) / step_;
}



template <typename I> inline
const bool integral_range_t<I>::empty() const 
{ 
	return first_ == last_;
}



template <typename I> inline
const bool integral_range_t<I>::operator!() const 
{ 
	return this->empty(); 
}



template <typename I> inline
integral_range_t<I>::operator const num::SafeBool() const
{ 
	return this->empty() ? num::safeFalse : num::safeTrue; 
}



template <typename I> inline
void integral_range_t<I>::swap(integral_range_t<I>& other)
{
	std::swap(first_, other.first_);
	std::swap(last_, other.last_);
	std::swap(step_, other.step_);
}



// --- protected -----------------------------------------------------------------------------------



// --- private -------------------------------------------------------------------------------------



// --- free functions ------------------------------------------------------------------------------

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
const bool operator==(const integral_range_t<I>& a, const integral_range_t<I>& b)
{
	return a.first_ == b.first_ && a.last_ == b.last_ && a.step_ == b.step_;
}



template <typename I> inline
const bool operator!=(const integral_range_t<I>& a, const integral_range_t<I>& b)
{
	return !(a == b);
}



}

}

#endif

// EOF
