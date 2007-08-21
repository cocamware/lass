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

#ifndef LASS_GUARDIAN_OF_INCLUSION_PRIM_LINE_2D_RAY_2D_H
#define LASS_GUARDIAN_OF_INCLUSION_PRIM_LINE_2D_RAY_2D_H

#include "prim_common.h"
#include "line_2d.h"
#include "ray_2d.h"
#include "../num/floating_point_consistency.h"

namespace lass
{
namespace prim
{

/** Find the intersection of a line and ray by their parameter t on the ray.
 *  @relates lass::prim::Line2D
 *  @relates lass::prim::Ray2D
 *
 *  @param line [in] the line
 *  @param ray [in] the ray
 *  @param t [out] the parameter of the intersection point > @a tMin.
 *  @param tMin [in] the minimum t that may be returned as valid intersection.
 *  @return @arg rNone      no intersections > @a tMin found
 *                          @a t is not assigned.
 *          @arg rOne       exactly one intersection > @a tMin found
 *                          @a t represents it.
 *          @arg rInfinite  infinite many intersections found (ray is coincident with plane),
 *                          @a t is not assigned.
 *          @arg rInvalid   @a iPlane or @a ray is invalid, no intersection.
 *                          @a t is not assigned.
 */
template<typename T, class EP1, class NP1, class NP2, class PP2>
Result intersect(const Line2D<T, EP1, NP1>& line,
				 const Ray2D<T, NP2, PP2>& ray,
				 T& t, const T& tMin = T())
{
	typedef typename Vector2D<T>::TValue TValue;
	typedef typename Vector2D<T>::TNumTraits TNumTraits;
	typedef num::Consistent<T> TConsistent;

	if (!line.isValid() || !ray.isValid())
	{
		return rInvalid;
	}

	const TValue nd = perpDot(line.normal(), ray.direction());
	if (nd == TNumTraits::zero)
	{
		// ray is parallel to plane, but is it also coincident?
		const Side side = line.classify(ray.support());
		LASS_ASSERT(side == sFront || side == sSurface || side == sBack);
		return side == sSurface ? rInfinite : rNone;
	}
	else
	{
		const TConsistent tCandidate = -line.equation(ray.support()) / nd;
		LASS_ASSERT(!num::isNaN(tCandidate));
		if (tCandidate > tMin)
		{
			t = tCandidate.value();
			return rOne;
		}
		return rNone;
	}
}

}
}

#endif
