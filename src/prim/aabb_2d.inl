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



#ifndef LASS_GUARDIAN_OF_INCLUSION_PRIM_AABB_2D_INL
#define LASS_GUARDIAN_OF_INCLUSION_PRIM_AABB_2D_INL



#include "aabb_2d.h"
#include <cstdlib>


namespace lass
{

namespace prim
{

/** Construct an empty bounding box.
 */
template <typename T, class MMP>
Aabb2D<T, MMP>::Aabb2D():
	min_(TNumTraits::max, TNumTraits::max),
	max_(TNumTraits::min, TNumTraits::min)
{
	LASS_ASSERT(isEmpty());
}



/** Construct bounding box, spanned by iMin and iMax
 */
template <typename T, class MMP>
Aabb2D<T, MMP>::Aabb2D(const TPoint& iMin, const TPoint& iMax):
	min_(iMin),
	max_(iMax)
{
	MMP::checkMinMax(min_, max_);
	LASS_ASSERT(isValid());
}



/** Construct bounding box around a single point (min == max)
 */
template <typename T, class MMP>
Aabb2D<T, MMP>::Aabb2D(const TPoint& iPoint):
	min_(iPoint),
	max_(iPoint)
{
	LASS_ASSERT(isValid());
}



/** copy constructor.
 */
template <typename T, class MMP>
template <class MMP2>
Aabb2D<T, MMP>::Aabb2D(const Aabb2D<T, MMP2>& iOther):
	min_(iOther.min()),
	max_(iOther.max())
{
	LASS_ASSERT(isValid());
}



/** return corner with smallest component values
 */
template <typename T, class MMP>
const typename Aabb2D<T, MMP>::TPoint&
Aabb2D<T, MMP>::min() const
{
	LASS_ASSERT(isValid());
	return min_;
}



/** Return corner with largest component values
 */
template <typename T, class MMP>
const typename Aabb2D<T, MMP>::TPoint&
Aabb2D<T, MMP>::max() const
{
	LASS_ASSERT(isValid());
	return max_;
}



/** set corner with smallest component values
 */
template <typename T, class MMP>
void Aabb2D<T, MMP>::setMin(const TPoint& iMin)
{
	if (isEmpty())
	{
		min_ = iMin;
		max_ = iMin;
	}
	else
	{
		MMP::setMin(min_, max_, iMin);
	}
	LASS_ASSERT(isValid());
}



/** set corner with larges component values
 */
template <typename T, class MMP>
void Aabb2D<T, MMP>::setMax(const TPoint& iMax)
{
	if (isEmpty())
	{
		min_ = iMax;
		max_ = iMax;
	}
	else
	{
		MMP::setMax(min_, max_, iMax);
	}
	LASS_ASSERT(isValid());
}



/** assign one bounding box to another.
 */
template <typename T, class MMP>
template <class MMP2>
typename Aabb2D<T, MMP>::TSelf&
Aabb2D<T, MMP>::operator=(const Aabb2D<T, MMP2>& iOther)
{
	TSelf temp(iOther);
	swap(temp);
	return *this;
}



/** Expand bounding box so it contains iPoint.
 */
template <typename T, class MMP>
typename Aabb2D<T, MMP>::TSelf&
Aabb2D<T, MMP>::operator+=(const TPoint& iPoint)
{
	min_ = pointwiseMin(min_, iPoint);
	max_ = pointwiseMax(max_, iPoint);
	LASS_ASSERT(isValid());
	return *this;
}



/** Expand bounding box so it contains the other bounding box.
 */
template <typename T, class MMP>
template <class MMP2>
typename Aabb2D<T, MMP>::TSelf&
Aabb2D<T, MMP>::operator+=(const Aabb2D<T, MMP2>& iOther)
{
	min_ = pointwiseMin(min_, iOther.min());
	max_ = pointwiseMax(max_, iOther.max());
	LASS_ASSERT(isValid());
	return *this;
}


/** Expand bounding box by distance iDistance.  Negative values causing
 *  reversal of the bounding box will cause the box to shrink to the
 *  empty box.
 */
template <typename T, class MMP>
void Aabb2D<T, MMP>::grow(TParam iDistance)
{
	min_.x -= iDistance;
	max_.x += iDistance;
	min_.y -= iDistance;
	max_.y += iDistance;
	if (max_.x < min_.x || max_.y < min_.y)
	{
		clear();
	}
	LASS_ASSERT(isValid());
}



/** Scale bounding box by scale iScale.  Fractions will shrink the bounding box.
 *  The origin of scaling is the center of the bounding box.  Negative values of the
 *  scale have same effect as positive ones.
 */
template <typename T, class MMP>
void Aabb2D<T, MMP>::scale(TParam iScale)
{
	const TVector extra = size() * ((num::abs(iScale) - 1) / 2);
	min_ -= extra;
	max_ += extra;
	LASS_ASSERT(isValid());
}



/** Return the center point of the bounding box.
 *  We return a homogeneous point to avoid the division by two (that might not be supported
 *  by some types like integers)
 */
template <typename T, class MMP>
const typename Aabb2D<T, MMP>::TPointH
Aabb2D<T, MMP>::center() const
{
	LASS_ASSERT(isValid());
	return min_ + max_;
}



/** Return size of bounding box per axis, max - min.
 */
template <typename T, class MMP>
const typename Aabb2D<T, MMP>::TVector
Aabb2D<T, MMP>::size() const
{
	LASS_ASSERT(isValid());
	return max_ - min_;
}



/** Returns perimeter of bounding box.
 */
template <typename T, class MMP>
const typename Aabb2D<T, MMP>::TValue
Aabb2D<T, MMP>::perimeter() const
{
	LASS_ASSERT(isValid());
	const TVector result = size();
	return 2 * (result.x + result.y);
}



/** Returns area of bounding box.
 */
template <typename T, class MMP>
const typename Aabb2D<T, MMP>::TValue
Aabb2D<T, MMP>::area() const
{
	LASS_ASSERT(isValid());
	const TVector result = size();
	return result.x * result.y;
}


/** Classify if a point is in or outside the bounding box, or on its surface
 *  @return sInside, sSurface, sOutside
 */
template <typename T, class MMP>
const Side Aabb2D<T, MMP>::classify(const TPoint& iPoint) const
{
	LASS_ASSERT(isValid());

	if (iPoint.x > min_.x && iPoint.x < max_.x &&
		iPoint.y > min_.y && iPoint.y < max_.y)
	{
		return sInside;
	}

	if (iPoint.x < min_.x || iPoint.x > max_.x ||
		iPoint.y < min_.y || iPoint.y > max_.y)
	{
		return sOutside;
	}

	return sSurface;
}



/** Returns true if point is inside bounding box or on its surface.
 *  Is equivalent to this->classify(iPoint) != sOutside, but might be faster.
 */
template <typename T, class MMP>
const bool Aabb2D<T, MMP>::contains(const TPoint& iPoint) const
{
	LASS_ASSERT(isValid());
	return  iPoint.x >= min_.x && iPoint.x <= max_.x &&
			iPoint.y >= min_.y && iPoint.y <= max_.y;
}



/** Returns true if the AABB iOther is inside (or on its surface) this AABB.
 *  - Is equivalent to this->contains(iOther.min()) && this->contains(iOther.max()).
 *  - if iOther is an empty AABB, it will always return true (an empty set is always a part of
 *    any other set).
 */
template <typename T, class MMP>
template <class MMP2>
const bool Aabb2D<T, MMP>::contains(const Aabb2D<T, MMP2>& iOther) const
{
	LASS_ASSERT(isValid() && iOther.isValid());
	return iOther.min().x >= min_.x && iOther.max().x <= max_.x
		&& iOther.min().y >= min_.y && iOther.max().y <= max_.y;
}



/** Check if two axis-aligned bounding boxes do intersect.
 *  @return @arg false      intersection of the AABBs is empty.
 *          @arg true       intersection of the AABBs is not empty.
 *
 *  @par FAQ: What's the difference between @c this->intersects(iOther) and @c this->collides(iOther) ?
 *      When two AABB are touching each other (surface to surface), the intersection isn't empty.
 *      i.e. the intersection is the line of points that belong to both the surfaces of the AABBs.
 *      In that case, the intersection is a degenerated AABB though, one with @c area()==0.
 *      In collision detection however, you don't want this degenerated case of intersection to be
 *      considered as a collision.  Two bodies may touch, but the must not have an overlap thas has a
 *      non-zero area.  That's why we have two methods: @c intersects returns true on touching
 *      AABBs, @c collides will return false.  Of course, in the non-degenerate cases, they behave the
 *      same.
 *
 *  Use the seperating axis test to test if two AABB's do intersect:
 *  GOMEZ M. (1999), <i>Simple Intersection Tests For Games</i>, Gamasutra,
 *  http://www.gamasutra.com,
 *  http://www.gamasutra.com/features/19991018/Gomez_3.htm
 */
template <typename T, class MMP>
template <class MMP2>
const bool Aabb2D<T, MMP>::intersects(const Aabb2D<T, MMP2>& iOther) const
{
	LASS_ASSERT(isValid() && iOther.isValid());

	// test if both AABB's do intersect by using the SEPERATING AXIS TEST.
	// notice that the weight of the homogenous points iA.center() and iB.center() is two,
	// so we can avond the division by two of doubleExtend.
	// that's why we also have to just take the sum of the sizes and not the half sum.

	LASS_ASSERT(center().weight() == 2 && iOther.center().weight() == 2);
	const TPointH doubleCenterToCenter = center() - iOther.center();

	LASS_ASSERT(doubleCenterToCenter.weight() == 0);
	const typename TPointH::TVector doubleCenterDistance =
		doubleCenterToCenter.position().transform(num::abs);

	const TVector doubleExtend = size() + iOther.size();
	return doubleCenterDistance.x <= doubleExtend.x && doubleCenterDistance.y <= doubleExtend.y;
}



/** Check if two axis-aligned bounding boxes do dollide.
 *  @return @arg true       the AABBs do collide.
 *          @arg false      they don't.
 *
 *  @par FAQ: What's the difference between @c this->intersects(iOther) and @c this->collides(iOther) ?
 *      When two AABB are touching each other (surface to surface), the intersection isn't empty.
 *      i.e. the intersection is the line of points that belong to both the surfaces of the AABBs.
 *      In that case, the intersection is a degenerated AABB though, one with @c area()==0.
 *      In collision detection however, you don't want this degenerated case of intersection to be
 *      considered as a collision.  Two bodies may touch, but the must not have an overlap thas has a
 *      non-zero area.  That's why we have two methods: @c intersects returns true on touching
 *      AABBs, @c collides will return false.  Of course, in the non-degenerate cases, they behave the
 *      same.
 *
 *  Use the seperating axis test to test if two AABB's do intersect:
 *  GOMEZ M. (1999), <i>Simple Intersection Tests For Games</i>, Gamasutra,
 *  http://www.gamasutra.com,
 *  http://www.gamasutra.com/features/19991018/Gomez_3.htm
 */
template <typename T, class MMP>
template <class MMP2>
const bool Aabb2D<T, MMP>::collides(const Aabb2D<T, MMP2>& iOther) const
{
	LASS_ASSERT(isValid() && iOther.isValid());

	// test if both AABB's do intersect by using the SEPERATING AXIS TEST.
	// notice that the weight of the homogenous points iA.center() and iB.center() is two,
	// so we can avond the division by two of doubleExtend.
	// that's why we also have to just take the sum of the sizes and not the half sum.

	LASS_ASSERT(center().weight() == 2 && iOther.center().weight() == 2);
	const TPointH doubleCenterToCenter = center() - iOther.center();

	LASS_ASSERT(doubleCenterToCenter.weight() == 0);
	const typename TPointH::TVector doubleCenterDistance =
		doubleCenterToCenter.position().transform(num::abs);

	const TVector doubleExtend = size() + iOther.size();
	return doubleCenterDistance.x < doubleExtend.x && doubleCenterDistance.y < doubleExtend.y;
}



/** Return a random point so that bounding box contains it.
 */
template <typename T, class MMP>
template <class RandomGenerator>
const typename Aabb2D<T, MMP>::TPoint
Aabb2D<T, MMP>::random(RandomGenerator& ioGenerator) const
{
	LASS_ASSERT(isValid());
	num::DistributionUniform<TValue, RandomGenerator> uniform(ioGenerator);
	const TVector t(uniform(), uniform());
	const TPoint result(min_ + t * (max_ - min_));
	LASS_ASSERT(contains(result));
	return result;
}



/** set AABB to an empty box
 */
template <typename T, class MMP>
void Aabb2D<T, MMP>::clear()
{
	min_ = TPoint(TNumTraits::max, TNumTraits::max);
	max_ = TPoint(TNumTraits::min, TNumTraits::min);
	LASS_ASSERT(isValid() && isEmpty());
}



/** Return true if bounding box contains no points.
 *  i.e. this->contains(Point3D<T>(x, y, z)) will return false for all possible values of
 *  x, y and z.
 */
template <typename T, class MMP>
const bool Aabb2D<T, MMP>::isEmpty() const
{
	LASS_ASSERT(isValid());
	return min_.x > max_.x;
}



/** internal check to see if AABB is valid.
 *  There are two valid states for the AABB:
 *  @arg <tt>max_.x < min_.x</tt> which means the box is empty
 *  @arg <tt>max_.x >= min_.x && max_.y >= min_.y</tt> which means the box is not empty.
 *  That gives us an invalid state as well:
 *  @arg <tt>max_.x >= min_.x && max_.y < min_.y</tt>.  This state would cause @c isEmpty() to yield
 *      false, while there's still nothing in it (there's no single point for which @c contains(p)
 *      would return true.
 *
 *  When the regular minmax policies are used (StrictMinMax and AutoMinMax), there's no way any AABB
 *  would become invalid, and this test counts as an invariant to the box.  However, when using the
 *  UncheckedMinMax policy, you're on your own.
 */
template <typename T, class MMP>
const bool Aabb2D<T, MMP>::isValid() const
{
	return (min_.x <= max_.x && min_.y <= max_.y) || (min_.x > max_.x);
}



/** swap two bounding boxes.
 */
template <typename T, class MMP>
template <typename MMP2>
void Aabb2D<T, MMP>::swap(Aabb2D<T, MMP2>& iOther)
{
	std::swap(min_, iOther.min_);
	std::swap(max_, iOther.max_);
	LASS_ASSERT(isValid() && iOther.isValid());
}



// --- free ----------------------------------------------------------------------------------------

/** join two AABBs
 *  @relates Aabb2D
 */
template <typename T, class MMPa, class MMPb> inline
const Aabb2D<T, MMPa> operator+(const Aabb2D<T, MMPa>& iA, const Aabb2D<T, MMPb>& iB)
{
	Aabb2D<T, MMPa> result(iA);
	result += iB;
	return result;
}



/** add a point to an AABB
 *  @relates Aabb2D
 */
template <typename T, class MMP> inline
const Aabb2D<T, MMP> operator+(const Aabb2D<T, MMP>& iA, const Point2D<T>& iB)
{
	Aabb2D<T, MMP> result(iA);
	result += iB;
	return result;
}



/** add a point to an AABB
 *  @relates Aabb2D
 */
template <typename T, class MMP> inline
const Aabb2D<T, MMP> operator+(const Point2D<T>& iA, const Aabb2D<T, MMP>& iB)
{
	Aabb2D<T, MMP> result(iB);
	result += iA;
	return result;
}



/** distance between AABB and point
 *  @relates Aabb2D
 *  @param iA   AABB
 *  @param iB   point
 *  @return absolute distance between point and AABB.  If point is inside AABB, distance is 0.
 *  @pre @a iA should not be empty.  Undefined behaviour if it is empty.
 */
template <typename T, class MMP> inline
T distance(const Aabb2D<T, MMP>& iA, const Point2D<T>& iB)
{
	LASS_ASSERT(!iA.isEmpty());
	typedef typename Point2D<T>::TVector TVector;
	return pointwiseMax(pointwiseMax(iA.min() - iB, iB - iA.max()), TVector()).norm();
}



/** distance between two AABBs
 *  @relates Aabb2D
 *  @param iA   AABB
 *  @param iB   AABB
 *  @return absolute distance.  If one AABB is completely inside the other, distance is 0.
 *  @pre @a iA and @a iB should not be empty.  Undefined behaviour if they are.
 */
template <typename T, class MMPa, class MMPb> inline
T distance(const Aabb2D<T, MMPa>& iA, const Aabb2D<T, MMPb>& iB)
{
	LASS_ASSERT(!iA.isEmpty() && !iB.isEmpty());
	typedef typename Point2D<T>::TVector TVector;
	return pointwiseMax(pointwiseMax(iA.min() - iB.max(), iB.min() - iA.max()), TVector()).norm();
}



/** Calculate the intersection of two axis aligned bounding boxes.
 *  @relates lass::prim::Aabb2D
 *  @param iA the first AABB :)
 *  @param iB the second AABB
 *  @param oResult the intersection of @a iA and @a iB.  In contrary to other intersection
 *                 functions, this output argument will @e always be assigned, even if there's no
 *                 result.  By no result we mean: the intersection is empty.  For most other
 *                 intersection functions, we can't assign a meaning full value if there's no
 *                 intersection, so we don't.  However, in this case we can assign an @e empty AABB.
 *                 And that's exactly what we do.  So, the output argument is @e always valid, even
 *                 if the return value suggests otherwise (in fact, you don't have to bother the
 *                 return value this time)
 *  @return @arg rNone      intersection of the AABBs is empty.
 *                          @a oResult is an @e empty AABB.
 *          @arg rOne       intersection of the AABBs is not empty.
 *                          @a oResult contains intersection.
 */
template <typename T, class MMPa, class MMPb, class MMPr>
Result intersect(const Aabb2D<T, MMPa>& iA, const Aabb2D<T, MMPb>& iB, Aabb2D<T, MMPr>& oResult)
{
	LASS_ASSERT(iA.isValid() && iB.isValid());

	if (!iA.intersects(iB))
	{
		oResult = Aabb2D<T, MMPr>(); // empty box
		return rNone;
	}

	// by now, we're sure they are intersecting.  now, we only need the highest minimum
	// and lowest maximum.
	//
	oResult = Aabb2D<T, MMPr>(pointwiseMax(iA.min(), iB.min()), pointwiseMin(iA.max(), iB.max()));
	return rOne;
}



/** @relates lass::prim::Aabb2D
 */
template <typename T, class MMP>
std::ostream& operator<<(std::ostream& ioOStream, const Aabb2D<T, MMP>& iAabb)
{
	LASS_ENFORCE_STREAM(ioOStream) << "{m=" << iAabb.min() << ", M=" << iAabb.max() << "}";
	return ioOStream;
}



/** @relates lass::prim::Aabb2D
 */
template<typename T, class MMP>
io::XmlOStream& operator<<(io::XmlOStream& ioOStream, const Aabb2D<T, MMP>& iAabb)
{
	LASS_ENFORCE_STREAM(ioOStream)
		<< "<Aabb2D>\n"
		<< "<min>" << iAabb.min() << "</min>\n"
		<< "<max>" << iAabb.max() << "</max>\n"
		<< "</Aabb2D>\n";

	return ioOStream;
}



/** @relates lass::prim::Aabb2D
 */
template <typename T, class MMP>
io::MatlabOStream& operator<<(io::MatlabOStream& ioOStream, const Aabb2D<T, MMP>& iAabb)
{
	typedef typename Aabb2D<T, MMP>::TPoint TPoint;
	const TPoint& min = iAabb.min();
	const TPoint& max = iAabb.max();

	ioOStream << "lasthandle = patch(";
	ioOStream << "[" << min.x() << "," << max.x() << "," << max.x() << "," << min.x() << "],";
	ioOStream << "[" << min.y() << "," << min.y() << "," << max.y() << "," << max.y() << "],";
	ioOStream << ioOStream.color() << ",'EdgeColor'," << ioOStream.color() << ",";

	if (ioOStream.flag("wireframe"))
	{
		ioOStream << "'FaceColor','none',";
	}
	else
	{
		ioOStream << "'FaceColor'," << ioOStream.color() << ",'FaceAlpha',0.25";
	}
	ioOStream << ");\n";

	return ioOStream;
}



}

}

#endif
