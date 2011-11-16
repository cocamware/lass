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

#ifndef LASS_GUARDIAN_OF_INCLUSION_STDE_TRANSFORM_ITERATOR_H
#define LASS_GUARDIAN_OF_INCLUSION_STDE_TRANSFORM_ITERATOR_H

#include "stde_common.h"

namespace lass
{
namespace stde
{

/** iterator adaptor to access members.
 *
 *  A prime example is to iterate over the keys or values in a @c  std::map.  
 *  The value_type of the map's iterator is a @c std::pair of key and value.  
 *  @c first_accessor_t is an accessor that will access the first element of
 *  the pair, and combined with @c access_iterator_t it allows you to 
 *  directly iterate over just the keys of the map.  
 *  A helper function @c first_iterator is provided to easily adapt a
 *  map's iterator:
 *
 *  @code
 *  // prints spam, ham, eggs
 *	std::map<std::string, int> map;
 *	map["spam"] = 1;
 *	map["ham"] = 2;
 *	map["eggs"] = 3;
 *	std::copy(
 *		first_iterator(map.begin()), 
 *		first_iterator(map.end()), 
 *		std::ostream_iterator<std::string>(std::cout, ", "));
 *	@endcode
 *
 *  @tparam Iterator the iterator type of the original sequence.  In the 
 *		example above, this would be @c std::map<std::string, int>::iterator
 *  @tparam Accessor functor type that takes an Iterator as argument and
 *		returns a reference to the accessed member.  Accessor shall also
 *		define @c value_type, @c reference and @c pointer.  An example is 
 *		@c first_accessor_t
 */
template 
<
	typename Iterator,
	typename Accessor
>
class access_iterator_t: 
	public std::iterator
	<
		typename std::iterator_traits<Iterator>::iterator_category,
		typename Accessor::value_type,
		typename std::iterator_traits<Iterator>::difference_type,
		typename Accessor::pointer,
		typename Accessor::reference
	>
{
public:
	typedef Iterator iterator_type;
	typedef Accessor accessor_type;
	typedef typename Accessor::value_type value_type;
	typedef typename std::iterator_traits<Iterator>::difference_type difference_type;
	typedef typename Accessor::pointer pointer;
	typedef typename Accessor::reference reference;

	access_iterator_t(iterator_type i = iterator_type(), accessor_type a = accessor_type()): current_(i), accessor_(a) {}

	iterator_type base() const { return current_; }
	pointer operator->() const { return &accessor_(current_); }
	reference operator*() const { return accessor_(current_); }

	access_iterator_t& operator++() { ++current_; return *this; }
	access_iterator_t operator++(int) { return access_iterator_t(current_++, accessor_); }
	access_iterator_t& operator--() { --current_; return *this; }
	access_iterator_t operator--(int) { return access_iterator_t(current_--, accessor_); }

	access_iterator_t& operator+=(difference_type n) { current_ += n; return *this; }
	access_iterator_t operator+(difference_type n) { return access_iterator_t(current_ + n, accessor_); }
	access_iterator_t& operator-=(difference_type n) { current_ -= n; return *this; }
	access_iterator_t operator-(difference_type n) { return access_iterator_t(current_ - n, accessor_); }

	reference operator[](difference_type n) const { return accessor_(current_[n]); }

public:
	iterator_type current_;
	accessor_type accessor_;
};

/** @relates access_iterator_t */
template <typename I, typename V> 
typename std::iterator_traits<I>::difference_type operator-(const access_iterator_t<I, V>& a, const access_iterator_t<I, V>& b) { return a.base() - b.base(); }
/** @relates access_iterator_t */
template <typename I, typename V> 
bool operator==(const access_iterator_t<I, V>& a, const access_iterator_t<I, V>& b) { return a.base() == b.base(); }
/** @relates access_iterator_t */
template <typename I, typename V> 
bool operator!=(const access_iterator_t<I, V>& a, const access_iterator_t<I, V>& b) { return a.base() != b.base(); }
/** @relates access_iterator_t */
template <typename I, typename V> 
bool operator<(const access_iterator_t<I, V>& a, const access_iterator_t<I, V>& b) { return a.base() < b.base(); }
/** @relates access_iterator_t */
template <typename I, typename V> 
bool operator>(const access_iterator_t<I, V>& a, const access_iterator_t<I, V>& b) { return a.base() > b.base(); }
/** @relates access_iterator_t */
template <typename I, typename V> 
bool operator<=(const access_iterator_t<I, V>& a, const access_iterator_t<I, V>& b) { return a.base() <= b.base(); }
/** @relates access_iterator_t */
template <typename I, typename V> 
bool operator>=(const access_iterator_t<I, V>& a, const access_iterator_t<I, V>& b) { return a.base() >= b.base(); }



template <typename BaseReference, typename Value> struct accessor_helper_t;

template <typename Base, typename Value>
struct accessor_helper_t<Base&, Value>: std::unary_function<Base&, Value&>
{
	typedef Value value_type;
	typedef Value& reference;
	typedef Value* pointer;
protected:
	~accessor_helper_t() {}
};

template <typename Base, typename Value>
struct accessor_helper_t<const Base&, Value>: std::unary_function<const Base&, const Value&>
{
	typedef Value value_type;
	typedef const Value& reference;
	typedef const Value* pointer;
protected:
	~accessor_helper_t() {}
};



template <typename It, typename First>
struct first_accessor_t: accessor_helper_t<typename std::iterator_traits<It>::reference, First>
{
	typedef typename accessor_helper_t<typename std::iterator_traits<It>::reference, First>::reference reference;
	reference operator()(It i) const { return i->first; }
};

/** @relates first_accessor_t 
 */
template <typename It> 
access_iterator_t< It, first_accessor_t<It, typename std::iterator_traits<It>::value_type::first_type > > 
first_iterator(It i)
{
	return access_iterator_t< It, first_accessor_t<It, typename std::iterator_traits<It>::value_type::first_type > >(i);
}



template <typename It, typename Second>
struct second_accessor_t: accessor_helper_t<typename std::iterator_traits<It>::reference, Second>
{
	typedef typename accessor_helper_t<typename std::iterator_traits<It>::reference, Second>::reference reference;
	reference operator()(It i) const { return i->second; }
};

/** @relates second_accessor_t 
 */
template <typename It> 
access_iterator_t< It, second_accessor_t<It, typename std::iterator_traits<It>::value_type::second_type > > 
second_iterator(It i)
{
	return access_iterator_t< It, second_accessor_t<It, typename std::iterator_traits<It>::value_type::second_type > >(i);
}


template <typename It, typename Value, typename Base>
struct member_accessor_t: accessor_helper_t<Base&, Value>
{
	typedef Value Base::*mem_ptr_type;
	typedef typename accessor_helper_t<Base&, Value>::reference reference;
	member_accessor_t(mem_ptr_type mem_ptr = mem_ptr_type()): mem_ptr_(mem_ptr) {}
	reference operator()(It i) const { return (*i).*mem_ptr_; }
private:
	mem_ptr_type mem_ptr_;
};

template <typename It, typename Value, typename Base>
access_iterator_t< It, member_accessor_t<It, Value, Base> >
member_iterator(It i, Value Base::*mem_ptr)
{
	return access_iterator_t< It, member_accessor_t<It, Value, Base> >(i, mem_ptr);
}

template <typename It, typename Value, typename Base>
access_iterator_t< It, member_accessor_t<It, const Value, const Base> >
const_member_iterator(It i, Value Base::*mem_ptr)
{
	return access_iterator_t< It, member_accessor_t<It, const Value, const Base> >(i, mem_ptr);
}

}
}

#endif

// EOF
