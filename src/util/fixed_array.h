/** 
*   @author Bram de Greve (bramz@users.sourceforge.net)
*   @author Tom De Muer (tomdemuer@users.sourceforge.net)
*
*	Distributed under the terms of the GPL (GNU Public License)
*
* 	The LASS License:
*   
*	Copyright 2004 Bram de Greve and Tom De Muer
*
*   LASS is free software; you can redistribute it and/or modify
*   it under the terms of the GNU General Public License as published by
*   the Free Software Foundation; either version 2 of the License, or
*   (at your option) any later version.
*
*   This program is distributed in the hope that it will be useful,
*   but WITHOUT ANY WARRANTY; without even the implied warranty of
*   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
*   GNU General Public License for more details.
*
*   You should have received a copy of the GNU General Public License
*   along with this program; if not, write to the Free Software
*   Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
*


 *
 *  $Revision$
 *  $Date$
 *  $Author$ (last commit)
 */



/** @struct lass::util::FixedArray
 *  @brief compile time fixed array that also supports zero length
 *  @author Bram de Greve [BdG]
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

template <typename T, int size_>
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

    iterator begin() throw() { return holder_.begin(); }
    iterator end() throw() { return holder_.end(); }

    const_iterator begin() const throw() { return holder_.begin(); }
    const_iterator end() const throw() { return holder_.end(); }

    reverse_iterator rbegin() throw() { return reverse_iterator(holder_.end()); }
    reverse_iterator rend() throw() { return reverse_iterator(holder_.begin()); }

    const_reverse_iterator rbegin() const throw() { return const_reverse_iterator(holder_.end()); }
    const_reverse_iterator rend() const throw() { return const_reverse_iterator(holder_.begin()); }


    // direct element access

    reference operator[](size_type iIndex) 
    {
        LASS_ASSERT(holder_[iIndex] != 0); 
        return *holder_[iIndex]; 
    }

    const_reference operator[](size_type iIndex) const
    {   
        LASS_ASSERT(holder_[iIndex] != 0);
        return *holder_[iIndex]; 
    }

    reference at(size_type iIndex) { return this->operator[](LASS_ENFORCE_INDEX(iIndex, size_)); }
    const_reference at(size_type iIndex) const { return this->operator[](LASS_ENFORCE_INDEX(iIndex, size_)); }

    reference front() { return this->operator[](0); }
    reference back() { return this->operator[](size_ - 1); }
    
    const_reference front() const { return this->operator[](0); }
    const_reference back() const { return this->operator[](size_ - 1); }


    // general

    size_type size() const throw() { return size_; }
    size_type max_size() const throw() { return size_; }
    bool empty() const throw() { return size_ == 0; }

private:

    template<size_type size_> 
    class Holder
    { 
    public:
        iterator begin() throw() { return array_; }
        iterator end() throw() { return array_ + size_; }
        const_iterator begin() const throw() { return array_; }
        const_iterator end() const throw() { return array_ + size_; }
        T* operator[](unsigned iIndex) 
        { 
            LASS_ASSERT(iIndex >= 0 && iIndex < size_);
            return &array_[iIndex]; 
        }
        const T* operator[](unsigned iIndex) const
        { 
            LASS_ASSERT(iIndex >= 0 && iIndex < size_);
            return &array_[iIndex]; 
        }
    private:
        value_type array_[size_];
    };

    template<> 
    class Holder<0>
    { 
    public:
        iterator begin() throw() { return 0; }
        iterator end() throw() { return 0; }
        const_iterator begin() const throw() { return 0; }
        const_iterator end() const throw() { return 0; }
        T* operator[](unsigned iIndex) throw() 
        {
            LASS_ASSERT(false); // you should never be in here!
            return 0; 
        }
        const T* operator[](unsigned iIndex) const throw() 
        {
            LASS_ASSERT(false); // you should never be in here!
            return 0; 
        }
    };

    Holder<size_> holder_;
};



}

}

#endif

/*
 *  $Log$
 *  Revision 1.15  2004/09/06 13:24:47  tgdemuer
 *  *** empty log message ***
 *
 *  Revision 1.14  2004/09/06 13:08:44  tgdemuer
 *  *** empty log message ***
 *
 *  Revision 1.13  2004/07/01 13:22:59  bdegreve
 *  *** empty log message ***
 *
 *  Revision 1.12  2004/07/01 09:55:16  bdegreve
 *  weird bugs
 *
 *  Revision 1.11  2004/06/21 13:26:25  bdegreve
 *  final fix for reverse iterators (i hope :)
 *
 *  Revision 1.10  2004/06/21 12:29:59  bdegreve
 *  *** empty log message ***
 *
 *  Revision 1.9  2004/06/21 11:24:43  bdegreve
 *  reverse iterator bug fixing (but now for real)
 *
 *  Revision 1.8  2004/06/21 11:22:19  bdegreve
 *  reverse iterator bug fixing
 *
 */

// --- END OF FILE -------------------------------------------------------------