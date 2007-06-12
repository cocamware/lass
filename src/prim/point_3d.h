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



/** @struct lass::prim::Point3D
 *  @brief 3D Point
 *  @author BdG
 *  @date 2003
 */



#ifndef LASS_GUARDIAN_OF_INCLUSION_PRIM_POINT_3D_H
#define LASS_GUARDIAN_OF_INCLUSION_PRIM_POINT_3D_H




#include "prim_common.h"
#include "vector_3d.h"


namespace lass
{

namespace prim
{

template<typename T>
struct Point3D
{
public:

	typedef Point3D<T> TSelf;

	typedef Vector3D<T> TVector;

	typedef typename TVector::TValue TValue;
	typedef typename TVector::TParam TParam;
	typedef typename TVector::TReference TReference;
	typedef typename TVector::TConstReference TConstReference;
	typedef typename TVector::TNumTraits TNumTraits;

	enum { dimension = TVector::dimension };    /**< number of dimensions */

	template <typename U> struct Rebind
	{
		typedef Point3D<U> Type;
	};

	TValue x;
	TValue y;
	TValue z;

	Point3D();
	Point3D(TParam x, TParam y, TParam z);
	template <typename U> explicit Point3D(const Point3D<U>& other);
	template <typename U> explicit Point3D(const Vector3D<U>& position);
	template <typename U> explicit Point3D(const U& x, const U& y, const U& z);

	const TVector position() const;
	TConstReference operator[](size_t index) const;
	TReference operator[](size_t index);
	TConstReference at(signed index) const;
	TReference at(signed index);

	Point3D<T>& operator+=(const Vector3D<T>& offset);
	Point3D<T>& operator-=(const Vector3D<T>& offset);

	const bool isZero() const;
	const bool isNaN() const;
};

template<typename T> bool operator==(const Point3D<T>& a, const Point3D<T>& b);
template<typename T> bool operator!=(const Point3D<T>& a, const Point3D<T>& b);

template<typename T> Point3D<T> operator+(const Point3D<T>& a, const Vector3D<T>& b);
template<typename T> Point3D<T> operator+(const Vector3D<T>& a, const Point3D<T>& b);
template<typename T> Point3D<T> operator-(const Point3D<T>& a, const Vector3D<T>& b);
template<typename T> Vector3D<T> operator-(const Point3D<T>& a, const Point3D<T>& b);

template<typename T> typename Point3D<T>::TValue distance(const Point3D<T>& a, const Point3D<T>& b);
template<typename T> typename Point3D<T>::TValue squaredDistance(const Point3D<T>& a, const Point3D<T>& b);
template<typename T> Point3D<T> pointwiseMin(const Point3D<T>& a, const Point3D<T>& b);
template<typename T> Point3D<T> pointwiseMax(const Point3D<T>& a, const Point3D<T>& b);
template<typename T> Point3D<T> lerp(const Point3D<T>& a, const Point3D<T>& b, typename Point3D<T>::TParam t);

template<typename T> std::ostream& operator<<(std::ostream& stream, const Point3D<T>& b);
template<typename T> io::XmlOStream& operator<<(io::XmlOStream& stream, const Point3D<T>& b);
template<typename T> std::istream& operator>>(std::istream& stream, Point3D<T>& b);



}

}

#include "point_3d.inl"
#include "pyobject_util.inl"

#endif
