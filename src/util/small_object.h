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
 *
 *  @par original code by Andrei Alexandrescu:
 *  The Loki Library, Copyright (c) 2001 by Andrei Alexandrescu\n
 *  This code (Loki) accompanies the book:\n
 *  Alexandrescu, Andrei. "Modern C++ Design: Generic Programming and Design Patterns Applied".
 *  Copyright (c) 2001. Addison-Wesley.\n
 *  <i>Permission to use, copy, modify, distribute and sell this software (Loki) for any purpose is
 *  hereby granted without fee, provided that the above copyright notice appear in all copies and
 *  that both that copyright notice and this permission notice appear in supporting documentation.\n
 *  The author or Addison-Wesley Longman make no representations about the suitability of this
 *  software (Loki) for any purpose. It is provided "as is" without express or implied warranty.</i>
 */

/** @class lass::util::SmallObject
 *  @brief base class for small objects that need a specialized allocator.
 *  @author Bram de Greve [Bramz]
 *
 *  This tool is the Lass implementation of Alexandrescu's SmallObject.  Let's copy some text as
 *  documentation (slightly modified to represent this implementation)
 *
 *  @section Summary
 *
 *  Some C++ idioms make heavy use of small objects allocated on the free store.  This is because
 *  in C++, runtime polymorphism goes hand in hand with dynamic allocation and pointer/reference
 *  semantics.  However, the default free store allocator (accessible thorugh the global
 *  @c ::operator @c new and @c ::operator @c delete) is often optimized for allocating large
 *  objects, not small ones.  This renders the default free store allocator unsuitable for
 *  allocating small objects because it is often slow and brings nonnegligible per-object memory
 *  overhead.
 *
 *  The solution is to rely on small-object allocators - specialized allocators that are tuned for
 *  dealing with small memory blocks (tens to hundreds of bytes).  Small-object allocators use
 *  larger chunks of memory and organize them in ingenious ways to reduce space and time penalties.
 *  The C++ runtime support helps by providing the size of the block to be released.  This
 *  information can be grabbed by simply using a less-known overloaded form of operator @c delete
 *
 *  Is Loki's [and Lass'] small-object allocator the fastest possible?  Definitely not.  Loki's
 *  allocator operates only within the confines of standard C++.  As you saw throughout this
 *  chapter [see the book], issues such as alignment must be treated very conservatively, and being
 *  conservative spells being less than optimal.  However, Loki is a reasonably fast, simple, and
 *  robust allocator that has the advantage of portability.
 *
 *  @section quick_facts Quick Facts
 *
 *  - The Allocator implemented by Loki (and Lass) has a four-layered architecture.  The first layer
 *    consists of a private type lass::util::impl::FixedAllocator::Chunk, which deals with
 *    organizing memory chunks in equally sized blocks.  The second layer is
 *    lass::util::impl::FixedAllocator, which uses a variable-length vector of chunks to statisfy
 *    memory allocation to the extend of the available memory in the system.  In the third layer,
 *    lass::util::impl::SmallObjectAllocator uses multiple @c FixedAllocator objects to provide
 *    allocations of any object size.  Small objects are allocated using a @c FixedAllocator, and
 *    requests for large objects are forwarded to the @c ::operator @c new.  Finally, the fourth
 *    layer consists of lass::util::SmallObject, a base class that wraps a @c SmallObjectAllocator
 *    singleton.
 *  - You can benifit from a small-object allocator by deriving from @c SmallObject.
 *  @code
 *  #include <lass/util/small_object.h>
 *
 *  struct MySmallObject: public lass::util::SmallObject
 *  {
 *      int foo;
 *      bool bar;
 *  };
 *
 *  MySmallObject* a = new MySmallObject;
 *  ...
 *  delete a;
 *  @endcode
 *
 *  @section more_info More info
 *
 *  ALEXANDRESCU A. (2001). <i>"Modern C++ Design: Generic Programming and Design Patterns
 *  Applied".</i> Addison-Wesley. pages 77-95.
 *
 *  @par original code by Andrei Alexandrescu:
 *  The Loki Library, Copyright (c) 2001 by Andrei Alexandrescu\n
 *  This code (Loki) accompanies the book:\n
 *  Alexandrescu, Andrei. "Modern C++ Design: Generic Programming and Design Patterns Applied".
 *  Copyright (c) 2001. Addison-Wesley.\n
 *  <i>Permission to use, copy, modify, distribute and sell this software (Loki) for any purpose is
 *  hereby granted without fee, provided that the above copyright notice appear in all copies and
 *  that both that copyright notice and this permission notice appear in supporting documentation.\n
 *  The author or Addison-Wesley Longman make no representations about the suitability of this
 *  software (Loki) for any purpose. It is provided "as is" without express or implied warranty.</i>
 */

#ifndef LASS_GUARDIAN_OF_INCLUSION_UTIL_SMALL_OBJECT_H
#define LASS_GUARDIAN_OF_INCLUSION_UTIL_SMALL_OBJECT_H

#include "util_common.h"
#include "impl/small_object_impl.h"

// #define LASS_BYPASS_SMALL_OBJECT_ALLOCATOR

namespace lass
{
namespace util
{

template
<
	std::size_t chunkSize = 4096,
	std::size_t maxObjectSize = 64,
	typename AtomType = unsigned char
>
class SmallObject
{
public:

	SmallObject() {}
	virtual ~SmallObject() {}

	static void* operator new(std::size_t iSize)
	{
#ifdef LASS_BYPASS_SMALL_OBJECT_ALLOCATOR
		return ::operator new(iSize);
#else
		return impl::smallObjectAllocator<chunkSize, maxObjectSize, AtomType>()->allocate(iSize);
#endif
	}

	static void* operator new(std::size_t iSize, void* p) throw()
	{
		return ::operator new(iSize, p);
	}

	static void* operator new(std::size_t iSize, const std::nothrow_t&) throw()
	{
		try
		{
			return SmallObject::operator new(iSize);
		}
		catch (...)
		{
			return 0;
		}
	}

	static void operator delete(void* iPointer, std::size_t iSize)
	{
#ifdef LASS_BYPASS_SMALL_OBJECT_ALLOCATOR
		::operator delete(iPointer);
#else
		impl::smallObjectAllocator<chunkSize, maxObjectSize, AtomType>()->deallocate(
			iPointer, iSize);
#endif
	}
};



}

}

#endif

// EOF
