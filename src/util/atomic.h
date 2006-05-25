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
#include "../num/basic_types.h"

namespace lass
{
namespace util
{

namespace impl
{

template <int byteSize> struct AtomicOperations;

#if LASS_COMPILER_TYPE == LASS_COMPILER_TYPE_MSVC || LASS_COMPILER_TYPE == LASS_COMPILER_TYPE_INTEL

#pragma warning(push)
#pragma warning(disable: 4035)

template <>
struct AtomicOperations<1>
{
	template <typename T> inline 
	static T LASS_CALL compareAndSwap(T& oDest, T iExpectedValue, T iNewValue)
	{
		T* const dest = &oDest;
		__asm 
		{
			mov cl, iNewValue
			mov al, iExpectedValue
			mov edx, dest
			lock cmpxchg [edx], cl
		}
		/* return eax */
	}
};

template <>
struct AtomicOperations<2>
{
	template <typename T> inline 
	static T LASS_CALL compareAndSwap(T& oDest, T iExpectedValue, T iNewValue)
	{
		T* const dest = &oDest;
		__asm 
		{
			mov cx, iNewValue
			mov ax, iExpectedValue
			mov edx, dest
			lock cmpxchg [edx], cx
		}
		/* return eax */
	}
};

template <>
struct AtomicOperations<4>
{
	template <typename T> inline 
	static T LASS_CALL compareAndSwap(T& oDest, T iExpectedValue, T iNewValue)
	{
		T* const dest = &oDest;
		__asm 
		{
			mov ecx, iNewValue
			mov eax, iExpectedValue
			mov edx, dest
			lock cmpxchg [edx], ecx
		}
		/* return eax */
	}

	template <typename T> inline
	static void LASS_CALL increment(T& ioValue)
	{
		T* const value = &ioValue;
		__asm 
		{
			mov edx, value
			lock inc [edx]
		}
	}

	template <typename T> inline
	static void LASS_CALL decrement(T& ioValue)
	{
		T* const value = &ioValue;
		__asm 
		{
			mov edx, value
			lock dec [edx]
		}
	}
};

}

#pragma warning(pop)

#endif

template <typename T> inline 
bool atomicCompareAndSwap(T& oDest, T iExpectedValue, T iNewValue)
{
	return impl::AtomicOperations< sizeof T >::compareAndSwap(oDest, iExpectedValue, iNewValue) 
		== iExpectedValue;
}

template <typename T> inline
void atomicIncrement(T& ioValue)
{
	impl::AtomicOperations< sizeof T >::increment(ioValue);
}

template <typename T> inline
void atomicDecrement(T& ioValue)
{
	impl::AtomicOperations< sizeof T >::decrement(ioValue);
}
}

}

#endif

// EOF
