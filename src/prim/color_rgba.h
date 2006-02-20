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



/** @struct lass::prim::ColorRGBA
 *  @brief an [0, 1] floating point RGB colour with Alpha channel.
 *  @author Bram de Greve [BdG]
 */

#ifndef LASS_GUARDIAN_OF_INCLUSION_PRIM_COLOR_RGBA_H
#define LASS_GUARDIAN_OF_INCLUSION_PRIM_COLOR_RGBA_H

#include "prim_common.h"
#include "vector_4d.h"

namespace lass
{
namespace prim
{

class LASS_DLL ColorRGBA
{
public:

	typedef Vector4D<float> TVector;

	typedef TVector::TValue TValue;
	typedef TVector::TParam TParam;
	typedef TVector::TReference TReference;
	typedef TVector::TConstReference TConstReference;
	typedef TVector::TNumTraits TNumTraits;

	enum { dimension = 4 }; /**< number of dimensions */

	TValue r;
	TValue g;
	TValue b;
	TValue a;

	ColorRGBA();
	ColorRGBA(TParam iRed, TParam iGreen, TParam iBlue,
			  TParam iAlpha = ColorRGBA::TNumTraits::one);
	explicit ColorRGBA(TParam iWhite, TParam iAlpha = ColorRGBA::TNumTraits::one);
	explicit ColorRGBA(const TVector& iVector);
	// HACK Yes, you have to use ColorRGBA::TNumTraits::one instead of TNumTraits::one because the
	// MSVC7.0 doesn't understand the latter [BdG].

	const TVector vector() const { return TVector(r, g, b, a); }
	TConstReference operator[](unsigned iIndex) const { return *(&r + iIndex); }
	TReference operator[](unsigned iIndex) { return *(&r + iIndex); }
	TConstReference at(signed iIndex) const { return *(&r + num::mod(iIndex, static_cast<unsigned>(dimension))); }
	TReference at(signed iIndex) { return *(&r + num::mod(iIndex, static_cast<unsigned>(dimension))); }

	const ColorRGBA& operator+() const;
	const ColorRGBA operator-() const;
	ColorRGBA& operator+=( const ColorRGBA& iOther );
	ColorRGBA& operator-=( const ColorRGBA& iOther );
	ColorRGBA& operator*=( const ColorRGBA& iOther );
	ColorRGBA& operator/=( const ColorRGBA& iOther );
	ColorRGBA& operator+=( TParam iWhite );
	ColorRGBA& operator-=( TParam iWhite );
	ColorRGBA& operator*=( TParam iWhite );
	ColorRGBA& operator/=( TParam iWhite );

	const TValue brightness() const;
	const bool isBlack() const;
	const bool isZero() const;
	const bool isNaN() const;

	const ColorRGBA darkened( TParam iFactor ) const;
	const ColorRGBA dissolved( TParam iFactor) const;
	const ColorRGBA gammaCorrected(TParam iGamma) const;
	const ColorRGBA exposed(TParam iTime) const;
	const ColorRGBA invExposed(TParam iTime) const;
	const ColorRGBA clamped() const;

	// matlab colormaps
	//
	static const ColorRGBA mapAutumn(TParam iValue);
	static const ColorRGBA mapBone(TParam iValue);
	static const ColorRGBA mapCool(TParam iValue);
	static const ColorRGBA mapCopper(TParam iValue);
	static const ColorRGBA mapGray(TParam iValue);
	static const ColorRGBA mapHot(TParam iValue);
	static const ColorRGBA mapHsv(TParam iValue);
	static const ColorRGBA mapJet(TParam iValue);
	static const ColorRGBA mapPink(TParam iValue);
	static const ColorRGBA mapSpring(TParam iValue);
	static const ColorRGBA mapSummer(TParam iValue);
	static const ColorRGBA mapWinter(TParam iValue);
	static const ColorRGBA mapCustom(TParam iValue, const std::vector<ColorRGBA>& iColorMap);
private:

	static const ColorRGBA doMap(TParam iValue, const ColorRGBA* iMap, int iMapSize);
};

LASS_DLL ColorRGBA LASS_CALL operator+( const ColorRGBA& iA, const ColorRGBA& iB );
LASS_DLL ColorRGBA LASS_CALL operator-( const ColorRGBA& iA, const ColorRGBA& iB );
LASS_DLL ColorRGBA LASS_CALL operator*( const ColorRGBA& iA, const ColorRGBA& iB );
LASS_DLL ColorRGBA LASS_CALL operator/( const ColorRGBA& iA, const ColorRGBA& iB );
LASS_DLL ColorRGBA LASS_CALL operator+( ColorRGBA::TParam iA, const ColorRGBA& iB );
LASS_DLL ColorRGBA LASS_CALL operator-( ColorRGBA::TParam iA, const ColorRGBA& iB );
LASS_DLL ColorRGBA LASS_CALL operator*( ColorRGBA::TParam iA, const ColorRGBA& iB );
LASS_DLL ColorRGBA LASS_CALL operator/( ColorRGBA::TParam iA, const ColorRGBA& iB );
LASS_DLL ColorRGBA LASS_CALL operator+( const ColorRGBA& iA, ColorRGBA::TParam iB );
LASS_DLL ColorRGBA LASS_CALL operator-( const ColorRGBA& iA, ColorRGBA::TParam iB );
LASS_DLL ColorRGBA LASS_CALL operator*( const ColorRGBA& iA, ColorRGBA::TParam iB );
LASS_DLL ColorRGBA LASS_CALL operator/( const ColorRGBA& iA, ColorRGBA::TParam iB );

LASS_DLL ColorRGBA LASS_CALL over( const ColorRGBA& iA, const ColorRGBA& iB );
LASS_DLL ColorRGBA LASS_CALL in( const ColorRGBA& iA, const ColorRGBA& iB );
LASS_DLL ColorRGBA LASS_CALL out( const ColorRGBA& iA, const ColorRGBA& iB );
LASS_DLL ColorRGBA LASS_CALL atop( const ColorRGBA& iA, const ColorRGBA& iB );
LASS_DLL ColorRGBA LASS_CALL plus( const ColorRGBA& iA, const ColorRGBA& iB );
LASS_DLL ColorRGBA LASS_CALL through( const ColorRGBA& iA, const ColorRGBA& iB );

LASS_DLL ColorRGBA::TValue LASS_CALL distance( const ColorRGBA& iA, const ColorRGBA& iB );

}

}

#include "pyobject_util.inl"

#endif

// EOF
