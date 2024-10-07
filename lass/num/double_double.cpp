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
 *	Copyright (C) 2024 the Initial Developer.
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

#include "lass_common.h"
#include "double_double.h"

namespace lass
{
namespace num
{
namespace impl
{

extern const double expC1;
extern const DoubleDouble expC2;
extern const size_t expI_q_size;
extern const DoubleDouble expI_q[];

}

using num::DoubleDouble;
using TNumTraits = NumTraits<DoubleDouble>;


/** Compute the exponential function of a DoubleDouble
 *
 *  @param x The argument of the exponential function.
 *  @return The exponential function of x
 *
 *  We use the argument reduction technique described in [Johansson2015]
 *  First, we compute n such that:
 *      exp(x) = 2^n * exp(xx) with 0 <= xx = x - n * log(2) < log(2).
 *  Then, we compute i so that:
 *      exp(xx) = exp(w) * exp(i/q) with 0 <= w = xx - i/q < 1/q = 1/256.
 */
DoubleDouble exp(const DoubleDouble& x)
{
    constexpr int r = 8;
    constexpr int q = 1 << r;
    constexpr double u = static_cast<double>(TNumTraits::epsilon) / 2;

    // Argument reduction: compute n such that 0 <= xx < log(2)
    // Using classical Code-Waite reduction [Muller2016] with expC1 + expC2 = log(2).
    const auto n = num::floor(x.high() / TNumTraits::log2.high());
    const auto xx = x - n * impl::expC1 - n * impl::expC2;
    LASS_ASSERT(xx.high() >= 0.0 && xx.high() < TNumTraits::log2.high());

    // Do a further reduction to 0 <= w < 1/q = 1/256
    const auto i = std::max(num::floor(q * xx.high()), 0.);
    const auto i_q = i / q; // exact
    const auto w = xx - i_q;

    // compute exp(w) - 1
    DoubleDouble expm1W = w;
    DoubleDouble ww = w;
    double denom = 1.;
    for (int k = 2; k < 12; ++k)
    {
        ww *= w;
        denom *= k;
        const DoubleDouble s = ww / denom;
        if (num::abs(s.high()) < u / 2)
        {
            break;
        }
        expm1W += s;
    }

    // exp(w + i/q) = exp(w) * exp(i/q)
    // but because w is very small, and exp(w) is close to 1,
    // precision is better preserved by computing using:
    // exp(w + i/q) = (exp(w) - 1) * exp(i/q) + exp(i/q)
    LASS_ASSERT(static_cast<size_t>(i) < impl::expI_q_size);
    const DoubleDouble expI_q = impl::expI_q[static_cast<size_t>(i)];
    const DoubleDouble expXX = expm1W * expI_q + expI_q;

    // Compute 2^n * exp(r)^256.
    return ldexp(expXX, static_cast<int>(n));
}

DoubleDouble log(const DoubleDouble& x)
{
    DoubleDouble y = num::log(x.high());
    y += x * exp(-y) - 1.;
    return y;
}


}
}
