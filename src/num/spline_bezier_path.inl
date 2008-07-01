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



#ifndef LASS_GUARDIAN_OF_INCLUSION_NUM_SPLINE_BEZIER_PATH_INL
#define LASS_GUARDIAN_OF_INCLUSION_NUM_SPLINE_BEZIER_PATH_INL

#include "num_common.h"
#include "spline_bezier_path.h"
#include "../stde/extended_iterator.h"

namespace lass
{
namespace num
{

// --- public --------------------------------------------------------------------------------------

template <typename S, typename D, typename T>
SplineBezierPath<S, D, T>::SplineBezierPath()
{
}



template <typename S, typename D, typename T>
template <typename PairInputIterator>
SplineBezierPath<S, D, T>::SplineBezierPath(
		PairInputIterator first, PairInputIterator last)
{
	init(first, last, first->second);
}



template <typename S, typename D, typename T>
template <typename ScalarInputIterator, typename DataInputIterator>
SplineBezierPath<S, D, T>::SplineBezierPath(ScalarInputIterator firstControl, 
									ScalarInputIterator lastControl,
									DataInputIterator firstData)
{
	init(firstControl, lastControl, firstData, *firstData);
}



template <typename S, typename D, typename T>
const typename SplineBezierPath<S, D, T>::TData
SplineBezierPath<S, D, T>::operator ()(TScalar x) const
{
	LASS_ASSERT(!isEmpty());

	const typename TNodes::const_iterator first = findNode(x);
	LASS_ASSERT(first != nodes_.end());
	const typename TNodes::const_iterator second = stde::next(first);
	LASS_ASSERT(second != nodes_.end());

	const TData& a = first->knot();
	const TData& b = first->right();
	const TData& c = second->left();
	const TData& d = second->knot();

	const TScalar t = (x - first->x) / (second->x - first->x);
	const TScalar s = 1 - t;

	TData y(a);
	TDataTraits::scale(y, s * s * s);
	TDataTraits::multiplyAccumulate(y, b, 3 * s * s * t);
	TDataTraits::multiplyAccumulate(y, c, 3 * s * t * t);
	TDataTraits::multiplyAccumulate(y, d, t * t * t);
	return y;

}



template <typename S, typename D, typename T>
const typename SplineBezierPath<S, D, T>::TData
SplineBezierPath<S, D, T>::derivative(TScalar x) const
{
	LASS_ASSERT(!isEmpty());

	const typename TNodes::const_iterator first = findNode(x);
	LASS_ASSERT(first != nodes_.end());
	const typename TNodes::const_iterator second = stde::next(first);
	LASS_ASSERT(second != nodes_.end());

	const TData& a = first->knot();
	const TData& b = first->right();
	const TData& c = second->left();
	const TData& d = second->knot();
	const TScalar t = (x - first->x) / (second->x - first->x);
	const TScalar s = 1 - t;

	TData dy = a;
	TDataTraits::scale(dy, -3 * s * s);
	TDataTraits::multiplyAccumulate(dy, b, 3 * s * (s - 2 * t));
	TDataTraits::multiplyAccumulate(dy, c, 3 * t * (2 * s - t));
	TDataTraits::multiplyAccumulate(dy, d, 3 * t * t);
	TDataTraits::scale(dy, num::inv(second->x - first->x));
	return dy;
}



template <typename S, typename D, typename T>
const typename SplineBezierPath<S, D, T>::TData
SplineBezierPath<S, D, T>::derivative2(TScalar x) const
{
	LASS_ASSERT(!isEmpty());

	const typename TNodes::const_iterator first = findNode(x);
	LASS_ASSERT(first != nodes_.end());
	const typename TNodes::const_iterator second = stde::next(first);
	LASS_ASSERT(second != nodes_.end());

	const TData& a = first->knot();
	const TData& b = first->right();
	const TData& c = second->left();
	const TData& d = second->knot();
	const TScalar dx = second->x - first->x;
	const TScalar t = (x - first->x) / dx;

	// (6-6*t)*A+(18*t-12)*B+(-18*t+6)*C+6*t*D

	TData dy = a;
	TDataTraits::scale(dy, 6 - 6 * t);
	TDataTraits::multiplyAccumulate(dy, b, 18 * t - 12);
	TDataTraits::multiplyAccumulate(dy, c, -18 * t + 6);
	TDataTraits::multiplyAccumulate(dy, d, 6 * t);
	TDataTraits::scale(dy, num::inv(num::sqr(dx)));
	return dy;
}



template <typename S, typename D, typename T>
const typename SplineBezierPath<S, D, T>::TData
SplineBezierPath<S, D, T>::integral(TScalar begin, TScalar end) const
{
	// -1/4*(1-t)^4*A+3*B*(1/4*t^4-2/3*t^3+1/2*t^2)+3*C*(-1/4*t^4+1/3*t^3)+1/4*t^4*D

	LASS_ASSERT(!isEmpty());

	typename TNodes::const_iterator first = findNode(begin);
	LASS_ASSERT(first != nodes_.end());
	typename TNodes::const_iterator last = findNode(end);
	LASS_ASSERT(last != nodes_.end());

	if (first == last)
	{
		const typename TNodes::const_iterator second = stde::next(first);
		LASS_ASSERT(second != nodes_.end());

		const TData& a = first->knot();
		const TData& b = first->right();
		const TData& c = second->left();
		const TData& d = second->knot();

		const TScalar dx = second->x - first->x;
		const TScalar invDx = num::inv(dx);
		const TScalar s = invDx * (begin - first->x);
		const TScalar t = invDx * (end - first->x);

		const TScalar st = t - s;
		const TScalar st2 = num::sqr(t) - num::sqr(s);
		const TScalar st3 = num::cubic(t) - num::cubic(s);
		const TScalar st4 = num::sqr(num::sqr(t)) - num::sqr(num::sqr(s));
		//const TScalar s4 = num::sqr(num::sqr(1 - end)) - num::sqr(num::sqr(1 - begin));

		TData inty(a);
		TDataTraits::scale(inty, dx * (-.25f * st4 + st3 - 1.5f * st2 + st));
		TDataTraits::multiplyAccumulate(inty, b, dx * (.75f * st4 - 2 * st3 + 1.5f * st2));
		TDataTraits::multiplyAccumulate(inty, c, dx * (-.75f * st4 + st3));
		TDataTraits::multiplyAccumulate(inty, d, dx * .25f * st4);
		TDataTraits::scale(inty, second->x - first->x);
		return inty;
	}
	else
	{
		TScalar multiplier = 1;
		if (end < begin)
		{
			std::swap(begin, end);
			std::swap(first, last);
			multiplier = -1;
		}

		typename TNodes::const_iterator second = stde::next(first);
		LASS_ASSERT(second != nodes_.end());

		TData inty(first->knot());
		{
			const TScalar dx = second->x - first->x;
			const TScalar s = (begin - first->x) / dx;
			const TScalar s2 = num::sqr(s);
			const TScalar s3 = num::cubic(s);
			const TScalar s4 = num::sqr(s2);

			TDataTraits::scale(inty, dx * (.25f + .25f * s4 - s3 + 1.5f * s2 - s));
			TDataTraits::multiplyAccumulate(inty, first->right(), dx * (.25f - .75f * s4 + 2 * s3 - 1.5f * s2));
			TDataTraits::multiplyAccumulate(inty, second->left(), dx * (.25f + .75f * s4 - s3));
			TDataTraits::multiplyAccumulate(inty, second->knot(), dx * .25f * (1 - s4));
		}

		first = second;
		second += 1;
		LASS_ASSERT(second != nodes_.end());

		while (first != last)
		{
			const TScalar dx = second->x - first->x;
			TDataTraits::multiplyAccumulate(inty, first->knot(), dx * .25f);
			TDataTraits::multiplyAccumulate(inty, first->right(), dx * .25f);
			TDataTraits::multiplyAccumulate(inty, second->left(), dx * .25f);
			TDataTraits::multiplyAccumulate(inty, second->knot(), dx * .25f);
			first = second;
			second += 1;
			LASS_ASSERT(second != nodes_.end());
		}

		{
			const TScalar dx = second->x - first->x;
			const TScalar t = (end - first->x) / dx;
			const TScalar t2 = num::sqr(t);
			const TScalar t3 = num::cubic(t);
			const TScalar t4 = num::sqr(t2);

			TDataTraits::multiplyAccumulate(inty, first->knot(), 
				dx * (-.25f * t4 + t3 - 1.5f * t2 + t));
			TDataTraits::multiplyAccumulate(inty, first->right(), 
				dx * (.75f * t4 - 2 * t3 + 1.5f * t2));
			TDataTraits::multiplyAccumulate(inty, second->left(), 
				dx * (-.75f * t4 + t3));
			TDataTraits::multiplyAccumulate(inty, second->knot(), 
				dx * .25f * t4);
		}
		
		TDataTraits::scale(inty, multiplier);
		return inty;
	}
}



/** return true if the spline contains any nodes at all.
 *  @par complexity: 
 *		O(1)
 */
template <typename S, typename D, typename T>
const bool SplineBezierPath<S, D, T>::isEmpty() const
{
	return nodes_.empty();
}



/** return the range of control values for which the spline can interpolate.
 *  @par complexity: 
 *		O(1)
 */
template <typename S, typename D, typename T>
const typename SplineBezierPath<S, D, T>::TControlRange
SplineBezierPath<S, D, T>::controlRange() const
{
	return TControlRange(nodes_.front().x, nodes_.back().x);
}



// --- private -------------------------------------------------------------------------------------

template <typename S, typename D, typename T>
template <typename PairInputIterator>
void SplineBezierPath<S, D, T>::init(
		PairInputIterator first, PairInputIterator last, const DataTriplet& /* dummy */)
{
	while (first != last)
	{
		Node node;
		node.x = first->first;
		node.triplet = first->second;
		nodes_.push_back(node);
		++first;
	}
	finalInit();
}

template <typename S, typename D, typename T>
template <typename PairInputIterator>
void SplineBezierPath<S, D, T>::init(
		PairInputIterator first, PairInputIterator last, const TData& /* dummy */)
{
	TSimpleNodes simpleNodes;
	std::copy(first, last, std::back_inserter(simpleNodes));
	makeFullNodes(simpleNodes);
	finalInit();
}

template <typename S, typename D, typename T>
template <typename ScalarInputIterator, typename DataInputIterator>
void SplineBezierPath<S, D, T>::init(
		ScalarInputIterator firstControl, ScalarInputIterator lastControl,
		DataInputIterator firstData, const DataTriplet& /* dummy */)
{
	while (firstControl != lastControl)
	{
		Node node;
		node.x = *firstControl++;
		node.triplet = *firstData++;
		nodes_.push_back(node);
	}
	finalInit();
}

template <typename S, typename D, typename T>
template <typename ScalarInputIterator, typename DataInputIterator>
void SplineBezierPath<S, D, T>::init(
		ScalarInputIterator firstControl, ScalarInputIterator lastControl,
		DataInputIterator firstData, const TData& /* dummy */)
{
	TSimpleNodes simpleNodes;
	while (firstControl != lastControl)
	{
		simpleNodes.push_back(std::make_pair(*firstControl++, *firstData++));
	}
	makeFullNodes(simpleNodes);
	finalInit();
}

template <typename S, typename D, typename T>
void SplineBezierPath<S, D, T>::makeFullNodes(const TSimpleNodes& simpleNodes)
{
	const TScalar dt = TScalar(1) / 3;
	const typename TNodes::size_type size = simpleNodes.size();

	TNodes nodes;
	switch (size)
	{
	case 0:
		break;

	case 1:
		{
			const TData& knot = simpleNodes[0].second;
			TData null;
			TDataTraits::zero(null, TDataTraits::dimension(knot));
			nodes.push_back(Node(DataTriplet(null, knot, null), simpleNodes[0].first));
		}
		break;

	default:
		{
			const TData& knot = simpleNodes[0].second;
			TData dy = simpleNodes[1].second;
			TDataTraits::multiplyAccumulate(dy, knot, -1);
			TData right = knot;
			TDataTraits::multiplyAccumulate(right, dy, dt);
			TData left = knot;
			TDataTraits::multiplyAccumulate(left, dy, -dt);
			nodes.push_back(Node(DataTriplet(left, knot, right), simpleNodes[0].first));
		}

		for (size_t i = 1; i < size - 1; ++i)
		{
			const TData& knot = simpleNodes[i].second;
			TData dy = simpleNodes[i + 1].second;
			TDataTraits::multiplyAccumulate(dy, simpleNodes[i - 1].second, -1);
			TDataTraits::scale(dy, .5);
			TData right = knot;
			TDataTraits::multiplyAccumulate(right, dy, dt);
			TData left = knot;
			TDataTraits::multiplyAccumulate(left, dy, -dt);
			nodes.push_back(Node(DataTriplet(left, knot, right), simpleNodes[i].first));
		}

		{
			const TData& knot = simpleNodes[size - 1].second;
			TData dy = knot;
			TDataTraits::multiplyAccumulate(dy, simpleNodes[size - 2].second, -1);
			TData right = knot;
			TDataTraits::multiplyAccumulate(right, dy, dt);
			TData left = knot;
			TDataTraits::multiplyAccumulate(left, dy, -dt);
			nodes.push_back(Node(DataTriplet(left, knot, right), simpleNodes[size - 1].first));
		}
	}

	nodes_.swap(nodes);
}

template <typename S, typename D, typename T>
void SplineBezierPath<S, D, T>::finalInit()
{
	// are there any nodes at all?  we need at least one!
	//
	const typename TNodes::size_type size = nodes_.size();
	if (size < 1)
	{
		LASS_THROW("A bezier path interpolator needs at least one node!");
	}

	typename TNodes::iterator prev = nodes_.begin();
	dataDimension_ = TDataTraits::dimension(prev->knot());
	const typename TNodes::iterator end = nodes_.end();
	for (typename TNodes::iterator i = stde::next(nodes_.begin()); i != end; prev = i++)
	{
		// check for increasing control components
		//
		if (prev->x >= i->x)
		{
			LASS_THROW("Nodes in bezier path interpolator must have absolutely increasing control "
				"values.");
		}

		// check dimension of data
		//
		if (TDataTraits::dimension(i->left()) != dataDimension_ ||
			TDataTraits::dimension(i->knot()) != dataDimension_ ||
			TDataTraits::dimension(i->right()) != dataDimension_)
		{
			LASS_THROW("All data in linear interpolator must be of same dimension.");
		}
	}
}



/** binary search to find node that belongs to x
 *
 *  @return
 *  @arg the iterator to node such that @a x is in the interval [@c node->x, @c next(node)->x)
 *  @arg nodes_.begin() @a x is smaller than @c nodes_.front().x</tt>
 *  @arg prev(nodes_.end(), 2) if @a x is greater than @c nodes_.back().x
 *
 *  complexity: O(ln N)
 */
template <typename S, typename D, typename T>
const typename SplineBezierPath<S, D, T>::TNodeConstIterator
SplineBezierPath<S, D, T>::findNode(TScalar x) const
{
	LASS_ASSERT(nodes_.size() >= 2);

	// early outs
	//
	if (x < nodes_.front().x)
	{
		return nodes_.begin();
	}
	if (x >= nodes_.back().x)
	{
		return stde::prev(nodes_.end(), 2);
	}

	// binary search
	//
	TNodeConstIterator first = nodes_.begin();
	TNodeConstIterator last = nodes_.end();
	while (stde::next(first) != last)
	{
		TNodeConstIterator middle = stde::next(first, std::distance(first, last) / 2);
		LASS_ASSERT(middle != first && middle != last);

		if (middle->x <= x)
		{
			first = middle;
		}
		else
		{
			last = middle;
		}
		LASS_ASSERT(first != last);
	}

	LASS_ASSERT(first->x <= x && last->x > x);
	return first;
}


}

}

#endif

// EOF
