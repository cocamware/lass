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

	virtual const TData operator()(TScalar iX) const = 0;
	virtual const TData derivative(TScalar iX) const = 0;
	virtual const TData derivative2(TScalar iX) const = 0;
	virtual const TData integral(TScalar iA, TScalar iB) const = 0;
};


template 
<
	typename ScalarType, 
	typename DataType
>
struct DefaultDataTraits
{
	/** return the number of scalar in one data element
	 */
	static size_t dimension(const DataType& iData)
	{
		return iData.size();
	}
	/** initialize @a ioData to an element with @a iDimension zeroes.
	 */
	static void zero(DataType& ioData, size_t iDimension)
	{
		ioData = DataType(iDimension);
	}
	/** get the @a iIndex'th scalar of @a iData
	 */
	static ScalarType get(const DataType& iData, size_t iIndex) 
	{ 
		return iData[iIndex]; 
	}
	/** set the @a iIndex'th scalar of @a iData to @a iValue
	 */
	static void set(DataType& ioData, size_t iIndex, ScalarType iValue) 
	{ 
		ioData[iIndex] = iValue; 
	}
	/** scale @a ioAcc by @a iScale
	 */
	static void scale(DataType& ioAcc, ScalarType iScale)
	{
		ioAcc *= iScale;
	}
	/** multiply @a iData by @a iScale and add result to @a ioAcc
	 */
	static void multiplyAccumulate(DataType& ioAcc, const DataType& iData, ScalarType iScale)
	{
		ioAcc += iData * iScale;
	}
};



template 
<
	typename ScalarType
>
struct DefaultDataTraits<ScalarType, ScalarType>
{
	/** return the number of scalar in one data element
	 */
	static size_t dimension(ScalarType iData)
	{
		return 1;
	}
	/** initialize @a ioData to an element with @a iDimension zeroes.
	 */
	static void zero(ScalarType& ioData, size_t iDimension)
	{
		LASS_ASSERT(iDimension == 1);
		ioData = ScalarType();
	}
	/** get the @a iIndex'th scalar of @a iData
	 */
	static ScalarType get(ScalarType iData, size_t iIndex) 
	{ 
		LASS_ASSERT(iIndex == 0);
		return iData; 
	}
	/** set the @a iIndex'th scalar of @a iData to @a iValue
	 */
	static void set(ScalarType& ioData, size_t iIndex, ScalarType iValue) 
	{ 
		LASS_ASSERT(iIndex == 0);
		ioData = iValue; 
	}
	/** scale @a ioAcc by @a iScale
	 */
	static void scale(ScalarType& ioAcc, ScalarType iScale)
	{
		ioAcc *= iScale;
	}
	/** multiply @a iData by @a iScale and add result to @a ioAcc
	 */
	static void multiplyAccumulate(ScalarType& ioAcc, ScalarType iData, ScalarType iScale)
	{
		ioAcc += iData * iScale;
	}
};



}

}

#endif

// EOF
