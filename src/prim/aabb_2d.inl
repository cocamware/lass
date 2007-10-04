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



/** Construct bounding box, spanned by min and max
 */
template <typename T, class MMP>
Aabb2D<T, MMP>::Aabb2D(const TPoint& min, const TPoint& max):
	min_(min),
	max_(max)
{
	MMP::checkMinMax(min_, max_);
	LASS_ASSERT(isValid());
}



/** Construct bounding box around a single point (min == max)
 */
template <typename T, class MMP>
Aabb2D<T, MMP>::Aabb2D(const TPoint& point):
	min_(point),
	max_(point)
{
	LASS_ASSERT(isValid());
}



/** copy constructor.
 */
template <typename T, class MMP>
template <class MMP2>
Aabb2D<T, MMP>::Aabb2D(const Aabb2D<T, MMP2>& other):
	min_(other.min()),
	max_(other.max())
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
void Aabb2D<T, MMP>::setMin(const TPoint& min)
{
	if (isEmpty())
	{
		min_ = min;
		max_ = min;
	}
	else
	{
		MMP::setMin(min_, max_, min);
	}
	LASS_ASSERT(isValid());
}



/** set corner with larges component values
 */
template <typename T, class MMP>
void Aabb2D<T, MMP>::setMax(const TPoint& max)
{
	if (isEmpty())
	{
		min_ = max;
		max_ = max;
	}
	else
	{
		MMP::setMax(min_, max_, max);
	}
	LASS_ASSERT(isValid());
}



/** assign one bounding box to another.
 */
template <typename T, class MMP>
template <class MMP2>
typename Aabb2D<T, MMP>::TSelf&
Aabb2D<T, MMP>::operator=(const Aabb2D<T, MMP2>& other)
{
	TSelf temp(other);
	swap(temp);
	return *this;
}



/** Expand bounding box so it contains point.
 */
template <typename T, class MMP>
typename Aabb2D<T, MMP>::TSelf&
Aabb2D<T, MMP>::operator+=(const TPoint& point)
{
	min_ = pointwiseMin(min_, point);
	max_ = pointwiseMax(max_, point);
	LASS_ASSERT(isValid());
	return *this;
}



/** Expand bounding box so it contains the other bounding box.
 */
