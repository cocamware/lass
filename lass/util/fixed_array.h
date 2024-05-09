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
 *	Copyright (C) 2004-2024 the Initial Developer.
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
	typedef std::ptrdiff_t difference_type;
	typedef value_type* iterator;
	typedef const value_type* const_iterator;
	typedef std::reverse_iterator<iterator> reverse_iterator;
	typedef std::reverse_iterator<const_iterator> const_reverse_iterator;


	// iterator support

	iterator begin() { return array_; }
	iterator end() { return array_ + size_; }

	const_iterator begin() const { return array_; }
	const_iterator end() const { return array_ + size_; }

	reverse_iterator rbegin() { return reverse_iterator(end()); }
	reverse_iterator rend() { return reverse_iterator(begin()); }

	const_reverse_iterator rbegin() const { return const_reverse_iterator(end()); }
	const_reverse_iterator rend() const { return const_reverse_iterator(begin()); }


	// direct element access

	reference operator[](size_type iIndex)
	{
		LASS_ASSERT(iIndex < size_);
		return array_[iIndex];
	}

	const_reference operator[](size_type iIndex) const
	{
		LASS_ASSERT(iIndex < size_);
		return array_[iIndex];
	}

	reference at(size_type iIndex) 
	{
		if (iIndex >= size_)
		{
			throw std::out_of_range("index out of range in lass::util::FixedArray::at");
		}
		return array_[iIndex];
	}

	const_reference at(size_type iIndex) const
	{
		if (iIndex >= size_)
		{
			throw std::out_of_range("index out of range in lass::util::FixedArray::at");
		}
		return array_[iIndex];
	}

	reference front() { return array_[0]; }
	reference back() { return array_[size_ - 1]; }

	const_reference front() const { return array_[0]; }
	const_reference back() const { return array_[size_ - 1]; }


	// general

	size_type size() const { return size_; }
	size_type max_size() const { return size_; }
	bool empty() const { return false; }

private:

	value_type array_[size_];
};



template <typename T>
  class FixedArray<T, 0>
{
public:

	// type definitions

	typedef T value_type;
	typedef value_type& reference;
	typedef const value_type& const_reference;
	typedef value_type* pointer;
	typedef const value_type* const_pointer;
	typedef size_t size_type;
	typedef std::ptrdiff_t difference_type;
	typedef value_type* iterator;
	typedef const value_type* const_iterator;
	typedef std::reverse_iterator<iterator> reverse_iterator;
	typedef std::reverse_iterator<const_iterator> const_reverse_iterator;


	// iterator support

	iterator begin() { return 0; }
	iterator end() { return 0; }

	const_iterator begin() const { return 0; }
	const_iterator end() const { return 0; }

	reverse_iterator rbegin() { return reverse_iterator(end()); }
	reverse_iterator rend() { return reverse_iterator(begin()); }

	const_reverse_iterator rbegin() const { return const_reverse_iterator(end()); }
	const_reverse_iterator rend() const { return const_reverse_iterator(begin()); }


	// direct element access

	reference operator[](size_type)
	{
		LASS_ASSERT(false);
		return *begin();
	}

	const_reference operator[](size_type) const
	{
		LASS_ASSERT(false);
		return *begin();
	}

	reference at(size_type) 
	{
		throw std::out_of_range("index out of range in lass::util::FixedArray::at");
	}

	const_reference at(size_type) const
	{
		throw std::out_of_range("index out of range in lass::util::FixedArray::at");
	}

	reference front() { return *begin(); }
	reference back() { return *begin(); }

	const_reference front() const { return *begin(); }
	const_reference back() const { return *begin(); }


	// general

	size_type size() const { return 0; }
	size_type max_size() const { return 0; }
	bool empty() const { return true; }
};


}

}

#endif

// --- END OF FILE -------------------------------------------------------------
