/** 
*   @author Bram de Greve (bramz@users.sourceforge.net)
*   @author Tom De Muer (tomdemuer@users.sourceforge.net)
*
*	Distributed under the terms of the GPL (GNU Public License)
*
* 	The LASS License:
*   
*	Copyright 2004 Bram de Greve and Tom De Muer
*
*   LASS is free software; you can redistribute it and/or modify
*   it under the terms of the GNU General Public License as published by
*   the Free Software Foundation; either version 2 of the License, or
*   (at your option) any later version.
*
*   This program is distributed in the hope that it will be useful,
*   but WITHOUT ANY WARRANTY; without even the implied warranty of
*   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
*   GNU General Public License for more details.
*
*   You should have received a copy of the GNU General Public License
*   along with this program; if not, write to the Free Software
*   Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
*


 */



/** @struct lass::prim::Point2DH
 *	@brief homogenous 2D Point
 *	@author BdG
 *	@date 2003
 */



#ifndef LASS_GUARDIAN_OF_INCLUSION_PRIM_POINT_2DH_H
#define LASS_GUARDIAN_OF_INCLUSION_PRIM_POINT_2DH_H
#pragma once



#include "prim_common.h"
#include "point_2d.h"
#include "vector_3d.h"


namespace lass
{

namespace prim
{

template<typename T>
struct Point2DH
{
    typedef typename Point2DH<T> TSelf;

	typedef typename Point2D<T> TPoint;
	typedef typename Vector3D<T> TVector;

	typedef typename TVector::TValue TValue;
	typedef typename TVector::TParam TParam;
	typedef typename TVector::TReference TReference;
	typedef typename TVector::TConstReference TConstReference;
	typedef typename TVector::TNumTraits TNumTraits;

	enum { dimension = TPoint::dimension };	/**< number of dimensions */

    template <typename U> struct Rebind
    {
        typedef Point2DH<U> Type;
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
			TValue z;
		};
	};

	Point2DH() throw();
	Point2DH(TParam iX, TParam iY, TParam iZ = TNumTraits::one) throw();
	Point2DH(const TPoint& iAffinePoint) throw();
	explicit Point2DH(const TVector& iPositionVector) throw();

	TConstReference operator[](unsigned iIndex) const;
	TReference operator[](unsigned iIndex);
	TConstReference at(signed iIndex) const throw();
	TReference at(signed iIndex) throw();

	const Point2DH<T>& operator+() const throw();
	const Point2DH<T> operator-() const throw();

	Point2DH<T>& operator+=(const Point2DH<T>& iB) throw();
	Point2DH<T>& operator-=(const Point2DH<T>& iB) throw();
	Point2DH<T>& operator*=(TParam iB) throw();
	Point2DH<T>& operator/=(TParam iB) throw();

	const bool isZero() const throw();
	const bool isInfinite() const throw();
	const bool isValid() const throw();
	const TValue weight() const throw();
	const Point2D<T> affine() const throw();

	void homogenize();
};

template<typename T> bool operator==(const Point2DH<T>& iA, const Point2DH<T>& iB);
template<typename T> bool operator!=(const Point2DH<T>& iA, const Point2DH<T>& iB);

template<typename T> Point2DH<T> operator+(const Point2DH<T>& iA, const Point2DH<T>& iB);
template<typename T> Point2DH<T> operator+(const Point2DH<T>& iA, const Point2D<T>& iB);
template<typename T> Point2DH<T> operator+(const Point2D<T>& iA, const Point2DH<T>& iB);
template<typename T> Point2DH<T> operator+(const Point2D<T>& iA, const Point2D<T>& iB);

template<typename T> Point2DH<T> operator-(const Point2DH<T>& iA, const Point2DH<T>& iB);
template<typename T> Point2DH<T> operator*(const Point2DH<T>& iA, typename Point2DH<T>::TParam iB);
template<typename T> Point2DH<T> operator/(const Point2DH<T>& iA, typename Point2DH<T>::TParam iB);
template<typename T> Point2DH<T> operator*(typename Point2DH<T>::TParam iA, const Point2DH<T>& iB);

template<typename T> std::ostream& operator<<(std::ostream& oOStream, const Point2DH<T>& iB);
template<typename T> io::XmlOStream& operator<<(io::XmlOStream& oOStream, const Point2DH<T>& iB);
template<typename T> std::istream& operator>>(std::istream& ioIStream, Point2DH<T>& oB);



}

}

#include "point_2dh.inl"

#endif

// --- END OF FILE ------------------------------------------------------------------------------