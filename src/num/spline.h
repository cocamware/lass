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



/** @class lass::num::Spline
 *  @brief abstract base class of splines.
 *  @author Bram de Greve [BdG]
 */

#ifndef LASS_GUARDIAN_OF_INCLUSION_NUM_SPLINE_H
#define LASS_GUARDIAN_OF_INCLUSION_NUM_SPLINE_H

#include "num_common.h"
#include "../stde/extended_iterator.h"

namespace lass
{
namespace num
{

template 
<
	typename ScalarType,
	typename DataType
>
class Spline
{
public:

	typedef ScalarType TScalar;
	typedef DataType TData;

	virtual ~Spline() {}

	virtual const TData operator()(TScalar iX) const = 0;
	virtual const TData derivative(TScalar iX) const = 0;
	virtual const TData derivative2(TScalar iX) const = 0;
	virtual const TData integral(TScalar iA, TScalar iB) const = 0;

	virtual const bool isEmpty() const = 0;
};





#pragma LASS_TODO("below are data traits that should move to a seperate header when appropriate, [Bramz]")

/** @defgroup DataTraits
 *  A set of trait classes on how to manipulate data
 *
 *  This is the concept each data traits class has to model:
 *
 *  @code
 *  struct
 *  {
 *		typedef ... TData; // type of data
 *		typedef ... TScalar; // type of a single scalar value in data
 *		static size_t dimension(const TData& iY); // returns number of scalar values in iY
 *		static void zero(TData& iY, size_t iDim); // resets iY to a data object with iDim zeros
 *		static TScalar get(const TData& iY, size_t iIndex); // return iIndex'th scalar in iY
 *		static void set(TData& ioY, size_t iIndex, TScalar iV); // set iIndex'th scalar of iY to iV
 *		static void scale(TDAta& ioAcc, TScalar iS); // scale each scalar of ioAcc by iS
 *		static void multiplyAccumulate(TData& ioAcc, const TData& iY, TScalar iS); // add each
 *			// element of iY multiplied by iS to the element in ioAcc with same index.
 *  };
 *	@endcode
 *
 *  - @ref DataTraitsSequence: concerning data acting like an STL sequence
 *  - @ref DataTraitsScalar: concerning scalar data
 *  - @ref DataTraitsDynamicVector: concerning data like num::Vector
 *  - @ref DataTraitsStaticVector: concerning data like prim::Vector2D
 */

/** @ingroup DataTraits
 */
template <typename SequenceType>
struct DataTraitsSequence
{
	typedef SequenceType TData;
	typedef typename SequenceType::value_type TScalar;
	static size_t dimension(const TData& iY) { return std::distance(iY.begin(), iY.end()); }
	static void zero(TData& ioY, size_t iDim) { ioY = TData(iDim); }
	static TScalar get(const TData& iY, size_t iIndex) { return *stde::next(iY.begin(), iIndex); }
	static void set(TData& ioY, size_t iIndex, TScalar iV) { *stde::next(ioY.begin(), iIndex) = iV; }
	static void scale(TData& ioAcc, TScalar iS) 
	{ 
		std::transform(ioAcc.begin(), ioAcc.end(), ioAcc.begin(), std::bind2nd(std::multiplies<TScalar>(), iS));
	}
	static void multiplyAccumulate(TData& ioAcc, const TData& iY, TScalar iS) 
	{
		std::transform(ioAcc.begin(), ioAcc.end(), iY.begin(), ioAcc.begin(), Mac(iS));
	}
private:
	class Mac
	{
		TScalar s_;
	public:
		Mac(TScalar iS): s_(iS) {}
		TScalar operator()(TScalar iA, TScalar iB) const { return iA + s_ * iB; }
	};
};

/** @ingroup DataTraits
 */
template <typename ScalarType>
struct DataTraitsScalar
{
	typedef ScalarType TData;
	typedef ScalarType TScalar;
	static size_t dimension(TScalar iY) { return 1; }
	static void zero(TScalar& ioY, size_t iDim) { ioY = TScalar(); }
	static TScalar get(TScalar iY, size_t iIndex) { return iY; }
	static void set(TScalar& ioY, size_t iIndex, TScalar iV) { ioY = iV; }
	static void scale(TScalar& ioAcc, TScalar iS) { ioAcc *= iS; }
	static void multiplyAccumulate(TScalar& ioAcc, TScalar iY, TScalar iS) { ioAcc += iY * iS; }
};


/** @ingroup DataTraits
 */
template <typename DataType>
struct DataTraitsDynamicVector
{
	typedef DataType TData;
	typedef typename DataType::TValue TScalar;
	static size_t dimension(const TData& iY) { return iY.size(); }
	static void zero(TData& ioY, size_t iDim) { ioY = TData(iDim); }
	static TScalar get(const TData& iY, size_t iIndex) { return iY[iIndex]; }
	static void set(TData& ioY, size_t iIndex, TScalar iV) { ioY[iIndex] = iV; }
	static void scale(TData& ioAcc, TScalar iS) { ioAcc *= iS; }
	static void multiplyAccumulate(TData& ioAcc, const TData& iY, TScalar iS)  { ioAcc += iY * iS; }
};

/** @ingroup DataTraits
 */
template <typename DataType>
struct DataTraitsStaticVector
{
	typedef DataType TData;
	typedef typename DataType::TValue TScalar;
	static size_t dimension(const TData& iY) { return TData::dimension; }
	static void zero(TData& ioY, size_t iDim) { ioY = TData(); }
	static TScalar get(const TData& iY, size_t iIndex) { return iY[iIndex]; }
	static void set(TData& ioY, size_t iIndex, TScalar iV) { ioY[iIndex] = iV; }
	static void scale(TData& ioAcc, TScalar iS) { ioAcc *= iS; }
	static void multiplyAccumulate(TData& ioAcc, const TData& iY, TScalar iS)  { ioAcc += iY * iS; }
};

}

}

#endif

// EOF
