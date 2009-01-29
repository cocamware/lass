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

namespace lass
{
namespace spat
{
namespace impl
{
		
template <typename ObjectTraits, size_t dimension>
class QuadTreeHelper
{
protected:
	typedef ObjectTraits TObjectTraits;
	typedef typename ObjectTraits::TPoint TPoint;
	typedef typename ObjectTraits::TVector TVector;
	typedef typename ObjectTraits::TValue TValue;

	const TValue minComponent(const TVector& v) const
	{
		TValue best = TObjectTraits::coord(v, 0);
		for (size_t k = 1; k < dimension; ++k)
		{
			best = std::min(best, TObjectTraits::coord(v, k));
		}
		return best;
	}

	const TValue maxComponent(const TVector& v) const
	{
		TValue best = TObjectTraits::coord(v, 0);
		for (size_t k = 1; k < dimension; ++k)
		{
			best = std::max(best, TObjectTraits::coord(v, k));
		}
		return best;
	}

	template <typename V>
	const V middle(const V& a, const V& b) const
	{
		V result;
		for (size_t k = 0; k < dimension; ++k)
		{
			TObjectTraits::coord(result, k, (TObjectTraits::coord(a, k) + TObjectTraits::coord(b, k)) / 2);
		}
		return result;
	}

	const TVector subtract(const TPoint& a, const TPoint& b) const
	{
		TVector result;
		for (size_t k = 0; k < dimension; ++k)
		{
			TObjectTraits::coord(result, k, TObjectTraits::coord(a, k) - TObjectTraits::coord(b, k));
		}
		return result;
	}

	const size_t entryNode(const TVector& tNear, const TVector& tMiddle) const
	{
		const TValue tEntry = maxComponent(tNear);
		size_t iEntry = 0;
		for (size_t k = 0, mask = 1; k < dimension; ++k, mask *= 2)
		{
			iEntry |= (TObjectTraits::coord(tMiddle, k) < tEntry) ? mask : 0;
		}
		return iEntry;
	}

	const size_t nextNode(size_t i, const TVector& tFar) const
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

	const size_t findSubNode(const TPoint& center, const TPoint& point) const
	{
		size_t i = 0;
		for (size_t k = 0, mask = 1; k < dimension; ++k, mask *= 2)
		{
			i |= TObjectTraits::coord(point, k) >= TObjectTraits::coord(center, k) ? mask : 0;
		}
		return i;
	}

	const size_t forcePositiveDirection(const TPoint& center, TPoint& support, TVector& direction) const
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

	void nearAndFar(const TPoint& min, const TPoint& max, const TPoint& support, 
		const TVector& reciprocalDirection, TVector& tNear, TVector& tFar) const
	{
		for (size_t k = 0; k < dimension; ++k)
		{
			TObjectTraits::coord(tNear, k, TObjectTraits::coord(reciprocalDirection, k) *
				(TObjectTraits::coord(min, k) - TObjectTraits::coord(support, k)));
			TObjectTraits::coord(tFar, k, TObjectTraits::coord(reciprocalDirection, k) *
				(TObjectTraits::coord(max, k) - TObjectTraits::coord(support, k)));
		}
	}

	void childNearAndFar(TVector& tNear, TVector& tFar, const TVector& tMiddle, size_t iChild) const
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

	template <typename QuadNodeType> 
	static void buildSubNodes(QuadNodeType* parentNode)
	{
		TVector newExtents(parentNode->extents);
		for (size_t k = 0; k < dimension; ++k)
		{
			TObjectTraits::coord(newExtents, k, TObjectTraits::coord(newExtents, k) / 2);
		}

		const size_t nodeCount = 1 << dimension;
		for (size_t i = 0; i < nodeCount; ++i)
		{
			TPoint newCenter = parentNode->center;
			for (size_t k = 0, mask = 1; k < dimension; ++k, mask *= 2)
			{
				TObjectTraits::coord(newCenter, k, 
					TObjectTraits::coord(newCenter, k) + (i & mask ? 1 : -1) * TObjectTraits::coord(newExtents, k));
			}
			parentNode->node[i] = new QuadNodeType(newCenter, newExtents);
		}
	}
};



template <typename ObjectTraits>
class QuadTreeHelper<ObjectTraits, 2>
{
protected:
	typedef ObjectTraits TObjectTraits;
	typedef typename ObjectTraits::TPoint TPoint;
	typedef typename ObjectTraits::TVector TVector;
	typedef typename ObjectTraits::TValue TValue;
	enum { dimension = 2 };

	const TValue minComponent(const TVector& v) const
	{
		return std::min(TObjectTraits::coord(v, 0), TObjectTraits::coord(v, 1));
	}

	const TValue maxComponent(const TVector& v) const
	{
		return std::max(TObjectTraits::coord(v, 0), TObjectTraits::coord(v, 1));
	}

	template <typename V>
	const V middle(const V& a, const V& b) const
	{
		return V(
			(TObjectTraits::coord(a, 0) + TObjectTraits::coord(b, 0)) / 2,
			(TObjectTraits::coord(a, 1) + TObjectTraits::coord(b, 1)) / 2);
	}

