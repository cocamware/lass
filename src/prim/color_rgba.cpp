/**	@file
 *	@author Bram de Greve (bramz@users.sourceforge.net)
 *	@author Tom De Muer (tomdemuer@users.sourceforge.net)
 *
 *	Distributed under the terms of the GPL (GNU Public License)
 *
 * 	The LASS License:
 *
 *	Copyright 2004 Bram de Greve and Tom De Muer
 *
 *	LASS is free software; you can redistribute it and/or modify
 *	it under the terms of the GNU General Public License as published by
 *	the Free Software Foundation; either version 2 of the License, or
 *	(at your option) any later version.
 *
 *	This program is distributed in the hope that it will be useful,
 *	but WITHOUT ANY WARRANTY; without even the implied warranty of
 *	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *	GNU General Public License for more details.
 *
 *	You should have received a copy of the GNU General Public License
 *	along with this program; if not, write to the Free Software
 *	Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */


//
//  Color Addition: R = A under B.
//  alphaR = 1 - (1 - alphaA) * (1 - alphaB) = alphaA * (1 - alphaB) + alphaB.
//  alphaR * colorR = alphaA * (1 - alphaB) * colorA + alphaB * colorB.
//
//  Color Multiplication: R = A through B.
//  alphaR = alphaA.
//  alphaR * colorR = alphaA * (1 - alphaB) * colorA + alphaA * alphaB * colorA * colorB.
//

#include "prim_common.h"
#include "color_rgba.h"
#include "vector_3d.h"

// for this translation unit, we don't want any warnings on numerical conversions.
#pragma warning(disable: 4305 4244)

