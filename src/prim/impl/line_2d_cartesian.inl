/** @file
 *  @author Bram de Greve (bramz@users.sourceforge.net)
 *  @author Tom De Muer (tomdemuer@users.sourceforge.net)
 *
 *  Distributed under the terms of the GPL (GNU Public License)
 *
 *  The LASS License:
 *
 *  Copyright 2004-2006 Bram de Greve and Tom De Muer
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



#ifndef LASS_GUARDIAN_OF_INCLUSION_PRIM_IMPL_LINE_2D_CARTESIAN_INL
#define LASS_GUARDIAN_OF_INCLUSION_PRIM_IMPL_LINE_2D_CARTESIAN_INL

#include "../prim_common.h"
#include "line_2d_cartesian.h"

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
Line2DCartesian<T, NP>::Line2DCartesian():
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
Line2DCartesian<T, NP>::Line2DCartesian(const TPoint& iSupport, const TPoint& iPoint):
	normal_((iPoint - iSupport).perp())
{
	NP::normalize(normal_);
	d_ = -dot(normal_, iSupport.position());
}



/** construct a line through a support point and by two direction vectors..
 *  - support point S is given by the point iSupport.
 *  - direction vector U is given by iDir.
 *  - normal vector N is given by the perp of N.
 *  - value d is choosen so that the support point is indeed a point of the line.
 */
template<typename T, class NP>
Line2DCartesian<T, NP>::Line2DCartesian(const TPoint& iSupport, const TVector& iDirection):
	normal_(iDirection.perp())
{
	NP::normalize(normal_);
	d_ = -dot(normal_, iSupport.position());
}



/** Construct a line through a support point and by a normal vector.
 *  - support point S is given by the point iSupport.
 *  - normal vector N is given by the vector iNormal.
 *  - direction vector U is automatically generated so that N is perp of U.
 *  - value d is choosen so that the support point is indeed a point of the line.
 */
template<typename T, class NP>
Line2DCartesian<T, NP>::Line2DCartesian(const TVector& iNormal, const TPoint& iSupport):
	normal_(iNormal)
{
	NP::normalize(normal_);
	d_ = -dot(normal_, iSupport.position());
}



/** Construct a line by a cartesian equation N.P + d == 0.
 *  - normal vector N is given by the vector iNormal.
 *  - value d is given by the value iD.
 *  - support point S automatically generated so that N.S + d == 0.
 *  - direction vector U is automatically generated so that N is perp of U.
 */
template<typename T, class NP>
Line2DCartesian<T, NP>::Line2DCartesian(const TVector& iNormal, TParam iD):
	normal_(iNormal),
	d_(iD)
{
	NP::normalizeAndScale(normal_, d_);
}



/** return @b generated support point.
 *  @return -d * N
 */
template<typename T, class NP>
const typename Line2DCartesian<T, NP>::TPoint Line2DCartesian<T, NP>::support() const
{
	return TPoint(normal_ * NP::divideBySquaredNorm(-d_, normal_));
}



/** return @b generated direction vector.
 */
template<typename T, class NP>
const typename Line2DCartesian<T, NP>::TVector Line2DCartesian<T, NP>::direction() const
{
	return -normal_.perp();
}



template<typename T, class NP>
void Line2DCartesian<T, NP>::getCartesian(TVector& oNormal, TReference oD) const
{
	oNormal = normal_;
	oD = d_;
}



template<typename T, class NP>
const typename Line2DCartesian<T, NP>::TVector& Line2DCartesian<T, NP>::normal() const
{
	return normal_;
}



template<typename T, class NP>
const typename Line2DCartesian<T, NP>::TParam Line2DCartesian<T, NP>::d() const
{
	return d_;
}



/** Return value of point in equation.
 */
template<typename T, class NP>
const typename Line2DCartesian<T, NP>::TValue
Line2DCartesian<T, NP>::equation(const TPoint& iPoint) const
{
	return dot(iPoint.position(), normal_) + d_;
}



