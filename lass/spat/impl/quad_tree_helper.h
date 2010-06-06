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



/** @class lass::spat::QuadTree
 *  A spatial container for generic objects.  The object needs a traits class which
 *  contains the necessary functions to perform the quad tree management for the
 *  particular ObjectType.  The traits class needs as a basis the following interface:
 *  <tt>
 *      static TAabb aabb(const TSimplePolygon3D& iP);
 *      static bool contains( const TSimplePolygon3D& iP, const TPoint& point)
 *  </tt>
 *  The above functions are only examples.  The dimensionality of the primitives must
 *  match but can be of any order.  So the quad tree can be used to classify in
 *  2 and 3 dimensions.  In three dimensions the more common name is OctTree.
 *
 *  Higher level divisions can in theory be supported but the dimensional specific
 *  part must be reimplemented.  Altough this is only 2 functions and could be written
 *  generally this is not yet available.
 *
 *  @brief a Quad tree for general objects
 *  @author Tom De Muer [TDM]
 */

#ifndef LASS_GUARDIAN_OF_INCLUSION_SPAT_IMPL_QUAD_TREE_HELPER_H
#define LASS_GUARDIAN_OF_INCLUSION_SPAT_IMPL_QUAD_TREE_HELPER_H

#include "../spat_common.h"
#include "../../prim/point_2d.h"
#include "../../prim/point_3d.h"

namespace lass
{
namespace spat
{
namespace impl
{
		
template <typename ObjectTraits, typename PointType>
class QuadTreeHelper
{
	enum { dimension = ObjectTraits::dimension };
protected:
	typedef ObjectTraits TObjectTraits;
	typedef typename ObjectTraits::TPoint TPoint;
	typedef typename ObjectTraits::TVector TVector;
	typedef typename ObjectTraits::TValue TValue;

	static TValue minComponent(const TVector& v)
	{
		TValue best = TObjectTraits::coord(v, 0);
		for (size_t k = 1; k < dimension; ++k)
		{
			best = std::min(best, TObjectTraits::coord(v, k));
		}
		return best;
	}

	static TValue maxComponent(const TVector& v)
	{
		TValue best = TObjectTraits::coord(v, 0);
		for (size_t k = 1; k < dimension; ++k)
		{
			best = std::max(best, TObjectTraits::coord(v, k));
		}
		return best;
	}

	template <typename V>
	static const V middle(const V& a, const V& b)
	{
		V result;
		for (size_t k = 0; k < dimension; ++k)
		{
			TObjectTraits::coord(result, k, (TObjectTraits::coord(a, k) + TObjectTraits::coord(b, k)) / 2);
		}
		return result;
	}

	static const TValue squaredDistance(const TPoint& a, const TPoint& b)
	{
		TValue d = 0;
		for (size_t k = 0; k < dimension; ++k)
		{
			d += num::sqr(TObjectTraits::coord(a, k) - TObjectTraits::coord(b, k));
		}
		return d;
	}

	static size_t entryNode(const TVector& tNear, const TVector& tMiddle)
	{
		const TValue tEntry = maxComponent(tNear);
		size_t iEntry = 0;
		for (size_t k = 0, mask = 1; k < dimension; ++k, mask *= 2)
		{
			iEntry |= (TObjectTraits::coord(tMiddle, k) < tEntry) ? mask : 0;
		}
		return iEntry;
	}

	static size_t nextNode(size_t i, const TVector& tFar)
	{
		size_t nextMask = 1;
		TValue min = TObjectTraits::coord(tFar, 0);
		for (size_t k = 1, mask = 2; k < dimension; ++k, mask *= 2)
		{
			const TValue x = TObjectTraits::coord(tFar, k);
			if (x < min)
			{
				min = x;
				nextMask = mask;
			}
		}
		if (i & nextMask)
		{
			return size_t(-1);
		}
		return i | nextMask;
	}

