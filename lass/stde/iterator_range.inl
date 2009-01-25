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

namespace lass
{
namespace stde
{

// --- public --------------------------------------------------------------------------------------

/** @brief empty range (sort of, read warning).
 *
 *	@warning Problemski: MSVC has something like _HAS_ITERATOR_DEBUGGING, which means it has the 
 *		annoying habit of whenever two iterators come together (e.g. on comparison), to check
 *		whether they belong to the same container.  While this makes sense in most situations,
 *		(ignoring any possible performance hit), we don't have any container when we create
 *		empty ranges.  Oh, did I tell you it also checks if the iterator belongs to ANY container
 *		AT ALL?.  So, even while we almost physically force first_ and last_ to be equal (think 
 *		Fat Ed's super fix-it way), MSVC will bork on almost any occasion.  
 *		Moral of the story: while you can _create_ default constructed ranges, DO NOT USE THEM, 
 *		like EVER!
 */
// TODO: Find a workaround for this containerless _HAS_ITERATOR_DEBUGGING madness [Bramz]
// https://sourceforge.net/tracker2/?func=detail&aid=2517798&group_id=118315&atid=680765
template <typename I> inline
iterator_range<I>::iterator_range(): 
	first_()
{
	// a default range is empty, so make sure both iterators are really the same!
	//
	last_ = first_;
}



template <typename I> inline
iterator_range<I>::iterator_range(iterator first, iterator last): 
	first_(first), 
	last_(last) 
{
}



template <typename I> inline
typename iterator_range<I>::iterator 
iterator_range<I>::begin() const 
{ 
	return first_; 
}



template <typename I> inline
typename iterator_range<I>::iterator 
iterator_range<I>::end() const 
{ 
	return last_; 
}



template <typename I> inline
typename iterator_range<I>::iterator 
iterator_range<I>::operator->() const 
{ 
	LASS_ASSERT(!this->empty()); 
	return first_; 
}



template <typename I> inline
typename iterator_range<I>::reference 
iterator_range<I>::operator*() const 
{ 
	LASS_ASSERT(!this->empty()); 
	return *first_; 
}



template <typename I> inline
typename iterator_range<I>::reference 
iterator_range<I>::operator[](difference_type index) const
{ 
	LASS_ASSERT(index < this->size());
	iterator result = first_;
	std::advance(result, index);
	return *result;
}



template <typename I> inline
iterator_range<I>& 
iterator_range<I>::operator++() 
{ 
	LASS_ASSERT(first_ != last_); 
	++first_; 
	return *this; 
}



template <typename I> inline
iterator_range<I>
iterator_range<I>::operator++(int) 
{ 
	iterator_range<I> result(*this); 
	this->operator++(); 
	return result; 
}



template <typename I> inline
const typename iterator_range<I>::difference_type 
iterator_range<I>::size() const 
{
	return std::distance(first_, last_);
}



template <typename I> inline
const bool iterator_range<I>::empty() const 
{ 
	return first_ == last_; 
}



template <typename I> inline
const bool iterator_range<I>::operator!() const 
{ 
	return this->empty(); 
}



template <typename I> inline
iterator_range<I>::operator const num::SafeBool() const
{ 
	return this->empty() ? num::safeFalse : num::safeTrue; 
}



template <typename I> inline
void iterator_range<I>::swap(iterator_range<I>& other)
{
	std::swap(first_, other.first_);
	std::swap(last_, other.last_);
}



// --- protected -----------------------------------------------------------------------------------



// --- private -------------------------------------------------------------------------------------



// --- free functions ------------------------------------------------------------------------------

template <typename I> inline
iterator_range<I> range(const I& a, const I& b)
{
	return iterator_range<I>(a, b);
}



template <typename I> inline
const bool operator==(const iterator_range<I>& a, const iterator_range<I>& b)
{
	return a.begin() == b.begin() && a.end() == b.end();
}



template <typename I> inline
const bool operator!=(const iterator_range<I>& a, const iterator_range<I>& b)
{
	return !(a == b);
}



}

}

// EOF
