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



#ifndef LASS_GUARDIAN_OF_INCLUSION_PRIM_RAY_2D_INL
#define LASS_GUARDIAN_OF_INCLUSION_PRIM_RAY_2D_INL




#include "ray_2d.h"



namespace lass
{

namespace prim
{

template <typename T, class NP, class PP>
Ray2D<T, NP, PP>::Ray2D():
	support_(),
	direction_()
{
	LASS_ASSERT(support_.isZero());
	LASS_ASSERT(direction_.isZero());
}



template <typename T, class NP, class PP>
Ray2D<T, NP, PP>::Ray2D(const TPoint& iSupport, const TVector& iDirection):
    support_(iSupport),
    direction_(iDirection)
{
    NP::normalize(direction_);
}



template <typename T, class NP, class PP>
Ray2D<T, NP, PP>::Ray2D(const TPoint& iSupport, const TPoint& iLookAt):
    support_(iSupport),
    direction_(iLookAt - iSupport)
{
    NP::normalize(direction_);
}



/** return origin of ray.
 *  the origin is also the support point of the ray.
 */
template <typename T, class NP, class PP>
const typename Ray2D<T, NP, PP>::TPoint& 
Ray2D<T, NP, PP>::support() const
{
	return support_;
}



/** access origin of ray
 *  the origin is also the support point of the ray.
 */
template <typename T, class NP, class PP>
typename Ray2D<T, NP, PP>::TPoint& 
Ray2D<T, NP, PP>::support()
{
	return support_;
}



/** Return direction of ray.
 */
template <typename T, class NP, class PP>
const typename Ray2D<T, NP, PP>::TVector& 
Ray2D<T, NP, PP>::direction() const
{
    return direction_;
}



/** Set direction and normalize it if that's the policy
 */
template <typename T, class NP, class PP>
void Ray2D<T, NP, PP>::setDirection(const TVector& iDirection)
{
    direction_ = iDirection;
    NP::normalize(direction_);
}



/** Set direction from origin to look-at point.
 */
template <typename T, class NP, class PP>
void Ray2D<T, NP, PP>::lookAt(const TPoint& iLookAt)
{
    direction_ = iLookAt - support_;
    NP::normalize(direction_);
}



/** Return point on ray by it's parameter.
 *  @exception throw an error if parameter is out of range t >= 0, if Bounded is
 *             used as @c ParameterPolicy.
 *  @return origin + t * direction
 */
template <typename T, class NP, class PP>
const typename Ray2D<T, NP, PP>::TPoint 
Ray2D<T, NP, PP>::point(TParam iT) const
{
	LASS_PRIM_ENFORCE_RANGE(PP, iT >= TNumTraits::zero);
    return support_ + iT * direction_;
}



/** Return parameter of point on the ray that is the projection of the given point.
 *  @warning it can return a (invalid) negative number even if you've used a bounded parameter policy.
 */
template <typename T, class NP, class PP>
const typename Ray2D<T, NP, PP>::TValue 
Ray2D<T, NP, PP>::t(const TPoint& iPoint) const
{
    return NP::divideBySquaredNorm(dot(iPoint - support_, direction_), direction_);
}



/** Return true if ray is valid (direction isn't a zero vector).
 */
template <typename T, class NP, class PP>
const bool Ray2D<T, NP, PP>::isValid() const
{
    return !direction_.isZero();
}



/** @relates lass::prim::Ray2D
 */
template<typename T, class NP> 
std::ostream& operator<<(std::ostream& ioOStream, const Ray2D<T, NP>& iRay)
{
	LASS_ENFORCE(ioOStream) << "{O=" << iRay.origin() << ", D=" << iRay.direction() << "}";
	return ioOStream;
}



/** @relates lass::prim::Ray2D
 */
template<typename T, class NP> 
io::XmlOStream& operator<<(io::XmlOStream& ioOStream, const Ray2D<T, NP>& iRay)
{
	LASS_ENFORCE_STREAM(ioOStream) 
		<< "<Ray2D>\n"
		<< "<support>" << iRay.support() << "</support>\n"
		<< "<direction>" << iRay.direction() << "</direction>\n"
		<< "</Ray2D>\n";
	return ioOStream;
}

}

}

#endif

// --- END OF FILE ------------------------------------------------------------------------------
