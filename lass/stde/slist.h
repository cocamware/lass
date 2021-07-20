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

/** @class lass::stde::slist
 *  @brief a single linked list, just like the one in SGI STL (and STLport ...)
 *  @author Bram de Greve [Bramz]
 */

#ifndef LASS_GUARDIAN_OF_INCLUSION_STDE_SLIST_H
#define LASS_GUARDIAN_OF_INCLUSION_STDE_SLIST_H

#include "stde_common.h"
#include "extended_io.h"
#include "../meta/bool.h"
#include "../meta/wrap.h"

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

	typedef T value_type;
	typedef value_type& reference;
	typedef const value_type& const_reference;
	typedef typename std::allocator_traits<Alloc>::template rebind_alloc<value_type> allocator_type;
	typedef typename std::allocator_traits<allocator_type>::pointer pointer;
	typedef typename std::allocator_traits<allocator_type>::const_pointer const_pointer;
	typedef typename std::allocator_traits<allocator_type>::size_type size_type;
	typedef typename std::allocator_traits<allocator_type>::difference_type difference_type;
	class const_iterator;
	friend class const_iterator;
	class iterator;
	friend class iterator;

	class iterator
	{
	public:
		typedef typename slist<T, Alloc>::value_type value_type;
		typedef typename slist<T, Alloc>::pointer pointer;
		typedef typename slist<T, Alloc>::reference reference;
		typedef typename slist<T, Alloc>::size_type size_type;
		typedef typename slist<T, Alloc>::difference_type difference_type;
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
		typedef typename slist<T, Alloc>::value_type value_type;
		typedef typename slist<T, Alloc>::pointer pointer;
		typedef typename slist<T, Alloc>::reference reference;
		typedef typename slist<T, Alloc>::size_type size_type;
		typedef typename slist<T, Alloc>::difference_type difference_type;
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

	typedef typename std::allocator_traits<allocator_type>::template rebind_alloc<node_t> node_allocator_type;

	node_t* make_node(const T& value) const;
	void unlink_and_destroy_after(node_base_t* position) const;
	void link_after(node_base_t* position, node_base_t* node) const;
	void splice_after(node_base_t* position, node_base_t* before_first,
		node_base_t* before_last) const;

	template <typename IntegerType>
	void insert_after(iterator position, IntegerType n, IntegerType value,
		meta::Wrap<meta::True> parameter_is_integral);
	template <typename InputIterator> void insert_after(iterator position, InputIterator first,
		InputIterator last, meta::Wrap<meta::False> parameter_is_iterator);

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
