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
 *	Copyright (C) 2004-2011 the Initial Developer.
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



#ifndef LASS_GUARDIAN_OF_INCLUSION_PRIM_IMPL_PLANE_3D_CARTESIAN_INL
#define LASS_GUARDIAN_OF_INCLUSION_PRIM_IMPL_PLANE_3D_CARTESIAN_INL

#include "../prim_common.h"
#include "plane_3d_cartesian.h"

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
Plane3DCartesian<T, NP>::Plane3DCartesian():
	normal_(),
	d_(TNumTraits::zero)
{
	LASS_ASSERT(!isValid());
}



/** Construct a plane through three points.
 *  - normal vector N is given by the cross product N = U x V.
 *  - value d is choosen so that the support point is indeed a point of the plane.
 */
template<typename T, class NP>
Plane3DCartesian<T, NP>::Plane3DCartesian(const TPoint& iSupport,
										  const TPoint& iPointU,
										  const TPoint& iPointV)
{
	Plane3DImplDetail::generateCartesian(iSupport, iPointU - iSupport, iPointV - iSupport,
										 normal_, d_);
	NP::normalizeAndScale(normal_, d_);
}



/** construct a plane through a support point and by two direction vectors..
 *  - normal vector N is given by the cross product N = U x V.
 *  - value d is choosen so that the support point is indeed a point of the plane.
 */
template<typename T, class NP>
Plane3DCartesian<T, NP>::Plane3DCartesian(const TPoint& iSupport,
										  const TVector& iDirectionU,
										  const TVector& iDirectionV)
{
	Plane3DImplDetail::generateCartesian(iSupport, iDirectionU, iDirectionV, normal_, d_);
	NP::normalizeAndScale(normal_, d_);
}



/** Construct a plane through a support point and by a normal vector.
 *  - normal vector N is given by the vector iNormal.
 *  - value d is choosen so that the support point is indeed a point of the plane.
 */
template<typename T, class NP>
Plane3DCartesian<T, NP>::Plane3DCartesian(const TVector& iNormal, const TPoint& iSupport):
	normal_(iNormal),
	d_(-dot(iNormal, iSupport.position()))
{
	NP::normalizeAndScale(normal_, d_);
}



/** Construct a plane by a cartesian equation N.P + d == 0.
 *  - normal vector N is given by the vector iNormal.
 *  - value d is given by the value iD.
 */
template<typename T, class NP>
Plane3DCartesian<T, NP>::Plane3DCartesian(const TVector& iNormal, TParam iD):
	normal_(iNormal),
	d_(iD)
{
	NP::normalizeAndScale(normal_, d_);
}



/** return support point.
 */
template<typename T, class NP>
const typename Plane3DCartesian<T, NP>::TPoint Plane3DCartesian<T, NP>::support() const
{
	return TPoint(-d_ * normal_);
}



/** return U and V direction vectors
 */
template<typename T, class NP>
void Plane3DCartesian<T, NP>::getDirections(TVector& oDirectionU, TVector& oDirectionV) const
{
	Plane3DImplDetail::generateDirections(normal_, oDirectionU, oDirectionV);
	NP::normalize(oDirectionU);
	NP::normalize(oDirectionV);
}



/** return U direction vector.
 */
template<typename T, class NP>
const typename Plane3DCartesian<T, NP>::TVector Plane3DCartesian<T, NP>::directionU() const
{
	TVector directionU;
	TVector directionV;
	getDirections(directionU, directionV);
	return directionU;
}



/** return V direction vector.
 */
template<typename T, class NP>
const typename Plane3DCartesian<T, NP>::TVector Plane3DCartesian<T, NP>::directionV() const
{
	TVector directionU;
	TVector directionV;
	getDirections(directionU, directionV);
	return directionV;
}



/** return reciprocal vectors for U and V direction vectors
 */
template<typename T, class NP>
void Plane3DCartesian<T, NP>::getReciprocals(TVector& oReciprocalU,
											 TVector& oReciprocalV) const
{
	TVector directionU;
	TVector directionV;
	getDirections(directionU, directionV);
	Plane3DImplDetail::generateReciprocal(directionU, directionV, oReciprocalU, oReciprocalV);
}



/** return reciprocal for U direction vector.
 */
template<typename T, class NP>
const typename Plane3DCartesian<T, NP>::TVector Plane3DCartesian<T, NP>::reciprocalU() const
{
	TVector reciprocalU;
	TVector reciprocalV;
	getReciprocals(reciprocalU, reciprocalV);
	return reciprocalU;
}



/** return reciprocal for V direction vector.
 */
template<typename T, class NP>
const typename Plane3DCartesian<T, NP>::TVector Plane3DCartesian<T, NP>::reciprocalV() const
{
	TVector reciprocalU;
	TVector reciprocalV;
	getReciprocals(reciprocalU, reciprocalV);
	return reciprocalV;
}



template<typename T, class NP>
void Plane3DCartesian<T, NP>::getCartesian(TVector& oNormal, TReference oD) const
{
	oNormal = normal_;
	oD = d_;
}



