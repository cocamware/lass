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
 *	Copyright (C) 2004-2024 the Initial Developer.
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

#ifndef LASS_GUARDIAN_OF_INCLUSION_PRIM_IMPL_INTERSECT_TRIANGLE_3D_H
#define LASS_GUARDIAN_OF_INCLUSION_PRIM_IMPL_INTERSECT_TRIANGLE_3D_H

#include "../../num/floating_point_consistency.h"
#include "../xyz.h"

namespace lass
{
namespace prim
{
namespace impl
{

/** @internal
 *
 *  @par Algorithm:
 *  @arg Tomas M�ller and Ben Trumbore.
 *	"Fast, minimum storage ray-triangle intersection."
 *	Journal of Graphics Tools, 2(1):21--28, 1997.
 *	http://www.graphics.cornell.edu/pubs/1997/MT97.html
 *  
 *  @arg comp.graphics.algorithms Frequently Asked Questions: 
 *	Subject 5.06: "How do I determine the intersection between a ray and a triangle?"
 *  http://www.faqs.org/faqs/graphics/algorithms-faq/ *
 */
template <typename Point, typename Vector, typename T> inline
Result intersectTriangle3D(const Point& iVertex0, const Vector& iEdge1, const Vector& iEdge2,
						   const Point& iSupport, const Vector& iDirection,
						   T& oU, T& oV, T& oT, const T& iMinT)
{
	typedef typename Vector::TNumTraits TNumTraits;
	typedef num::Consistent<T> TConsistent;

	const Vector pvec = cross(iDirection, iEdge2);
	const T det = dot(pvec, iEdge1);

	if (det == TNumTraits::zero)
	{
		return rNone;
	}

	const T invDet = num::inv(det);

	const Vector tvec = iSupport - iVertex0;
	const T u = dot(tvec, pvec) * invDet;
	if (u < TNumTraits::zero || u > TNumTraits::one)
	{
		return rNone;
	}

	const Vector qvec = cross(tvec, iEdge1);
	const T v = dot(iDirection, qvec) * invDet;
	if (v < TNumTraits::zero || (u + v) > TNumTraits::one)
	{
		return rNone;
	}

	const TConsistent t = dot(iEdge2, qvec) * invDet;
	if (t <= iMinT)
	{
		return rNone;
	}

	oU = u;
	oV = v;
	oT = t.value();
	return rOne;
}



/** @internal
 *
 *  @par Algorithm:
 *  @arg Sven Woop, Carsten Benthin, and Ingo Wald.
 *	"Watertight Ray/Triangle Intersection."
 *	Journal of Computer Graphics Techniques (JCGT), 2(1):65--82, 2019.
 *	https://jcgt.org/published/0002/01/05/
 */
template <typename Point>
class IntersectTriangle3DWoop
{
public:

	using TPoint3D = Point;
	using TVector3D = typename Point::TVector;
	using TValue = typename Point::TValue;

	IntersectTriangle3DWoop(const TPoint3D& support, const TVector3D& direction):
		support_(support)
	{
		kz_ = direction.majorAxis();
		kx_ = kz_ + 1;
		ky_ = kx_ + 1;
		if (direction[kz_] < 0)
		{
			std::swap(kx_, ky_);
		}
		shear_.z = num::inv(direction[kz_]);
		shear_.x = -direction[kx_] * shear_.z;
		shear_.y = -direction[ky_] * shear_.z;
	}

	Result operator()(
		const TPoint3D& vertex0, const TPoint3D& vertex1, const TPoint3D& vertex2,
		TValue b[3], TValue& t, const TValue& tMin) const
	{
		const TVector3D v0 = vertex0 - support_;
		const TVector3D v1 = vertex1 - support_;
		const TVector3D v2 = vertex2 - support_;

		const TValue v0x = v0[kx_] + v0[kz_] * shear_.x;
		const TValue v0y = v0[ky_] + v0[kz_] * shear_.y;
		const TValue v1x = v1[kx_] + v1[kz_] * shear_.x;
		const TValue v1y = v1[ky_] + v1[kz_] * shear_.y;
		const TValue v2x = v2[kx_] + v2[kz_] * shear_.x;
		const TValue v2y = v2[ky_] + v2[kz_] * shear_.y;

		TValue u = v2x * v1y - v2y * v1x;
		TValue v = v0x * v2y - v0y * v2x;
		TValue w = v1x * v0y - v1y * v0x;

		using TDouble = typename num::DoublePrecision<TValue>::Type;
		if constexpr (!std::is_same_v<TValue, TDouble>)
		{
			if (u == 0 || v == 0 || w == 0)
			{
				// retry with double precision
				const TDouble v21xy = static_cast<TDouble>(v2x) * static_cast<TDouble>(v1y);
				const TDouble v21yx = static_cast<TDouble>(v2y) * static_cast<TDouble>(v1x);
				u = static_cast<TValue>(v21xy - v21yx);

				const TDouble v02xy = static_cast<TDouble>(v0x) * static_cast<TDouble>(v2y);
				const TDouble v02yx = static_cast<TDouble>(v0y) * static_cast<TDouble>(v2x);
				v = static_cast<TValue>(v02xy - v02yx);

				const TDouble v10xy = static_cast<TDouble>(v1x) * static_cast<TDouble>(v0y);
				const TDouble v10yx = static_cast<TDouble>(v1y) * static_cast<TDouble>(v0x);
				w = static_cast<TValue>(v10xy - v10yx);
			}
		}

		if ((u < 0 || v < 0 || w < 0) && (u > 0 || v > 0 || w > 0))
		{
			return rNone;
		}

		const TValue det = u + v + w;
		if (det == 0)
		{
			return rNone;
		}
		const TValue invDet = num::inv(det);

		const TValue v0z = v0[kz_] * shear_.z;
		const TValue v1z = v1[kz_] * shear_.z;
		const TValue v2z = v2[kz_] * shear_.z;
		const TValue tt = (v0z * u + v1z * v + v2z * w) * invDet;
		if (tt <= tMin)
		{
			return rNone;
		}

		b[0] = u * invDet;
		b[1] = v * invDet;
		b[2] = w * invDet;
		t = tt;
		return rOne;
	}

private:
	TPoint3D support_;
	TVector3D shear_;
	XYZ kx_;
	XYZ ky_;
	XYZ kz_;
};


}
}
}

#endif

// EOF
