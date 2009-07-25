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
 *	Copyright (C) 2004-2009 the Initial Developer.
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

#ifndef LASS_GUARDIAN_OF_INCLUSION_PRIM_AABB_2D_TRIANGLE_2D_H
#define LASS_GUARDIAN_OF_INCLUSION_PRIM_AABB_2D_TRIANGLE_2D_H

#include "prim_common.h"
#include "aabb_2d.h"
#include "triangle_2d.h"

namespace lass
{
namespace prim
{

/** determine axis aligned bounding box of a 2D triangle
 *  @relates lass::prim::Triangle2D
 *	@sa lass::prim::Aabb2D
 */
template <typename T> 
Aabb2D<T> aabb(const Triangle2D<T>& triangle)
{
	Aabb2D<T, AutoMinMax> result(triangle[0], triangle[1]);
	result += triangle[2];
	return result;
}



/*  @relates lass::prim::Triangle2D
 *	@sa lass::prim::Aabb2D
 */
template <typename T, typename MMP>
bool intersects(const Triangle2D<T>& triangle, const Aabb2D<T, MMP>& box)
{
	if (!box.intersects(aabb(triangle)))
	{
		return false;
	}

	typedef typename Triangle2D<T>::TPoint TPoint;
	typedef typename Triangle2D<T>::TVector TVector;
	typedef typename Triangle2D<T>::TValue TValue;
	int k1 = 1, k2 = 2;
	for (int k0 = 0; k0 < 3; ++k0)
	{
		const TPoint& tail = triangle[k0];
		const TVector edge = triangle[k1] - tail;
		const TVector min = box.min() - tail;
		const TVector max = box.max() - tail;
		
		const TValue t[4] = {
			perpDot(edge, min),
			perpDot(edge, max),
			perpDot(edge, TVector(min.x, max.y)),
			perpDot(edge, TVector(max.x, min.y))
		};

		if (t[0] < 0 && t[1] < 0 && t[2] < 0 && t[3] < 0)
		{
			return false;
		}

		const TValue tMax = perpDot(edge, triangle[k2] - tail);
		LASS_ASSERT(tMax >= 0);
		if (t[0] > tMax && t[1] > tMax && t[2] > tMax && t[3] > tMax)
		{
			return false;
		}

		k1 = k2;
		k2 = k0;
	}

	return true;
}



/*  @relates lass::prim::Aabb2D
 *	@sa lass::prim::Triangle2D
 */
template <typename T, typename MMP>
bool intersects(const Aabb2D<T, MMP>& box, const Triangle2D<T>& triangle)
{
	return intersects(triangle, box);
}



/*  @relates lass::prim::Triangle2D
 *	@sa lass::prim::Aabb2D
 */
template <typename T, typename MMP>
bool collides(const Triangle2D<T>& triangle, const Aabb2D<T, MMP>& box)
{
	if (!box.collides(aabb(triangle)))
	{
		return false;
	}

	typedef typename Triangle2D<T>::TPoint TPoint;
	typedef typename Triangle2D<T>::TVector TVector;
	typedef typename Triangle2D<T>::TValue TValue;
	int k1 = 1, k2 = 2;
	for (int k0 = 0; k0 < 3; ++k0)
	{
		const TPoint& tail = triangle[k0];
		const TVector edge = triangle[k1] - tail;
		const TVector min = box.min() - tail;
		const TVector max = box.max() - tail;
		
		const TValue t[4] = {
			perpDot(edge, min),
			perpDot(edge, max),
			perpDot(edge, TVector(min.x, max.y)),
			perpDot(edge, TVector(max.x, min.y))
		};

		if (t[0] <= 0 && t[1] <= 0 && t[2] <= 0 && t[3] <= 0)
		{
			return false;
		}

		const TValue tMax = perpDot(edge, triangle[k2] - tail);
		LASS_ASSERT(tMax >= 0);
		if (t[0] >= tMax && t[1] >= tMax && t[2] >= tMax && t[3] >= tMax)
		{
			return false;
		}

		k1 = k2;
		k2 = k0;
	}

	return true;
}



/*  @relates lass::prim::Aabb2D
 *	@sa lass::prim::Triangle2D
 */
template <typename T, typename MMP>
bool collides(const Aabb2D<T, MMP>& box, const Triangle2D<T>& triangle)
{
	return collides(triangle, box);
}

}
}

#endif

// EOF
