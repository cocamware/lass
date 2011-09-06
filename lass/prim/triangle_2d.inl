/**	@file
 *	@author Bram de Greve (bram@cocamware.com)
 *	@author Tom De Muer (tom@cocamware.com)
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
 *	Copyright (C) 2004-2010 the Initial Developer.
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



#ifndef LASS_GUARDIAN_OF_INCLUSION_PRIM_TRIANGLE_2D_INL
#define LASS_GUARDIAN_OF_INCLUSION_PRIM_TRIANGLE_2D_INL

#include "prim_common.h"
#include "triangle_2d.h"
#include "line_2d.h"

namespace lass
{
namespace prim
{

/** constructs an empty triangle.
 *  all vertices are (0, 0) and thus equal.
 */
template <typename T>
Triangle2D<T>::Triangle2D()
{
}



/** Constructs a triangle through three points in positive sequence.
 */
template <typename T>
Triangle2D<T>::Triangle2D(const TPoint& a, const TPoint& b, const TPoint& c)
{
	vertices_[0] = a;
	vertices_[1] = b;
	vertices_[2] = c;
}



/** @copydoc SimplePolygon2D::operator[]
 */
template <typename T>
const typename Triangle2D<T>::TPoint& Triangle2D<T>::operator[](size_t vertexIndex) const
{
	LASS_ASSERT(isInRange(vertexIndex));
	return vertices_[vertexIndex];
}



/** @copydoc SimplePolygon2D::operator[]
 */
template <typename T>
typename Triangle2D<T>::TPoint& Triangle2D<T>::operator[](size_t vertexIndex)
{
	LASS_ASSERT(isInRange(vertexIndex));
	return vertices_[vertexIndex];
}



/** @copydoc SimplePolygon2D::at
 */
template <typename T>
const typename Triangle2D<T>::TPoint& Triangle2D<T>::at(int vertexIndex) const
{
	const size_t i = num::mod(vertexIndex, static_cast<unsigned>(size_));
	LASS_ASSERT(isInRange(i));
	return vertices_[i];
}



/** @copydoc SimplePolygon2D::at
 */
template <typename T>
typename Triangle2D<T>::TPoint& Triangle2D<T>::at(int vertexIndex)
{
	const size_t i = num::mod(vertexIndex, static_cast<unsigned>(size_));
	LASS_ASSERT(isInRange(i));
	return vertices_[i];
}



/** @copydoc SimplePolygon2D::edge
 */
template <typename T>
const typename Triangle2D<T>::TLineSegment Triangle2D<T>::edge(int tailVertexIndex) const
{
	return TLineSegment(at(tailVertexIndex), at(tailVertexIndex + 1));
}



/** @copydoc SimplePolygon2D::vector
 */
template <typename T>
const typename Triangle2D<T>::TVector Triangle2D<T>::vector(int tailVertexIndex) const
{
	return at(tailVertexIndex + 1) - at(tailVertexIndex);
}



/** @copydoc SimplePolygon2D::isEmpty
 *  @par Triangle specific:
 *      if all vertices are equal, we assume the triangle is empty
 */
template <typename T>
bool Triangle2D<T>::isEmpty() const
{
	return vertices_[0] == vertices_[1] && vertices_[0] == vertices_[2];
}



/** @copydoc SimplePolygon2D::size
 */
template <typename T>
int Triangle2D<T>::size() const
{
	return size_;
}



/** @copydoc SimplePolygon2D::signedArea
 */
template <typename T>
const typename Triangle2D<T>::TValue Triangle2D<T>::signedArea() const
{
	LASS_ASSERT(size_ == 3);
	return perpDot(vertices_[1] - vertices_[0], vertices_[2] - vertices_[0]) / T(2);
}



/** @copydoc SimplePolygon2D::area
 */
template <typename T>
const typename Triangle2D<T>::TValue Triangle2D<T>::area() const
{
	return num::abs(signedArea());
}



/** @copydoc SimplePolygon2D::perimeter
 */
template <typename T>
const typename Triangle2D<T>::TValue Triangle2D<T>::perimeter() const
{
	return distance(vertices_[0], vertices_[1]) +
		   distance(vertices_[1], vertices_[2]) +
		   distance(vertices_[2], vertices_[0]);
}



