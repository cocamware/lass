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


 *  @date 2004
 */

#ifndef LASS_GUARDIAN_OF_INCLUSION_NUM_RANDOM_INL
#define LASS_GUARDIAN_OF_INCLUSION_NUM_RANDOM_INL

#include "num_common.h"
#include "random.h"

namespace lass
{
namespace num
{

// --- RandomStandard ------------------------------------------------------------------------------

inline RandomStandard::TValue RandomStandard::operator ()() const
{
    return rand();
}



// --- RandomMT19937 -------------------------------------------------------------------------------

/** construct by a range of initializing keys
 *  Requirements:
 *      @arg ForwardIterator is a forward iterator to elements of type TValue.  It must really be a 
 *           forward iterator and not just an input iterator, since it must be possible to iterator
 *           over the same range more than once.
 */
template <typename ForwardIterator>
RandomMT19937::RandomMT19937(ForwardIterator iBegin, ForwardIterator iEnd)
{
    seed(iBegin, iEnd);
}



/** initialize by a range of initializing keys 
 *  Requirements:
 *      @arg ForwardIterator is a forward iterator to elements of type TValue.  It must really be a 
 *           forward iterator and not just an input iterator, since it must be possible to iterator
 *           over the same range more than once.
 */
template <typename ForwardIterator>
void RandomMT19937::seed(ForwardIterator iBegin, ForwardIterator iEnd)
{
    seed(19650218UL);

    const int keySize = std::distance(iBegin, iEnd);
    int i = 1;
    int j = 0;
    ForwardIterator key = iBegin;
    for (int k = (stateSize_ > keySize ? stateSize_ : keySize); k > 0; --k)
    {
        state_[i] = (state_[i] ^ ((state_[i - 1] ^ (state_[i - 1] >> 30)) * 1664525UL)) + *key + j;
        state_[i] &= wordMask_;

        ++i;
        if (i >= stateSize_)
        {
            state_[0] = state_[stateSize_ - 1];
            i = 1;
        }

        ++key;
        ++j;
        if (key == iEnd)
        {
            key = iBegin;
            j = 0;
        }
        LASS_ASSERT(j < keySize);
    }
    for (int k = stateSize_ - 1; k > 0; --k)
    {
        state_[i] = (state_[i] ^ ((state_[i - 1] ^ (state_[i - 1] >> 30)) * 1566083941UL)) - i;
        state_[i] &= wordMask_;
        ++i;
        if (i >= stateSize_)
        {
            state_[0] = state_[stateSize_ - 1];
            i = 1;
        }
    }

    state_[0] = upperMask_;
}



inline RandomMT19937::TValue RandomMT19937::operator()()
{
    if (index_ >= stateSize_)
    {
        reload();
    }

    TValue y = state_[index_++];

    // Tempering
    //
    y ^= (y >> 11);
    y ^= (y << 7) & 0x9d2c5680UL;
    y ^= (y << 15) & 0xefc60000UL;
    y ^= (y >> 18);

    return y;
}


       


}

}

#endif

// EOF
