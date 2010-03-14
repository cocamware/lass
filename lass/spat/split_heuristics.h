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
	DefaultSplitHeuristics(size_t maxObjectsPerLeaf = 10, size_t maxDepth = 16): 
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
		size_t axis = 0;
		TValue maxDistance = ObjectTraits::coord(max, 0) - ObjectTraits::coord(min, 0);
		for (size_t k = 1; k < ObjectTraits::dimension; ++k)
		{
			const TValue distance = ObjectTraits::coord(max, k) - ObjectTraits::coord(min, k);
			if (distance > maxDistance)
			{
				axis = k;
				maxDistance = distance;
			}
		}

		const TValue x = (ObjectTraits::coord(min, axis) + ObjectTraits::coord(max, axis)) / 2;
		return SplitInfo<ObjectTraits>(aabb, x, axis);
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
		typedef ObjectTraits OT;
		const typename OT::TValue x = 
			(OT::coord(OT::aabbMin(input.aabb), split_.axis) + OT::coord(OT::aabbMax(input.aabb), split_.axis)) / 2;
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
		typedef ObjectTraits OT;
		const typename OT::TValue xa = 
			(OT::coord(OT::aabbMin(a.aabb), axis_) + OT::coord(OT::aabbMax(a.aabb), axis_)) / 2;
		const typename OT::TValue xb = 
			(OT::coord(OT::aabbMin(b.aabb), axis_) + OT::coord(OT::aabbMax(b.aabb), axis_)) / 2;
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
