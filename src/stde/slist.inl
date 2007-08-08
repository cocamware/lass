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

#include "../meta/is_integral.h"

namespace lass
{
namespace stde
{

namespace impl
{

/** @internal
 */
struct slist_traits
{
	template <typename Container, typename T>
	static void push(Container& container, const T& value)
	{
		container.push_front(value);
	}
	template <typename Container>
	static void temp_to_output(Container& temp, Container& output)
	{
		temp.swap(output);
	}
};

}



// --- public --------------------------------------------------------------------------------------

/** Creates an empty list, using the provided allocator
 *
 *  @param allocator to be used as memory model, defaults to @c Alloc().
 *
 *  @b complexity: O(1)
 */
template <typename T, class Alloc>
slist<T, Alloc>::slist(const Alloc& allocator):
	Alloc(allocator)
{
	head_.next = 0;
}



/** Creates an list with @a n elements - each of which is a copy of @a value - and an allocator.
 *
 *  @param n is the number of copies of @a value to put in the list.
 *  @param value will be used to copy construct all of the @a n elements of the list,
 *      defaults to @c T().
 *  @param allocator to be used as memory model, defaults to @c Alloc().
 *
 *  @b complexity: O(N) with N = @a n.
 */
template <typename T, class Alloc>
slist<T, Alloc>::slist(size_type n, const T& value, const Alloc& allocator):
	Alloc(allocator)
{
	head_.next = 0;
	insert_after(before_begin(), n, value);
}



/** creates a list with as elements a copy of a range, and an allocator.
 *
 *  @param first iterator to first element of range to be copied in list.
 *  @param last iterator to position after the last element to be copied in list.
 *      @a last must be forward reachable from @a first.
 *  @param allocator to be used as memory model, defaults to @c Alloc().
 *
 *  @b complexity: O(N) with N = <tt>std::distance(first, last)</tt>.
 */
template <typename T, class Alloc>
template <typename InputIterator>
slist<T, Alloc>::slist(InputIterator first, InputIterator last, const Alloc& allocator):
	Alloc(allocator)
{
	head_.next = 0;
	insert_after(before_begin(), first, last);
}



/** copies an entire list and its elements and allocator.
 *
 *  @param other the list to be copied
 *
 *  @b complexity: O(N) with N = @c other.size()
 */
template <typename T, class Alloc>
slist<T, Alloc>::slist(const slist<T, Alloc>& other):
	Alloc(other.get_allocator())
{
	head_.next = 0;
	insert_after(before_begin(), other.begin(), other.end());
}



/** destroys all elements and frees memory.
 *
 *  @b complexity: O(N) with N = @c this->size()
 */
template <typename T, class Alloc>
slist<T, Alloc>::~slist()
{
	while (head_.next)
	{
		unlink_and_destroy_after(&head_);
	}
}



/** replace @c *this by a copy of @a other.
 *
 *  @param other list to be copied
 *
 *  @b complexity: O(N1) + O(N2) with N1 = @c this->size() and N2 = @c other.size().
 */
template <typename T, class Alloc>
slist<T, Alloc>& slist<T, Alloc>::operator=(slist<T, Alloc> other)
{
	swap(other);
	return *this;
}



/** replace @c *this by a list copied from a range.
 *
 *  @param first iterator to first element of range to be copied in list.
 *  @param last iterator to position after the last element to be copied in list.
 *      @a last must be forward reachable from @a first.
 *
 *  @b complexity: O(N1) + O(N2) with N1 = @c this->size() and
 *      N2 = <tt>std::distance(first, last)</tt>.
 */
template <typename T, class Alloc>
template <typename ForwardIterator>
void slist<T, Alloc>::assign(ForwardIterator first, ForwardIterator last)
{
	slist<T, Alloc> temp(first, last, get_allocator());
	swap(temp);
}



/** replace @c *this by a list with @a n copies of @a value
 *
 *  @param n is the number of copies of @a value to put in the list.
 *  @param value will be used to copy construct all of the @a n elements of the list.
 *
 *  @b complexity: O(N1) + O(N2) with N1 = @c this->size() and N2 = @a n.
 */
template <typename T, class Alloc>
void slist<T, Alloc>::assign(size_type n, const T& value)
{
	slist<T, Alloc> temp(n, value, get_allocator());
	swap(temp);
}



/** returns the memory model of the list.
 *
 *  @b complexity: O(1)
 */
template <typename T, class Alloc>
typename slist<T, Alloc>::allocator_type
slist<T, Alloc>::get_allocator() const
{
	return *static_cast<const allocator_type*>(this);
}



/** returns an iterator to the first element of the list.
 *
 *  @b complexity: O(1)
 */
template <typename T, class Alloc>
typename slist<T, Alloc>::iterator
slist<T, Alloc>::begin()
{
	return iterator(head_.next);
}



/** returns an iterator to the first element of the list.
 *
 *  @b complexity: O(1)
 */
template <typename T, class Alloc>
typename slist<T, Alloc>::const_iterator
slist<T, Alloc>::begin() const
{
	return const_iterator(head_.next);
}



/** returns an iterator for the position after the last element of the list.
 *
 *  @b complexity: O(1)
 */
template <typename T, class Alloc>
typename slist<T, Alloc>::iterator
slist<T, Alloc>::end()
{
	return iterator(0);
}



/** returns an iterator for the position after the last element of the list.
 *
 *  @b complexity: O(1)
 */
template <typename T, class Alloc>
typename slist<T, Alloc>::const_iterator
slist<T, Alloc>::end() const
{
	return const_iterator(0);
}



/** returns an iterator for the position before the first element of the list.
 *  This iterator is not dereferencable!  It's only an iterator for which the next one is
 *  @c this->begin().  Use this one in methods like @c insert_after and @c erase_after when you want
 *  the method to affect on the first element in the list.  Using @c begin() would fail for this
 *  purpose the element after @c this->begin() is already the second in the list.
 *
 *  @b complexity: O(1)
 */
template <typename T, class Alloc>
typename slist<T, Alloc>::iterator
slist<T, Alloc>::before_begin()
{
	return iterator(&head_);
}



/** returns an iterator for the position before the first element of the list.
 *  This iterator is not dereferencable!  It's only an iterator for which the next one is
 *  @c this->begin().  Use this one in methods like @c insert_after and @c erase_after when you want
 *  the method to affect on the first element in the list.  Using @c begin() would fail for this
 *  purpose the element after @c this->begin() is already the second in the list.
 *
 *  @b complexity: O(1)
 */
template <typename T, class Alloc>
typename slist<T, Alloc>::const_iterator
slist<T, Alloc>::before_begin() const
{
	return const_iterator(&head_);
}



/** return the iterator that sits @a position and start searching for it from @a start.
 *  Equivalent to <tt>this->prior(position, this->before_begin())</tt>.
 *
 *  @param position must be valid in @c *this and should not be @c this->before_begin()
 *
 *  @b complexity: O(N) with N = <tt>std::distance(this->begin(), position)</tt>
 */
template <typename T, class Alloc>
typename slist<T, Alloc>::iterator
slist<T, Alloc>::prior(iterator position) const
{
	return prior(position, const_cast<slist<T, Alloc>*>(this)->before_begin());
}



/** return the iterator that sits @a position and start searching for it from @a start.
 *  Equivalent to <tt>this->prior(position, this->before_begin())</tt>.
 *
 *  @param position must be valid in @c *this and should not be @c this->before_begin()
 *
 *  @b complexity: O(N) with N = <tt>std::distance(this->begin(), position)</tt>
 */
template <typename T, class Alloc>
typename slist<T, Alloc>::const_iterator
slist<T, Alloc>::prior(const_iterator position) const
{
	return prior(position, before_begin());
}



/** return the iterator that sits @a position and start searching for it from @a start.
 *  The method scans linearly all iterators from @a start until it find the one for which the next
 *  one is @a position.  It returns the same result as @c std::prior(position) would have returned
 *  if the iterator had support for the decrement operation.
 *
 *  @param position must be valid in @c *this and should not be @c this->before_begin()
 *  @param start must be a valid iterator in @c [this->begin(),position)
 *
 *  @b complexity: O(N) with N = <tt>std::distance(start, position)</tt>
 */
template <typename T, class Alloc>
typename slist<T, Alloc>::iterator
slist<T, Alloc>::prior(iterator position, iterator start) const
{
	iterator result = start;
	while (result.node_->next != position.node_)
	{
		LASS_ASSERT(result.node_->next);
		++result;
	}
	return result;
}



/** return the iterator that sits @a position and start searching for it from @a start.
 *  The method scans linearly all iterators from @a start until it find the one for which the next
 *  one is @a position.  It returns the same result as @c std::prior(position) would have returned
 *  if the iterator had support for the decrement operation.
 *
 *  @param position must be valid in @c *this and should not be @c this->before_begin()
 *  @param start must be a valid iterator in @c [this->begin(),position)
 *
 *  @b complexity: O(N) with N = <tt>std::distance(start, position)</tt>
 */
template <typename T, class Alloc>
typename slist<T, Alloc>::const_iterator
slist<T, Alloc>::prior(const_iterator position, const_iterator start) const
{
	const_iterator result = start;
	while (result.node_->next != position.node_)
	{
		LASS_ASSERT(result.node_->next);
		++result;
	}
	return result;
}



/** returns wether the list is empty.
 *  Is semantically identical to <tt>this->size() == 0</tt>, but it is a lot faster.
 *  So use it whenever you can ... even to do the dishes.
 *
 *  @b complexity: O(1)
 */
template <typename T, class Alloc>
bool slist<T, Alloc>::empty() const
{
	return head_.next == 0;
}



/** returns the N, the number of elements in the list.
 *
 *  @b complexity: O(N) with N = @c this->size().
 */
template <typename T, class Alloc>
typename slist<T, Alloc>::size_type
slist<T, Alloc>::size() const
{
	size_type n = 0;
	for (node_base_t* i = head_.next; i != 0; i = i->next)
	{
		++n;
	}
	return n;
}



/** returns a @e very rough estimation on what's the maximum number of elements you can
 *  ever have in an @c slist.
 *
 *  Frankly, we don't know this number, so we return the highest number we know of =)
 *
 *  @b complexity: O(1)
 */
template <typename T, class Alloc>
typename slist<T, Alloc>::size_type
slist<T, Alloc>::max_size() const
{
	// if anyone knows something better, please feel free to do so [Bramz]
	//
	return size_type(-1);
}



/** at the end of the list, inserts copies of @a value or erases elements so that list size is @a n.
 *
 *  @param n the size of the list after inserting or erasing elements.
 *  @param value if the list must grow, copies of @a value are inserted at the end.
 *      Defaults to T().
 *
 *  @b complexity: O(N) with N = @a n.
 */
template <typename T, class Alloc>
void slist<T, Alloc>::resize(size_type n, const T& value)
{
	node_base_t* i = &head_;
	while (i->next && n)
	{
		--n;
		i = i->next;
	}
	if (n)
	{
		insert_after(iterator(i), n, value);
	}
	else
	{
		while (i->next)
		{
			unlink_and_destroy_after(i);
		}
	}
}



/** returns reference to first element in list.
 *
 *  @c *this must not be empty.  There's no check on wheter the first element exists.
 *  It's semantically equivalent to @c *this->begin()
 *
 *  @b complexity: O(1)
 */
template <typename T, class Alloc>
typename slist<T, Alloc>::reference
slist<T, Alloc>::front()
{
	return static_cast<node_t*>(head_.next)->value;
}



/** returns reference to first element in list.
 *
 *  @c *this must not be empty.  There's no check on wheter the first element exists.
 *  It's semantically equivalent to @c *this->begin()
 *
 *  @b complexity: O(1)
 */
template <typename T, class Alloc>
typename slist<T, Alloc>::const_reference
slist<T, Alloc>::front() const
{
	return static_cast<node_t*>(head_.next)->value;
}



/** insert a copy of @a value at the front of the list so that the current list comes behind it.
 *
 *  @param value to be inserted at front
 *
 *  It's semantically equivalent to <tt>this->insert(this->begin(), value)</tt>.
 *  All iterators stay valid.
 *
 *  @b complexity: O(1)
 */
template <typename T, class Alloc>
void slist<T, Alloc>::push_front(const T& value)
{
	node_base_t* node = make_node(value);
	node->next = head_.next;
	head_.next = node;
}



/** erases the first element in the list.
 *
 *  It's semantically equivalent to <tt>this->erase(this->begin())</tt>.
 *  All iterators except @c this->begin() stay valid.
 *
 *  @b complexity: O(1)
 */
template <typename T, class Alloc>
void slist<T, Alloc>::pop_front()
{
	unlink_and_destroy_after(&head_);
}



/** inserts a new element before iterator @a position and returns iterator to it.
 *
 *  @param position must be valid in @c *this and should not be @c this->before_begin()
 *  @param value to be inserted
 *
 *  @warning this operation is in linear time because it has to call @c prior(position).
 *  Try to use @c insert_after whenever you can.
 *
 *  All iterators stay valid.
 *
 *  @b complexity: O(N) with N = <tt>std::distance(this->begin(), position)</tt>
 */
template <typename T, class Alloc>
typename slist<T, Alloc>::iterator
slist<T, Alloc>::insert(iterator position, const T& value)
{
	const iterator before_position = prior(position);
	return insert_after(before_position, value);
}



/** inserts @a n copies of @a value before iterator @a position.
 *
 *  @param position must be valid in @c *this and should not be @c this->before_begin()
 *  @param n number of copies to be inserted.
 *  @param value to be inserted
 *
 *  @warning this operation is in linear time because it has to call @c prior(position).
 *  Try to use @c insert_after whenever you can.
 *
 *  All iterators stay valid.
 *
 *  @b complexity: O(N1) + O(N2) with N1 = <tt>std::distance(this->begin(), position)</tt>
 *      and N2 = @a n.
 */
template <typename T, class Alloc>
void slist<T, Alloc>::insert(iterator position, size_type n, const T& value)
{
	const iterator before_position = prior(position);
	insert_after(before_position, n, value);
}



/** inserts a copy of a range before iterator @a position.
 *
 *  @param position must be valid in @c *this and should not be @c this->before_begin()
 *  @param first iterator to first element to be inserted.
 *  @param last iterator to position after last element to be insert.  Must be forward reachable
 *      from @a first.
 *
 *  @warning this operation is in linear time because it has to call @c prior(position).
 *  Try to use @c insert_after whenever you can.
 *
 *  All iterators stay valid.
 *
 *  @b complexity: O(N1) + O(N2) with N1 = <tt>std::distance(this->begin(), position)</tt>
 *      and N2 = @a <tt>std::distance(first, last)</tt>
 */
template <typename T, class Alloc>
template <typename InputIterator>
void slist<T, Alloc>::insert(iterator position, InputIterator first, InputIterator last)
{
	const iterator before_position = prior(position);
	insert_after(before_position, first, last);
}



/** inserts a new element after iterator @a position and returns iterator to it.
 *
 *  @param position must be valid in @c *this and should not be @c this->end()
 *  @param value to be inserted
 *
 *  All iterators stay valid.
 *
 *  @b complexity: O(1)
 */
template <typename T, class Alloc>
typename slist<T, Alloc>::iterator
slist<T, Alloc>::insert_after(iterator position, const T& value)
{
	node_base_t* new_node = make_node(value);
	link_after(position.node_, new_node);
	return iterator(new_node);
}



/** inserts @a n copies of @a value after iterator @a position.
 *
 *  @param position must be valid in @c *this and should not be @c this->end()
 *  @param n number of copies to be inserted.
 *  @param value to be inserted
 *
 *  All iterators stay valid.
 *
 *  @b complexity: O(N) with N = @a n.
 */
template <typename T, class Alloc>
void slist<T, Alloc>::insert_after(iterator position, size_type n, const T& value)
{
	node_base_t* node = position.node_;
	for (size_type i = 0; i < n; ++i)
	{
		node_t* new_node = make_node(value);
		link_after(node, new_node);
		node = new_node;
	}
}



/** inserts a copy of a range before iterator @a position.
 *
 *  @param position must be valid in @c *this and should not be @c this->end()
 *  @param first iterator to first element to be inserted.
 *  @param last iterator to position after last element to be insert.  Must be forward reachable
 *      from @a first.
 *
 *  All iterators stay valid.
 *
 *  @b complexity: O(N) with N = <tt>std::distance(first, last)</tt>.
 */
template <typename T, class Alloc>
template <typename InputIterator>
void slist<T, Alloc>::insert_after(iterator position, InputIterator first, InputIterator last)
{
	insert_after(position, first, last, meta::Wrap<typename meta::IsIntegral<InputIterator>::Type>());
}



/** removes element at iterator @a position from the list and return iterator to the next element.
 *
 *  @param position element to be erased.
 *      Must be valid in @c *this and should not be @c this->before_begin() or @c this->end().
 *
 *  @warning this operation is in linear time because it has to call @c prior(position).
 *  Try to use @c erase_after whenever you can.
 *
 *  All iterators except @a position stay valid.
 *
 *  @b complexity: O(N) with N = <tt>std::distance(this->begin(), position)</tt>
 */
template <typename T, class Alloc>
typename slist<T, Alloc>::iterator
slist<T, Alloc>::erase(iterator position)
{
	iterator before_position = prior(position);
	erase_after(before_position);
	return ++before_position;
}



/** removes element in range from the list and return iterator to the next element.
 *
 *  @param position iterator to first element to be erased.
 *      Must be valid in @c *this and should not be @c this->before_begin() or @c this->end().
 *  @param last iterator to position behind last element to be erased, must be valid in
 *      @c *this and forward reachable from @a position.
 *
 *  @warning this operation is in linear time because it has to call @c prior(position).
 *  Try to use @c erase_after whenever you can.
 *
 *  All iterators except these in range [ @a position, @a last ) stay valid.
 *
 *  @b complexity: O(N1) + O(N2) with N1 = <tt>std::distance(this->begin(), position)</tt> and
 *      N2 = <tt>std::distance(position, last)</tt>
 */
template <typename T, class Alloc>
typename slist<T, Alloc>::iterator
slist<T, Alloc>::erase(iterator position, iterator last)
{
	iterator before_position = prior(position);
	erase_after(before_position, last);
	return ++before_position;
}



/** removes element after iterator @a position from the list.
 *
 *  @param position iterator to position before element to be erased, must be valid in @c *this,
 *      and should not be @c this->end()
 *
 *  All iterators except the one after @a position stay valid.
 *
 *  @b complexity: O(1)
 */
template <typename T, class Alloc>
void slist<T, Alloc>::erase_after(iterator position)
{
	LASS_ASSERT(position.node_ && position.node_->next);
	unlink_and_destroy_after(position.node_);
}



/** removes element in range from the list and return iterator to the next element.
 *
 *  @param position iterator to position before first element to be erased.
 *      Must be valid in @c *this and should not be @c this->end()
 *  @param last iterator to position behind last element to be erased, must be valid in
 *      @c *this and forward reachable from @a position.
 *
 *  All iterators except these in range ( @a position, @a last ) stay valid.
 *
 *  @b complexity: O(N) with N = <tt>std::distance(position, last)</tt>
 */
template <typename T, class Alloc>
void slist<T, Alloc>::erase_after(iterator position, iterator last)
{
	LASS_ASSERT(position.node_);
	while (position.node_->next != last.node_)
	{
		erase_after(position);
	}
}



/** exchange all data between two lists.
 *
 *  All iterators stay valid.
 *
 *  @b complexity: O(1)
 */
template <typename T, class Alloc>
void slist<T, Alloc>::swap(slist<T, Alloc>& other)
{
	LASS_ASSERT(get_allocator() == other.get_allocator());
	std::swap(head_.next, other.head_.next);
}



/** removes all elements from the list
 *
 *  @b complexity: O(N) with N = @c this->size()
 */
template <typename T, class Alloc>
void slist<T, Alloc>::clear()
{
	slist<T, Alloc> temp(get_allocator());
	swap(temp);
}



/** moves all elements from @a other to @c *this before @a position without copying, and
 *  clears @a other.
 *
 *  @param position must be valid in @c *this and should not be @c this->before_begin()
 *  @param other must not be the same as @c *this (<tt>&other != this</tt>).
 *
 *  @warning this operation is in linear time because it has to call @c prior(position).
 *  Try to use @c splice_after whenever you can.
 *
 *  All iterators stay valid.
 *
 *  @b complexity: O(N1) + O(N2) with N1 = <tt>std::distance(this->begin(), position)</tt>
 *      and N2 = @c other.size()
 */
template <typename T, class Alloc>
void slist<T, Alloc>::splice(iterator position, slist<T, Alloc>& other)
{
	const iterator before_position = prior(position);
	splice_after(before_position, other);
}



/** moves @a x from @a other to @c *this before @a position without copying.
 *
 *  @param position must be valid in @c *this and should not be @c this->before_begin()
 *  @param other may be the same as @c *this.
 *  @param x must be valid in @c other and should not be @c other.before_begin()
 *
 *  @warning this operation is in linear time because it has to call @c prior(position) and
 *  @c prior(x).  Try to use @c splice_after whenever you can.
 *
 *  All iterators stay valid.
 *
 *  @b complexity: O(N1) + O(N2) with N1 = <tt>std::distance(this->begin(), position)</tt>
 *      and N2 = <tt>std::distance(other.begin(), x)</tt>
 */
template <typename T, class Alloc>
void slist<T, Alloc>::splice(iterator position, slist<T, Alloc>& other, iterator x)
{
	const iterator before_position = prior(position);
	const iterator before_x = other.prior(x);
	splice_after(before_position, other, before_x);
}



/** moves [ @a first , @a last ) from @a other to @c *this before @a position without copying.
 *
 *  @param position must be valid in @c *this and should not be @c this->before_begin().
 *      @a position should not be an element of [ @a first , @a last ).
 *  @param other may be the same as @c *this.
 *  @param first must be valid in @c other and should not be @c other.before_begin()
 *  @param last must be valid in @c other and should be forward reachable from @a first.
 *
 *  @warning this operation is in linear time because it has to call @c prior(position) and
 *  @c prior(x).  Try to use @c splice_after whenever you can.
 *
 *  All iterators stay valid.
 *
 *  @b complexity: O(N1) + O(N2) with N1 = <tt>std::distance(this->begin(), position)</tt>
 *      and N2 = <tt>std::distance(other.begin(), x)</tt>
 */
template <typename T, class Alloc>
void slist<T, Alloc>::splice(iterator position, slist<T, Alloc>& other, iterator first, iterator last)
{
	const iterator before_position = prior(position);
	const iterator before_first = other.prior(first);
	const iterator before_last = other.prior(last, before_first);
	splice_after(before_position, other, before_first, before_last);
}



/** moves all elements from @a other to @c *this after @a position without copying, and
 *  clears @a other.
 *
 *  @param position must be valid in @c *this and should not be @c this->end()
 *  @param other must not be the same as @c *this (<tt>&other != this</tt>).
 *
 *  All iterators stay valid.
 *
 *  @b complexity: O(N) with N = @c other.size()
 */
template <typename T, class Alloc>
void slist<T, Alloc>::splice_after(iterator position, slist<T, Alloc>& other)
{
	node_base_t* other_before_last = &other.head_;
	while (other_before_last->next)
	{
		other_before_last = other_before_last->next;
	}

	splice_after(position.node_, &other.head_, other_before_last);
}



/** moves element before @a before_x from @a other to @c *this after @a position without copying.
 *
 *  @param position must be valid in @c *this and should not be @c this->end()
 *  @param other may be the same as @c *this.
 *  @param before_x must be valid in @c other and should not be @c other.end()
 *
 *  All iterators stay valid.
 *
 *  @b complexity: O(1)</tt>
 */
template <typename T, class Alloc>
void slist<T, Alloc>::splice_after(iterator position, slist<T, Alloc>& /*other*/, iterator before_x)
{
	splice_after(position.node_, before_x.node_, before_x.node_->next);
}



/** moves ( before_first, before_last ] from @a other to @c *this after @a position without copying.
 *
 *  @param position must be valid in @c *this and should not be @c this->end().
 *      @a position should not be an element of ( before_first, before_last ].
 *  @param other may be the same as @c *this.
 *  @param before_first must be valid in @c other and should not be @c other.end()
 *  @param before_last must be valid in @c other and should be forward reachable from @a before_first.
 *
 *  All iterators stay valid.
 *
 *  @b complexity: O(1)
 */
template <typename T, class Alloc>
void slist<T, Alloc>::splice_after(iterator position, slist<T, Alloc>& /*other*/, iterator before_first,
							   iterator before_last)
{
	splice_after(position.node_, before_first.node_, before_last.node_);
}



/** removes all elements with iterarors @c i for which <tt>*i == value</tt>
 *
 *  @param value value of the elements that must be removed.
 *
 *  @b complexity: O(N) with N = @c this->size()
 */
template <typename T, class Alloc>
void slist<T, Alloc>::remove(const T& value)
{
	node_base_t* node = &head_;
	while (node->next)
	{
		if (static_cast<node_t*>(node->next)->value == value)
		{
			unlink_and_destroy_after(node);
		}
		else
		{
			node = node->next;
		}
	}
}



/** removes all elements with iterarors @c i for which @c predicate(*i) yields @c true.
 *
 *  @param predicate should returns true for the elements that must be removed.
 *
 *  @b complexity: O(N) with N = @c this->size()
 */
template <typename T, class Alloc>
template <class UnaryPredicate>
void slist<T, Alloc>::remove_if(UnaryPredicate predicate)
{
	node_base_t* node = &head_;
	while (node->next)
	{
		if (predicate(static_cast<node_t*>(node->next)->value))
		{
			unlink_and_destroy_after(node);
		}
		else
		{
			node = node->next;
		}
	}
}



/** removes all but the first element in every consecutive group of equal elements.
 *
 *  The relative order of elements that are not removed is unchanged, and iterators to elements
 *  that are not removed remain valid.
 *
 *  @b complexity: O(N) with N = @c this->size()
 */
template <typename T, class Alloc>
void slist<T, Alloc>::unique()
{
	node_base_t* node = head_.next;
	if (!node)
	{
		return;
	}
	while (node->next)
	{
		if (static_cast<node_t*>(node)->value == static_cast<node_t*>(node->next)->value)
		{
			unlink_and_destroy_after(node);
		}
		else
		{
			node = node->next;
		}
	}
}



/** removes all but the first element in every consecutive group of equivalent elements by a predicate.
 *
 *  @param predicate two elements @c *i and @c *j are considered equivalent if
 *      <tt>predicate(*i, *j)</tt> is true.
 *
 *  The relative order of elements that are not removed is unchanged, and iterators to elements
 *  that are not removed remain valid.
 *
 *  @b complexity: O(N) with N = @c this->size()
 */
template <typename T, class Alloc>
template <class BinaryPredicate>
void slist<T, Alloc>::unique(BinaryPredicate predicate)
{
	node_base_t* node = head_.next;
	if (!node)
	{
		return;
	}
	while (node->next)
	{
		if (predicate(static_cast<node_t*>(node)->value, static_cast<node_t*>(node->next)->value))
		{
			unlink_and_destroy_after(node);
		}
		else
		{
			node = node->next;
		}
	}
}



/** Merge two sorted containers to one big sorted.
 *
 *  Both @c *this and @a other must be sorted according to @c operator<, and they must be
 *  distinct (<tt>&other != this</tt>). This function removes all of @a other's elements and
 *  inserts them in order into @c *this. The merge is stable; that is, if an element from @c *this
 *  is equivalent to one from @a other, then the element from @c *this will precede the one from
 *  @a other. All iterators to elements in @c *this and @a other remain valid.
 *
 *  @param other the other sorted @c slist
 *
 *  @b complexity: O(N1) + O(N2) with N1 = @c this->size() and N2 = @c other.size()
 */
template <typename T, class Alloc>
void slist<T, Alloc>::merge(slist<T, Alloc>& other)
{
	merge(other, std::less<value_type>());
}



/** Merge two by @a compare sorted containers to one big sorted.
 *
 *  Both @c *this and @a other must be sorted according to @a compare, and they must be
 *  distinct (<tt>&other != this</tt>). This function removes all of @a other's elements and
 *  inserts them in order into @c *this. The merge is stable; that is, if an element from @c *this
 *  is equivalent to one from @a other, then the element from @c *this will precede the one from
 *  @a other. All iterators to elements in @c *this and @a other remain valid.
 *
 *  @param other the other sorted @c slist
 *  @param compare must define strict weak ordening.  Both containers must be sorted
 *      by this predicate.
 *
 *  @b complexity: O(N1) + O(N2) with N1 = @c this->size() and N2 = @c other.size()
 */
template <typename T, class Alloc>
template <class BinaryPredicate>
void slist<T, Alloc>::merge(slist<T, Alloc>& other, BinaryPredicate compare)
{
	node_base_t* node = &head_;
	while (node->next && other.head_.next)
	{
		if (compare(static_cast<node_t*>(other.head_.next)->value,
					static_cast<node_t*>(node->next)->value))
		{
			splice_after(node, &other.head_, other.head_.next);
		}
		node = node->next;
	}
	if (other.head_.next)
	{
		node->next = other.head_.next;
		other.head_.next = 0;
	}
}



/** Sorts @c *this according to @c operator<.
 *
 *  The sort is stable, that is, the relative order of equivalent elements is preserved.
 *  All iterators remain valid and continue to point to the same elements.
 *
 *  @b complexity: O(N log N) with N = @c this->size()
 */
template <typename T, class Alloc>
void slist<T, Alloc>::sort()
{
	sort(std::less<value_type>());
}







/** Sorts @c *this according to @a compare.
 *
 *  @param compare must define strict weak ordening.  Both containers must be sorted
 *      by this predicate.
 *
 *  The sort is stable, that is, the relative order of equivalent elements is preserved.
 *  All iterators remain valid and continue to point to the same elements.
 *
 *  @b complexity: O(N log N) with N = @c this->size()
 */
template <typename T, class Alloc>
template <class BinaryPredicate>
void slist<T, Alloc>::sort(BinaryPredicate compare)
{
	if (head_.next && head_.next->next)
	{
		slist<T, Alloc> carry;
		slist<T, Alloc> counter[64];
		size_type fill = 0;
		while (!empty())
		{
			splice_after(&carry.head_, &head_, head_.next);
			size_type i = 0;
			while (i < fill && !counter[i].empty())
			{
				counter[i].merge(carry, compare);
				carry.swap(counter[i]);
				++i;
			}
			carry.swap(counter[i]);
			if (i == fill)
			{
				++fill;
			}
		}
		for (size_type i = 1; i < fill; ++i)
		{
			counter[i].merge(counter[i - 1], compare);
		}
		swap(counter[fill - 1]);
	}
}



/** reverses the order of the elements.
 *
 *  All iterators stay valid.
 *
 *  @b complexity: O(N) with N = @c this->size()
 */
template <typename T, class Alloc>
void slist<T, Alloc>::reverse()
{
	node_base_t* begin = 0;
	while (head_.next)
	{
		node_base_t* node = head_.next;
		head_.next = node->next;
		node->next = begin;
		begin = node;
	}
	head_.next = begin;
}

// --- protected -----------------------------------------------------------------------------------



// --- private -------------------------------------------------------------------------------------

/** @internal
 */
template <typename T, class Alloc>
typename slist<T, Alloc>::node_t*
slist<T, Alloc>::make_node(const T& value) const
{
	allocator_type allocator = get_allocator();
	node_allocator_type node_allocator = node_allocator_type(allocator);

	node_t* node = node_allocator.allocate(1);
	try
	{
		allocator.construct(&node->value, value);
	}
	catch (...)
	{
		node_allocator.deallocate(node, 1);
		throw;
	}
	node->next = 0;
	return node;
}



/** @internal
 */
template <typename T, class Alloc>
void slist<T, Alloc>::unlink_and_destroy_after(node_base_t* before) const
{
	allocator_type allocator = get_allocator();
	node_allocator_type node_allocator = node_allocator_type(allocator);

	LASS_ASSERT(before && before->next);

	node_base_t* node = before->next;
	before->next = node->next;
	allocator.destroy(&static_cast<node_t*>(node)->value);
	node_allocator.deallocate(static_cast<node_t*>(node), 1);
}



/** @internal
 */
template <typename T, class Alloc>
void slist<T, Alloc>::link_after(node_base_t* position, node_base_t* node) const
{
	LASS_ASSERT(position && node);
	node->next = position->next;
	position->next = node;
}



/** @internal
 */
template <typename T, class Alloc>
void slist<T, Alloc>::splice_after(node_base_t* position, node_base_t* before_first,
								   node_base_t* before_last) const
{
	LASS_ASSERT(before_first != before_last);
	if (position != before_first && position != before_last)
	{
		node_base_t* const first = before_first->next;
		before_first->next = before_last->next;
		before_last->next = position->next;
		position->next = first;
	}
}



/** @internal
 */
template <typename T, class Alloc>
void slist<T, Alloc>::insert_after(iterator position, size_type n, const value_type& value,
								   meta::Wrap<meta::True>)
{
	node_base_t* node = position.node_;
	for (size_type i = 0; i < n; ++i)
	{
		node_t* new_node = make_node(value);
		link_after(node, new_node);
		node = new_node;
	}
}



/** @internal
 */
template <typename T, class Alloc>
template <typename InputIterator>
void slist<T, Alloc>::insert_after(iterator position, InputIterator first, InputIterator last,
								   meta::Wrap<meta::False>)
{
	while (first != last)
	{
		node_t* new_node = make_node(*first);
		link_after(position.node_, new_node);
		++position;
		++first;
	}
}



// --- free functions ------------------------------------------------------------------------------

/** returns wether @a a and @a b are lexicographical idential.
 *  @relates slist
 *
 *  @param a first @c slist
 *  @param b second @c slist
 *
 *  returns true if <tt>a.size() == b.size()</tt> and each element of @a is considered
 *  equal to its corresponding element in @a b by using @c operator==
 *
 *  @complexity O(N) with N = <tt>a.size() == b.size() ? a.size() : 1</tt>
 */
template <typename T, class Alloc>
bool operator==(const slist<T, Alloc>& a, const slist<T, Alloc>& b)
{
	typedef typename slist<T, Alloc>::const_iterator const_iterator;
	const const_iterator a_end = a.end();
	const const_iterator b_end = b.end();
	const_iterator i = a.begin();
	const_iterator j = b.begin();
	while (i != a_end && j != b_end)
	{
		if (*i != *j)
		{
			return false;
		}
		++i;
		++j;
	}
	return i == a_end && j == b_end;
}



/** returns wether @a a and @a b are not lexicographical idential.
 *  @relates slist
 *
 *  @param a first @c slist
 *  @param b second @c slist
 *
 *  Is equivalent to <tt>!(a == b)</tt>
 *
 *  @complexity O(N) with N = <tt>a.size() == b.size() ? a.size() : 1</tt>
 */
template <typename T, class Alloc>
bool operator!=(const slist<T, Alloc>& a, const slist<T, Alloc>& b)
{
	return !(a == b);
}



/** returns wether @a a is lexicographical less than @a b.
 *  @relates slist
 *
 *  @param a first @c slist
 *  @param b second @c slist
 *
 *  returns true if for the first difference between @a a and @a b the element of @a a is less
 *  than the corresponding one of @a b.  In case no different elements between @a and @a b can be
 *  found, it returns true if <tt>a.size() < b.size()</tt>
 *
 *  @complexity O(N) with N = <tt>std::min(a.size(), b.size())</tt>
 */
template <typename T, class Alloc>
bool operator<(const slist<T, Alloc>& a, const slist<T, Alloc>& b)
{
	typedef typename slist<T, Alloc>::const_iterator const_iterator;
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
 *  @relates slist
 *
 *  @param a first @c slist
 *  @param b second @c slist
 *
 *  Is equivalent to <tt>(b < a)</tt>
 *
 *  @complexity O(N) with N = <tt>std::min(a.size(), b.size())</tt>
 */
template <typename T, class Alloc>
bool operator>(const slist<T, Alloc>& a, const slist<T, Alloc>& b)
{
	return b < a;
}



/** returns wether @a a is lexicographical less or equal to @a b.
 *  @relates slist
 *
 *  @param a first @c slist
 *  @param b second @c slist
 *
 *  Is equivalent to <tt>!(b < a)</tt>
 *
 *  @complexity O(N) with N = <tt>std::min(a.size(), b.size())</tt>
 */
template <typename T, class Alloc>
bool operator<=(const slist<T, Alloc>& a, const slist<T, Alloc>& b)
{
	return !(b < a);
}



/** returns wether @a b is lexicographical less or equal to @a a.
 *  @relates slist
 *
 *  @param a first @c slist
 *  @param b second @c slist
 *
 *  Is equivalent to <tt>!(a < b)</tt>
 *
 *  @complexity O(N) with N = <tt>std::min(a.size(), b.size())</tt>
 */
template <typename T, class Alloc>
bool operator>=(const slist<T, Alloc>& a, const slist<T, Alloc>& b)
{
	return !(a < b);
}



/** @relates slist
 *  writes list to output stream.
 *
 *  @param ostream should be a good stream.
 *  @param container @c slist to be written as [foo, bar, spam, ham]
 *
 *  @b complexity: O(N) with N = @c container.size()
 */
template <typename T, class Alloc, typename Char, typename Traits>
std::basic_ostream<Char, Traits>&
operator<<(std::basic_ostream<Char, Traits>& ostream,
		   const slist<T, Alloc>& container)
{
	return impl::print_sequence(
		ostream, container.begin(), container.end(), "[", ", ", "]");
}



/** @relates slist
 *  reads list from stream.
 *
 *  @param istream should be a good stream.
 *  @param container @c slist to be read as [foo, bar, spam, ham]
 *
 *  @b complexity: O(N) with N = number of elements to be read.
 */
template <typename T, class Alloc, typename Char, typename Traits>
std::basic_istream<Char, Traits>&
operator>>(std::basic_istream<Char, Traits>& istream,
		   slist<T, Alloc>& container)
{
	std::basic_istream<Char, Traits>& result =
		impl::read_container<impl::slist_traits, impl::value_traits, T, Char>(
			istream, container, '[', ',', 0, ']');
	container.reverse();
	return result;
}



}

}

// EOF
