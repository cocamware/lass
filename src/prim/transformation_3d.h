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



/** @class lass::prim::Transformation3D
 *  @brief a linear 3D transformation
 *  @author Bram de Greve [BdG]
 */

#ifndef LASS_GUARDIAN_OF_INCLUSION_PRIM_TRANSFORMATION_3D_H
#define LASS_GUARDIAN_OF_INCLUSION_PRIM_TRANSFORMATION_3D_H

#include "prim_common.h"
#include "../num/num_traits.h"

namespace lass
{
namespace prim
{

template <typename T>
class Transformation3D
{
public:

    typedef Transformation3D<T> TSelf;

	typedef typename util::CallTraits<T>::TValue TValue;
	typedef typename util::CallTraits<T>::TParam TParam;
	typedef typename util::CallTraits<T>::TReference TReference;
	typedef typename util::CallTraits<T>::TConstReference TConstReference;
	typedef num::NumTraits<T> TNumTraits;
    typedef size_t TSize;

	enum { dimension = 3 };	/**< number of dimensions of vector */

	template <typename U> struct Rebind
	{
		typedef Transformation3D<U> Type;
	};

    Transformation3D();

    TConstReference operator()(TSize iRow, TSize iCol) const;
    TReference operator()(TSize iRow, TSize iCol);
    TConstReference at(TSize iRow, TSize iCol) const;
    TReference at(TSize iRow, TSize iCol);

    Transformation3D<T>& operator*=(const Transformation3D<T>& iOther);

    const TValue* data() const;
    TValue* data();

private:

    Transformation3D<T>(bool iDontInitialise);

    TValue v_[16];
};

template <typename T> Transformation3D<T> operator*(const Transformation3D<T>& iA, const Transformation3D<T>& iB);
template <typename T> Vector3D<T> operator*(const Transformation3D<T>& iA, const Vector3D<T>& iB);
template <typename T> Point3D<T> operator*(const Transformation3D<T>& iA, const Point3D<T>& iB);

template<typename T, typename Char, typename Traits> 
std::basic_ostream<Char, Traits>& operator<<(
    std::basic_ostream<Char, Traits>& ioOStream, const Vector3D<T>& iB);

template<typename T> io::XmlOStream& operator<<(io::XmlOStream& ioOStream, const Vector3D<T>& iB);

}

}

#include "transformation_3d.inl"

#define LASS_PRIM_PYOBJECT_UTIL_TRANSFORMATION_3D
#ifdef LASS_GUARDIAN_OF_INCLUSION_PRIM_PYOBJECT_UTIL_H
#	include "pyobject_util.h"
#endif

#endif

// EOF
