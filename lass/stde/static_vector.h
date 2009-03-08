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

#ifdef LASS_GUARDIAN_OF_INCLUSION_UTIL_PYSEQUENCE_H
#	include "../python/pysequence.h"
#endif

#endif

// EOF
