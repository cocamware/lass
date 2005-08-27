/** @file
 *  @author Bram de Greve (bramz@users.sourceforge.net)
 *  @author Tom De Muer (tomdemuer@users.sourceforge.net)
 *
 *  Distributed under the terms of the GPL (GNU Public License)
 *
 *  The LASS License:
 *
 *  Copyright 2004 Bram de Greve and Tom De Muer
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
#include "../meta/select.h"

namespace lass
{
namespace spat
{

template <typename ObjectType, typename AabbType, typename RayType>
struct DefaultObjectTraits
{
	typedef ObjectType TObject;					/**< type of object */
	typedef AabbType TAabb;						/**< an nD AABB */
	typedef RayType TRay;						/**< an nD Ray */

	typedef const TObject* TObjectIterator;		/**< iterator to object */
	typedef const TObject& TObjectReference;	/**< reference to object */

	typedef typename TAabb::TPoint TPoint;		/**< an nD point */
	typedef typename TAabb::TVector TVector;	/**< an nD vector */
	typedef typename TAabb::TValue TValue;		/**< numerical type used in TPoint and TVector */
	typedef typename TAabb::TParam TParam;		/**< best type for function parameters of TValue */
	typedef typename TAabb::TReference TReference;	/**< reference to TValue */
	typedef typename TAabb::TConstReference TConstReference; /**< const reference to TValue */
	
	enum { dimension = TAabb::dimension }; /**< number of dimensions of TPoint */



	/** return the AABB of an object
	 */
	static const TAabb aabb(TObjectIterator iObject) 
	{ 
		return prim::aabb(*iObject); 
	}
	
	/** return true if object contains a point, return false otherwise
	 */
	static const bool contains(TObjectIterator iObject, const TPoint& iPoint) 
	{ 
		return iObject->contains(iPoint); 
	}

	/** return true if object is intersected by ray
	 */
	static const bool intersect(TObjectIterator iObject, const TRay& iRay, TReference oT)
	{
		TValue dummy;
		return prim::intersect(iRay, *iObject, oT, dummy) != prim::rNone;
	}
	



	/** return true if AABB contains a point, return false otherwise
	 */
	static const bool contains(const TAabb& iAabb, const TPoint& iPoint) 
	{ 
		return iAabb.contains(iPoint); 
	}

	/** return true if AABB is intersected by ray
	 */
	static const bool intersect(const TAabb& iAabb, const TRay& iRay, TReference oT)
	{
		return prim::intersect(iAabb, iRay, oT) != prim::rNone;
	}
	
	/** join two AABBs and return the result
	 */
	static const TAabb join(const TAabb& iA, const TAabb& iB) 
	{ 
		return iA + iB; 
	}
	
	/** return the minimum corner of the AABB
	 */
	static const TPoint min(const TAabb& iAabb) 
	{ 
		return iAabb.min(); 
	}

	/** return the maximum corner of the AABB 
	 */
	static const TPoint max(const TAabb& iAabb) 
	{ 
		return iAabb.max(); 
	}

	/** return the @a iAxis component value of @a iPoint.
	 */
	static const TValue component(const TPoint& iPoint, size_t iAxis) 
	{ 
		return iPoint[iAxis]; 
	}
};



}

}

#endif

// EOF