template<typename T, class NP>
const typename Plane3DCartesian<T, NP>::TVector& Plane3DCartesian<T, NP>::normal() const
{
	return normal_;
}



template<typename T, class NP>
const typename Plane3DCartesian<T, NP>::TParam Plane3DCartesian<T, NP>::d() const
{
	return d_;
}



/** Return value of point in equation.
 */
template<typename T, class NP>
const typename Plane3DCartesian<T, NP>::TValue
Plane3DCartesian<T, NP>::equation(const TPoint& iPoint) const
{
	return dot(iPoint.position(), normal_) + d_;
}



/** Return value of point in equation.
 */
template<typename T, class NP>
const typename Plane3DCartesian<T, NP>::TValue
Plane3DCartesian<T, NP>::equation(const TPoint& iPoint, TParam iRelativeTolerance) const
{
	const TValue a = dot(iPoint.position(), normal_);
	return num::almostEqual(a, -d_, iRelativeTolerance) ? TNumTraits::zero : (a + d_);
}



/** return the vector that, if added to the PROJECTION of iPoint, you get iPoint again.
 *  iPoint == (almost) project(iPoint) + reject(iPoint)
 */
template<typename T, class NP>
const typename Plane3DCartesian<T, NP>::TVector
Plane3DCartesian<T, NP>::reject(const TPoint& iPoint) const
{
	return normal_ * NP::divideBySquaredNorm(equation(iPoint), normal_);
}



/** return the part of iVector that is orthogonal to the plane.
 *  it's the vector that, if added to the PROJECTION of iVector, you get iVector again.
 *  iVector == (almost) project(iVector) + reject(iVector).
 */
template<typename T, class NP>
const typename Plane3DCartesian<T, NP>::TVector
Plane3DCartesian<T, NP>::reject(const TVector& iVector) const
{
	return normal_ * NP::divideBySquaredNorm(dot(normal_, iVector), normal_);
}



/** project a point orthogonally onto the plane
 */
template<typename T, class NP>
const typename Plane3DCartesian<T, NP>::TPoint
Plane3DCartesian<T, NP>::project(const TPoint& iPoint) const
{
	return iPoint - reject(iPoint);
}



/** project a vector orthogonally onto the plane
 */
template<typename T, class NP>
const typename Plane3DCartesian<T, NP>::TVector
Plane3DCartesian<T, NP>::project(const TVector& iVector) const
{
	return iVector - reject(iVector);
}



/** reflect a point orthogonally into the plane.
 */
template<typename T, class NP>
const typename Plane3DCartesian<T, NP>::TPoint
Plane3DCartesian<T, NP>::reflect(const TPoint& iPoint) const
{
	return iPoint - 2 * reject(iPoint);
}



/** reflect a vector orthogonally into the plane
 */
template<typename T, class NP>
const typename Plane3DCartesian<T, NP>::TVector
Plane3DCartesian<T, NP>::reflect(const TVector& iVector) const
{
	return iVector - 2 * reject(iVector);
}



/** return point by filling in the parametric equation: P(u, v) = S + u * U + v * V
 */
template<typename T, class NP>
const typename Plane3DCartesian<T, NP>::TPoint
Plane3DCartesian<T, NP>::point(TParam iU, TParam iV) const
{
	return point(TIndex(iU, iV));
}



/** return point by filling in the parametric equation: P(u, v) = S + u * U + v * V
 */
template<typename T, class NP>
const typename Plane3DCartesian<T, NP>::TPoint
Plane3DCartesian<T, NP>::point(const TUV& iUV) const
{
	TVector directionU;
	TVector directionV;
	getDirections(directionU, directionV);
	return support() + iUV.x * directionU + iUV.y * directionV;
}



/** return UV pair of parameters
 */
template<typename T, class NP>
const typename Plane3DCartesian<T, NP>::TUV
Plane3DCartesian<T, NP>::uv(const TPoint& iPoint) const
{
	TVector reciprocalU;
	TVector reciprocalV;
	getReciprocals(reciprocalU, reciprocalV);
	const TVector relative = iPoint - support();
	return TUV(dot(relative, reciprocalU), dot(relative, reciprocalV));
}



template <typename T, class NP>
void Plane3DCartesian<T, NP>::flip()
{
	normal_ = -normal_;
	d_ = -d_;
}



/** return true if plane is a valid plane (no normal or direction vectors that are zero).
 */
template<typename T, class NP>
bool Plane3DCartesian<T, NP>::isValid() const
{
	return !normal_.isZero();
}




// --- protected -----------------------------------------------------------------------------------



// --- private -------------------------------------------------------------------------------------



// --- free ----------------------------------------------------------------------------------------

template<typename T, class NP>
std::ostream& operator<<(std::ostream& ioOStream, const Plane3DCartesian<T, NP>& iPlane)
{
	LASS_ENFORCE(ioOStream) << "{N=" << iPlane.normal() << ", d=" << iPlane.d() << "}";
	return ioOStream;
}



}

}

}

#endif

// EOF
