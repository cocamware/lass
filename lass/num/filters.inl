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
	template <typename T, typename ForwardIterator>
	num::Polynomial<T> laplaceToZHelper(ForwardIterator first, ForwardIterator last, const T& samplingFrequency)
	{
		typedef num::NumTraits<T> TNumTraits;
		typedef num::Polynomial<T> TPolynomial;

		TPolynomial oneMinZ = 2 * samplingFrequency * (TNumTraits::one - TPolynomial::x());
		TPolynomial onePlusZ = TNumTraits::one + TPolynomial::x();
		TPolynomial result;
		const int n = static_cast<int>(std::distance(first, last));
		int i = 0;
		while (first != last)
		{
			result += *first * oneMinZ.pow(i) * onePlusZ.pow(n - i - 1);
			++first;
			++i;
		}
		return result;
	}

	template <typename T, typename ForwardIterator1, typename ForwardIterator2>
	std::pair< std::vector<T>, std::vector<T> >
	laplaceToZ(ForwardIterator1 numeratorFirst, ForwardIterator1 numeratorLast, 
			ForwardIterator2 denominatorFirst, ForwardIterator2 denominatorLast, 
			const T& samplingFrequency)
	{
		typedef num::NumTraits<T> TNumTraits;
		typedef num::Polynomial<T> TPolynomial;

		TPolynomial num = laplaceToZHelper(numeratorFirst, numeratorLast, samplingFrequency);
		TPolynomial den = laplaceToZHelper(denominatorFirst, denominatorLast, samplingFrequency);
		TPolynomial onePlusZ = TNumTraits::one + TPolynomial::x();
		const size_t m = num.size();
		const size_t n = den.size();
		if (n > m)
		{
			num *= onePlusZ.pow(static_cast<unsigned>(n - m));
		}
		else
		{
			den *= onePlusZ.pow(static_cast<unsigned>(m - n));
		}
		return std::make_pair(num.coefficients(), den.coefficients());
	}

	template <typename T>
	std::pair< std::vector<T>, std::vector<T> > 
	laplaceButterworthLowPass(unsigned n, const T& cutoff, const T& gain)
	{
		typedef num::NumTraits<T> TNumTraits;
		typedef num::Polynomial<T> TPolynomial;
		
		const TPolynomial s = TPolynomial::x() / cutoff;
		const TPolynomial s2 = s * s;

		TPolynomial nom(gain);
		TPolynomial den(TNumTraits::one);
		if (n % 2 == 1)
			den += s;
		for (unsigned k = 0; k < n / 2; ++k)
		{
			const T theta = (TNumTraits::pi * (2 * k + n + 1)) / (2 * n);
			den *= s2 - 2 * num::cos(theta) * s + TNumTraits::one;
		}

		return std::make_pair(nom.coefficients(), den.coefficients());
	}

	template <typename T>
	std::pair< std::vector<T>, std::vector<T> > 
	laplaceButterworthHighPass(unsigned n, const T& cutoff, const T& gain)
	{
		typedef num::NumTraits<T> TNumTraits;
		typedef std::pair< std::vector<T>, std::vector<T> > TValuesPair;

		TValuesPair result = laplaceButterworthLowPass<T>(n, TNumTraits::one, TNumTraits::one);

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
FirFilter<T, InIt, OutIt>::FirFilter(const TValues& impulseResponse):
	taps_(impulseResponse),
	buffer_(2 * impulseResponse.size()),
	nextIndex_(impulseResponse.size()),
	tapSize_(impulseResponse.size()),
	bufferIndex_(0)
{
	if (impulseResponse.empty())
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
FirFilter<T, InIt, OutIt>::doFilter(TInputIterator first, TInputIterator last, TOutputIterator output)
{
	const T* const taps = &taps_[0];
	while (first != last)
	{
		buffer_[tapSize_ + bufferIndex_] = buffer_[bufferIndex_] = *first++;
		const T* const buf = &buffer_[bufferIndex_];
		TValue accumulator = TNumTraits::zero;
		for (size_t i = 0; i < tapSize_; ++i)
		{
			accumulator += taps[i] * buf[i];
		}
		bufferIndex_ = nextIndex_[bufferIndex_];
		*output++ = accumulator;
	}
	return output;
}



template <typename T, typename InIt, typename OutIt>
void FirFilter<T, InIt, OutIt>::doReset()
{
	std::fill(buffer_.begin(), buffer_.end(), TNumTraits::zero);
}



// --- IirFilter -----------------------------------------------------------------------------------

/** construct IIR filter
 *
 * @code
 *        a[0] + a[1] * z^-1 + a[2] * z^-2 + ... + a[m-1] * z^-(m-1)
 * H(z) = ----------------------------------------------------------
 *        b[0] + b[1] * z^-1 + b[2] * z^-2 + ... + b[m-1] * z^-(m-1)
 * @endcode
 *
 * @param numerator [in] coefficients a[i] of transfer function H(z).
 * @param denominator [in] coefficients b[i] of transfer function H(z).
 */
template <typename T, typename InIt, typename OutIt>
IirFilter<T, InIt, OutIt>::IirFilter(const TValues& numerator, const TValues& denominator)
{
	this->init(numerator, denominator);
	this->reset();
}



/** construct IIR filter
 *
 * @param coefficients [in] numerator (=first) and denominator (=second) of transfer function H(z).
 */
template <typename T, typename InIt, typename OutIt>
IirFilter<T, InIt, OutIt>::IirFilter(const TValuesPair& coefficients)
{
	this->init(coefficients.first, coefficients.second);
	this->reset();
}



/* make an IIR filter from a transfer function H(s) in Laplace domain.
 *
 * @code
 *        a[0] + a[1] * s + a[2] * s^2 + ... + a[m-1] * s^m-1
 * H(s) = ---------------------------------------------------
 *        b[0] + b[1] * s + b[2] * s^2 + ... + b[n-1] * s^n-1
 * @endcode
 *
 * @param numerator [in] coefficients a[i] of Laplace transfer function H(s).
 * @param denominator [in] coefficients b[i] of Laplace transfer function H(s).
 * @param samplingFrequency [in] sampling frequency of the digital signal.
 */
template <typename T, typename InIt, typename OutIt>
IirFilter<T, InIt, OutIt> IirFilter<T, InIt, OutIt>::makeLaplace(
		const TValues& numerator, const TValues& denominator, TParam samplingFrequency)
{
	return doMakeLaplace(numerator.begin(), numerator.end(), denominator.begin(), denominator.end(), samplingFrequency);
}



/** make an IIR filter implementing a low-pass butterworth filter.
 *
 *	@param order [in] order of filter.  filter rolls of at (order * 6) dB per decade.
 *	@param cutoffAngularFrequency [in] cutoff frequency measured in radians per sec (w = 2 pi f).
 *	@param gain [in] DC gain
 *	@param samplingFrequency [in] sampling frequency of digital signal
 */
template <typename T, typename InIt, typename OutIt>
IirFilter<T, InIt, OutIt> IirFilter<T, InIt, OutIt>::makeButterworthLowPass(
		unsigned order, TParam cutoffAngularFrequency, TParam gain, TParam samplingFrequency)
{
	return doMakeLaplace(impl::laplaceButterworthLowPass(order, cutoffAngularFrequency, gain), samplingFrequency);
}



/** make an IIR filter implementing a high-pass butterworth filter.
 *
 *	@param order [in] order of filter.  filter rolls of at (order * 6) dB per decade.
 *	@param cutoffAngularFrequency [in] cutoff frequency measured in radians per sec (w = 2 pi f).
 *	@param gain [in] DC gain
 *	@param samplingFrequency [in] sampling frequency of digital signal
 */
template <typename T, typename InIt, typename OutIt>
IirFilter<T, InIt, OutIt> IirFilter<T, InIt, OutIt>::makeButterworthHighPass(
		unsigned order, TParam cutoffAngularFrequency, TParam gain, TParam samplingFrequency)
{
	return doMakeLaplace(impl::laplaceButterworthHighPass(order, cutoffAngularFrequency, gain), samplingFrequency);
}



/** make an IIR filter implementing an low-pass RLC circuit.
 *
 *	@code
 *	 o---[R]---[sL]---*-----o
 *	                  |
 *	Vin            [1/sC]  Vout
 *	                  |
 *	 o----------------*-----o
 *	@endcode
 *
 *  @param qFactor [in] quality factor Q = sqrt(L/C)/R (Q > 1 for resonance peak, Q = 1/sqrt(2) for 2nd order butterworth).
 *  @param cutoffAngularFrequency [in] cutoff frequency measured in radians per sec (w = 2 pi f = 1 / sqrt(LC))
 *	@param gain [in] DC gain
 *	@param samplingFrequency [in] sampling frequency of digital signal
 */
template <typename T, typename InIt, typename OutIt>
IirFilter<T, InIt, OutIt> IirFilter<T, InIt, OutIt>::makeRlcLowPass(
		TParam qFactor, TParam cutoffAngularFrequency, TParam gain, TParam samplingFrequency)
{
	const TValue num[] = { gain };
	const TValue den[] = { TNumTraits::one, num::inv(cutoffAngularFrequency * qFactor), num::inv(num::sqr(cutoffAngularFrequency)) };
	return doMakeLaplace(num, num + 1, den, den + 3, samplingFrequency);
}



/** make an IIR filter implementing an high-pass RLC circuit.
 *
 *	@code
 *	 o---[R]---[1/sC]---*----o
 *	                    |
 *	Vin               [sL]  Vout
 *	                    |
 *	 o------------------*----o
 *	@endcode
 *
 *  @param qFactor [in] quality factor Q = sqrt(L/C)/R (Q > 1 for resonance peak, Q = 1/sqrt(2) for 2nd order butterworth).
 *  @param cutoffAngularFrequency [in] cutoff frequency measured in radians per sec (w = 2 pi f = 1 / sqrt(LC))
 *	@param gain [in] DC gain
 *	@param samplingFrequency [in] sampling frequency of digital signal
 */
template <typename T, typename InIt, typename OutIt>
IirFilter<T, InIt, OutIt> IirFilter<T, InIt, OutIt>::makeRlcHighPass(
		TParam qFactor, TParam cutoffAngularFrequency, TParam gain, TParam samplingFrequency)
{
	const TValue num[] = { 0, 0, gain / num::sqr(cutoffAngularFrequency) };
	const TValue den[] = { TNumTraits::one, num::inv(cutoffAngularFrequency * qFactor), num::inv(num::sqr(cutoffAngularFrequency)) };
	return doMakeLaplace(num, num + 3, den, den + 3, samplingFrequency);
}



/** make an IIR filter implementing an band-pass RLC circuit.
 *
 *	@code
 *	 o---[sL]---[1/sC]---*----o
 *	                     |
 *	Vin                 [R]  Vout
 *	                     |
 *	 o-------------------*----o
 *	@endcode
 *
 *  @param qFactor [in] quality factor Q = sqrt(L/C)/R (greater Q is more narrow stop band).
 *  @param centerAngularFrequency [in] frequency to be filter out, measured in radians per sec (w = 2 pi f = 1 / sqrt(LC))
 *	@param gain [in] DC gain
 *	@param samplingFrequency [in] sampling frequency of digital signal
 */
template <typename T, typename InIt, typename OutIt>
IirFilter<T, InIt, OutIt> IirFilter<T, InIt, OutIt>::makeRlcBandPass(
		TParam qFactor, TParam centerAngularFrequency, TParam gain, TParam samplingFrequency)
{
	const TValue num[] = { TNumTraits::zero, gain / (centerAngularFrequency * qFactor) };
	const TValue den[] = { TNumTraits::one, num::inv(centerAngularFrequency * qFactor), num::inv(num::sqr(centerAngularFrequency)) };
	return doMakeLaplace(num, num + 2, den, den + 3, samplingFrequency);
}



/** make an IIR filter implementing an notch RLC circuit.
 *
 *	@code
 *	 o---[R]---*----o
 *	           |
 *	         [sL]
 *	Vin        |   Vout
 *	        [1/sC]
 *	           |
 *	 o---------*----o
 *	@endcode
 *
 *  @param qFactor [in] quality factor Q = sqrt(L/C)/R (greater Q is more narrow stop band).
 *  @param centerAngularFrequency [in] frequency to be filter out, measured in radians per sec (w = 2 pi f = 1 / sqrt(LC))
 *	@param gain [in] DC gain
 *	@param samplingFrequency [in] sampling frequency of digital signal
 */
template <typename T, typename InIt, typename OutIt>
IirFilter<T, InIt, OutIt> IirFilter<T, InIt, OutIt>::makeRlcNotch(
		TParam qFactor, TParam centerAngularFrequency, TParam gain, TParam samplingFrequency)
{
	const TValue num[] = { gain, TNumTraits::zero, gain / num::sqr(centerAngularFrequency) };
	const TValue den[] = { TNumTraits::one, num::inv(centerAngularFrequency * qFactor), num::inv(num::sqr(centerAngularFrequency)) };
	return doMakeLaplace(num, num + 3, den, den + 3, samplingFrequency);
}



/** make an IIR filter implementing a perfect integrator
 *
 *	@param gain [in] DC gain
 *	@param samplingFrequency [in] sampling frequency of digital signal
 */
template <typename T, typename InIt, typename OutIt>
IirFilter<T, InIt, OutIt> IirFilter<T, InIt, OutIt>::makeIntegrator(TParam gain, TParam samplingFrequency)
{
	TValue num[] = { gain };
	TValue den[] = { TNumTraits::zero, TNumTraits::one, };
	return doMakeLaplace(num, num + 1, den, den + 2, samplingFrequency);
}



/** make an IIR filter implementing a perfect differentiator
 *
 *	@param gain [in] DC gain
 *	@param samplingFrequency [in] sampling frequency of digital signal
 */
template <typename T, typename InIt, typename OutIt>
IirFilter<T, InIt, OutIt> IirFilter<T, InIt, OutIt>::makeDifferentiator(TParam gain, TParam samplingFrequency)
{
	TValue num[] = { TNumTraits::zero, gain, };
	TValue den[] = { TNumTraits::one, };
	return doMakeLaplace(num, num + 2, den, den + 1, samplingFrequency);
}



/** make an A weighting filter
 *
 *	@param samplingFrequency [in] sampling frequency of digital signal
 */
template <typename T, typename InIt, typename OutIt>
IirFilter<T, InIt, OutIt> IirFilter<T, InIt, OutIt>::makeAWeighting(TParam samplingFrequency)
{
	TValue num[] = { 0, 0, 0, 0, 7.39705e9 };
	TValue den[] = { .3086662405e21, .5301498365e19, .2674964995e17, .3343329206e14, 6734684542., 158881.5, 1 };
	return doMakeLaplace(
		num, num + sizeof(num)/sizeof(TValue), 
		den, den + sizeof(den)/sizeof(TValue), 
		samplingFrequency);
}



template <typename T, typename InIt, typename OutIt>
typename IirFilter<T, InIt, OutIt>::TOutputIterator
IirFilter<T, InIt, OutIt>::doFilter(TInputIterator first, TInputIterator last, TOutputIterator output)
{
	const T* const xTaps = &xTaps_[0];
	const T* const yTaps = yTaps_.empty() ? 0 : &yTaps_[0];
	while (first != last)
	{
		xBuffer_[xTapSize_ + xBufferIndex_] = xBuffer_[xBufferIndex_] = *first++;
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
		*output++ = accumulator;
	}
	return output;
}



template <typename T, typename InIt, typename OutIt>
void IirFilter<T, InIt, OutIt>::doReset()
{
	std::fill(xBuffer_.begin(), xBuffer_.end(), TNumTraits::zero);
	std::fill(yBuffer_.begin(), yBuffer_.end(), TNumTraits::zero);
}



template <typename T, typename InIt, typename OutIt>
void IirFilter<T, InIt, OutIt>::init(const TValues& numerator, const TValues& denominator)
{
	if (numerator.empty() || denominator.empty())
	{
		LASS_THROW("Cannot use an empty vector as numerator or denominator");
	}
	if (denominator[0] == TNumTraits::zero)
	{
		LASS_THROW("Cannot use a zero as first element in the denominator");
	}

	xTaps_ = numerator;
	yTaps_.assign(stde::next(denominator.begin()), denominator.end());
	xTapSize_ = xTaps_.size();
	yTapSize_ = yTaps_.size();
	const TValue scaler = num::inv(denominator[0]);
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



/** little private helper
 */
template <typename T, typename InIt, typename OutIt>
template <typename FwdIt1, typename FwdIt2>
IirFilter<T, InIt, OutIt> IirFilter<T, InIt, OutIt>::doMakeLaplace(
		FwdIt1 numFirst, FwdIt1 numLast, FwdIt2 denFirst, FwdIt2 denLast, TParam samplingFrequency)
{
	return IirFilter<T, InIt, OutIt>(impl::laplaceToZ(numFirst, numLast, denFirst, denLast, samplingFrequency));
}



/** little private helper
 */
template <typename T, typename InIt, typename OutIt>
IirFilter<T, InIt, OutIt> IirFilter<T, InIt, OutIt>::doMakeLaplace(
		const TValuesPair& coefficients, TParam samplingFrequency)
{
	return doMakeLaplace(coefficients.first.begin(), coefficients.first.end(),
		coefficients.second.begin(), coefficients.second.end(), 
		samplingFrequency);
}

}

}

#endif

// EOF