/** @copydoc SimplePolygon2D::vertexCentroid
 */
template <typename T>
const typename Triangle2D<T>::TPointH
Triangle2D<T>::vertexCentroid() const
{
	TPointH result = vertices_[0] + vertices_[1] + vertices_[2];
	return result;
}



/** @copydoc SimplePolygon2D::vertexCentroid
 */
template <typename T> inline
const typename Triangle2D<T>::TPointH
Triangle2D<T>::surfaceCentroid() const
{
	return vertexCentroid();
}



/** @copydoc SimplePolygon2D::isSimple
 *  @par Triangle specific:
 *      A triangle always is simple
 */
template <typename T>
bool Triangle2D<T>::isSimple() const
{
	return true;
}



/** @copydoc SimplePolygon2D::isConvex
 *  @par Triangle specific:
 *      A triangle always is convex
 */
template <typename T>
bool Triangle2D<T>::isConvex() const
{
	return true;
}



/** @copydoc SimplePolygon2D::orientation
 */
template <typename T>
Orientation Triangle2D<T>::orientation() const
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



/** @copydoc SimplePolygon2D::isReflex
 *  @par Triangle specific:
 *      triangles never have reflex vertices, so always returns false.
 */
template <typename T>
bool Triangle2D<T>::isReflex(int vertexIndex) const
{
	return false;
}



/** return true when a point is inside or on the edge of a triangle.
 */
template <typename T>
bool Triangle2D<T>::contains(const TPoint& point) const
{
	return
		perpDot(vertices_[1] - vertices_[0], point - vertices_[0]) >= TNumTraits::zero &&
		perpDot(vertices_[2] - vertices_[1], point - vertices_[1]) >= TNumTraits::zero &&
		perpDot(vertices_[0] - vertices_[2], point - vertices_[2]) >= TNumTraits::zero;
}



template <typename T>
Side Triangle2D<T>::classify(const TPoint& point) const
{
	return contains(point) ? sInside : sOutside;
}



/** flip orientation of polygon.
 */
template <typename T>
void Triangle2D<T>::flip()
{
	std::swap(vertices_[0], vertices_[2]);
}



// --- private -------------------------------------------------------------------------------------

/** return if index of vertex is in range of the std::vector
 */
template <typename T>
bool Triangle2D<T>::isInRange(size_t vertexIndex) const
{
	return vertexIndex < size_;
}



// --- free ----------------------------------------------------------------------------------------

/** @relates lass::prim::Triangle2D
 */
template <typename T>
const T squaredDistance(const Triangle2D<T>& triangle, const Point2D<T>& point)
{
	typedef typename Triangle2D<T>::TPoint TPoint;
	typedef typename Triangle2D<T>::TVector TVector;
	typedef typename Triangle2D<T>::TValue TValue;
	typedef typename Triangle2D<T>::TNumTraits TNumTraits;

	TValue sqrBest = TNumTraits::infinity;
	for (int k1 = 0, k0 = 2; k1 < 3; k0 = k1++)
	{
		const TPoint& tail = triangle[k0];
		const TPoint& head = triangle[k1];
		const TVector vector = point - tail;
		const TVector edge = head - tail;
		const TValue t = dot(vector, edge);
		const TValue tMax = dot(edge, edge);
		if (t > 0 && t < tMax)
		{
			const TVector rejected = vector - edge * (t / tMax);
			sqrBest = std::min(sqrBest, rejected.squaredNorm());
		}
		sqrBest = std::min(sqrBest, vector.squaredNorm());
	}

	return sqrBest;
}



/** @relates lass::prim::Triangle2D
 */
template <typename T>
const T distance(const Triangle2D<T>& triangle, const Point2D<T>& point)
{
	return num::sqrt(squaredDistance(triangle, point));
}


