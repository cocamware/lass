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



/** @class lass::num::Vector
 *  @brief a dynamic sized n-dimensional vector with vector expression templates
 *  @author Bram de Greve [BdG]
 */

#ifndef LASS_GUARDIAN_OF_INCLUSION_NUM_SPLINE_CUBIC_H
#define LASS_GUARDIAN_OF_INCLUSION_NUM_SPLINE_CUBIC_H

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
	typename DataTraits = DefaultDataTraits<ScalarType, DataType>
>
class SplineCubic: public Spline<ScalarType, DataType>
{
public:

	typedef DataTraits TDataTraits;
	typedef Spline<ScalarType, DataType> TSplineBase;
	typedef typename TSplineBase::TScalar TScalar;
	typedef typename TSplineBase::TData TData;

	SplineCubic();
	template <typename PairInputIterator>
	SplineCubic(PairInputIterator iFirst, PairInputIterator iLast);
	template <typename ScalarInputIterator, typename DataInputIterator>
	SplineCubic(ScalarInputIterator iFirstControl, ScalarInputIterator iLastControl,
		DataInputIterator iFirstData);

	const TData operator()(TScalar iX) const;
	const TData derivative(TScalar iX) const;
	const TData derivative2(TScalar iX) const;
	const TData integral(TScalar iA, TScalar iB) const;

	const bool isEmpty() const;

private:

	struct Node
	{
		TData a;
		TData b;
		TData c;
		TData d;
		TScalar x;
	};

	typedef std::vector<Node> TNodes;
	typedef typename TNodes::const_iterator TNodeConstIterator;

	void init();
	const TNodeConstIterator findNode(TScalar iX) const;

	TNodes nodes_;
	size_t dataDimension_;
};


}

}

#include "spline_cubic.inl"

#endif

// EOF
