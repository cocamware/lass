/**	@file
 *	@author Bram de Greve (bram@cocamware.com)
 *	@author Tom De Muer (tom@cocamware.com)
 *
 *	*** BEGIN LICENSE INFORMATION ***
 *	
 *	The contents of this file are subject to the Common Public Attribution License 
 *	Version 1.0 (the "License"); you may not use this file except in compliance with 
 *	the License. You may obtain a copy of the License at 
 *	http://lass.sourceforge.net/cpal-license. The License is based on the 
 *	Mozilla Public License Version 1.1 but Sections 14 and 15 have been added to cover 
 *	use of software over a computer network and provide for limited attribution for 
 *	the Original Developer. In addition, Exhibit A has been modified to be consistent 
 *	with Exhibit B.
 *	
 *	Software distributed under the License is distributed on an "AS IS" basis, WITHOUT 
 *	WARRANTY OF ANY KIND, either express or implied. See the License for the specific 
 *	language governing rights and limitations under the License.
 *	
 *	The Original Code is LASS - Library of Assembled Shared Sources.
 *	
 *	The Initial Developer of the Original Code is Bram de Greve and Tom De Muer.
 *	The Original Developer is the Initial Developer.
 *	
 *	All portions of the code written by the Initial Developer are:
 *	Copyright (C) 2004-2025 the Initial Developer.
 *	All Rights Reserved.
 *	
 *	Contributor(s):
 *
 *	Alternatively, the contents of this file may be used under the terms of the 
 *	GNU General Public License Version 2 or later (the GPL), in which case the 
 *	provisions of GPL are applicable instead of those above.  If you wish to allow use
 *	of your version of this file only under the terms of the GPL and not to allow 
 *	others to use your version of this file under the CPAL, indicate your decision by 
 *	deleting the provisions above and replace them with the notice and other 
 *	provisions required by the GPL License. If you do not delete the provisions above,
 *	a recipient may use your version of this file under either the CPAL or the GPL.
 *	
 *	*** END LICENSE INFORMATION ***
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
	TParameterPolicy::enforceRange(iT, TNumTraits::zero);
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



/** Project vector on the axis of the ray
 */
template <typename T, class NP, class PP>
const typename Ray2D<T, NP, PP>::TVector
Ray2D<T, NP, PP>::project(const TVector& iVector) const
{
	return direction_ * NP::divideBySquaredNorm(dot(iVector, direction_), direction_);
}



/** Reject vector against the axis of the ray
 */
template <typename T, class NP, class PP>
const typename Ray2D<T, NP, PP>::TVector
Ray2D<T, NP, PP>::reject(const TVector& iVector) const
{
	return iVector - project(iVector);
}



/** Reflect vector against the axis of the ray
 */
template <typename T, class NP, class PP>
const typename Ray2D<T, NP, PP>::TVector
Ray2D<T, NP, PP>::reflect(const TVector& iVector) const
{
	return 2 * project(iVector) - iVector;
}




/** Project point on the axis of the ray
 */
template <typename T, class NP, class PP>
const typename Ray2D<T, NP, PP>::TPoint
Ray2D<T, NP, PP>::project(const TPoint& iPoint) const
{
	return support_ + project(iPoint - support_);
}



/** Reject point against the axis of the ray
 */
template <typename T, class NP, class PP>
const typename Ray2D<T, NP, PP>::TVector
Ray2D<T, NP, PP>::reject(const TPoint& iPoint) const
{
	return reject(iPoint - support_);
}



/** Reject point against the axis of the ray
 */
template <typename T, class NP, class PP>
const typename Ray2D<T, NP, PP>::TPoint
Ray2D<T, NP, PP>::reflect(const TPoint& iPoint) const
{
	return support_ + reflect(iPoint - support_);
}



/** Return on what side a point is located.
 */
template <typename T, class NP, class PP>
Side Ray2D<T, NP, PP>::classify(const TPoint& iPoint) const
{
	const T surf = perpDot(iPoint - support_, direction_);
	if (surf>T(0))
		return sRight;
	if (surf<T(0))
		return sLeft;
	return sSurface;
}


/** Return true if ray is valid (direction isn't a zero vector).
 */
template <typename T, class NP, class PP>
bool Ray2D<T, NP, PP>::isValid() const
{
	return !direction_.isZero();
}



/** @relates lass::prim::Ray2D
 */
template<typename T, class NP>
std::ostream& operator<<(std::ostream& ioOStream, const Ray2D<T, NP>& iRay)
{
	LASS_ENFORCE(ioOStream) << "{S=" << iRay.support() << ", D=" << iRay.direction() << "}";
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