	const TVector subtract(const TPoint& a, const TPoint& b) const
	{
		return TVector(
			TObjectTraits::coord(a, 0) - TObjectTraits::coord(b, 0),
			TObjectTraits::coord(a, 1) - TObjectTraits::coord(b, 1));
	}

	const size_t entryNode(const TVector& tNear, const TVector& tMiddle) const
	{
		if (TObjectTraits::coord(tNear, 0) > TObjectTraits::coord(tNear, 1))
		{
			if (TObjectTraits::coord(tMiddle, 1) < TObjectTraits::coord(tNear, 0))
			{
				return 0x2;
			}
		}
		else
		{
			if (TObjectTraits::coord(tMiddle, 0) < TObjectTraits::coord(tNear, 1))
			{
				return 0x1;
			}
		}
		return 0x0;
	}

	const size_t nextNode(size_t i, const TVector& tFar) const
	{
		if (TObjectTraits::coord(tFar, 0) <  TObjectTraits::coord(tFar, 1))
		{
			return i & 0x1 ? size_t(-1) : i | 0x1;
		}
		else
		{
			return i & 0x2 ? size_t(-1) : i | 0x2;
		}
	}

	const size_t findSubNode(const TPoint& center, const TPoint& point) const
	{
		return (TObjectTraits::coord(point, 0) >= TObjectTraits::coord(center, 0) ? 0x1 : 0x0) 
			| (TObjectTraits::coord(point, 1) >= TObjectTraits::coord(center, 1) ? 0x2 : 0x0);
	}

	const size_t forcePositiveDirection(const TPoint& center, TPoint& support, TVector& direction) const
	{
		size_t flipMask = 0;
		const TValue dx = TObjectTraits::coord(direction, 0);
		if (dx < 0)
		{
			TObjectTraits::coord(support, 0, 2 * TObjectTraits::coord(center, 0) - TObjectTraits::coord(support, 0));
			TObjectTraits::coord(direction, 0, -dx);
			flipMask |= 0x1;
		}
		const TValue dy = TObjectTraits::coord(direction, 1);
		if (dy < 0)
		{
			TObjectTraits::coord(support, 1, 2 * TObjectTraits::coord(center, 1) - TObjectTraits::coord(support, 1));
			TObjectTraits::coord(direction, 1, -dy);
			flipMask |= 0x2;
		}
		return flipMask;
	}

	void nearAndFar(const TPoint& min, const TPoint& max, const TPoint& support, 
		const TVector& reciprocalDirection, TVector& tNear, TVector& tFar) const
	{
		TObjectTraits::coord(tNear, 0, TObjectTraits::coord(reciprocalDirection, 0) * (TObjectTraits::coord(min, 0) - TObjectTraits::coord(support, 0)));
		TObjectTraits::coord(tNear, 1, TObjectTraits::coord(reciprocalDirection, 1) * (TObjectTraits::coord(min, 1) - TObjectTraits::coord(support, 1)));
		TObjectTraits::coord(tFar, 0, TObjectTraits::coord(reciprocalDirection, 0) * (TObjectTraits::coord(max, 0) - TObjectTraits::coord(support, 0)));
		TObjectTraits::coord(tFar, 1, TObjectTraits::coord(reciprocalDirection, 1) * (TObjectTraits::coord(max, 1) - TObjectTraits::coord(support, 1)));
	}

	void childNearAndFar(TVector& tNear, TVector& tFar, const TVector& tMiddle, size_t iChild) const
	{
		TObjectTraits::coord(iChild & 0x1 ? tNear : tFar, 0, TObjectTraits::coord(tMiddle, 0));
		TObjectTraits::coord(iChild & 0x2 ? tNear : tFar, 1, TObjectTraits::coord(tMiddle, 1));
	}

	template <typename QuadNodeType> 
	static void buildSubNodes(QuadNodeType* parentNode)
	{
		TVector newExtents(parentNode->extents);
		TObjectTraits::coord(newExtents, 0, TObjectTraits::coord(newExtents, 0) / 2);
		TObjectTraits::coord(newExtents, 1, TObjectTraits::coord(newExtents, 1) / 2);
		
		for (size_t i = 0; i < 4; ++i)
		{
			TPoint newCenter = parentNode->center;
			TObjectTraits::coord(newCenter, 0, 
				TObjectTraits::coord(newCenter, 0) + (i & 0x1 ? 1 : -1) * TObjectTraits::coord(newExtents, 0));
			TObjectTraits::coord(newCenter, 1, 
				TObjectTraits::coord(newCenter, 1) + (i & 0x2 ? 1 : -1) * TObjectTraits::coord(newExtents, 1));
			parentNode->node[i] = new QuadNodeType(newCenter, newExtents);
		}
	}
};



template <typename ObjectTraits>
class QuadTreeHelper<ObjectTraits, 3>
{
	enum { dimension = 3 };
protected:
	typedef ObjectTraits TObjectTraits;
	typedef typename ObjectTraits::TPoint TPoint;
	typedef typename ObjectTraits::TVector TVector;
	typedef typename ObjectTraits::TValue TValue;

