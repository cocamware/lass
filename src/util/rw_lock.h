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

#ifndef LASS_GUARDIAN_OF_INCLUSION_UTIL_RW_LOCK_H
#define LASS_GUARDIAN_OF_INCLUSION_UTIL_RW_LOCK_H

#include "util_common.h"
#include "atomic.h"

namespace lass
{
namespace util
{

namespace impl
{
} //namespace impl

class RWLock : public NonCopyable
{

public:
	RWLock(util::CallTraits<size_t>::TParam iMaxReaders);
	~RWLock();

	void rlock();
	void wlock();
	void unlock();

	void tryRlock();
	void tryWlock();

private:
	size_t maxReaders_;		/**< the maximum of simultaneous readers allowed */
	int spinLock_;
};


RWLock::RWLock(util::CallTraits<size_t>::TParam iMaxReaders)
{
	maxReaders_ = iMaxReaders;
	spinLock_ = maxReaders_;
}

RWLock::~RWLock()
{
	LASS_ENFORCE(spinLock_==maxReaders_);
}

void RWLock::rlock()
{

}

void RWLock::wlock()
{

}

void RWLock::unlock()
{

}

void RWLock::tryRlock()
{
	
}


void RWLock::tryWlock()
{

}

} //namespace util
} //namespace lass


#endif

// EOF
