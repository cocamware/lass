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



/** @class lass::spat::DefaultObjectTraits
 *  @brief default traits for objects to be stored in spatial subdivision trees
 *  @author Bram de Greve [BdG]
 */

#ifndef LASS_GUARDIAN_OF_INCLUSION_SPAT_DEFAULT_OBJECT_TRAITS_H
#define LASS_GUARDIAN_OF_INCLUSION_SPAT_DEFAULT_OBJECT_TRAITS_H

#include "spat_common.h"
#include "../prim/result.h"
#include "../meta/is_same.h"
#include "../meta/wrap.h"

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
	typedef typename TAabb::TNumTraits TNumTraits;
	
	enum { dimension = TAabb::dimension };		/**< nD = number of dimensions of TPoint */

	// AABB

	/** return empty AABB
	 */
	static const TAabb aabbEmpty()
	{
		return TAabb();
	}

	/** return true if AABB contains a point, return false otherwise
	 */
	static const bool aabbContains(const TAabb& aabb, const TPoint& point) 
	{ 
		return aabb.contains(point); 
	}

	/** return true if AABB is intersected by ray
	 */
	static const bool aabbIntersect(const TAabb& aabb, const TRay& ray, TReference t, const TParam tMin)
	{
		return intersect(aabb, ray, t, tMin) != prim::rNone;
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



	// POINTS AND VECTORS

	/** return the @a axis coordinate value of @a point.
	 */
	static const TValue pointCoordinate(const TPoint& point, size_t axis) 
	{ 
		return point[axis]; 
	}

	/** return the @a axis component value of @a vector.
	 */
	static const TValue vectorComponent(const TVector& vector, size_t axis)
	{
		return vector[axis];
	}

	/** return the reciprocal vector of @a vector
	 */
	static const TVector vectorReciprocal(const TVector& vector)
	{
		return vector.reciprocal();
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
	typedef typename DefaultAabbRayTraits<AabbType, RayType>::TNumTraits TNumTraits;

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
	static const bool objectContains(TObjectIterator it, const TPoint& point, const TInfo* /*iInfo*/) 
	{ 
		return object(it).contains(point); 
	}

	/** return true if object is intersected by ray
	 */
	static const bool objectIntersect(TObjectIterator it, const TRay& ray, TReference t, TParam tMin, const TInfo* /*iInfo*/)
	{
		return intersect(object(it), ray, t, tMin) != prim::rNone;
	}

	/** return true if object is intersected by ray
	 */
	static const bool objectIntersects(TObjectIterator it, const TRay& ray, TParam tMin, TParam tMax, const TInfo* /*iInfo*/)
	{
		TValue t;
		return intersect(object(it), ray, t, tMin) != prim::rNone && t < tMax;
	}
};



}

}

#endif

// EOF
