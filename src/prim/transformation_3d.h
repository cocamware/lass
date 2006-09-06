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



/** @class lass::prim::Transformation3D
 *  @brief a linear 3D transformation
 *  @author Bram de Greve [BdG]
 */

#ifndef LASS_GUARDIAN_OF_INCLUSION_PRIM_TRANSFORMATION_3D_H
#define LASS_GUARDIAN_OF_INCLUSION_PRIM_TRANSFORMATION_3D_H

#include "prim_common.h"
#include "point_3d.h"
#include "xyz.h"
#include "../io/xml_o_stream.h"
#include "../num/num_traits.h"
#include "../util/shared_ptr.h"
#include "../util/thread.h"

namespace lass
{
namespace prim
{

template <typename T>
class Transformation3D
{
public:

	typedef Transformation3D<T> TSelf;

	typedef Point3D<T> TPoint;
	typedef typename TPoint::TVector TVector;
	typedef typename util::CallTraits<T>::TValue TValue;
	typedef typename util::CallTraits<T>::TParam TParam;
	typedef typename util::CallTraits<T>::TReference TReference;
	typedef typename util::CallTraits<T>::TConstReference TConstReference;
	typedef num::NumTraits<T> TNumTraits;
	typedef size_t TSize;

	enum { dimension = TPoint::dimension }; /**< number of dimensions of vector */

	template <typename U> struct Rebind
	{
		typedef Transformation3D<U> Type;
	};

	Transformation3D();
	Transformation3D(const TPoint& origin, const TVector& baseX, const TVector& baseY, 
		const TVector& baseZ);
	template <typename InputIterator> Transformation3D(InputIterator iBegin, InputIterator iEnd);

	const TSelf inverse() const;

	const TValue* matrix() const;
	void swap(TSelf& ioOther);

	static const TSelf identity();
	static const TSelf translation(const Vector3D<T>& iOffset);
	static const TSelf scaler(const T& iScale);
	static const TSelf scaler(const Vector3D<T>& iScale);
	static const TSelf rotation(XYZ iAxis, TParam iRadians);
	static const TSelf rotation(const Vector3D<T>& iAxis, TParam iRadians);

private:

	enum { matrixSize_ = 16 };

	typedef util::SharedPtr<TValue, util::ArrayStorage> TMatrix;

	Transformation3D(const TMatrix& iMatrix, const TMatrix& iInverseMatrix, bool iDummy);

	TMatrix matrix_;
	mutable TMatrix inverseMatrix_;

	static util::Semaphore sync_;
};

template <typename T> Transformation3D<T> concatenate(const Transformation3D<T>& iA, const Transformation3D<T>& iB);

template <typename T> Vector3D<T> transform(const Vector3D<T>& iSubject, const Transformation3D<T>& iTransformation);
template <typename T> Point3D<T> transform(const Point3D<T>& iSubject, const Transformation3D<T>& iTransformation);
template <typename T> Vector3D<T> normalTransform(const Vector3D<T>& iSubject, const Transformation3D<T>& iTransformation);
template <typename T> std::pair<Vector3D<T>, T> normalTransform(const std::pair<Vector3D<T>, T>& iSubject, const Transformation3D<T>& iTransformation);

template<typename T, typename Char, typename Traits>
std::basic_ostream<Char, Traits>& operator<<(std::basic_ostream<Char, Traits>& oOStream, const Transformation3D<T>& iB);

template<typename T> io::XmlOStream& operator<<(io::XmlOStream& oOStream, const Transformation3D<T>& iB);

// static member initialisation

template <typename T> util::Semaphore Transformation3D<T>::sync_;

}

}

#include "transformation_3d.inl"
#include "pyobject_util.inl"

#ifdef LASS_GUARDIAN_OF_INCLUSION_PRIM_AABB_3D_H
#	include "aabb_3d_transformation_3d.h"
#endif

#ifdef LASS_GUARDIAN_OF_INCLUSION_PRIM_PLANE_3D_H
#	include "plane_3d_transformation_3d.h"
#endif

#ifdef LASS_GUARDIAN_OF_INCLUSION_PRIM_RAY_3D_H
#	include "ray_3d_transformation_3d.h"
#endif

#endif

// EOF
