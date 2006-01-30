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

/** @class lass::stde::slist
 *  @brief a single linked list, just like the one in SGI STL (and STLport ...)
 *  @author Bram de Greve [Bramz]
 */

#ifndef LASS_GUARDIAN_OF_INCLUSION_STDE_SLIST_H
#define LASS_GUARDIAN_OF_INCLUSION_STDE_SLIST_H

#include "stde_common.h"
#include "extended_io.h"
#include "../meta/bool.h"
#include "../meta/type_2_type.h"

namespace lass
{
namespace stde
{

template <typename T, class Alloc = std::allocator<T> >
class slist: private Alloc
{
private:

	struct node_base_t
	{
		node_base_t* next;
	};

	struct node_t: public node_base_t
	{
		T value;
	};

public:

	typedef typename Alloc::template rebind<T>::other allocator_type;
	typedef T value_type;
	typedef typename allocator_type::pointer pointer;
	typedef typename allocator_type::const_pointer const_pointer;
	typedef typename allocator_type::reference reference;
	typedef typename allocator_type::const_reference const_reference;
	typedef typename allocator_type::size_type size_type;
	typedef typename allocator_type::difference_type difference_type;

	class const_iterator;
	friend class const_iterator;
	class iterator;
	friend class iterator;

	class iterator
	{
	public:
		typedef T value_type;
		typedef typename allocator_type::pointer pointer;
		typedef typename allocator_type::reference reference;
		typedef typename allocator_type::size_type size_type;
		typedef typename allocator_type::difference_type difference_type;
		typedef std::forward_iterator_tag iterator_category;
		iterator(): node_(0) {}
		reference operator*() const
		{
			LASS_ASSERT(node_);
			return static_cast<node_t* const>(node_)->value;
		}
		pointer operator->() const
		{
			LASS_ASSERT(node_);
			return &static_cast<node_t* const>(node_)->value;
		}
		iterator& operator++() { node_ = node_->next; return *this; }
		iterator operator++(int) { iterator result(*this); ++(*this); return result; }
		bool operator==(const iterator& other) const { return node_ == other.node_; }
		bool operator!=(const iterator& other) const { return !(*this == other); }
	private:
		friend class slist<T, Alloc>;
		friend class const_iterator;
		explicit iterator(node_base_t* node): node_(node) {}
		node_base_t* node_;
	};

	class const_iterator
	{
	public:
		typedef const T value_type;
		typedef typename allocator_type::const_pointer pointer;
		typedef typename allocator_type::const_reference reference;
		typedef typename allocator_type::size_type size_type;
		typedef typename allocator_type::difference_type difference_type;
		typedef std::forward_iterator_tag iterator_category;
		const_iterator(): node_(0) {}
		const_iterator(iterator i): node_(i.node_) {}
		const_reference operator*() const
		{
			LASS_ASSERT(node_);
			return static_cast<const node_t* const>(node_)->value;
		}
		const_pointer operator->() const
		{
			LASS_ASSERT(node_);
			return &static_cast<const node_t* const>(node_)->value;
		}
		const_iterator& operator++() { node_ = node_->next; return *this; }
		const_iterator operator++(int) { const_iterator result(*this); ++(*this); return result; }
		bool operator==(const const_iterator& other) const { return node_ == other.node_; }
		bool operator!=(const const_iterator& other) const { return !(*this == other); }
	private:
		friend class slist<T, Alloc>;
		explicit const_iterator(const node_base_t* node): node_(node) {}
		const node_base_t* node_;
	};

	explicit slist(const Alloc& allocator = Alloc());
	explicit slist(size_type n, const T& value = T(), const Alloc& allocator = Alloc());
	template <typename InputIterator> slist(InputIterator first, InputIterator last,
		const Alloc& allocator = Alloc());
	slist(const slist<T, Alloc>& other);
	~slist();

	slist<T, Alloc>& operator=(slist<T, Alloc> other);
	template <typename InputIterator> void assign(InputIterator first, InputIterator last);
	void assign(size_type n, const T& value = T());
	allocator_type get_allocator() const;

