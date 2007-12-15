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
SimplePolygon2D<T, DP>::operator[](size_t iIndexOfVertex) const
{
	LASS_ASSERT(iIndexOfVertex < vertices_.size());
	return vertices_[iIndexOfVertex];
}



/** return vertex of polygon by its index, not wrapped, no bounds check.
 */
template <typename T, class DP>
typename SimplePolygon2D<T, DP>::TPoint&
SimplePolygon2D<T, DP>::operator[](size_t iIndexOfVertex)
{
	LASS_ASSERT(iIndexOfVertex < vertices_.size());
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
	const int i = num::mod(iIndexOfVertex, static_cast<unsigned>(vertices_.size()));
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
	const int i = num::mod(iIndexOfVertex, static_cast<unsigned>(vertices_.size()));
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

	const int i = num::mod(iIndexOfVertex, static_cast<unsigned>(vertices_.size()));
	LASS_ASSERT(isInRange(i));
	vertices_.insert(vertices_.begin() + i, iVertex);
}



/** remove the vertex at(iIndex)
 */
template <typename T, class DP>
void SimplePolygon2D<T, DP>::erase(int iIndexOfVertex)
{
	LASS_ENFORCE(!isEmpty());
	const int i = num::mod(iIndexOfVertex, static_cast<unsigned>(vertices_.size()));
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
const size_t SimplePolygon2D<T, DP>::size() const
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
	const size_t n = size();
	for (size_t prevI = n - 1, i = 0; i < n; prevI = i++)
	{
		result += perpDot(vertices_[prevI].position(), vertices_[i].position());
	}
	return result / T(2);
}



/** return area of the polygons surface.
 *
 *  <i>The area of a surface is the amount of material needed to "cover" it completely</i>,
 *  Eric W. Weisstein. "Area." From MathWorld--A Wolfram Web Resource.
 *	http://mathworld.wolfram.com/Area.html
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
	const TValue signArea = TDegeneratePolicy::enforceNonZeroSignedArea(*this); // DP::enforceSimple(*this);
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
	const size_t n = size();
	for (size_t prevI = n - 1, i = 0; i < n; prevI = i++)
	{
		result += distance(vertices_[prevI], vertices_[i]);
	}
	return result;
}



/** return the barycenter of all vertices.
 *  The vertex centroid is the homogenous sum of all vertices.
 *
 *  @warning for non-convex polygons, it's NOT guaranteed that this center is inside the polygon.
 */