/** Return value of point in equation, snapped to zero by @a iRelativeTolerance
 */
template<typename T, class NP>
const typename Line2DCartesian<T, NP>::TValue
Line2DCartesian<T, NP>::equation(const TPoint& iPoint, TParam iRelativeTolerance) const
{
	const TValue d = dot(iPoint.position(), normal_);
	return num::almostEqual(d, -d_, iRelativeTolerance) ? TNumTraits::zero : (d + d_);
}



/** return the vector that, if added to the PROJECTION of iPoint, you get iPoint again.
 *  iPoint == (almost) project(iPoint) + reject(iPoint)
 */
template<typename T, class NP>
const typename Line2DCartesian<T, NP>::TVector
Line2DCartesian<T, NP>::reject(const TPoint& iPoint) const
{
	return normal_ * NP::divideBySquaredNorm(equation(iPoint), normal_);
}



/** return the part of iVector that is orthogonal to the line.
 *  it's the vector that, if added to the PROJECTION of iVector, you get iVector again.
 *  iVector == (almost) project(iVector) + reject(iVector).
 */
template<typename T, class NP>
const typename Line2DCartesian<T, NP>::TVector
Line2DCartesian<T, NP>::reject(const TVector& iVector) const
{
	return normal_ * NP::divideBySquaredNorm(dot(normal_, iVector), normal_);
}



/** project a point orthogonally onto the line
 */
template<typename T, class NP>
const typename Line2DCartesian<T, NP>::TPoint
Line2DCartesian<T, NP>::project(const TPoint& iPoint) const
{
	return iPoint - reject(iPoint);
}



/** project a vector orthogonally onto the line
 */
template<typename T, class NP>
const typename Line2DCartesian<T, NP>::TVector
Line2DCartesian<T, NP>::project(const TVector& iVector) const
{
	return iVector - reject(iVector);
}



/** reflect a point orthogonally into the line.
 */
template<typename T, class NP>
const typename Line2DCartesian<T, NP>::TPoint
Line2DCartesian<T, NP>::reflect(const TPoint& iPoint) const
{
	return iPoint - T(2) * reject(iPoint);
}



/** reflect a vector orthogonally into the line
 */
template<typename T, class NP>
const typename Line2DCartesian<T, NP>::TVector
Line2DCartesian<T, NP>::reflect(const TVector& iVector) const
{
	return iVector - T(2) * reject(iVector);
}



/** return point by filling in parameter in @b generated parametric equation
 */
template<typename T, class NP>
const typename Line2DCartesian<T, NP>::TPoint
Line2DCartesian<T, NP>::point(TParam iT) const
{
	return support() + iT * direction();
}



/** return parameter along @b generated paremetric equation.
 */
template<typename T, class NP>
const typename Line2DCartesian<T, NP>::TValue Line2DCartesian<T, NP>::t(const TPoint& iPoint) const
{
	const TVector dir = direction();
	return NP::divideBySquaredNorm(dot(dir, iPoint - support()), dir);
}



template <typename T, class NP>
void Line2DCartesian<T, NP>::flip()
{
	normal_ = -normal_;
	d_ = -d_;
}



/** return true if line is a valid line (no normal or direction vectors that are zero).
 */
template<typename T, class NP>
const bool Line2DCartesian<T, NP>::isValid() const
{
	return !normal_.isZero();
}




// --- protected -----------------------------------------------------------------------------------



// --- private -------------------------------------------------------------------------------------



// --- free ----------------------------------------------------------------------------------------

template<typename T, class NP>
std::ostream& operator<<(std::ostream& ioOStream, const Line2DCartesian<T, NP>& iLine)
{
	LASS_ENFORCE(ioOStream) << "{N=" << iLine.normal() << ", d=" << iLine.d() << "}";
	return ioOStream;
}



}

}

}

#endif

// EOF
