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



#ifndef LASS_GUARDIAN_OF_INCLUSION_PRIM_SIMPLE_POLYGON_2D_INL
#define LASS_GUARDIAN_OF_INCLUSION_PRIM_SIMPLE_POLYGON_2D_INL

#include "prim_common.h"
#include "simple_polygon_2d.h"

namespace lass
{
namespace prim
{

// --- public --------------------------------------------------------------------------------------

template <typename T, class DP>
SimplePolygon2D<T, DP>::SimplePolygon2D():
	vertices_()
{
}



template <typename T, class DP>
template <typename InputIterator>
SimplePolygon2D<T, DP>::SimplePolygon2D(InputIterator iFirstVertex, InputIterator iLastVertex):
	vertices_(iFirstVertex, iLastVertex)
{
}



/** return vertex of polygon by its index, not wrapped, no bounds check.
 */
template <typename T, class DP>
const typename SimplePolygon2D<T, DP>::TPoint&
SimplePolygon2D<T, DP>::operator[](int iIndexOfVertex) const
{
	LASS_ASSERT(isInRange(iIndexOfVertex));
	return vertices_[iIndexOfVertex];
}



/** return vertex of polygon by its index, not wrapped, no bounds check.
 */
template <typename T, class DP>
typename SimplePolygon2D<T, DP>::TPoint&
SimplePolygon2D<T, DP>::operator[](int iIndexOfVertex)
{
	LASS_ASSERT(isInRange(iIndexOfVertex));
	return vertices_[iIndexOfVertex];
}



/** return vertex of polygon by its index, but wrap around the bounds.
 *  this->at(-1) will return the same vertex as this->at(this->size() - 1);
 *  @throw an exception is thrown if polygon is empty
 */
template <typename T, class DP>
const typename SimplePolygon2D<T, DP>::TPoint&
SimplePolygon2D<T, DP>::at(int iIndexOfVertex) const
{
	LASS_ENFORCE(!isEmpty());
	const int i = num::mod(iIndexOfVertex, static_cast<unsigned>(size()));
	LASS_ASSERT(isInRange(i));
	return vertices_[i];
}



/** return vertex of polygon by its index, but wrap around the bounds.
 *  this->at(-1) will return the same vertex as this->at(this->size() - 1);
 *  @throw an exception is thrown if polygon is empty
 */
template <typename T, class DP>
typename SimplePolygon2D<T, DP>::TPoint&
SimplePolygon2D<T, DP>::at(int iIndexOfVertex)
{
	LASS_ENFORCE(!isEmpty());
	const int i = num::mod(iIndexOfVertex, static_cast<unsigned>(size()));
	LASS_ASSERT(isInRange(i));

	return vertices_[i];
}



/** return the edge of the polygon between vertices at(iIndex) and at(iIndex + 1).
 *  @throw an exception is thrown if polygon has less than two vertices
 */
template <typename T, class DP>
const typename SimplePolygon2D<T, DP>::TLineSegment
SimplePolygon2D<T, DP>::edge(int iIndexOfTailVertex) const
{
	DP::enforceEdge(*this, iIndexOfTailVertex);
	return TLineSegment(at(iIndexOfTailVertex), at(iIndexOfTailVertex + 1));
}



/** return the vector between vertices at(iIndex) and at(iIndex + 1)\
 */
template <typename T, class DP>
const typename SimplePolygon2D<T, DP>::TVector
SimplePolygon2D<T, DP>::vector(int iIndexOfTailVertex) const
{
	DP::enforceEdge(*this, iIndexOfTailVertex);
	return at(iIndexOfTailVertex + 1) - at(iIndexOfTailVertex);
}



/** add a point at the "end" of the vertex list.
 *  this is almost the same as <tt>insert(0, iVertex)</tt> with the difference that
 *  <tt>add(iVertex)</tt> is also valid for empty polygons.
 */
template <typename T, class DP>
void SimplePolygon2D<T, DP>::add(const TPoint& iVertex)
{
	vertices_.push_back(iVertex);
}


/** insert a vertex at iIndex (so it will sit before the current at(iIndex)).
 */
template <typename T, class DP>
void SimplePolygon2D<T, DP>::insert(int iIndexOfVertex, const TPoint& iVertex)
{
	LASS_ENFORCE(!isEmpty());

	const int i = num::mod(iIndexOfVertex, static_cast<unsigned>(size()));
	LASS_ASSERT(isInRange(i));
	vertices_.insert(vertices_.begin() + i, iVertex);
}



/** remove the vertex at(iIndex)
 */
template <typename T, class DP>
void SimplePolygon2D<T, DP>::erase(int iIndexOfVertex)
{
	LASS_ENFORCE(!isEmpty());
	const int i = num::mod(iIndexOfVertex, static_cast<unsigned>(size()));
	LASS_ASSERT(isInRange(i));
	vertices_.erase(vertices_.begin() + i);
}



/** return true if polygon has no vertices
 */
template <typename T, class DP>
const bool SimplePolygon2D<T, DP>::isEmpty() const
{
	return vertices_.empty();
}



/** return number of vertices
 */
template <typename T, class DP>
const int SimplePolygon2D<T, DP>::size() const
{
	LASS_ASSERT(vertices_.size() >= 0);
	return static_cast<int>(vertices_.size());
}



/** return signed polygon area.
 *
 *  <i>The area of a convex polygon is defined to be positive if the points are arranged in a
 *  counterclockwise order, and negative if they are in clockwise order.</i>,
 *  http://mathworld.wolfram.com/PolygonArea.html
 *
 *  @warning polygon must be simple accoring degenerate policy.
 */
template <typename T, class DP>
const typename SimplePolygon2D<T, DP>::TValue
SimplePolygon2D<T, DP>::signedArea() const
{
	DP::enforceSimple(*this);

	if (size() < 3)
	{
		return TNumTraits::zero;
	}

	TValue result = TNumTraits::zero;
	const int n = size();
	for (int i = 0; i < n; ++i)
	{
		result += perpDot(at(i).position(), at(i + 1).position());
	}
	return result / 2;
}



/** return area of the polygons surface.
 *
 *  <i>The area of a surface is the amount of material needed to "cover" it completely</i>,
 *  http://mathworld.wolfram.com/Area.html
 *
 *  @warning polygon must be simple accoring @a DegeneratePolicy.
 */
template <typename T, class DP>
const typename SimplePolygon2D<T, DP>::TValue
SimplePolygon2D<T, DP>::area() const
{
	return num::abs(signedArea()); // DP::enforceSimple(*this);
}



/** return orientation of polygon.
 *
 *  @warning polygon must be simple accoring @a DegeneratePolicy.
 */
template <typename T, class DP>
const Orientation SimplePolygon2D<T, DP>::orientation() const
{
	const TValue signArea = signedArea(); // DP::enforceSimple(*this);
	LASS_PRIM_ENFORCE_NO_DEGENERATE(DP, signArea != TNumTraits::zero);

	return signArea == TNumTraits::zero ? oInvalid :
		(signArea < TNumTraits::zero ? oClockWise : oCounterClockWise);
}



/** return sum of the lengths of all edges
 */
template <typename T, class DP>
const typename SimplePolygon2D<T, DP>::TValue
SimplePolygon2D<T, DP>::perimeter() const
{
	TValue result = TNumTraits::zero;
	const int n = size();
	for (int i = 0; i < n; ++i)
	{
		result += distance(at(i), at(i + 1));
	}
	return result;
}



/** return the barycenter of all vertices.
 *  The barycenter is the homogenous sum of all vertices.
 *
 *  @warning for non-convex polygons, it's NOT guaranteed that this center is inside the polygon.
 */
template <typename T, class DP>
const typename SimplePolygon2D<T, DP>::TPointH
SimplePolygon2D<T, DP>::center() const
{
	TPointH result;;
	const int n = size();
	for (int i = 0; i < n; ++i)
	{
		result += vertices_[i];
	}
	return result;
}


/** return true if polygon is simple, false if not.
 *
 *  <i>A polygon P is said to be simple (or Jordan) if the only points of the plane belonging to
 *  two polygon edges of P are the polygon vertices of P. Such a polygon has a well defined
 *  interior and exterior. Simple polygons are topologically equivalent to a disk.</i>,
 *  http://mathworld.wolfram.com/SimplePolygon.html.
 *
 *  A polygon with less than four vertices is always simple.
 *
 *  @warning this is a brute force test.  we simple test for all edges if they are not intersecting
 *           Hence, this is O(n^2).
 */
template <typename T, class DP>
const bool SimplePolygon2D<T, DP>::isSimple() const
{
	const int n = size();
	if (n < 4)
	{
		return true;
	}

	for (int i = 0; i < n; ++i)
	{
		const TLineSegment e = edge(i);
		TValue t1;
		TValue t2;
		if (intersect(e, edge(i + 1), t1, t2) != rOne)
		{
			return false;
		}
		for (int j = i + 2; j < n - 1; ++j)
		{
			if (intersect(e, edge(j), t1, t2) != rNone)
			{
				return false;
			}
		}
		if (i != 0 && intersect(e, edge(n - 1), t1, t2) != rOne)
		{
			return false;
		}
	}

	return true;
}



/** return true if polygon is convex, false if not.
 *
 *  <i>A planar polygon is convex if it contains all the line segments connecting any pair of its
 *  points. Thus, for example, a regular pentagon is convex, while an indented pentagon is not.
 *  A planar polygon that is not convex is said to be a concave polygon</i>,
 *  http://mathworld.wolfram.com/ConvexPolygon.html.
 *
 *  A simple polygon is convex if all the cross products of adjacent edges will be the same sign
 *  (we ignore zero cross products of colinear edges, only + or - are taken in account), a concave polygon
 *  will have a mixture of cross product signs.
 *
 *  A polygon with less than three vertices is always convex.  A polygon with all coincident.
 *  vertices is considered convex if DegeneratePolicy allows this.
 *
 *  @warning polygon must be simple and should not have coincident vertices, according
 *           @a DegeneratePolicy.
 */
template <typename T, class DP>
const bool SimplePolygon2D<T, DP>::isConvex() const
{
	DP::enforceSimple(*this);

	if (size() < 3)
	{
		return true;
	}

	TValue sign = TNumTraits::zero;
	const int n = size();
	for (int i = 0; i < n; ++i)
	{
		const TValue s = num::sign(perpDot(vector(i - 1), vector(i))); // Ax(-B) = BxA
		if (sign != s && s != TNumTraits::zero)
		{
			if (sign == TNumTraits::zero)
			{
				sign = s;
			}
			else
			{
				return false;
			}
		}
	}

	return true;
}



/** return true if inner angle of vertex is reflex (is > 180 degrees).
 *
 *  <i>Reflect Angle: An angle more than 180°</i>,
 *  http://mathworld.wolfram.com/ReflexAngle.html
 *
 *  test if signedArea() and perdDot(...) have different sign.
 *  if one of them is zero, it will return false by default.
 *
 *  @warning polygon must be simple accoring @a DegeneratePolicy.
 */
template <typename T, class DP>
const bool SimplePolygon2D<T, DP>::isReflex(int iIndexOfVertex) const
{
	DP::enforceSimple(*this);

	const TValue pd = perpDot(vector(iIndexOfVertex - 1), vector(iIndexOfVertex)); // Ax(-B) = BxA
	LASS_ASSERT(!isEmpty()); // vector(i) should enforce this
	return signedArea() * pd < TNumTraits::zero;
}



/** return true when a point is inside or on a polygon.  when a point lies
 *  on a polygon edge the answer can either be true or false but in a way that
 *  for a meshing the answer will only be true for one polygon.  More precise:
 *  for polygons sharing an edge only one of them will return true for a point
 *  on the edge.
 *  For an explanation of how this exactly works:
 *  http://www.ecse.rpi.edu/Homepages/wrf/geom/pnpoly.html (Wm Randolph Franklin)
 *
 */
template <typename T, class DP>
const bool SimplePolygon2D<T, DP>::contains(const TPoint& iP) const
{
	unsigned i, j;
	const TVector& p = iP.position();
	bool c = false;
	const unsigned npol = size();
	for (i = 0, j = npol-1; i < npol; j = i++)
	{
		const TVector& a = vertices_[i].position();
		const TVector& b = vertices_[j].position();
		if (((a.y <= p.y && p.y < b.y) || (b.y <= p.y && p.y < a.y)) &&
			p.x < (b.x - a.x) * (p.y - a.y) / (b.y - a.y) + a.x)
		{
			c = !c;
		}
	}
	return c;
}



template <typename T, class DP>
const Side SimplePolygon2D<T, DP>::classify(const TPoint& iP) const
{
	return contains(iP) ? sInside : sOutside;
}



/** flip orientation of polygon.
 */
template <typename T, class DP>
void SimplePolygon2D<T, DP>::flip()
{
	std::reverse(vertices_.begin(), vertices_.end());
}



/** fixes degenerate polygons as far as possible.
 *
 *  things that can be repared are:
 *  - coincident vertices: if two or more adjacent vertices are coincident, they can be reduced to
 *                         one vertex.
 *  - colinear edges: if two or more adjacent edges are colinear, they can be merged to one edge.
 *  - zero area: if a polygon has no area, this is pretty much the same as an @a empty polygon.
 *               All vertices will be removed.
 *
 *  Things that can't repared (and will cause an exception to be thrown) are:
 *  - non-simple polygons: there's no way we can repare polygons that are not simple, so we don't
 *                         even try to!  An exception is thrown regardless the @a DegeneratePolicy.
 */
template <typename T, class DP>
void SimplePolygon2D<T, DP>::fixDegenerate()
{
	// remove coincident vertices
	//
	int i = 0;
	while (i < size())
	{
		if (at(i) == at(i + 1))
		{
			erase(i);
		}
		else
		{
			++i;
		}
	}

	// merge colinear edges
	//
	while (size() > 2 && i < size())
	{
		if (perpDot(vector(i - 1), vector(i)) == TNumTraits::zero)
		{
			erase(i);
		}
		else
		{
			++i;
		}
	}

	// by now there are no coincident points that can trigger policy predicates in @c isSimple()
	// so exceptions are not thrown before we get the change to fix it.
	//
	LASS_ENFORCE(isSimple());

	// check zero area of polygons in two ways: literally, and by the knowledge that less than
	// three vertices is zero area for sure.
	//
	if (size() < 3 || signedArea() == TNumTraits::zero)
	{
		vertices_.clear();
	}
}



/** a simple polygon is valid if it is not degenerate.
 */
template <typename T, class DP>
const bool SimplePolygon2D<T, DP>::isValid() const
{
	return size() >= 3 && isSimple() && signedArea() != TNumTraits::zero;
}



// --- protected -----------------------------------------------------------------------------------



// --- private -------------------------------------------------------------------------------------

/** return if index of vertex is in range of the std::vector
 */
template <typename T, class DP>
const bool SimplePolygon2D<T, DP>::isInRange(int iIndexOfVertex) const
{
	return iIndexOfVertex >= 0 && iIndexOfVertex < size();
}



// --- free ----------------------------------------------------------------------------------------

/** @relates lass::prim::SimplePolygon2D
 */
template <typename T, class DP>
io::XmlOStream& operator<<(io::XmlOStream& ioOStream, const SimplePolygon2D<T, DP>& iPolygon)
{
	const unsigned n = iPolygon.size();
	LASS_ENFORCE_STREAM(ioOStream) << "<SimplePolygon2D>\n";
	for (unsigned i = 0; i < n; ++i)
	{
		ioOStream << "<vertex id='" << i << "'>" << iPolygon[i] << "</vertex>\n";
	}
	ioOStream << "</SimplePolygon2D>\n";
	return ioOStream;
}



}

}

#endif

// EOF
