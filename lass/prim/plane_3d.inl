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
	TImpl(iNormal, iSupport)
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
Side Plane3D<T, EP, NP>::classify(const TPoint& iPoint) const
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
Side Plane3D<T, EP, NP>::classify(const TPoint& iPoint, TParam iRelativeTolerance) const
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
	return NP::divideByNorm(equation(iPoint, iRelativeTolerance), this->normal());
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
	const TVector absNormal = this->normal().transform(num::abs);
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