	static size_t findSubNode(const TPoint& center, const TPoint& point)
	{
		size_t i = 0;
		for (size_t k = 0, mask = 1; k < dimension; ++k, mask *= 2)
		{
			i |= TObjectTraits::coord(point, k) >= TObjectTraits::coord(center, k) ? mask : 0;
		}
		return i;
	}

	static size_t forcePositiveDirection(const TPoint& center, TPoint& support, TVector& direction)
	{
		size_t flipMask = 0;
		for (size_t k = 0, mask = 1; k < dimension; ++k, mask *= 2)
		{
			const TValue d = TObjectTraits::coord(direction, k);
			if (d < 0)
			{
				TObjectTraits::coord(support, k, 
					2 * TObjectTraits::coord(center, k) - TObjectTraits::coord(support, k));
				TObjectTraits::coord(direction, k, -d);
				flipMask |= mask;
			}
		}
		return flipMask;
	}

	static void nearAndFar(const TPoint& min, const TPoint& max, const TPoint& support, 
		const TVector& reciprocalDirection, TVector& tNear, TVector& tFar)
	{
		for (size_t k = 0; k < dimension; ++k)
		{
			TObjectTraits::coord(tNear, k, TObjectTraits::coord(reciprocalDirection, k) *
				(TObjectTraits::coord(min, k) - TObjectTraits::coord(support, k)));
			TObjectTraits::coord(tFar, k, TObjectTraits::coord(reciprocalDirection, k) *
				(TObjectTraits::coord(max, k) - TObjectTraits::coord(support, k)));
		}
	}

	static void childNearAndFar(TVector& tNear, TVector& tFar, const TVector& tMiddle, size_t iChild)
	{
		for (size_t k = 0, mask = 1; k < dimension; ++k, mask *= 2)
		{
			if (iChild & mask)
			{
				TObjectTraits::coord(tNear, k, TObjectTraits::coord(tMiddle, k));
			}
			else
			{
				TObjectTraits::coord(tFar, k, TObjectTraits::coord(tMiddle, k));
			}
		}
	}
};


//*
template <typename ObjectTraits, typename T>
class QuadTreeHelper<ObjectTraits, prim::Point2D<T> >
{
protected:
	typedef ObjectTraits TObjectTraits;
	typedef typename ObjectTraits::TPoint TPoint;
	typedef typename ObjectTraits::TVector TVector;
	typedef typename ObjectTraits::TValue TValue;
	enum { dimension = 2 };

	static TValue minComponent(const TVector& v)
	{
		return std::min(v.x, v.y);
	}

	static TValue maxComponent(const TVector& v)
	{
		return std::max(v.x, v.y);
	}

	template <typename V>
	static const V middle(const V& a, const V& b)
	{
		return V((a.x + b.x) / 2, (a.y + b.y) / 2);
	}

	static const TValue squaredDistance(const TPoint& a, const TPoint& b)
	{
		return prim::squaredDistance(a, b);
	}

	static size_t entryNode(const TVector& tNear, const TVector& tMiddle)
	{
		const TValue tEntry = maxComponent(tNear);
		return (tMiddle.x < tEntry ? 0x1 : 0) | (tMiddle.y < tEntry ? 0x2 : 0);
	}

	static size_t nextNode(size_t i, const TVector& tFar)
	{
		if (tFar.x <  tFar.y)
		{
			return i & 0x1 ? size_t(-1) : i | 0x1;
		}
		else
		{
			return i & 0x2 ? size_t(-1) : i | 0x2;
		}
	}

	static size_t findSubNode(const TPoint& center, const TPoint& point)
	{
		return (point.x >= center.x ? 0x1 : 0x0) | (point.y >= center.y ? 0x2 : 0x0);
	}

	static size_t forcePositiveDirection(const TPoint& center, TPoint& support, TVector& direction)
	{
		size_t flipMask = 0;
		if (direction.x < 0)
		{
			support.x = 2 * center.x - support.x;
			direction.x = -direction.x;
			flipMask |= 0x1;
		}
		if (direction.y < 0)
		{
			support.y = 2 * center.y - support.y;
			direction.y = -direction.y;
			flipMask |= 0x2;
		}
		return flipMask;
	}

