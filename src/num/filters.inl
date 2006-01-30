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

#ifndef LASS_GUARDIAN_OF_INCLUSION_NUM_FILTERS_INL
#define LASS_GUARDIAN_OF_INCLUSION_NUM_FILTERS_INL

#include "num_common.h"
#include "filters.h"
#include "../stde/extended_iterator.h"

namespace lass
{
namespace num
{

// --- FirFilter -----------------------------------------------------------------------------------

template <typename T, typename InIt, typename OutIt>
FirFilter<T, InIt, OutIt>::FirFilter(const TValues& iImpulseResponse):
	taps_(iImpulseResponse),
	buffer_(2 * iImpulseResponse.size()),
	nextIndex_(iImpulseResponse.size()),
	tapSize_(iImpulseResponse.size()),
	bufferIndex_(0)
{
	if (iImpulseResponse.empty())
	{
		LASS_THROW("Cannot use an empty vector as impulse response");
	}

	for (size_t i = 0; i < tapSize_; ++i)
	{
		nextIndex_[i] = (i - 1 + tapSize_) % tapSize_;
	}
	reset();
}



template <typename T, typename InIt, typename OutIt>
typename FirFilter<T, InIt, OutIt>::TOutputIterator
FirFilter<T, InIt, OutIt>::doFilter(TInputIterator iFirst, TInputIterator iLast, TOutputIterator oOutput)
{
	const T* const taps = &taps_[0];
	while (iFirst != iLast)
	{
		buffer_[tapSize_ + bufferIndex_] = buffer_[bufferIndex_] = *iFirst++;
		const T* const buf = &buffer_[bufferIndex_];
		TValue accumulator = TNumTraits::zero;
		for (size_t i = 0; i < tapSize_; ++i)
		{
			accumulator += taps[i] * buf[i];
		}
		bufferIndex_ = nextIndex_[bufferIndex_];
		*oOutput++ = accumulator;
	}
	return oOutput;
}



template <typename T, typename InIt, typename OutIt>
void FirFilter<T, InIt, OutIt>::doReset()
{
	std::fill(buffer_.begin(), buffer_.end(), TNumTraits::zero);
}

// --- IirFilter -----------------------------------------------------------------------------------

template <typename T, typename InIt, typename OutIt>
IirFilter<T, InIt, OutIt>::IirFilter(const TValues& iNominator, const TValues& iDenominator)
{
	init(std::make_pair(iNominator, iDenominator));
	reset();
}



template <typename T, typename InIt, typename OutIt>
IirFilter<T, InIt, OutIt>::IirFilter(const TValuesPair& iCoefficients)
{
	init(iCoefficients);
	reset();
}



template <typename T, typename InIt, typename OutIt>
typename IirFilter<T, InIt, OutIt>::TOutputIterator
IirFilter<T, InIt, OutIt>::doFilter(TInputIterator iFirst, TInputIterator iLast, TOutputIterator oOutput)
{
	const T* const xTaps = &xTaps_[0];
	const T* const yTaps = &yTaps_[0];
	while (iFirst != iLast)
	{
		xBuffer_[xTapSize_ + xBufferIndex_] = xBuffer_[xBufferIndex_] = *iFirst++;
		const T* const xBuf = &xBuffer_[xBufferIndex_];
		const T* const yBuf = &yBuffer_[yBufferIndex_];
		TValue accumulator = TNumTraits::zero;
		for (size_t i = 0; i < xTapSize_; ++i)
		{
			accumulator += xTaps[i] * xBuf[i];
		}
		for (size_t i = 0; i < yTapSize_; ++i)
		{
			accumulator -= yTaps[i] * yBuf[i];
		}
		xBufferIndex_ = xNextIndex_[xBufferIndex_];
		yBufferIndex_ = yNextIndex_[yBufferIndex_];
		yBuffer_[yTapSize_ + yBufferIndex_] = yBuffer_[yBufferIndex_] = accumulator;
		*oOutput++ = accumulator;
	}
	return oOutput;
}



template <typename T, typename InIt, typename OutIt>
void IirFilter<T, InIt, OutIt>::doReset()
{
	std::fill(xBuffer_.begin(), xBuffer_.end(), TNumTraits::zero);
	std::fill(yBuffer_.begin(), yBuffer_.end(), TNumTraits::zero);
}



template <typename T, typename InIt, typename OutIt>
void IirFilter<T, InIt, OutIt>::init(const TValuesPair& iCoefficients)
{
	if (iCoefficients.first.empty() || iCoefficients.second.empty())
	{
		LASS_THROW("Cannot use an empty vector as coefficients");
	}
	if (iCoefficients.second[0] == TNumTraits::zero)
	{
		LASS_THROW("Cannot use an zero as first element in the denominator");
	}

	xTaps_ = iCoefficients.first;
	yTaps_.assign(stde::next(iCoefficients.second.begin()), iCoefficients.second.end());
	xTapSize_ = xTaps_.size();
	yTapSize_ = yTaps_.size();
	const TValue scaler = num::inv(iCoefficients.second[0]);
	for (size_t i = 0; i < xTapSize_; ++i)
	{
		xTaps_[i] *= scaler;
	}
	for (size_t i = 0; i < yTapSize_; ++i)
	{
		yTaps_[i] *= scaler;
	}
    xBuffer_.resize(2 * xTapSize_);
    yBuffer_.resize(2 * yTapSize_);
	xBufferIndex_ = 0;
	yBufferIndex_ = 0;
	xNextIndex_.resize(xTapSize_);
	yNextIndex_.resize(yTapSize_);
	for (size_t i = 0; i < xTapSize_; ++i)
	{
		xNextIndex_[i] = (i + xTapSize_ - 1) % xTapSize_;
	}
	for (size_t i = 0; i < yTapSize_; ++i)
	{
		yNextIndex_[i] = (i + yTapSize_ - 1) % yTapSize_;
	}
}



// --- LaplaceIirFilter ----------------------------------------------------------------------------

template <typename T, typename InIt, typename OutIt>
LaplaceIirFilter<T, InIt, OutIt>::LaplaceIirFilter(const TValues& iNominator, const TValues& iDenominator, 
									  TParam iSamplingFrequency):
	IirFilter<T, InIt, OutIt>(laplaceToZ(std::make_pair(iNominator, iDenominator), iSamplingFrequency))
{
}



template <typename T, typename InIt, typename OutIt>
LaplaceIirFilter<T, InIt, OutIt>::LaplaceIirFilter(const TValuesPair& iCoefficients, TParam iSamplingFrequency):
	IirFilter<T, InIt, OutIt>(laplaceToZ(iCoefficients, iSamplingFrequency))
{
}



template <typename T, typename InIt, typename OutIt>
typename LaplaceIirFilter<T, InIt, OutIt>::TValuesPair
LaplaceIirFilter<T, InIt, OutIt>::laplaceToZ(const TValuesPair& iCoefficients, TParam iSamplingFrequency)
{
	TPolynomial num = laplaceToZHelper(iCoefficients.first, iSamplingFrequency);
	TPolynomial den = laplaceToZHelper(iCoefficients.second, iSamplingFrequency);
	TPolynomial onePlusZ = TNumTraits::one + TPolynomial::x();
	const size_t m = iCoefficients.first.size();
	const size_t n = iCoefficients.second.size();
	if (n > m)
	{
		num *= onePlusZ.pow(n - m);
	}
	else
	{
		den *= onePlusZ.pow(m - n);
	}
	return std::make_pair(num.coefficients(), den.coefficients());
}



template <typename T, typename InIt, typename OutIt>
typename LaplaceIirFilter<T, InIt, OutIt>::TPolynomial
LaplaceIirFilter<T, InIt, OutIt>::laplaceToZHelper(const TValues& iCoefficients, TParam iSamplingFrequency)
{
	TValue twoFs = 2 * iSamplingFrequency;

	TPolynomial oneMinZ = 2 * iSamplingFrequency * (TNumTraits::one - TPolynomial::x());
	TPolynomial onePlusZ = TNumTraits::one + TPolynomial::x();
	TPolynomial result;
	const size_t n = iCoefficients.size();
	for (size_t i = 0; i < n; ++i)
	{
		result += iCoefficients[i] * oneMinZ.pow(i) * onePlusZ.pow(n - i - 1);
	}
	return result;
}



}

}

#endif

// EOF
