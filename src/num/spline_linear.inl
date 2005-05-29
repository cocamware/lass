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



#ifndef LASS_GUARDIAN_OF_INCLUSION_NUM_SPLINE_LINEAR_INL
#define LASS_GUARDIAN_OF_INCLUSION_NUM_SPLINE_LINEAR_INL

#include "num_common.h"
#include "spline_linear.h"
#include "../stde/extended_iterator.h"

namespace lass
{
namespace num
{

// --- public --------------------------------------------------------------------------------------

template <typename S, typename D, typename T>
template <typename ScalarInputIterator, typename DataInputIterator>
SplineLinear<S, D, T>::SplineLinear(ScalarInputIterator iFirstControl, 
									ScalarInputIterator iLastControl,
									DataInputIterator iFirstData)
{
	while (iFirstControl != iLastControl)
	{
		Node node;
		node.x = *iFirstControl++;
		node.y = *iFirstData++;
		nodes_.push_back(node);
	}
	init();
}



template <typename S, typename D, typename T>
typename const SplineLinear<S, D, T>::TData
SplineLinear<S, D, T>::operator ()(TScalar iX) const
{
	const TNodes::const_iterator n = findNode(iX);
	const TScalar dx = iX - n->x;

	TData result(n->y);
	TDataTraits::multiplyAccumulate(result, n->dy, dx);
	return result;
}



template <typename S, typename D, typename T>
typename const SplineLinear<S, D, T>::TData
SplineLinear<S, D, T>::derivative(TScalar iX) const
{
	return findNode(iX)->dy;
}



template <typename S, typename D, typename T>
typename const SplineLinear<S, D, T>::TData
SplineLinear<S, D, T>::derivative2(TScalar iX) const
{
	TData result;
	TDataTraits::zero(result, dataDimension_);
	return result;
}



template <typename S, typename D, typename T>
typename const SplineLinear<S, D, T>::TData
SplineLinear<S, D, T>::integral(TScalar iBegin, TScalar iEnd) const
{
	TNodeConstIterator first = findNode(iBegin);
	TNodeConstIterator last = findNode(iEnd);
	if (first == last)
	{
		TData result(first->y);
		TDataTraits::scale(result, iEnd - iBegin);
		TDataTraits::multiplyAccumulate(result, first->dy, 
			(num::sqr(iEnd - first->x) - num::sqr(iBegin - first->x)) / 2);
		return result;
	}
	else
	{
		TScalar multiplier = 1;
		if (iEnd < iBegin)
		{
			std::swap(iBegin, iEnd);
			std::swap(first, last);
			multiplier = -1;
		}

		TNodeConstIterator next = stde::next(first);

		TData result(first->y);
		TDataTraits::scale(result, next->x - iBegin);
		TDataTraits::multiplyAccumulate(result, first->dy, 
			(num::sqr(next->x - first->x) - num::sqr(iBegin - first->x)) / 2);
		first = next++;

		while (first != last)
		{
			const TScalar dx = next->x - first->x;
			TDataTraits::multiplyAccumulate(result, first->y, dx);
			TDataTraits::multiplyAccumulate(result, first->dy, num::sqr(dx) / 2);
			first = next++;
		}

		const TScalar dx = iEnd - first->x;
		TDataTraits::multiplyAccumulate(result, first->y, dx);
		TDataTraits::multiplyAccumulate(result, first->dy, num::sqr(dx) / 2);

		TDataTraits::scale(result, multiplier);
		return result;
	}
}




// --- private -------------------------------------------------------------------------------------

template <typename S, typename D, typename T>
void SplineLinear<S, D, T>::init()
{
	// are there any nodes at all?  we need at least two!
	//
	const TNodes::size_type size = nodes_.size();
	if (size < 2)
	{
		LASS_THROW("A linear interpolator needs at least two nodes!  This one only has '"
			<< static_cast<unsigned>(nodes_.size()) << "'.");
	}

	typename TNodes::iterator prev = nodes_.begin();
	dataDimension_ = TDataTraits::dimension(prev->y);
	const typename TNodes::iterator end = nodes_.end();
	for (typename TNodes::iterator i = stde::next(nodes_.begin()); i != end; prev = i++)
	{
		// check for increasing control components
		//
		if (prev->x >= i->x)
		{
			LASS_THROW("Nodes in linear interpolator must have absolutely increasing control "
				"values.");
		}

		// check dimension of data
		//
		if (TDataTraits::dimension(i->y) != dataDimension_)
		{
			LASS_THROW("All data in linear interpolator must be of same dimension.");
		}

		// calculate derivative of previous node
		//
		prev->dy = i->y;
		TDataTraits::multiplyAccumulate(prev->dy, prev->y, -1);
		TDataTraits::scale(prev->dy, num::inv(i->x - prev->x));
	}

	// extend last node with same derivative as last edge.
	stde::prior(end)->dy = stde::prior(end, 2)->dy;
}



/** binary search to find node that belongs to iX
 *
 *  @return
 *  @arg the index @a i if @a iX is in the interval [@c nodes_[i].x, @c nodes_[i+1].x)
 *  @arg 0 if @a iX is smaller than @c nodes_[0].x</tt>
 *  @arg @c nodes_.size()-2 if @a iX is greater than @c nodes_[nodes_.size()-1].x
 *
 *  complexity: O(ln N)
 */
template <typename S, typename D, typename T>
const typename SplineLinear<S, D, T>::TNodeConstIterator
SplineLinear<S, D, T>::findNode(TScalar iX) const
{
	const typename TNodes::size_type size = nodes_.size();
	LASS_ASSERT(nodes_.size() >= 2);

	// early outs
	//
	if (iX < nodes_.front().x)
	{
		return nodes_.begin();
	}
	if (iX >= nodes_.back().x)
	{
		return stde::prior(nodes_.end());
	}

	// binary search
	//
	TNodeConstIterator first = nodes_.begin();
	TNodeConstIterator last = nodes_.end();
	while (stde::next(first) != last)
	{
		TNodeConstIterator middle = stde::next(first, std::distance(first, last) / 2);
		LASS_ASSERT(middle != first && middle != last);

		if (middle->x <= iX)
		{
			first = middle;
		}
		else
		{
			last = middle;
		}
		LASS_ASSERT(first != last);
	}

	LASS_ASSERT(first->x <= iX && last->x > iX);
	return first;
}



}

}

#endif

// EOF
