/** @file
 *  @author Bram de Greve (bramz@users.sourceforge.net)
 *  @author Tom De Muer (tomdemuer@users.sourceforge.net)
 *
 *  Distributed under the terms of the GPL (GNU Public License)
 *
 *  The LASS License:
 *
 *  Copyright 2004 Bram de Greve and Tom De Muer
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



#ifndef LASS_GUARDIAN_OF_INCLUSION_PRIM_IMPL_PLANE_3D_COMBINED_INL
#define LASS_GUARDIAN_OF_INCLUSION_PRIM_IMPL_PLANE_3D_COMBINED_INL

#include "../prim_common.h"
#include "plane_3d_combined.h"

namespace lass
{
namespace prim
{
namespace impl
{

// --- public --------------------------------------------------------------------------------------

/** initializes to an invalid state.
 */
template<typename T, class NP>
Plane3DCombined<T, NP>::Plane3DCombined():
	support_(),
	directionU_(),
	directionV_(),
	reciprocalU_(),
	reciprocalV_(),
	normal_(),
	d_(TNumTraits::zero)
{
	LASS_ASSERT(!isValid());
}



/** Construct a plane through three points.
 *  - support point S is given by the first point iSupport.
 *  - direction vectors U and V are choosen from iSupport to iPointU and iPointV respectively
 *    (U = iPointU - iSupport, V = iPointV - iSupport).
 *  - normal vector N is given by the cross product N = U x V.
 *  - value d is choosen so that the support point is indeed a point of the plane.
 */
template<typename T, class NP>
Plane3DCombined<T, NP>::Plane3DCombined(const TPoint& iSupport,
										const TPoint& iPointU,
										const TPoint& iPointV):
	support_(iSupport),
	directionU_(iPointU - iSupport),
	directionV_(iPointV - iSupport)
{
	Plane3DImplDetail::generateCartesian(support_, directionU_, directionV_, normal_, d_);
	NP::normalize(directionU_);
	NP::normalize(directionV_);
	NP::normalizeAndScale(normal_, d_);
	Plane3DImplDetail::generateReciprocal(directionU_, directionV_, reciprocalU_, reciprocalV_);
}



/** construct a plane through a support point and by two direction vectors..
 *  - support point S is given by the point iSupport.
 *  - direction vectors U and V are given by iDirectionU and iDirectionV.
 *  - normal vector N is given by the cross product N = U x V.
 *  - value d is choosen so that the support point is indeed a point of the plane.
 */
template<typename T, class NP>
Plane3DCombined<T, NP>::Plane3DCombined(const TPoint& iSupport,
										const TVector& iDirectionU,
										const TVector& iDirectionV):
	support_(iSupport),
	directionU_(iDirectionU),
	directionV_(iDirectionV)
{
	Plane3DImplDetail::generateCartesian(support_, directionU_, directionV_, normal_, d_);
	NP::normalize(directionU_);
	NP::normalize(directionV_);
	NP::normalizeAndScale(normal_, d_);
	Plane3DImplDetail::generateReciprocal(directionU_, directionV_, reciprocalU_, reciprocalV_);
}



/** Construct a plane through a support point and by a normal vector.
 *  - support point S is given by the point iSupport.
 *  - normal vector N is given by the vector iNormal.
 *  - direction vectors U and V are automatically generated so that N == U x V.
 *  - value d is choosen so that the support point is indeed a point of the plane.
 */
template<typename T, class NP>
Plane3DCombined<T, NP>::Plane3DCombined(const TVector& iNormal, const TPoint& iSupport):
	support_(iSupport),
	normal_(iNormal),
	d_(-dot(iNormal, iSupport.position()))
{
	Plane3DImplDetail::generateDirections(normal_, directionU_, directionV_);
	NP::normalize(directionU_);
	NP::normalize(directionV_);
	NP::normalizeAndScale(normal_, d_);
	Plane3DImplDetail::generateReciprocal(directionU_, directionV_, reciprocalU_, reciprocalV_);
}