namespace impl
{
	template <typename T>
	bool hasSeperatingAxis(const Triangle2D<T>& a, const Triangle2D<T>& b)
	{
		// applying seperating axis theorem, normal to each edge.
		typedef typename Triangle2D<T>::TVector TVector;
		typedef typename Triangle2D<T>::TPoint TPoint;

		size_t j = 1, k = 2;
		for (size_t i = 0; i < 3; ++i)
		{
			const TPoint& aTail = a[i];
			const TPoint& aHead = a[j];
			const TVector aEdge = aHead - aTail;

			const T b0 = perpDot(aEdge, b[0] - aTail);
			const T b1 = perpDot(aEdge, b[1] - aTail);
			const T b2 = perpDot(aEdge, b[2] - aTail);
			const T aRef = perpDot(aEdge, a[k] - aTail);

			if (aRef > 0)
			{
				if (b0 < 0 && b1 < 0 && b2 < 0 || b0 > aRef && b1 > aRef && b2 > aRef)
				{
					return true;
				}
			}
			else
			{
				if (b0 > 0 && b1 > 0 && b2 > 0 || b0 < aRef && b1 < aRef && b2 < aRef)
				{
					return true;
				}
			}

			j = k;
			k = i;
		}
		
		return false;
	}
}

/** @relates lass::prim::Triangle2D
 */
template <typename T>
bool intersects(const Triangle2D<T>& a, const Triangle2D<T>& b)
{
	return !(impl::hasSeperatingAxis(a, b) || impl::hasSeperatingAxis(b, a));
}



/** @relates lass::prim::Triangle2D
 */
template <typename T>
io::XmlOStream& operator<<(io::XmlOStream& ioOStream, const Triangle2D<T>& iTriangle)
{
	LASS_ENFORCE_STREAM(ioOStream) << "<Triangle2D>\n";
	for (unsigned i = 0; i < 3; ++i)
	{
		ioOStream << "<vertex id='" << i << "'>" << iTriangle[i] << "</vertex>\n";
	}
	ioOStream << "</Triangle2D>\n";
	return ioOStream;
}



/** @relates lass::prim::Triangle2D
 */
template <typename T>
std::ostream& operator<<(std::ostream& ioOStream, const Triangle2D<T>& iTriangle)
{
	LASS_ENFORCE_STREAM(ioOStream) 
		<< "{" << iTriangle[0] << ", " << iTriangle[1] << ", " << iTriangle[2] << "}";
	return ioOStream;
}

/** @relates lass::prim::Triangle2D
 */
template<typename T>
lass::io::MatlabOStream& operator<<(lass::io::MatlabOStream& oOStream,
									const Triangle2D<T>& iTriangle)
{
	LASS_ENFORCE_STREAM(oOStream) << "lasthandle = patch(";
	oOStream << "[" << iTriangle[0].x << "," << iTriangle[1].x << "," << iTriangle[2].x << "],";
	oOStream << "[" << iTriangle[0].y << "," << iTriangle[1].y << "," << iTriangle[2].y << "],";
	oOStream << oOStream.color() << ");\n";
	return oOStream;
}


/** @relates lass::prim::Triangle2D
*   Returns the surface of the partial Voronoi cell constructed around vertex 
*   vertexIndex (say vertex a in triangle abc).  Then the surface is determined by
*   the quad built by a, the two midpoints on ab and ac and the intersection of the two
*	perpendicular bisectors.
*/
template <typename T>
T partialVoronoiArea(const Triangle2D<T> iT, int vertexIndex)
{
	// compute the two midpoints
	typedef typename Triangle2D<T>::TPoint	TPoint;
	typedef typename Triangle2D<T>::TPointH	TPointH;
	typedef typename Triangle2D<T>::TVector	TVector;
	typedef Line2D<T> TLine;
	TPoint a = iT.at(vertexIndex);
	TPoint b = iT.at(vertexIndex+1);
	TPoint c = iT.at(vertexIndex-1);

	TPointH abh = a+b;
	TPointH ach = a+c;
	TPoint ab = abh.affine();
	TPoint ac = ach.affine();
	TLine pbisAb(ab, (b-a).perp());
	TLine pbisAc(ac, (c-a).perp());

	TPoint m;
	LASS_ENFORCE(rOne == intersect(pbisAb, pbisAc,m));

	Triangle2D<T> aAbm(a,ab,m);
	Triangle2D<T> amAc(a,m,ac);

	return aAbm.area()+amAc.area();
}

}

}

#endif

// EOF
