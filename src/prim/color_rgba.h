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

class ColorRGBA
{
public:

	typedef Vector4D<float> TVector;

	typedef TVector::TValue TValue;
	typedef TVector::TParam TParam;
	typedef TVector::TReference TReference;
	typedef TVector::TConstReference TConstReference;
	typedef TVector::TNumTraits TNumTraits;

	enum { dimension = 4 };	/**< number of dimensions */

	ColorRGBA();
	ColorRGBA(TParam iWhite, TParam iAlpha = ColorRGBA::TNumTraits::one);
	ColorRGBA(TParam iRed, TParam iGreen, TParam iBlue, 
              TParam iAlpha = ColorRGBA::TNumTraits::one);
	// HACK Yes, you have to use ColorRGBA::TNumTraits::one instead of TNumTraits::one because the 
	// MSVC7.0 doesn't understand the latter [BdG].

	const TVector& vector() const { return vector_; }
	TConstReference operator[](unsigned iIndex) const { return vector_[iIndex]; }
	TConstReference at(signed iIndex) const { return vector_.at(iIndex); }
	TConstReference a() const { return vector_.x; }
	TConstReference r() const { return vector_.y; }
	TConstReference g() const { return vector_.z; }
	TConstReference b() const { return vector_.w; }

	TVector& vector() { return vector_; }
	TReference operator[](unsigned iIndex) { return vector_[iIndex]; }
	TReference at(signed iIndex) { return vector_.at(iIndex); }
	TReference a() { return vector_.x; }
	TReference r() { return vector_.y; }
	TReference g() { return vector_.z; }
	TReference b() { return vector_.w; }

	ColorRGBA& operator+=( const ColorRGBA& iOther );
	ColorRGBA& operator*=( const ColorRGBA& iOther );

	const TValue brightness() const;

	void gamma(TParam iGamma);
	const TValue clamp();
	const TValue expose(TParam iTime);

	// matlab colormaps
	//
	static const ColorRGBA mapAutumn(TValue iValue);
	static const ColorRGBA mapBone(TValue iValue);
	static const ColorRGBA mapCool(TValue iValue);
	static const ColorRGBA mapCopper(TValue iValue);
	static const ColorRGBA mapGray(TValue iValue);
	static const ColorRGBA mapHot(TValue iValue);
	static const ColorRGBA mapHsv(TValue iValue);
	static const ColorRGBA mapJet(TValue iValue);
	static const ColorRGBA mapPink(TValue iValue);
	static const ColorRGBA mapSpring(TValue iValue);
	static const ColorRGBA mapSummer(TValue iValue);
	static const ColorRGBA mapWinter(TValue iValue);

private:

	static const ColorRGBA map(const ColorRGBA* iMap, int iMapSize, TValue iValue);

	TVector vector_;
};

ColorRGBA operator+( const ColorRGBA& iA, const ColorRGBA& iB );
ColorRGBA operator*( const ColorRGBA& iA, const ColorRGBA& iB );

ColorRGBA::TValue distance( const ColorRGBA& iA, const ColorRGBA& iB );

}

}

#define LASS_PRIM_PYOBJECT_UTIL_COLOR_RGBA
#ifdef LASS_GUARDIAN_OF_INCLUSION_UTIL_PYOBJECT_UTIL_H
#	include "pyobject_util.h"
#endif

#endif

// EOF