/** Construct a plane by a cartesian equation N.P + d == 0.
 *  - normal vector N is given by the vector iNormal.
 *  - value d is given by the value iD.
 *  - support point S automatically generated so that N.S + d == 0.
 *  - direction vectors U and V are automatically generated so that N == U x V.
 */
template<typename T, class NP>
Plane3DCombined<T, NP>::Plane3DCombined(const TVector& iNormal, TParam iD):
	support_(iNormal * (-iD / iNormal.squaredNorm())),
	normal_(iNormal),
	d_(iD)
{
	Plane3DImplDetail::generateDirections(normal_, directionU_, directionV_);
	NP::normalize(directionU_);
	NP::normalize(directionV_);
	NP::normalizeAndScale(normal_, d_);
	Plane3DImplDetail::generateReciprocal(directionU_, directionV_, reciprocalU_, reciprocalV_);
}



/** return support point.
 */
template<typename T, class NP>
const typename Plane3DCombined<T, NP>::TPoint& Plane3DCombined<T, NP>::support() const
{
	return support_;
}



/** return U and V direction vectors
 */
template<typename T, class NP>
void Plane3DCombined<T, NP>::getDirections(TVector& oDirectionU, TVector& oDirectionV) const
{
	oDirectionU = directionU_;
	oDirectionV = directionV_;
}



/** return U direction vector.
 */
template<typename T, class NP>
const typename Plane3DCombined<T, NP>::TVector& Plane3DCombined<T, NP>::directionU() const
{
	return directionU_;
}



/** return V direction vector.
 */
template<typename T, class NP>
const typename Plane3DCombined<T, NP>::TVector& Plane3DCombined<T, NP>::directionV() const
{
	return directionV_;
}



/** return reciprocal vectors for U and V direction vectors
 */
template<typename T, class NP>
void Plane3DCombined<T, NP>::getReciprocals(TVector& oReciprocalU,
											TVector& oReciprocalV) const
{
	oReciprocalU = reciprocalU_;
	oReciprocalV = reciprocalV_;
}



/** return reciprocal for U direction vector.
 */
template<typename T, class NP>
const typename Plane3DCombined<T, NP>::TVector& Plane3DCombined<T, NP>::reciprocalU() const
{
	return reciprocalU_;
}



/** return reciprocal for V direction vector.
 */
template<typename T, class NP>
const typename Plane3DCombined<T, NP>::TVector& Plane3DCombined<T, NP>::reciprocalV() const
{
	return reciprocalV_;
}



template<typename T, class NP>
void Plane3DCombined<T, NP>::getCartesian(TVector& oNormal, TReference oD) const
{
	oNormal = normal_;
	oD = d_;
}



template<typename T, class NP>
const typename Plane3DCombined<T, NP>::TVector& Plane3DCombined<T, NP>::normal() const
{
	return normal_;
}



template<typename T, class NP>
const typename Plane3DCombined<T, NP>::TParam Plane3DCombined<T, NP>::d() const
{
	return d_;
}



/** Return on what side a point is located.
 */
template<typename T, class NP>
const Side Plane3DCombined<T, NP>::classify(const TPoint& iPoint) const
{
	const TValue eq = equation(iPoint);
	return eq > TNumTraits::zero ? sFront : (eq < TNumTraits::zero ? sBack : sSurface);
}



/** Return value of point in equation.
 */
template<typename T, class NP>
const typename Plane3DCombined<T, NP>::TValue
Plane3DCombined<T, NP>::equation(const TPoint& iPoint) const
{
	return dot(iPoint.position(), normal_) + d_;
}



/** Return signed distance of point to plane.
 *  negative value means point is in the back.
 */
template<typename T, class NP>
const typename Plane3DCombined<T, NP>::TValue
Plane3DCombined<T, NP>::signedDistance(const TPoint& iPoint) const
{
	return NP::divideByNorm(equation(iPoint), normal_);
}



/** Return squared distance of point to plane.
 */
template<typename T, class NP>
const typename Plane3DCombined<T, NP>::TValue
Plane3DCombined<T, NP>::squaredDistance(const TPoint& iPoint) const
{
	return num::sqr(signedDistance(iPoint));
}



