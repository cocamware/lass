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



/** @struct lass::prim::Point_2d
 *	@brief 2D Point
 *	@author BdG
 *	@date 2003
 */



#ifndef LASS_GUARDIAN_OF_INCLUSION_PRIM_POINT_2D_H
#define LASS_GUARDIAN_OF_INCLUSION_PRIM_POINT_2D_H
#pragma once



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
    typedef typename Point2D<T> TSelf;

	typedef typename Vector2D<T> TVector;

	typedef typename TVector::TValue TValue;
	typedef typename TVector::TParam TParam;
	typedef typename TVector::TReference TReference;
	typedef typename TVector::TConstReference TConstReference;
	typedef typename TVector::TNumTraits TNumTraits;

	enum { dimension = TVector::dimension };	/**< number of dimensions of vector */

    template <typename U> struct Rebind
    {
        typedef typename Point2D<U> Type;
    };

	union
	{
		struct
		{
			TVector position;
		};
		struct
		{
			TValue x;
			TValue y;
		};
	};

	Point2D() throw();
	Point2D(TParam iX, TParam iY) throw();

	template <typename U> explicit Point2D(const Point2D<U>& iOther) throw():
		x(iOther.x), 
		y(iOther.y)
	{
	}

	template <typename U> explicit Point2D(const Vector2D<U>& iPositionVector) throw():
		position(iPositionVector)
	{
	}

	TConstReference operator[](unsigned iIndex) const;
	TReference operator[](unsigned iIndex);
	TConstReference at(signed iIndex) const throw();
	TReference at(signed iIndex) throw();

	Point2D<T>& operator+=(const Vector2D<T>& iOffset) throw();
	Point2D<T>& operator-=(const Vector2D<T>& iOffset) throw();

	const bool isZero() const throw();
};

template<typename T> bool operator==(const Point2D<T>& iA, const Point2D<T>& iB) throw();
template<typename T> bool operator!=(const Point2D<T>& iA, const Point2D<T>& iB) throw();

template<typename T> Point2D<T> operator+(const Point2D<T>& iA, const Vector2D<T>& iB) throw();
template<typename T> Point2D<T> operator+(const Vector2D<T>& iA, const Point2D<T>& iB) throw();
template<typename T> Point2D<T> operator-(const Point2D<T>& iA, const Vector2D<T>& iB) throw();
template<typename T> Vector2D<T> operator-(const Point2D<T>& iA, const Point2D<T>& iB) throw();

template<typename T> typename Point2D<T>::TValue distance(const Point2D<T>& iA, const Point2D<T>& iB) throw();
template<typename T> typename Point2D<T>::TValue squaredDistance(const Point2D<T>& iA, const Point2D<T>& iB) throw();
template<typename T> Point2D<T> pointwiseMin(const Point2D<T>& iA, const Point2D<T>& iB) throw();
template<typename T> Point2D<T> pointwiseMax(const Point2D<T>& iA, const Point2D<T>& iB) throw();

template<typename T> std::ostream& operator<<(std::ostream& oOStream, const Point2D<T>& iB);
template<typename T> io::XmlOStream& operator<<(io::XmlOStream& oOStream, const Point2D<T>& iB);
template<typename T> std::istream& operator>>(std::istream& ioIStream, Point2D<T>& oB);
template<typename T> lass::io::MatlabOStream& operator<<(lass::io::MatlabOStream& oOStream, const Point2D<T>& iB);


template<typename T> T doubleTriangleArea( const Point2D<T>& iA, const Point2D<T>& iB, const Point2D<T>& iC );
template<typename T> bool ccw( const Point2D<T>& iA, const Point2D<T>& iB, const Point2D<T>& iC );
template<typename T> bool weakCcw( const Point2D<T>& iA, const Point2D<T>& iB, const Point2D<T>& iC );
template<typename T> bool inCircle( const Point2D<T>& iA, const Point2D<T>& iB, const Point2D<T>& iC, const Point2D<T>& iD );

}

}

#include "point_2d.inl"

#define LASS_PRIM_PYTHON_EXTENSION_POINT_2D
#ifdef LASS_GUARDIAN_OF_INCLUSION_PRIM_PYOBJECT_UTIL_H
#	include "pyobject_util.h"
#endif

#endif
