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
	typedef typename LineSegment2D<T, PP>::TVector TVector;
	typedef typename LineSegment2D<T, PP>::TValue TValue;

	const TVector edge = segment.vector();
	const TVector v = point - segment.tail();
	const TValue t = dot(v, edge);
	const TValue tMax = dot(edge, edge);
	if (t <= 0)
	{
		return v.squaredNorm();
	}
	if (t >= tMax)
	{
		return squaredDistance(segment.head(), point);
	}
	return (v - edge * (t / tMax)).squaredNorm();
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
