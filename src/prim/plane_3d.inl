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



#ifndef LASS_GUARDIAN_OF_INCLUSION_PRIM_PLANE_3D_INL
#define LASS_GUARDIAN_OF_INCLUSION_PRIM_PLANE_3D_INL

#include "prim_common.h"
#include "plane_3d.h"

namespace lass
{
namespace prim
{

// --- public --------------------------------------------------------------------------------------

template<typename T, typename EP, typename NP>
Plane3D<T, EP, NP>::Plane3D():
	TImpl()
{
}




template<typename T, typename EP, typename NP>
Plane3D<T, EP, NP>::Plane3D(const TPoint& iSupport,
							const TPoint& iPointU,
							const TPoint& iPointV):
	TImpl(iSupport, iPointU, iPointV)
{
}



template<typename T, typename EP, typename NP>
Plane3D<T, EP, NP>::Plane3D(const TPoint& iSupport,
							const TVector& iDirU,
							const TVector& iDirV):
	TImpl(iSupport, iDirU, iDirV)
{
}



template<typename T, typename EP, typename NP>
Plane3D<T, EP, NP>::Plane3D(const TVector& iNormal, const TPoint& iSupport):
	TImpl(iSupport, iNormal)
{
}



template<typename T, typename EP, typename NP>
Plane3D<T, EP, NP>::Plane3D(const TVector& iNormal, TParam iD):
	TImpl(iNormal, iD)
{
}



/** Return on what side a point is located.
 */
template<typename T, typename EP, typename NP>
const Side Plane3D<T, EP, NP>::classify(const TPoint& iPoint) const
{
	const TValue eq = this->equation(iPoint);
	return eq > TNumTraits::zero ? sFront : (eq < TNumTraits::zero ? sBack : sSurface);
}



/** Return signed distance of point to plane.
 *  negative value means point is in the back.
 */
template<typename T, typename EP, typename NP>
const typename Plane3D<T, EP, NP>::TValue
Plane3D<T, EP, NP>::signedDistance(const TPoint& iPoint) const
{
	return NP::divideByNorm(this->equation(iPoint), this->normal());
}



/** Return squared distance of point to plane.
 */
template<typename T, typename EP, typename NP>
const typename Plane3D<T, EP, NP>::TValue
Plane3D<T, EP, NP>::squaredDistance(const TPoint& iPoint) const
{
	return num::sqr(this->signedDistance(iPoint));
}



/** Return on what side a point is located.
 */
template<typename T, typename EP, typename NP>
const Side Plane3D<T, EP, NP>::classify(const TPoint& iPoint, TParam iRelativeTolerance) const
{
	const TValue eq = this->equation(iPoint, iRelativeTolerance);
	return eq > TNumTraits::zero ? sFront : (eq < TNumTraits::zero ? sBack : sSurface);
}



/** Return signed distance of point to plane.
 *  negative value means point is in the back.
 */
template<typename T, typename EP, typename NP>
const typename Plane3D<T, EP, NP>::TValue
Plane3D<T, EP, NP>::signedDistance(const TPoint& iPoint, TParam iRelativeTolerance) const
{
	return NP::divideByNorm(equation(iPoint, iRelativeTolerance), normal_);
}



/** Return squared distance of point to plane.
 */
template<typename T, typename EP, typename NP>
const typename Plane3D<T, EP, NP>::TValue
Plane3D<T, EP, NP>::squaredDistance(const TPoint& iPoint, TParam iRelativeTolerance) const
{
	return num::sqr(signedDistance(iPoint, iRelativeTolerance));
}



/** determines the major axis of the normal vector.
 *  The major axis is the one with the largest (absolute) component value.  e.g. if the normal
 *  vector is (-1, 4, -8), this will be the @e z axis because abs(-8) > abs(4) > abs(-1).
 *  In case there's more than one major axis possible, the "highest" index is choosen.  e.g.
 *  if the normal vector is (1, 1, 0), then @e y axis will be choosen, because @e y has a higher
 *  index than @e x .
 */
template<typename T, typename EP, typename NP>
const XYZ Plane3D<T, EP, NP>::majorAxis() const
{
	const TVector absNormal = normal().transform(num::abs);
	if (absNormal.x > absNormal.y && absNormal.x > absNormal.z)
	{
		return 0; // x
	}
	else if (absNormal.y > absNormal.z)
	{
		return 1; // y
	}
	else
	{
		return 2; // z
	}
	LASS_ASSERT(false);
	return 0;
}



// --- protected -----------------------------------------------------------------------------------



// --- private -------------------------------------------------------------------------------------



// --- free ----------------------------------------------------------------------------------------


/** @relates Plane3D
 */
template <typename T>
io::XmlOStream& operator<<(io::XmlOStream& ioOStream, const Plane3D<T, Cartesian>& iPlane)
{
	LASS_ENFORCE_STREAM(ioOStream)
		<< "<Plane3D>\n"
		<< "<normal>" << iPlane.normal() << "</normal>\n"
		<< "<d>" << iPlane.d() << "</d>\n"
		<< "</Plane3D>\n";

	return ioOStream;
}



/** @relates Plane3D
 */
template <typename T>
io::XmlOStream& operator<<(io::XmlOStream& ioOStream, const Plane3D<T, Parametric>& iPlane)
{
	LASS_ENFORCE_STREAM(ioOStream)
		<< "<Plane3D>\n"
		<< "<support>" << iPlane.support() << "</support>\n"
		<< "<directionU>" << iPlane.directionU() << "</directionU>\n"
		<< "<directionV>" << iPlane.directionV() << "</directionV>\n"
		<< "</Plane3D>\n";

	return ioOStream;
}



/** @relates Plane3D
 */
template <typename T>
io::XmlOStream& operator<<(io::XmlOStream& ioOStream, const Plane3D<T, Combined>& iPlane)
{
	LASS_ENFORCE_STREAM(ioOStream)
		<< "<Plane3D>\n"
		<< "<support>" << iPlane.support() << "</support>\n"
		<< "<directionU>" << iPlane.directionU() << "</directionU>\n"
		<< "<directionV>" << iPlane.directionV() << "</directionV>\n"
		<< "<normal>" << iPlane.normal() << "</normal>\n"
		<< "<d>" << iPlane.d() << "</d>\n"
		<< "</Plane3D>\n";

	return ioOStream;
}



}

}

#endif

// EOF
