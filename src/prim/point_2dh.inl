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



#ifndef LASS_GUARDIAN_OF_INCLUSION_PRIM_POINT_2DH_INL
#define LASS_GUARDIAN_OF_INCLUSION_PRIM_POINT_2DH_INL
#pragma once



#include "point_2dh.h"



namespace lass
{
namespace prim
{

template<typename T> inline 
Point2DH<T>::Point2DH():
	position()
{
	LASS_ASSERT(position.isZero());
}



template<typename T> inline 
Point2DH<T>::Point2DH(TParam iX, TParam iY, TParam iZ):
	position(iX, iY, iZ)
{
}



template<typename T> inline 
Point2DH<T>::Point2DH(const TPoint& iAffinePoint):
	position(iAffinePoint.x, iAffinePoint.y, TNumTraits::one)
{
}



template<typename T> inline 
Point2DH<T>::Point2DH(const TVector& iPositionVector):
	position(iPositionVector)
{
}



template<typename T> inline 
typename Point2DH<T>::TConstReference 
Point2DH<T>::operator[](unsigned iIndex) const
{
	return position[iIndex];
}



template<typename T> inline 
typename Point2DH<T>::TReference 
Point2DH<T>::operator[](unsigned iIndex)
{
	return position[iIndex];
}



/** Wrap index around range. 
 */
template<typename T> inline 
typename Point2DH<T>::TConstReference 
Point2DH<T>::at(signed iIndex) const
{
	return position.at(iIndex);
}



/** Wrap index around range. 
 */
template<typename T> inline 
typename Point2DH<T>::TReference 
Point2DH<T>::at(signed iIndex)
{
	return position.at(iIndex);
}



/** A weird way to get back the same object 
 */
template<typename T> inline 
const Point2DH<T>& Point2DH<T>::operator+() const
{
	return *this;
}



template<typename T> inline 
const Point2DH<T> Point2DH<T>::operator-() const
{
	return Point2DH(-position);
}



template<typename T> inline 
Point2DH<T>& Point2DH<T>::operator+=(const Point2DH<T>& iB) 
{
	position += iB.position;
	return *this;
}



template<typename T> inline 
Point2DH<T>& Point2DH<T>::operator-=(const Point2DH<T>& iB)
{
	position -= iB.position;
	return *this;
}



template<typename T> inline 
Point2DH<T>& Point2DH<T>::operator*=(TParam iB)
{
	position *= iB;
	return *this;
}



template<typename T> inline 
Point2DH<T>& Point2DH<T>::operator/=(TParam iB)
{
	position /= iB;
	return *this;
}



/** Return true if point is origin (0, 0, z).  
 *	z may be 0 but doesn't has to be.
 */
template<typename T> inline 
const bool Point2DH<T>::isZero() const
{
	return x == TNumTraits::zero && y == TNumTraits::zero;
}



/** Return true if point is at infinite distance of origin.  test if z == 0.
 */
template<typename T> inline 
const bool Point2DH<T>::isInfinite() const
{
	return z == TNumTraits::zero;
}



/** Return true if point is valid.	test if point != (0, 0, 0)
 */
template<typename T> inline 
const bool Point2DH<T>::isValid() const
{
	return !position.isZero();
}



/** Return weight of point.  weight = z.
 */
template<typename T> inline 
typename const Point2DH<T>::TValue Point2DH<T>::weight() const
{
	return z;
}



/** Return rescaled version of point with weight = 1.
 *	Does not influence original poitn.
 */
template<typename T> inline
const Point2D<T> Point2DH<T>::affine() const
{
	Point2DH<T> result(*this);
	result.homogenize();
	return Point2D<T>(result.x, result.y);
}



/** Rescale point so that weight is 1.
 */
template<typename T> inline
void Point2DH<T>::homogenize()
{
	position /= z;
	z = TNumTraits::one;
}



// --- FREE FUNCTIONS ---------------------------------------------------------------------------

/** @relates lass::prim::Point2DH
 */
template<typename T> inline 
bool operator==(const Point2DH<T>& iA, const Point2DH<T>& iB)
{
	return iA.position == iB.position;
}



/** @relates lass::prim::Point2DH
 */
template<typename T> inline 
bool operator!=(const Point2DH<T>& iA, const Point2DH<T>& iB)
{
	return !(iA == iB);
}



/** @relates lass::prim::Point2DH
 */
template<typename T> inline 
Point2DH<T> operator+(const Point2DH<T>& iA, const Point2DH<T>& iB)
{
	Point2DH<T> result(iA);
	result += iB;
	return result;
}



/** @relates lass::prim::Point2DH
 */
template<typename T> inline 
Point2DH<T> operator+(const Point2DH<T>& iA, const Point2D<T>& iB)
{
	Point2DH<T> result(iA);
	result += iB;
	return result;
}



/** @relates lass::prim::Point2DH
 */
template<typename T> inline 
Point2DH<T> operator+(const Point2D<T>& iA, const Point2DH<T>& iB)
{
	Point2DH<T> result(iA);
	result += iB;
	return result;
}



/** @relates lass::prim::Point2DH
 */
template<typename T> inline 
Point2DH<T> operator+(const Point2D<T>& iA, const Point2D<T>& iB)
{
	Point2DH<T> result(iA);
	result += iB;
	return result;
}



/** @relates lass::prim::Point2DH
 */
template<typename T> inline 
Point2DH<T> operator-(const Point2DH<T>& iA, const Point2DH<T>& iB)
{
	Point2DH<T> result(iA.position);
	result -= iB;
	return result;
}



/** @relates lass::prim::Point2DH
 */
template<typename T> inline 
Point2DH<T> operator*(const Point2DH<T>& iA, typename Point2DH<T>::TParam iB)
{
	Point2DH<T> result(iA);
	result *= iB;
	return result;
}



/** @relates lass::prim::Point2DH
 */
template<typename T> inline 
Point2DH<T> operator/(const Point2DH<T>& iA, typename Point2DH<T>::TParam iB)
{
	Point2DH<T> result(iA);
	result /= iB;
	return result;
}



/** @relates lass::prim::Point2DH
 */
template<typename T>
std::ostream& operator<<(std::ostream& ioOStream, const Point2DH<T>& iB)
{
	LASS_ENFORCE_STREAM(ioOStream) << iB.position;
	return ioOStream;
}



/** @relates lass::prim::Point2DH
 */
template<typename T>
io::XmlOStream& operator<<(io::XmlOStream& oOStream, const Point2DH<T>& iB)
{
	LASS_ENFORCE_STREAM(oOStream) 
		<< "<Point2DH>" << iB.x << " " << iB.y << " " << iB.z << "</Point2DH>" << std::endl;
	return oOStream;
}



/** @relates lass::prim::Point2DH
 */
template<typename T>
std::istream& operator>>(std::istream& ioIStream, Point2DH<T>& iB)
{
	LASS_ENFORCE(ioIStream) >> iB.position;
	return ioIStream;
}



}

}

#endif

// --- END OF FILE ------------------------------------------------------------------------------
