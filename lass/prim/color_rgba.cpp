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


#include "lass_common.h"
#include "color_rgba.h"
#include "vector_3d.h"

// for this translation unit, we don't want any warnings on numerical conversions.
#if LASS_COMPILER_TYPE == LASS_COMPILER_TYPE_MSVC
#	pragma warning(disable: 4305 4244)
#endif

namespace lass
{
namespace prim
{



// --- public --------------------------------------------------------------------------------------

/** construct an unexisting color (black with zero alpha)
 */
ColorRGBA::ColorRGBA():
	r(TNumTraits::zero),
	g(TNumTraits::zero),
	b(TNumTraits::zero),
	a(TNumTraits::zero)
{
	LASS_ASSERT(isZero());
}



/** construct a color with values for all channels
 */
ColorRGBA::ColorRGBA(TParam iRed, TParam iGreen, TParam iBlue, TParam iAlpha):
	r(iRed),
	g(iGreen),
	b(iBlue),
	a(iAlpha)
{
}



/** construct a color in white range (r == g == b),  with an alpha value
 */
ColorRGBA::ColorRGBA(TParam iWhite, TParam iAlpha):
	r(iWhite),
	g(iWhite),
	b(iWhite),
	a(iAlpha)
{
}



/** construct a color from a raw vector
 */
ColorRGBA::ColorRGBA(const TVector& iVector):
	r(iVector.x),
	g(iVector.y),
	b(iVector.z),
	a(iVector.w)
{
}



/** @e raw addition of @a iOther to this color, including alpha channel
 */
ColorRGBA& ColorRGBA::operator+=(const ColorRGBA& iOther)
{
	r += iOther.r;
	g += iOther.g;
	b += iOther.b;
	a += iOther.a;
	return *this;
}



/** @e raw subtraction @a iOther from this color, including alpha channel
 */
ColorRGBA& ColorRGBA::operator-=(const ColorRGBA& iOther)
{
	r -= iOther.r;
	g -= iOther.g;
	b -= iOther.b;
	a -= iOther.a;
	return *this;
}



/** @e raw multiplication of @a iOther with this color, including alpha channel
 */
ColorRGBA& ColorRGBA::operator*=(const ColorRGBA& iOther)
{
	r *= iOther.r;
	g *= iOther.g;
	b *= iOther.b;
	a *= iOther.a;
	return *this;
}



/** @e raw division of this color by @a iOther, including alpha channel
 */
ColorRGBA& ColorRGBA::operator/=(const ColorRGBA& iOther)
{
	r /= iOther.r;
	g /= iOther.g;
	b /= iOther.b;
	a /= iOther.a;
	return *this;
}



/** @e raw addition of @a iWhite to this color, including alpha channel
 */
ColorRGBA& ColorRGBA::operator+=(TParam iWhite)
{
	r += iWhite;
	g += iWhite;
	b += iWhite;
	a += iWhite;
	return *this;
}



/** @e raw subtraction @a iWhite from this color, including alpha channel
 */
ColorRGBA& ColorRGBA::operator-=(TParam iWhite)
{
	r -= iWhite;
	g -= iWhite;
	b -= iWhite;
	a -= iWhite;
	return *this;
}



/** @e raw multiplication of @a iWhite with this color, including alpha channel
 */
ColorRGBA& ColorRGBA::operator*=(TParam iWhite)
{
	r *= iWhite;
	g *= iWhite;
	b *= iWhite;
	a *= iWhite;
	return *this;
}



/** @e raw division of this color by @a iWhite, including alpha channel
 */
ColorRGBA& ColorRGBA::operator/=(TParam iWhite)
{
	const TValue invWhite = TNumTraits::one / iWhite;
	r *= invWhite;
	g *= invWhite;
	b *= invWhite;
	a *= invWhite;
	return *this;
}



const ColorRGBA::TValue ColorRGBA::brightness() const
{
	return (r + g + b) / 3;
}



/** return true if all color components are zero
 */
const bool ColorRGBA::isBlack() const
{
	return r == TNumTraits::zero && g == TNumTraits::zero && b == TNumTraits::zero;
}



/** return true if all components are zero
 */
const bool ColorRGBA::isZero() const
{
	return r == TNumTraits::zero && g == TNumTraits::zero && b == TNumTraits::zero &&
		a == TNumTraits::zero;
}



/** Return true if at least one of the components is NaN
 */
const bool ColorRGBA::isNaN() const
{
	return num::isNaN(r) || num::isNaN(g) || num::isNaN(b) || num::isNaN(a);
}



/** return darkened colour without changing the opaqueness.
 *
 *  @pre @a *this are considered non-premultiplied
 *
 *  @code
 *  alphaR = alphaA.
 *  ColorR = ColorA * darkenFactor.
 *  @endcode
 *
 *  @arg T. Porter, T. Duff. Compositing Digital Images, Comp. Graphics, 18(3):253-259, July 1984.
 */
const ColorRGBA ColorRGBA::darkened(ColorRGBA::TParam iFactor) const
{
	ColorRGBA result(*this);
	result *= iFactor;
	result.a = a;
	return result;
}



/** return color with dissolved opaqueness
 *
 *  @pre @a iA and @a iB are considered non-premultiplied
 *
 *  @code
 *  alphaR = alphaA * dissolveFactor.
 *  ColorR = ColorA.
 *  @endcode
 *
 *  @arg T. Porter, T. Duff. Compositing Digital Images, Comp. Graphics, 18(3):253-259, July 1984.
 */
const ColorRGBA ColorRGBA::dissolved(TParam iFactor) const
{
	ColorRGBA result(*this);
	result.a *= iFactor;
	return result;
}



/** return gamma corrected color.
 *
 *  corrects gamma of all channels, except alpha.
 *
 *  @pre @a iA and @a iB are considered non-premultiplied
 *
 *  @code
 *  alphaR = alphaA.
 *  ColorR = ColorA ** (1 / gamma).
 *  @endcode
 */
const ColorRGBA ColorRGBA::gammaCorrected(TParam iGammaExponent) const
{
	const TValue invGamma = num::inv(iGammaExponent);
	return ColorRGBA(num::pow(r, invGamma), num::pow(g, invGamma), num::pow(b, invGamma), a);
}



/** return exposed color.
 *
 *  apply exposure function to color and clamp alpha channel
 *
 *  @pre @a iA and @a iB are considered non-premultiplied
 *  @post: all channel values ar in the range [0, 1].
 *
 *  @code
 *  alphaR = alphaA.
 *  ColorR = 1 - exp(-time * ColorA).
 *  @endcode
 */
const ColorRGBA ColorRGBA::exposed(TParam iExposureTime) const
{
	const TValue f = -iExposureTime;
	return ColorRGBA(
		num::clamp(TNumTraits::one - num::exp(f * r), TNumTraits::zero, TNumTraits::one),
		num::clamp(TNumTraits::one - num::exp(f * g), TNumTraits::zero, TNumTraits::one),
		num::clamp(TNumTraits::one - num::exp(f * b), TNumTraits::zero, TNumTraits::one),
		num::clamp(a, TNumTraits::zero, TNumTraits::one));
}



/** return result of inverse exposure function
 *
 *  apply inverse of exposure function to color and clamp alpha channel
 *
 *  @pre @a iA and @a iB are considered non-premultiplied
 *  @post: all channel values ar in the range [0, 1].
 *
 *  @code
 *  alphaR = alphaA.
 *  ColorR = log(1 - colorA) / -time
 *  @endcode
 */
const ColorRGBA ColorRGBA::invExposed(TParam iExposureTime) const
{
	const TValue f =  num::inv(-iExposureTime);
	return ColorRGBA(
		f * num::log(num::clamp(TNumTraits::one - r, TNumTraits::minStrictPositive, TNumTraits::one)),
		f * num::log(num::clamp(TNumTraits::one - g, TNumTraits::minStrictPositive, TNumTraits::one)),
		f * num::log(num::clamp(TNumTraits::one - b, TNumTraits::minStrictPositive, TNumTraits::one)),
		a);
}



/** clamp all channels (including alpha channel) to the range [0, 1].
 *  @post: all channel values ar in the range [0, 1].
 */
const ColorRGBA ColorRGBA::clamped() const
{
	return ColorRGBA(
		num::clamp(r, TNumTraits::zero, TNumTraits::one),
		num::clamp(g, TNumTraits::zero, TNumTraits::one),
		num::clamp(b, TNumTraits::zero, TNumTraits::one),
		num::clamp(a, TNumTraits::zero, TNumTraits::one));
}



/** convert a value in range [0, 1] to a color like in colormap 'autumn' of matlab.
 */
const ColorRGBA ColorRGBA::mapAutumn(TParam iValue)
{
	num::inpclamp(iValue, TNumTraits::zero, TNumTraits::one);
	return ColorRGBA(TNumTraits::one, iValue, TNumTraits::zero);
}



/** convert a value in range [0, 1] to a color like in colormap 'bone' of matlab.
 */
const ColorRGBA ColorRGBA::mapBone(TParam iValue)
{
	const static ColorRGBA keys[] =
	{
		ColorRGBA(0.    , 0.    , 0.    ),
		ColorRGBA(0.3194, 0.3194, 0.4444),
		ColorRGBA(0.6528, 0.7778, 0.7778),
		ColorRGBA(1.    , 1.    , 1.    )
	};
	return doMap(iValue, keys, 4);
}



/** convert a value in range [0, 1] to a color like in colormap 'cool' of matlab.
 */
const ColorRGBA ColorRGBA::mapCool(TParam iValue)
{
	num::inpclamp(iValue, TNumTraits::zero, TNumTraits::one);
	return ColorRGBA(iValue, TNumTraits::one - iValue, TNumTraits::one);
}



/** convert a value in range [0, 1] to a color like in colormap 'copper' of matlab.
 */
const ColorRGBA ColorRGBA::mapCopper(TParam iValue)
{
	num::inpclamp(iValue, TNumTraits::zero, TNumTraits::one);
	return ColorRGBA(iValue < .8 ? iValue / .8 : 1., .8 * iValue, .5 * iValue);
}



/** convert a value in range [0, 1] to a color like in colormap 'gray' of matlab.
 */
const ColorRGBA ColorRGBA::mapGray(TParam iValue)
{
	num::inpclamp(iValue, TNumTraits::zero, TNumTraits::one);
	return ColorRGBA(iValue);
}



/** convert a value in range [0, 1] to a color like in colormap 'hot' of matlab.
 */
const ColorRGBA ColorRGBA::mapHot(TParam iValue)
{
	const static ColorRGBA keys[] =
	{
		ColorRGBA(0.     , 0.     , 0.),
		ColorRGBA(1. / 3., 0.     , 0.),
		ColorRGBA(2. / 3., 0.     , 0.),
		ColorRGBA(1.     , 0.     , 0.),
		ColorRGBA(1.     , 1. / 3., 0.),
		ColorRGBA(1.     , 2. / 3., 0.),
		ColorRGBA(1.     , 1.     , 0.),
		ColorRGBA(1.     , 1.     , .5),
		ColorRGBA(1.     , 1.     , 1.)
	};
	return doMap(iValue, keys, 9);
}



/** convert a value to a color like in colormap 'hsv' of matlab.
 */
const ColorRGBA ColorRGBA::mapHsv(TParam iValue)
{
	const static ColorRGBA keys[] =
	{
		ColorRGBA(1., 0., 0.),
		ColorRGBA(1., 1., 0.),
		ColorRGBA(0., 1., 0.),
		ColorRGBA(0., 1., 1.),
		ColorRGBA(0., 0., 1.),
		ColorRGBA(1., 0., 1.),
		ColorRGBA(1., 0., 0.)
	};
	return doMap(iValue - num::floor(iValue), keys, 7);
}



/** convert a value in range [0, 1] to a color like in colormap 'jet' of matlab.
 */
const ColorRGBA ColorRGBA::mapJet(TParam iValue)
{
	const static ColorRGBA keys[] =
	{
		ColorRGBA(0., 0., .5), // 240
		ColorRGBA(0., 0., 1.), // 240
		ColorRGBA(0., .5, 1.), // 210
		ColorRGBA(0., 1., 1.), // 180
		ColorRGBA(.5, 1., .5), // 90
		ColorRGBA(1., 1., 0.), // 60
		ColorRGBA(1., .5, 0.), // 30
		ColorRGBA(1., .0, 0.), // 0
		ColorRGBA(.5, .0, 0.)  // 0
	};

	return doMap(iValue, keys, 9);
}



/** convert a value in range [0, 1] to a color like in colormap 'pink' of matlab.
 */
const ColorRGBA ColorRGBA::mapPink(TParam iValue)
{
	const static ColorRGBA keys[] =
	{
		ColorRGBA(0.    , 0.    , 0.    ),
		ColorRGBA(0.2955, 0.1782, 0.1782),
		ColorRGBA(0.4303, 0.2722, 0.2722),
		ColorRGBA(0.5320, 0.3412, 0.3412),
		ColorRGBA(0.6172, 0.3984, 0.3984),
		ColorRGBA(0.6920, 0.4484, 0.4484),
		ColorRGBA(0.7594, 0.4933, 0.4933),
		ColorRGBA(0.7868, 0.5842, 0.5345),
		ColorRGBA(0.8133, 0.6627, 0.5727),
		ColorRGBA(0.8389, 0.7328, 0.6086),
		ColorRGBA(0.8637, 0.7968, 0.6424),
		ColorRGBA(0.8879, 0.8560, 0.6746),
		ColorRGBA(0.9114, 0.9114, 0.7052),
		ColorRGBA(0.9344, 0.9344, 0.7893),
		ColorRGBA(0.9567, 0.9567, 0.8653),
		ColorRGBA(0.9786, 0.9786, 0.9351),
		ColorRGBA(1.    , 1.    , 1.    ),
	};
	return doMap(iValue, keys, 17);
}



/** convert a value in range [0, 1] to a color like in colormap 'spring' of matlab.
 */
const ColorRGBA ColorRGBA::mapSpring(TParam iValue)
{
	num::inpclamp(iValue, TNumTraits::zero, TNumTraits::one);
	return ColorRGBA(TNumTraits::one, iValue, TNumTraits::one - iValue);
}



/** convert a value in range [0, 1] to a color like in colormap 'summer' of matlab.
 */
const ColorRGBA ColorRGBA::mapSummer(TParam iValue)
{
	num::inpclamp(iValue, TNumTraits::zero, TNumTraits::one);
	return ColorRGBA(iValue, (TNumTraits::one + iValue) / 2, 0.4 * TNumTraits::one);
}



/** convert a value in range [0, 1] to a color like in colormap 'winter' of matlab.
 */
const ColorRGBA ColorRGBA::mapWinter(TParam iValue)
{
	num::inpclamp(iValue, TNumTraits::zero, TNumTraits::one);
	return ColorRGBA(0, TNumTraits::one - iValue, TNumTraits::one / 2);
}



/** convert a value in range [0, 1] to a color from a custom color map.
 */
const ColorRGBA ColorRGBA::mapCustom(TParam iValue, const std::vector<ColorRGBA>& iColorMap)
{
	LASS_ASSERT(static_cast<int>(iColorMap.size()) >= 0);
	return doMap(iValue, &iColorMap[0], static_cast<int>(iColorMap.size()));
}



// --- protected -----------------------------------------------------------------------------------



// --- private -------------------------------------------------------------------------------------

const ColorRGBA ColorRGBA::doMap(TParam iValue, const ColorRGBA* iMap, int iMapSize)
{
	LASS_ASSERT(iMapSize > 1);

	num::inpclamp(iValue, TNumTraits::zero, TNumTraits::one);
	const TValue x = iValue * (iMapSize - 1);
	const TValue x0 = num::floor(x);

	const int i = static_cast<int>(x0);
	LASS_ASSERT(i >= 0 && i < iMapSize);

	if (i == iMapSize - 1)
	{
		return iMap[i];
	}

	const TValue dx = x - x0;
	return iMap[i] * (TNumTraits::one - dx) + iMap[i + 1] * dx;
}

// --- free ----------------------------------------------------------------------------------------

/** raw addition of @a iA and @a iB, including alpha channels
 *  @relates lass::prim::ColorRGBA
 */
ColorRGBA operator+(const ColorRGBA& iA, const ColorRGBA& iB)
{
	ColorRGBA result(iA);
	result += iB;
	return result;
}



/** raw subtraction of @a iA and @a iB, including alpha channels
 *  @relates lass::prim::ColorRGBA
 */
ColorRGBA operator-(const ColorRGBA& iA, const ColorRGBA& iB)
{
	ColorRGBA result(iA);
	result -= iB;
	return result;
}



/** raw addition of @a iA and @a iB, including alpha channels
 *  @relates lass::prim::ColorRGBA
 */
ColorRGBA operator*(const ColorRGBA& iA, const ColorRGBA& iB)
{
	ColorRGBA result(iA);
	result *= iB;
	return result;
}



/** raw addition of @a iA and @a iB, including alpha channels
 *  @relates lass::prim::ColorRGBA
 */
ColorRGBA operator/(const ColorRGBA& iA, const ColorRGBA& iB)
{
	ColorRGBA result(iA);
	result /= iB;
	return result;
}



/** raw addition of @a iA and @a iB, including alpha channels
 *  @relates lass::prim::ColorRGBA
 */
ColorRGBA operator+(ColorRGBA::TParam iA, const ColorRGBA& iB)
{
	ColorRGBA result(iB);
	result += iA;
	return result;
}



/** raw subtraction of @a iA and @a iB, including alpha channels
 *  @relates lass::prim::ColorRGBA
 */
ColorRGBA operator-(ColorRGBA::TParam iA, const ColorRGBA& iB)
{
	ColorRGBA result(iA);
	result -= iB;
	return result;
}



/** raw addition of @a iA and @a iB, including alpha channels
 *  @relates lass::prim::ColorRGBA
 */
ColorRGBA operator*(ColorRGBA::TParam iA, const ColorRGBA& iB)
{
	ColorRGBA result(iB);
	result *= iA;
	return result;
}



/** raw addition of @a iA and @a iB, including alpha channels
 *  @relates lass::prim::ColorRGBA
 */
ColorRGBA operator/(ColorRGBA::TParam iA, const ColorRGBA& iB)
{
	ColorRGBA result(iA);
	result /= iB;
	return result;
}



/** raw addition of @a iA and @a iB, including alpha channels
 *  @relates lass::prim::ColorRGBA
 */
ColorRGBA operator+(const ColorRGBA& iA, ColorRGBA::TParam iB)
{
	ColorRGBA result(iA);
	result += iB;
	return result;
}



/** raw subtraction of @a iA and @a iB, including alpha channels
 *  @relates lass::prim::ColorRGBA
 */
ColorRGBA operator-(const ColorRGBA& iA, ColorRGBA::TParam iB)
{
	ColorRGBA result(iA);
	result -= iB;
	return result;
}



/** raw addition of @a iA and @a iB, including alpha channels
 *  @relates lass::prim::ColorRGBA
 */
ColorRGBA operator*(const ColorRGBA& iA, ColorRGBA::TParam iB)
{
	ColorRGBA result(iA);
	result *= iB;
	return result;
}



/** raw addition of @a iA and @a iB, including alpha channels
 *  @relates lass::prim::ColorRGBA
 */
ColorRGBA operator/(const ColorRGBA& iA, ColorRGBA::TParam iB)
{
	ColorRGBA result(iA);
	result /= iB;
	return result;
}



/** placement of foreground @a iA in front of background @a iB.
 *
 *  @a iA is painted over @a iB and leaves only a that part of @a iB visible that isn't painted 
 *  over: 1 - alphaA.
 *
 *  @pre @a iA and @a iB are considered non-premultiplied
 *
 *  @code
 *  alphaR = alphaA + (1 - alphaA) * alphaB.
 *  ColorR * alphaR = ColorA * alphaA + ColorB * alphaB * (1 - alphaA).
 *  @endcode
 *
 *  @arg T. Porter, T. Duff. Compositing Digital Images, Comp. Graphics, 18(3):253-259, July 1984.
 *  @arg http://en.wikipedia.org/wiki/Alpha_channel
 */
ColorRGBA over(const ColorRGBA& iA, const ColorRGBA& iB)
{
	const ColorRGBA::TValue unfilteredB = (ColorRGBA::TNumTraits::one - iA.a) * iB.a;
	const ColorRGBA::TValue alphaR = iA.a + unfilteredB;
	ColorRGBA result(iB);
	result *= unfilteredB;
	result *= iA * iA.a;
	result /= alphaR;
	result.a = alphaR;
	return result;
}



/** part of @a iA inside @a iB.
 *
 *  @a iA is only painted where @a iB is present, and @a iB is not painted at all.
 *  This is the equivalent of only drawing @iA clipped by the alpha channel of @a iB
 *
 *  @pre @a iA and @a iB are considered non-premultiplied
 *
 *  @code
 *  alphaR = alphaA * alphaB.
 *  ColorR = ColorA.
 *  @endcode
 *
 *  @arg T. Porter, T. Duff. Compositing Digital Images, Comp. Graphics, 18(3):253-259, July 1984.
 *  @arg http://en.wikipedia.org/wiki/Alpha_channel
 */
ColorRGBA in(const ColorRGBA& iA, const ColorRGBA& iB)
{
	ColorRGBA result(iA);
	result.a *= iB.a;
	return result;
}



/** @a iA held out by @a iB, part of @a iA outside @a iB.
 *
 *  @a iA is only painted where @a iB is not present, and @a iB is not painted at all.
 *  This is the equivalent of only drawing @iA clipped by the inverse alpha channel of @a iB.
 *
 *  @pre @a iA and @a iB are considered non-premultiplied
 *
 *  @code
 *  alphaR = alphaA * (1 - alphaB).
 *  ColorR = ColorA.
 *  @endcode
 *
 *  @arg T. Porter, T. Duff. Compositing Digital Images, Comp. Graphics, 18(3):253-259, July 1984.
 *  @arg http://en.wikipedia.org/wiki/Alpha_channel
 */
ColorRGBA out(const ColorRGBA& iA, const ColorRGBA& iB)
{
	ColorRGBA result(iA);
	result.a *= (ColorRGBA::TNumTraits::one - iB.a);
	return result;
}



/** union of @a iA in @a iB and @a iB out @a iA.
 *
 *  @a iA atop @a iB includes @a iA where it's on top of @a iB, otherwise it's @a iB.  But nothing 
 *  outside @a iB is included.
 *
 *  @pre @a iA and @a iB are considered non-premultiplied
 *
 *  @code
 *  alphaR = alphaB.
 *  ColorR = ColorA * alphaA + ColorB * (1 - alphaA).
 *  @endcode
 *
 *  @arg T. Porter, T. Duff. Compositing Digital Images, Comp. Graphics, 18(3):253-259, July 1984.
 *  @arg http://en.wikipedia.org/wiki/Alpha_channel
 */
ColorRGBA atop(const ColorRGBA& iA, const ColorRGBA& iB)
{
	ColorRGBA result(iA);
	result *= iA.a;
	result += iB * (ColorRGBA::TNumTraits::one - iA.a);
	result.a = iB.a;
	return result;
}



/** @pre @a iA and @a iB are considered non-premultiplied
 *
 *  @code
 *  alphaR = alphaA + alphaB
 *  colorR * alphaR = colorA * alphaA + colorB * alphaB.
 *  @endcode
 *
 *  @arg T. Porter, T. Duff. Compositing Digital Images, Comp. Graphics, 18(3):253-259, July 1984.
 *  @arg http://en.wikipedia.org/wiki/Alpha_channel
 */
ColorRGBA plus(const ColorRGBA& iA, const ColorRGBA& iB)
{
	ColorRGBA result(iA);
	result *= iA;
	result += iB * iB.a;
	result /= iA.a + iB.a;
	result.a = iA.a + iB.a;
	return result;
}



/** @a iA seen through color filter @a iB.
 *
 *  @pre @a iA and @a iB are considered non-premultiplied
 *
 *  @code
 *  alphaR = alphaA.
 *  colorR = colorA * (1 - alphaB) + colorA * colorB * alphaB.
 *  @endcode
 */
ColorRGBA through(const ColorRGBA& iA, const ColorRGBA& iB)
{
	ColorRGBA result(iA);
	result *= iB;
	result *= iB.a;
	result += iA * (ColorRGBA::TNumTraits::one - iA.b);
	result.a = iA.a;
	return result;
}



/** distance between non-non-premultiplied colours as 3D points (alpha channel is disregarded).
 *  @return num::sqrt(dR ** 2 + dG ** 2 + dB ** 2)
 */
ColorRGBA::TValue distance(const ColorRGBA& iA, const ColorRGBA& iB)
{
	ColorRGBA delta(iA);
	delta -= iB;
	return num::sqrt(num::sqr(delta.r) + num::sqr(delta.g) + num::sqr(delta.a));
}

}

}

// EOF
