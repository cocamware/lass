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



/** @struct lass::prim::Point_2d
 *  @brief 2D Point
 *  @author BdG
 *  @date 2003
 */



#ifndef LASS_GUARDIAN_OF_INCLUSION_PRIM_POINT_2D_H
#define LASS_GUARDIAN_OF_INCLUSION_PRIM_POINT_2D_H




#include "prim_common.h"
#include "vector_2d.h"
#include "../io/matlab_o_stream.h"

namespace lass
{
namespace prim
{

template<typename T>
struct Point2D
{
public:

	typedef Point2D<T> TSelf;

	typedef Vector2D<T> TVector;

	typedef typename TVector::TValue TValue;
	typedef typename TVector::TParam TParam;
	typedef typename TVector::TReference TReference;
	typedef typename TVector::TConstReference TConstReference;
	typedef typename TVector::TNumTraits TNumTraits;

	enum { dimension = TVector::dimension };    /**< number of dimensions of vector */

	template <typename U> struct Rebind
	{
		typedef Point2D<U> Type;
	};

	TValue x;
	TValue y;

	Point2D();
	Point2D(TParam iX, TParam iY);
	template <typename U> explicit Point2D(const Point2D<U>& iOther);
	template <typename U> explicit Point2D(const Vector2D<U>& iPositionVector);
	template <typename U> explicit Point2D(const U& iX, const U& iY);

	const TVector position() const;
	TConstReference operator[](size_t iIndex) const;
	TReference operator[](size_t iIndex);
	TConstReference at(signed iIndex) const;
	TReference at(signed iIndex);

	Point2D<T>& operator+=(const Vector2D<T>& iOffset);
	Point2D<T>& operator-=(const Vector2D<T>& iOffset);

	const bool isZero() const;
	const bool isNaN() const;
};

template<typename T> bool operator==(const Point2D<T>& iA, const Point2D<T>& iB);
template<typename T> bool operator!=(const Point2D<T>& iA, const Point2D<T>& iB);

template<typename T> Point2D<T> operator+(const Point2D<T>& iA, const Vector2D<T>& iB);
template<typename T> Point2D<T> operator+(const Vector2D<T>& iA, const Point2D<T>& iB);
template<typename T> Point2D<T> operator-(const Point2D<T>& iA, const Vector2D<T>& iB);
template<typename T> Vector2D<T> operator-(const Point2D<T>& iA, const Point2D<T>& iB);

template<typename T> typename Point2D<T>::TValue distance(const Point2D<T>& iA, const Point2D<T>& iB);
template<typename T> typename Point2D<T>::TValue squaredDistance(const Point2D<T>& iA, const Point2D<T>& iB);
template<typename T> Point2D<T> pointwiseMin(const Point2D<T>& iA, const Point2D<T>& iB);
template<typename T> Point2D<T> pointwiseMax(const Point2D<T>& iA, const Point2D<T>& iB);

template<typename T> std::ostream& operator<<(std::ostream& oOStream, const Point2D<T>& iB);
template<typename T> io::XmlOStream& operator<<(io::XmlOStream& oOStream, const Point2D<T>& iB);
template<typename T> std::istream& operator>>(std::istream& ioIStream, Point2D<T>& oB);
template<typename T> lass::io::MatlabOStream& operator<<(lass::io::MatlabOStream& oOStream, const Point2D<T>& iB);


template<typename T> T doubleTriangleArea( const Point2D<T>& iA, const Point2D<T>& iB, const Point2D<T>& iC );
template<typename T> bool cw( const Point2D<T>& iA, const Point2D<T>& iB, const Point2D<T>& iC );
template<typename T> bool ccw( const Point2D<T>& iA, const Point2D<T>& iB, const Point2D<T>& iC );
template<typename T> bool weakCw( const Point2D<T>& iA, const Point2D<T>& iB, const Point2D<T>& iC );
template<typename T> bool weakCcw( const Point2D<T>& iA, const Point2D<T>& iB, const Point2D<T>& iC );
template<typename T> bool inCircle( const Point2D<T>& iA, const Point2D<T>& iB, const Point2D<T>& iC, const Point2D<T>& iD );

}

}

#include "point_2d.inl"
#include "pyobject_util.inl"

#endif
