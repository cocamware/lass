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

#ifndef LASS_GUARDIAN_OF_INCLUSION_UTIL_CALL_TRAITS_H
#define LASS_GUARDIAN_OF_INCLUSION_UTIL_CALL_TRAITS_H

#include "util_common.h"

namespace lass
{
namespace util
{

template <typename T> 
struct CallTraits
{
	typedef T TValue;
	typedef T const & TParam;
	typedef T const & TConstReference;
	typedef T & TReference;
};

template <typename T> 
struct CallTraits<T const>
{
	typedef T const TValue;
	typedef T const & TParam;
	typedef T const & TConstReference;
	typedef T const & TReference;
};

template <typename T> 
struct CallTraits<T &>
{
	typedef T & TValue;
	typedef T & TParam;
	typedef T const & TConstReference;
	typedef T & TReference;
};

template <typename T> 
struct CallTraits<T const &>
{
	typedef T const & TValue;
	typedef T const & TParam;
	typedef T const & TConstReference;
	typedef T const & TReference;
};

template <typename T> 
struct CallTraits<T *>
{
	typedef T * TValue;
	typedef T * TParam;
	typedef T * const & TConstReference;
	typedef T * & TReference;
};

template <typename T> 
struct CallTraits<T const *>
{
	typedef T const * TValue;
	typedef T const * TParam;
	typedef T const * const & TConstReference;
	typedef T const * & TReference;
};

template <typename T, int N> 
struct CallTraits<T [N]>
{
	typedef T * TValue;
	typedef T * TParam;
	typedef T * const & TConstReference;
	typedef T * & TReference;
};

template <typename T, int N> 
struct CallTraits<T const [N]>
{
	typedef T const * TValue;
	typedef T const * TParam;
	typedef T const * const & TConstReference;
	typedef T const * & TReference;
};

#define LASS_UTIL_CALL_TRAITS_FUNDAMENTAL(T)\
	template <> struct CallTraits<T>\
	{\
		typedef T TValue;\
		typedef T TParam;\
		typedef T const & TConstReference;\
		typedef T & TReference;\
	};\
	template <> struct CallTraits<T const>\
	{\
		typedef T const TValue;\
		typedef T const TParam;\
		typedef T const & TConstReference;\
		typedef T const & TReference;\
	};\
	/**/

LASS_UTIL_CALL_TRAITS_FUNDAMENTAL(char)
LASS_UTIL_CALL_TRAITS_FUNDAMENTAL(unsigned char)
LASS_UTIL_CALL_TRAITS_FUNDAMENTAL(signed char)
LASS_UTIL_CALL_TRAITS_FUNDAMENTAL(unsigned short)
LASS_UTIL_CALL_TRAITS_FUNDAMENTAL(signed short)
LASS_UTIL_CALL_TRAITS_FUNDAMENTAL(unsigned int)
LASS_UTIL_CALL_TRAITS_FUNDAMENTAL(signed int)
LASS_UTIL_CALL_TRAITS_FUNDAMENTAL(unsigned long)
LASS_UTIL_CALL_TRAITS_FUNDAMENTAL(signed long)
LASS_UTIL_CALL_TRAITS_FUNDAMENTAL(float)
LASS_UTIL_CALL_TRAITS_FUNDAMENTAL(double)
LASS_UTIL_CALL_TRAITS_FUNDAMENTAL(long double)
LASS_UTIL_CALL_TRAITS_FUNDAMENTAL(bool)

}
}

#endif

// EOF
