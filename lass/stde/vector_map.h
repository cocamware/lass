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

/** @class lass::stde::vector_map
 *  @brief a map-like container built on a sorted vector (write-rarely, read-many)
 *  @author Bram de Greve [Bramz]
 */

#ifndef LASS_GUARDIAN_OF_INCLUSION_STDE_VECTOR_MAP_H
#define LASS_GUARDIAN_OF_INCLUSION_STDE_VECTOR_MAP_H

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

	/** @warning this actually _should_ be a std::pair<const Key, T> as required by associative containers.
	 *		But if we do, the underlying std::vector that stores the elements will bork on it as it also
	 *		requires value_type to be copy assignable.  Until we have a solution that works for both,
	 *		we'll open the security hole by have a mutable key.  It of course goes without saying that you
	 *		should never ever mutate the key of an element stored in a vector_map!
	 */
	typedef std::pair<Key, T> value_type;

	typedef Compare key_compare;
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

	typedef typename Allocator::template rebind<value_type>::other allocator_type;
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

	explicit vector_map(const key_compare& key_comp = key_compare(), const allocator_type& allocator = Allocator());
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

	std::pair<iterator, bool> insert(const value_type& x);
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

template <typename K, typename T, typename C, typename A>
void swap(lass::stde::vector_map<K, T, C, A>& a, lass::stde::vector_map<K, T, C, A>& b);

}

#include "vector_map.inl"

#ifdef LASS_GUARDIAN_OF_INCLUSION_UTIL_PYMAP_H
#	include "../python/pymap.h"
#endif

#endif

// EOF
