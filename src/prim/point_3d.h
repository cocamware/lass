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



/** @struct lass::prim::Point3D
 *	@brief 3D Point
 *	@author BdG
 *	@date 2003
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
class Point3D
{
public:

    typedef Point3D<T> TSelf;

	typedef Vector3D<T> TVector;

	typedef typename TVector::TValue TValue;
	typedef typename TVector::TParam TParam;
	typedef typename TVector::TReference TReference;
	typedef typename TVector::TConstReference TConstReference;
	typedef typename TVector::TNumTraits TNumTraits;

	enum { dimension = TVector::dimension };	/**< number of dimensions */

    template <typename U> struct Rebind
    {
        typedef Point3D<U> Type;
    };

	Point3D();
	Point3D(TParam iX, TParam iY, TParam iZ);

	template <typename U> explicit Point3D(const Point3D<U>& iOther):
		x(iOther.x), 
		y(iOther.y),
		z(iOther.z)
	{
	}

	template <typename U> explicit Point3D(const Vector3D<U>& iPositionVector):
		position_(iPositionVector)
	{
	}

	const TVector& position() const;
	TConstReference operator[](unsigned iIndex) const;
	TConstReference at(signed iIndex) const;
	TConstReference x() const;
	TConstReference y() const;
	TConstReference z() const;

	TVector& position();
	TReference operator[](unsigned iIndex);
	TReference at(signed iIndex);
	TReference x();
	TReference y();
	TReference z();

	Point3D<T>& operator+=(const Vector3D<T>& iOffset);
	Point3D<T>& operator-=(const Vector3D<T>& iOffset);

	const bool isZero() const;

private:

	TVector position_;
};

template<typename T> bool operator==(const Point3D<T>& iA, const Point3D<T>& iB);
template<typename T> bool operator!=(const Point3D<T>& iA, const Point3D<T>& iB);

template<typename T> Point3D<T> operator+(const Point3D<T>& iA, const Vector3D<T>& iB);
template<typename T> Point3D<T> operator+(const Vector3D<T>& iA, const Point3D<T>& iB);
template<typename T> Point3D<T> operator-(const Point3D<T>& iA, const Vector3D<T>& iB);
template<typename T> Vector3D<T> operator-(const Point3D<T>& iA, const Point3D<T>& iB);

template<typename T> typename Point3D<T>::TValue distance(const Point3D<T>& iA, const Point3D<T>& iB);

template<typename T> Point3D<T> pointwiseMin(const Point3D<T>& iA, const Point3D<T>& iB);
template<typename T> Point3D<T> pointwiseMax(const Point3D<T>& iA, const Point3D<T>& iB);

template<typename T> std::ostream& operator<<(std::ostream& oOStream, const Point3D<T>& iB);
template<typename T> io::XmlOStream& operator<<(io::XmlOStream& oOStream, const Point3D<T>& iB);
template<typename T> std::istream& operator>>(std::istream& ioIStream, Point3D<T>& oB);



}

}

#include "point_3d.inl"

#define LASS_PRIM_PYOBJECT_UTIL_POINT_3D
#ifdef LASS_GUARDIAN_OF_INCLUSION_PRIM_PYOBJECT_UTIL_H
#	include "pyobject_util.h"
#endif

#endif
