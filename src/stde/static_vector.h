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

/** @class lass::stde::static_vector
 *  @brief it looks like a vector, it smells like a vector, but it only uses a fixed amout of memory
 *  @author Bram de Greve [Bramz]
 */

#ifndef LASS_GUARDIAN_OF_INCLUSION_STDE_STATIC_VECTOR_H
#define LASS_GUARDIAN_OF_INCLUSION_STDE_STATIC_VECTOR_H

#include "stde_common.h"
#include "extended_io.h"
#include "../meta/bool.h"
#include "../meta/wrap.h"

namespace lass
{
namespace stde
{

template <typename T, size_t maxsize>
class static_vector
{
public:

	typedef T& reference;
	typedef const T& const_reference;
	typedef T* iterator;
	typedef const T* const_iterator;
	typedef size_t size_type;
	typedef ptrdiff_t difference_type;
	typedef T value_type;
	typedef T* pointer;
	typedef const T* const_pointer;
	typedef std::reverse_iterator<iterator> reverse_iterator;
	typedef std::reverse_iterator<const_iterator> const_reverse_iterator;

	static_vector();
	static_vector(size_type n, const value_type& value = value_type());
	template <typename InputIterator> static_vector(InputIterator first, InputIterator last);
	~static_vector();

	void assign(size_type n = 0, const value_type& value = value_type());
	template <typename InputIterator> void assign(InputIterator first, InputIterator last);

	iterator begin();
	const_iterator begin() const;
	iterator end();
	const_iterator end() const;
	reverse_iterator rbegin();
	const_reverse_iterator rbegin() const;
	reverse_iterator rend();
	const_reverse_iterator rend() const;

	size_type size() const;
	size_type max_size() const;
	void resize(size_type n, const value_type& value = value_type());
	size_type capacity() const;
	bool empty() const;
	void reserve(size_type n);

	reference operator[](size_type i);
	const_reference operator[](size_type i) const;
	reference at(size_type i);
	const_reference at(size_type i) const;
	reference front();
	const_reference front() const;
	reference back();
	const_reference back() const;

	void push_back(const value_type& value);
	void pop_back();
	void insert(iterator position, const value_type& value);
	void insert(iterator position, size_type n, const value_type& value);
	template <typename InputIterator> void insert(iterator position, InputIterator first, 
		InputIterator last);
	iterator erase(iterator position);
	iterator erase(iterator first, iterator last);
	void clear();
	void swap(static_vector<T, maxsize>& iOther);

private:

	enum { max_size_ = maxsize };

	pointer get_element(size_type i) { return reinterpret_cast<pointer>(data_) + i; }
	const_pointer get_element(size_type i) const { return reinterpret_cast<const_pointer>(data_) + i; }
	void insert(iterator position, size_type n, value_type value, 
		meta::Wrap<meta::True> parameter_is_integral);
	template <typename InputIterator> void insert(iterator position, InputIterator first, 
		InputIterator last, meta::Wrap<meta::False> parameter_is_iterator);
	void assign(size_type n, value_type value, meta::Wrap<meta::True> parameter_is_integral);
	template <typename InputIterator> void assign(InputIterator first, InputIterator last, 
		meta::Wrap<meta::False> parameter_is_iterator);
	void move_to_back(iterator first, iterator last, size_type step);
	void move_to_front(iterator first, iterator last, size_type step);
	void enforce_valid_size(size_type new_size) const;

	char data_[max_size_ * sizeof(T)];
	size_type size_;
};

template <typename T, size_t max_size>
bool operator==(const static_vector<T, max_size>& x, const static_vector<T, max_size>& y);
template <typename T, size_t max_size>
bool operator!=(const static_vector<T, max_size>& x, const static_vector<T, max_size>& y);
template <typename T, size_t max_size>
bool operator<(const static_vector<T, max_size>& x, const static_vector<T, max_size>& y);
template <typename T, size_t max_size>
bool operator>(const static_vector<T, max_size>& x, const static_vector<T, max_size>& y);
template <typename T, size_t max_size>
bool operator<=(const static_vector<T, max_size>& x, const static_vector<T, max_size>& y);
template <typename T, size_t max_size>
bool operator>=(const static_vector<T, max_size>& x, const static_vector<T, max_size>& y);

template <typename T, size_t max_size, typename Char, typename Traits>
std::basic_ostream<Char, Traits>& operator<<(std::basic_ostream<Char, Traits>& o_stream, 
	const static_vector<T, max_size>& container);

template <typename T, size_t max_size, typename Char, typename Traits>
std::basic_istream<Char, Traits>& operator>>(std::basic_istream<Char, Traits>& i_stream,
	static_vector<T, max_size>& container);

}

}

#include "static_vector.inl"

#endif

// EOF
