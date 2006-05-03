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



/** @class lass::prim::impl::Plane3DImplDetail
 *  @brief common implementation stuff for Plane3D implementations.
 *  @author Bram de Greve [BdG]
 *
 *  Here are some common generator functions gahtered, that are used to generate some values
 *  automatically from others.
 */

#ifndef LASS_GUARDIAN_OF_INCLUSION_PRIM_IMPL_PLANE_3D_IMPL_DETAIL_H
#define LASS_GUARDIAN_OF_INCLUSION_PRIM_IMPL_PLANE_3D_IMPL_DETAIL_H

#include "../prim_common.h"
#include "../num/floating_point_comparison.h"

namespace lass
{
namespace prim
{
namespace impl
{

struct Plane3DImplDetail
{
	/** Generate cartesian equation out of parametric equation.
	 *  Generate normal vector N and value d of cartesian equaition N.P + d == 0,
	 *  given support point S and direction vectors U and V of parametric
	 *  equation P = S + x*U + y*V.
	 */
	template <typename T>
	static void generateCartesian(const Point3D<T>& iSupport,
								  const Vector3D<T>& iDirU,
								  const Vector3D<T>& iDirV,
								  Vector3D<T>& oNormal,
								  T& oD)
	{
		oNormal = cross(iDirU, iDirV);
		oD = -dot(oNormal, iSupport.position());
	}

	/** Generate directions vectors U and V of parametric equation P = S + x*U + y*V,
	 *  based on the normal vector N of the cartesian equation N.P + S == 0.
	 */
	template <typename T>
	static void generateDirections(const Vector3D<T>& iNormal,
								   Vector3D<T>& oDirU,
								   Vector3D<T>& oDirV)
	{
		typedef Vector3D<T> TVector;
		typedef typename TVector::TValue TValue;
		typedef typename TVector::TNumTraits TNumTraits;

		const TValue absX = num::abs(iNormal.x);
		const TValue absY = num::abs(iNormal.y);
		const TValue absZ = num::abs(iNormal.z);

		if (absZ > absX && absZ > absY)
		{
			const TVector j(TNumTraits::zero, TNumTraits::one, TNumTraits::zero);
			oDirU = cross(j, iNormal);
			oDirV = cross(iNormal, oDirU);
		}
		else if (absX > absY)
		{
			const TVector k(TNumTraits::zero, TNumTraits::zero, TNumTraits::one);
			oDirU = cross(k, iNormal);
			oDirV = cross(iNormal, oDirU);
		}
		else
		{
			const TVector i(TNumTraits::one, TNumTraits::zero, TNumTraits::zero);
			oDirU = cross(i, iNormal);
			oDirV = cross(iNormal, oDirU);
		}
	}

	/** generate reciprocal direction vectors Ur and Vr given directions U and V.
	 *  Reciprocal vectors can be used to find the values x and y in A=x*U+y*V,
	 *  given A, U and V.  With Ur and Vr, they can be found by x=A.Ur and y=A.Vr.
	 */
	template <typename T>
	static void generateReciprocal(const Vector3D<T>& iDirU,
								   const Vector3D<T>& iDirV,
								   Vector3D<T>& oReciprocalDirU,
								   Vector3D<T>& oReciprocalDirV)
	{
		const typename Vector3D<T>::TValue dotUV = dot(iDirU, iDirV);
		oReciprocalDirU = iDirU - iDirV * (dotUV / iDirV.squaredNorm());
		oReciprocalDirV = iDirV - iDirU * (dotUV / iDirU.squaredNorm());
		oReciprocalDirU /= dot(oReciprocalDirU, iDirU);
		oReciprocalDirV /= dot(oReciprocalDirV, iDirV);
	}

};



}

}

}

#endif

// EOF
