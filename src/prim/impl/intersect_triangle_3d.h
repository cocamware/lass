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

#ifndef LASS_GUARDIAN_OF_INCLUSION_PRIM_IMPL_INTERSECT_TRIANGLE_3D_H
#define LASS_GUARDIAN_OF_INCLUSION_PRIM_IMPL_INTERSECT_TRIANGLE_3D_H

namespace lass
{
namespace prim
{
namespace impl
{

/** @internal
 *
 *  @par Algorithm:
 *  @arg Tomas Möller and Ben Trumbore.
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

	const Vector pvec = cross(iDirection, iEdge2);
	const T det = dot(pvec, iEdge1);

	if (det == TNumTraits::zero)
	{
		return rNone;
	}

	const Vector tvec = iSupport - iVertex0;
	const T u = dot(tvec, pvec);
	if (u < TNumTraits::zero || u > det)
	{
		return rNone;
	}

	const Vector qvec = cross(tvec, iEdge1);
	const T v = dot(iDirection, qvec);
	if (v < TNumTraits::zero || (u + v) > det)
	{
		return rNone;
	}

	const T t = dot(iEdge2, qvec);
	if (t <= (iMinT * det))
	{
		return rNone;
	}

	const T invDet = num::inv(det);
	oU = u * invDet;
	oV = v * invDet;
	oT = t * invDet;
	return rOne;
}

}
}
}

#endif

// EOF
