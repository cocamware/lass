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
 *	Copyright (C) 2004-2009 the Initial Developer.
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



#ifndef LASS_GUARDIAN_OF_INCLUSION_PRIM_IMPL_LINE_2D_COMBINED_INL
#define LASS_GUARDIAN_OF_INCLUSION_PRIM_IMPL_LINE_2D_COMBINED_INL

#include "../prim_common.h"
#include "line_2d_combined.h"

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
Line2DCombined<T, NP>::Line2DCombined():
	support_(),
	direction_(),
	normal_(),
	d_(TNumTraits::zero)
{
	LASS_ASSERT(!isValid());
}



/** Construct a line through three points.
 *  - support point S is given by the first point iSupport.
 *  - direction vector U is choosen from iSupport to iPointU and iPointV respectively
 *    (U = iPoint - iSupport).
 *  - normal vector N is given by the perp of U.
 *  - value d is choosen so that the support point is indeed a point of the line.
 */
template<typename T, class NP>
Line2DCombined<T, NP>::Line2DCombined(const TPoint& iSupport, const TPoint& iPoint):
	support_(iSupport),
	direction_(iPoint - iSupport)
{
	NP::normalize(direction_);
	normal_ = direction_.perp();
	d_ = -dot(normal_, iSupport.position());
}



/** construct a line through a support point and by two direction vectors..
 *  - support point S is given by the point iSupport.
 *  - direction vector U is given by iDirection.
 *  - normal vector N is given by the perp of N.
 *  - value d is choosen so that the support point is indeed a point of the line.
 */
template<typename T, class NP>
Line2DCombined<T, NP>::Line2DCombined(const TPoint& iSupport, const TVector& iDirection):
	support_(iSupport),
	direction_(iDirection)
{
	NP::normalize(direction_);
	normal_ = direction_.perp();
	d_ = -dot(normal_, iSupport.position());
}



/** Construct a line through a support point and by a normal vector.
 *  - support point S is given by the point iSupport.
 *  - normal vector N is given by the vector iNormal.
 *  - direction vector U is automatically generated so that N is perp of U.
 *  - value d is choosen so that the support point is indeed a point of the line.
 */
template<typename T, class NP>
Line2DCombined<T, NP>::Line2DCombined(const TVector& iNormal, const TPoint& iSupport):
	support_(iSupport),
	normal_(iNormal)
{
	NP::normalize(normal_);
	direction_ = -normal_.perp();
	d_ = -dot(normal_, iSupport.position());
}



/** Construct a line by a cartesian equation N.P + d == 0.
 *  - normal vector N is given by the vector iNormal.
 *  - value d is given by the value iD.
 *  - support point S automatically generated so that N.S + d == 0.
 *  - direction vector U is automatically generated so that N is perp of U.
 */
template<typename T, class NP>
Line2DCombined<T, NP>::Line2DCombined(const TVector& iNormal, TParam iD):
	support_(iNormal * (-iD / iNormal.squaredNorm())),
	normal_(iNormal),
	d_(iD)
{
	NP::normalizeAndScale(normal_, d_);
	direction_ = -normal_.perp();
}



/** return support point.
 */
template<typename T, class NP>
const typename Line2DCombined<T, NP>::TPoint& Line2DCombined<T, NP>::support() const
{
	return support_;
}



/** return direction vector.
 */
template<typename T, class NP>
const typename Line2DCombined<T, NP>::TVector& Line2DCombined<T, NP>::direction() const
{
	return direction_;
}



template<typename T, class NP>
void Line2DCombined<T, NP>::getCartesian(TVector& oNormal, TReference oD) const
{
	oNormal = normal_;
	oD = d_;
}



template<typename T, class NP>
const typename Line2DCombined<T, NP>::TVector& Line2DCombined<T, NP>::normal() const
{
	return normal_;
}



template<typename T, class NP>
const typename Line2DCombined<T, NP>::TParam Line2DCombined<T, NP>::d() const
{
	return d_;
}



/** Return value of point in equation.
 */
