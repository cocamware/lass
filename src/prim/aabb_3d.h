/** @file
 *  @author Bram de Greve (bramz@users.sourceforge.net)
 *  @author Tom De Muer (tomdemuer@users.sourceforge.net)
 *
 *  Distributed under the terms of the GPL (GNU Public License)
 *
 *  The LASS License:
 *
 *  Copyright 2004 Bram de Greve and Tom De Muer
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



/** @class lass::prim::Aabb3D
 *  @brief your momma's axis aligned bounding box.
 *  @author Bram de Greve [BdG]
 *  @date 2003
 *
 *  An AABB (Axis Aligned Bounding Box) is a rectangular box of a given dimension
 *  (in this case a 3D box), that is often used as a simple bounding volume of another
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



#ifndef LASS_GUARDIAN_OF_INCLUSION_PRIM_AABB_3D_H
#define LASS_GUARDIAN_OF_INCLUSION_PRIM_AABB_3D_H
#pragma once



#include "min_max_policy.h"
#include "point_3dh.h"


namespace lass
{

namespace prim
{

template
<
	typename T,
	class MinMaxPolicy = StrictMinMax
>
class Aabb3D
{
public:

	typedef Aabb3D<T, MinMaxPolicy> TSelf;
	typedef MinMaxPolicy TMinMaxPolicy;

	typedef Point3D<T> TPoint;
	typedef Point3DH<T> TPointH;
	typedef typename TPoint::TVector TVector;

	typedef typename TPoint::TValue TValue;
	typedef typename TPoint::TParam TParam;
	typedef typename TPoint::TReference TReference;
	typedef typename TPoint::TConstReference TConstReference;
	typedef typename TPoint::TNumTraits TNumTraits;

	enum { dimension = TPoint::dimension }; /**< number of dimensions of vector */

	template <typename U> struct Rebind
	{
		typedef Aabb3D<U, MinMaxPolicy> Type;
	};

	Aabb3D();
	Aabb3D(const TPoint& iMin, const TPoint& iMax);
	explicit Aabb3D(const TPoint& iPoint);
	template <class MMP2> Aabb3D(const Aabb3D<T, MMP2>& iOther);

	const TPoint& min() const;
	const TPoint& max() const;
	void setMin(const TPoint& iMin);
	void setMax(const TPoint& iMax);

	template <class MMP2> TSelf& operator=(const Aabb3D<T, MMP2>& iOther);

	TSelf& operator+=(const TPoint& iPoint);
	template<class MMP2> TSelf& operator+=(const Aabb3D<T, MMP2>& iOther);
	void grow(TParam iDistance);
	void scale(TParam iScale);

	const TPointH center() const;
	const TVector size() const;
	const TValue area() const;
	const TValue volume() const;

	const Side classify(const TPoint& iPoint) const;
	const bool contains(const TPoint& iPoint) const;
	template <class MMP2> const bool contains(const Aabb3D<T, MMP2>& iOther) const;
	template <class MMP2> const bool intersects(const Aabb3D<T, MMP2>& iOther) const;
	template <class MMP2> const bool collides(const Aabb3D<T, MMP2>& iOther) const;

	template <class RandomGenerator> const TPoint random(RandomGenerator& ioRandom) const;

	void clear();
	const bool isEmpty() const;
	const bool isValid() const;

	template <typename MMP2> void swap(Aabb3D<T, MMP2>& iOther);

private:

	TPoint min_;
	TPoint max_;
};

template <typename T, class MMP>
T distance(const Aabb3D<T, MMP>& iA, const Point3D<T>& iB);

template <typename T, class MMPa, class MMPb>
T distance(const Aabb3D<T, MMPa>& iA, const Aabb3D<T, MMPb>& iB);

template <typename T, class MMPa, class MMPb, class MMPr>
Result intersect(const Aabb3D<T, MMPa>& iA, const Aabb3D<T, MMPb>& iB, Aabb3D<T, MMPr>& oResult);

template <typename T, class MMP>
std::ostream& operator<<(std::ostream& ioOStream, const Aabb3D<T, MMP>& iAabb);

template <typename T, class MMP>
io::XmlOStream& operator<<(io::XmlOStream& ioOStream, const Aabb3D<T, MMP>& iAabb);

}

}

#include "aabb_3d.inl"

#define LASS_PRIM_PYOBJECT_UTIL_AABB_3D
#ifdef LASS_GUARDIAN_OF_INCLUSION_PRIM_PYOBJECT_UTIL_H
#	include "pyobject_util.h"
#endif

#endif
