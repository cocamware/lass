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



/** @class lass::prim::impl::Plane3DCartesian
 *  @brief implementation of plane 3d with a cartesian equation
 *  @author Bram de Greve [BdG]
 *
 *  The implementation of this class uses the cartesian equation a * x + b * y + c * z + d == 0.
 *
 *  @warning The interface requires to have two direction vectors too, but of course, the cartesian
 *  equation does not provide this.  So, it has to generate two direction vectors on request.
 *  THESE ARE NOT NECESSARELY THE SAME AS THE ONE YOU'VE MIGHT TO USE TO SET THE PLANE.  i.e.
 *  if you go like this:  
 *
 *  @code
 *  Vector3D<double> directionV(1, 2, 3);
 *  Vector3D<double> directionV(4, 5, 6);
 *  Plane3D<double, Cartesian> plane(directionV, directionV);
 *  plane.getDirections(directionV, directionV);
 *  @endcode
 *
 *  then at the end, directionV and directionV will probably not be the same as at first.  This is because 
 *  the plane will only create a normal vector based on the direction vectors, and then forget about
 *  them.  If you ask to get the direction vectors, it has to create new ones.  
 */



#ifndef LASS_GUARDIAN_OF_INCLUSION_PRIM_IMPL_PLANE_3D_CARTESIAN_H
#define LASS_GUARDIAN_OF_INCLUSION_PRIM_IMPL_PLANE_3D_CARTESIAN_H

#include "../prim_common.h"

namespace lass
{
namespace prim
{
namespace impl
{

template <typename T, class NormalizingPolicy = Normalized>
class Plane3DCartesian
{
public:

    typedef NormalizingPolicy TNormalizingPolicy;

    typedef typename Point3D<T> TPoint;
    typedef typename TPoint::TVector TVector;
    typedef typename Point2D<T> TUV;

    typedef typename TPoint::TValue TValue;
    typedef typename TPoint::TParam TParam;
    typedef typename TPoint::TReference TReference;
    typedef typename TPoint::TConstReference TConstReference;
    typedef typename TPoint::TNumTraits TNumTraits;

    enum { dimension = TPoint::dimension };	/**< number of dimensions of vector */

	Plane3DCartesian();
	Plane3DCartesian(const TPoint& iSupport, const TPoint& iPointU, const TPoint& iPointV);
	Plane3DCartesian(const TPoint& iSupport, const TVector& iDirectionU, const TVector& iDirectionV);
	Plane3DCartesian(const TVector& iNormal, const TPoint& iSupport);
	Plane3DCartesian(const TVector& iNormal, TParam iD);

	const TPoint support() const;
    void getDirections(TVector& oDirectionU, TVector& oDirectionV) const;
    const TVector directionU() const;
	const TVector directionV() const;

    void getReciprocals(TVector& oReciprocalU, TVector& oReciprocalV) const;
    const TVector reciprocalU() const;
	const TVector reciprocalV() const;

    void getCartesian(TVector& oNormal, TReference oD) const;
    const TVector& normal() const;
	const TParam d() const;

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

    TVector normal_;
	TValue d_;
};



}

}

}

#include "plane_3d_cartesian.inl"

#endif

// EOF
