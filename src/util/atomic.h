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

#ifndef LASS_GUARDIAN_OF_INCLUSION_UTIL_ATOMIC_H
#define LASS_GUARDIAN_OF_INCLUSION_UTIL_ATOMIC_H

#include "util_common.h"
#include "impl/atomic_impl.h"

namespace lass
{
namespace util
{

template <typename T> inline 
bool atomicCompareAndSwap(T& dest, T expectedValue, T newValue)
{
	return impl::AtomicOperations< sizeof(T) >::compareAndSwap(dest, expectedValue, newValue) 
		== expectedValue;
}

template <typename T1, typename T2> inline 
bool atomicCompareAndSwap(T1& dest1, T1 expected1, T2 expected2, T1 new1, T2 new2)
{
	LASS_META_ASSERT(sizeof(T1) == sizeof(T2), T1_and_T2_must_be_of_same_size);
	return impl::AtomicOperations< sizeof(T1) >::compareAndSwap(
		dest1, expected1, expected2, new1, new2);
}

template <typename T> inline
void atomicIncrement(T& value)
{
	impl::AtomicOperations< sizeof(T) >::increment(value);
}

template <typename T> inline
void atomicDecrement(T& value)
{
	impl::AtomicOperations< sizeof(T) >::decrement(value);
}

}
}

#endif

// EOF
