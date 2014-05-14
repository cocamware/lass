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

template <typename T>
void inpminmax(T& a, T& b)
{
	if (a > b)
	{
		std::swap(a, b);
	}
}

template <typename P>
bool intersectsHelperTriangleAabb3D(const P& v0, const P& v1, const P& v2, const P& a, const P& b)
{
	typedef typename P::TVector TVector;
	typedef typename P::TValue TValue;

	// this should move somedday to some place more appropriate.
	class Range
	{
	public:
		Range(TValue min, TValue max): min_(min), max_(max)
		{
			inpminmax(min_, max_);
		}
		Range& operator+=(const Range& other)
		{
			min_ += other.min_;
			max_ += other.max_;
			return *this;
		}
		bool seperated(const Range& other) const
		{
			return other.max_ < min_ || other.min_ > max_;
		}
	private:
		TValue min_;
		TValue max_;
	};

	const TVector f = v1 - v0; // triangleEdge

	{
		// const V aabbEdge(1, 0, 0);
		// const V axis = cross(aabbEdge, triangleEdge);
		const Range triangleRange(
			v0.z * v1.y - v0.y * v1.z, // == dot(v0, axis) == dot(v1, axis)
			f.y * v2.z - f.z * v2.y  // == dot(v2, axis)
			);
		Range boxRange(a.y * -f.z, b.y * -f.z);
		boxRange += Range(a.z * f.y, b.z * f.y);
		if (triangleRange.seperated(boxRange)) return false;
	}

	{
		// const V aabbEdge(0, 1, 0);
		const Range triangleRange(
			v0.x * v1.z - v0.z * v1.x,
			f.z * v2.x - f.x * v2.z
			);
		Range boxRange(a.z * -f.x, b.z * -f.x);
		boxRange += Range(a.x * f.z, b.x * f.z);
		if (triangleRange.seperated(boxRange)) return false;
	}

	{
		// const V aabbEdge(0, 0, 1);
		const Range triangleRange(
			v0.y * v1.x - v0.x * v1.y,
			f.x * v2.y - f.y * v2.x
			);
		Range boxRange(a.x * -f.y, b.x * -f.y);
		boxRange += Range(a.y * f.x, b.y * f.x);
		if (triangleRange.seperated(boxRange)) return false;
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
	// bullet 3
	if (!impl::intersectsHelperTriangleAabb3D(triangle[0], triangle[1], triangle[2], box.min(), box.max())) return false;
	if (!impl::intersectsHelperTriangleAabb3D(triangle[1], triangle[2], triangle[0], box.min(), box.max())) return false;
	if (!impl::intersectsHelperTriangleAabb3D(triangle[2], triangle[0], triangle[1], box.min(), box.max())) return false;

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
