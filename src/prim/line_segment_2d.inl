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

#ifndef LASS_GUARDIAN_OF_INCLUSION_PRIM_LINE_SEGMENT_2D_INL
#define LASS_GUARDIAN_OF_INCLUSION_PRIM_LINE_SEGMENT_2D_INL

#include "line_segment_2d.h"
#include "point_2dh.h"
#include "../num/floating_point_consistency.h"

namespace lass
{
namespace prim
{

template <typename T, class PP>
LineSegment2D<T, PP>::LineSegment2D():
	tail_(),
	head_()
{
	LASS_ASSERT(tail_.isZero());
	LASS_ASSERT(head_.isZero());
}



template <typename T, class PP>
LineSegment2D<T, PP>::LineSegment2D(const TPoint& tail, const TPoint& head):
	tail_(tail),
	head_(head)
{
}



template <typename T, class PP> inline
const typename LineSegment2D<T, PP>::TPoint&
LineSegment2D<T, PP>::tail() const
{
	return tail_;
}



template <typename T, class PP> inline
typename LineSegment2D<T, PP>::TPoint&
LineSegment2D<T, PP>::tail()
{
	return tail_;
}



template <typename T, class PP> inline
const typename LineSegment2D<T, PP>::TPoint&
LineSegment2D<T, PP>::head() const
{
	return head_;
}



template <typename T, class PP> inline
typename LineSegment2D<T, PP>::TPoint&
LineSegment2D<T, PP>::head()
{
	return head_;
}



/** Return point on ray by it's parameter.
 *  @return origin + t * direction
 */
template <typename T, class PP>
const typename LineSegment2D<T, PP>::TPoint
LineSegment2D<T, PP>::point(TParam t) const
{
	TParameterPolicy::enforceRange(t, TNumTraits::zero, TNumTraits::one);
	return tail_ + t * vector();
}



/** Return parameter of @e projection of @a iPoint on line segment.
 *  @warning the result can be out of bound [0, 1] regardless the parameter policy used.
 */
template <typename T, class PP>
const typename LineSegment2D<T, PP>::TValue
LineSegment2D<T, PP>::t(const TPoint& point) const
{
	const TVector v = vector();
	const TValue t1 =  dot(point - tail_, v);
	const TValue t2 = -dot(point - head_, v);
	const TValue t = std::max(t1,t2) / (t1 + t2);
	return t1 > t2 ? t : TNumTraits::one - t;
}



/** Return vector from tail to head.
 */
template <typename T, class PP>
const typename LineSegment2D<T, PP>::TVector
LineSegment2D<T, PP>::vector() const
{
	return head_ - tail_;
}



/** Return length of line segment.
 */
template <typename T, class PP>
const typename LineSegment2D<T, PP>::TValue
LineSegment2D<T, PP>::length() const
{
	const TVector v = vector();
	return v.norm();
}



template <typename T, class PP>
T squaredDistance(const LineSegment2D<T, PP>& segment, const Point2D<T>& point)
{
	const T dTail = squaredDistance(segment.tail(), point);
	const T dHead = squaredDistance(segment.head(), point);
	const T dLine = segment.vector().reject(point - segment.tail()).squaredNorm();
	return std::min(dLine, std::min(dTail, dHead));
}



template <typename T, class PP>
T distance(const LineSegment2D<T, PP>& segment, const Point2D<T>& point)
{
	return num::sqrt(squaredDistance(segment, point));
}



/** intersection of two line segments
 *  @relates LineSegment2D
 *  @param a [in]	line segment A
 *  @param b [in]	line segment B
 *  @param aT [out]	parameter of intersection point on line segment A
 *  @param bT [out]	parameter of intersection point on line segment B
 *  @return @arg rNone      the line segments don't intersect, they have no points in common.
 *                          @a tA and @a tB are not assigned.
 *          @arg rOne       both line segments have exactly one point in common.
 *                          @a tA and @a tB contain parameters of intersection point.
 *          @arg rInfinite  the line segments have more than one point in common, they overlap.
 *                          @a tA and @a tB are not assigned.
 */
template <typename T, class PPa, class PPb>
Result intersect(const LineSegment2D<T, PPa>& a, const LineSegment2D<T, PPb>& b, T& tA, T& tB)
{
	typedef typename LineSegment2D<T, PPa>::TVector TVector;
	typedef typename LineSegment2D<T, PPa>::TValue TValue;
	typedef typename LineSegment2D<T, PPa>::TNumTraits TNumTraits;
	typedef num::Consistent<TValue> TConsistent;

	const TVector dirA(a.vector());
	const TVector dirB(b.vector());

	const TValue denominator = -perpDot(dirA, dirB);
	if (denominator == TNumTraits::zero)
	{
		const TValue tTail = a.t(b.tail());
		const TValue tHead = b.t(b.head());
		if ((tTail < TNumTraits::zero && tHead < TNumTraits::zero) ||
			(tTail > TNumTraits::one && tHead > TNumTraits::one))
		{
			return rNone; // complete seperated along axis
		}
		else
		{
			// overlapping on axis, yet, they can lay on "different" axes.
			//
			if (doubleTriangleArea(a.tail(), a.head(), b.tail()) == TNumTraits::zero)
			{
				return rInfinite; // coincident axes
			}
			else
			{
				return rNone; // parallel
			}
		}
	}
	else
	{
		const TVector difference = b.tail() - a.tail();
		const TConsistent candidateA = -perpDot(difference, dirB) / denominator;
		const TConsistent candidateB = perpDot(dirA, difference) / denominator;
		if (candidateA < TNumTraits::zero || candidateA > TNumTraits::one ||
			candidateB < TNumTraits::zero || candidateB > TNumTraits::one)
		{
			return rNone;
		}
		else
		{
			tA = candidateA.value();
			tB = candidateB.value();
			return rOne;
		}
	}
}



/** intersection of two line segments
 *  @relates Line2D
 *  @param a [in]	line segment A
 *  @param b [in]	line segment B
 *  @param point [out]	intersection point
 *  @return @arg rNone      the line segments don't intersect, they have no points in common.
 *                          @a point is not assigned.
 *          @arg rOne       both line segments have exactly one point in common.
 *                          @a point contains intersection point.
 *          @arg rInfinite  the line segments have more than one point in common, they overlap.
 *                          @a point is not assigned.
 */
template <typename T, class PPa, class PPb>
Result intersect(const LineSegment2D<T, PPa>& a, const LineSegment2D<T, PPb>& b, Point2D<T>& point)
{
	T tA;
	T tB;
	const Result result = intersect(a, b, tA, tB);
	if (result == rOne)
	{
		Point2DH<T> intersection(a.point(tA));
		intersection += b.point(tB);
		point = intersection.affine(); // take average for more stable point.
	}
	return result;
}


/** @relates lass::prim::LineSegment2D
 */
template <typename T, class PPa, class PPb> bool operator==(const LineSegment2D<T, PPa>& a, const LineSegment2D<T, PPb>& b)
{
	return a.tail()==b.tail() && a.head()==b.head();
}

/** @relates lass::prim::LineSegment2D
 */
template <typename T, class PPa, class PPb> bool operator!=(const LineSegment2D<T, PPa>& a, const LineSegment2D<T, PPb>& b)
{
	return !(a==b);
}


/** @relates lass::prim::LineSegment2D
 */
template<typename T, class PP>
std::ostream& operator<<(std::ostream& stream, const LineSegment2D<T, PP>& segment)
{
	LASS_ENFORCE_STREAM(stream) << "{T=" << segment.tail() << ", H=" << segment.head() << "}";
	return stream;
}



/** @relates lass::prim::LineSegment2D
 */
template<typename T, class PP>
io::XmlOStream& operator<<(io::XmlOStream& stream, const LineSegment2D<T, PP>& segment)
{
	LASS_ENFORCE_STREAM(stream)
		<< "<LineSegment2D>\n"
		<< "<tail>" << segment.tail() << "</tail>\n"
		<< "<head>" << segment.head() << "</head>\n"
		<< "</LineSegment2D>\n";

	return stream;
}


/** @relates lass::prim::LineSegment2D
 */
template<typename T, class PP>
lass::io::MatlabOStream& operator<<(lass::io::MatlabOStream& stream, const LineSegment2D<T, PP>& segment)
{
	LASS_ENFORCE_STREAM(stream) << "lasthandle = line(";
	stream << "[" << segment.tail().x << "," << segment.head().x << "],";
	stream << "[" << segment.tail().y << "," << segment.head().y << "],";
	stream << "'Color'," << stream.color() << ");\n";
	return stream;
}


}

}

#endif
