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



#ifndef LASS_GUARDIAN_OF_INCLUSION_PRIM_POINT_2D_INL
#define LASS_GUARDIAN_OF_INCLUSION_PRIM_POINT_2D_INL
#pragma once



#include "point_2d.h"



namespace lass
{

namespace prim
{

template<typename T> inline 
Point2D<T>::Point2D():
	position()
{
	LASS_ASSERT(position.isZero());
}



template<typename T> inline 
Point2D<T>::Point2D(TParam iX, TParam iY):
	position(iX, iY)
{
}



template<typename T> inline 
typename Point2D<T>::TConstReference 
Point2D<T>::operator[](unsigned iIndex) const
{
	return position[iIndex];
}



template<typename T> inline 
typename Point2D<T>::TReference 
Point2D<T>::operator[](unsigned iIndex)
{
	return position[iIndex];
}



/** Wrap index around range. 
 */
template<typename T> inline 
typename Point2D<T>::TConstReference 
Point2D<T>::at(signed iIndex) const
{
	return position.at(iIndex);
}



/** Wrap index around range. 
 */
template<typename T> inline 
typename Point2D<T>::TReference 
Point2D<T>::at(signed iIndex)
{
	return position.at(iIndex);
}



template<typename T> inline 
typename Point2D<T>& 
Point2D<T>::operator+=(const Vector2D<T>& iB)
{
	position += iB;
	return *this;
}



template<typename T> inline
typename Point2D<T>&
Point2D<T>::operator-=(const Vector2D<T>& iB)
{
	position -= iB;
	return *this;
}



template<typename T> inline
const bool Point2D<T>::isZero() const
{
	return position.isZero();
}



// --- FREE FUNCTIONS ---------------------------------------------------------------------------

/** @relates lass::prim::Point2D
 */
template<typename T> inline 
bool operator==(const Point2D<T>& iA, const Point2D<T>& iB)
{
	return iA.position == iB.position;
}



/** @relates lass::prim::Point2D
 */
template<typename T> inline 
bool operator!=(const Point2D<T>& iA, const Point2D<T>& iB)
{
	return !(iA == iB);
}



/** @relates lass::prim::Point2D
 */
template<typename T> inline 
Point2D<T> operator+(const Point2D<T>& iA, const Vector2D<T>& iB)
{
	Point2D<T> result(iA);
	result += iB;
	return result;
}



/** @relates lass::prim::Point2D  
 */
template<typename T> inline
Point2D<T> operator+(const Vector2D<T>& iA, const Point2D<T>& iB)
{
	Point2D<T> result(iB);
	result += iA;
	return result;
}



/** @relates lass::prim::Point2D  
 */
template<typename T> inline 
Point2D<T> operator-(const Point2D<T>& iA, const Vector2D<T>& iB)
{
	Point2D<T> result(iA);
	result -= iB;
	return result;
}



/** @relates lass::prim::Point2D   
 */
template<typename T> inline 
Vector2D<T> operator-(const Point2D<T>& iA, const Point2D<T>& iB)
{
	Vector2D<T> result(iA.position);
	result -= iB.position;
	return result;
}



/** @relates lass::prim::Point2D    
 */
template<typename T> inline 
typename Point2D<T>::TValue distance(const Point2D<T>& iA, const Point2D<T>& iB)
{
	const Vector2D<T> difference = iA - iB;
	return difference.norm();
}

/** @relates lass::prim::Point2D    
 */
template<typename T> inline 
typename Point2D<T>::TValue squaredDistance(const Point2D<T>& iA, const Point2D<T>& iB)
{
	const Vector2D<T> difference = iA - iB;
	return difference.squaredNorm();
}


/** return a point with, for each coordinate, the minimum value of iA and iB
 *  @relates lass::prim::Point2D
 */
template<typename T> inline
Point2D<T> pointwiseMin(const Point2D<T>& iA, const Point2D<T>& iB)
{
	return Point2D<T>(pointwiseMin(iA.position, iB.position));
}



/** return a point with, for each coordinate, the maximum value of iA and iB
 *  @relates lass::prim::Point2D
 */
template<typename T> inline
Point2D<T> pointwiseMax(const Point2D<T>& iA, const Point2D<T>& iB)
{
	return Point2D<T>(pointwiseMax(iA.position, iB.position));
}



/** @relates lass::prim::Point2D 
 */
template<typename T>
std::ostream& operator<<(std::ostream& ioOStream, const Point2D<T>& iB)
{
	LASS_ENFORCE_STREAM(ioOStream) << iB.position;
	return ioOStream;
}



/** @relates lass::prim::Point2D
 */
template<typename T>
io::XmlOStream& operator<<(io::XmlOStream& oOStream, const Point2D<T>& iB)
{
	LASS_ENFORCE_STREAM(oOStream) 
		<< "<Point2D>" << iB.x << " " << iB.y << "</Point2D>" << std::endl;
	return oOStream;
}



/** @relates lass::prim::Point2D 
 */
template<typename T> 
lass::io::MatlabOStream& operator<<(lass::io::MatlabOStream& ioOStream, const Point2D<T>& iB)
{
	LASS_ENFORCE_STREAM(ioOStream) << "lasthandle = line(";
	ioOStream << iB[0] << "," << iB[1] << ");" << std::endl;
	ioOStream << "set(lasthandle,'Marker','o');" << std::endl;
	ioOStream << "set(lasthandle,'markersize',2);" << std::endl;
	return ioOStream;
}


/** @relates lass::prim::Point2D 
 */
template<typename T>
std::istream& operator>>(std::istream& ioIStream, Point2D<T>& iB)
{
	LASS_ENFORCE_STREAM(ioIStream) >> iB.position;
	return ioIStream;
}

/** returns twice signed area of triangle iA,iB,iC
 *  @relates lass::prim::Point2D 
 */
template<typename T> 
T doubleTriangleArea( const Point2D<T>& iA, const Point2D<T>& iB, const Point2D<T>& iC )
{
	return	(	( iB.x * iC.y - iB.y * iC.x )
			  - ( iA.x * iC.y - iA.y * iC.x )
			  + ( iA.x * iB.y - iA.y * iB.x ) );
}


/** returns true when the line iB->iC is counter clockwise oriented with respect to iA->iB
 *  @relates lass::prim::Point2D 
 */
template<typename T> 
bool ccw( const Point2D<T>& iA, const Point2D<T>& iB, const Point2D<T>& iC )
{
	return	doubleTriangleArea(iA,iB,iC) > static_cast<T>(0.0);
}

/** returns true when the line iB->iC is counter clockwise oriented with respect to iA->iB.
 *  When iC is in line of iA and iB also returns true.
 *  @relates lass::prim::Point2D 
 */
template<typename T> 
bool weakCcw( const Point2D<T>& iA, const Point2D<T>& iB, const Point2D<T>& iC )
{
	return	doubleTriangleArea(iA,iB,iC) >= static_cast<T>(0.0);
}

/** returns true when the point iD is strictly (within numerical precision) in the circle 
 *	going through iA, iB and iC.
 *  @relates lass::prim::Point2D 
 *	@note this test is used for establishing Delaunay neighborhoods and this tests 
 *		  numerical stability determines the overall stability of the Delaunay meshers
 */
template<typename T> 
bool inCircle( const Point2D<T>& iA, const Point2D<T>& iB, const Point2D<T>& iC, const Point2D<T>& iD )
{
	T az = iA.position.squaredNorm();
	T bz = iB.position.squaredNorm();
	T cz = iC.position.squaredNorm();
	T dz = iD.position.squaredNorm();

	T det = (az * doubleTriangleArea(iB, iC, iD) - bz * doubleTriangleArea(iA, iC, iD)
		+    cz * doubleTriangleArea(iA, iB, iD) - dz * doubleTriangleArea(iA, iB, iC));

	return (det > 0);
}


}

}

#endif
