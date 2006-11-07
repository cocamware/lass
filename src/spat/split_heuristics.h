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

#ifndef LASS_GUARDIAN_OF_INCLUSION_SPAT_SPLIT_HEURISTICS_H
#define LASS_GUARDIAN_OF_INCLUSION_SPAT_SPLIT_HEURISTICS_H

#include "spat_common.h"

namespace lass
{
namespace spat
{

template <typename ObjectTraits>
struct SplitInfo
{
	typedef typename ObjectTraits::TAabb TAabb;
	typedef typename ObjectTraits::TValue TValue;
	typedef int TAxis;

	SplitInfo(const TAabb& iAabb, TValue iX, TAxis iAxis):
		aabb(iAabb), x(iX), axis(iAxis)
	{
	}

	TAabb aabb;
	TValue x;
	TAxis axis;
};

template <int numObjectsPerLeaf = 1>
struct DefaultSplitHeuristics
{
	template <typename ObjectTraits, typename RandomIterator>
	static SplitInfo<ObjectTraits> split(RandomIterator iFirst, RandomIterator iLast)
	{
		typedef typename ObjectTraits::TAabb TAabb;
		typedef typename ObjectTraits::TPoint TPoint;
		typedef typename ObjectTraits::TValue TValue;

		LASS_ASSERT(numObjectsPerLeaf > 0);

		TAabb aabb = ObjectTraits::emptyAabb();
		for (RandomIterator i = iFirst; i != iLast; ++i)
		{
			aabb = ObjectTraits::join(aabb, i->second);
		}

		const int n = static_cast<int>(iLast - iFirst);
		if (n <= numObjectsPerLeaf)
		{
			return SplitInfo<ObjectTraits>(aabb, 0, -1);
		}
		
		const TPoint min = ObjectTraits::min(aabb);
		const TPoint max = ObjectTraits::max(aabb);
		int axis = 0;
		TValue maxDistance = ObjectTraits::coordinate(max, 0) - ObjectTraits::coordinate(min, 0);
		for (int k = 1; k < ObjectTraits::dimension; ++k)
		{
			const TValue distance = 
				ObjectTraits::coordinate(max, k) - ObjectTraits::coordinate(min, k);
			if (distance > maxDistance)
			{
				axis = k;
				maxDistance = distance;
			}
		}

		const TValue x = 
			(ObjectTraits::coordinate(min, axis) + ObjectTraits::coordinate(max, axis)) / 2;
		return SplitInfo<ObjectTraits>(aabb, x, axis);
	}
};

namespace impl
{

template <typename ObjectTraits>
class Splitter
{
public:
	Splitter(const SplitInfo<ObjectTraits>& iSplit): split_(iSplit) {}
	template <typename Input> bool operator()(const Input& iInput) const
	{
		const typename ObjectTraits::TValue x = 
			(ObjectTraits::coordinate(ObjectTraits::min(iInput.second), split_.axis) +
			ObjectTraits::coordinate(ObjectTraits::max(iInput.second), split_.axis)) / 2;
		return x < split_.x;
	}			
private:
	SplitInfo<ObjectTraits> split_;
};

template <typename ObjectTraits>
class LessAxis
{
public:
	LessAxis(int iAxis): axis_(iAxis) {}
	template <typename Input> bool operator()(const Input& a, const Input& b) const
	{
		const typename ObjectTraits::TValue xa = 
			(ObjectTraits::coordinate(ObjectTraits::min(a.second), axis_) +
			ObjectTraits::coordinate(ObjectTraits::max(a.second), axis_)) / 2;
		const typename ObjectTraits::TValue xb = 
			(ObjectTraits::coordinate(ObjectTraits::min(b.second), axis_) +
			ObjectTraits::coordinate(ObjectTraits::max(b.second), axis_)) / 2;
		return xa < xb;
	}			
private:
	int axis_;
};

}

}

}

#endif

// EOF
