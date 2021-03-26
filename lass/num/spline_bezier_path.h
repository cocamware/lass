/** @file
 *  @author Bram de Greve (bram@cocamware.com)
 *  @author Tom De Muer (tom@cocamware.com)
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



/** @class lass::num::SplineBezierPath
 *  @brief connects the data nodes with cubic bezier splines.
 *  @author Bram de Greve [BdG]
 */

#ifndef LASS_GUARDIAN_OF_INCLUSION_NUM_SPLINE_BEZIER_PATH_H
#define LASS_GUARDIAN_OF_INCLUSION_NUM_SPLINE_BEZIER_PATH_H

#include "num_common.h"
#include "spline.h"

namespace lass
{
namespace num
{

template 
<
	typename ScalarType,
	typename DataType,
	typename DataTraits
>
class SplineBezierPath: public Spline<ScalarType, DataType>
{
public:

	typedef DataTraits TDataTraits;
	typedef Spline<ScalarType, DataType> TSplineBase;
	typedef typename TSplineBase::TScalar TScalar;
	typedef typename TSplineBase::TData TData;
	typedef typename TSplineBase::ControlRange TControlRange;

	class DataTriplet
	{
	public:
		DataTriplet(const TData& left, const TData& knot, const TData& right): 
			left_(left), knot_(knot), right_(right) {}
		const TData& left() const { return left_; }
		const TData& knot() const { return knot_; }
		const TData& right() const { return right_; }
	private:
		friend class SplineBezierPath;
		TData left_;
		TData knot_;
		TData right_;
	};

	SplineBezierPath();
	template <typename PairInputIterator>
	SplineBezierPath(PairInputIterator iFirst, PairInputIterator iLast);
	template <typename ScalarInputIterator, typename DataInputIterator>
	SplineBezierPath(ScalarInputIterator iFirstControl, ScalarInputIterator iLastControl,
		DataInputIterator iFirstData);

	const TData operator()(TScalar iX) const override;
	const TData derivative(TScalar iX) const override;
	const TData derivative2(TScalar iX) const override;
	const TData integral(TScalar iA, TScalar iB) const override;

	bool isEmpty() const override;
	const TControlRange controlRange() const override;

private:

	struct Node
	{
		DataTriplet triplet;
		TScalar x;

		Node(const DataTriplet& triplet, TScalar x): triplet(triplet), x(x) {}
		const TData& left() const { return triplet.left(); }
		const TData& knot() const { return triplet.knot(); }
		const TData& right() const { return triplet.right(); }

		static bool less(const Node& iA, const Node& iB) { return iA.x < iB.x; }
	};
	typedef std::vector<Node> TNodes;
	typedef typename TNodes::const_iterator TNodeConstIterator;

	typedef std::pair<TScalar, TData> TSimpleNode;
	typedef std::vector<TSimpleNode> TSimpleNodes;

	template <typename PairInputIterator>
	void init(PairInputIterator first, PairInputIterator last, const DataTriplet& dummy);
	template <typename PairInputIterator>
	void init(PairInputIterator first, PairInputIterator last, const TData& dummy);
	template <typename ScalarInputIterator, typename DataInputIterator>
	void init(ScalarInputIterator firstControl, ScalarInputIterator lastControl,
		DataInputIterator firstData, const DataTriplet& dummy);
	template <typename ScalarInputIterator, typename DataInputIterator>
	void init(ScalarInputIterator firstControl, ScalarInputIterator lastControl,
		DataInputIterator firstData, const TData& dummy);
	void makeFullNodes(const TSimpleNodes& simpleNodes);
	void finalInit();

	const TNodeConstIterator findNode(TScalar iX) const;

	TNodes nodes_;
	size_t dataDimension_;
};



}

}

#include "spline_bezier_path.inl"

#endif

// EOF
