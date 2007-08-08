
#ifndef LASS_GUARDIAN_OF_INCLUSION_PRIM_IMPL_ALLOCATOR_H
#define LASS_GUARDIAN_OF_INCLUSION_PRIM_IMPL_ALLOCATOR_H

#include "../prim_common.h"
#include "../../meta/is_integral.h"
#include "../../meta/is_trivial.h"

namespace lass
{
namespace prim
{
namespace impl
{

LASS_DLL void* allocateMemory(size_t size);
LASS_DLL void deallocateMemory(void* mem, size_t size);

template <typename T, bool hasTrivialConstructorAndDestructor>
class AllocatorHelper
{
public:
	static T* allocate(size_t n)
	{
		return static_cast<T*>(allocateMemory(n * sizeof(T)));
	}
	static void deallocate(T* p, size_t n)
	{
		deallocateMemory(p, n * sizeof(T));
	}
};

template <typename T>
class AllocatorHelper<T, false>
{
public:
	static T* allocate(size_t n)
	{
		// Kids, don't try this at home, we're trained professionals here! ;)
		//
		T* p = static_cast<T*>(allocateMemory(n * sizeof(T)));
		for (size_t i = 0; i < n; ++i)
		{
			try
			{
				new(&p[i]) T();
			}
			catch (...)
			{
				deallocate(p, n, i);
				throw;
			}
		}
		return p;
	}
	static void deallocate(T* p, size_t n)
	{
		deallocate(p, n, n);
	}
private:
	static void deallocate(T* p, size_t n, size_t i)
	{
		while (i > 0)
		{
			p[--i].~T();
		}
		deallocateMemory(p, n * sizeof(T));
	}
};

template <typename T>
T* allocateArray(size_t n)
{
	return AllocatorHelper<T, meta::IsTrivial<T>::value>::allocate(n);
}

template <typename T>
void deallocateArray(T* p, size_t n)
{
	AllocatorHelper<T, meta::IsTrivial<T>::value>::deallocate(p, n);
}


}
}
}

#endif

// EOF
