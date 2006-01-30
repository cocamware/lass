/** @file
 *  @author Bram de Greve (bramz@users.sourceforge.net)
 *  @author Tom De Muer (tomdemuer@users.sourceforge.net)
 *
 *  Distributed under the terms of the GPL (GNU Public License)
 *
 *  The LASS License:
 *
 *  Copyright 2004-2006 Bram de Greve and Tom De Muer
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



/** @class lass::prim::SimplePolygon2D
 *  @brief convex or concave polygon in 2D (not selfintersecting, no holes)
 *  @author Bram de Greve [BdG]
 *
 *  @warning SimplePolygon2D only ASSUMES it's simple.  there's no guarantee at any time.
 *           It's your own responsibility to keep it simple.  We do it this way because
 *           it's just to costly to check it at every access to the polygon.  However, we
 *           provide some methods to check it yourself.
 */

#ifndef LASS_GUARDIAN_OF_INCLUSION_PRIM_SIMPLE_POLYGON_2D_H
#define LASS_GUARDIAN_OF_INCLUSION_PRIM_SIMPLE_POLYGON_2D_H

#include "prim_common.h"
#include "degenerate_policy.h"
#include "orientation.h"
#include "line_segment_2d.h"

namespace lass
{
namespace prim
{

template
<
	typename T,
	class DegeneratePolicy = NoDegenerate
>
class SimplePolygon2D
{
public:

	typedef SimplePolygon2D<T, NoDegenerate> TSelf;
	typedef DegeneratePolicy TDegeneratePolicy;
	
	typedef Point2D<T> TPoint;
	typedef Point2DH<T> TPointH;
	typedef typename TPoint::TVector TVector;
	typedef LineSegment2D<T> TLineSegment;

	typedef typename TPoint::TValue TValue;
	typedef typename TPoint::TParam TParam;
	typedef typename TPoint::TReference TReference;
	typedef typename TPoint::TConstReference TConstReference;
	typedef typename TPoint::TNumTraits TNumTraits;

	enum { dimension = TPoint::dimension }; /**< number of dimensions */

	template <typename U> struct Rebind
	{
		typedef SimplePolygon2D<U, NoDegenerate> Type;
	};

	SimplePolygon2D();
	template <typename InputIterator> 
	SimplePolygon2D(InputIterator iFirstVertex, InputIterator iLastVertex);

	const TPoint& operator[](size_t iIndexOfVertex) const;
	TPoint& operator[](size_t iIndexOfVertex);
	const TPoint& at(int iIndexOfVertex) const;
	TPoint& at(int iIndexOfVertex);
	const TLineSegment edge(int iIndexOfTailVertex) const;
	const TVector vector(int iIndexOfTailVertex) const;

	void add(const TPoint& iVertex);
	void insert(int iIndexOfVertex, const TPoint& iVertex);
	void erase(int iIndexOfVertex);

	const bool isEmpty() const;
	const size_t size() const;

	const TValue signedArea() const;
	const TValue area() const;
	const TValue perimeter() const;
	const TPointH vertexCentroid() const;
	const TPointH surfaceCentroid() const;
    
	const bool isSimple() const;
	const bool isConvex() const;
	const Orientation orientation() const;

	const bool isReflex(int iIndexOfVertex) const;

	const Side classify(const TPoint& iP) const;
	const bool contains(const TPoint& iP) const;

	void flip();
	void fixDegenerate();
	const bool isValid() const;

private:

	const bool isInRange(int iIndexOfVertex) const;

	typedef std::vector<TPoint> TVertices;

	TVertices vertices_;
};



template <typename T, class DP>
io::XmlOStream& operator<<(io::XmlOStream& ioOStream, const SimplePolygon2D<T, DP>& iPolygon);

template <typename T, class DP>
std::ostream& operator<<(std::ostream& ioOStream, const SimplePolygon2D<T, DP>& iPolygon);

/** C = A \ B */
template <typename T, class DP>
bool set_difference(const SimplePolygon2D<T, DP>& iPolygonA,const SimplePolygon2D<T, DP>& iPolygonB, std::vector<SimplePolygon2D<T, DP> >& oPolygonsC);

/** C = A U B */
template <typename T, class DP>
bool set_union(const SimplePolygon2D<T, DP>& iPolygonA,const SimplePolygon2D<T, DP>& iPolygonB, std::vector<SimplePolygon2D<T, DP> >& oPolygonsC);

/** C = (A U B) \ (A \ B) \ (B \ A)  */
template <typename T, class DP>
bool set_intersect(const SimplePolygon2D<T, DP>& iPolygonA,const SimplePolygon2D<T, DP>& iPolygonB, std::vector<SimplePolygon2D<T, DP> >& oPolygonsC);


}

}

#include "simple_polygon_2d.inl"

#ifdef LASS_GUARDIAN_OF_INCLUSION_PRIM_AABB_2D_H
#	include "aabb_2d_simple_polygon_2d.h"
#endif

#ifdef LASS_GUARDIAN_OF_INCLUSION_PRIM_RAY_2D_H
#	include "ray_2d_simple_polygon_2d.h"
#endif

#endif

// EOF
