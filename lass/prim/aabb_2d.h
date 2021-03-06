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
 *	Copyright (C) 2004-2011 the Initial Developer.
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



/** @class lass::prim::Aabb2D
 *  @brief your momma's axis aligned bounding box.
 *  @author Bram de Greve [BdG]
 *  @date 2003
 *
 *  An AABB (Axis Aligned Bounding Box) is a rectangular box of a given dimension
 *  (in this case a 2D rectangle), that is often used as a simple bounding volume of another
 *  primitive or data structure.
 *
 *  <i>"A form of a bounding box where the box is aligned to the axis therefore only
 *  two points in space are needed to define it. AABB's are much faster to use, and
 *  take up less memory, but are very limited in the sense that they can only be
 *  aligned to the axis."</i>, http://www.gamedev.net/dict/term.asp?TermID=525
 *
 *  The way an AABB handles its minima and maxima can be set by the @c MinMaxPolicy.
 *  On policy StrictPolicy will enforce you to use correct minima and maxima, and on
 *  any suspicious behaviour, it will throw an exception.  The other policy AutoPolicy
 *  will try to correct misbehaviour without your notice.  For more information on
 *  these policies, I refer to the documentation compagning these policies.
 */



#ifndef LASS_GUARDIAN_OF_INCLUSION_PRIM_AABB_2D_H
#define LASS_GUARDIAN_OF_INCLUSION_PRIM_AABB_2D_H



#include "min_max_policy.h"
#include "point_2dh.h"
#include "../io/matlab_o_stream.h"

namespace lass
{

namespace prim
{

template
<
	typename T,
	class MinMaxPolicy = StrictMinMax
>
class LASS_SIMD_ALIGN Aabb2D
{
public:

	typedef Aabb2D<T, MinMaxPolicy> TSelf;
	typedef MinMaxPolicy TMinMaxPolicy;

	typedef Point2D<T> TPoint;
	typedef Point2DH<T> TPointH;
	typedef typename TPoint::TVector TVector;

	typedef typename TPoint::TValue TValue;
	typedef typename TPoint::TParam TParam;
	typedef typename TPoint::TReference TReference;
	typedef typename TPoint::TConstReference TConstReference;
	typedef typename TPoint::TNumTraits TNumTraits;

	enum { dimension = TPoint::dimension }; /**< number of dimensions of vector */

	template <typename U> struct Rebind
	{
		typedef Aabb2D<U, MinMaxPolicy> Type;
	};

	Aabb2D();
	Aabb2D(const TPoint& min, const TPoint& max);
	explicit Aabb2D(const TPoint& point);
	template <class MMP2> Aabb2D(const Aabb2D<T, MMP2>& other);

	const TPoint& min() const;
	const TPoint& max() const;
	void setMin(const TPoint& min);
	void setMax(const TPoint& max);

	template <class MMP2> TSelf& operator=(const Aabb2D<T, MMP2>& other);

	TSelf& operator+=(const TPoint& point);
	template<class MMP2> TSelf& operator+=(const Aabb2D<T, MMP2>& other);
	void grow(TParam iDistance);
	void scale(TParam iScale);

	const TPointH center() const;
	const TVector size() const;
	const TValue perimeter() const;
	const TValue area() const;

	Side classify(const TPoint& point) const;
	bool contains(const TPoint& point) const;
	template <class MMP2> bool contains(const Aabb2D<T, MMP2>& other) const;
	template <class MMP2> bool intersects(const Aabb2D<T, MMP2>& other) const;
	template <class MMP2> bool collides(const Aabb2D<T, MMP2>& other) const;

	template <class RandomGenerator> const TPoint random(RandomGenerator& random) const;

	void clear();
	bool isEmpty() const;
	bool isValid() const;

	template <typename MMP2> void swap(Aabb2D<T, MMP2>& other);

private:

	TPoint min_;
	TPoint max_;
};

template <typename T, class MMPa, class MMPb>
const Aabb2D<T, MMPa> operator+(const Aabb2D<T, MMPa>& a, const Aabb2D<T, MMPb>& b);

template <typename T, class MMP>
const Aabb2D<T, MMP> operator+(const Aabb2D<T, MMP>& a, const Point2D<T>& b);

template <typename T, class MMP>
const Aabb2D<T, MMP> operator+(const Point2D<T>& a, const Aabb2D<T, MMP>& b);

template <typename T>
const Aabb2D<T> aabb(const Point2D<T>& point);

template <typename T, class MMP>
T distance(const Aabb2D<T, MMP>& a, const Point2D<T>& b);

template <typename T, class MMPa, class MMPb>
T distance(const Aabb2D<T, MMPa>& a, const Aabb2D<T, MMPb>& b);

template <typename T, class MMPa, class MMPb, class MMPr>
Result intersect(const Aabb2D<T, MMPa>& a, const Aabb2D<T, MMPb>& b, Aabb2D<T, MMPr>& result);

template <typename T, class MMPa, class MMPb>
bool intersects(const Aabb2D<T, MMPa>& a, const Aabb2D<T, MMPb>& b);

template <typename T, class MMP>
bool intersects(const Aabb2D<T, MMP>& a, const Point2D<T>& b);

template <typename T, class MMP>
bool intersects(const Point2D<T>& a, const Aabb2D<T, MMP>& b);

template <typename T, class MMPa, class MMPb>
bool collides(const Aabb2D<T, MMPa>& a, const Aabb2D<T, MMPb>& b);

template <typename T, class MMP>
bool collides(const Aabb2D<T, MMP>& a, const Point2D<T>& b);

template <typename T, class MMP>
bool collides(const Point2D<T>& a, const Aabb2D<T, MMP>& b);

template <typename T, class MMP>
std::ostream& operator<<(std::ostream& ioOStream, const Aabb2D<T, MMP>& aabb);

template <typename T, class MMP>
io::XmlOStream& operator<<(io::XmlOStream& ioOStream, const Aabb2D<T, MMP>& aabb);

template <typename T, class MMP>
io::MatlabOStream& operator<<(io::MatlabOStream& ioOStream, const Aabb2D<T, MMP>& aabb);

}

}

#include "aabb_2d.inl"

#define LASS_PRIM_HAVE_PY_EXPORT_TRAITS_AABB_2D
#ifdef LASS_GUARDIAN_OF_INCLUSION_UTIL_PYOBJECT_PLUS_H
#	include "../python/export_traits_prim.h"
#endif

#ifdef LASS_GUARDIAN_OF_INCLUSION_PRIM_RAY_2D_H
#	include "aabb_2d_ray_2d.h"
#endif

#ifdef LASS_GUARDIAN_OF_INCLUSION_PRIM_SIMPLE_POLYGON_2D_H
#	include "aabb_2d_simple_polygon_2d.h"
#endif

#ifdef LASS_GUARDIAN_OF_INCLUSION_PRIM_TRIANGLE_2D_H
#	include "aabb_2d_triangle_2d.h"
#endif

#ifdef LASS_GUARDIAN_OF_INCLUSION_PRIM_TRANSFORMATION_2D_H
#	include "aabb_2d_transformation_2d.h"
#endif

#endif
