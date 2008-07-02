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

template 
<
	typename T,
	class PlaneEquationPolicy = Cartesian,
	class PlaneNormalizingPolicy = Normalized
>
class SimplePolygon3D
{
public:

	typedef SimplePolygon3D<T> TSelf;

	typedef Point3D<T> TPoint;
	typedef Point3DH<T> TPointH;
	typedef typename TPoint::TVector TVector;
	typedef LineSegment3D<T> TLineSegment;
	typedef Plane3D<T, PlaneEquationPolicy, PlaneNormalizingPolicy> TPlane;

	typedef typename TPoint::TValue TValue;
	typedef typename TPoint::TParam TParam;
	typedef typename TPoint::TReference TReference;
	typedef typename TPoint::TConstReference TConstReference;
	typedef typename TPoint::TNumTraits TNumTraits;

	enum { dimension = TPoint::dimension }; /**< number of dimensions */

	template <typename U> struct Rebind
	{
		typedef SimplePolygon3D<U, PlaneEquationPolicy, PlaneNormalizingPolicy> Type;
	};

	SimplePolygon3D(const TPlane& iPlane);
	SimplePolygon3D(const TPoint& iA, const TPoint& iB, const TPoint& iC);

	const TPoint& operator[](size_t iIndexOfVertex) const;
	TPoint& operator[](size_t iIndexOfVertex);
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
	const size_t size() const;

	const TValue signedArea() const;
	const TValue area() const;
	const TValue perimeter() const;
	const TPointH vertexCentroid() const;
	const TPointH surfaceCentroid() const;

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

template<typename T, class EP, class NP, class PP>
Result intersect(const SimplePolygon3D<T, EP, NP>& iPolygon, 
				 const LineSegment3D<T, PP>& iSegment, 
				 T& oT, const T& iMinT = 0);

template<typename T, class EP1, class NP1, class EP2, class NP2>
SimplePolygon3D<T, EP2, NP2> clip(const Plane3D<T, EP1, NP1>& iPlane, 
								  const SimplePolygon3D<T, EP2, NP2>& iPolygon);

template <typename T, class EP, class NP>
io::XmlOStream& operator<<(io::XmlOStream& ioOStream, const SimplePolygon3D<T, EP, NP>& iPolygon);

template <typename T, class EP, class NP>
std::ostream& operator<<(std::ostream& ioOStream, const SimplePolygon3D<T, EP, NP>& iPolygon);

}

}

#include "simple_polygon_3d.inl"

#define LASS_PRIM_HAVE_PY_EXPORT_TRAITS_SIMPLE_POLYGON_3D
#ifdef LASS_GUARDIAN_OF_INCLUSION_UTIL_PYOBJECT_PLUS_H
#	include "pyobject_util.h"
#endif

#ifdef LASS_GUARDIAN_OF_INCLUSION_PRIM_AABB_3D_H
#	include "aabb_3d_simple_polygon_3d.h"
#endif

#ifdef LASS_GUARDIAN_OF_INCLUSION_PRIM_RAY_3D_H
#	include "ray_3d_simple_polygon_3d.h"
#endif

#endif

// EOF
