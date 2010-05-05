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

#ifndef LASS_GUARDIAN_OF_INCLUSION_NUM_INVERSE_TRANSFORM_SAMPLING_H
#define LASS_GUARDIAN_OF_INCLUSION_NUM_INVERSE_TRANSFORM_SAMPLING_H

#include "num_common.h"
#include "../prim/point_2d.h"

#if LASS_COMPILER_TYPE == LASS_COMPILER_TYPE_MSVC
#	pragma warning(push)
#	pragma warning(disable: 4996) // std::partial_sum, std::transform, ...: Function call with parameters that may be unsafe
#endif

namespace lass
{
namespace num
{
namespace impl
{

/** 
 * @param [first, last) [in]
 * @param sample [in] value between 0 and 1, inclusive
 * @param pdf [out] probability density of returned sample
 * @param index [out]
 * @return value between 0 and 1.
 */
template <typename RandomIterator, typename T>
T sampleCdf1D(RandomIterator first, RandomIterator last, T sample, T& pdf, size_t& index)
{
	LASS_ASSERT(first != last);
	const T n = static_cast<T>(last - first);
	const RandomIterator it = std::lower_bound(first, last, sample);
	if (it == first)
	{
		pdf = n * *first;
		index = 0;
		return sample / pdf;
	}
	if (it == last)
	{
		pdf = n * (*(last - 1) - *(last - 2));
		index = static_cast<size_t>(last - first) - 1;
		return 1;
	}
	const RandomIterator prev = it - 1;
	index = static_cast<size_t>(it - first);
	pdf = n * (*it - *prev);
	const T x0 = static_cast<T>(index) / n;
	return x0 + (sample - *prev) / pdf;
}

}

template <typename T>
class InverseTransformSampling2D
{
public:
	typedef T TValue;
	typedef prim::Point2D<T> TSample;
	typedef prim::Point2D<size_t> TIndex;

	InverseTransformSampling2D(): uSize_(0), vSize_(0) {}

	template <typename InputIterator>
	InverseTransformSampling2D(InputIterator first, InputIterator last, size_t uSize, size_t vSize)
	{
		reset(first, last, uSize, vSize);
	}

	template <typename InputIterator>
	void reset(InputIterator first, InputIterator last, size_t uSize, size_t vSize)
	{
		LASS_ENFORCE(uSize > 0 && vSize > 0);
		uSize_ = uSize; 
		vSize_ = vSize;
		condCdfV_.resize(uSize * vSize);
		std::copy(first, last, condCdfV_.begin());
		buildCdf();
	}

	TSample operator()(const TSample& in, TValue& pdf, TIndex& index) const
	{
		LASS_ASSERT(!margCdfU_.empty());
		TValue pdfU, pdfV;
		const TValue u = impl::sampleCdf1D(margCdfU_.begin(), margCdfU_.end(), in.x, pdfU, index.x);
		const TValue* cdfV = &condCdfV_[index.x * vSize_];
		const TValue v = impl::sampleCdf1D(cdfV, cdfV + vSize_, in.y, pdfV, index.y);
		pdf = pdfU * pdfV;
		return TSample(u, v);
	}

private:
	void buildCdf()
	{
		LASS_ENFORCE(condCdfV_.size() == uSize_ * vSize_);
		margCdfU_.resize(uSize_);
		for (size_t i = 0; i < uSize_; ++i)
		{
			TValue* cdfV = &condCdfV_[i * vSize_];
			std::partial_sum(cdfV, cdfV + vSize_, cdfV);
			margCdfU_[i] = cdfV[vSize_ - 1];
			std::transform(cdfV, cdfV + vSize_, cdfV, std::bind2nd(std::divides<TValue>(), cdfV[vSize_ - 1]));
		}
		std::partial_sum(margCdfU_.begin(), margCdfU_.end(), margCdfU_.begin());
		std::transform(margCdfU_.begin(), margCdfU_.end(), margCdfU_.begin(), std::bind2nd(std::divides<TValue>(), margCdfU_.back()));
	}

	std::vector<TValue> condCdfV_;
	std::vector<TValue> margCdfU_;
	size_t uSize_;
	size_t vSize_;
};


}
}

#if LASS_COMPILER_TYPE == LASS_COMPILER_TYPE_MSVC
#	pragma warning(pop)
#endif

#endif