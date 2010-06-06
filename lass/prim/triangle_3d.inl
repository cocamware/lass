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
Triangle3D<T>::operator[](size_t iIndexOfVertex) const
{
	LASS_ASSERT(isInRange(iIndexOfVertex));
	return vertices_[iIndexOfVertex];
}



/** @copydoc SimplePolygon3D::operator[]
 */
template <typename T>
typename Triangle3D<T>::TPoint&
Triangle3D<T>::operator[](size_t iIndexOfVertex)
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
bool Triangle3D<T>::isEmpty() const
{
	return vertices_[0] == vertices_[1] && vertices_[0] == vertices_[2];
}



/** @copydoc SimplePolygon3D::size
 */
template <typename T>
size_t Triangle3D<T>::size() const
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
	return num::sqrt(squaredArea());
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



/** @copydoc SimplePolygon3D::vertexCentroid
 */
template <typename T>
const typename Triangle3D<T>::TPointH
Triangle3D<T>::vertexCentroid() const
{
	TPointH result = vertices_[0] + vertices_[1] + vertices_[2];
	return result;
}



/** @copydoc SimplePolygon3D::surfaceCentroid
 */
template <typename T> inline
const typename Triangle3D<T>::TPointH
Triangle3D<T>::surfaceCentroid() const
{
	return vertexCentroid();
}



/** @copydoc SimplePolygon3D::isSimple
 *  @par Triangle specific:
 *      A triangle is always simple
 */
template <typename T>
bool Triangle3D<T>::isSimple() const
{
	return true;
}



/** @copydoc SimplePolygon3D::isConvex
 *  @par Triangle specific:
 *      A triangle is always convex
 */
template <typename T>
bool Triangle3D<T>::isConvex() const
{
	return true;
}



/** @copydoc SimplePolygon3D::isReflex
 *  @par Triangle specific:
 *	A triangle never has reflex vertices
 */
template <typename T>
bool Triangle3D<T>::isReflex(int iIndexOfVertex) const
{
	return false;
}



// --- private -------------------------------------------------------------------------------------

/** return if index of vertex is in range of the std::vector
 */
template <typename T>
bool Triangle3D<T>::isInRange(size_t iIndexOfVertex) const
{
	return iIndexOfVertex < size_;
}



// --- free ----------------------------------------------------------------------------------------

/** @relates lass::prim::Triangle3D
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



/** @relates lass::prim::Triangle3D
 */
template <typename T>
std::ostream& operator<<(std::ostream& ioOStream, const Triangle3D<T>& iTriangle)
{
	LASS_ENFORCE_STREAM(ioOStream) 
		<< "{" << iTriangle[0] << ", " << iTriangle[1] << ", " << iTriangle[2] << "}";
	return ioOStream;
}



}

}

#endif

// EOF
