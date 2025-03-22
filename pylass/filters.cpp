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
 *	Copyright (C) 2004-2025 the Initial Developer.
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

#include "pylass_common.h"
#include "filters.h"

namespace pylass
{

typedef double TValue;
typedef std::vector<TValue> TValues;
typedef std::pair<TValues, TValues> TValuesPair;

template <typename FilterType>
TValues call(FilterType& filter, const TValues& seq)
{
	const TValue* first = &seq[0];
	const size_t n = seq.size();
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