namespace lass
{
namespace prim
{



// --- public --------------------------------------------------------------------------------------

/** construct an unexisting color (black with zero alpha)
 */
ColorRGBA::ColorRGBA():
	vector_()
{
	LASS_ASSERT(vector_.isZero());
}



/** construct a color in white range (r == g == b),  with an alpha value
 */
ColorRGBA::ColorRGBA(TParam iWhite, TParam iAlpha):
	vector_(iWhite, iWhite, iWhite, iAlpha)
{
}



/** construct a color with values for all channels
 */
ColorRGBA::ColorRGBA(TParam iRed, TParam iGreen, TParam iBlue, TParam iAlpha):
	vector_(iRed, iGreen, iBlue, iAlpha)
{
}



/** color addition means "this under iOther"
 */
ColorRGBA& ColorRGBA::operator+=(const ColorRGBA& iOther)
{
	const TValue unfiltered = a() * (TNumTraits::one - iOther.a());
	const TValue aResult = unfiltered + iOther.a();
	const TValue aResultInverse = TNumTraits::one / aResult;

	vector_ *= unfiltered;
	vector_ += iOther.a() * iOther.vector();
	vector_.w = aResult;

	return *this;
}



const ColorRGBA::TValue ColorRGBA::brightness() const
{
	return (r() + g() + b()) / 3;
}



/** color mulitplication means "this through iOther"
 */
ColorRGBA& ColorRGBA::operator*=(const ColorRGBA& iOther)
{
	const TValue unfiltered = TNumTraits::one - iOther.a();
	vector_.x *= (unfiltered + iOther.a() * iOther.r());
	vector_.y *= (unfiltered + iOther.a() * iOther.g());
	vector_.w *= (unfiltered + iOther.a() * iOther.b());
	return *this;
}



/** gamma correction.
 *  corrects gamma of all channels, except alpha.
 *  out = in ^ (1 / iGamma).
 */
void ColorRGBA::gamma(TParam iGamma)
{
	const TValue invGamma = TNumTraits::one / iGamma;
	vector_.x = num::pow(vector_.x, invGamma);
	vector_.y = num::pow(vector_.y, invGamma);
	vector_.w = num::pow(vector_.w, invGamma);
}



/** clamp all channels (including alpha channel) to the range [0, 1].
 *  @return the difference on the alpha channel (original alhpa - new alpha) and can be used
 *  as an indicator for the bleeding of the color.
 *  @post: all channel values ar in the range [0, 1].
 */
const ColorRGBA::TValue ColorRGBA::clamp()
{
	const TValue originalAlpha = vector_.w;

	vector_.x = num::clamp(vector_.x, TNumTraits::zero, TNumTraits::one);
	vector_.y = num::clamp(vector_.y, TNumTraits::zero, TNumTraits::one);
	vector_.z = num::clamp(vector_.z, TNumTraits::zero, TNumTraits::one);
	vector_.w = num::clamp(vector_.w, TNumTraits::zero, TNumTraits::one);

	return originalAlpha - vector_.w;
}



/** simulate the exposion of a light with this color to a chemical film during an exposure time.
 *  @return the difference on the alpha channel (original alhpa - new alpha) and can be used
 *  as an indicator for the bleeding of the color.
 *  @post: all channel values ar in the range [0, 1].
 */
const ColorRGBA::TValue ColorRGBA::expose(TValue iTime)
{
    const TValue originalBrightness = brightness();
    const TValue originalAlpha = vector_.w;

	vector_.x = vector_.x > TNumTraits::zero ? TNumTraits::one - num::exp(-iTime * vector_.x) : TNumTraits::zero;
	vector_.y = vector_.y > TNumTraits::zero ? TNumTraits::one - num::exp(-iTime * vector_.y) : TNumTraits::zero;
	vector_.z = vector_.z > TNumTraits::zero ? TNumTraits::one - num::exp(-iTime * vector_.z) : TNumTraits::zero;

	// color bleeding ...  if the chemical gets overexposes, it's opacity increases ...
	const TValue difference = originalBrightness - brightness();
	if (difference > TNumTraits::zero)
	{
		vector_.w *= (TNumTraits::one + difference);
	}
	vector_.w = num::clamp(vector_.w, TNumTraits::zero, TNumTraits::one);

	return originalAlpha - vector_.w;
}



/** convert a value in range [0, 1] to a color like in colormap 'autumn' of matlab.
 */
const ColorRGBA ColorRGBA::mapAutumn(TValue iValue)
{
	num::inpclamp(iValue, TNumTraits::zero, TNumTraits::one);
	return ColorRGBA(1., iValue, 0.);
}



/** convert a value in range [0, 1] to a color like in colormap 'bone' of matlab.
 */
const ColorRGBA ColorRGBA::mapBone(TValue iValue)
{
	const static ColorRGBA keys[] = 
	{
		ColorRGBA(0.    , 0.    , 0.    ),
		ColorRGBA(0.3194, 0.3194, 0.4444),
		ColorRGBA(0.6528, 0.7778, 0.7778),
		ColorRGBA(1.    , 1.    , 1.    )
	};
	return map(keys, 4, iValue);
}



/** convert a value in range [0, 1] to a color like in colormap 'cool' of matlab.
 */
const ColorRGBA ColorRGBA::mapCool(TValue iValue)
{
	num::inpclamp(iValue, TNumTraits::zero, TNumTraits::one);
	return ColorRGBA(iValue, TNumTraits::one - iValue, TNumTraits::one);
}



/** convert a value in range [0, 1] to a color like in colormap 'copper' of matlab.
 */
const ColorRGBA ColorRGBA::mapCopper(TValue iValue)
{
	num::inpclamp(iValue, TNumTraits::zero, TNumTraits::one);
	return ColorRGBA(iValue < .8 ? iValue / .8 : 1., .8 * iValue, .5 * iValue);
}



/** convert a value in range [0, 1] to a color like in colormap 'gray' of matlab.
 */
const ColorRGBA ColorRGBA::mapGray(TValue iValue)
{
	num::inpclamp(iValue, TNumTraits::zero, TNumTraits::one);
	return ColorRGBA(iValue);
}



/** convert a value in range [0, 1] to a color like in colormap 'hot' of matlab.
 */
const ColorRGBA ColorRGBA::mapHot(TValue iValue)
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
	return map(keys, 9, iValue);
}



/** convert a value to a color like in colormap 'hsv' of matlab.
 */
const ColorRGBA ColorRGBA::mapHsv(TValue iValue)
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
	return map(keys, 7, iValue - num::floor(iValue));
}



