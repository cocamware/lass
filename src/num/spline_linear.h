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



/** @class lass::num::SplineLinear
 *  @brief connects the data nodes with linear segments.
 *  @author Bram de Greve [BdG]
 */

#ifndef LASS_GUARDIAN_OF_INCLUSION_NUM_SPLINE_LINEAR_H
#define LASS_GUARDIAN_OF_INCLUSION_NUM_SPLINE_LINEAR_H

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
class SplineLinear: public Spline<ScalarType, DataType>
{
public:

	typedef DataTraits TDataTraits;
	typedef Spline<ScalarType, DataType> TSplineBase;
	typedef typename TSplineBase::TScalar TScalar;
	typedef typename TSplineBase::TData TData;
	typedef typename TSplineBase::ControlRange TControlRange;

	SplineLinear();
	template <typename PairInputIterator>
	SplineLinear(PairInputIterator iFirst, PairInputIterator iLast);
	template <typename ScalarInputIterator, typename DataInputIterator>
	SplineLinear(ScalarInputIterator iFirstControl, ScalarInputIterator iLastControl,
		DataInputIterator iFirstData);

	const TData operator()(TScalar iX) const;
	const TData derivative(TScalar iX) const;
	const TData derivative2(TScalar iX) const;
	const TData integral(TScalar iA, TScalar iB) const;

	const bool isEmpty() const;
	const TControlRange controlRange() const;

private:

	struct Node
	{
		TData y;
		TData dy;
		TScalar x;

		static bool less(const Node& iA, const Node& iB) { return iA.x < iB.x; }
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

#include "spline_linear.inl"

#endif

// EOF
