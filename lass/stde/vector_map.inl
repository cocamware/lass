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
 *	Copyright (C) 2004-2011 the Initial Developer.
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

#include "extended_iterator.h"

namespace lass
{
namespace stde
{

template <typename K, typename T, typename C, typename A>
vector_map<K, T, C, A>::vector_map(const key_compare& key_comp, const allocator_type& allocator):
	data_(allocator),
	key_comp_(key_comp)
{
}



template <typename K, typename T, typename C, typename A>
template <typename InputIterator>
vector_map<K, T, C, A>::vector_map(InputIterator first, InputIterator last, 
		const key_compare& key_comp, const allocator_type& allocator):
	data_(first, last, allocator),
	key_comp_(key_comp)
{
	std::stable_sort(data_.begin(), data_.end(), value_compare(key_comp_));
	iterator end = std::unique(data_.begin(), data_.end(),
		[this](const value_type& a, const value_type& b) { return !key_comp_(a.first, b.first) && !key_comp_(b.first, a.first); });
	data_.erase(end, data_.end());
}



template <typename K, typename T, typename C, typename A>
vector_map<K, T, C, A>::vector_map(const vector_map<K, T, C, A>& other):
	data_(other.data_),
	key_comp_(other.key_comp_)
{
}



template <typename K, typename T, typename C, typename A>
vector_map<K, T, C, A>::vector_map(vector_map<K, T, C, A>&& other) noexcept:
	data_(std::move(other.data_)),
	key_comp_(std::move(other.key_comp_))
{
}



template <typename K, typename T, typename C, typename A>
vector_map<K, T, C, A>::vector_map(std::initializer_list<value_type> init,
		const key_compare& key_comp, const allocator_type& allocator):
	vector_map(init.begin(), init.end(), key_comp, allocator)
{
}



template <typename K, typename T, typename C, typename A>
vector_map<K, T, C, A>::~vector_map()
{
}



template <typename K, typename T, typename C, typename A>
vector_map<K, T, C, A>& vector_map<K, T, C, A>::operator=(const vector_map<K, T, C, A>& other)
{
	vector_map<K, T, C, A> temp(other);
	swap(temp);
	return *this;
}



template <typename K, typename T, typename C, typename A>
vector_map<K, T, C, A>& vector_map<K, T, C, A>::operator=(vector_map<K, T, C, A>&& other) noexcept
{
	vector_map<K, T, C, A> temp(std::move(other));
	swap(temp);
	return *this;
}



template <typename K, typename T, typename C, typename A> inline
typename vector_map<K, T, C, A>::iterator
vector_map<K, T, C, A>::begin() noexcept
{
	return data_.begin();
}



template <typename K, typename T, typename C, typename A> inline
typename vector_map<K, T, C, A>::const_iterator
vector_map<K, T, C, A>::begin() const noexcept
{
	return data_.begin();
}



template <typename K, typename T, typename C, typename A> inline
typename vector_map<K, T, C, A>::const_iterator
vector_map<K, T, C, A>::cbegin() const noexcept
{
	return data_.cbegin();
}



template <typename K, typename T, typename C, typename A> inline
typename vector_map<K, T, C, A>::iterator
vector_map<K, T, C, A>::end() noexcept
{
	return data_.end();
}



template <typename K, typename T, typename C, typename A> inline
typename vector_map<K, T, C, A>::const_iterator
vector_map<K, T, C, A>::end() const noexcept
{
	return data_.end();
}



template <typename K, typename T, typename C, typename A> inline
typename vector_map<K, T, C, A>::const_iterator
vector_map<K, T, C, A>::cend() const noexcept
{
	return data_.cend();
}



template <typename K, typename T, typename C, typename A> inline
typename vector_map<K, T, C, A>::reverse_iterator
vector_map<K, T, C, A>::rbegin() noexcept
{
	return data_.rbegin();
}



template <typename K, typename T, typename C, typename A> inline
typename vector_map<K, T, C, A>::const_reverse_iterator
vector_map<K, T, C, A>::rbegin() const noexcept
{
	return data_.rbegin();
}



template <typename K, typename T, typename C, typename A> inline
typename vector_map<K, T, C, A>::const_reverse_iterator
vector_map<K, T, C, A>::crbegin() const noexcept
{
	return data_.crbegin();
}



template <typename K, typename T, typename C, typename A> inline
typename vector_map<K, T, C, A>::reverse_iterator
vector_map<K, T, C, A>::rend() noexcept
{
	return data_.rend();
}



template <typename K, typename T, typename C, typename A> inline
typename vector_map<K, T, C, A>::const_reverse_iterator
vector_map<K, T, C, A>::rend() const noexcept
{
	return data_.rend();
}



template <typename K, typename T, typename C, typename A> inline
typename vector_map<K, T, C, A>::const_reverse_iterator
vector_map<K, T, C, A>::crend() const noexcept
{
	return data_.crend();
}



template <typename K, typename T, typename C, typename A> inline
bool vector_map<K, T, C, A>::empty() const noexcept
{
	return data_.empty();
}



template <typename K, typename T, typename C, typename A> inline
typename vector_map<K, T, C, A>::size_type
vector_map<K, T, C, A>::size() const noexcept
{
	return data_.size();
}



template <typename K, typename T, typename C, typename A> inline
typename vector_map<K, T, C, A>::size_type
vector_map<K, T, C, A>::max_size() const noexcept
{
	return data_.max_size();
}



template <typename K, typename T, typename C, typename A> inline
typename vector_map<K, T, C, A>::mapped_type&
vector_map<K, T, C, A>::at(const key_type& key)
{
	auto it = find(key);
	if (it == end())
	{
		throw std::out_of_range("no such element");
	}
	return it->second;
}



template <typename K, typename T, typename C, typename A> inline
const typename vector_map<K, T, C, A>::mapped_type&
vector_map<K, T, C, A>::at(const key_type& key) const
{
	auto it = find(key);
	if (it == end())
	{
		throw std::out_of_range("no such element");
	}
	return it->second;
}



template <typename K, typename T, typename C, typename A> inline
typename vector_map<K, T, C, A>::mapped_type&
vector_map<K, T, C, A>::operator[](const key_type& key)
{
	return (try_emplace(key, mapped_type()).first)->second;
}



template <typename K, typename T, typename C, typename A> inline
typename vector_map<K, T, C, A>::mapped_type&
vector_map<K, T, C, A>::operator[](key_type&& key)
{
	return (try_emplace(std::move(key), mapped_type()).first)->second;
}



template <typename K, typename T, typename C, typename A>
std::pair<typename vector_map<K, T, C, A>::iterator, bool>
vector_map<K, T, C, A>::insert(const value_type& x)
{
	iterator i = lower_bound(x.first);
	if (i == end() || key_comp_(x.first, i->first))
	{
		i = data_.insert(i, x);
		return std::make_pair(i, true);
	}
	return std::make_pair(i, false);
}



template <typename K, typename T, typename C, typename A>
std::pair<typename vector_map<K, T, C, A>::iterator, bool>
vector_map<K, T, C, A>::insert(value_type&& x)
{
	iterator i = lower_bound(x.first);
	if (i == end() || key_comp_(x.first, i->first))
	{
		i = data_.insert(i, std::move(x));
		return std::make_pair(i, true);
	}
	return std::make_pair(i, false);
}



template <typename K, typename T, typename C, typename A> inline
typename vector_map<K, T, C, A>::iterator
vector_map<K, T, C, A>::insert(const_iterator hint, const value_type& x)
{
	return is_insert_position(hint, x.first)
		? data_.insert(hint, x)
		: insert(x).first;
}



template <typename K, typename T, typename C, typename A>
template <typename InputIterator>
void vector_map<K, T, C, A>::insert(InputIterator first, InputIterator last)
{
	while (first != last)
	{
		insert(*first++);
	}
}



template <typename K, typename T, typename C, typename A>
template <typename... Args>
std::pair<typename vector_map<K, T, C, A>::iterator, bool>
vector_map<K, T, C, A>::emplace(Args&&... args)
{
	value_type x{ std::forward<Args>(args)... };
	return insert(std::move(x));
}



template <typename K, typename T, typename C, typename A>
template <typename... Args>
typename vector_map<K, T, C, A>::iterator
vector_map<K, T, C, A>::emplace_hint(const_iterator hint, Args&&... args)
{
	value_type x{ std::forward<Args>(args)... };
	return is_insert_position(hint, x.first)
		? data_.insert(hint, std::move(x))
		: insert(std::move(x)).first;
}


template <typename K, typename T, typename C, typename A>
template<typename... Args> 
std::pair<typename vector_map<K, T, C, A>::iterator, bool>
vector_map<K, T, C, A>::try_emplace(const key_type& key, Args&&... args)
{
	iterator i = lower_bound(key);
	if (i == end() || key_comp_(key, i->first))
	{
		i = data_.emplace(i, key, std::forward<Args>(args)...);
		return std::make_pair(i, true);
	}
	return std::make_pair(i, false);
}


template <typename K, typename T, typename C, typename A>
template<typename... Args>
std::pair<typename vector_map<K, T, C, A>::iterator, bool>
vector_map<K, T, C, A>::try_emplace(key_type&& key, Args&&... args)
{
	key_type k{ std::move(key) };
	iterator i = lower_bound(k);
	if (i == end() || key_comp_(k, i->first))
	{
		i = data_.emplace(i, std::move(k), std::forward<Args>(args)...);
		return std::make_pair(i, true);
	}
	return std::make_pair(i, false);
}


template <typename K, typename T, typename C, typename A>
template<typename... Args>
typename vector_map<K, T, C, A>::iterator
vector_map<K, T, C, A>::try_emplace(const_iterator hint, const key_type& key, Args&&... args)
{
	return is_insert_position(hint, key)
		? data_.emplace(hint, key, std::forward<Args>(args)...)
		: try_emplace(key, std::forward<Args>(args)...).first;
}


template <typename K, typename T, typename C, typename A>
template<typename... Args>
typename vector_map<K, T, C, A>::iterator
vector_map<K, T, C, A>::try_emplace(const_iterator hint, key_type&& key, Args&&... args)
{
	key_type k{ std::move(key) };
	return is_insert_position(hint, k)
		? data_.emplace(hint, std::move(k), std::forward<Args>(args)...)
		: try_emplace(std::move(k), std::forward<Args>(args)...).first;
}


template <typename K, typename T, typename C, typename A> inline
void vector_map<K, T, C, A>::erase(const_iterator i)
{
	data_.erase(i);
}



template <typename K, typename T, typename C, typename A>
typename vector_map<K, T, C, A>::size_type
vector_map<K, T, C, A>::erase(const key_type& x)
{
	const const_iterator i = find(x);
	if (i != cend())
	{
		erase(i);
		return 1;
	}
	return 0;
}



template <typename K, typename T, typename C, typename A> inline
void vector_map<K, T, C, A>::erase(const_iterator first, const_iterator last)
{
	data_.erase(first, last);
}



template <typename K, typename T, typename C, typename A> inline
void vector_map<K, T, C, A>::swap(vector_map<K, T, C, A>& other) noexcept
{
	data_.swap(other.data_);
	std::swap(key_comp_, other.key_comp_);
}



template <typename K, typename T, typename C, typename A> inline
void vector_map<K, T, C, A>::clear() noexcept
{
	data_.clear();
}



template <typename K, typename T, typename C, typename A> inline
typename vector_map<K, T, C, A>::key_compare
vector_map<K, T, C, A>::key_comp() const
{
	return key_comp_;
}



template <typename K, typename T, typename C, typename A> inline
typename vector_map<K, T, C, A>::value_compare
vector_map<K, T, C, A>::value_comp() const
{
	return value_compare(key_comp_);
}



template <typename K, typename T, typename C, typename A>
typename vector_map<K, T, C, A>::iterator
vector_map<K, T, C, A>::find(const key_type& key)
{
	const iterator i = lower_bound(key);
	if (i == end() || key_comp_(key, i->first))
	{
		return end();
	}
	return i;
}



template <typename K, typename T, typename C, typename A>
typename vector_map<K, T, C, A>::const_iterator
vector_map<K, T, C, A>::find(const key_type& key) const
{
	const const_iterator i = lower_bound(key);
	if (i == end() || key_comp_(key, i->first))
	{
		return end();
	}
	return i;
}



template <typename K, typename T, typename C, typename A> inline
typename vector_map<K, T, C, A>::size_type
vector_map<K, T, C, A>::count(const key_type& key) const
{
	return find(key) != end() ? 1 : 0;
}



template <typename K, typename T, typename C, typename A> inline
bool vector_map<K, T, C, A>::contains(const key_type& key) const
{
	return find(key) != end();
}



template <typename K, typename T, typename C, typename A> inline
typename vector_map<K, T, C, A>::iterator
vector_map<K, T, C, A>::lower_bound(const key_type& key)
{
	return std::lower_bound(data_.begin(), data_.end(), key,
		[this](const value_type& x, const key_type& k) { return key_comp_(x.first, k); });
}



template <typename K, typename T, typename C, typename A> inline
typename vector_map<K, T, C, A>::const_iterator
vector_map<K, T, C, A>::lower_bound(const key_type& key) const
{
	return std::lower_bound(data_.cbegin(), data_.cend(), key,
		[this](const value_type& x, const key_type& k) { return key_comp_(x.first, k); });

}



template <typename K, typename T, typename C, typename A> inline
typename vector_map<K, T, C, A>::iterator
vector_map<K, T, C, A>::upper_bound(const key_type& key)
{
	return std::upper_bound(data_.begin(), data_.end(), key,
		[this](const key_type& k, const value_type& x) { return key_comp_(k, x.first); });

}



template <typename K, typename T, typename C, typename A> inline
typename vector_map<K, T, C, A>::const_iterator
vector_map<K, T, C, A>::upper_bound(const key_type& key) const
{
	return std::upper_bound(data_.cbegin(), data_.cend(), key,
		[this](const key_type& k, const value_type& x) { return key_comp_(k, x.first); });
}



template <typename K, typename T, typename C, typename A> inline
std::pair<typename vector_map<K, T, C, A>::iterator, typename vector_map<K, T, C, A>::iterator>
vector_map<K, T, C, A>::equal_range(const key_type& key)
{
	const value_type k(key, mapped_type{});
	return std::equal_range(data_.begin(), data_.end(), k,
		[this](const value_type& a, const value_type& b) { return key_comp_(a.first, b.first); });
}



template <typename K, typename T, typename C, typename A> inline
std::pair<typename vector_map<K, T, C, A>::const_iterator, typename vector_map<K, T, C, A>::const_iterator>
vector_map<K, T, C, A>::equal_range(const key_type& key) const
{
	const value_type k(key, mapped_type{});
	return std::equal_range(data_.cbegin(), data_.cend(), k,
		[this](const value_type& a, const value_type& b) { return key_comp_(a.first, b.first); });
}



template <typename K, typename T, typename C, typename A>
bool vector_map<K, T, C, A>::is_insert_position(const_iterator hint, const key_type& key) const
{
	// return true if prev(hint)->key < key < hint->key
	return (hint == cend() || key_comp_(key, hint->first)) &&
		(hint == cbegin() || key_comp_(stde::prev(hint)->first, key));
}


// --- free functions ------------------------------------------------------------------------------

/** @relates vector_map
 */
template <typename K, typename T, typename C, typename A>
bool operator==(const vector_map<K, T, C, A>& a, const vector_map<K, T, C, A>& b)
{
	return a.size() == b.size() && std::equal(a.begin(), a.end(), b.begin(), a.value_comp());
}



/** @relates vector_map
 */
template <typename K, typename T, typename C, typename A> inline
bool operator!=(const vector_map<K, T, C, A>& a, const vector_map<K, T, C, A>& b)
{
	return !(a == b);
}



/** @relates vector_map
 */
template <typename K, typename T, typename C, typename A> inline
bool operator<(const vector_map<K, T, C, A>& a, const vector_map<K, T, C, A>& b)
{
	return std::lexicographical_compare(a.begin(), a.end(), b.begin, b.end(), a.value_comp());
}
	


/** @relates vector_map
 */
template <typename K, typename T, typename C, typename A> inline
bool operator>(const vector_map<K, T, C, A>& a, const vector_map<K, T, C, A>& b)
{
	return b < a;
}
	


/** @relates vector_map
 */
template <typename K, typename T, typename C, typename A> inline
bool operator<=(const vector_map<K, T, C, A>& a, const vector_map<K, T, C, A>& b)
{
	return !(b < a);
}	



/** @relates vector_map
 */
template <typename K, typename T, typename C, typename A> inline
bool operator>=(const vector_map<K, T, C, A>& a, const vector_map<K, T, C, A>& b)
{
	return !(a < b);
}



/** @relates vector_map
 */
template <typename K, typename T, typename C, typename A, typename Char, typename Traits>
std::basic_ostream<Char, Traits>&
operator<<(std::basic_ostream<Char, Traits>& ostream, vector_map<K, T, C, A>& container)
{
	return impl::print_map<Char>(ostream, container.begin(), container.end(), "{", ", ", ": ", "}");
}



/** @relates vector_map
 */
template <typename Char, typename Traits, typename K, typename T, typename C, typename A>
std::basic_istream<Char, Traits>&
operator>>(std::basic_istream<Char, Traits>& istream, vector_map<K, T, C, A>& container)
{
	return impl::read_container<impl::set_traits, impl::pair_traits, std::pair<K, T>, Char>(
		istream, container, '{', ',', ':', '}');
}


}
}

namespace std
{

/** @relates vector_map
 */
template <typename K, typename T, typename C, typename A>
void swap(lass::stde::vector_map<K, T, C, A>& a, lass::stde::vector_map<K, T, C, A>& b) noexcept
{
	a.swap(b);
}

}

// EOF
