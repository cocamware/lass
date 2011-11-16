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
#include "../../num/floating_point_comparison.h"

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
