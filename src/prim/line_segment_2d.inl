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
LineSegment2D<T, PP>::LineSegment2D(const TPoint& iTail, const TPoint& iHead):
	tail_(iTail),
	head_(iHead)
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
LineSegment2D<T, PP>::point(TParam iT) const
{
	TParameterPolicy::enforceRange(iT, TNumTraits::zero, TNumTraits::one);
	return tail_ + iT * vector();
}



/** Return parameter of @e projection of @a iPoint on line segment.
 *  @warning the result can be out of bound [0, 1] regardless the parameter policy used.
 */
template <typename T, class PP>
const typename LineSegment2D<T, PP>::TValue
LineSegment2D<T, PP>::t(const TPoint& iPoint) const
{
	const TVector v = vector();
	const TValue t1 =  dot(iPoint - tail_, v);
	const TValue t2 = -dot(iPoint - head_, v);
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



/** intersection of two line segments
 *  @relates LineSegment2D
 *  @param iA   line segment A
 *  @param iB   line segment B
 *  @param oTa  parameter of intersection point on line segment A
 *  @param oTb  parameter of intersection point on line segment B
 *  @return @arg rNone      the line segments don't intersect, they have no points in common.
 *                          @a oTa and @a oTb are not assigned.
 *          @arg rOne       both line segments have exactly one point in common.
 *                          @a oTa and @a oTb contain parameters of intersection point.
 *          @arg rInfinite  the line segments have more than one point in common, they overlap.
 *                          @a oTa and @a oTb are not assigned.
 */
template <typename T, class PPa, class PPb>
Result intersect(const LineSegment2D<T, PPa>& iA, const LineSegment2D<T, PPb>& iB,
				 T& oTa, T& oTb)
{
	typedef typename LineSegment2D<T, PPa>::TVector TVector;
	typedef typename LineSegment2D<T, PPa>::TValue TValue;
	typedef typename LineSegment2D<T, PPa>::TNumTraits TNumTraits;

	const TVector dirA(iA.vector());
	const TVector dirB(iB.vector());

	const TValue denominator = -perpDot(dirA, dirB);
	if (denominator == TNumTraits::zero)
	{
		const TValue tTail = iA.t(iB.tail());
		const TValue tHead = iB.t(iB.head());
		if ((tTail < TNumTraits::zero && tHead < TNumTraits::zero) ||
			(tTail > TNumTraits::one && tHead > TNumTraits::one))
		{
			return rNone; // complete seperated along axis
		}
		else
		{
			// overlapping on axis, yet, they can lay on "different" axes.
			//
			if (doubleTriangleArea(iA.tail(), iA.head(), iB.tail()) == TNumTraits::zero)
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
		const TVector difference = iB.tail() - iA.tail();
		const TValue tA = -perpDot(difference, dirB) / denominator;
		const TValue tB = perpDot(dirA, difference) / denominator;
		if (tA < TNumTraits::zero || tA > TNumTraits::one ||
			tB < TNumTraits::zero || tB > TNumTraits::one)
		{
			return rNone;
		}
		else
		{
			oTa = tA;
			oTb = tB;
			return rOne;
		}
	}
}



/** intersection of two line segments
 *  @relates Line2D
 *  @param iA   line segment A
 *  @param iB   line segment B
 *  @param oPoint   intersection point
 *  @return @arg rNone      the line segments don't intersect, they have no points in common.
 *                          @a oPoint is not assigned.
 *          @arg rOne       both line segments have exactly one point in common.
 *                          @a oPoint contains intersection point.
 *          @arg rInfinite  the line segments have more than one point in common, they overlap.
 *                          @a oPoint is not assigned.
 */
template <typename T, class PPa, class PPb>
Result intersect(const LineSegment2D<T, PPa>& iA, const LineSegment2D<T, PPb>& iB,
				 Point2D<T>& oPoint)
{
	T tA;
	T tB;
	const Result result = intersect(iA, iB, tA, tB);
	if (result == rOne)
	{
		Point2DH<T> intersection(iA.point(tA));
		intersection += iB.point(tB);
		oPoint = intersection.affine(); // take average for more stable point.
	}
	return result;
}


/** @relates lass::prim::LineSegment2D
 */
template <typename T, class PPa, class PPb> bool operator==(const LineSegment2D<T, PPa>& iA, const LineSegment2D<T, PPb>& iB)
{
	return iA.tail()==iB.tail() && iA.head()==iB.head();
}

/** @relates lass::prim::LineSegment2D
 */
template <typename T, class PPa, class PPb> bool operator!=(const LineSegment2D<T, PPa>& iA, const LineSegment2D<T, PPb>& iB)
{
	return !(iA==iB);
}


/** @relates lass::prim::LineSegment2D
 */
template<typename T, class PP>
std::ostream& operator<<(std::ostream& ioOStream, const LineSegment2D<T, PP>& iLineSegment)
{
	LASS_ENFORCE_STREAM(ioOStream) << "{T=" << iLineSegment.tail() << ", H=" << iLineSegment.head() << "}";
	return ioOStream;
}



/** @relates lass::prim::LineSegment2D
 */
template<typename T, class PP>
io::XmlOStream& operator<<(io::XmlOStream& ioOStream, const LineSegment2D<T, PP>& iLineSegment)
{
	LASS_ENFORCE_STREAM(ioOStream)
		<< "<LineSegment2D>\n"
		<< "<tail>" << iLineSegment.tail() << "</tail>\n"
		<< "<head>" << iLineSegment.head() << "</head>\n"
		<< "</LineSegment2D>\n";

	return ioOStream;
}


/** @relates lass::prim::LineSegment2D
 */
template<typename T, class PP>
lass::io::MatlabOStream& operator<<(lass::io::MatlabOStream& oOStream, const LineSegment2D<T, PP>& iLineSegment)
{
	LASS_ENFORCE_STREAM(oOStream) << "lasthandle = line(";
	oOStream << "[" << iLineSegment.tail().x << "," << iLineSegment.head().x << "],";
	oOStream << "[" << iLineSegment.tail().y << "," << iLineSegment.head().y << "],";
	oOStream << "'Color'," << oOStream.color() << ");\n";
	return oOStream;
}


}

}

#endif
