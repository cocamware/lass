/**	@file
 *	@author Bram de Greve (bramz@users.sourceforge.net)
 *	@author Tom De Muer (tomdemuer@users.sourceforge.net)
 *
 *	Distributed under the terms of the GPL (GNU Public License)
 *
 * 	The LASS License:
 *
 *	Copyright 2004 Bram de Greve and Tom De Muer
 *
 *	LASS is free software; you can redistribute it and/or modify
 *	it under the terms of the GNU General Public License as published by
 *	the Free Software Foundation; either version 2 of the License, or
 *	(at your option) any later version.
 *
 *	This program is distributed in the hope that it will be useful,
 *	but WITHOUT ANY WARRANTY; without even the implied warranty of
 *	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *	GNU General Public License for more details.
 *
 *	You should have received a copy of the GNU General Public License
 *	along with this program; if not, write to the Free Software
 *	Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */



/** @class lass::prim::Triangle3D
 *  @brief A very simple 3D polygon :)
 *  @author Bram de Greve [BdG]
 */

#ifndef LASS_GUARDIAN_OF_INCLUSION_PRIM_TRIANGLE_3D_H
#define LASS_GUARDIAN_OF_INCLUSION_PRIM_TRIANGLE_3D_H

#include "prim_common.h"
#include "line_segment_3d.h"


namespace lass
{
namespace prim
{

template <typename T>
class Triangle3D
{
public:

    typedef Triangle3D<T> TSelf;

	typedef Point3D<T> TPoint;
	typedef Point3DH<T> TPointH;
    typedef typename TPoint::TVector TVector;
	typedef LineSegment3D<T> TLineSegment;
	typedef Plane3D<T, Cartesian, Normalized> TPlane;
	
    typedef typename TPoint::TValue TValue;
    typedef typename TPoint::TParam TParam;
    typedef typename TPoint::TReference TReference;
    typedef typename TPoint::TConstReference TConstReference;
	typedef typename TPoint::TNumTraits TNumTraits;

    enum { dimension = TPoint::dimension };	/**< number of dimensions */

    template <typename U> struct Rebind
    {
        typedef Triangle3D<U> Type;
    };

	Triangle3D();
	Triangle3D(const TPoint& iA, const TPoint& iB, const TPoint& iC);

	const TPoint& operator[](int iIndexOfVertex) const;
	TPoint& operator[](int iIndexOfVertex);
	const TPoint& at(int iIndexOfVertex) const;
	TPoint& at(int iIndexOfVertex);
	const TLineSegment edge(int iIndexOfTailVertex) const;
	const TVector vector(int iIndexOfTailVertex) const;
	const TPlane plane() const;

	const bool isEmpty() const;
    const int size() const;

	const TValue squaredArea() const;
	const TValue area() const;
	const TValue perimeter() const;
	const TPointH center() const;

	const bool isSimple() const;
	const bool isConvex() const;
	const bool isConcave() const;

	const bool isReflex(int iIndexOfVertex) const;

private:

	const bool isInRange(int iIndexOfVertex) const;

    enum { size_ = 3 };
	
	TPoint vertices_[size_];
};

template <typename T, class DP>
io::XmlOStream& operator<<(io::XmlOStream& ioOStream, const Triangle3D<T>& iTriangle);



}

}

#include "triangle_3d.inl"

#endif

// EOF
