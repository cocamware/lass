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



/** @class lass::prim::Transformation2D
 *  @brief a linear 2D transformation
 *  @author Bram de Greve [BdG]
 */

#ifndef LASS_GUARDIAN_OF_INCLUSION_PRIM_TRANSFORMATION_2D_H
#define LASS_GUARDIAN_OF_INCLUSION_PRIM_TRANSFORMATION_2D_H

#include "prim_common.h"
#include "xy.h"
#include "../num/num_traits.h"
#include "../util/shared_ptr.h"

namespace lass
{
namespace prim
{

template <typename T>
class Transformation2D
{
public:

	typedef Transformation2D<T> TSelf;

	typedef typename util::CallTraits<T>::TValue TValue;
	typedef typename util::CallTraits<T>::TParam TParam;
	typedef typename util::CallTraits<T>::TReference TReference;
	typedef typename util::CallTraits<T>::TConstReference TConstReference;
	typedef num::NumTraits<T> TNumTraits;
	typedef size_t TSize;

	enum { dimension = 3 }; /**< number of dimensions of vector */

	template <typename U> struct Rebind
	{
		typedef Transformation2D<U> Type;
	};

	Transformation2D();
	template <typename InputIterator> Transformation2D(InputIterator iBegin, InputIterator iEnd);

	const Transformation2D<T> inverse() const;

	const TValue* matrix() const;

	static const Transformation2D<T> translation(const Vector2D<T>& iOffset);
	static const Transformation2D<T> scaler(const T& iScale);
	static const Transformation2D<T> scaler(const Vector2D<T>& iScale);
	static const Transformation2D<T> rotation(TParam iRadians);

private:

	enum { matrixSize_ = 9 };


	typedef util::SharedPtr<TValue, util::ArrayStorage> TMatrix;

	Transformation2D(const TMatrix& iMatrix, const TMatrix& iInverseMatrix, bool iDummy);

	TMatrix matrix_;
	mutable TMatrix inverseMatrix_;
};

template <typename T> Transformation2D<T> concatenate(const Transformation2D<T>& iA, const Transformation2D<T>& iB);

template <typename T> Vector2D<T> transform(const Vector2D<T>& iSubject, const Transformation2D<T>& iTransformation);
template <typename T> Point3D<T> transform(const Point3D<T>& iSubject, const Transformation2D<T>& iTransformation);
template <typename T> Vector2D<T> normalTransform(const Vector2D<T>& iSubject, const Transformation2D<T>& iTransformation);
template <typename T> std::pair<Vector2D<T>, T> normalTransform(const std::pair<Vector2D<T>, T>& iSubject, const Transformation2D<T>& iTransformation);

template<typename T, typename Char, typename Traits>
std::basic_ostream<Char, Traits>& operator<<(std::basic_ostream<Char, Traits>& oOStream, const Transformation2D<T>& iB);

template<typename T> io::XmlOStream& operator<<(io::XmlOStream& oOStream, const Transformation2D<T>& iB);

}

}

#include "transformation_2d.inl"

#define LASS_PRIM_PYOBJECT_UTIL_TRANSFORMATION_2D
#ifdef LASS_GUARDIAN_OF_INCLUSION_PRIM_PYOBJECT_UTIL_H
#	include "pyobject_util.h"
#endif

#ifdef LASS_GUARDIAN_OF_INCLUSION_PRIM_AABB_2D_H
#	include "aabb_3d_transformation_3d.h"
#endif

#endif

// EOF