	static void nearAndFar(const TPoint& min, const TPoint& max, const TPoint& support, 
		const TVector& reciprocalDirection, TVector& tNear, TVector& tFar)
	{
		tNear = reciprocalDirection * (min - support);
		tFar = reciprocalDirection * (max - support);
	}

	static void childNearAndFar(TVector& tNear, TVector& tFar, const TVector& tMiddle, size_t iChild)
	{
		(iChild & 0x1 ? tNear : tFar).x = tMiddle.x;
		(iChild & 0x2 ? tNear : tFar).y = tMiddle.y;
	}
};



template <typename ObjectTraits, typename T>
class QuadTreeHelper<ObjectTraits, prim::Point3D<T> >
{
	enum { dimension = 3 };
protected:
	typedef ObjectTraits TObjectTraits;
	typedef typename ObjectTraits::TPoint TPoint;
	typedef typename ObjectTraits::TVector TVector;
	typedef typename ObjectTraits::TValue TValue;

	static TValue minComponent(const TVector& v)
	{
		return std::min(std::min(v.x, v.y), v.z);
	}

	static TValue maxComponent(const TVector& v)
	{
		return std::max(std::max(v.x, v.y), v.z);
	}

	template <typename V>
	static const V middle(const V& a, const V& b)
	{
		return V(
			(a.x + b.x) / 2,
			(a.y + b.y) / 2,
			(a.z + b.z) / 2);
	}

	static const TValue squaredDistance(const TPoint& a, const TPoint& b)
	{
		return prim::squaredDistance(a, b);
	}

	static size_t entryNode(const TVector& tNear, const TVector& tMiddle)
	{
		const TValue tEntry = maxComponent(tNear);
		return (tMiddle.x < tEntry ? 0x1 : 0)
			| (tMiddle.y < tEntry ? 0x2 : 0)
			| (tMiddle.z < tEntry ? 0x4 : 0);
	}

	static size_t nextNode(size_t i, const TVector& tFar)
	{
		if (tFar.x < tFar.y && tFar.x < tFar.z)
		{
			return i & 0x1 ? size_t(-1) : i | 0x1;
		}
		else if (tFar.y < tFar.z)
		{
			return i & 0x2 ? size_t(-1) : i | 0x2;
		}
		else
		{
			return i & 0x4 ? size_t(-1) : i | 0x4;
		}
	}

	static size_t findSubNode(const TPoint& center, const TPoint& point)
	{
		return (point.x >= center.x ? 0x1 : 0x0) 
			| (point.y >= center.y ? 0x2 : 0x0)
			| (point.z >= center.z ? 0x4 : 0x0);
	}

	static size_t forcePositiveDirection(const TPoint& center, TPoint& support, TVector& direction)
	{
		size_t flipMask = 0;
		for (size_t k = 0, mask = 1; k < dimension; ++k, mask *= 2)
		{
			if (direction[k] < 0)
			{
				support[k] = 2 * center[k] - support[k];
				direction[k] = -direction[k];
				flipMask |= mask;
			}
		}
		return flipMask;
	}

	static void nearAndFar(const TPoint& min, const TPoint& max, const TPoint& support, 
		const TVector& reciprocalDirection, TVector& tNear, TVector& tFar)
	{
		tNear = reciprocalDirection * (min - support);
		tFar = reciprocalDirection * (max - support);
	}

	static void childNearAndFar(TVector& tNear, TVector& tFar, const TVector& tMiddle, size_t iChild)
	{
		(iChild & 0x1 ? tNear : tFar).x = tMiddle.x;
		(iChild & 0x2 ? tNear : tFar).y = tMiddle.y;
		(iChild & 0x4 ? tNear : tFar).z = tMiddle.z;
	}
};
/**/
}
}
}

#endif

// EOF