	const TValue minComponent(const TVector& v) const
	{
		return std::min(std::min(TObjectTraits::coord(v, 0), TObjectTraits::coord(v, 1)), TObjectTraits::coord(v, 2));
	}

	const TValue maxComponent(const TVector& v) const
	{
		return std::max(std::max(TObjectTraits::coord(v, 0), TObjectTraits::coord(v, 1)), TObjectTraits::coord(v, 2));
	}

	template <typename V>
	const V middle(const V& a, const V& b) const
	{
		return V(
			(TObjectTraits::coord(a, 0) + TObjectTraits::coord(b, 0)) / 2,
			(TObjectTraits::coord(a, 1) + TObjectTraits::coord(b, 1)) / 2,
			(TObjectTraits::coord(a, 2) + TObjectTraits::coord(b, 2)) / 2);
	}

	const TVector subtract(const TPoint& a, const TPoint& b) const
	{
		return TVector(
			TObjectTraits::coord(a, 0) - TObjectTraits::coord(b, 0),
			TObjectTraits::coord(a, 1) - TObjectTraits::coord(b, 1),
			TObjectTraits::coord(a, 2) - TObjectTraits::coord(b, 2));
	}

	const size_t entryNode(const TVector& tNear, const TVector& tMiddle) const
	{
		const TValue tEntry = maxComponent(tNear);
		size_t iEntry = 0;
		for (size_t k = 0, mask = 1; k < dimension; ++k, mask *= 2)
		{
			iEntry |= (TObjectTraits::coord(tMiddle, k) < tEntry) ? mask : 0;
		}
		return iEntry;
	}

	const size_t nextNode(size_t i, const TVector& tFar) const
	{
		const TValue x = TObjectTraits::coord(tFar, 0);
		const TValue y = TObjectTraits::coord(tFar, 1);
		const TValue z = TObjectTraits::coord(tFar, 2);
		if (x < y && x < z)
		{
			return i & 0x1 ? size_t(-1) : i | 0x1;
		}
		else if (y < z)
		{
			return i & 0x2 ? size_t(-1) : i | 0x2;
		}
		else
		{
			return i & 0x4 ? size_t(-1) : i | 0x4;
		}
	}

	const size_t findSubNode(const TPoint& center, const TPoint& point) const
	{
		return (TObjectTraits::coord(point, 0) >= TObjectTraits::coord(center, 0) ? 0x1 : 0x0) 
			| (TObjectTraits::coord(point, 1) >= TObjectTraits::coord(center, 1) ? 0x2 : 0x0)
			| (TObjectTraits::coord(point, 2) >= TObjectTraits::coord(center, 2) ? 0x4 : 0x0);
	}

	const size_t forcePositiveDirection(const TPoint& center, TPoint& support, TVector& direction) const
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

	void nearAndFar(const TPoint& min, const TPoint& max, const TPoint& support, 
		const TVector& reciprocalDirection, TVector& tNear, TVector& tFar) const
	{
		for (size_t k = 0; k < dimension; ++k)
		{
			TObjectTraits::coord(tNear, k, TObjectTraits::coord(reciprocalDirection, k) *
				(TObjectTraits::coord(min, k) - TObjectTraits::coord(support, k)));
			TObjectTraits::coord(tFar, k, TObjectTraits::coord(reciprocalDirection, k) *
				(TObjectTraits::coord(max, k) - TObjectTraits::coord(support, k)));
		}
	}

	void childNearAndFar(TVector& tNear, TVector& tFar, const TVector& tMiddle, size_t iChild) const
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

	template <typename QuadNodeType> 
	static void buildSubNodes(QuadNodeType* parentNode)
	{
		TVector newExtents(parentNode->extents);
		TObjectTraits::coord(newExtents, 0, TObjectTraits::coord(newExtents, 0) / 2);
		TObjectTraits::coord(newExtents, 1, TObjectTraits::coord(newExtents, 1) / 2);
		TObjectTraits::coord(newExtents, 2, TObjectTraits::coord(newExtents, 2) / 2);
		
		for (size_t i = 0; i < 8; ++i)
		{
			TPoint newCenter = parentNode->center;
			TObjectTraits::coord(newCenter, 0, 
				TObjectTraits::coord(newCenter, 0) + (i & 0x1 ? 1 : -1) * TObjectTraits::coord(newExtents, 0));
			TObjectTraits::coord(newCenter, 1, 
				TObjectTraits::coord(newCenter, 1) + (i & 0x2 ? 1 : -1) * TObjectTraits::coord(newExtents, 1));
			TObjectTraits::coord(newCenter, 2, 
				TObjectTraits::coord(newCenter, 2) + (i & 0x4 ? 1 : -1) * TObjectTraits::coord(newExtents, 2));
			parentNode->node[i] = new QuadNodeType(newCenter, newExtents);
		}
	}
};

}
}
}

#endif

// EOF
