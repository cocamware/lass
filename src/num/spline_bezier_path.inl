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



/** construct a spline from a range of control/datatriplet pairs.
 *
 *  Each node consists of a control value and a data triplet.  This contstructor accepts a single 
 *	range [first, last) of control/data pairs.  The iterator type should have two fields
 *  @c first and @c second that contain respectively the control value and data triplet.  This is
 *  choosen so that a std::pair can be used as a representation of the control/data pair.
 *
 *  @pre
 *  @arg [first, last) is a valid range.
 *  @arg @c PairInputIterator has a member @c first containing the control value
 *  @arg @c PairInputIterator has a member @c second containing the data triplet
 *
 *  @par complexity: 
 *		O(D * log(N)) with 
 *		@arg D = a figure that indicates the complexity of operations on data values.
 *				 Is most probably linear with the dimension of the data value
 *		@arg N = number of nodes
 */
template <typename S, typename D, typename T>
template <typename PairInputIterator>
SplineBezierPath<S, D, T>::SplineBezierPath(
		PairInputIterator first, PairInputIterator last)
{
	init(first, last, first->second);
}



/** construct a spline from seperate ranges.
 *
 *  Each node consists of a control value and a data triplet.  This contstructor accepts seperate
 *  ranges for control values and data triplets.  The control values are given by the range 
 *  [firstControl , lastControl).  Of the range of the data triplets, only the iterator
 *  firstData to the the first element is given.
 *
 *  @pre
 *  @arg [firstControl, lastControl) is a valid range.
 *  @arg [firstData, firstData + (lastControl - firstControl)) is a valid range.
 *
 *  @par complexity: 
 *		O(D * log(N)) with 
 *		@arg D = a figure that indicates the complexity of operations on data values.
 *				 Is most probably linear with the dimension of the data value
 *		@arg N = number of nodes
 */
template <typename S, typename D, typename T>
template <typename ScalarInputIterator, typename DataInputIterator>
SplineBezierPath<S, D, T>::SplineBezierPath(ScalarInputIterator firstControl, 
									ScalarInputIterator lastControl,
									DataInputIterator firstData)
{
	init(firstControl, lastControl, firstData, *firstData);
}



/** Get the linear interpolated data value that corresponds with constrol value @a iX.
 *
 *  @pre this->isEmpty() == false
 *
 *  @par complexity: 
 *		O(D * log(N)) with 
 *		@arg D = a figure that indicates the complexity of operations on data values.
 *				 Is most probably linear with the dimension of the data value
 *		@arg N = number of nodes
 */
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



/** Get the first derivative of data value that corresponds with constrol value @a iX.
 *
 *  As long as @a iX is exact on a node, it equals to lim_{dx->0} (*this(iX + dx) - *this(iX)) / dx.
 *  With linear splines, in theory the first derivative does not exist on the nodes.  This
 *  function however will return the first derivative on the right of the node. *  
 *
 *  @pre this->isEmpty() == false
 *
 *  @par complexity: 
 *		O(D * log(N)) with 
 *		@arg D = a figure that indicates the complexity of operations on data values.
 *				 Is most probably linear with the dimension of the data value
 *		@arg N = number of nodes
 */
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



/** Get the second derivative of data value that corresponds with constrol value @a iX.
 *
 *  For a linear spline, the second derivative is always zero, except on the nodes where it
 *  does not exist.  This function however will always return zero, even on the nodes.
 *
 *  @pre this->isEmpty() == false
 *
 *  @par complexity: 
 *		O(D * log(N)) with 
 *		@arg D = a figure that indicates the complexity of operations on data values.
 *				 Is most probably linear with the dimension of the data value
 *		@arg N = number of nodes
 */
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
	const TScalar s = 1 - t;

	// 6*(1-t)*A+6*t*B-12*(1-t)*B-12*t*C+6*(1-t)*C+6*t*D

	TData dy = a;
	TDataTraits::scale(dy, 6 * s);
	TDataTraits::multiplyAccumulate(dy, b, 6 * (t - 2 * s));
	TDataTraits::multiplyAccumulate(dy, c, 6 * (s - 2 * t));
	TDataTraits::multiplyAccumulate(dy, d, 6 * t);
	TDataTraits::scale(dy, num::inv(num::sqr(dx)));
	return dy;
}



