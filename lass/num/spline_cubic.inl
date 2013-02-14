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
 *	Copyright (C) 2004-2011 the Initial Developer.
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



#ifndef LASS_GUARDIAN_OF_INCLUSION_NUM_SPLINE_CUBIC_INL
#define LASS_GUARDIAN_OF_INCLUSION_NUM_SPLINE_CUBIC_INL

#include "num_common.h"
#include "spline_cubic.h"
#include "num_cast.h"
#include "impl/matrix_solve.h"
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



/** construct a spline from a range of control/data pairs.
 *
 *  Each node consists of a control value and a data value.  This contstructor accepts a single 
 *	range [iFirst, iLast) of control/data pairs.  The iterator type should have two fields
 *  @c first and @c second that contain respectively the control and data values.  This is
 *  choosen so that a std::pair can be used as a representatin of the control/data pair.
 *
 *  @pre
 *  @arg [iFirst, iLast) is a valid range.
 *  @arg @c PairInputIterator has a member @c first containing the control value
 *  @arg @c PairInputIterator has a member @c second containing the data value
 *
 *  @par complexity: 
 *		O(D * log(N)) with 
 *		@arg D = a figure that indicates the complexity of operations on data values.
 *				 Is most probably linear with the dimension of the data value
 *		@arg N = number of nodes
 */
template <typename S, typename D, typename T>
template <typename PairInputIterator>
SplineCubic<S, D, T>::SplineCubic(PairInputIterator iFirst, PairInputIterator iLast)
{
	while (iFirst != iLast)
	{
		Node node;
		node.x = iFirst->first;
		node.d = iFirst->second;
		nodes_.push_back(node);
		++iFirst;
	}
	init();
}



/** construct a spline from seperate ranges.
 *
 *  Each node consists of a control value and a data value.  This contstructor accepts seperate
 *  ranges for control and data values.  The control values are given by the range 
 *  [iFirstControl , iLastControl).  Of the range of the data values, only the iterator
 *  iFirstData to the the first element is given.
 *
 *  @pre
 *  @arg [iFirstControl, iLastControl) is a valid range.
 *  @arg [iFirstData, iFirstData + (iLastControl - iFirstControl)) is a valid range.
 *
 *  @par complexity: 
 *		O(D * log(N)) with 
 *		@arg D = a figure that indicates the complexity of operations on data values.
 *				 Is most probably linear with the dimension of the data value
 *		@arg N = number of nodes
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
		node.x = *iFirstControl++;
		node.d = *iFirstData++;
		nodes_.push_back(node);
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
bool SplineCubic<S, D, T>::isEmpty() const
{
	return nodes_.empty();
}



/** return the range of control values for which the spline can interpolate.
 *  @par complexity: 
 *		O(1)
 */
template <typename S, typename D, typename T>
const typename SplineCubic<S, D, T>::TControlRange
SplineCubic<S, D, T>::controlRange() const
{
	return TControlRange(nodes_.front().x, nodes_.back().x);
}



// --- private -------------------------------------------------------------------------------------

template <typename S, typename D, typename T>
void SplineCubic<S, D, T>::init()
{
	typedef std::vector<TScalar> TVector;

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
	TVector h;
	for (size_t i = 1; i < n; ++i)
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

	// --- init some elements ---

	for (size_t i = 0; i < n; ++i)
	{
		TDataTraits::zero(nodes_[i].b, dataDimension_);
	}

	// --- precalculate coefficients. ---

	// find coefficients for tridiagonal matrix
	//
	const size_t numUnknowns = n - 2;

	//*
	TVector ma(numUnknowns);
	TVector mb(numUnknowns);
	TVector mc(numUnknowns);
	TVector temp(numUnknowns);
	TVector unknowns(numUnknowns);

	mb[0] = 2 * (h[0] + h[1]);
	mc[0] = h[1];
	for (size_t i = 1; i < numUnknowns - 1; ++i)
	{
		ma[i] = h[i];
		mb[i] = 2 * (h[i] + h[i + 1]);
		mc[i] = h[i + 1];
	}
	ma[numUnknowns - 1] = h[numUnknowns - 1];
	mb[numUnknowns - 1] = 2 * (h[numUnknowns - 1] + h[numUnknowns]);

	for (size_t k = 0; k < dataDimension_; ++k)
	{
		for (size_t i = 0; i < numUnknowns; ++i)
		{
			// d_i / h_i - d_(i+1) * (h_i + h_(i+1)) / (h_i * h_(i+1)) + d_(i+2) / h_(i+1)
			const TScalar d0 = TDataTraits::get(nodes_[i].d, k);
			const TScalar d1 = TDataTraits::get(nodes_[i + 1].d, k);
			const TScalar d2 = TDataTraits::get(nodes_[i + 2].d, k);
			unknowns[i] = 3 * (d0 / h[i] - d1 * (h[i] + h[i + 1]) / (h[i] * h[i + 1]) + d2 / h[i + 1]);
		}
		if (!num::impl::solveTridiagonal<TScalar>(\
				ma.begin(), mb.begin(), mc.begin(), unknowns.begin(), temp.begin(), numCast<ptrdiff_t>(numUnknowns)))
		{
			LASS_THROW("serious logic error, could not solve equation, contact [Bramz]");
		}
		for (size_t i = 0; i < numUnknowns; ++i)
		{
			TDataTraits::set(nodes_[i + 1].b, k, unknowns[i]);
		}
	}

	// find parameters for splines
	//
	for (size_t i = 0; i < n - 1; ++i)
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



}

}

#endif

// EOF
