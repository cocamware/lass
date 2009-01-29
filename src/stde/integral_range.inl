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
 *	Copyright (C) 2004-2009 the Initial Developer.
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

// EOF
