/** 
*   @author Bram de Greve (bramz@users.sourceforge.net)
*   @author Tom De Muer (tomdemuer@users.sourceforge.net)
*
*	Distributed under the terms of the GPL (GNU Public License)
*
* 	The LASS License:
*   
*	Copyright 2004 Bram de Greve and Tom De Muer
*
*   LASS is free software; you can redistribute it and/or modify
*   it under the terms of the GNU General Public License as published by
*   the Free Software Foundation; either version 2 of the License, or
*   (at your option) any later version.
*
*   This program is distributed in the hope that it will be useful,
*   but WITHOUT ANY WARRANTY; without even the implied warranty of
*   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
*   GNU General Public License for more details.
*
*   You should have received a copy of the GNU General Public License
*   along with this program; if not, write to the Free Software
*   Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
*


 *  @date 2003
 */

#ifndef LASS_GUARDIAN_OF_INCLUSION_PRIM_SIMPLE_POLYGON_3D_INL
#define LASS_GUARDIAN_OF_INCLUSION_PRIM_SIMPLE_POLYGON_3D_INL

#include "prim_common.h"
#include "simple_polygon_3d.h"

namespace lass
{
namespace prim
{

template <typename T>
SimplePolygon3D<T>::SimplePolygon3D(const TPlane& iPlane) : plane_(iPlane)
{
}

template <typename T>
SimplePolygon3D<T>::SimplePolygon3D(const TPoint& iA, const TPoint& iB, const TPoint& iC) :
plane_(iA,iB,iC)
{

}


/** return vertex of polygon by its index, not wrapped, no bounds check.
 */
template <typename T>
const typename SimplePolygon3D<T>::TPoint& SimplePolygon3D<T>::operator[](int iIndexOfVertex) const
{
	LASS_ASSERT(isInRange(iIndexOfVertex));
	return vertices_[iIndexOfVertex];
}



/** return vertex of polygon by its index, not wrapped, no bounds check.
 */
template <typename T>
typename SimplePolygon3D<T>::TPoint& SimplePolygon3D<T>::operator[](int iIndexOfVertex)
{
	LASS_ASSERT(isInRange(iIndexOfVertex));
	return vertices_[iIndexOfVertex];
}



/** return vertex of polygon by its index, but wrap around the bounds.
 *  this->at(-1) will return the same vertex as this->at(this->size() - 1);
 */
template <typename T>
const typename SimplePolygon3D<T>::TPoint& SimplePolygon3D<T>::at(int iIndexOfVertex) const
{
	const int i = num::mod(iIndexOfVertex, static_cast<unsigned>(size()));
	LASS_ASSERT(isInRange(i));
	return vertices_[i];
}



/** return vertex of polygon by its index, but wrap around the bounds.
 *  this->at(-1) will return the same vertex as this->at(this->size() - 1);
 */
template <typename T>
typename SimplePolygon3D<T>::TPoint& SimplePolygon3D<T>::at(int iIndexOfVertex)
{
	const int i = num::mod(iIndexOfVertex, static_cast<unsigned>(size()));
	LASS_ASSERT(isInRange(i));
	return vertices_[i];
}



/** return the edge of the polygon between vertices at(iIndex) and at(iIndex + 1).
 */
template <typename T>
const typename SimplePolygon3D<T>::TLineSegment SimplePolygon3D<T>::edge(int iIndexOfTailVertex) const
{
	return SimplePolygon3D<T>::TLineSegment(at(iIndexOfTailVertex), at(iIndexOfTailVertex + 1));
}



/** return the vector between vertices at(iIndex) and at(iIndex + 1)\
 */
template <typename T>
const typename SimplePolygon3D<T>::TVector SimplePolygon3D<T>::vector(int iIndexOfTailVertex) const
{
	return at(iIndexOfTailVertex + 1) - at(iIndexOfTailVertex);
}



/** return support plane of polygon.
 */
template <typename T>
const typename SimplePolygon3D<T>::TPlane& SimplePolygon3D<T>::plane() const
{
	return plane_;
}



/** access support plane of polygon.
 */
template <typename T>
typename SimplePolygon3D<T>::TPlane& SimplePolygon3D<T>::plane()
{
	return plane_;
}



/** return normal of plane
 */
template <typename T>
const typename SimplePolygon3D<T>::TVector SimplePolygon3D<T>::normal() const
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
template <typename T>
const XYZ SimplePolygon3D<T>::majorAxis() const
{
	return plane_.majorAxis();
}



/** add a point at the "end" of the vertex list
 */
template <typename T>
void SimplePolygon3D<T>::add(const TPoint& iVertex)
{
	vertices_.push_back(iVertex);
}


/** insert a vertex at iIndex (so it will sit before the current at(iIndex)).
 */
template <typename T>
void SimplePolygon3D<T>::insert(int iIndexOfVertex, const TPoint& iVertex)
{
	const int i = num::mod(iIndexOfVertex, static_cast<unsigned>(size()));
	LASS_ASSERT(isInRange(i));
	vertices_.insert(i, iVertex);
}



/** remove the vertex at(iIndex)
 */
template <typename T>
void SimplePolygon3D<T>::remove(int iIndexOfVertex)
{
	const int i = num::mod(iIndexOfVertex, static_cast<unsigned>(size()));
	LASS_ASSERT(isInRange(i));
	vertices_.remove(i);
}



/** return true if polygon has no vertices
 */
template <typename T>
const bool SimplePolygon3D<T>::isEmpty() const
{
	return vertices_.empty();
}



/** return number of vertices
 */
template <typename T>
const int SimplePolygon3D<T>::size() const
{
	return vertices_.size();
}



/** return signed polygon area.
 *
 *  <i>The area of a convex polygon is defined to be positive if the points are arranged in a 
 *  counterclockwise order, and negative if they are in clockwise order.</i>,
 *  http://mathworld.wolfram.com/PolygonArea.html
 */
template <typename T>
const typename SimplePolygon3D<T>::TValue SimplePolygon3D<T>::signedArea() const
{
	if (size() < 3)
	{
		return TNumTraits::zero;
	}

	TValue result = TNumTraits::zero;
	for (int i = 0; i < size(); ++i)
	{
		result += cross(at(i).position, at(i + 1).position);
	}
	return result / 2;
}



/** return area of the polygons surface.
 *
 *  <i>The area of a surface is the amount of material needed to "cover" it completely</i>,
 *  http://mathworld.wolfram.com/Area.html
 */
template <typename T>
const typename SimplePolygon3D<T>::TValue SimplePolygon3D<T>::area() const
{
	return num::abs(signedArea());
}



/** return sum of the lengths of all edges
 */
template <typename T>
const typename SimplePolygon3D<T>::TValue SimplePolygon3D<T>::perimeter() const
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
template <typename T>
const typename SimplePolygon3D<T>::TPointH SimplePolygon3D<T>::center() const
{
	TPointH result;;
	for (int i = 0; i < size(); ++i)
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
 *  In 3D, we test if the 2D mapping on the major axis is simple.
 *
 *  @warning this is a brute force test.  we simple test for all edges if they are not intersecting
 *           Hence, this is O(n^2).
 */
template <typename T>
const bool SimplePolygon3D<T>::isSimple() const
{
	return mapping(majorAxis()).isSimple();
}



/** return true if polygon is convex, false if not.
 *  @warning assumes polygon is simple
 *
 *  <i>A planar polygon is convex if it contains all the line segments connecting any pair of its 
 *  points. Thus, for example, a regular pentagon is convex, while an indented pentagon is not. 
 *  A planar polygon that is not convex is said to be a concave polygon</i>,
 *  http://mathworld.wolfram.com/ConvexPolygon.html.
 *
 *  A simple polygon is convex if all the cross products of adjacent edges will be the same sign 
 *  (we ignore zero signs, only + or - are taken in account), a concave polygon will have a mixture 
 *  of cross product signs. 
 *
 *  A polygon with less than three vertices is always convex.  A polygon with all colinear 
 *  vertices is considered convex (not very usefull maybe, but convex).
 */
template <typename T>
const bool SimplePolygon3D<T>::isConvex() const
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
template <typename T>
const Orientation SimplePolygon3D<T>::orientation() const
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
 *	test if signedArea() and perdDot(...) have different sign.
 *  if one of them is zero, it will return false by default.
 */
template <typename T>
const bool SimplePolygon3D<T>::isReflex(int iIndexOfVertex) const
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
template <typename T>
const SimplePolygon2D<T> SimplePolygon3D<T>::mapping(XYZ iAxis) const
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

template <typename T>
const Side SimplePolygon3D<T>::classify(const TPoint& iP) const
{
    return contains(iP) ? sInside : sOutside;
}


template <typename T>
const bool SimplePolygon3D<T>::contains(const TPoint& iP) const
{
	const XYZ x = majorAxis() + 1;
	const XYZ y = majorAxis() + 2;

	unsigned i, j;
    bool c = false;
	const unsigned npol = size();
    for (i = 0, j = npol-1; i < npol; j = ++i) 
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



// --- private -------------------------------------------------------------------------------------

/** return if index of vertex is in range of the std::vector
 */
template <typename T>
const bool SimplePolygon3D<T>::isInRange(int iIndexOfVertex) const
{
	return iIndexOfVertex >= 0 && iIndexOfVertex < vertices_.size();
}



// --- free ----------------------------------------------------------------------------------------

/** @relates lass::prim::SimplePolygon3D
 */
template <typename T>
io::XmlOStream& operator<<(io::XmlOStream& ioOStream, const SimplePolygon3D<T>& iPolygon)
{
	const unsigned n = iPolygon.size();
	LASS_ENFORCE_STREAM(ioOStream) << "<SimplePolygon3D>" << std::endl;
	for (unsigned i = 0; i < n; ++i)
	{
		ioOStream << "<vertex id='" << i << "'>" << iPolygon[i] << "</vertex>" << std::endl;
	}
	ioOStream << "<plane>" << iPolygon.plane() << "</plane>" << std::endl;
	ioOStream << "</SimplePolygon3D>" << std::endl;
	return ioOStream;
}



}

}

#endif

// EOF