template <typename T, class DP>
const typename SimplePolygon2D<T, DP>::TPointH
SimplePolygon2D<T, DP>::vertexCentroid() const
{
	TPointH result;
	const size_t n = size();
	for (size_t i = 0; i < n; ++i)
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
template <typename T, class DP>
const typename SimplePolygon2D<T, DP>::TPointH
SimplePolygon2D<T, DP>::surfaceCentroid() const
{
	const size_t n = size();
	if (n < 3)
	{
		return vertexCentroid();
	}

	TPointH result;
	for (size_t prevI = n - 1, i = 0; i < n; prevI = i++)
	{
		const TValue triangleWeight = perpDot(vertices_[prevI].position(), vertices_[i].position());
		const TPointH triangleCenter = vertices_[prevI] + vertices_[i] + TPoint();
		result += triangleWeight * triangleCenter;
	}
	return result;
}



/** return true if polygon is simple, false if not.
 *
 *  <i>A polygon P is said to be simple (or Jordan) if the only points of the plane belonging to
 *  two polygon edges of P are the polygon vertices of P. Such a polygon has a well defined
 *  interior and exterior. Simple polygons are topologically equivalent to a disk.</i>,
 *  Eric W. Weisstein. "Simple Polygon." From MathWorld--A Wolfram Web Resource. 
 *  http://mathworld.wolfram.com/SimplePolygon.html 
 *
 *  A polygon with less than four vertices is always simple.
 *
 *  @warning this is a brute force test.  we simple test for all edges if they are not intersecting
 *           Hence, this is O(n^2).
 */
template <typename T, class DP>
const bool SimplePolygon2D<T, DP>::isSimple() const
{
	const int n = static_cast<int>(size());
	LASS_ASSERT(n >= 0);
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
 *  Eric W. Weisstein. "Convex Polygon." From MathWorld--A Wolfram Web Resource. 
 *  http://mathworld.wolfram.com/ConvexPolygon.html 
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

	const int n = static_cast<int>(size());
	LASS_ASSERT(n >= 0);
	if (n < 3)
	{
		return true;
	}

	TValue sign = TNumTraits::zero;
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
 *  Eric W. Weisstein. "Reflex Angle." From MathWorld--A Wolfram Web Resource. 
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



/** return true when a point is inside or on a polygon.
 *
 *	When a point lies on a polygon edge the answer can either be true or false but in a way that
 *  for a meshing the answer will only be true for one polygon.  More precise: for polygons sharing
 *	an edge only one of them will return true for a point on the edge.
 *  For an explanation of how this exactly works:
 *  http://www.ecse.rpi.edu/Homepages/wrf/geom/pnpoly.html (Wm Randolph Franklin)
 *
 *  @par Algorithm:
 *  comp.graphics.algorithms Frequently Asked Questions: 
 *	Subject 2.03: "How do I find if a point lies within a polygon?"
 *  http://www.faqs.org/faqs/graphics/algorithms-faq/
 */
template <typename T, class DP>
const bool SimplePolygon2D<T, DP>::contains(const TPoint& iP) const
{
	size_t i, j;
	const TVector& p = iP.position();
	bool c = false;
	const size_t npol = size();
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
	LASS_ASSERT(static_cast<int>(size()) >= 0);
	return iIndexOfVertex >= 0 && iIndexOfVertex < static_cast<int>(size());
}



// --- free ----------------------------------------------------------------------------------------

/** @relates lass::prim::SimplePolygon2D
 */
template <typename T, class DP>
io::XmlOStream& operator<<(io::XmlOStream& ioOStream, const SimplePolygon2D<T, DP>& iPolygon)
{
	const size_t n = iPolygon.size();
	LASS_ENFORCE_STREAM(ioOStream) << "<SimplePolygon2D>\n";
	for (size_t i = 0; i < n; ++i)
	{
		ioOStream << "<vertex id='" << static_cast<unsigned long>(i) << "'>" << iPolygon[i] << "</vertex>\n";
	}
	ioOStream << "</SimplePolygon2D>\n";
	return ioOStream;
}


/** @relates lass::prim::SimplePolygon2D
 */
template <typename T, class DP>
std::ostream& operator<<(std::ostream& ioOStream, const SimplePolygon2D<T, DP>& iPolygon)
{
	const size_t n = iPolygon.size();
	LASS_ENFORCE_STREAM(ioOStream) << "{";
	if (n > 0)
	{
		ioOStream << iPolygon[0];
	}	
	for (size_t i = 1; i < n; ++i)
	{
		ioOStream << ", " << iPolygon[i];
	}
	ioOStream << "}";
	return ioOStream;
}

/** @relates lass::prim::SimplePolygon2D
 */
template <typename T, class DP>
lass::io::MatlabOStream& operator<<(lass::io::MatlabOStream& oOStream,
									const SimplePolygon2D<T, DP>& iPolygon)
{
	LASS_ENFORCE_STREAM(oOStream) << "lasthandle = patch(";
	oOStream << "[" << iPolygon[0].x;
	for (size_t i=1;i<iPolygon.size();++i)
		oOStream << "," << iPolygon[i].x;
	oOStream << "],";
	oOStream << "[" << iPolygon[0].y;
	for (size_t i=1;i<iPolygon.size();++i)
		oOStream << "," << iPolygon[i].y;
	oOStream << "],";
	oOStream << "'Color'," << oOStream.color() << ");\n";
	return oOStream;
}


}

}

#endif

// EOF
