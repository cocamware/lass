/** @file
 *  @author Bram de Greve (bramz@users.sourceforge.net)
 *  @author Tom De Muer (tomdemuer@users.sourceforge.net)
 *
 *  Distributed under the terms of the GPL (GNU Public License)
 *
 *  The LASS License:
 *
 *  Copyright 2004 Bram de Greve and Tom De Muer
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



/** @struct lass::util::FixedArray
 *  @brief compile time fixed array that also supports zero length
 *  @author Bram de Greve [Bramz]
 *  @date 2002-2003
 *
 *  FixedArray is an array of compile-time-fixed length.  The unique property of this implementation
 *  is that this length can also be zero.  As a surplus, this class is given an STL container
 *  interface with iterators etc.
 */



#ifndef LASS_GUARDIAN_OF_INCLUSION_UTIL_FIXED_ARRAY_H
#define LASS_GUARDIAN_OF_INCLUSION_UTIL_FIXED_ARRAY_H

// --- OLD INTERFACES ----------------------------------------------------------

#include "util_common.h"
#include <cstddef>
#include <iterator>



// --- NEW INTERFACES ----------------------------------------------------------

namespace lass
{
namespace util
{

template <typename T, size_t size_>
class FixedArray
{
public:

	// type definitions

	typedef T value_type;
	typedef value_type& reference;
	typedef const value_type& const_reference;
	typedef value_type* pointer;
	typedef const value_type* const_pointer;
	typedef size_t size_type;
	typedef ptrdiff_t difference_type;
	typedef value_type* iterator;
	typedef const value_type* const_iterator;

	// stlport thing to declare the std::reverse_iterator and std::const_reverse_iterator
	//
	typedef std::reverse_iterator<iterator> reverse_iterator;
	typedef std::reverse_iterator<const_iterator> const_reverse_iterator;


	// iterator support

	iterator begin() { return holder_.begin(); }
	iterator end() { return holder_.end(); }

	const_iterator begin() const { return holder_.begin(); }
	const_iterator end() const { return holder_.end(); }

	reverse_iterator rbegin() { return reverse_iterator(holder_.end()); }
	reverse_iterator rend() { return reverse_iterator(holder_.begin()); }

	const_reverse_iterator rbegin() const { return const_reverse_iterator(holder_.end()); }
	const_reverse_iterator rend() const { return const_reverse_iterator(holder_.begin()); }


	// direct element access

	reference operator[](size_type iIndex)
	{
		return *holder_[iIndex];
	}

	const_reference operator[](size_type iIndex) const
	{
		return *holder_[iIndex];
	}

	reference at(size_type iIndex) 
	{
		if (!holder_.inRange(iIndex))
		{
			throw std::out_of_range("index out of range in lass::util::FixedArray::at");
		}
		return *holder_[iIndex];
	}

	const_reference at(size_type iIndex) const
	{
		if (!holder_.inRange(iIndex))
		{
			throw std::out_of_range("index out of range in lass::util::FixedArray::at");
		}
		return *holder_[iIndex];
	}

	reference front() { return this->operator[](0); }
	reference back() { return this->operator[](size_ - 1); }

	const_reference front() const { return this->operator[](0); }
	const_reference back() const { return this->operator[](size_ - 1); }


	// general

	size_type size() const { return size_; }
	size_type max_size() const { return size_; }
	bool empty() const { return size_ == 0; }

private:

	template<size_type size_>
	class Holder
	{
	public:
		iterator begin() { return array_; }
		iterator end() { return array_ + size_; }
		const_iterator begin() const { return array_; }
		const_iterator end() const { return array_ + size_; }
		T* operator[](size_type iIndex)
		{
			LASS_ASSERT(inRange(iIndex));
			return &array_[iIndex];
		}
		const T* operator[](size_type iIndex) const
		{
			LASS_ASSERT(inRange(iIndex));
			return &array_[iIndex];
		}
		bool inRange(size_type iIndex) const { return iIndex < size_; }
	private:
		value_type array_[size_];
	};

	template<>
	class Holder<0>
	{
	public:
		iterator begin() { return 0; }
		iterator end() { return 0; }
		const_iterator begin() const { return 0; }
		const_iterator end() const { return 0; }
		T* operator[](size_type iIndex)
		{
			LASS_ASSERT(false); // you should never be in here!
			return 0;
		}
		const T* operator[](size_type iIndex) const
		{
			LASS_ASSERT(false); // you should never be in here!
			return 0;
		}
		bool inRange(size_type iIndex) const { return false; }
	};

	Holder<size_> holder_;
};



}

}

#endif

// --- END OF FILE -------------------------------------------------------------
