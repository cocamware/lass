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



/** @struct lass::prim::Point_2d
 *  @brief 2D Point
 *  @author BdG
 *  @date 2003
 */



#ifndef LASS_GUARDIAN_OF_INCLUSION_PRIM_POINT_2D_H
#define LASS_GUARDIAN_OF_INCLUSION_PRIM_POINT_2D_H




#include "prim_common.h"
#include "vector_2d.h"
#include "../io/matlab_o_stream.h"

namespace lass
{
namespace prim
{

template<typename T>
struct Point2D
{
public:

	typedef Point2D<T> TSelf;

	typedef Vector2D<T> TVector;

	typedef typename TVector::TValue TValue;
	typedef typename TVector::TParam TParam;
	typedef typename TVector::TReference TReference;
	typedef typename TVector::TConstReference TConstReference;
	typedef typename TVector::TNumTraits TNumTraits;

	enum { dimension = TVector::dimension };    /**< number of dimensions of vector */

	template <typename U> struct Rebind
	{
		typedef Point2D<U> Type;
	};

	TValue x;
	TValue y;

	Point2D();
	Point2D(TParam x, TParam y);
	template <typename U> explicit Point2D(const Point2D<U>& other);
	template <typename U> explicit Point2D(const Vector2D<U>& position);
	template <typename U> explicit Point2D(const U& x, const U& y);

	const TVector position() const;
	TConstReference operator[](size_t index) const;
	TReference operator[](size_t index);
	TConstReference at(signed index) const;
	TReference at(signed index);

	Point2D<T>& operator+=(const Vector2D<T>& offset);
	Point2D<T>& operator-=(const Vector2D<T>& offset);

	const bool isZero() const;
	const bool isNaN() const;
};

template<typename T> bool operator==(const Point2D<T>& a, const Point2D<T>& b);
template<typename T> bool operator!=(const Point2D<T>& a, const Point2D<T>& b);

template<typename T> Point2D<T> operator+(const Point2D<T>& a, const Vector2D<T>& b);
template<typename T> Point2D<T> operator+(const Vector2D<T>& a, const Point2D<T>& b);
template<typename T> Point2D<T> operator-(const Point2D<T>& a, const Vector2D<T>& b);
template<typename T> Vector2D<T> operator-(const Point2D<T>& a, const Point2D<T>& b);

template<typename T> typename Point2D<T>::TValue distance(const Point2D<T>& a, const Point2D<T>& b);
template<typename T> typename Point2D<T>::TValue squaredDistance(const Point2D<T>& a, const Point2D<T>& b);
template<typename T> Point2D<T> pointwiseMin(const Point2D<T>& a, const Point2D<T>& b);
template<typename T> Point2D<T> pointwiseMax(const Point2D<T>& a, const Point2D<T>& b);
template<typename T> Point2D<T> lerp(const Point2D<T>& a, const Point2D<T>& b, typename Point2D<T>::TParam t);

template<typename T> std::ostream& operator<<(std::ostream& stream, const Point2D<T>& b);
template<typename T> io::XmlOStream& operator<<(io::XmlOStream& stream, const Point2D<T>& b);
template<typename T> std::istream& operator>>(std::istream& stream, Point2D<T>& b);
template<typename T> lass::io::MatlabOStream& operator<<(lass::io::MatlabOStream& stream, const Point2D<T>& b);


template<typename T> T doubleTriangleArea( const Point2D<T>& a, const Point2D<T>& b, const Point2D<T>& c );
template<typename T> bool cw( const Point2D<T>& a, const Point2D<T>& b, const Point2D<T>& c );
template<typename T> bool ccw( const Point2D<T>& a, const Point2D<T>& b, const Point2D<T>& c );
template<typename T> bool weakCw( const Point2D<T>& a, const Point2D<T>& b, const Point2D<T>& c );
template<typename T> bool weakCcw( const Point2D<T>& a, const Point2D<T>& b, const Point2D<T>& c );
template<typename T> bool inCircle( const Point2D<T>& a, const Point2D<T>& b, const Point2D<T>& c, const Point2D<T>& d );

}

}

#include "point_2d.inl"

#define LASS_PRIM_HAVE_PY_EXPORT_TRAITS_POINT_2D
#ifdef LASS_GUARDIAN_OF_INCLUSION_UTIL_PYOBJECT_PLUS_H
#	include "../python/export_traits_prim.h"
#endif

#endif
