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



#ifndef LASS_GUARDIAN_OF_INCLUSION_PRIM_IMPL_LINE_2D_PARAMETRIC_INL
#define LASS_GUARDIAN_OF_INCLUSION_PRIM_IMPL_LINE_2D_PARAMETRIC_INL

#include "../prim_common.h"
#include "line_2d_parametric.h"

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
Line2DParametric<T, NP>::Line2DParametric():
	support_(),
	direction_()
{
	LASS_ASSERT(!isValid());
}



/** Construct a line through three points.
 *  - support point S is given by the first point iSupport.
 *  - direction vector U is choosen from iSupport to iPointU and iPointV respectively
 *    (U = iPoint - iSupport).
 */
template<typename T, class NP>
Line2DParametric<T, NP>::Line2DParametric(const TPoint& iSupport, const TPoint& iPoint):
	support_(iSupport),
	direction_(iPoint - iSupport)
{
	NP::normalize(direction_);
}



/** construct a line through a support point and by two direction vectors..
 *  - support point S is given by the point iSupport.
 *  - direction vector U is given by iDirection.
 */
template<typename T, class NP>
Line2DParametric<T, NP>::Line2DParametric(const TPoint& iSupport, const TVector& iDirection):
	support_(iSupport),
	direction_(iDirection)
{
	NP::normalize(direction_);
}



/** Construct a line through a support point and by a normal vector.
 *  - support point S is given by the point iSupport.
 *  - direction vector U is automatically generated so that iNormal is perp of U.
 */
template<typename T, class NP>
Line2DParametric<T, NP>::Line2DParametric(const TVector& iNormal, const TPoint& iSupport):
	support_(iSupport),
	direction_(-iNormal.perp())
{
	NP::normalize(direction_);
}



/** Construct a line by a cartesian equation N.P + d == 0.
 *  - support point S automatically generated so that iNormal.S + iD == 0.
 *  - direction vector U is automatically generated so that iNormal is perp of U.
 */
template<typename T, class NP>
Line2DParametric<T, NP>::Line2DParametric(const TVector& iNormal, TParam iD):
	support_(iNormal * (-iD / iNormal.squaredNorm())),
	direction_(-iNormal.perp())
{
	NP::normalize(direction_);
}



/** return support point.
 */
template<typename T, class NP>
const typename Line2DParametric<T, NP>::TPoint& Line2DParametric<T, NP>::support() const
{
	return support_;
}



/** return direction vector.
 */
template<typename T, class NP>
const typename Line2DParametric<T, NP>::TVector& Line2DParametric<T, NP>::direction() const
{
	return direction_;
}



template<typename T, class NP>
void Line2DParametric<T, NP>::getCartesian(TVector& oNormal, TReference oD) const
{
	oNormal = direction_.perp();
	oD = -dot(oNormal, support_.position());
}



template<typename T, class NP>
const typename Line2DParametric<T, NP>::TVector Line2DParametric<T, NP>::normal() const
{
	TVector normal;
	TValue d;
	getCartesian(normal, d);
	return normal;
}



template<typename T, class NP>
const typename Line2DParametric<T, NP>::TValue Line2DParametric<T, NP>::d() const
{
	TVector normal;
	TValue d;
	getCartesian(normal, d);
	return d;
}



/** Return value of point in equation.
 */
template<typename T, class NP>
const typename Line2DParametric<T, NP>::TValue
Line2DParametric<T, NP>::equation(const TPoint& iPoint) const
{
	TVector normal;
	TValue d;
	getCartesian(normal, d);
	return dot(iPoint.position(), normal) + d;
}



/** Return value of point in equation.
 */
template<typename T, class NP>
const typename Line2DParametric<T, NP>::TValue
Line2DParametric<T, NP>::equation(const TPoint& iPoint, TParam iRelativeTolerance) const
{
	TVector normal;
	TValue d;
	getCartesian(normal, d);
	const TValue pn = dot(iPoint.position(), normal);
	return num::almostEqual(pn, -d, iRelativeTolerance) ? TNumTraits::zero : (pn + d);
}



/** return the vector that, if added to the PROJECTION of iPoint, you get iPoint again.
 *  iPoint == (almost) project(iPoint) + reject(iPoint)
 */
template<typename T, class NP>
const typename Line2DParametric<T, NP>::TVector
Line2DParametric<T, NP>::reject(const TPoint& iPoint) const
{
	return reject(iPoint - support_);
}



/** return the part of iVector that is orthogonal to the line.
 *  it's the vector that, if added to the PROJECTION of iVector, you get iVector again.
 *  iVector == (almost) project(iVector) + reject(iVector).
 */
template<typename T, class NP>
const typename Line2DParametric<T, NP>::TVector
Line2DParametric<T, NP>::reject(const TVector& iVector) const
{

	return iVector - project(iVector);
}



/** project a point orthogonally onto the line
 */
template<typename T, class NP>
const typename Line2DParametric<T, NP>::TPoint
Line2DParametric<T, NP>::project(const TPoint& iPoint) const
{
	return support_ + project(iPoint - support_);
}



/** project a vector orthogonally onto the line
 */
template<typename T, class NP>
const typename Line2DParametric<T, NP>::TVector
Line2DParametric<T, NP>::project(const TVector& iVector) const
{
	return direction_ * NP::divideBySquaredNorm(dot(iVector, direction_), direction_);
}



/** reflect a point orthogonally into the line.
 */
template<typename T, class NP>
const typename Line2DParametric<T, NP>::TPoint
Line2DParametric<T, NP>::reflect(const TPoint& iPoint) const
{
	return support_ + reflect(iPoint - support_);
}



/** reflect a vector orthogonally to the line
 */
template<typename T, class NP>
const typename Line2DParametric<T, NP>::TVector
Line2DParametric<T, NP>::reflect(const TVector& iVector) const
{
	return T(2) * project(iVector) - iVector;
}



/** return point by filling in the parametric equation: P(t) = S + t * U
 */
template<typename T, class NP>
const typename Line2DParametric<T, NP>::TPoint
Line2DParametric<T, NP>::point(TParam iT) const
{
	return support_ + iT * direction_;
}



/** return UV pair of parameters
 */
template<typename T, class NP>
const typename Line2DParametric<T, NP>::TValue
Line2DParametric<T, NP>::t(const TPoint& iPoint) const
{
	return NP::divideBySquaredNorm(dot(iPoint - support_, direction_), direction_);
}



template <typename T, class NP>
void Line2DParametric<T, NP>::flip()
{
	direction_ = -direction_;
}



/** return true if line is a valid line (no normal or direction vectors that are zero).
 */
template<typename T, class NP>
bool Line2DParametric<T, NP>::isValid() const
{
	return !direction_.isZero();
}




// --- protected -----------------------------------------------------------------------------------



// --- private -------------------------------------------------------------------------------------



// --- free ----------------------------------------------------------------------------------------

template<typename T, class NP>
std::ostream& operator<<(std::ostream& ioOStream, const Line2DParametric<T, NP>& iLine)
{
	LASS_ENFORCE(ioOStream) << "{S=" << iLine.support() << ", D=" << iLine.direction() << "}";
	return ioOStream;
}



}

}

}

#endif

// EOF
