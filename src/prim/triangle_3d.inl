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



#ifndef LASS_GUARDIAN_OF_INCLUSION_PRIM_SIMPLE_TRIANGLE_3D_INL
#define LASS_GUARDIAN_OF_INCLUSION_PRIM_SIMPLE_TRIANGLE_3D_INL

#include "prim_common.h"
#include "triangle_3d.h"

namespace lass
{
namespace prim
{

/** constructs an empty triangle.
 *  all vertices are (0, 0) and thus equal.
 */
template <typename T>
Triangle3D<T>::Triangle3D()
{
}



/** Constructs a triangle through three points in positive sequence.
 */
template <typename T>
Triangle3D<T>::Triangle3D(const TPoint& iA, const TPoint& iB, const TPoint& iC)
{
	vertices_[0] = iA;
	vertices_[1] = iB;
	vertices_[2] = iC;
}



/** @copydoc SimplePolygon3D::operator[]
 */
template <typename T>
const typename Triangle3D<T>::TPoint&
Triangle3D<T>::operator[](int iIndexOfVertex) const
{
	LASS_ASSERT(isInRange(iIndexOfVertex));
	return vertices_[iIndexOfVertex];
}



/** @copydoc SimplePolygon3D::operator[]
 */
template <typename T>
typename Triangle3D<T>::TPoint&
Triangle3D<T>::operator[](int iIndexOfVertex)
{
	LASS_ASSERT(isInRange(iIndexOfVertex));
	return vertices_[iIndexOfVertex];
}



/** @copydoc SimplePolygon3D::at
 */
template <typename T>
const typename Triangle3D<T>::TPoint&
Triangle3D<T>::at(int iIndexOfVertex) const
{
	const int i = num::mod(iIndexOfVertex, static_cast<unsigned>(size_));
	LASS_ASSERT(isInRange(i));
	return vertices_[i];
}



/** @copydoc SimplePolygon3D::at
 */
template <typename T>
typename Triangle3D<T>::TPoint&
Triangle3D<T>::at(int iIndexOfVertex)
{
	const int i = num::mod(iIndexOfVertex, static_cast<unsigned>(size_));
	LASS_ASSERT(isInRange(i));
	return vertices_[i];
}



/** @copydoc SimplePolygon3D::edge
 */
template <typename T>
const typename Triangle3D<T>::TLineSegment
Triangle3D<T>::edge(int iIndexOfTailVertex) const
{
	return TLineSegment(at(iIndexOfTailVertex), at(iIndexOfTailVertex + 1));
}



/** @copydoc SimplePolygon3D::vector
 */
template <typename T>
const typename Triangle3D<T>::TVector
Triangle3D<T>::vector(int iIndexOfTailVertex) const
{
	return at(iIndexOfTailVertex + 1) - at(iIndexOfTailVertex);
}



/** @copydoc SimplePolygon3D::plane
 */
template <typename T>
const typename Triangle3D<T>::TPlane
Triangle3D<T>::plane() const
{
	return TPlane(vertices_[0], vertices_[1], vertices_[2]);
}



/** @copydoc SimplePolygon3D::isEmpty
 *  @par Triangle specific:
 *      if all vertices are equal, we assume the triangle is empty
 */
template <typename T>
const bool Triangle3D<T>::isEmpty() const
{
	return vertices_[0] == vertices_[1] && vertices_[0] == vertices_[2];
}



/** @copydoc SimplePolygon3D::size
 */
template <typename T>
const int Triangle3D<T>::size() const
{
	return size_;
}



/** returns squared area of triangle to avoid the square root.
 *  @return num::sqr(area()) but faster :)
 */
template <typename T>
const typename Triangle3D<T>::TValue
Triangle3D<T>::squaredArea() const
{
	LASS_ASSERT(size_ == 3);
	return cross(vertices_[1] - vertices_[0], vertices_[2] - vertices_[0]).squaredNorm() / 4;
}



/** @copydoc SimplePolygon3D::area
 */
template <typename T>
const typename Triangle3D<T>::TValue
Triangle3D<T>::area() const
{
	return num::sqrt(squaredNorm());
}



/** @copydoc SimplePolygon3D::perimeter
 */
template <typename T>
const typename Triangle3D<T>::TValue
Triangle3D<T>::perimeter() const
{
	return distance(vertices_[0], vertices_[1]) +
		   distance(vertices_[1], vertices_[2]) +
		   distance(vertices_[2], vertices_[0]);
}



/** @copydoc SimplePolygon3D::center
 */
template <typename T>
const typename Triangle3D<T>::TPointH
Triangle3D<T>::center() const
{
	TPointH result = vertices_[0] + vertices_[1] + vertices_[2];
	return result;
}



/** @copydoc SimplePolygon3D::isSimple
 *  @par Triangle specific:
 *      A triangle is always simple
 */
template <typename T>
const bool Triangle3D<T>::isSimple() const
{
	return true;
}



/** @copydoc SimplePolygon3D::isConvex
 *  @par Triangle specific:
 *      A triangle is always convex
 */
template <typename T>
const bool Triangle3D<T>::isConvex() const
{
	return true;
}



/** return true if inner angle of vertex is reflex (is > 180 degrees).
 *  @warning assumes polygon is simple
 *
 *  test if signedArea() and perdDot(...) have different sign.
 *  if one of them is zero, it will return false by default.
 */
template <typename T>
const bool Triangle3D<T>::isReflex(int iIndexOfVertex) const
{
	const TValue pd = perdDot(vector(iIndexOfVertex - 1), vector(iIndexOfVertex)); // Ax(-B) = BxA
	return signedArea() * pd < TNumTraits::zero;
}



// --- private -------------------------------------------------------------------------------------

/** return if index of vertex is in range of the std::vector
 */
template <typename T>
const bool Triangle3D<T>::isInRange(int iIndexOfVertex) const
{
	return iIndexOfVertex >= 0 && iIndexOfVertex < size_;
}



// --- free ----------------------------------------------------------------------------------------

/** @relates lass::prim::Triangle2D
 */
template <typename T>
io::XmlOStream& operator<<(io::XmlOStream& ioOStream, const Triangle3D<T>& iTriangle)
{
	LASS_ENFORCE_STREAM(ioOStream) << "<Triangle3D>\n";
	for (unsigned i = 0; i < 3; ++i)
	{
		ioOStream << "<vertex id='" << i << "'>" << iTriangle[i] << "</vertex>\n";
	}
	ioOStream << "</Triangle3D>\n";
	return ioOStream;
}



}

}

#endif

// EOF
