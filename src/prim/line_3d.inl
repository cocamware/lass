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



#ifndef LASS_GUARDIAN_OF_INCLUSION_PRIM_LINE_3D_INL
#define LASS_GUARDIAN_OF_INCLUSION_PRIM_LINE_3D_INL




#include "line_3d.h"



namespace lass
{

namespace prim
{

template <typename T, class NP>
Line3D<T, NP>::Line3D():
	support_(),
	direction_()
{
	LASS_ASSERT(support_.isZero());
	LASS_ASSERT(direction_.isZero());
}



template <typename T, class NP>
Line3D<T, NP>::Line3D(const TPoint& iSupport, const TVector& iDirection):
    support_(iSupport),
    direction_(iDirection)
{
    NP::normalize(direction_);
}



template <typename T, class NP>
Line3D<T, NP>::Line3D(const TPoint& iSupport, const TPoint& iLookAt):
    support_(iSupport),
    direction_(iLookAt - iSupport)
{
    NP::normalize(direction_);
}



template <typename T, class NP> inline
const typename Line3D<T, NP>::TPoint& 
Line3D<T, NP>::support() const
{
	return support_;
}



template <typename T, class NP> inline
typename Line3D<T, NP>::TPoint& 
Line3D<T, NP>::support()
{
	return support_;
}



/** Return direction of line.
 */
template <typename T, class NP> inline
const typename Line3D<T, NP>::TVector& 
Line3D<T, NP>::direction() const
{
    return direction_;
}



/** Set direction and normalize it if that's the policy
 */
template <typename T, class NP> inline
void Line3D<T, NP>::setDirection(const TVector& iDirection)
{
    direction_ = iDirection;
    NP::normalize(direction_);
}



/** Set direction from support to look-at point.
 */
template <typename T, class NP>
void Line3D<T, NP>::lookAt(const TPoint& iLookAt)
{
    direction_ = iLookAt - support_;
    NP::normalize(direction_);
}



/** return the vector that, if added to the @e projection of @a iPoint, you get @a iPoint again.
 *  <tt>iPoint == (almost) project(iPoint) + reject(iPoint)</tt>
 */
template<typename T, class NP>
const typename Line3D<T, NP>::TVector 
Line3D<T, NP>::reject(const TPoint& iPoint) const
{
    return reject(iPoint - support_);
}



/** return the part of @a iVector that is orthogonal to the line. 
 *  it's the vector that, if added to the @e projection of @a iVector, you get @a iVector again.
 *  <tt>iVector == (almost) project(iVector) + reject(iVector)</tt>.
 */
template<typename T, class NP>
const typename Line3D<T, NP>::TVector 
Line3D<T, NP>::reject(const TVector& iVector) const
{

    return iVector - project(iVector);
}



/** project a point orthogonally onto the line
 */
template<typename T, class NP>
const typename Line3D<T, NP>::TPoint 
Line3D<T, NP>::project(const TPoint& iPoint) const
{
	return support_ + project(iPoint - support_);
}



/** project a vector orthogonally onto the line
 */
template<typename T, class NP>
const typename Line3D<T, NP>::TVector 
Line3D<T, NP>::project(const TVector& iVector) const
{
	return direction_ * NP::divideBySquaredNorm(dot(iVector, direction_), direction_);
}



/** reflect a point orthogonally into the line.
 */
template<typename T, class NP>
const typename Line3D<T, NP>::TPoint 
Line3D<T, NP>::reflect(const TPoint& iPoint) const
{
	return support_ + reflect(iPoint - support_);
}



/** reflect a vector orthogonally to the line
 */
template<typename T, class NP>
const typename Line3D<T, NP>::TVector 
Line3D<T, NP>::reflect(const TVector& iVector) const
{
	return T(2) * project(iVector) - iVector;
}



/** Return point on line by it's parameter.
 *  @return support + t * direction
 */
template <typename T, class NP>
const typename Line3D<T, NP>::TPoint 
Line3D<T, NP>::point(TParam iT) const
{
    return support_ + iT * direction_;
}



/** Return parameter of point on the line that is the projection of the given point.
 */
template <typename T, class NP>
const typename Line3D<T, NP>::TValue 
Line3D<T, NP>::t(const TPoint& iPoint) const
{
    return NP::divideBySquaredNorm(dot(iPoint - support_, direction_), direction_);
}



/** Return true if line is valid (direction isn't a zero vector).
 */
template <typename T, class NP>
const bool Line3D<T, NP>::isValid() const
{
    return !direction_.isZero();
}



/** @relates lass::prim::Line3D
 */
template<typename T, class NP> 
std::ostream& operator<<(std::ostream& ioOStream, const Line3D<T, NP>& iLine)
{
	LASS_ENFORCE(ioOStream) << "{S=" << iLine.support() << ", U=" << iLine.direction() << "}";
	return ioOStream;
}



/** @relates lass::prim::Line3D
 */
template<typename T, class NP> 
io::XmlOStream& operator<<(io::XmlOStream& ioOStream, const Line3D<T, NP>& iLine)
{
	LASS_ENFORCE_STREAM(ioOStream) 
		<< "<Line3D>" << std::endl
		<< "<support>" << iLine.support() << "</support>" << std::endl
		<< "<direction>" << iLine.direction() << "</direction>" << std::endl
		<< "</Line3D>" << std::endl;

	return ioOStream;
}



}

}

#endif

// EOF
