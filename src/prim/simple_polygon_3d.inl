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



#ifndef LASS_GUARDIAN_OF_INCLUSION_PRIM_SIMPLE_POLYGON_3D_INL
#define LASS_GUARDIAN_OF_INCLUSION_PRIM_SIMPLE_POLYGON_3D_INL

#include "prim_common.h"
#include "simple_polygon_3d.h"

namespace lass
{
namespace prim
{

template <typename T, class EP, class NP>
SimplePolygon3D<T, EP, NP>::SimplePolygon3D(const TPlane& iPlane) : plane_(iPlane)
{
}

template <typename T, class EP, class NP>
SimplePolygon3D<T, EP, NP>::SimplePolygon3D(const TPoint& iA, const TPoint& iB, const TPoint& iC) :
plane_(iA,iB,iC)
{
	add(iA);
	add(iB);
	add(iC);
}


/** return vertex of polygon by its index, not wrapped, no bounds check.
 */
template <typename T, class EP, class NP>
const typename SimplePolygon3D<T, EP, NP>::TPoint& 
SimplePolygon3D<T, EP, NP>::operator[](size_t iIndexOfVertex) const
{
	LASS_ASSERT(iIndexOfVertex < vertices_.size());
	return vertices_[iIndexOfVertex];
}



/** return vertex of polygon by its index, not wrapped, no bounds check.
 */
template <typename T, class EP, class NP>
typename SimplePolygon3D<T, EP, NP>::TPoint& 
SimplePolygon3D<T, EP, NP>::operator[](size_t iIndexOfVertex)
{
	LASS_ASSERT(iIndexOfVertex < vertices_.size());
	return vertices_[iIndexOfVertex];
}



/** return vertex of polygon by its index, but wrap around the bounds.
 *  this->at(-1) will return the same vertex as this->at(this->size() - 1);
 */
template <typename T, class EP, class NP>
const typename SimplePolygon3D<T, EP, NP>::TPoint& 
SimplePolygon3D<T, EP, NP>::at(int iIndexOfVertex) const
{
	const int i = num::mod(iIndexOfVertex, vertices_.size());
	LASS_ASSERT(isInRange(i));
	return vertices_[i];
}



/** return vertex of polygon by its index, but wrap around the bounds.
 *  this->at(-1) will return the same vertex as this->at(this->size() - 1);
 */
template <typename T, class EP, class NP>
typename SimplePolygon3D<T, EP, NP>::TPoint& 
SimplePolygon3D<T, EP, NP>::at(int iIndexOfVertex)
{
	const int i = num::mod(iIndexOfVertex, vertices_.size());
	LASS_ASSERT(isInRange(i));
	return vertices_[i];
}



/** return the edge of the polygon between vertices at(iIndex) and at(iIndex + 1).
 */
template <typename T, class EP, class NP>
const typename SimplePolygon3D<T, EP, NP>::TLineSegment 
SimplePolygon3D<T, EP, NP>::edge(int iIndexOfTailVertex) const
{
	return SimplePolygon3D<T, EP, NP>::TLineSegment(at(iIndexOfTailVertex), at(iIndexOfTailVertex + 1));
}



/** return the vector between vertices at(iIndex) and at(iIndex + 1)\
 */
template <typename T, class EP, class NP>
const typename SimplePolygon3D<T, EP, NP>::TVector 
SimplePolygon3D<T, EP, NP>::vector(int iIndexOfTailVertex) const
{
	return at(iIndexOfTailVertex + 1) - at(iIndexOfTailVertex);
}



/** return support plane of polygon.
 */
template <typename T, class EP, class NP>
const typename SimplePolygon3D<T, EP, NP>::TPlane& 
SimplePolygon3D<T, EP, NP>::plane() const
{
	return plane_;
}



/** access support plane of polygon.
 */
template <typename T, class EP, class NP>
typename SimplePolygon3D<T, EP, NP>::TPlane& 
SimplePolygon3D<T, EP, NP>::plane()
{
	return plane_;
}



/** return normal of plane
 */
template <typename T, class EP, class NP>
const typename SimplePolygon3D<T, EP, NP>::TVector 
SimplePolygon3D<T, EP, NP>::normal() const
{
	return plane_.normal();
}



/** determines the major axis of the normal vector.
 *  The major axis is the one with the largest (absolute) component value.  e.g. if the normal
 *  vector is (-1, 4, -8), this will be the @e z axis because abs(-8) > abs(4) > abs(-1).
 *  In case there's more than one major axis possible, the "highest" index is choosen.  e.g.
 *  if the normal vector is (1, 1, 0), then @e y axis will be choosen, because @e y has a higher
 *  index than @e x .
 */
template <typename T, class EP, class NP>
const XYZ SimplePolygon3D<T, EP, NP>::majorAxis() const
{
	return plane_.majorAxis();
}



/** add a point at the "end" of the vertex list
 */
template <typename T, class EP, class NP>
void SimplePolygon3D<T, EP, NP>::add(const TPoint& iVertex)
{
	vertices_.push_back(iVertex);
}


/** insert a vertex at iIndex (so it will sit before the current at(iIndex)).
 */
template <typename T, class EP, class NP>
void SimplePolygon3D<T, EP, NP>::insert(int iIndexOfVertex, const TPoint& iVertex)
{
	const int i = num::mod(iIndexOfVertex, static_cast<unsigned>(vertices_.size()));
	LASS_ASSERT(isInRange(i));
	vertices_.insert(i, iVertex);
}



/** remove the vertex at(iIndex)
 */
template <typename T, class EP, class NP>
void SimplePolygon3D<T, EP, NP>::remove(int iIndexOfVertex)
{
	const int i = num::mod(iIndexOfVertex, static_cast<unsigned>(vertices_.size()));
	LASS_ASSERT(isInRange(i));
	vertices_.remove(i);
}



/** return true if polygon has no vertices
 */
template <typename T, class EP, class NP>
const bool SimplePolygon3D<T, EP, NP>::isEmpty() const
{
	return vertices_.empty();
}



/** return number of vertices
 */
template <typename T, class EP, class NP>
const size_t SimplePolygon3D<T, EP, NP>::size() const
{
	return vertices_.size();
}



/** return signed polygon area.
 *
 *  <i>The area of a convex polygon is defined to be positive if the points are arranged in a
 *  counterclockwise order, and negative if they are in clockwise order.</i>,
 *  Eric W. Weisstein. "Polygon Area." From MathWorld--A Wolfram Web Resource. 
 *  http://mathworld.wolfram.com/PolygonArea.html
 *
 *  @par Algorithm:
 *  comp.graphics.algorithms Frequently Asked Questions: 
 *	Subject 2.01: "How do I find the area of a polygon?"
 *  http://www.faqs.org/faqs/graphics/algorithms-faq/
 */
template <typename T, class EP, class NP>
const typename SimplePolygon3D<T, EP, NP>::TValue 
SimplePolygon3D<T, EP, NP>::signedArea() const
{
	const size_t n = size();
	if (size < 3)
	{
		return TNumTraits::zero;
	}

	const TVector& normal = normal().normal();
	const TPoint& reference = vertices_[0];
	TValue result = TNumTraits::zero;
	for (size_t prevI = 1, i = 2; i < n; prevI = i++)
	{
		const TVector a = vertices_[prevI] - reference;
		const TVector b = vertices_[i] - reference;
		result += dot(cross(a, b), normal);
	}
	return result / 2;
}



/** return area of the polygons surface.
 *
 *  <i>The area of a surface is the amount of material needed to "cover" it completely</i>,
 *  Eric W. Weisstein. "Area." From MathWorld--A Wolfram Web Resource. 
 *	http://mathworld.wolfram.com/Area.html
 */
template <typename T, class EP, class NP>
const typename SimplePolygon3D<T, EP, NP>::TValue 
SimplePolygon3D<T, EP, NP>::area() const
{
	return num::abs(signedArea());
}



/** return sum of the lengths of all edges
 */
template <typename T, class EP, class NP>
const typename SimplePolygon3D<T, EP, NP>::TValue 
SimplePolygon3D<T, EP, NP>::perimeter() const
{
	TValue result = TNumTraits::zero;
	for (int i = 0; i < size(); ++i)
	{
		result += distance(at(i).position, at(i + 1).position);
	}
	return result;
}



/** return the barycenter of all vertices.
 *  The barycenter is the homogenous sum of all vertices.
 *  @warning for non-convex polygons, it's NOT guaranteed that this center is inside the polygon.
 */
template <typename T, class EP, class NP>
const typename SimplePolygon3D<T, EP, NP>::TPointH 
SimplePolygon3D<T, EP, NP>::vertexCentroid() const
{
	TPointH result;
	for (size_t i = 0; i < size(); ++i)
	{
		result += vertices_[i];
	}
	return result;
}



/** return the centroid of the filled polygon.
 *
 *  Eric W. Weisstein. "Geometric Centroid." From MathWorld--A Wolfram Web Resource. 
 *  http://mathworld.wolfram.com/GeometricCentroid.html
 *
 *  @par Algorithm:
 *  comp.graphics.algorithms Frequently Asked Questions: 
 *	Subject 2.02: "How can the centroid of a polygon be computed?"
 *  http://www.faqs.org/faqs/graphics/algorithms-faq/
 *
 *  @warning for non-convex polygons, it's NOT guaranteed that this center is inside the polygon.
 */
template <typename T, class EP, class NP>
const typename SimplePolygon3D<T, EP, NP>::TPointH 
SimplePolygon3D<T, EP, NP>::surfaceCentroid() const
{
	if (size < 3)
	{
		return vertexCentroid();
	}

	const TVector& normal = normal().normal();
	const TPoint& reference = vertices_[0];
	TPointH result;
	for (size_t prevI = 1, i = 2; i < n; prevI = i++)
	{
		const TVector a = vertices_[prevI] - reference;
		const TVector b = vertices_[i] - reference;
		const TValue triangleWeight = dot(cross(a, b), normal);
		const TPointH triangleCentroid = a + b + reference;
		result += triangleWeight * triangleCentroid;
	}
}



/** return true if polygon is simple, false if not.
 *
 *  <i>A polygon P is said to be simple (or Jordan) if the only points of the plane belonging to
 *  two polygon edges of P are the polygon vertices of P. Such a polygon has a well defined
 *  interior and exterior. Simple polygons are topologically equivalent to a disk.</i>,
 *  Eric W. Weisstein. "Simple Polygon." From MathWorld--A Wolfram Web Resource. 
 *	http://mathworld.wolfram.com/SimplePolygon.html 
 *
 *  In 3D, we test if the 2D mapping on the major axis is simple.
 *
 *  @warning this is a brute force test.  we simple test for all edges if they are not intersecting
 *           Hence, this is O(n^2).
 */
template <typename T, class EP, class NP>
const bool SimplePolygon3D<T, EP, NP>::isSimple() const
{
	return mapping(majorAxis()).isSimple();
}



/** return true if polygon is convex, false if not.
 *  @warning assumes polygon is simple
 *
 *  <i>A planar polygon is convex if it contains all the line segments connecting any pair of its
 *  points. Thus, for example, a regular pentagon is convex, while an indented pentagon is not.
 *  A planar polygon that is not convex is said to be a concave polygon</i>,
 *  Eric W. Weisstein. "Convex Polygon." From MathWorld--A Wolfram Web Resource. 
 *	http://mathworld.wolfram.com/ConvexPolygon.html 
 *
 *  A simple polygon is convex if all the cross products of adjacent edges will be the same sign
 *  (we ignore zero signs, only + or - are taken in account), a concave polygon will have a mixture
 *  of cross product signs.
 *
 *  A polygon with less than three vertices is always convex.  A polygon with all colinear
 *  vertices is considered convex (not very usefull maybe, but convex).
 */
template <typename T, class EP, class NP>
const bool SimplePolygon3D<T, EP, NP>::isConvex() const
{
	if (size() < 3)
	{
		return true;
	}

	TValue sign = TNumTraits::zero;
	for (int i = 0; i < size(); ++i)
	{
		const TValue s = num::sign(perpDot(vector(i - 1), vector(i))); // Ax(-B) = BxA
		if (sign != s && s != TNumTraits::zero)
		{
			if (sign != TNumTraits::zero)
			{
				return false;
			}
			else
			{
				sign = s;
			}
		}
	}

	return true;
}



/** return orientation of polygon
 *  @warning assumes polygon is simple
 */
template <typename T, class EP, class NP>
const Orientation SimplePolygon3D<T, EP, NP>::orientation() const
{
	const TValue area = signedArea();
	if (area > TNumTraits::zero)
	{
		return oCounterClockWise;
	}
	else if (area < TNumTraits::zero)
	{
		return oClockWise;
	}
	else
	{
		return oInvalid;
	}
}



/** return true if inner angle of vertex is reflex (is > 180 degrees).
 *  @warning assumes polygon is simple
 *
 *  test if signedArea() and perdDot(...) have different sign.
 *  if one of them is zero, it will return false by default.
 */
template <typename T, class EP, class NP>
const bool SimplePolygon3D<T, EP, NP>::isReflex(int iIndexOfVertex) const
{
	const TValue pd = perdDot(vector(iIndexOfVertex - 1), vector(iIndexOfVertex)); // Ax(-B) = BxA
	return signedArea() * pd < TNumTraits::zero;
}



/** maps a 3D polygon as a 2D polygon by ignoring the component along an axis.
 *
 *  if @a iAxis is @e z, then it's easy.  We ignore the @e z component and we get a polygon with
 *  only the @e x and @e y components.
 *
 *  if @a iAxis is @e x, then we have to keep the @e z axis while there's no @e z axis in 2D.  We
 *  solve this by mapping the 3D @e y axis on the 2D @e x axis, and the 3D @e z axis on the
 *  2D @e y axis.
 *
 *  if @a iAxis is @e y, then we have a similar problem.  This time the 3D @e z axis is mapped on
 *  the 2D @e x axis, and the 3D @e x axis is mapped on the 2D @e y axis.
 *
 *  You can write this in short by saying the 2D @e x axis will correspond with 3D axis (@a iAxis
 *  + 1) and the 2D @e y axis with 3D axis (@a iAxis + 2).
 *
 *  @todo explain this better
 */
template <typename T, class EP, class NP>
const SimplePolygon2D<T> SimplePolygon3D<T, EP, NP>::mapping(XYZ iAxis) const
{
	const XYZ x = iAxis + 1;
	const XYZ y = iAxis + 2;
	SimplePolygon2D<T> result;

	const int n = size();
	for (int i = 0; i < n; ++i)
	{
		result.add(Point2D<T>(vertices_[i][x], vertices_[i][y]));
	}

	return result;
}



template <typename T, class EP, class NP>
const Side SimplePolygon3D<T, EP, NP>::classify(const TPoint& iP) const
{
	return contains(iP) ? sInside : sOutside;
}



/** return true if a point @a iP is inside the polygon, on condition @a iP is on the plane
 *
 *  @par Algorithm:
 *  comp.graphics.algorithms Frequently Asked Questions: 
 *	Subject 2.03: "How do I find if a point lies within a polygon?"
 *  http://www.faqs.org/faqs/graphics/algorithms-faq/
 */
template <typename T, class EP, class NP>
const bool SimplePolygon3D<T, EP, NP>::contains(const TPoint& iP) const
{
	const XYZ major = majorAxis();
	const XYZ x = major + 1;
	const XYZ y = major + 2;

	size_t i, j;
	bool c = false;
	const size_t npol = size();
	for (i = 0, j = npol-1; i < npol; j = i++)
	{
		const TPoint& a = vertices_[i];
		const TPoint& b = vertices_[j];
		if ((((a[y]<=iP[y]) && (iP[y]<b[y])) ||
			((b[y]<=iP[y]) && (iP[y]<a[y]))) &&
			(iP[x] < (b[x] - a[x]) * (iP[y] - a[y]) / (b[y] - a[y]) + a[x]))
		c = !c;
	}
	return c;
}



/** flip normal and reverse sequence of vertices
 */
template <typename T, class EP, class NP>
void SimplePolygon3D<T, EP, NP>::flip()
{
	plane_.flip();
	std::reverse(vertices_.begin(), vertices_.end());
}



// --- private -------------------------------------------------------------------------------------

/** return if index of vertex is in range of the std::vector
 */
template <typename T, class EP, class NP>
const bool SimplePolygon3D<T, EP, NP>::isInRange(int iIndexOfVertex) const
{
	return iIndexOfVertex >= 0 && iIndexOfVertex < static_cast<int>(vertices_.size());
}



// --- free ----------------------------------------------------------------------------------------

/** Find the intersection of a line segment and a simple polygon by their parameter t on the 
 *	line segment.
 *  @relates lass::prim::LineSegment3D
 *  @relates lass::prim::SimplePolygon3D
 *
 *  @param iPolygon [in] the simple polygon
 *  @param iSegment [in] the line segment
 *  @param oT [out] the parameter of the intersection point > @a iMinT.
 *  @param iMinT [in] the minimum t that may be returned as valid intersection.
 *  @return @arg rNone      no intersections with @a oT > @a iMinT found
 *                          @a oT is not assigned.
 *          @arg rOne       a intersection with @a oT > @a iMinT is found
 *							@a oT is assigned.
 */
template<typename T, class EP, class NP, class PP>
Result intersect(const SimplePolygon3D<T, EP, NP>& iPolygon, 
				 const LineSegment3D<T, PP>& iSegment, 
				 T& oT, const T& iMinT)
{
	typedef Point2D<T> TPoint;
	typedef typename TPoint::TValue TValue;

	TValue t;
	if (intersect(iPolygon.plane(), iSegment, t, iMinT) == rOne)
	{
		if (iPolygon.contains(iSegment.point(t)))
		{
			oT = t;
			return rOne;
		}
	}
	return rNone;
}



/** Clip a polygon to a plane.
 *  @relates lass::prim::SimplePolygon3D
 *  @relates lass::prim::Plane3D
 *
 *  @param iPlane [in] the plane to clip to
 *  @param iPolygon [in] the polygon to be clipped
 *  @return the clipped polygon.
 */
template <typename T, class EP1, class NP1, class EP2, class NP2>
SimplePolygon3D<T, EP2, NP2> clip(const Plane3D<T, EP1, NP1>& iPlane, 
								  const SimplePolygon3D<T, EP2, NP2>& iPolygon)
{
	typedef Plane3D<T, EP1, NP1> TPlane;
	typedef SimplePolygon3D<T, EP2, NP2> TPolygon;
	typedef Point3D<T> TPoint;
	typedef TPoint::TValue TValue;
	typedef TPoint::TNumTraits TNumTraits;

    const size_t size = iPolygon.size();
	if (size < 2)
	{
		return iPolygon;
	}

    bool allOut = true;
    bool allIn = true;
	std::vector<T> e(size);
    for (size_t i = 0; i < size; ++i)
    {
        e[i] = iPlane.equation(iPolygon[i]);
		allOut &= (e[i] <= TNumTraits::zero);
        allIn &= (e[i] >= TNumTraits::zero);
    }

    if (allIn)
    {
        return iPolygon;
    }

    if (allOut)
    {
        return TPolygon(iPolygon.plane());
    }

    TPolygon result(iPolygon.plane());
    TPoint tail = iPolygon[size - 1];
    TValue tailE = e[size - 1];
	bool tailInside = tailE >= TNumTraits::zero;

    for (size_t i = 0; i < size; ++i)
    {
        const TPoint& head = iPolygon[i];
        const TValue headE = e[i];
        const bool headInside = headE >= TNumTraits::zero;

        if (tailInside)
        {
            if (headInside)
            {
                // both in, add this vertex
                result.add(head);
            }
            else
            {
                // going out, add intersection point
                const TValue t = tailE / (tailE - headE);
                LASS_ASSERT(0.0 <= t && t <= 1.0);
				const TPoint p = tail + t * (head - tail);
                result.add(p);
            }
        }
        else
        {
            if (headInside)
            {
                // coming in, add intersection point and this vertex.
                const TValue t = tailE / (tailE - headE);
                LASS_ASSERT(0.0 <= t && t <= 1.0);
				const TPoint p = tail + t * (head - tail);
                result.add(p);
                result.add(head);
            }
            else
            {
                // both out, do nothing.
            }
        }

        tail = head;
        tailE = headE;
        tailInside = headInside;
    }

    return result;
}       

/** @relates lass::prim::SimplePolygon3D
 */
template <typename T, class EP, class NP>
io::XmlOStream& operator<<(io::XmlOStream& ioOStream, const SimplePolygon3D<T, EP, NP>& iPolygon)
{
	const size_t n = iPolygon.size();
	LASS_ENFORCE_STREAM(ioOStream) << "<SimplePolygon3D>\n";
	for (size_t i = 0; i < n; ++i)
	{
		ioOStream << "<vertex id='" << static_cast<unsigned long>(i) << "'>" << iPolygon[i] << "</vertex>\n";
	}
	ioOStream << "<plane>" << iPolygon.plane() << "</plane>\n";
	ioOStream << "</SimplePolygon3D>\n";
	return ioOStream;
}



}

}

#endif

// EOF
