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



/** @class lass::prim::SimplePolygon3D
 *  @brief convex or concave polygon in 3D (not selfintersecting, no holes)
 *  @author Bram de Greve [BdG]
 *
 *  @warning SimplePolygon3D only @e assumes it's simple.  there's no guarantee at any time.
 *           It's your own responsibility to keep it simple.  We do it this way because 
 *           it's just to costly to check it at every access to the polygon.  However, we
 *           provide some methods to check it yourself.
 *
 *  @warning also, SimplePolygon3D only @e assumes it's flat!  It's up to you to feed it with 
 *           vertices that are coplanar.  However ... We provide tools to "flatten" it.
 */

#ifndef LASS_GUARDIAN_OF_INCLUSION_PRIM_SIMPLE_POLYGON_3D_H
#define LASS_GUARDIAN_OF_INCLUSION_PRIM_SIMPLE_POLYGON_3D_H

#include "prim_common.h"
#include "line_segment_3d.h"
#include "orientation.h"
#include "plane_3d.h"
#include "simple_polygon_2d.h"
#include "xyz.h"

namespace lass
{
namespace prim
{

template <typename T>
class SimplePolygon3D
{
public:
    
    typedef SimplePolygon3D<T> TSelf;

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
        typedef SimplePolygon3D<U> Type;
    };

	SimplePolygon3D(const TPlane& iPlane);
	SimplePolygon3D(const TPoint& iA, const TPoint& iB, const TPoint& iC);

	const TPoint& operator[](int iIndexOfVertex) const;
	TPoint& operator[](int iIndexOfVertex);
	const TPoint& at(int iIndexOfVertex) const;
	TPoint& at(int iIndexOfVertex);
	const TLineSegment edge(int iIndexOfTailVertex) const;
	const TVector vector(int iIndexOfTailVertex) const;
	const TPlane& plane() const;
	TPlane& plane();
	const TVector normal() const;
	const XYZ majorAxis() const;

	void add(const TPoint& iVertex);
	void insert(int iIndexOfVertex, const TPoint& iVertex);
	void remove(int iIndexOfVertex);

	const bool isEmpty() const;
    const int size() const;

	const TValue signedArea() const;
	const TValue area() const;
	const TValue perimeter() const;
	const TPointH center() const;

	const bool isSimple() const;
	const bool isConvex() const;
	const bool isConcave() const;
	const Orientation orientation() const;

	const bool isReflex(int iIndexOfVertex) const;

	const SimplePolygon2D<T> mapping(XYZ iAxis) const;

	const Side classify(const TPoint& iP) const;
	const bool contains(const TPoint& iP) const;

	void flip();

private:

	const bool isInRange(int iIndexOfVertex) const;

	typedef std::vector<TPoint> TVertices;
	
	TVertices vertices_;
	TPlane plane_;
};

template <typename T>
io::XmlOStream& operator<<(io::XmlOStream& ioOStream, const SimplePolygon3D<T>& iPolygon);

}

}

#include "simple_polygon_3d.inl"

#endif

// EOF