/** return the vector that, if added to the PROJECTION of iPoint, you get iPoint again.
 *  iPoint == (almost) project(iPoint) + reject(iPoint)
 */
template<typename T, class NP>
const typename Plane3DCombined<T, NP>::TVector
Plane3DCombined<T, NP>::reject(const TPoint& iPoint) const
{
	return normal_ * NP::divideBySquaredNorm(equation(iPoint), normal_);
}



/** return the part of iVector that is orthogonal to the plane.
 *  it's the vector that, if added to the PROJECTION of iVector, you get iVector again.
 *  iVector == (almost) project(iVector) + reject(iVector).
 */
template<typename T, class NP>
const typename Plane3DCombined<T, NP>::TVector
Plane3DCombined<T, NP>::reject(const TVector& iVector) const
{
	return normal_ * NP::divideBySquaredNorm(dot(normal_, iVector), normal_);
}



/** project a point orthogonally onto the plane
 */
template<typename T, class NP>
const typename Plane3DCombined<T, NP>::TPoint
Plane3DCombined<T, NP>::project(const TPoint& iPoint) const
{
	return iPoint - reject(iPoint);
}



/** project a vector orthogonally onto the plane
 */
template<typename T, class NP>
const typename Plane3DCombined<T, NP>::TVector
Plane3DCombined<T, NP>::project(const TVector& iVector) const
{
	return iVector - reject(iVector);
}



/** reflect a point orthogonally into the plane.
 */
template<typename T, class NP>
const typename Plane3DCombined<T, NP>::TPoint
Plane3DCombined<T, NP>::reflect(const TPoint& iPoint) const
{
	return iPoint - 2 * reject(iPoint);
}



/** reflect a vector orthogonally into the plane
 */
template<typename T, class NP>
const typename Plane3DCombined<T, NP>::TVector
Plane3DCombined<T, NP>::reflect(const TVector& iVector) const
{
	return iVector - 2 * reject(iVector);
}



/** return point by filling in the parametric equation: P(u, v) = S + u * U + v * V
 */
template<typename T, class NP>
const typename Plane3DCombined<T, NP>::TPoint
Plane3DCombined<T, NP>::point(TParam iU, TParam iV) const
{
	return point(TIndex(iU, iV));
}



/** return point by filling in the parametric equation: P(u, v) = S + u * U + v * V
 */
template<typename T, class NP>
const typename Plane3DCombined<T, NP>::TPoint
Plane3DCombined<T, NP>::point(const TUV& iUV) const
{
	return support_ + iUV.x * directionU_ + iUV.y * directionV_;
}



/** return UV pair of parameters
 */
template<typename T, class NP>
const typename Plane3DCombined<T, NP>::TUV
Plane3DCombined<T, NP>::uv(const TPoint& iPoint) const
{
	const TVector relative = iPoint - support_;
	return TUV(dot(relative, reciprocalU_), dot(relative, reciprocalV_));
}



template <typename T, class NP>
void Plane3DCombined<T, NP>::flip()
{
	directionV_ = -directionV_;
	normal_ = -normal_;
	d_ = -d_;
	Plane3DImplDetail::generateReciprocal(directionU_, directionV_, reciprocalU_, reciprocalV_);
}



/** return true if plane is a valid plane (no normal or direction vectors that are zero).
 */
template<typename T, class NP>
const bool Plane3DCombined<T, NP>::isValid() const
{
	return !normal_.isZero() && !cross(directionU_, directionV_).isZero();
}




// --- protected -----------------------------------------------------------------------------------



// --- private -------------------------------------------------------------------------------------



// --- free ----------------------------------------------------------------------------------------

template<typename T, class NP>
std::ostream& operator<<(std::ostream& ioOStream, const Plane3DCombined<T, NP>& iPlane)
{
	LASS_ENFORCE(ioOStream) << "{S=" << iPlane.support() << ", U=" << iPlane.directionU() << ", V="
		<< iPlane.directionV() << ", N=" << iPlane.normal() << ", d=" << iPlane.d() << "}";
	return ioOStream;
}



}

}

}

#endif

// EOF
