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



#ifndef LASS_GUARDIAN_OF_INCLUSION_PRIM_POINT_3DH_INL
#define LASS_GUARDIAN_OF_INCLUSION_PRIM_POINT_3DH_INL




#include "point_3dh.h"



namespace lass
{

namespace prim
{

template<typename T> inline
Point3DH<T>::Point3DH():
	position_()
{
	LASS_ASSERT(position_.isZero());
}



template<typename T> inline
Point3DH<T>::Point3DH(TParam iX, TParam iY, TParam iZ, TParam iW) :
	position_(iX, iY, iZ, iW)
{
}



template<typename T> inline
Point3DH<T>::Point3DH(const TPoint& iAffinePoint) :
	position_(iAffinePoint.x(), iAffinePoint.y(), iAffinePoint.z(), TNumTraits::one)
{
}



template<typename T> inline
Point3DH<T>::Point3DH(const TVector& iPositionVector) :
	position_(iPositionVector)
{
}



template <typename T> inline
const typename Point3DH<T>::TVector&
Point3DH<T>::position() const
{
	return position_;
}



template<typename T> inline 
typename Point3DH<T>::TConstReference 
Point3DH<T>::operator[](unsigned iIndex) const
{
	return position_[iIndex];
}



/** Wrap index around range. 
 */
template<typename T> inline 
typename Point3DH<T>::TConstReference 
Point3DH<T>::at(signed iIndex) const
{
	return position_.at(iIndex);
}



template<typename T> inline 
typename Point3DH<T>::TConstReference 
Point3DH<T>::x() const
{
	return position_.x;
}



template<typename T> inline 
typename Point3DH<T>::TConstReference 
Point3DH<T>::y() const
{
	return position_.y;
}



template<typename T> inline 
typename Point3DH<T>::TConstReference 
Point3DH<T>::z() const
{
	return position_.z;
}



template<typename T> inline 
typename Point3DH<T>::TConstReference 
Point3DH<T>::w() const
{
	return position_.w;
}



template <typename T> inline
typename Point3DH<T>::TVector&
Point3DH<T>::position()
{
	return position_;
}



template<typename T> inline 
typename Point3DH<T>::TReference 
Point3DH<T>::operator[](unsigned iIndex)
{
	return position_[iIndex];
}



/** Wrap index around range. 
 */
template<typename T> inline 
typename Point3DH<T>::TReference 
Point3DH<T>::at(signed iIndex)
{
	return position_.at(iIndex);
}



template<typename T> inline 
typename Point3DH<T>::TReference 
Point3DH<T>::x()
{
	return position_.x;
}



template<typename T> inline 
typename Point3DH<T>::TReference 
Point3DH<T>::y()
{
	return position_.y;
}



template<typename T> inline 
typename Point3DH<T>::TReference 
Point3DH<T>::z()
{
	return position_.z;
}



template<typename T> inline 
typename Point3DH<T>::TReference 
Point3DH<T>::w()
{
	return position_.w;
}



/** A weird way to get back the same object 
 */
template<typename T> inline
const Point3DH<T>& Point3DH<T>::operator+() const
{
	return *this;
}



template<typename T> inline
const Point3DH<T> Point3DH<T>::operator-() const 
{
	return Point3DH(-position_);
}



template<typename T> inline
Point3DH<T>& Point3DH<T>::operator+=(const Point3DH<T>& iB)  
{
	position_ += iB.position_;
	return *this;
}



template<typename T> inline
Point3DH<T>& Point3DH<T>::operator-=(const Point3DH<T>& iB)
{
	position_ -= iB.position_;
	return *this;
}



template<typename T> inline
Point3DH<T>& Point3DH<T>::operator*=(TParam iB)
{
	position_ *= iB;
	return *this;
}



template<typename T> inline
Point3DH<T>& Point3DH<T>::operator/=(TParam iB)
{
	position_ /= iB;
	return *this;
}



/** Return true if point is origin (0, 0, 0, w).  
 *	w may be 0 but doesn't has to be.
 */
template<typename T> inline
const bool Point3DH<T>::isZero() const
{
	return position_.x == TNumTraits::zero && position_.y == TNumTraits::zero && 
		position_.z == TNumTraits::zero;
}



/** Return true if point is at infinite distance of origin.  test if w == 0.
 */
template<typename T> inline
const bool Point3DH<T>::isInfinite() const
{
	return position_.w == TNumTraits::zero;
}



/** Return true if point is valid.	test if point != (0, 0, 0, 0)
 */
template<typename T> inline
const bool Point3DH<T>::isValid() const
{
	return !position_.isZero();
}



/** Return weight of point.  weight = w.
 */
template<typename T> inline
const typename Point3DH<T>::TParam 
Point3DH<T>::weight() const
{
	return position_.w;
}



/** Return rescaled version of point with weight = 1.
 *	Does not influence original poitn.
 */
template<typename T>
const Point3D<T> Point3DH<T>::affine() const
{
	Point3DH<T> result(*this);
	result.homogenize();
	return Point3D<T>(result.x(), result.y(), result.z());
}



/** Rescale point so that weight is 1.
 */
template<typename T> inline
void Point3DH<T>::homogenize() 
{
	position_ /= position_.w;
	position_.w = TNumTraits::one;
}



// --- FREE FUNCTIONS ---------------------------------------------------------------------------

/** @relates lass::prim::Point3DH
 */
template<typename T> inline
bool operator==(const Point3DH<T>& iA, const Point3DH<T>& iB)
{
	return iA.position() == iB.position();
}



/** @relates lass::prim::Point3DH
 */
template<typename T> inline
bool operator!=(const Point3DH<T>& iA, const Point3DH<T>& iB)
{
	return !(iA == iB);
}



/** @relates lass::prim::Point3DH
 */
template<typename T> inline
Point3DH<T> operator+(const Point3DH<T>& iA, const Point3DH<T>& iB)
{
	Point3DH<T> result(iA);
	result += iB;
	return result;
}



/** @relates lass::prim::Point3DH
 */
template<typename T> inline
Point3DH<T> operator-(const Point3DH<T>& iA, const Point3DH<T>& iB)
{
	Point3DH<T> result(iA.position());
	result -= iB;
	return result;
}



/** @relates lass::prim::Point3DH
 */
template<typename T> inline
Point3DH<T> operator*(const Point3DH<T>& iA, typename Point3DH<T>::TParam iB)
{
	Point3DH<T> result(iA);
	result *= iB;
	return result;
}



/** @relates lass::prim::Point3DH
 */
template<typename T> inline
Point3DH<T> operator/(const Point3DH<T>& iA, typename Point3DH<T>::TParam iB)
{
	Point3DH<T> result(iA);
	result /= iB;
	return result;
}



/** @relates lass::prim::Point3DH
 */
template<typename T>
std::ostream& operator<<(std::ostream& ioOStream, const Point3DH<T>& iB)
{
	LASS_ENFORCE_STREAM(ioOStream) << iB.position();
	return ioOStream;
}



/** @relates lass::prim::Point3DH
 */
template<typename T>
io::XmlOStream& operator<<(io::XmlOStream& oOStream, const Point3DH<T>& iB) 
{
	LASS_ENFORCE_STREAM(oOStream) 
		<< "<Point3DH>" << iB.x() << " " << iB.y() << " " << iB.z() << " " << iB.w() 
		<< "</Point3DH>\n";
	return oOStream;
}



/** @relates lass::prim::Point3DH
 */
template<typename T>
std::istream& operator>>(std::istream& ioIStream, Point3DH<T>& iB)
{
	LASS_ENFORCE_STREAM(ioIStream) >> iB.position();
	return ioIStream;
}



}

}

#endif

// --- END OF FILE ------------------------------------------------------------------------------
