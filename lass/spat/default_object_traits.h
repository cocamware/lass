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
 *	Copyright (C) 2004-2010 the Initial Developer.
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



/** @class lass::spat::DefaultObjectTraits
 *  @brief default traits for objects to be stored in spatial subdivision trees
 *  @author Bram de Greve [BdG]
 */

#ifndef LASS_GUARDIAN_OF_INCLUSION_SPAT_DEFAULT_OBJECT_TRAITS_H
#define LASS_GUARDIAN_OF_INCLUSION_SPAT_DEFAULT_OBJECT_TRAITS_H

#include "spat_common.h"
#include "../prim/result.h"
#include "../meta/int.h"

namespace lass
{
namespace spat
{

template
<
	typename AabbType, 
	typename RayType
>
struct DefaultAabbRayTraits
{
	typedef AabbType TAabb;						/**< an nD AABB */
	typedef RayType TRay;						/**< an nD Ray */

	typedef typename TAabb::TPoint TPoint;		/**< an nD point */
	typedef typename TAabb::TVector TVector;	/**< an nD vector */
	typedef typename TAabb::TValue TValue;		/**< numerical type used in TPoint and TVector */
	typedef typename TAabb::TParam TParam;		/**< best type for function parameters of TValue */
	typedef typename TAabb::TReference TReference;	/**< reference to TValue */
	typedef typename TAabb::TConstReference TConstReference; /**< const reference to TValue */
	
	enum { dimension = TAabb::dimension };		/**< nD = number of dimensions of TPoint */

	// AABB

	/** return empty AABB
	 */
	static const TAabb aabbEmpty()
	{
		return TAabb();
	}

	static const TAabb aabbMake(const TPoint& min, const TPoint& max)
	{
		return TAabb(min, max);
	}

	/** return true if AABB contains a point, return false otherwise
	 */
	static bool aabbContains(const TAabb& aabb, const TPoint& point) 
	{ 
		return aabb.contains(point); 
	}

	static bool aabbContains(const TAabb& aabb, const TAabb& other)
	{
		return aabb.contains(other);
	}

	static bool aabbIntersects(const TAabb& aabb, const TAabb& other)
	{
		return aabb.intersects(other);
	}

	/** return true if AABB is intersected by ray
	 */
	static bool aabbIntersect(const TAabb& aabb, const TRay& ray, TReference t, const TParam tMin)
	{
		return intersect(aabb, ray, t, tMin) != prim::rNone;
	}

	/** return true if AABB is intersected by ray, with the reciprocal ray direction already supplied.
	 */
	static bool aabbIntersect(const TAabb& aabb, const TRay& ray, const TVector& invDirection, TReference t, const TParam tMin)
	{
		return intersect(aabb, ray, invDirection, t, tMin) != prim::rNone;
	}
	
	/** join two AABBs and return the result
	 */
	static const TAabb aabbJoin(const TAabb& a, const TAabb& b) 
	{ 
		return a + b; 
	}
	
	/** return the minimum corner of the AABB
	 */
	static const TPoint aabbMin(const TAabb& aabb) 
	{ 
		return aabb.min(); 
	}

	/** return the maximum corner of the AABB 
	 */
	static const TPoint aabbMax(const TAabb& aabb) 
	{ 
		return aabb.max(); 
	}

	/** return surface area of the AABB
	 */
	static TValue aabbSurfaceArea(const TAabb& aabb)
	{
		return aabbSurfaceArea(aabb, meta::Int<dimension>());
	}


	// RAY

	/** return the support point of the ray
	 */
	static const TPoint raySupport(const TRay& ray)
	{
		return ray.support();
	}

	/** return the direction vector of the ray
	 */
	static const TVector rayDirection(const TRay& ray)
	{
		return ray.direction();
	}

	/** return a point along the ray
	 */
	static const TPoint rayPoint(const TRay& ray, TParam t)
	{
		return ray.point(t);
	}


	// POINTS AND VECTORS

	/** return the @a axis coordinate value of @a point.
	 */
	static const TValue coord(const TPoint& point, size_t axis) 
	{ 
		return point[axis]; 
	}