template <typename T, class MMP>
template <class MMP2>
typename Aabb2D<T, MMP>::TSelf&
Aabb2D<T, MMP>::operator+=(const Aabb2D<T, MMP2>& other)
{
	min_ = pointwiseMin(min_, other.min());
	max_ = pointwiseMax(max_, other.max());
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
const Side Aabb2D<T, MMP>::classify(const TPoint& point) const
{
	LASS_ASSERT(isValid());

	if (point.x > min_.x && point.x < max_.x &&
		point.y > min_.y && point.y < max_.y)
	{
		return sInside;
	}

	if (point.x < min_.x || point.x > max_.x ||
		point.y < min_.y || point.y > max_.y)
	{
		return sOutside;
	}

	return sSurface;
}



/** Returns true if point is inside bounding box or on its surface.
 *  Is equivalent to this->classify(point) != sOutside, but might be faster.
 */
template <typename T, class MMP>
const bool Aabb2D<T, MMP>::contains(const TPoint& point) const
{
	LASS_ASSERT(isValid());
	return  point.x >= min_.x && point.x <= max_.x &&
			point.y >= min_.y && point.y <= max_.y;
}



/** Returns true if the AABB other is inside (or on its surface) this AABB.
 *  - Is equivalent to this->contains(other.min()) && this->contains(other.max()).
 *  - if other is an empty AABB, it will always return true (an empty set is always a part of
 *    any other set).
 */
template <typename T, class MMP>
template <class MMP2>
const bool Aabb2D<T, MMP>::contains(const Aabb2D<T, MMP2>& other) const
{
	LASS_ASSERT(isValid() && other.isValid());
	return other.min().x >= min_.x && other.max().x <= max_.x
		&& other.min().y >= min_.y && other.max().y <= max_.y;
}



/** Check if two axis-aligned bounding boxes do intersect.
 *  @return @arg false      intersection of the AABBs is empty.
 *          @arg true       intersection of the AABBs is not empty.
 *
 *  @par FAQ: What's the difference between @c this->intersects(other) and @c this->collides(other) ?
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
const bool Aabb2D<T, MMP>::intersects(const Aabb2D<T, MMP2>& other) const
{
	LASS_ASSERT(isValid() && other.isValid());

	// test if both AABB's do intersect by using the SEPERATING AXIS TEST.
	// notice that the weight of the homogenous points a.center() and b.center() is two,
	// so we can avond the division by two of doubleExtend.
	// that's why we also have to just take the sum of the sizes and not the half sum.

	LASS_ASSERT(center().weight() == 2 && other.center().weight() == 2);
	const TPointH doubleCenterToCenter = center() - other.center();

	LASS_ASSERT(doubleCenterToCenter.weight() == 0);
	const typename TPointH::TVector doubleCenterDistance =
		doubleCenterToCenter.position().transform(num::abs);

	const TVector doubleExtend = size() + other.size();
	return doubleCenterDistance.x <= doubleExtend.x && doubleCenterDistance.y <= doubleExtend.y;
}



/** Check if two axis-aligned bounding boxes do dollide.
 *  @return @arg true       the AABBs do collide.
 *          @arg false      they don't.
 *
 *  @par FAQ: What's the difference between @c this->intersects(other) and @c this->collides(other) ?
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
const bool Aabb2D<T, MMP>::collides(const Aabb2D<T, MMP2>& other) const
{
	LASS_ASSERT(isValid() && other.isValid());

	// test if both AABB's do intersect by using the SEPERATING AXIS TEST.
	// notice that the weight of the homogenous points a.center() and b.center() is two,
	// so we can avond the division by two of doubleExtend.
	// that's why we also have to just take the sum of the sizes and not the half sum.

	LASS_ASSERT(center().weight() == 2 && other.center().weight() == 2);
	const TPointH doubleCenterToCenter = center() - other.center();

	LASS_ASSERT(doubleCenterToCenter.weight() == 0);
	const typename TPointH::TVector doubleCenterDistance =
		doubleCenterToCenter.position().transform(num::abs);

	const TVector doubleExtend = size() + other.size();
	return doubleCenterDistance.x < doubleExtend.x && doubleCenterDistance.y < doubleExtend.y;
}



/** Return a random point so that bounding box contains it.
 */
template <typename T, class MMP>
template <class RandomGenerator>
const typename Aabb2D<T, MMP>::TPoint
Aabb2D<T, MMP>::random(RandomGenerator& generator) const
{
	LASS_ASSERT(isValid());
	num::DistributionUniform<TValue, RandomGenerator> uniform(generator);
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
void Aabb2D<T, MMP>::swap(Aabb2D<T, MMP2>& other)
{
	std::swap(min_, other.min_);
	std::swap(max_, other.max_);
	LASS_ASSERT(isValid() && other.isValid());
}



// --- free ----------------------------------------------------------------------------------------

/** join two AABBs
 *  @relates Aabb2D
 */
template <typename T, class MMPa, class MMPb> inline
const Aabb2D<T, MMPa> operator+(const Aabb2D<T, MMPa>& a, const Aabb2D<T, MMPb>& b)
{
	Aabb2D<T, MMPa> result(a);
	result += b;
	return result;
}



/** add a point to an AABB
 *  @relates Aabb2D
 */
template <typename T, class MMP> inline
const Aabb2D<T, MMP> operator+(const Aabb2D<T, MMP>& a, const Point2D<T>& b)
{
	Aabb2D<T, MMP> result(a);
	result += b;
	return result;
}



/** add a point to an AABB
 *  @relates Aabb2D
 */
template <typename T, class MMP> inline
const Aabb2D<T, MMP> operator+(const Point2D<T>& a, const Aabb2D<T, MMP>& b)
{
	Aabb2D<T, MMP> result(b);
	result += a;
	return result;
}



/** create an aabb with a single point in it
 *	@relates Aabb2D
 */
template <typename T>
const Aabb2D<T> aabb(const Point2D<T>& point)
{
	return Aabb2D<T>(point, point);
}



/** distance between AABB and point
 *  @relates Aabb2D
 *  @param a   AABB
 *  @param b   point
 *  @return absolute distance between point and AABB.  If point is inside AABB, distance is 0.
 *  @pre @a a should not be empty.  Undefined behaviour if it is empty.
 */
template <typename T, class MMP> inline
T distance(const Aabb2D<T, MMP>& a, const Point2D<T>& b)
{
	LASS_ASSERT(!a.isEmpty());
	typedef typename Point2D<T>::TVector TVector;
	return pointwiseMax(pointwiseMax(a.min() - b, b - a.max()), TVector()).norm();
}



/** distance between two AABBs
 *  @relates Aabb2D
 *  @param a   AABB
 *  @param b   AABB
 *  @return absolute distance.  If one AABB is completely inside the other, distance is 0.
 *  @pre @a a and @a b should not be empty.  Undefined behaviour if they are.
 */
template <typename T, class MMPa, class MMPb> inline
T distance(const Aabb2D<T, MMPa>& a, const Aabb2D<T, MMPb>& b)
{
	LASS_ASSERT(!a.isEmpty() && !b.isEmpty());
	typedef typename Point2D<T>::TVector TVector;
	return pointwiseMax(pointwiseMax(a.min() - b.max(), b.min() - a.max()), TVector()).norm();
}



/** Calculate the intersection of two axis aligned bounding boxes.
 *  @relates lass::prim::Aabb2D
 *  @param a the first AABB :)
 *  @param b the second AABB
 *  @param result the intersection of @a a and @a b.  In contrary to other intersection
 *                 functions, this output argument will @e always be assigned, even if there's no
 *                 result.  By no result we mean: the intersection is empty.  For most other
 *                 intersection functions, we can't assign a meaning full value if there's no
 *                 intersection, so we don't.  However, in this case we can assign an @e empty AABB.
 *                 And that's exactly what we do.  So, the output argument is @e always valid, even
 *                 if the return value suggests otherwise (in fact, you don't have to bother the
 *                 return value this time)
 *  @return @arg rNone      intersection of the AABBs is empty.
 *                          @a result is an @e empty AABB.
 *          @arg rOne       intersection of the AABBs is not empty.
 *                          @a result contains intersection.
 */
template <typename T, class MMPa, class MMPb, class MMPr>
Result intersect(const Aabb2D<T, MMPa>& a, const Aabb2D<T, MMPb>& b, Aabb2D<T, MMPr>& result)
{
	LASS_ASSERT(a.isValid() && b.isValid());

	if (!a.intersects(b))
	{
		result = Aabb2D<T, MMPr>(); // empty box
		return rNone;
	}

	// by now, we're sure they are intersecting.  now, we only need the highest minimum
	// and lowest maximum.
	//
	result = Aabb2D<T, MMPr>(pointwiseMax(a.min(), b.min()), pointwiseMin(a.max(), b.max()));
	return rOne;
}



/** @relates lass::prim::Aabb2D
 */
template <typename T, typename MMPa, typename MMPb> inline
const bool intersects(const Aabb2D<T, MMPa>& a, const Aabb2D<T, MMPb>& b)
{
	return a.intersects(b);
}



/** @relates lass::prim::Aabb2D
 */
template <typename T, typename MMP> inline
const bool intersects(const Aabb2D<T, MMP>& a, const Point2D<T>& b)
{
	return a.contains(b);
}



/** @relates lass::prim::Aabb2D
 */
template <typename T, typename MMP> inline
const bool intersects(const Point2D<T>& a, const Aabb2D<T, MMP>& b)
{
	return b.contains(a);
}



/** @relates lass::prim::Aabb2D
 */
template <typename T, typename MMPa, typename MMPb> inline
const bool collides(const Aabb2D<T, MMPa>& a, const Aabb2D<T, MMPb>& b)
{
	return a.collides(b);
}



/** @relates lass::prim::Aabb2D
 */
template <typename T, typename MMP> inline
const bool collides(const Aabb2D<T, MMP>& a, const Point2D<T>& b)
{
	typedef typename Aabb2D<T, MMP>::TPoint TPoint;
	const TPoint& min = a.min();
	const TPoint& max = a.max();
	return min.x < b.x && b.x < max.x && min.y < b.y && b.y < max.y;
}



/** @relates lass::prim::Aabb2D
 */
template <typename T, typename MMP> inline
const bool collides(const Point2D<T>& a, const Aabb2D<T, MMP>& b)
{
	return collides(b, a);
}



/** @relates lass::prim::Aabb2D
 */
template <typename T, class MMP>
std::ostream& operator<<(std::ostream& ioOStream, const Aabb2D<T, MMP>& aabb)
{
	LASS_ENFORCE_STREAM(ioOStream) << "{m=" << aabb.min() << ", M=" << aabb.max() << "}";
	return ioOStream;
}



/** @relates lass::prim::Aabb2D
 */
template<typename T, class MMP>
io::XmlOStream& operator<<(io::XmlOStream& ioOStream, const Aabb2D<T, MMP>& aabb)
{
	LASS_ENFORCE_STREAM(ioOStream)
		<< "<Aabb2D>\n"
		<< "<min>" << aabb.min() << "</min>\n"
		<< "<max>" << aabb.max() << "</max>\n"
		<< "</Aabb2D>\n";

	return ioOStream;
}



/** @relates lass::prim::Aabb2D
 */
template <typename T, class MMP>
io::MatlabOStream& operator<<(io::MatlabOStream& ioOStream, const Aabb2D<T, MMP>& aabb)
{
	typedef typename Aabb2D<T, MMP>::TPoint TPoint;
	const TPoint& min = aabb.min();
	const TPoint& max = aabb.max();

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
