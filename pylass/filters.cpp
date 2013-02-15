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

#include "pylass_common.h"
#include "filters.h"

namespace pylass
{

typedef double TValue;
typedef std::vector<TValue> TValues;
typedef std::pair<TValues, TValues> TValuesPair;

template <typename FilterType>
TValues call(FilterType& filter, const TValues& in)
{
	const TValue* first = &in[0];
	const size_t n = in.size();
	TValues out(n);
	filter(first, first + n, &out[0]);
	return out;
}

template <typename FilterType>
void reset(FilterType& filter)
{
	filter.reset();
}

num::IirFilter<double> makeLaplaceFilter(const TValues& numerator, const TValues& denominator, double samplingFrequency)
{
	return num::IirFilter<double>::makeLaplace(numerator, denominator, samplingFrequency);
}

PY_DECLARE_CLASS(FirFilter)
PY_CLASS_CONSTRUCTOR_1(FirFilter, TValues)
PY_CLASS_FREE_METHOD_NAME(FirFilter, call< num::FirFilter<double> >, "__call__")
PY_CLASS_METHOD(FirFilter, reset)

PY_DECLARE_CLASS(IirFilter)
PY_CLASS_CONSTRUCTOR_2(IirFilter, TValues, TValues)
PY_CLASS_CONSTRUCTOR_1(IirFilter, TValuesPair)
PY_CLASS_FREE_METHOD_NAME(IirFilter, call< num::IirFilter<double> >, "__call__")
PY_CLASS_METHOD(IirFilter, reset)
PY_CLASS_STATIC_METHOD(IirFilter, makeLaplace)
PY_CLASS_STATIC_METHOD(IirFilter, makeButterworthLowPass)
PY_CLASS_STATIC_METHOD(IirFilter, makeButterworthHighPass)
PY_CLASS_STATIC_METHOD(IirFilter, makeRlcLowPass)
PY_CLASS_STATIC_METHOD(IirFilter, makeRlcHighPass)
PY_CLASS_STATIC_METHOD(IirFilter, makeRlcBandPass)
PY_CLASS_STATIC_METHOD(IirFilter, makeRlcNotch)
PY_CLASS_STATIC_METHOD(IirFilter, makeIntegrator)
PY_CLASS_STATIC_METHOD(IirFilter, makeDifferentiator)
PY_CLASS_STATIC_METHOD(IirFilter, makeAWeighting)

}
