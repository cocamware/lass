/**	@file
 *	@author Bram de Greve (bramz@users.sourceforge.net)
 *	@author Tom De Muer (tomdemuer@users.sourceforge.net)
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
 *	Copyright (C) 2004-2007 the Initial Developer.
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



#ifndef LASS_GUARDIAN_OF_INCLUSION_PRIM_IMPL_PLANE_3D_PARAMETRIC_INL
#define LASS_GUARDIAN_OF_INCLUSION_PRIM_IMPL_PLANE_3D_PARAMETRIC_INL

#include "../prim_common.h"
#include "plane_3d_parametric.h"

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
Plane3DParametric<T, NP>::Plane3DParametric():
	support_(),
	directionU_(),
	directionV_()
{
	LASS_ASSERT(!isValid());
}



/** Construct a plane through three points.
 *  - support point S is given by the first point iSupport.
 *  - direction vectors U and V are choosen from iSupport to iPointU and iPointV respectively
 *    (U = iPointU - iSupport, V = iPointV - iSupport).
 */
template<typename T, class NP>
Plane3DParametric<T, NP>::Plane3DParametric(const TPoint& iSupport,
										const TPoint& iPointU,
										const TPoint& iPointV):
	support_(iSupport),
	directionU_(iPointU - iSupport),
	directionV_(iPointV - iSupport)
{
	NP::normalize(directionU_);
	NP::normalize(directionV_);
}



/** construct a plane through a support point and by two direction vectors..
 *  - support point S is given by the point iSupport.
 *  - direction vectors U and V are given by iDirectionU and iDirectionV.
 */
template<typename T, class NP>
Plane3DParametric<T, NP>::Plane3DParametric(const TPoint& iSupport,
										const TVector& iDirectionU,
										const TVector& iDirectionV):
	support_(iSupport),
	directionU_(iDirectionU),
	directionV_(iDirectionV)
{
	NP::normalize(directionU_);
	NP::normalize(directionV_);
}



/** Construct a plane through a support point and by a normal vector.
 *  - support point S is given by the point iSupport.
 *  - direction vectors U and V are automatically generated so that N == U x V.
 */
template<typename T, class NP>
Plane3DParametric<T, NP>::Plane3DParametric(const TVector& iNormal, const TPoint& iSupport):
	support_(iSupport)
{
	Plane3DImplDetail::generateDirections(iNormal, directionU_, directionV_);
	NP::normalize(directionU_);
	NP::normalize(directionV_);
}



/** Construct a plane by a cartesian equation N.P + d == 0.
 *  - support point S automatically generated so that N.S + d == 0.
 *  - direction vectors U and V are automatically generated so that N == U x V.
 */
template<typename T, class NP>
Plane3DParametric<T, NP>::Plane3DParametric(const TVector& iNormal, TParam iD):
	support_(iNormal * (-iD / iNormal.squaredNorm()))
{
	Plane3DImplDetail::generateDirections(iNormal, directionU_, directionV_);
	NP::normalize(directionU_);
	NP::normalize(directionV_);
}



/** return support point.
 */
template<typename T, class NP>
const typename Plane3DParametric<T, NP>::TPoint& Plane3DParametric<T, NP>::support() const
{
	return support_;
}



/** return U and V direction vectors
 */
template<typename T, class NP>
void Plane3DParametric<T, NP>::getDirections(TVector& oDirectionU, TVector& oDirectionV) const
{
	oDirectionU = directionU_;
	oDirectionV = directionV_;
}



/** return U direction vector.
 */
template<typename T, class NP>
const typename Plane3DParametric<T, NP>::TVector& Plane3DParametric<T, NP>::directionU() const
{
	return directionU_;
}



/** return V direction vector.
 */
template<typename T, class NP>
const typename Plane3DParametric<T, NP>::TVector& Plane3DParametric<T, NP>::directionV() const
{
	return directionV_;
}



/** return reciprocal vectors for U and V direction vectors
 */
template<typename T, class NP>
void Plane3DParametric<T, NP>::getReciprocals(TVector& oReciprocalU,
											  TVector& oReciprocalV) const
{
	Plane3DImplDetail::generateReciprocal(directionU_, directionV_, oReciprocalU, oReciprocalV);
}



/** return reciprocal for U direction vector.
 */
template<typename T, class NP>
const typename Plane3DParametric<T, NP>::TVector Plane3DParametric<T, NP>::reciprocalU() const
{
	TVector reciprocalU;
	TVector reciprocalV;
	getReciprocals(reciprocalU, reciprocalV);
	return reciprocalU;
}



/** return reciprocal for V direction vector.
 */
template<typename T, class NP>
const typename Plane3DParametric<T, NP>::TVector Plane3DParametric<T, NP>::reciprocalV() const
{
	TVector reciprocalU;
	TVector reciprocalV;
	getReciprocals(reciprocalU, reciprocalV);
	return reciprocalV;
}



template<typename T, class NP>
void Plane3DParametric<T, NP>::getCartesian(TVector& oNormal, TReference oD) const
{
	Plane3DImplDetail::generateCartesian(support_, directionU_, directionV_, oNormal, oD);
	NP::normalizeAndScale(oNormal, oD);
}



