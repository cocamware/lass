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
#include "polynomial.h"
#include "../stde/extended_iterator.h"

namespace lass
{
namespace num
{
namespace impl
{
	template <typename T>
	num::Polynomial<T> laplaceToZHelper(const std::vector<T>& iCoefficients, const T& iSamplingFrequency)
	{
		typedef num::NumTraits<T> TNumTraits;
		typedef num::Polynomial<T> TPolynomial;

		const T twoFs = 2 * iSamplingFrequency;

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

	template <typename T>
	std::pair< std::vector<T>, std::vector<T> >
	laplaceToZ(const std::pair< std::vector<T>, std::vector<T> >& iCoefficients, const T& iSamplingFrequency)
	{
		typedef num::NumTraits<T> TNumTraits;
		typedef num::Polynomial<T> TPolynomial;

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

	template <typename T>
	std::pair< std::vector<T>, std::vector<T> > 
	lowpassButterworthCoefficients(unsigned n, const T& cutoff, const T& gain)
	{
		typedef num::NumTraits<T> TNumTraits;
		typedef num::Polynomial<T> TPolynomial;
		
		const TPolynomial s = TPolynomial::x() / cutoff;
		const TPolynomial s2 = s * s;

		TPolynomial den(TNumTraits::one);
		if (n % 2 == 1)
			den += s;
		for (unsigned k = 0; k < n / 2; ++k)
		{
			const T theta = (TNumTraits::pi * (2 * k + n + 1)) / (2 * n);
			den *= s2 - 2 * num::cos(theta) * s + TNumTraits::one;
		}
		return std::make_pair(TPolynomial::one().coefficients(), den.coefficients());
	}

	template <typename T>
	std::pair< std::vector<T>, std::vector<T> > 
	highpassButterworthCoefficients(unsigned n, const T& cutoff, const T& gain)
	{
		typedef num::NumTraits<T> TNumTraits;
		typedef std::pair< std::vector<T>, std::vector<T> > TValuesPair;

		TValuesPair result = lowpassButterworthCoefficients<T>(n, TNumTraits::one, TNumTraits::one);

		std::reverse(result.second.begin(), result.second.end());
		for (size_t k = 0; k < result.second.size(); ++k)
		{
			result.second[k] /= num::pow(cutoff, T(k));
		}

		result.first.resize(n + 1);
		std::fill(result.first.begin(), result.first.end(), TNumTraits::zero);
		result.first[n] = gain / num::pow(cutoff, T(n));

		return result;
	}
}

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
	this->reset();
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
	this->init(std::make_pair(iNominator, iDenominator));
	this->reset();
}



template <typename T, typename InIt, typename OutIt>
IirFilter<T, InIt, OutIt>::IirFilter(const TValuesPair& iCoefficients)
{
	this->init(iCoefficients);
	this->reset();
}



template <typename T, typename InIt, typename OutIt>
typename IirFilter<T, InIt, OutIt>::TOutputIterator
IirFilter<T, InIt, OutIt>::doFilter(TInputIterator iFirst, TInputIterator iLast, TOutputIterator oOutput)
{
	const T* const xTaps = &xTaps_[0];
	const T* const yTaps = yTaps_.empty() ? 0 : &yTaps_[0];
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
	
	const size_t xBufferSize = xTapSize_;
	const size_t yBufferSize = std::max<size_t>(yTapSize_, 1);
	xBuffer_.resize(2 * xBufferSize);
    yBuffer_.resize(2 * yBufferSize);
	xNextIndex_.resize(xBufferSize);
	yNextIndex_.resize(yBufferSize);
	for (size_t i = 0; i < xBufferSize; ++i)
	{
		xNextIndex_[i] = (i + xBufferSize - 1) % xBufferSize;
	}
	for (size_t i = 0; i < yBufferSize; ++i)
	{
		yNextIndex_[i] = (i + yBufferSize - 1) % yBufferSize;
	}

	xBufferIndex_ = 0;
	yBufferIndex_ = 0;
}



// --- LaplaceIirFilter ----------------------------------------------------------------------------

template <typename T, typename InIt, typename OutIt>
LaplaceIirFilter<T, InIt, OutIt>::LaplaceIirFilter(const TValues& iNominator, const TValues& iDenominator, 
									  TParam iSamplingFrequency):
	IirFilter<T, InIt, OutIt>(impl::laplaceToZ(std::make_pair(iNominator, iDenominator), iSamplingFrequency))
{
}



template <typename T, typename InIt, typename OutIt>
LaplaceIirFilter<T, InIt, OutIt>::LaplaceIirFilter(const TValuesPair& iCoefficients, TParam iSamplingFrequency):
	IirFilter<T, InIt, OutIt>(impl::laplaceToZ(iCoefficients, iSamplingFrequency))
{
}



// --- LowpassButterworthFilter --------------------------------------------------------------------

template <typename T, typename InIt, typename OutIt>
LowpassButterworthFilter<T, InIt, OutIt>::LowpassButterworthFilter(
		unsigned filterOrder, TParam cutoffAngularFrequency, TParam gain, TParam samplingFrequency):
	LaplaceIirFilter<T, InIt, OutIt>(impl::lowpassButterworthCoefficients(
		filterOrder, cutoffAngularFrequency, gain), samplingFrequency)
{
}



// --- HighpassButterworthFilter --------------------------------------------------------------------

template <typename T, typename InIt, typename OutIt>
HighpassButterworthFilter<T, InIt, OutIt>::HighpassButterworthFilter(
		unsigned filterOrder, TParam cutoffAngularFrequency, TParam gain, TParam samplingFrequency):
	LaplaceIirFilter<T, InIt, OutIt>(impl::highpassButterworthCoefficients(
		filterOrder, cutoffAngularFrequency, gain), samplingFrequency)
{
}






}

}

#endif

// EOF
