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



#ifndef LASS_GUARDIAN_OF_INCLUSION_STDE_STATIC_VECTOR_INL
#define LASS_GUARDIAN_OF_INCLUSION_STDE_STATIC_VECTOR_INL

#include "stde_common.h"
#include "static_vector.h"
#include "../meta/is_integral_type.h"
	
namespace lass
{
namespace stde
{

namespace impl
{

/** @internal
 */
struct static_vector_traits
{
	template <typename Container, typename T>
	static void push(Container& container, const T& value)
	{
		container.push_back(value);
	}
	template <typename Container>
	static void temp_to_output(Container& temp, Container& output)
	{
		output.assign(temp.begin(), temp.end());
	}
};

}



// --- public --------------------------------------------------------------------------------------

template <typename T, size_t maxsize> inline
static_vector<T, maxsize>::static_vector():
	size_(0)
{
}



template <typename T, size_t maxsize> inline
static_vector<T, maxsize>::static_vector(size_type n, const value_type& value):
	size_(0)
{
	insert(end(), n, value, meta::Type2Type<meta::True>());
}



template <typename T, size_t maxsize>
template <typename InputIterator> inline
static_vector<T, maxsize>::static_vector(InputIterator first, InputIterator last):
	size_(0)
{
	insert(end(), first, last, meta::Type2Type<typename meta::IsIntegralType<InputIterator>::Type>());       				
}



template <typename T, size_t maxsize> inline
void static_vector<T, maxsize>::assign(size_type n, const value_type& value)
{
	erase(begin(), end());
	insert(end(), n, value, meta::Type2Type<meta::True>());
}



template <typename T, size_t maxsize>
template <typename InputIterator> inline
void static_vector<T, maxsize>::assign(InputIterator first, InputIterator last)
{
	erase(begin(), end());
	insert(end(), first, last, meta::Type2Type<typename meta::IsIntegralType<InputIterator>::Type>());
}




template <typename T, size_t maxsize> inline
static_vector<T, maxsize>::~static_vector()
{
	clear();
}



template <typename T, size_t maxsize> inline
typename static_vector<T, maxsize>::iterator
static_vector<T, maxsize>::begin()
{
	return get_element(0);
}



template <typename T, size_t maxsize> inline
typename static_vector<T, maxsize>::const_iterator
static_vector<T, maxsize>::begin() const
{
	return get_element(0);
}



template <typename T, size_t maxsize> inline
typename static_vector<T, maxsize>::iterator
static_vector<T, maxsize>::end()
{
	return get_element(size_);
}



template <typename T, size_t maxsize> inline
typename static_vector<T, maxsize>::const_iterator
static_vector<T, maxsize>::end() const
{
	return get_element(size_);
}



template <typename T, size_t maxsize> inline
typename static_vector<T, maxsize>::reverse_iterator
static_vector<T, maxsize>::rbegin()
{
	return reverse_iterator(get_element(size_));
}



template <typename T, size_t maxsize> inline
typename static_vector<T, maxsize>::const_reverse_iterator
static_vector<T, maxsize>::rbegin() const
{
	return reverse_iterator(get_element(size_));
}



template <typename T, size_t maxsize> inline
typename static_vector<T, maxsize>::reverse_iterator
static_vector<T, maxsize>::rend()
{
	return reverse_iterator(get_element(0));
}



template <typename T, size_t maxsize> inline
typename static_vector<T, maxsize>::const_reverse_iterator
static_vector<T, maxsize>::rend() const
{
	return reverse_iterator(get_element(0));
}



template <typename T, size_t maxsize> inline
typename static_vector<T, maxsize>::size_type
static_vector<T, maxsize>::size() const
{
	return size_;
}



template <typename T, size_t maxsize> inline
typename static_vector<T, maxsize>::size_type
static_vector<T, maxsize>::max_size() const
{
	return max_size_;
}



template <typename T, size_t maxsize>
void static_vector<T, maxsize>::resize(size_type n, const value_type& value)
{
	enforce_valid_size(n);
	while (size_ > n)
	{
		get_element(--size_)->~value_type();
	}
	while (size_ < n)
	{
		new (get_element(size_)) value_type(value);
		++size_;
	}
}



template <typename T, size_t maxsize> inline
typename static_vector<T, maxsize>::size_type
static_vector<T, maxsize>::capacity() const
{
	return max_size_;
}



template <typename T, size_t maxsize> inline
bool static_vector<T, maxsize>::empty() const
{
	return size_ == 0;
}



template <typename T, size_t maxsize> inline
void static_vector<T, maxsize>::reserve(size_type n)
{
	enforce_valid_size(n);
}



template <typename T, size_t maxsize> inline
typename static_vector<T, maxsize>::reference
static_vector<T, maxsize>::operator[](size_type i)
{
	return *get_element(i);
}



template <typename T, size_t maxsize> inline
typename static_vector<T, maxsize>::const_reference
static_vector<T, maxsize>::operator[](size_type i) const
{
	return *get_element(i);
}



template <typename T, size_t maxsize>
typename static_vector<T, maxsize>::reference
static_vector<T, maxsize>::at(size_type i)
{
	if (i >= size_)
	{
		throw std::out_of_range("index out of range in lass::stde::static_vector::at");
	}
	return *get_element(i);
}



template <typename T, size_t maxsize>
typename static_vector<T, maxsize>::const_reference
static_vector<T, maxsize>::at(size_type i) const
{
	if (i >= size_)
	{
		throw std::out_of_range("index out of range in lass::stde::static_vector::at");
	}
	return *get_element(i);
}



template <typename T, size_t maxsize> inline
typename static_vector<T, maxsize>::reference
static_vector<T, maxsize>::front()
{
	return *get_element(0);
}



template <typename T, size_t maxsize> inline
typename static_vector<T, maxsize>::const_reference
static_vector<T, maxsize>::front() const
{
	return *get_element(0);
}



template <typename T, size_t maxsize> inline
typename static_vector<T, maxsize>::reference
static_vector<T, maxsize>::back()
{
	return *get_element(size_ - 1);
}



template <typename T, size_t maxsize> inline
typename static_vector<T, maxsize>::const_reference
static_vector<T, maxsize>::back() const
{
	return *get_element(size_ - 1);
}



template <typename T, size_t maxsize> inline
void static_vector<T, maxsize>::push_back(const value_type& value)
{
	enforce_valid_size(size_ + 1);
	new (get_element(size_)) value_type(value);
	++size_;
}



template <typename T, size_t maxsize> inline
void static_vector<T, maxsize>::pop_back()
{
	get_element(--size_)->~value_type();
}



template <typename T, size_t maxsize>
void static_vector<T, maxsize>::insert(iterator position, const value_type& value)
{
	enforce_valid_size(size_ + 1);
	move_to_back(position, end(), 1);
    new (position) value_type(value);
	++size_;
}



template <typename T, size_t maxsize> inline
void static_vector<T, maxsize>::insert(iterator position, size_type n, const value_type& value)
{
	insert(position, n, value, meta::Type2Type<meta::True>());
}



template <typename T, size_t maxsize>
template <typename InputIterator> inline
void static_vector<T, maxsize>::insert(iterator position, InputIterator first, InputIterator last)
{
	insert(position, first, last, meta::Type2Type<typename meta::IsIntegralType<InputIterator>::Type>());
}



template <typename T, size_t maxsize>
typename static_vector<T, maxsize>::iterator
static_vector<T, maxsize>::erase(iterator position)
{
	position->~value_type();
	move_to_front(position + 1, end(), 1);
	--size_;
	return position;
}



template <typename T, size_t maxsize>
typename static_vector<T, maxsize>::iterator
static_vector<T, maxsize>::erase(iterator first, iterator last)
{
	for (iterator i = first; i != last; ++i)
	{
		i->~value_type();
	}
	const size_type n = last - first;;
	move_to_front(last, end(), n);
	size_ -= n;
	return first;
}



template <typename T, size_t maxsize>
void static_vector<T, maxsize>::clear()
{
	for (size_type i = 0; i < size_; ++i)
	{
		get_element(i)->~value_type();
	}
	size_ = 0;
}


	
// --- private -------------------------------------------------------------------------------------

template <typename T, size_t maxsize> inline
void static_vector<T, maxsize>::move_to_back(iterator first, iterator last, size_type step)
{
	while (last != first)
	{
		--last;
		new (last + step) value_type(*last);
		last->~value_type();
	}
}



template <typename T, size_t maxsize> inline
void static_vector<T, maxsize>::move_to_front(iterator first, iterator last, size_type step)
{
	while (first != last)
	{
		new (first - step) value_type(*first);
		first->~value_type();
		++first;
	}
}



template <typename T, size_t maxsize>
void static_vector<T, maxsize>::insert(iterator position, size_t n, value_type value,  
									   meta::Type2Type<meta::True>)
{
	enforce_valid_size(size_ + n);
	move_to_back(position, end(), n);
	for (const iterator last = position + n; position != last; ++position)
	{
		new (position) value_type(value);
	}
	size_ += n;
}
	


template <typename T, size_t maxsize>
template <typename InputIterator> 
void static_vector<T, maxsize>::insert(iterator position, InputIterator first, InputIterator last,
										meta::Type2Type<meta::False>)
{
	const size_type n = std::distance(first, last);
	enforce_valid_size(size_ + n);
	move_to_back(position, end(), n);
	while (first != last)
	{
		new (position++) value_type(*first++);
	}
	size_ += n;
}



template <typename T, size_t maxsize>
void static_vector<T, maxsize>::assign(size_t n, value_type value, meta::Type2Type<meta::True>)
{
	enforce_valid_size(n);
	clear();
	insert(begin(), n, value, meta::Type2Type<meta::True>());     				
}
	


template <typename T, size_t maxsize>
template <typename InputIterator> 
void static_vector<T, maxsize>::assign(InputIterator first, InputIterator last,
									   meta::Type2Type<meta::False>)
{
	const size_type n = std::distance(first, last);
	enforce_valid_size(n);
	clear();
	insert(begin(), n, value, meta::Type2Type<meta::False>());     				
}



template <typename T, size_t maxsize> inline
void static_vector<T, maxsize>::enforce_valid_size(size_type new_size) const
{
	if (new_size > max_size_)
	{
		throw std::length_error("lass::stde::static_vector cannot reallocate to expand capacity");
	}
}



// --- free ----------------------------------------------------------------------------------------

/** returns wether @a a and @a b are lexicographical idential.
 *  @relates static_vector
 *
 *  @param a first @c static_vector
 *  @param b second @c static_vector
 *
 *  returns true if <tt>a.size() == b.size()</tt> and each element of @a is considered
 *  equal to its corresponding element in @a b by using @c operator==
 *
 *  @complexity O(N) with N = <tt>a.size() == b.size() ? a.size() : 1</tt>
 */
template <typename T, size_t maxsize>
bool operator==(const static_vector<T, maxsize>& a, const static_vector<T, maxsize>& b)
{
	if (a.size() != b.size())
	{
		return false;
	}
	typedef typename static_vector<T, maxsize>::const_iterator const_iterator;
	const const_iterator a_end = a.end();
	for (const_iterator i = a.begin(), j = b.begin(); i != a_end; ++i, ++j)
	{
		if (*i != *j)
		{
			return false;
		}
	}
	return true;
}



/** returns wether @a a and @a b are not lexicographical idential.
 *  @relates static_vector
 *
 *  @param a first @c static_vector
 *  @param b second @c static_vector
 *
 *  Is equivalent to <tt>!(a == b)</tt>
 *
 *  @complexity O(N) with N = <tt>a.size() == b.size() ? a.size() : 1</tt>
 */
template <typename T, size_t maxsize>
bool operator!=(const static_vector<T, maxsize>& a, const static_vector<T, maxsize>& b)
{
	return !(a == b);
}



/** returns wether @a a is lexicographical less than @a b.
 *  @relates static_vector
 *
 *  @param a first @c static_vector
 *  @param b second @c static_vector
 *
 *  returns true if for the first difference between @a a and @a b the element of @a a is less
 *  than the corresponding one of @a b.  In case no different elements between @a and @a b can be
 *  found, it returns true if <tt>a.size() < b.size()</tt>
 *
 *  @complexity O(N) with N = <tt>std::min(a.size(), b.size())</tt>
 */
template <typename T, size_t maxsize>
bool operator<(const static_vector<T, maxsize>& a, const static_vector<T, maxsize>& b)
{
	typedef typename static_vector<T, maxsize>::const_iterator const_iterator;
	const const_iterator a_end = a.end();
	const const_iterator b_end = b.end();
	const_iterator i = a.begin();
	const_iterator j = b.begin();
	while (i != a_end && j != b_end)
	{
		if (!(*i < *j))
		{
			return false;
		}
		++i;
		++j;
	}
	return j != b_end;
}



/** returns wether @a b is lexicographical less than @a a.
 *  @relates static_vector
 *
 *  @param a first @c static_vector
 *  @param b second @c static_vector
 *
 *  Is equivalent to <tt>(b < a)</tt>
 *
 *  @complexity O(N) with N = <tt>std::min(a.size(), b.size())</tt>
 */
template <typename T, size_t maxsize>
bool operator>(const static_vector<T, maxsize>& a, const static_vector<T, maxsize>& b)
{
	return b < a;
}



/** returns wether @a a is lexicographical less or equal to @a b.
 *  @relates static_vector
 *
 *  @param a first @c static_vector
 *  @param b second @c static_vector
 *
 *  Is equivalent to <tt>!(b < a)</tt>
 *
 *  @complexity O(N) with N = <tt>std::min(a.size(), b.size())</tt>
 */
template <typename T, size_t maxsize>
bool operator<=(const static_vector<T, maxsize>& a, const static_vector<T, maxsize>& b)
{
	return !(b < a);
}



/** returns wether @a b is lexicographical less or equal to @a a.
 *  @relates static_vector
 *
 *  @param a first @c static_vector
 *  @param b second @c static_vector
 *
 *  Is equivalent to <tt>!(a < b)</tt>
 *
 *  @complexity O(N) with N = <tt>std::min(a.size(), b.size())</tt>
 */
template <typename T, size_t maxsize>
bool operator>=(const static_vector<T, maxsize>& a, const static_vector<T, maxsize>& b)
{
	return !(a < b);
}



/** @relates static_vector
 *  writes static_vector to output stream.
 *
 *  @param ostream should be a good stream.
 *  @param container @c static_vector to be written as [foo, bar, spam, ham]
 *
 *  @b complexity: O(N) with N = @c container.size()
 */
template <typename T, size_t maxsize, typename Char, typename Traits>
std::basic_ostream<Char, Traits>&
operator<<(std::basic_ostream<Char, Traits>& ostream,
		   const static_vector<T, maxsize>& container)
{
	return impl::print_sequence(
		ostream, container.begin(), container.end(), "[", ", ", "]");
}



/** @relates static_vector
 *  reads list from stream.
 *
 *  @param istream should be a good stream.
 *  @param container @c static_vector to be read as [foo, bar, spam, ham]
 *
 *  @b complexity: O(N) with N = number of elements to be read.
 *
 *  @pre @a maxsize should be large enough to read all elements
 */
template <typename T, size_t maxsize, typename Char, typename Traits>
std::basic_istream<Char, Traits>&
operator>>(std::basic_istream<Char, Traits>& istream,
		   static_vector<T, maxsize>& container)
{
	std::basic_istream<Char, Traits>& result =
		impl::readContainer<impl::static_vector_traits, impl::value_traits, T, Char>(
			istream, container, '[', ',', 0, ']');
	return result;
}


}

}

#endif

// EOF
