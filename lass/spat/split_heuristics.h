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

#ifndef LASS_GUARDIAN_OF_INCLUSION_SPAT_SPLIT_HEURISTICS_H
#define LASS_GUARDIAN_OF_INCLUSION_SPAT_SPLIT_HEURISTICS_H

#include "spat_common.h"

namespace lass
{
namespace spat
{
namespace impl
{
	template <typename ObjectTraits> 
	typename ObjectTraits::TValue aabbCenterForAxis(const typename ObjectTraits::TAabb& box, size_t axis)
	{
		return (ObjectTraits::coord(ObjectTraits::aabbMin(box), axis) + ObjectTraits::coord(ObjectTraits::aabbMax(box), axis)) / 2;
	}

	template <typename ObjectTraits>
	class LessAxis
	{
	public:
		LessAxis(int iAxis): axis_(iAxis) {}
		template <typename Input> bool operator()(const Input& a, const Input& b) const
		{
			return aabbCenterForAxis<ObjectTraits>(a.aabb, axis_) < aabbCenterForAxis<ObjectTraits>(b.aabb, axis_);
		}			
	private:
		int axis_;
	};
}

template <typename ObjectTraits>
struct SplitInfo
{
	typedef typename ObjectTraits::TAabb TAabb;
	typedef typename ObjectTraits::TValue TValue;
	typedef int TAxis;

	SplitInfo(const TAabb& aabb, TValue x, TAxis axis):
		aabb(aabb), x(x), axis(axis)
	{
	}

	TAabb aabb;
	TValue x;
	TAxis axis;
};



class DefaultSplitHeuristics
{
public:
	DefaultSplitHeuristics(size_t maxObjectsPerLeaf, size_t maxDepth): 
		maxObjectsPerLeaf_(maxObjectsPerLeaf),
		maxDepth_(maxDepth)
	{
	}

	size_t maxObjectsPerLeaf() const { return maxObjectsPerLeaf_; }
	size_t maxDepth() const { return maxDepth_; }

protected:

	void swap(DefaultSplitHeuristics& other)
	{
		std::swap(maxObjectsPerLeaf_, other.maxObjectsPerLeaf_);
		std::swap(maxDepth_, other.maxDepth_);
	}

	template <typename ObjectTraits, typename RandomIterator>
	SplitInfo<ObjectTraits> split(RandomIterator first, RandomIterator last)
	{
		typedef typename ObjectTraits::TAabb TAabb;
		typedef typename ObjectTraits::TPoint TPoint;
		typedef typename ObjectTraits::TValue TValue;

		LASS_ASSERT(maxObjectsPerLeaf_ > 0);

		TAabb aabb = ObjectTraits::aabbEmpty();
		for (RandomIterator i = first; i != last; ++i)
		{
			aabb = ObjectTraits::aabbJoin(aabb, i->aabb);
		}

		const size_t n = static_cast<size_t>(last - first);
		if (n <= maxObjectsPerLeaf_)
		{
			return SplitInfo<ObjectTraits>(aabb, 0, -1);
		}
		
		const TPoint min = ObjectTraits::aabbMin(aabb);
		const TPoint max = ObjectTraits::aabbMax(aabb);
		int axis = 0;
		TValue maxDistance = ObjectTraits::coord(max, 0) - ObjectTraits::coord(min, 0);
		for (int k = 1; k < ObjectTraits::dimension; ++k)
		{
			const TValue distance = ObjectTraits::coord(max, k) - ObjectTraits::coord(min, k);
			if (distance > maxDistance)
			{
				axis = k;
				maxDistance = distance;
			}
		}

		const TValue x = impl::aabbCenterForAxis<ObjectTraits>(aabb, axis);
		return SplitInfo<ObjectTraits>(aabb, x, axis);
	}

private:
	size_t maxObjectsPerLeaf_;
	size_t maxDepth_;
};



class SAHSplitHeuristics
{
public:
	SAHSplitHeuristics(size_t maxObjectsPerLeaf, size_t maxDepth): 
		maxObjectsPerLeaf_(maxObjectsPerLeaf),
		maxDepth_(maxDepth)
	{
	}

	size_t maxObjectsPerLeaf() const { return maxObjectsPerLeaf_; }
	size_t maxDepth() const { return maxDepth_; }

protected:

	void swap(SAHSplitHeuristics& other)
	{
		std::swap(maxObjectsPerLeaf_, other.maxObjectsPerLeaf_);
		std::swap(maxDepth_, other.maxDepth_);
	}

	template <typename ObjectTraits, typename RandomIterator>
	SplitInfo<ObjectTraits> split(RandomIterator first, RandomIterator last)
	{
		typedef typename ObjectTraits::TAabb TAabb;
		typedef typename ObjectTraits::TPoint TPoint;
		typedef typename ObjectTraits::TValue TValue;

		const TValue costNode = 1; // must be non zero!
		const TValue costObject = 1000;

		LASS_ASSERT(maxObjectsPerLeaf_ > 0);

		const size_t n = static_cast<size_t>(last - first);
		if (n <= maxObjectsPerLeaf_)
		{
			TAabb aabb = ObjectTraits::aabbEmpty();
			for (RandomIterator i = first; i != last; ++i)
			{
				aabb = ObjectTraits::aabbJoin(aabb, i->aabb);
			}			
			return SplitInfo<ObjectTraits>(aabb, 0, -1);
		}

		TValue bestCost = n * costObject;
		int bestAxis = -1;
		TValue bestX = 0;
		std::vector<TValue> leftArea(n);
		TAabb totalBox = ObjectTraits::aabbEmpty();
		TValue totalArea = 0;

		for (int axis = 0; axis < ObjectTraits::dimension; ++axis)
		{
			std::sort(first, last, impl::LessAxis<ObjectTraits>(axis));

			TAabb box = ObjectTraits::aabbEmpty();
			for (size_t i = 0; i < n; ++i)
			{
				box = ObjectTraits::aabbJoin(box, first[i].aabb);
				leftArea[i] = ObjectTraits::aabbSurfaceArea(box);
			}

			if (axis == 0)
			{
				totalBox = box;
				totalArea = ObjectTraits::aabbSurfaceArea(totalBox);
				if (totalArea == 0)
				{
					SplitInfo<ObjectTraits>(totalBox, 0, -1);
				}
			}

			box = ObjectTraits::aabbEmpty();
			for (size_t i = n; i > 0; --i)
			{
				const TValue rightArea = ObjectTraits::aabbSurfaceArea(box);
				const TValue cost = 2 * costNode + (leftArea[i - 1] * i + rightArea * (n - i)) * costObject / totalArea;
				if (cost < bestCost)
				{
					bestCost = cost;
					bestAxis = axis;
					bestX = impl::aabbCenterForAxis<ObjectTraits>(first[i - 1].aabb, axis);
				}
				box = ObjectTraits::aabbJoin(box, first[i - 1].aabb);
			}
		}
		
		return SplitInfo<ObjectTraits>(totalBox, bestX, bestAxis);
	}

private:

	size_t maxObjectsPerLeaf_;
	size_t maxDepth_;
};



namespace impl
{

template <typename ObjectTraits>
class Splitter
{
public:
	Splitter(const SplitInfo<ObjectTraits>& split): split_(split) {}
	template <typename Input> bool operator()(const Input& input) const
	{
		return aabbCenterForAxis<ObjectTraits>(input.aabb, split_.axis) <= split_.x;
	}			
private:
	SplitInfo<ObjectTraits> split_;
};


}

}

}

#endif

// EOF
