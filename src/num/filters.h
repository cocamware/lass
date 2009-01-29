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

/** @defgroup Filters
 *  @brief one dimensional causal filters ...
 */

#ifndef LASS_GUARDIAN_OF_INCLUSION_NUM_FILTERS_H
#define LASS_GUARDIAN_OF_INCLUSION_NUM_FILTERS_H

#include "num_common.h"
#include "num_traits.h"

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
	TOutputIterator operator()(TInputIterator first, TInputIterator last, TOutputIterator output) { return doFilter(first, last, output); }
	void reset() { doReset(); }

private:
	virtual TOutputIterator doFilter(TInputIterator first, TInputIterator last, TOutputIterator output) = 0;
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
	typedef typename Filter<T, InputIterator, OutputIterator>::TValue TValue;
	typedef typename Filter<T, InputIterator, OutputIterator>::TParam TParam;
	typedef typename Filter<T, InputIterator, OutputIterator>::TReference TReference;
	typedef typename Filter<T, InputIterator, OutputIterator>::TConstReference TConstReference;
	typedef typename Filter<T, InputIterator, OutputIterator>::TInputIterator TInputIterator;
	typedef typename Filter<T, InputIterator, OutputIterator>::TOutputIterator TOutputIterator;
	typedef typename Filter<T, InputIterator, OutputIterator>::TNumTraits TNumTraits;

	typedef std::vector<T> TValues;

	FirFilter(const TValues& impulseResponse);
private:
	typedef std::vector<size_t> TIndexTable;

	TOutputIterator doFilter(TInputIterator first, TInputIterator last, TOutputIterator output);
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
	typedef typename Filter<T, InputIterator, OutputIterator>::TValue TValue;
	typedef typename Filter<T, InputIterator, OutputIterator>::TParam TParam;
	typedef typename Filter<T, InputIterator, OutputIterator>::TReference TReference;
	typedef typename Filter<T, InputIterator, OutputIterator>::TConstReference TConstReference;
	typedef typename Filter<T, InputIterator, OutputIterator>::TInputIterator TInputIterator;
	typedef typename Filter<T, InputIterator, OutputIterator>::TOutputIterator TOutputIterator;
	typedef typename Filter<T, InputIterator, OutputIterator>::TNumTraits TNumTraits;

	typedef std::vector<T> TValues;
	typedef std::pair<TValues, TValues> TValuesPair;

	IirFilter(const TValues& numerator, const TValues& denominator);
	IirFilter(const TValuesPair& coefficients);

	static IirFilter makeLaplace(const TValues& nominator, const TValues& denominator, TParam samplingFrequency);
	static IirFilter makeButterworthLowPass(unsigned order, TParam cutoffAngularFrequency, TParam gain, TParam samplingFrequency);
	static IirFilter makeButterworthHighPass(unsigned order, TParam cutoffAngularFrequency, TParam gain, TParam samplingFrequency);
	static IirFilter makeRlcLowPass(TParam qFactor, TParam cutoffAngularFrequency, TParam gain, TParam samplingFrequency);
	static IirFilter makeRlcHighPass(TParam qFactor, TParam cutoffAngularFrequency, TParam gain, TParam samplingFrequency);
	static IirFilter makeRlcBandPass(TParam qFactor, TParam centerAngularFrequency, TParam gain, TParam samplingFrequency);
	static IirFilter makeRlcNotch(TParam qFactor, TParam centerAngularFrequency, TParam gain, TParam samplingFrequency);
	static IirFilter makeIntegrator(TParam gain, TParam samplingFrequency);
	static IirFilter makeDifferentiator(TParam gain, TParam samplingFrequency);
	static IirFilter makeAWeighting(TParam samplingFrequency);

private:
	typedef std::vector<size_t> TIndexTable;

	TOutputIterator doFilter(TInputIterator first, TInputIterator last, TOutputIterator output);
	void doReset();
	void init(const TValues& numerator, const TValues& denominator);
	
	template <typename FwdIt1, typename FwdIt2>
	static IirFilter doMakeLaplace(FwdIt1 numFirst, FwdIt1 numLast, FwdIt2 denFirst, FwdIt2 denLast, TParam sampleFrequency);
	static IirFilter doMakeLaplace(const TValuesPair& coefficients, TParam sampleFrequency);

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

}

}

#include "filters.inl"

#endif

// EOF