	iterator begin();
	const_iterator begin() const;
	iterator end();
	const_iterator end() const;
	iterator before_begin();
	const_iterator before_begin() const;

	iterator prior(iterator position) const;
	const_iterator prior(const_iterator position) const;
	iterator prior(iterator position, iterator start) const;
	const_iterator prior(const_iterator position, const_iterator start) const;

	bool empty() const;
	size_type size() const;
	size_type max_size() const;
	void resize(size_type n, const T& value = T());

	reference front();
	const_reference front() const;
	void push_front(const T& value);
	void pop_front();

	iterator insert(iterator position, const T& value);
	void insert(iterator position, size_type n, const T& value);
	template <typename InputIterator> void insert(iterator position, InputIterator first,
		InputIterator last);

	iterator insert_after(iterator position, const T& value);
	void insert_after(iterator position, size_type n, const T& value);
	template <typename InputIterator> void insert_after(iterator position, InputIterator first,
		InputIterator last);

	iterator erase(iterator position);
	iterator erase(iterator position, iterator last);
	void erase_after(iterator position);
	void erase_after(iterator position, iterator last);
	void swap(slist<T, Alloc>& other);
	void clear();

	void splice(iterator position, slist<T, Alloc>& other);
	void splice(iterator position, slist<T, Alloc>& other, iterator x);
	void splice(iterator position, slist<T, Alloc>& other, iterator first, iterator last);

	void splice_after(iterator position, slist<T, Alloc>& other);
	void splice_after(iterator position, slist<T, Alloc>& other, iterator before_x);
	void splice_after(iterator position, slist<T, Alloc>& other, iterator before_first,
		iterator before_last);

	void remove(const T& value);
	template <class UnaryPredicate> void remove_if(UnaryPredicate predicate);

	void unique();
	template <class BinaryPredicate> void unique(BinaryPredicate predicate);

	void merge(slist<T, Alloc>& other);
	template <class BinaryPredicate> void merge(slist<T, Alloc>& other, BinaryPredicate compare);

	void sort();
	template <class Compare> void sort(Compare compare);

	void reverse();

private:

	typedef typename allocator_type::template rebind<node_t>::other node_allocator_type;

	node_t* make_node(const T& value) const;
	void unlink_and_destroy_after(node_base_t* position) const;
	void link_after(node_base_t* position, node_base_t* node) const;
	void splice_after(node_base_t* position, node_base_t* before_first,
		node_base_t* before_last) const;

	void insert_after(iterator position, size_type n, const value_type& value,
		meta::Type2Type<meta::True> parameter_is_integral);
	template <typename InputIterator> void insert_after(iterator position, InputIterator first,
		InputIterator last, meta::Type2Type<meta::False> parameter_is_iterator);

	node_base_t head_;
};

template <typename T, class Alloc>
bool operator==(const slist<T, Alloc>& a, const slist<T, Alloc>& b);
template <typename T, class Alloc>
bool operator!=(const slist<T, Alloc>& a, const slist<T, Alloc>& b);
template <typename T, class Alloc>
bool operator<(const slist<T, Alloc>& a, const slist<T, Alloc>& b);
template <typename T, class Alloc>
bool operator>(const slist<T, Alloc>& a, const slist<T, Alloc>& b);
template <typename T, class Alloc>
bool operator<=(const slist<T, Alloc>& a, const slist<T, Alloc>& b);
template <typename T, class Alloc>
bool operator>=(const slist<T, Alloc>& a, const slist<T, Alloc>& b);

template <typename T, class Alloc, typename Char, typename Traits>
std::basic_ostream<Char, Traits>&
operator<<(std::basic_ostream<Char, Traits>& o_stream,
		   const slist<T, Alloc>& container);
template <typename T, class Alloc, typename Char, typename Traits>
std::basic_istream<Char, Traits>&
operator>>(std::basic_istream<Char, Traits>& i_stream,
		   slist<T, Alloc>& container);

}

}

#include "slist.inl"

#endif

// EOF
