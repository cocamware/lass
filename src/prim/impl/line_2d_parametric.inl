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



#ifndef LASS_GUARDIAN_OF_INCLUSION_PRIM_IMPL_LINE_2D_PARAMETRIC_INL
#define LASS_GUARDIAN_OF_INCLUSION_PRIM_IMPL_LINE_2D_PARAMETRIC_INL

#include "../prim_common.h"
#include "line_2d_parametric.h"
#include "side.h"

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
typename const Line2DParametric<T, NP>::TPoint& Line2DParametric<T, NP>::support() const
{
	return support_;
}



/** return direction vector.
 */
template<typename T, class NP>
typename const Line2DParametric<T, NP>::TVector& Line2DParametric<T, NP>::direction() const
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
typename const Line2DParametric<T, NP>::TVector Line2DParametric<T, NP>::normal() const
{
    TVector normal;
    TValue d;
    getCartesian(normal, d);
    return normal;
}



template<typename T, class NP>
typename const Line2DParametric<T, NP>::TValue Line2DParametric<T, NP>::d() const
{
    TVector normal;
    TValue d;
    getCartesian(normal, d);
    return d;
}



/** Return on what side a point is located.
 */
template<typename T, class NP>
const Side Line2DParametric<T, NP>::classify(const TPoint& iPoint) const
{
    const TValue eq = equation(iPoint);
    return eq > TNumTraits::zero ? sFront : (eq < TNumTraits::zero ? sBack : sSurface);
}



/** Return value of point in equation.
 */
template<typename T, class NP>
typename const Line2DParametric<T, NP>::TValue 
Line2DParametric<T, NP>::equation(const TPoint& iPoint) const
{
    TVector normal;
    TValue d;
    getCartesian(normal, d);
    return dot(iPoint.position(), normal) + d;
}



/** Return signed distance of point to line.
 *  negative value means point is in the back.
 */
template<typename T, class NP>
typename const Line2DParametric<T, NP>::TValue 
Line2DParametric<T, NP>::signedDistance(const TPoint& iPoint) const
{
    return NP::divideByNorm<T, TVector>(equation(iPoint), normal());
}



/** return the vector that, if added to the PROJECTION of iPoint, you get iPoint again.
 *  iPoint == (almost) project(iPoint) + reject(iPoint)
 */
template<typename T, class NP>
typename const Line2DParametric<T, NP>::TVector 
Line2DParametric<T, NP>::reject(const TPoint& iPoint) const
{
    return reject(iPoint - support_);
}



/** return the part of iVector that is orthogonal to the line. 
 *  it's the vector that, if added to the PROJECTION of iVector, you get iVector again.
 *  iVector == (almost) project(iVector) + reject(iVector).
 */
template<typename T, class NP>
typename const Line2DParametric<T, NP>::TVector 
Line2DParametric<T, NP>::reject(const TVector& iVector) const
{

    return iVector - project(iVector);
}



/** project a point orthogonally onto the line
 */
template<typename T, class NP>
typename const Line2DParametric<T, NP>::TPoint 
Line2DParametric<T, NP>::project(const TPoint& iPoint) const
{
	return support_ + project(iPoint - support_);
}



/** project a vector orthogonally onto the line
 */
template<typename T, class NP>
typename const Line2DParametric<T, NP>::TVector 
Line2DParametric<T, NP>::project(const TVector& iVector) const
{
	return direction_ * NP::divideBySquaredNorm(dot(iVector, direction_), direction_);
}



/** reflect a point orthogonally into the line.
 */
template<typename T, class NP>
typename const Line2DParametric<T, NP>::TPoint 
Line2DParametric<T, NP>::reflect(const TPoint& iPoint) const
{
	return support_ + reflect(iPoint - support_);
}



/** reflect a vector orthogonally to the line
 */
template<typename T, class NP>
typename const Line2DParametric<T, NP>::TVector 
Line2DParametric<T, NP>::reflect(const TVector& iVector) const
{
	return T(2) * project(iVector) - iVector;
}



/** return point by filling in the parametric equation: P(t) = S + t * U
 */
template<typename T, class NP>
typename const Line2DParametric<T, NP>::TPoint 
Line2DParametric<T, NP>::point(TParam iT) const
{
	return support_ + iT * direction_;
}



/** return UV pair of parameters 
 */
template<typename T, class NP>
typename const Line2DParametric<T, NP>::TValue 
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
const bool Line2DParametric<T, NP>::isValid() const
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
