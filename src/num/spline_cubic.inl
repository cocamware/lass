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



#ifndef LASS_GUARDIAN_OF_INCLUSION_NUM_SPLINE_CUBIC_INL
#define LASS_GUARDIAN_OF_INCLUSION_NUM_SPLINE_CUBIC_INL

#include "num_common.h"
#include "spline_cubic.h"
#include "matrix.h"
#include "../stde/extended_iterator.h"

namespace lass
{
namespace num
{

// --- public --------------------------------------------------------------------------------------

/** construct an empty spline 
 */
template <typename S, typename D, typename T>
SplineCubic<S, D, T>::SplineCubic()
{
}



/** construct a spline with a range of nodes.
 *
 *  Each node consists of a control value and a data value.  You must provide the control
 *  and data values in seperate ranges.  The control values are passed by a pair of iterators
 *  [iFirstControl , iLastControl).  Of the range of the data values, only the iterator
 *  iFirstData to the the first element is given.
 *
 *  @pre
 *  @arg [iFirstControl, iLastControl) is a valid range.
 *  @arg [iFirstData, iFirstData + (iLastControl - iFirstControl)) is a valid range.
 */
template <typename S, typename D, typename T>
template <typename ScalarInputIterator, typename DataInputIterator>
SplineCubic<S, D, T>::SplineCubic(ScalarInputIterator iFirstControl,
								  ScalarInputIterator iLastControl,
								  DataInputIterator iFirstData)
{
	while (iFirstControl != iLastControl)
	{
		Node node;
		node.x = *iFirstControl;
		node.d = *iFirstData;
		nodes_.push_back(node);
		++iFirstControl;
		++iFirstData;
	}
	init();
}



/** Get the linear interpolated data value that corresponds with constrol value @a iX.
 *
 *  @pre this->isEmpty() == false
 *
 *  @par Complexity: 
 *		O(D * log(N)) with 
 *		@arg D = a figure that indicates the complexity of operations on data values.
 *				 Is most probably linear with the dimension of the data value
 *		@arg N = number of nodes
 */
template <typename S, typename D, typename T>
const typename SplineCubic<S, D, T>::TData
SplineCubic<S, D, T>::operator ()(TScalar iX) const
{
	LASS_ASSERT(!isEmpty());

	const TNodeConstIterator n = findNode(iX);
	const TScalar s = iX - n->x;
	TData result(n->d);
	TDataTraits::multiplyAccumulate(result, n->c, s);
	TDataTraits::multiplyAccumulate(result, n->b, num::sqr(s));
	TDataTraits::multiplyAccumulate(result, n->a, num::cubic(s));
	return result;
}



/** Get the first derivative of data value that corresponds with constrol value @a iX.
 *
 *  @pre this->isEmpty() == false
 *
 *  @par Complexity: 
 *		O(D * log(N)) with 
 *		@arg D = a figure that indicates the complexity of operations on data values.
 *				 Is most probably linear with the dimension of the data value
 *		@arg N = number of nodes
 */
template <typename S, typename D, typename T>
const typename SplineCubic<S, D, T>::TData
SplineCubic<S, D, T>::derivative(TScalar iX) const
{
	LASS_ASSERT(!isEmpty());

	const TNodeConstIterator n = findNode(iX);
	const TScalar s = iX - n->x;
	TData result(n->c);
	TDataTraits::multiplyAccumulate(result, n->b, 2 * s);
	TDataTraits::multiplyAccumulate(result, n->a, 3 * num::sqr(s));
	return result;
}



/** Get the second derivative of data value that corresponds with constrol value @a iX.
 *
 *  @pre this->isEmpty() == false
 *
 *  @par Complexity: 
 *		O(D * log(N)) with 
 *		@arg D = a figure that indicates the complexity of operations on data values.
 *				 Is most probably linear with the dimension of the data value
 *		@arg N = number of nodes
 */
template <typename S, typename D, typename T>
const typename SplineCubic<S, D, T>::TData
SplineCubic<S, D, T>::derivative2(TScalar iX) const
{
	LASS_ASSERT(!isEmpty());

	const TNodeConstIterator n = findNode(iX);
	const TScalar s = iX - n->x;
	TData result(n->b);
	TDataTraits::multiplyAccumulate(result, n->a, 6 * s);
	return result;
}



/** Get the integrated data value between control points @a iA and @a iB.
 *
 *  @pre this->isEmpty() == false
 *
 *  @par Complexity: 
 *		O(D * M * log(N)) with 
 *		@arg D = a figure that indicates the complexity of operations on data values.
 *				 Is most probably linear with the dimension of the data value
 *		@arg M = number of nodes between @a iA and @a iB.
 *		@arg N = total number of nodes in spline
 */
template <typename S, typename D, typename T>
const typename SplineCubic<S, D, T>::TData
SplineCubic<S, D, T>::integral(TScalar iBegin, TScalar iEnd) const
{
	LASS_ASSERT(!isEmpty());

	TNodeConstIterator first = findNode(iBegin);
	TNodeConstIterator last = findNode(iEnd);
	if (first == last)
	{
		const TScalar s1 = iBegin - first->x;
		const TScalar s2 = iEnd - first->x;
		TData result(first->d);
		TDataTraits::scale(result, s2 - s1);
		TDataTraits::multiplyAccumulate(result, first->c, (num::sqr(s2) - num::sqr(s1)) / 2);
		TDataTraits::multiplyAccumulate(result, first->b, (num::cubic(s2) - num::cubic(s1)) / 3);
		TDataTraits::multiplyAccumulate(result, first->a, (num::sqr(num::sqr(s2)) - num::sqr(num::sqr(s1))) / 4);
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

		const TScalar s1 = iBegin - first->x;
		const TScalar s2 = next->x - first->x;
		TData result(first->d);
		TDataTraits::scale(result, s2 - s1);
		TDataTraits::multiplyAccumulate(result, first->c, (num::sqr(s2) - num::sqr(s1)) / 2);
		TDataTraits::multiplyAccumulate(result, first->b, (num::cubic(s2) - num::cubic(s1)) / 3);
		TDataTraits::multiplyAccumulate(result, first->a, (num::sqr(num::sqr(s2)) - num::sqr(num::sqr(s1))) / 4);
		first = next++;

		while (first != last)
		{
			const TScalar s = next->x - first->x;
			TDataTraits::multiplyAccumulate(result, first->d, s);
			TDataTraits::multiplyAccumulate(result, first->c, num::sqr(s) / 2);
			TDataTraits::multiplyAccumulate(result, first->b, num::cubic(s) / 3);
			TDataTraits::multiplyAccumulate(result, first->a, num::sqr(num::sqr(s)) / 4);
			first = next++;
		}

		const TScalar s = iEnd - first->x;
		TDataTraits::multiplyAccumulate(result, first->d, s);
		TDataTraits::multiplyAccumulate(result, first->c, num::sqr(s) / 2);
		TDataTraits::multiplyAccumulate(result, first->b, num::cubic(s) / 3);
		TDataTraits::multiplyAccumulate(result, first->a, num::sqr(num::sqr(s)) / 4);

		TDataTraits::scale(result, multiplier);
		return result;
	}
}



/** return true if the spline contains any nodes at all.
 *
 *  @par Complexity: 
 *		O(1) 
 */
template <typename S, typename D, typename T>
const bool SplineCubic<S, D, T>::isEmpty() const
{
	return nodes_.empty();
}



// --- private -------------------------------------------------------------------------------------

template <typename S, typename D, typename T>
void SplineCubic<S, D, T>::init()
{
	// are there any nodes at all?  we need at least two!
	//
	const size_t n = nodes_.size();
	if (n < 2)
	{
		LASS_THROW("A cubic spline needs at least two nodes!  This one only has '"
			<< static_cast<unsigned>(n) << "'.");
	}

	// - check for matching data dimensions
	// - check for increasing control components
	// - precalculate h_i = x_(i+1) - x_i
	//
	dataDimension_ = TDataTraits::dimension(nodes_[0].d);
	std::vector<TScalar> h;
	size_t i;
	for (i = 1; i < n; ++i)
	{
		h.push_back(nodes_[i].x - nodes_[i - 1].x);

		if (h.back() <= 0)
		{
			LASS_THROW("Nodes in cubic spline must have absolutely increasing control components.");
		}

		if (TDataTraits::dimension(nodes_[i].d) != dataDimension_)
		{
			LASS_THROW("All data elements in cubic spline must have same dimension.");
		}
	}

	// --- precalculate coefficients. ---

	// find coefficients for tridiagonal matrix
	//
	const size_t numUnknows = n - 2;

	// a tridiagonal matrix with 1, 4, 1
	num::Matrix<TScalar> m(numUnknows, numUnknows);
	m(0, 0) = 2 * (h[0] + h[1]);
	m(0, 1) = h[1];
	for (i = 1; i < numUnknows - 1; ++i)
	{
		m(i, i - 1) = h[i];
		m(i, i) = 2 * (h[i] + h[i + 1]);
		m(i, i + 1) = h[i + 1];
	}
	m(numUnknows - 1, numUnknows - 2) = h[numUnknows - 1];
	m(numUnknows - 1, numUnknows - 1) = 2 * (h[numUnknows - 1] + h[numUnknows]);

	num::Matrix<TScalar> unknowns(numUnknows, dataDimension_);
	for (i = 0; i < numUnknows; ++i)
	{
		// d_i / h_i - d_(i+1) * (h_i + h_(i+1)) / (h_i * h_(i+1)) + d_(i+2) / h_(i+1)
		TData d = nodes_[i].d;
		TDataTraits::scale(d, 1 / h[i]);
		TDataTraits::multiplyAccumulate(d, nodes_[i + 1].d, -(h[i] + h[i + 1]) / (h[i] * h[i + 1]));
		TDataTraits::multiplyAccumulate(d, nodes_[i + 2].d, 1 / h[i + 1]);
		TDataTraits::scale(d, 3);
		for (size_t k = 0; k < dataDimension_; ++k)
		{
			unknowns(i, k) = TDataTraits::get(d, k);
		}
	}

	if (!num::solve(m, unknowns))
	{
		LASS_THROW("serious logic error, could not solve equation, contact [BdG]");
	}

	TDataTraits::zero(nodes_[0].b, dataDimension_);
	for (i = 0; i < numUnknows; ++i)
	{
		TData& b = nodes_[i + 1].b;
		TDataTraits::zero(b, dataDimension_);
		for (size_t k = 0; k < dataDimension_; ++k)
		{
			TDataTraits::set(b, k, unknowns(i, k));
		}
	}
	TDataTraits::zero(nodes_[n - 1].b, dataDimension_);

	// find parameters for splines
	//
	for (i = 0; i < n - 1; ++i)
	{
		Node& node = nodes_[i];
		const Node& nextNode = nodes_[i + 1];

		// a_i = (b_(i+1) - b_i) / (3h_i)
		node.a = nextNode.b;
		TDataTraits::multiplyAccumulate(node.a, node.b, -1);
		TDataTraits::scale(node.a, num::inv(3 * h[i]));

		// c_i = (y_(i+1) - y_i) / h - (M_(i+1) + 2M_i) * h/6
		node.c = nextNode.d;
		TDataTraits::multiplyAccumulate(node.c, node.d, -1);
		TDataTraits::scale(node.c, num::inv(h[i]));
		TDataTraits::multiplyAccumulate(node.c, nextNode.b, -h[i] / 3);
		TDataTraits::multiplyAccumulate(node.c, node.b, -2 * h[i] / 3);
	}

	Node& node = nodes_[n - 1];
	const Node& prevNode = nodes_[n - 2];
	node.c = prevNode.c;
	TDataTraits::multiplyAccumulate(node.c, prevNode.b, 2 * h[n - 2]);
	TDataTraits::multiplyAccumulate(node.c, prevNode.a, 3 * num::sqr(h[n - 2]));
	node.a = prevNode.a;

}


/** binary search to find node that belongs to iX
 *
 *  @return
 *  @arg the index @a i if @a iX is in the interval [@c nodes_[i].x, @c nodes_[i+1].x)
 *  @arg 0 if @a iX is smaller than @c nodes_[0].x</tt>
 *  @arg @c nodes_.size()-2 if @a iX is greater than @c nodes_[nodes_.size()-1].x
 *
 *  @par complexity: 
 *		O(log N)
 */
template <typename S, typename D, typename T>
const typename SplineCubic<S, D, T>::TNodeConstIterator
SplineCubic<S, D, T>::findNode(TScalar iX) const
{
	const typename TNodes::size_type size = nodes_.size();
	LASS_ASSERT(nodes_.size() >= 2);

	// early outs
	//
	if (iX < nodes_[1].x)
	{
		return nodes_.begin();
	}
	if (iX >= nodes_[size - 2].x)
	{
		return stde::prior(nodes_.end(), 2);
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
