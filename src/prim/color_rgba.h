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