template<typename T, class NP>
const typename Plane3DParametric<T, NP>::TVector Plane3DParametric<T, NP>::normal() const
{
	TVector normal;
	TValue d;
	getCartesian(normal, d);
	return normal;
}



template<typename T, class NP>
const typename Plane3DParametric<T, NP>::TValue Plane3DParametric<T, NP>::d() const
{
	TVector normal;
	TValue d;
	getCartesian(normal, d);
	return d;
}



/** Return value of point in equation.
 */
template<typename T, class NP>
const typename Plane3DParametric<T, NP>::TValue
Plane3DParametric<T, NP>::equation(const TPoint& iPoint) const
{
	TVector normal;
	TValue d;
	getCartesian(normal, d);
	return dot(iPoint.position(), normal) + d;
}



/** Return value of point in equation.
 */
template<typename T, class NP>
const typename Plane3DParametric<T, NP>::TValue
Plane3DParametric<T, NP>::equation(const TPoint& iPoint, TParam iRelativeTolerance) const
{
	TVector normal;
	TValue d;
	getCartesian(normal, d);
	const TValue a = dot(iPoint.position(), normal);
	return almostEqual(a, -d, iRelativeTolerance) ? TNumTraits::zero : (a + d);
}



/** return the vector that, if added to the PROJECTION of iPoint, you get iPoint again.
 *  iPoint == (almost) project(iPoint) + reject(iPoint)
 */
template<typename T, class NP>
const typename Plane3DParametric<T, NP>::TVector
Plane3DParametric<T, NP>::reject(const TPoint& iPoint) const
{
	TVector normal;
	TValue d;
	getCartesian(normal, d);
	return normal * NP::divideBySquaredNorm(dot(iPoint.position(), normal) + d, normal);
}



/** return the part of iVector that is orthogonal to the plane.
 *  it's the vector that, if added to the PROJECTION of iVector, you get iVector again.
 *  iVector == (almost) project(iVector) + reject(iVector).
 */
template<typename T, class NP>
const typename Plane3DParametric<T, NP>::TVector
Plane3DParametric<T, NP>::reject(const TVector& iVector) const
{
	TVector normal;
	TValue d;
	getCartesian(normal, d);
	return normal * NP::divideBySquaredNorm(dot(iVector, normal), normal);
}



/** project a point orthogonally onto the plane
 */
template<typename T, class NP>
const typename Plane3DParametric<T, NP>::TPoint
Plane3DParametric<T, NP>::project(const TPoint& iPoint) const
{
	return iPoint - reject(iPoint);
}



/** project a vector orthogonally onto the plane
 */
template<typename T, class NP>
const typename Plane3DParametric<T, NP>::TVector
Plane3DParametric<T, NP>::project(const TVector& iVector) const
{
	return iVector - reject(iVector);
}



/** reflect a point orthogonally into the plane.
 */
template<typename T, class NP>
const typename Plane3DParametric<T, NP>::TPoint
Plane3DParametric<T, NP>::reflect(const TPoint& iPoint) const
{
	return iPoint - 2 * reject(iPoint);
}



/** reflect a vector orthogonally into the plane
 */
template<typename T, class NP>
const typename Plane3DParametric<T, NP>::TVector
Plane3DParametric<T, NP>::reflect(const TVector& iVector) const
{
	return iVector - 2 * reject(iVector);
}



/** return point by filling in the parametric equation: P(u, v) = S + u * U + v * V
 */
template<typename T, class NP>
const typename Plane3DParametric<T, NP>::TPoint
Plane3DParametric<T, NP>::point(TParam iU, TParam iV) const
{
	return point(TIndex(iU, iV));
}



/** return point by filling in the parametric equation: P(u, v) = S + u * U + v * V
 */
template<typename T, class NP>
const typename Plane3DParametric<T, NP>::TPoint
Plane3DParametric<T, NP>::point(const TUV& iUV) const
{
	return support_ + iUV.x * directionU_ + iUV.y * directionV_;
}



/** return UV pair of parameters
 */
template<typename T, class NP>
const typename Plane3DParametric<T, NP>::TUV
Plane3DParametric<T, NP>::uv(const TPoint& iPoint) const
{
	TVector reciprocalU;
	TVector reciprocalV;
	getReciprocals(reciprocalU, reciprocalV);
	const TVector relative = iPoint - support_;
	return TUV(dot(relative, reciprocalU), dot(relative, reciprocalV));
}



template <typename T, class NP>
void Plane3DParametric<T, NP>::flip()
{
	directionV_ = -directionV_;
}



/** return true if plane is a valid plane (no direction vector is zero and they're not
 *  colinear.
 */
template<typename T, class NP>
const bool Plane3DParametric<T, NP>::isValid() const
{
	return !cross(directionU_, directionV_).isZero(); // zero vectors will give zero cross product too.
}




// --- protected -----------------------------------------------------------------------------------



// --- private -------------------------------------------------------------------------------------



// --- free ----------------------------------------------------------------------------------------

template<typename T, class NP>
std::ostream& operator<<(std::ostream& ioOStream, const Plane3DParametric<T, NP>& iPlane)
{
	LASS_ENFORCE(ioOStream) << "{S=" << iPlane.support() << ", U=" << iPlane.directionU() << ", V="
		<< iPlane.directionV() << "}";
	return ioOStream;
}



}

}

}

#endif

// EOF
