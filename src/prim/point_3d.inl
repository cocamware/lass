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



#ifndef LASS_GUARDIAN_OF_INCLUSION_PRIM_POINT_3D_INL
#define LASS_GUARDIAN_OF_INCLUSION_PRIM_POINT_3D_INL




#include "point_3d.h"



namespace lass
{
	
namespace prim
{

template<typename T> inline 
Point3D<T>::Point3D() :
	position_()
{
	LASS_ASSERT(position_.isZero());
}



template<typename T> inline 
Point3D<T>::Point3D(TParam iX, TParam iY, TParam iZ) :
	position_(iX, iY, iZ)
{
}



template<typename T>
template<typename U>
Point3D<T>::Point3D(const Point3D<U>& iOther):
	position_(iOther.position_)
{
}



template<typename T>
template<typename U>
Point3D<T>::Point3D(const Vector3D<U>& iPositionVector):
	position_(iPositionVector)
{
}



template <typename T> inline
typename const Point3D<T>::TVector&
Point3D<T>::position() const
{
	return position_;
}



template<typename T> inline 
typename Point3D<T>::TConstReference 
Point3D<T>::operator[](unsigned iIndex) const
{
	return position_[iIndex];
}



/** Wrap index around range. 
 */
template<typename T> inline 
typename Point3D<T>::TConstReference 
Point3D<T>::at(signed iIndex) const
{
	return position_.at(iIndex);
}



template<typename T> inline 
typename Point3D<T>::TConstReference 
Point3D<T>::x() const
{
	return position_.x;
}



template<typename T> inline 
typename Point3D<T>::TConstReference 
Point3D<T>::y() const
{
	return position_.y;
}



template<typename T> inline 
typename Point3D<T>::TConstReference 
Point3D<T>::z() const
{
	return position_.z;
}



template <typename T> inline
typename Point3D<T>::TVector&
Point3D<T>::position()
{
	return position_;
}



template<typename T> inline 
typename Point3D<T>::TReference 
Point3D<T>::operator[](unsigned iIndex)
{
	return position_[iIndex];
}



/** Wrap index around range. 
 */
template<typename T> inline 
typename Point3D<T>::TReference 
Point3D<T>::at(signed iIndex)
{
	return position_.at(iIndex);
}



template<typename T> inline 
typename Point3D<T>::TReference 
Point3D<T>::x()
{
	return position_.x;
}



template<typename T> inline 
typename Point3D<T>::TReference 
Point3D<T>::y()
{
	return position_.y;
}



template<typename T> inline 
typename Point3D<T>::TReference 
Point3D<T>::z()
{
	return position_.z;
}



template<typename T> inline 
Point3D<T>& Point3D<T>::operator+=(const TVector& iB) 
{
	position_ += iB;
	return *this;
}



template<typename T> inline 
Point3D<T>& Point3D<T>::operator-=(const TVector& iB) 
{
	position_ -= iB;
	return *this;
}



template<typename T> inline 
const bool Point3D<T>::isZero() const
{
	return position_.isZero();
}



// --- FREE FUNCTIONS ---------------------------------------------------------------------------

/** @relates lass::prim::Point3D
 */
template<typename T> inline 
bool operator==(const Point3D<T>& iA, const Point3D<T>& iB) 
{
	return iA.position() == iB.position();
}



/** @relates lass::prim::Point3D
 */
template<typename T> inline
bool operator!=(const Point3D<T>& iA, const Point3D<T>& iB) 
{
	return !(iA == iB);
}



/** @relates lass::prim::Point3D
 */
template<typename T> inline 
Point3D<T> operator+(const Point3D<T>& iA, const Vector3D<T>& iB)
{
	Point3D<T> result(iA);
	result += iB;
	return result;
}



/** @relates lass::prim::Point3D
 */
template<typename T> inline 
Point3D<T> operator-(const Point3D<T>& iA, const Vector3D<T>& iB)
{
	Point3D<T> result(iA);
	result -= iB;
	return result;
}



/** @relates lass::prim::Point3D
 */
template<typename T> inline 
Point3D<T> operator+(const Vector3D<T>& iA, const Point3D<T>& iB)
{
	Point3D<T> result(iB);
	result += iA;
	return result;
}



/** @relates lass::prim::Point3D
 */
template<typename T> inline 
Vector3D<T> operator-(const Point3D<T>& iA, const Point3D<T>& iB)
{
	Vector3D<T> result(iA.position());
	result -= iB.position();
	return result;
}



/** return the distance between two points
 *  @relates lass::prim::Point3D
 */
template<typename T> inline 
typename Point3D<T>::TValue distance(const Point3D<T>& iA, const Point3D<T>& iB)
{
	const Vector3D<T> difference = iA - iB;
	return difference.norm();
}



/** return a point with, for each coordinate, the minimum value of iA and iB
 *  @relates lass::prim::Point3D
 */
template<typename T> inline 
Point3D<T> pointwiseMin(const Point3D<T>& iA, const Point3D<T>& iB)
{
	return Point3D<T>(pointwiseMin(iA.position(), iB.position()));
}



/** return a point with, for each coordinate, the maximum value of iA and iB
 *  @relates lass::prim::Point3D
 */
template<typename T> inline 
Point3D<T> pointwiseMax(const Point3D<T>& iA, const Point3D<T>& iB)
{
    return Point3D<T>(pointwiseMax(iA.position(), iB.position()));
}



/** @relates lass::prim::Point3D
 */
template<typename T>
std::ostream& operator<<(std::ostream& ioOStream, const Point3D<T>& iB)
{
	LASS_ENFORCE_STREAM(ioOStream) << iB.position();
	return ioOStream;
}



/** @relates lass::prim::Point3D
 */
template<typename T>
io::XmlOStream& operator<<(io::XmlOStream& oOStream, const Point3D<T>& iB)
{
	LASS_ENFORCE_STREAM(oOStream) 
		<< "<Point3D>" << iB.x() << " " << iB.y() << " " << iB.z() << "</Point3D>" << std::endl;
	return oOStream;
}


/** @relates lass::prim::Point3D
 */
template<typename T>
std::istream& operator>>(std::istream& ioIStream, Point3D<T>& iB)
{
	LASS_ENFORCE_STREAM(ioIStream) >> iB.position();
	return ioIStream;
}



}

}

#endif
