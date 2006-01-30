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



#ifndef LASS_GUARDIAN_OF_INCLUSION_STDE_ITERATOR_RANGE_INL
#define LASS_GUARDIAN_OF_INCLUSION_STDE_ITERATOR_RANGE_INL

#include "stde_common.h"
#include "iterator_range.h"

namespace lass
{
namespace stde
{

// --- public --------------------------------------------------------------------------------------

template <typename I> inline
iterator_range<I>::iterator_range(): 
	first_(0), 
	last_(0) 
{
}



template <typename I> inline
iterator_range<I>::iterator_range(iterator first, iterator last): 
	first_(first), 
	last_(last) 
{
}



template <typename I> inline
typename iterator_range<I>::iterator 
iterator_range<I>::begin() const 
{ 
	return first_; 
}



template <typename I> inline
typename iterator_range<I>::iterator 
iterator_range<I>::end() const 
{ 
	return last_; 
}



template <typename I> inline
typename iterator_range<I>::iterator 
iterator_range<I>::operator->() const 
{ 
	LASS_ASSERT(!this->empty()); 
	return first_; 
}



template <typename I> inline
typename iterator_range<I>::reference 
iterator_range<I>::operator*() const 
{ 
	LASS_ASSERT(!this->empty()); 
	return *first_; 
}



template <typename I> inline
typename iterator_range<I>::reference 
iterator_range<I>::operator[](difference_type index) const
{ 
	LASS_ASSERT(index < this->size());
	iterator result = first_;
	std::advance(result, index);
	return *result;
}



template <typename I> inline
iterator_range<I>& 
iterator_range<I>::operator++() 
{ 
	LASS_ASSERT(first_ != last_); 
	++first_; 
	return *this; 
}



template <typename I> inline
iterator_range<I>
iterator_range<I>::operator++(int) 
{ 
	iterator_range<I> result(*this); 
	this->operator++(); 
	return result; 
}



template <typename I> inline
const typename iterator_range<I>::difference_type 
iterator_range<I>::size() const 
{
	return std::distance(first_, last_);
}



template <typename I> inline
const bool iterator_range<I>::empty() const 
{ 
	return first_ == last_; 
}



template <typename I> inline
const bool iterator_range<I>::operator!() const 
{ 
	return this->empty(); 
}



template <typename I> inline
iterator_range<I>::operator const num::SafeBool() const
{ 
	return this->empty() ? num::safeFalse : num::safeTrue; 
}



template <typename I> inline
void iterator_range<I>::swap(iterator_range<I>& other)
{
	std::swap(first_, other.first_);
	std::swap(last_, other.last_);
}



// --- protected -----------------------------------------------------------------------------------



// --- private -------------------------------------------------------------------------------------



// --- free functions ------------------------------------------------------------------------------

template <typename I> inline
iterator_range<I> range(const I& a, const I& b)
{
	return iterator_range<I>(a, b);
}



template <typename I> inline
const bool operator==(const iterator_range<I>& a, const iterator_range<I>& b)
{
	return (a.begin() == b.begin() && a.end() == b.end()) || (a.empty() && b.empty());
}



template <typename I> inline
const bool operator!=(const iterator_range<I>& a, const iterator_range<I>& b)
{
	return !(a == b);
}



}

}

#endif

// EOF
