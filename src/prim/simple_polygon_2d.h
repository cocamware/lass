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
 *	Copyright (C) 2004-2009 the Initial Developer.
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

template <typename T, class DP>
lass::io::MatlabOStream& operator<<(lass::io::MatlabOStream& oOStream,
									const SimplePolygon2D<T, DP>& iPolygon);


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

#define LASS_PRIM_HAVE_PY_EXPORT_TRAITS_SIMPLE_POLYGON_2D
#ifdef LASS_GUARDIAN_OF_INCLUSION_UTIL_PYOBJECT_PLUS_H
#	include "pyobject_util.h"
#endif

#ifdef LASS_GUARDIAN_OF_INCLUSION_PRIM_AABB_2D_H
#	include "aabb_2d_simple_polygon_2d.h"
#endif

#ifdef LASS_GUARDIAN_OF_INCLUSION_PRIM_RAY_2D_H
#	include "ray_2d_simple_polygon_2d.h"
#endif

#endif

// EOF
