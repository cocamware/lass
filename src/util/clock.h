/**	@file
 *	@author Bram de Greve (bramz@users.sourceforge.net)
 *	@author Tom De Muer (tomdemuer@users.sourceforge.net)
 *
 *	Distributed under the terms of the GPL (GNU Public License)
 *
 * 	The LASS License:
 *
 *	Copyright 2004 Bram de Greve and Tom De Muer
 *
 *	LASS is free software; you can redistribute it and/or modify
 *	it under the terms of the GNU General Public License as published by
 *	the Free Software Foundation; either version 2 of the License, or
 *	(at your option) any later version.
 *
 *	This program is distributed in the hope that it will be useful,
 *	but WITHOUT ANY WARRANTY; without even the implied warranty of
 *	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *	GNU General Public License for more details.
 *
 *	You should have received a copy of the GNU General Public License
 *	along with this program; if not, write to the Free Software
 *	Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */



#ifndef LASS_GUARDIAN_OF_INCLUSION_UTIL_CLOCK_H
#define LASS_GUARDIAN_OF_INCLUSION_UTIL_CLOCK_H

#include "util_common.h"
#include "impl/clock_impl.h"
#include "../num/num_traits.h"

namespace lass
{
namespace util
{

class Clock
{
public:

    typedef double TTime;
    typedef impl::ClockImpl::TTick TTick;

    explicit Clock(TTime iStartTime = 0) 
    { 
        frequency_ = impl::ClockImpl::frequency();
        resolution_ = num::NumTraits<TTime>::one / frequency_;
        reset(iStartTime);
    }

    void reset(TTime iStartTime = 0) 
    { 
        startTick_ = impl::ClockImpl::tick(); 
        startTime_ = iStartTime; 
    }

    const TTime time() const { return startTime_ + resolution_ * this->tick(); }
    const TTick tick() const { return impl::ClockImpl::tick() - startTick_; }

    const TTick frequency() const { return frequency_; }
    const TTime resolution() const { return resolution_; }

private:

    TTime startTime_;
    TTime resolution_;
    TTick startTick_;
    TTick frequency_;
};

}

}

#endif

// EOF
