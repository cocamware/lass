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



#ifndef LASS_GUARDIAN_OF_INCLUSION_UTIL_CALL_TRAITS_INL
#define LASS_GUARDIAN_OF_INCLUSION_UTIL_CALL_TRAITS_INL

#include "call_traits.h"

// we only have to specialize if we're not boosting.
//
#ifndef LASS_UTIL_CALL_TRAITS_BOOST

namespace lass
{

namespace util
{

// specialize for built-in types.

#define LASS_UTIL_CALL_TRAITS_BUILT_IN( type )\
template<>\
struct CallTraits<type>\
{\
	typedef type TValue;\
	typedef type& TReference;\
	typedef const type& TConstReference;\
	typedef type TParam;\
}

LASS_UTIL_CALL_TRAITS_BUILT_IN(char);
LASS_UTIL_CALL_TRAITS_BUILT_IN(signed char);
LASS_UTIL_CALL_TRAITS_BUILT_IN(unsigned char);
LASS_UTIL_CALL_TRAITS_BUILT_IN(signed short);
LASS_UTIL_CALL_TRAITS_BUILT_IN(unsigned short);
LASS_UTIL_CALL_TRAITS_BUILT_IN(signed int);
LASS_UTIL_CALL_TRAITS_BUILT_IN(unsigned int);
LASS_UTIL_CALL_TRAITS_BUILT_IN(signed long);
LASS_UTIL_CALL_TRAITS_BUILT_IN(unsigned long);
LASS_UTIL_CALL_TRAITS_BUILT_IN(float);
LASS_UTIL_CALL_TRAITS_BUILT_IN(double);
LASS_UTIL_CALL_TRAITS_BUILT_IN(long double);
LASS_UTIL_CALL_TRAITS_BUILT_IN(bool);


// here we enter the partial specialization realm.
// If you can't do it, and you don't want to boost, this is the end of the line for you.  Goodbye.
// (PS. if you boost, you wouldn't be here anyway, check out call_traits.h :)
#ifndef LASS_NO_TEMPLATE_PARTIAL_SPECIALIZATION



//  specialize for pointer types

template<typename T>
struct CallTraits<T*>
{
	typedef T* TValue;
	typedef T*& TReference;
	typedef T* const& TConstReference;
	typedef T* const TParam;
};



// specialize for reference types

template<typename T>
struct CallTraits<T&>
{
	typedef T& TValue;
	typedef T& TReference;
	typedef const T& TConstReference;
	typedef T& TParam;
};



// specialize for constant reference types

template<typename T>
struct CallTraits<const T&>
{
	typedef const T& TValue;
	typedef const T& TReference;
	typedef const T& TConstReference;
	typedef const T& TParam;
};



// specialize for array types

template <typename T, unsigned N>
struct CallTraits<T [N]>
{
private:

   typedef T TArray[N];

public:

   typedef const T* TValue; // degrades array to pointer
   typedef TArray& TReference;
   typedef const TArray& TConstReference;
   typedef const T* const TParam;
};



// specialize for const array types

template <typename T, unsigned N>
struct CallTraits<const T [N]>
{
private:

   typedef const T TArray[N];

public:

   typedef const T* TValue; // degrades array to pointer
   typedef TArray& TReference;
   typedef const TArray& TConstReference;
   typedef const T* const TParam;
};

#endif // !LASS_NO_TEMPLATE_PARTIAL_SPECIALIZATION

}

}

#endif // !LASS_UTIL_CALL_TRAITS_BOOST

#endif
