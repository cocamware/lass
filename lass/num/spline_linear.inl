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
SplineLinear<S, D, T>::SplineLinear()
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
SplineLinear<S, D, T>::SplineLinear(PairInputIterator iFirst, 
									PairInputIterator iLast)
{
	while (iFirst != iLast)
	{
		Node node;
		node.x = iFirst->first;
		node.y = iFirst->second;
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
const typename SplineLinear<S, D, T>::TData
SplineLinear<S, D, T>::operator ()(TScalar iX) const
{
	LASS_ASSERT(!isEmpty());

	const typename TNodes::const_iterator n = findNode(iX);
	const TScalar dx = iX - n->x;

	TData result(n->y);
	TDataTraits::multiplyAccumulate(result, n->dy, dx);
	return result;
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
const typename SplineLinear<S, D, T>::TData
SplineLinear<S, D, T>::derivative(TScalar iX) const
{
	LASS_ASSERT(!isEmpty());

	return findNode(iX)->dy;
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
const typename SplineLinear<S, D, T>::TData
SplineLinear<S, D, T>::derivative2(TScalar /*iX*/) const
{
	LASS_ASSERT(!isEmpty());

	TData result;
	TDataTraits::zero(result, dataDimension_);
	return result;
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
const typename SplineLinear<S, D, T>::TData
SplineLinear<S, D, T>::integral(TScalar iBegin, TScalar iEnd) const
{
	LASS_ASSERT(!isEmpty());

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



/** return true if the spline contains any nodes at all.
 *  @par complexity: 
 *		O(1)
 */
template <typename S, typename D, typename T>
bool SplineLinear<S, D, T>::isEmpty() const
{
	return nodes_.empty();
}



/** return the range of control values for which the spline can interpolate.
 *  @par complexity: 
 *		O(1)
 */
template <typename S, typename D, typename T>
const typename SplineLinear<S, D, T>::TControlRange
SplineLinear<S, D, T>::controlRange() const
{
	typedef typename SplineLinear<S, D, T>::TControlRange TRange;
	return TRange(nodes_.front().x, nodes_.back().x);
}



// --- private -------------------------------------------------------------------------------------

template <typename S, typename D, typename T>
void SplineLinear<S, D, T>::init()
{
	// are there any nodes at all?  we need at least two!
	//
	const typename TNodes::size_type size = nodes_.size();
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
	stde::prev(end)->dy = stde::prev(end, 2)->dy;
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
	LASS_ASSERT(nodes_.size() >= 2);

	// early outs
	//
	if (iX < nodes_.front().x)
	{
		return nodes_.begin();
	}
	if (iX >= nodes_.back().x)
	{
		return stde::prev(nodes_.end());
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