	/** set the @a axis coordinate value of @a point.
	 */
	static void coord(TPoint& point, size_t axis, TParam value) 
	{ 
		point[axis] = value;
	}

	/** return the @a axis component value of @a vector.
	 */
	static const TValue coord(const TVector& vector, size_t axis)
	{
		return vector[axis];
	}

	/** set the @a axis component value of @a vector.
	 */
	static void coord(TVector& vector, size_t axis, TParam value)
	{
		vector[axis] = value;
	}

	/** return the reciprocal vector of @a vector
	 */
	static const TVector vectorReciprocal(const TVector& vector)
	{
		return vector.reciprocal();
	}

private:

	static TValue aabbSurfaceArea(const TAabb& aabb, meta::Int<2>)
	{
		return aabb.perimeter();
	}
	static TValue aabbSurfaceArea(const TAabb& aabb, meta::Int<3>)
	{
		return aabb.area();
	}
};



template 
<
	typename ObjectType, 
	typename AabbType = typename ObjectType::TAabb, 
	typename RayType = typename ObjectType::TRay,
	typename ObjectIterator = const ObjectType*
>
struct DefaultObjectTraits: public DefaultAabbRayTraits<AabbType, RayType>
{
	typedef ObjectType TObject;	/**< type of nD object */
	typedef ObjectIterator TObjectIterator;	/**< iterator to object */
	typedef const TObject& TObjectReference;	/**< const reference to object */
	typedef void TInfo;	/**< extra info for contain/intersect operations */
	
	typedef typename DefaultAabbRayTraits<AabbType, RayType>::TAabb TAabb;
	typedef typename DefaultAabbRayTraits<AabbType, RayType>::TRay TRay;
	typedef typename DefaultAabbRayTraits<AabbType, RayType>::TPoint TPoint;
	typedef typename DefaultAabbRayTraits<AabbType, RayType>::TVector TVector;
	typedef typename DefaultAabbRayTraits<AabbType, RayType>::TValue TValue;
	typedef typename DefaultAabbRayTraits<AabbType, RayType>::TParam TParam;
	typedef typename DefaultAabbRayTraits<AabbType, RayType>::TReference TReference;
	typedef typename DefaultAabbRayTraits<AabbType, RayType>::TConstReference TConstReference;

	/** return reference to object
	 */
	static TObjectReference object(TObjectIterator it)
	{
		return *it;
	}

	/** return the AABB of an object
	 */
	static const TAabb objectAabb(TObjectIterator it) 
	{ 
		return aabb(object(it));
	}
	
	/** return true if object contains a point, return false otherwise
	 */
	static bool objectContains(TObjectIterator it, const TPoint& point, const TInfo* /*iInfo*/) 
	{ 
		return object(it).contains(point); 
	}

	/** return true if object is intersected by ray
	 */
	static bool objectIntersect(TObjectIterator it, const TRay& ray, TReference t, TParam tMin, const TInfo* /*iInfo*/)
	{
		return intersect(object(it), ray, t, tMin) != prim::rNone;
	}

	/** return true if object is intersected by ray
	 */
	static bool objectIntersects(TObjectIterator it, const TRay& ray, TParam tMin, TParam tMax, const TInfo* /*iInfo*/)
	{
		TValue t;
		return intersect(object(it), ray, t, tMin) != prim::rNone && t < tMax;
	}

	/** return true if part of the object is inside the bounding box
	 */
	static bool objectIntersects(TObjectIterator it, const TAabb& aabb, const TInfo* /*iInfo*/)
	{
		return intersects(object(it), aabb);
	}

	/** return squared distance between object and point)
	 */
	static const TValue objectSquaredDistance(TObjectIterator it, const TPoint& point, const TInfo* /* info */)
	{
		return squaredDistance(object(it), point);
	}

	/** return surface area of object
	 */
	static TValue objectSurfaceArea(TObjectIterator it)
	{
		return object(it).area();
	}
};



}

}

#endif

// EOF
