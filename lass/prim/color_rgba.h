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
	ColorRGBA(TParam red, TParam green, TParam blue,
			  TParam alpha = ColorRGBA::TNumTraits::one);
	explicit ColorRGBA(TParam white, TParam alpha = ColorRGBA::TNumTraits::one);
	explicit ColorRGBA(const TVector& vector);
	// HACK Yes, you have to use ColorRGBA::TNumTraits::one instead of TNumTraits::one because the
	// MSVC7.0 doesn't understand the latter [BdG].

	const TVector vector() const { return TVector(r, g, b, a); }
	TConstReference operator[](size_t index) const { return *(&r + index); }
	TReference operator[](size_t index) { return *(&r + index); }
	TConstReference at(signed index) const { return *(&r + num::mod(index, static_cast<unsigned>(dimension))); }
	TReference at(signed index) { return *(&r + num::mod(index, static_cast<unsigned>(dimension))); }

	const ColorRGBA& operator+() const;
	const ColorRGBA operator-() const;
	ColorRGBA& operator+=( const ColorRGBA& other );
	ColorRGBA& operator-=( const ColorRGBA& other );
	ColorRGBA& operator*=( const ColorRGBA& other );
	ColorRGBA& operator/=( const ColorRGBA& other );
	ColorRGBA& operator+=( TParam white );
	ColorRGBA& operator-=( TParam white );
	ColorRGBA& operator*=( TParam white );
	ColorRGBA& operator/=( TParam white );

	TValue brightness() const;
	bool isBlack() const;
	bool isZero() const;
	bool isNaN() const;

	const ColorRGBA darkened( TParam factor ) const;
	const ColorRGBA dissolved( TParam factor) const;
	const ColorRGBA gammaCorrected(TParam gamma) const;
	const ColorRGBA exposed(TParam time) const;
	const ColorRGBA invExposed(TParam time) const;
	const ColorRGBA clamped() const;

	// matlab colormaps
	//
	static const ColorRGBA mapAutumn(TParam value);
	static const ColorRGBA mapBone(TParam value);
	static const ColorRGBA mapCool(TParam value);
	static const ColorRGBA mapCopper(TParam value);
	static const ColorRGBA mapGray(TParam value);
	static const ColorRGBA mapHot(TParam value);
	static const ColorRGBA mapHsv(TParam value);
	static const ColorRGBA mapJet(TParam value);
	static const ColorRGBA mapPink(TParam value);
	static const ColorRGBA mapSpring(TParam value);
	static const ColorRGBA mapSummer(TParam value);
	static const ColorRGBA mapWinter(TParam value);
	static const ColorRGBA mapCustom(TParam value, const std::vector<ColorRGBA>& colorMap);
private:

	static const ColorRGBA doMap(TParam value, const ColorRGBA* iMap, int iMapSize);
};

LASS_DLL ColorRGBA LASS_CALL operator+( const ColorRGBA& a, const ColorRGBA& b );
LASS_DLL ColorRGBA LASS_CALL operator-( const ColorRGBA& a, const ColorRGBA& b );
LASS_DLL ColorRGBA LASS_CALL operator*( const ColorRGBA& a, const ColorRGBA& b );
LASS_DLL ColorRGBA LASS_CALL operator/( const ColorRGBA& a, const ColorRGBA& b );
LASS_DLL ColorRGBA LASS_CALL operator+( ColorRGBA::TParam a, const ColorRGBA& b );
LASS_DLL ColorRGBA LASS_CALL operator-( ColorRGBA::TParam a, const ColorRGBA& b );
LASS_DLL ColorRGBA LASS_CALL operator*( ColorRGBA::TParam a, const ColorRGBA& b );
LASS_DLL ColorRGBA LASS_CALL operator/( ColorRGBA::TParam a, const ColorRGBA& b );
LASS_DLL ColorRGBA LASS_CALL operator+( const ColorRGBA& a, ColorRGBA::TParam b );
LASS_DLL ColorRGBA LASS_CALL operator-( const ColorRGBA& a, ColorRGBA::TParam b );
LASS_DLL ColorRGBA LASS_CALL operator*( const ColorRGBA& a, ColorRGBA::TParam b );
LASS_DLL ColorRGBA LASS_CALL operator/( const ColorRGBA& a, ColorRGBA::TParam b );

LASS_DLL ColorRGBA LASS_CALL over( const ColorRGBA& a, const ColorRGBA& b );
LASS_DLL ColorRGBA LASS_CALL in( const ColorRGBA& a, const ColorRGBA& b );
LASS_DLL ColorRGBA LASS_CALL out( const ColorRGBA& a, const ColorRGBA& b );
LASS_DLL ColorRGBA LASS_CALL atop( const ColorRGBA& a, const ColorRGBA& b );
LASS_DLL ColorRGBA LASS_CALL plus( const ColorRGBA& a, const ColorRGBA& b );
LASS_DLL ColorRGBA LASS_CALL through( const ColorRGBA& a, const ColorRGBA& b );

LASS_DLL ColorRGBA::TValue LASS_CALL distance( const ColorRGBA& a, const ColorRGBA& b );

}

}

#define LASS_PRIM_HAVE_PY_EXPORT_TRAITS_COLOR_RGBA
#ifdef LASS_GUARDIAN_OF_INCLUSION_UTIL_PYOBJECT_PLUS_H
#	include "../python/export_traits_prim.h"
#endif

#ifdef LASS_GUARDIAN_OF_INCLUSION_PRIM_TRANSFORMATION_3D_H
#	include "color_rgba_transformation_3d.h"
#endif

#endif

// EOF