/** Get the integrated data value between control points @a iA and @a iB.
 *
 *  @pre this->isEmpty() == false
 *
 *  @par complexity: 
 *		O(D * M * log(N)) with 
 *		@arg D = a figure that indicates the complexity of operations on data values.
 *				 Is most probably linear with the dimension of the data value
 *		@arg M = number of nodes between @a iA and @a iB.
 *		@arg N = total number of nodes in spline
 */
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

	TData inty;
	TDataTraits::zero(inty, TDataTraits::dimension(first->knot()));

	if (first == last)
	{
		const typename TNodes::const_iterator second = stde::next(first);
		LASS_ASSERT(second != nodes_.end());

		const TData& a = first->knot();
		const TData& b = first->right();
		const TData& c = second->left();
		const TData& d = second->knot();

		const TScalar t2 = num::sqr(end) - num::sqr(begin);
		const TScalar t3 = num::cubic(end) - num::cubic(begin);
		const TScalar t4 = num::sqr(num::sqr(end)) - num::sqr(num::sqr(begin));
		const TScalar s4 = num::sqr(num::sqr(1 - end)) - num::sqr(num::sqr(1 - begin));

		TData inty(a);
		TDataTraits::scale(inty, -s4 / 4);
		TDataTraits::multiplyAccumulate(inty, b, 3 * t4 / 4 - 2 * t3 + 3 * t2 / 2);
		TDataTraits::multiplyAccumulate(inty, c, -3 * t4 / 4 + t3);
		TDataTraits::multiplyAccumulate(inty, d, t4 / 4);
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

		{
			const TScalar dx = second->x - first->x;
			const TScalar s = (begin - first->x) / dx;
			const TScalar s2 = num::sqr(s);
			const TScalar s3 = num::cubic(s);
			const TScalar s4 = num::sqr(s2);

			TData inty(first->knot());
			TDataTraits::scale(inty, dx * (1 + s4 - 4 * s3 + 6 * s2 - 4 * s) / 4);
			TDataTraits::multiplyAccumulate(inty, first->right(), dx * (1 - 3 * s4 + 8 * s3 - 6 * s2) / 4);
			TDataTraits::multiplyAccumulate(inty, second->left(), dx * (1 - 3 * s4 - 4 * s3) / 4);
			TDataTraits::multiplyAccumulate(inty, second->knot(), dx * (1 - s4) / 4);
		}

		first = second;
		second += 1;
		LASS_ASSERT(second != nodes_.end());

		while (first != last)
		{
			const TScalar dx = second->x - first->x;
			TDataTraits::multiplyAccumulate(inty, first->knot(), dx / 4);
			TDataTraits::multiplyAccumulate(inty, first->right(), dx / 4);
			TDataTraits::multiplyAccumulate(inty, second->left(), dx / 4);
			TDataTraits::multiplyAccumulate(inty, second->knot(), dx / 4);
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
				dx * (-t4 + 4 * t3 - 6 * t2 + t) / 4);
			TDataTraits::multiplyAccumulate(inty, first->right(), 
				dx * (3 * t4 - 8 * t3 + 6 * t2) / 4);
			TDataTraits::multiplyAccumulate(inty, second->left(), 
				dx * (-3 * t4 + 4 * t3) / 4);
			TDataTraits::multiplyAccumulate(inty, second->knot(), 
				dx * t4 / 4);
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
const typename SplineBezierPath<S, D, T>::ControlRange
SplineBezierPath<S, D, T>::controlRange() const
{
	return ControlRange(nodes_.front().x, nodes_.back().x);
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
	const TScalar dt = 1./3;
	const typename TNodes::size_type size = simpleNodes.size();
	if (size == 0)
	{
		return;
	}

	if (size == 1)
	{
		const TData& knot = simpleNodes[0].second;
		TData null;
		TDataTraits::zero(null, TDataTraits::dimension(knot));
		nodes_.push_back(Node(DataTriplet(null, knot, null), simpleNodes[0].first));
		return;
	}

	{
		const TScalar x = simpleNodes[0].first;
		const TScalar dx = simpleNodes[1].first - x;
		const TData& knot = simpleNodes[0].second;
		TData dy = simpleNodes[1].second;
		TDataTraits::multiplyAccumulate(dy, knot, -1);
		TData right = knot;
		TDataTraits::multiplyAccumulate(right, dy, dt);
		TData left = knot;
		TDataTraits::multiplyAccumulate(left, dy, -dt);
		nodes_.push_back(Node(DataTriplet(left, knot, right), x));
	}

	for (size_t i = 1; i < size - 1; ++i)
	{
		const TScalar x = simpleNodes[i].first;
		const TScalar dx = simpleNodes[i + 1].first - simpleNodes[i - 1].first;
		const TData& knot = simpleNodes[i].second;
		TData dy = simpleNodes[i + 1].second;
		TDataTraits::multiplyAccumulate(dy, simpleNodes[i - 1].second, -1);
		TDataTraits::scale(dy, .5);
		TData right = knot;
		TDataTraits::multiplyAccumulate(right, dy, dt);
		TData left = knot;
		TDataTraits::multiplyAccumulate(left, dy, -dt);
		nodes_.push_back(Node(DataTriplet(left, knot, right), x));
	}

	{
		const TScalar x = simpleNodes[size - 1].first;
		const TScalar dx = x - simpleNodes[size - 2].first;
		const TData& knot = simpleNodes[size - 1].second;
		TData dy = knot;
		TDataTraits::multiplyAccumulate(dy, simpleNodes[size - 2].second, -1);
		TData right = knot;
		TDataTraits::multiplyAccumulate(right, dy, dt);
		TData left = knot;
		TDataTraits::multiplyAccumulate(left, dy, -dt);
		nodes_.push_back(Node(DataTriplet(left, knot, right), x));
	}
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
const typename SplineBezierPath<S, D, T>::TNodeConstIterator
SplineBezierPath<S, D, T>::findNode(TScalar iX) const
{
	LASS_ASSERT(nodes_.size() >= 2);

	// early outs
	//
	if (iX < nodes_.front().x)
	{
		return nodes_.begin();
	}
	if (iX >= nodes_.back().x)
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



template <typename S, typename D, typename T>
const typename SplineBezierPath<S, D, T>::TData
SplineBezierPath<S, D, T>::extrapolate(const TData& a, const TData& b, TScalar t) const
{
	TData result(a);
	TDataTraits::scale(result, 1 - t);
	TDataTraits::multiplyAccumulate(result, b, t);
	return result;
}



template <typename S, typename D, typename T>
const typename SplineBezierPath<S, D, T>::TData
SplineBezierPath<S, D, T>::interpolate(const TData& a, const TData& b, const TData& c, const TData& d, TScalar t) const
{
	const TScalar s = 1 - t;
	const TScalar t2 = t * t;
	const TScalar s2 = s * s;

	TData result(a);
	TDataTraits::scale(result, s2 * s);
	TDataTraits::multiplyAccumulate(result, b, 3 * s2 * t);
	TDataTraits::multiplyAccumulate(result, c, 3 * s * t2);
	TDataTraits::multiplyAccumulate(result, d, t * t2);
	return result;
}


}

}

#endif

// EOF
