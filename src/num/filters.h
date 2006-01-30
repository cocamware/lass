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

/** @defgroup Filters
 *  @brief one dimensional causal filters ...
 */

#ifndef LASS_GUARDIAN_OF_INCLUSION_NUM_FILTERS_H
#define LASS_GUARDIAN_OF_INCLUSION_NUM_FILTERS_H

#include "num_common.h"
#include "num_traits.h"
#include "polynomial.h"

namespace lass
{
namespace num
{

/** Base class for all one dimensional causal filters
 *  @ingroup Filters
 */
template 
<	
	typename T,
	typename InputIterator = const T*,
	typename OutputIterator = T*
>
class Filter
{
public:
	typedef typename util::CallTraits<T>::TValue TValue;
	typedef typename util::CallTraits<T>::TParam TParam;
	typedef typename util::CallTraits<T>::TReference TReference;
	typedef typename util::CallTraits<T>::TConstReference TConstReference;
	typedef InputIterator TInputIterator;
	typedef OutputIterator TOutputIterator;
	typedef NumTraits<T> TNumTraits;

	virtual ~Filter() {}
	TOutputIterator operator()(TInputIterator iFirst, TInputIterator iLast, TOutputIterator oOutput) { return doFilter(iFirst, iLast, oOutput); }
	void reset() { doReset(); }

private:
	virtual TOutputIterator doFilter(TInputIterator iFirst, TInputIterator iLast, TOutputIterator oOutput) = 0;
	virtual void doReset() {}
};



/** Finite Impulse Response filter
 *  @ingroup Filters
 */
template 
<	
	typename T,
	typename InputIterator = const T*,
	typename OutputIterator = T*
>
class FirFilter: public Filter<T, InputIterator, OutputIterator>
{
public:
	typedef std::vector<T> TValues;

	FirFilter(const TValues& iImpulseResponse);
private:
	typedef std::vector<size_t> TIndexTable;

	TOutputIterator doFilter(TInputIterator iFirst, TInputIterator iLast, TOutputIterator oOutput);
	void doReset();

	TValues taps_;
	TValues buffer_;
	TIndexTable nextIndex_;
	size_t tapSize_;
	size_t bufferIndex_;
};



/** Infinite Impulse Response filter
 *  @ingroup Filters
 */
template 
<	
	typename T,
	typename InputIterator = const T*,
	typename OutputIterator = T*
>
class IirFilter: public Filter<T, InputIterator, OutputIterator>
{
public:
	typedef std::vector<T> TValues;
	typedef std::pair<TValues, TValues> TValuesPair;

	IirFilter(const TValues& iNominator, const TValues& iDenominator);
	IirFilter(const TValuesPair& iCoefficients);
private:
	typedef std::vector<size_t> TIndexTable;

	TOutputIterator doFilter(TInputIterator iFirst, TInputIterator iLast, TOutputIterator oOutput);
	void doReset();
	void init(const TValuesPair& iCoefficients);

	TValues xTaps_;
	TValues yTaps_;
	TValues xBuffer_;
	TValues yBuffer_;
	TIndexTable xNextIndex_;
	TIndexTable yNextIndex_;
	size_t xTapSize_;
	size_t yTapSize_;
	size_t xBufferIndex_;
	size_t yBufferIndex_;
};



/** Wrapper for IirFilter with transfer function in Laplace domain.
 *  @ingroup Filters
 */
template 
<	
	typename T,
	typename InputIterator = const T*,
	typename OutputIterator = T*
>
class LaplaceIirFilter: public IirFilter<T, InputIterator, OutputIterator>
{
public:
	LaplaceIirFilter(const TValues& iNominator, const TValues& iDenominator, TParam iSamplingFrequency);
	LaplaceIirFilter(const TValuesPair& iCoefficients, TParam iSamplingFrequency);
private:
	typedef num::Polynomial<T> TPolynomial;
	static TValuesPair laplaceToZ(const TValuesPair& iCoefficients, TParam iSamplingFrequency);
	static TPolynomial laplaceToZHelper(const TValues& iCoefficients, TParam iSamplingFrequency);
};

}

}

#include "filters.inl"

#endif

// EOF
