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

#ifndef LASS_GUARDIAN_OF_INCLUSION_PRIM_AABB_3D_TRIANGLE_3D_H
#define LASS_GUARDIAN_OF_INCLUSION_PRIM_AABB_3D_TRIANGLE_3D_H

#include "prim_common.h"
#include "aabb_3d.h"
#include "triangle_3d.h"

namespace lass
{
namespace prim
{

/** determine axis aligned bounding box of a 3D triangle
 *  @relates Aabb3D
 */
template <typename T> 
Aabb3D<T> aabb(const Triangle3D<T>& triangle)
{
	Aabb3D<T, AutoMinMax> result(triangle[0], triangle[1]);
	result += triangle[2];
	return result;
}

namespace impl
{

template <typename V>
bool intersectsHelperTriangleAabb3D(const V& v0, const V& v1, const V& v2, const V& h)
{
	typedef typename V::TValue TValue;

	const V f0 = v1 - v0;
	const V af0(num::abs(f0.x), num::abs(f0.y), num::abs(f0.z));

	TValue min = v0.z * v1.y - v0.y * v1.z;
	TValue max = f0.y * v2.z - f0.z * v2.y;
	const TValue rx = h.y * af0.z + h.z * af0.y;
	if (max < min)
	{
		std::swap(min, max);
	}
	if (min > rx || max < -rx)
	{
		return false;
	}

	min = v0.x * v1.z - v0.z * v1.x;
	max = f0.z * v2.x - f0.x * v2.z;
	const TValue ry = h.z * af0.x + h.x * af0.z;
	if (max < min)
	{
		std::swap(min, max);
	}
	if (min > ry || max < -ry)
	{
		return false;
	}

	min = v0.y * v1.x - v0.x * v1.y;
	max = f0.x * v2.y - f0.y * v2.x;
	const TValue rz = h.x * af0.y + h.y * af0.x;
	if (max < min)
	{
		std::swap(min, max);
	}
	if (min > rz || max < -rz)
	{
		return false;
	}

	return true;
}

}

/*  @relates lass::prim::Triangle3D
 *	@sa lass::prim::Aabb3D
 *
 *  Using the Tomas Akenine-Moller algorithm from "Fast 3D Triangle-Box Overlap Testing"
 */
template <typename T, typename MMP>
bool intersects(const Triangle3D<T>& triangle, const Aabb3D<T, MMP>& box)
{
	typedef typename Triangle3D<T>::TPoint TPoint;
	typedef typename Triangle3D<T>::TVector TVector;
	typedef typename Triangle3D<T>::TValue TValue;

	const TPoint center = box.center().affine();
	const TVector extent = box.size() / 2;

	const TVector verts[] = 
	{
		triangle[0] - center,
		triangle[1] - center,
		triangle[2] - center
	};

	// bullet 3
	if (!impl::intersectsHelperTriangleAabb3D(verts[0], verts[1], verts[2], extent)) return false;
	if (!impl::intersectsHelperTriangleAabb3D(verts[1], verts[2], verts[0], extent)) return false;
	if (!impl::intersectsHelperTriangleAabb3D(verts[2], verts[0], verts[1], extent)) return false;

	// bullet 1
	if (!box.intersects(aabb(triangle)))
	{
		return false;
	}

	// bullet 2
	return intersects(triangle.plane(), box);
}


}
}

#endif

// EOF