/** convert a value in range [0, 1] to a color like in colormap 'jet' of matlab.
 */
const ColorRGBA ColorRGBA::mapJet(TValue iValue)
{
	const static ColorRGBA keys[] = 
	{
		ColorRGBA(0., 0., .5),
		ColorRGBA(0., 0., 1.),
		ColorRGBA(0., .5, 1.),
		ColorRGBA(0., 1., 1.),
		ColorRGBA(.5, 1., .5),
		ColorRGBA(1., 1., 0.),
		ColorRGBA(1., .5, 0.),
		ColorRGBA(1., .0, 0.),
		ColorRGBA(.5, .0, 0.)
	};

	return map(keys, 9, iValue);
}



/** convert a value in range [0, 1] to a color like in colormap 'pink' of matlab.
 */
const ColorRGBA ColorRGBA::mapPink(TValue iValue)
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
	return map(keys, 17, iValue);
}



/** convert a value in range [0, 1] to a color like in colormap 'spring' of matlab.
 */
const ColorRGBA ColorRGBA::mapSpring(TValue iValue)
{
	num::inpclamp(iValue, TNumTraits::zero, TNumTraits::one);
	return ColorRGBA(TNumTraits::one, iValue, TNumTraits::one - iValue);
}



/** convert a value in range [0, 1] to a color like in colormap 'summer' of matlab.
 */
const ColorRGBA ColorRGBA::mapSummer(TValue iValue)
{
	num::inpclamp(iValue, TNumTraits::zero, TNumTraits::one);
	return ColorRGBA(iValue, (TNumTraits::one + iValue) / 2, 0.4 * TNumTraits::one);
}



/** convert a value in range [0, 1] to a color like in colormap 'winter' of matlab.
 */
const ColorRGBA ColorRGBA::mapWinter(TValue iValue)
{
	num::inpclamp(iValue, TNumTraits::zero, TNumTraits::one);
	return ColorRGBA(0, TNumTraits::one - iValue, TNumTraits::one / 2);
}


// --- protected -----------------------------------------------------------------------------------



// --- private -------------------------------------------------------------------------------------

const ColorRGBA ColorRGBA::map(const ColorRGBA* iMap, int iMapSize, TValue iValue)
{
	LASS_ASSERT(iMapSize > 1);

	num::inpclamp(iValue, TNumTraits::zero, TNumTraits::one);
	const TValue x = iValue * (iMapSize - 1);
	const TValue x0 = num::floor(x);
	const TValue dx = x - x0;

	const int i = static_cast<int>(x0);
	LASS_ASSERT(i >= 0 && i < iMapSize);

	if (i == iMapSize - 1)
	{
		return iMap[i];
	}

	ColorRGBA result;
	result.vector_ += iMap[i].vector_ * (TNumTraits::one - dx);
	result.vector_ += iMap[i + 1].vector_ * dx;
	return result;
}

// --- free ----------------------------------------------------------------------------------------

/** color addition means "iA under iB"
 *  @relates lass::prim::ColorRGBA
 */
ColorRGBA operator+(const ColorRGBA& iA, const ColorRGBA& iB)
{
	ColorRGBA result(iA);
	result += iB;
	return result;
}



/** colour multiplication means "iA through iB"
 *  @relates lass::prim::ColorRGBA
 */
ColorRGBA operator*(const ColorRGBA& iA, const ColorRGBA& iB)
{
	ColorRGBA result(iA);
	result *= iB;
	return result;
}



/** distance between colours as 3D points (alpha channel is disregarded).
 *  @return num::sqrt(dr * dr + dg * dg + db * db)
 */
ColorRGBA::TValue distance(const ColorRGBA& iA, const ColorRGBA& iB)
{
	const Vector3D<ColorRGBA::TValue> delta(iA.r() - iB.r(), iA.g() - iB.g(), iA.b() - iB.b());
	return delta.norm();
}

}

}

// EOF