template<typename T, class NP>
const typename Line2DCombined<T, NP>::TValue
Line2DCombined<T, NP>::equation(const TPoint& iPoint) const
{
	return dot(iPoint.position(), normal_) + d_;
}



/** Return value of point in equation.
 */
template<typename T, class NP>
const typename Line2DCombined<T, NP>::TValue
Line2DCombined<T, NP>::equation(const TPoint& iPoint, TParam iRelativeTolerance) const
{
	const TValue d = dot(iPoint.position(), normal_);
	return num::almostEqual(d, -d_, iRelativeTolerance) ? TNumTraits::zero : (d + d_);
}



/** return the vector that, if added to the PROJECTION of iPoint, you get iPoint again.
 *  iPoint == (almost) project(iPoint) + reject(iPoint)
 */
template<typename T, class NP>
const typename Line2DCombined<T, NP>::TVector
Line2DCombined<T, NP>::reject(const TPoint& iPoint) const
{
	return normal_ * NP::divideBySquaredNorm(equation(iPoint), normal_);
}



/** return the part of iVector that is orthogonal to the line.
 *  it's the vector that, if added to the PROJECTION of iVector, you get iVector again.
 *  iVector == (almost) project(iVector) + reject(iVector).
 */
template<typename T, class NP>
const typename Line2DCombined<T, NP>::TVector
Line2DCombined<T, NP>::reject(const TVector& iVector) const
{
	return normal_ * NP::divideBySquaredNorm(dot(normal_, iVector), normal_);
}



/** project a point orthogonally onto the line
 */
template<typename T, class NP>
const typename Line2DCombined<T, NP>::TPoint
Line2DCombined<T, NP>::project(const TPoint& iPoint) const
{
	return iPoint - reject(iPoint);
}



/** project a vector orthogonally onto the line
 */
template<typename T, class NP>
const typename Line2DCombined<T, NP>::TVector
Line2DCombined<T, NP>::project(const TVector& iVector) const
{
	return iVector - reject(iVector);
}



/** reflect a point orthogonally into the line.
 */
template<typename T, class NP>
const typename Line2DCombined<T, NP>::TPoint
Line2DCombined<T, NP>::reflect(const TPoint& iPoint) const
{
	return support_ + reflect(iPoint - support_);
}



/** reflect a vector orthogonally into the line
 */
template<typename T, class NP>
const typename Line2DCombined<T, NP>::TVector
Line2DCombined<T, NP>::reflect(const TVector& iVector) const
{
	return iVector - T(2) * reject(iVector);
}



/** return point by filling in the parametric equation: P(t) = S + t * U
 */
template<typename T, class NP>
const typename Line2DCombined<T, NP>::TPoint
Line2DCombined<T, NP>::point(TParam iT) const
{
	return support_ + iT * direction_;
}



/** return UV pair of parameters
 */
template<typename T, class NP>
const typename Line2DCombined<T, NP>::TValue
Line2DCombined<T, NP>::t(const TPoint& iPoint) const
{
	return NP::divideBySquaredNorm(dot(direction_, iPoint - support_), direction_);
}



template <typename T, class NP>
void Line2DCombined<T, NP>::flip()
{
	direction_ = -direction_;
	normal_ = -normal_;
	d_ = -d_;
}



/** return true if line is a valid line (no normal or direction vectors that are zero).
 */
template<typename T, class NP>
bool Line2DCombined<T, NP>::isValid() const
{
	return !normal_.isZero() && !direction_.isZero();
}




// --- protected -----------------------------------------------------------------------------------



// --- private -------------------------------------------------------------------------------------



// --- free ----------------------------------------------------------------------------------------

template<typename T, class NP>
std::ostream& operator<<(std::ostream& ioOStream, const Line2DCombined<T, NP>& iLine)
{
	LASS_ENFORCE(ioOStream) << "{S=" << iLine.support() << ", D=" << iLine.direction()
		<< ", N=" << iLine.normal() << ", d=" << iLine.d() << "}";
	return ioOStream;
}



}

}

}

#endif

// EOF
