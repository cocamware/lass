/**	@file
 *	@author Bram de Greve (bramz@users.sourceforge.net)
 *	@author Tom De Muer (tomdemuer@users.sourceforge.net)
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
 *	Copyright (C) 2004-2007 the Initial Developer.
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

}
}
}

#endif

// EOF
