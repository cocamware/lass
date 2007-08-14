/**	@file
 *	@author Bram de Greve (bramz@users.sourceforge.net)
 *	@author Tom De Muer (tomdemuer@users.sourceforge.net)
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
 *	Copyright (C) 2004-2007 the Initial Developer.
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
