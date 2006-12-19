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

/** @class lass::stde::vector_map
 *  @brief a map-like container built on a sorted vector (write-rarely, read-many)
 *  @author Bram de Greve [Bramz]
 */

#ifndef LASS_GUARDIAN_OF_INCLUSION_STDE_LINEAR_MAP_H
#define LASS_GUARDIAN_OF_INCLUSION_STDE_LINEAR_MAP_H

#include "stde_common.h"

namespace lass
{
namespace stde
{

template
<
	typename Key,
	typename T,
	typename Compare = std::less<Key>,
	typename Allocator = std::allocator< std::pair<const Key, T> >
>
class vector_map
{
public:

	typedef Key key_type;
	typedef T mapped_type;
	typedef std::pair<const Key, T> value_type;
	typedef Compare key_compare;

	typedef Allocator allocator_type;
	typedef typename Allocator::reference reference;
	typedef typename Allocator::const_reference const_reference;
	typedef typename Allocator::pointer pointer;
	typedef typename Allocator::const_pointer const_pointer;
	typedef typename Allocator::size_type size_type;
	typedef typename Allocator::difference_type difference_type;

	typedef std::vector<value_type, Allocator> vector_type;
	typedef typename vector_type::iterator iterator;
	typedef typename vector_type::const_iterator const_iterator;
	typedef typename vector_type::reverse_iterator reverse_iterator;
	typedef typename vector_type::const_reverse_iterator const_reverse_iterator;

	class value_compare: public std::binary_function<value_type, value_type, bool>
	{
	public:
		bool operator()(const value_type& a, const value_type& b) const
		{
			return key_comp_(a.first, b.first);
		}
	private:
		friend class vector_map;
		value_compare(const key_compare& key_comp): key_comp_(key_comp) {}
		key_compare key_comp_;
	};

	explicit vector_map(const key_compare& key_comp, const allocator_type& allocator = Allocator());
	template <typename InputIterator>
	vector_map(InputIterator first, InputIterator last, 
			const key_compare& key_comp, const allocator_type& allocator = Allocator());
	vector_map(const vector_map<Key, T, Compare, Allocator>& other);
	~vector_map();

	vector_map<Key, T, Compare, Allocator>& operator=(
			const vector_map<Key, T, Compare, Allocator>& other);

	iterator begin();
	const_iterator begin() const;
	iterator end();
	const_iterator end() const;
	reverse_iterator rbegin();
	const_reverse_iterator rbegin() const;
	reverse_iterator rend();
	const_reverse_iterator rend() const;

	bool empty() const;
	size_type size() const;
	size_type max_size() const;

	mapped_type& operator[](const key_type& x);

	std::pair<iterator, pool> insert(const value_type& x);
	iterator insert(iterator position, const value_type& x);
	template <typename InputIterator> void insert(InputIterator first, InputIterator last);

	void erase(iterator position);
	size_type erase(const key_type& x);
	void erase(iterator first, iterator last);
	void swap(vector_map<Key, T, Compare, Allocator>& other);
	void clear();

	key_compare key_comp() const;
	value_compare value_comp() const;

	iterator find(const key_type& x);
	const_iterator find(const key_type& x) const;
	size_type count(const key_type& x) const;

	iterator lower_bound(const key_type& x);
	const_iterator lower_bound(const key_type& x) const;
	iterator upper_bound(const key_type& x);
	const_iterator upper_bound(const key_type& x) const;

	std::pair<iterator, iterator> equal_range(const key_type& x);
	std::pair<const_iterator, const_iterator> equal_range(const key_type& x) const;

private:

	vector_type data_;
	key_compare key_comp_;
};

template <typename Key, typename T, typename Compare, typename Allocator>
bool operator==(const vector_map<Key, T, Compare, Allocator>& a, 
		const vector_map<Key, T, Compare, Allocator>& b);

template <typename Key, typename T, typename Compare, typename Allocator>
bool operator<(const vector_map<Key, T, Compare, Allocator>& a, 
		const vector_map<Key, T, Compare, Allocator>& b);

template <typename Key, typename T, typename Compare, typename Allocator>
bool operator!=(const vector_map<Key, T, Compare, Allocator>& a, 
		const vector_map<Key, T, Compare, Allocator>& b);

template <typename Key, typename T, typename Compare, typename Allocator>
bool operator>(const vector_map<Key, T, Compare, Allocator>& a, 
		const vector_map<Key, T, Compare, Allocator>& b);

template <typename Key, typename T, typename Compare, typename Allocator>
bool operator>=(const vector_map<Key, T, Compare, Allocator>& a, 
		const vector_map<Key, T, Compare, Allocator>& b);

template <typename Key, typename T, typename Compare, typename Allocator>
bool operator<=(const vector_map<Key, T, Compare, Allocator>& a, 
		const vector_map<Key, T, Compare, Allocator>& b);

template <typename Key, typename T, typename Compare, typename Allocator, typename Char, typename Traits>
std::basic_ostream<Char, Traits>&
operator<<(std::basic_ostream<Char, Traits>& o_stream,
		   const vector_map<Key, T, Compare, Allocator>& container);
template <typename Key, typename T, typename Compare, typename Allocator, typename Char, typename Traits>
std::basic_istream<Char, Traits>&
operator>>(std::basic_istream<Char, Traits>& i_stream,
		   vector_map<Key, T, Compare, Allocator>& container);

}
}

namespace std
{

template <typename Key, typename T, typename Compare, typename Allocator>
void swap(vector_map<Key, T, Compare, Allocator>& a, vector_map<Key, T, Compare, Allocator>& b);

}

#include "vector_map.inl"

#endif

// EOF
