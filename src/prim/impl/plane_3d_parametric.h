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



/** @class lass::prim::impl::Plane3DParametric
 *  @brief implementation of lass::prim::Plane3D with a parametric equation
 *  @author Bram de Greve [BdG]
 *
 *  This implementation of the plane uses the parametric equation P(u, v) = S + u * U + v * V.
 *  with S a support point, U and V two direction vectors, and u and v two parameters.
 *
 *  @warning currently, the plane will not automatically orthonormalize the direction vectors.
 *           There is a normalizing policy, but that's only for normalizing vectors, not for
 *           orthogonalizing two direction vectors.  IF SOMEONE IS USING THIS PARAMETRIC PLANE
 *           CLASS AND RUNS IN SOME TROUBLES ON THIS, PLEASE CONTACT ME SO WE CAN WORK OUT
 *           A SOLUTION.  Bram de Greve.
 */

#ifndef LASS_GUARDIAN_OF_INCLUSION_PRIM_IMPL_PLANE_3D_PARAMETRIC_H
#define LASS_GUARDIAN_OF_INCLUSION_PRIM_IMPL_PLANE_3D_PARAMETRIC_H

#include "../prim_common.h"

namespace lass
{
namespace prim
{
namespace impl
{

template <typename T, class NormalizingPolicy = Normalized>
class Plane3DParametric
{
public:

    typedef NormalizingPolicy TNormalizingPolicy;

    typedef Point3D<T> TPoint;
    typedef typename TPoint::TVector TVector;
    typedef Point2D<T> TUV;

    typedef typename TPoint::TValue TValue;
    typedef typename TPoint::TParam TParam;
    typedef typename TPoint::TReference TReference;
    typedef typename TPoint::TConstReference TConstReference;
    typedef typename TPoint::TNumTraits TNumTraits;

    enum { dimension = TPoint::dimension };	/**< number of dimensions of vector */

	Plane3DParametric();
	Plane3DParametric(const TPoint& iSupport, const TPoint& iPointU, const TPoint& iPointV);
	Plane3DParametric(const TPoint& iSupport, const TVector& iDirectionU, const TVector& iDirectionV);
	Plane3DParametric(const TVector& iNormal, const TPoint& iSupport);
	Plane3DParametric(const TVector& iNormal, TParam iD);

	const TPoint& support() const;
    void getDirections(TVector& oDirectionU, TVector& oDirectionV) const;
    const TVector& directionU() const;
	const TVector& directionV() const;

    void getReciprocals(TVector& oReciprocalU, TVector& oReciprocalV) const;
    const TVector reciprocalU() const;
	const TVector reciprocalV() const;

    void getCartesian(TVector& oNormal, TReference oD) const;
    const TVector normal() const;
	const TValue d() const;

    const Side classify(const TPoint& iPoint) const;
    const TValue equation(const TPoint& iPoint) const;
    const TValue signedDistance(const TPoint& iPoint) const;
	const TValue squaredDistance(const TPoint& iPoint) const;

	const TVector reject(const TPoint& iPoint) const;
	const TVector reject(const TVector& iVector) const;
	const TPoint project(const TPoint& iPoint) const;
	const TVector project(const TVector& iVector) const;
	const TPoint reflect(const TPoint& iPoint) const;
	const TVector reflect(const TVector& iVector) const;

	const TPoint point(TParam iU, TParam iV) const;
	const TPoint point(const TUV& iUV) const;
    const TUV uv(const TPoint& iPoint) const;

    void flip();
    const bool isValid() const;

private:

    TPoint support_;
    TVector directionU_;
    TVector directionV_;
};



}

}

}

#include "plane_3d_parametric.inl"

#endif

// EOF
