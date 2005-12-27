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



/** @struct lass::prim::Point3DH
 *  @brief homogenous 3D Point
 *  @author BdG
 *  @date 2003
 */



#ifndef LASS_GUARDIAN_OF_INCLUSION_PRIM_POINT_3DH_H
#define LASS_GUARDIAN_OF_INCLUSION_PRIM_POINT_3DH_H




#include "prim_common.h"
#include "point_3d.h"
#include "vector_4d.h"


namespace lass
{

namespace prim
{

template<typename T>
struct Point3DH
{
public:

	typedef Point3DH<T> TSelf;

	typedef Point3D<T> TPoint;
	typedef Vector4D<T> TVector;

	typedef typename TVector::TValue TValue;
	typedef typename TVector::TParam TParam;
	typedef typename TVector::TReference TReference;
	typedef typename TVector::TConstReference TConstReference;
	typedef typename TVector::TNumTraits TNumTraits;

	enum { dimension = TPoint::dimension }; /**< number of dimensions */

	template <typename U> struct Rebind
	{
		typedef Point3DH<U> Type;
	};

	TValue x;
	TValue y;
	TValue z;
	TValue w;

	Point3DH();
	Point3DH(TParam iX, TParam iY, TParam iZ, TParam iW = TNumTraits::one);
	Point3DH(const TPoint& iAffinePoint);
	explicit Point3DH(const TVector& iPositionVector);

	const TVector position() const;
	TConstReference operator[](size_t iIndex) const;
	TReference operator[](size_t iIndex);
	TConstReference at(signed iIndex) const;
	TReference at(signed iIndex);

	const Point3DH<T>& operator+() const;
	const Point3DH<T> operator-() const;

	Point3DH<T>& operator+=(const Point3DH<T>& iB);
	Point3DH<T>& operator-=(const Point3DH<T>& iB);
	Point3DH<T>& operator*=(TParam iB);
	Point3DH<T>& operator/=(TParam iB);

	const bool isZero() const;
	const bool isNaN() const;
	const bool isInfinite() const;
	const bool isValid() const;
	const TParam weight() const;
	const TPoint affine() const;

	void homogenize();
};

template<typename T> bool operator==(const Point3DH<T>& iA, const Point3DH<T>& iB);
template<typename T> bool operator!=(const Point3DH<T>& iA, const Point3DH<T>& iB);

template<typename T> Point3DH<T> operator+(const Point3DH<T>& iA, const Point3DH<T>& iB);
template<typename T> Point3DH<T> operator-(const Point3DH<T>& iA, Point3DH<T>& iB);

template<typename T> Point3DH<T> operator*(const Point3DH<T>& iA, typename Point3DH<T>::TParam iB);
template<typename T> Point3DH<T> operator/(const Point3DH<T>& iA, typename Point3DH<T>::TParam iB);
template<typename T> Point3DH<T> operator*(typename Point3DH<T>::TParam iA, const Point3DH<T>& iB);

template<typename T> std::ostream& operator<<(std::ostream& oOStream, const Point3DH<T>& iB);
template<typename T> io::XmlOStream& operator<<(io::XmlOStream& oOStream, const Point3DH<T>& iB);
template<typename T> std::istream& operator>>(std::istream& ioIStream, Point3DH<T>& oB);


// redirectors (stuff because template instantiation doesn't understand it can convert
// regular points to homogenous points:
template<typename T> inline Point3DH<T> operator+(const Point3D<T>& iA, const Point3D<T>& iB)
{
	return Point3DH<T>(iA) + Point3DH<T>(iB);
}
template<typename T> inline Point3DH<T> operator*(typename Point3D<T>::TParam iA, const Point3D<T>& iB)
{
	return iA * Point3DH<T>(iB);
}
template<typename T> inline Point3DH<T> operator*(const Point3D<T>& iA, typename Point3D<T>::TParam iB)
{
	return Point3DH<T>(iA) * iB;
}



}

}

#include "point_3dh.inl"

#endif

// --- END OF FILE ------------------------------------------------------------------------------
