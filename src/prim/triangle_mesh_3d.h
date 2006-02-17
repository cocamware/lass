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



/** @class lass::prim::TriangleMesh3D
 *  @brief One of the simplier meshes
 *  @author Bram de Greve [BdG]
 */

#ifndef LASS_GUARDIAN_OF_INCLUSION_PRIM_TRIANGLE_MESH_3D_H
#define LASS_GUARDIAN_OF_INCLUSION_PRIM_TRIANGLE_MESH_3D_H

#include "prim_common.h"
#include "aabb_3d.h"
#include "point_2d.h"
#include "ray_3d.h"
#include "impl/intersect_triangle_3d.h"
#include "../stde/iterator_range.h"
#include "../num/impl/matrix_solve.h"

namespace lass
{
namespace prim
{

struct IndexTriangle
{
	std::size_t vertices[3];
	std::size_t normals[3];
	std::size_t uvs[3];

	static const std::size_t null() { return static_cast<std::size_t>(-1); }
};

template 
<
	typename T,
	template <typename T, typename ObjectTraits> class BoundingVolumeHierarchy
>
class TriangleMesh3D
{
public:

	typedef TriangleMesh3D<T, BoundingVolumeHierarchy> TSelf;

	typedef Point3D<T> TPoint;
	typedef typename TPoint::TVector TVector;
	typedef Point2D<T> TUv;
	typedef Aabb3D<T> TAabb;
	typedef Ray3D<T> TRay;

	typedef typename TPoint::TValue TValue;
	typedef typename TPoint::TParam TParam;
	typedef typename TPoint::TReference TReference;
	typedef typename TPoint::TConstReference TConstReference;
	typedef typename TPoint::TNumTraits TNumTraits;

	enum { dimension = TPoint::dimension };

	template <typename U> struct Rebind
	{
		typedef TriangleMesh3D<U, BoundingVolumeHierarchy> Type;
	};

	struct IntersectionContext
	{
		TVector normal;
		TVector dPoint_dU;
		TVector dPoint_dV;
		TVector dNormal_dU;
		TVector dNormal_dV;
		TVector geometricNormal;
		TUv uv;
	};

	struct Triangle
	{
		const TPoint* vertices[dimension];
		const TVector* normals[dimension];
		const TUv* uvs[dimension];

		const Result intersect(const TRay& iRay, TReference oT, TParam iMinT = 0,
			IntersectionContext* oContext = 0) const;
	};

	typedef IntersectionContext TIntersectionContext;
	typedef Triangle TTriangle;
	typedef IndexTriangle TIndexTriangle;
	typedef std::vector<Triangle> TTriangles;
	typedef std::vector<TPoint> TVertices;
	typedef std::vector<TVector> TNormals;
	typedef std::vector<TUv> TUvs;

	typedef typename TTriangles::const_iterator TTriangleIterator;
	typedef typename TVertices::const_iterator TVertexIterator;
	typedef typename TNormals::const_iterator TNormalIterator;
	typedef typename TUvs::const_iterator TUvIterator;

	template <typename VertexInputRange, typename NormalInputRange,
		typename UvInputRange, typename IndexTriangleInputRange>
	TriangleMesh3D(
		const VertexInputRange& iVertices, const NormalInputRange& iNormals,
		const UvInputRange& iUvs, const IndexTriangleInputRange& iTriangles);

	const TTriangles& triangles() const;
	const TVertices& vertices() const;
	const TNormals& normals() const;
	const TUvs& uvs() const;

	const TAabb aabb() const;
	const TValue area() const;

	void smoothNormals(TParam iMaxAngleInRadians);

	const Result intersect(const TRay& iRay, TTriangleIterator& oTriangle, TReference oT, 
		TParam iMinT = 0, IntersectionContext* oContext = 0) const;
	const bool intersects(const TRay& iRay, TParam iMinT, TParam iMaxT) const;
	
private:

	struct ObjectTraits
	{
		typedef typename TSelf::TTriangle TObject;
		typedef typename TSelf::TAabb TAabb;
		typedef typename TSelf::TRay TRay;

		typedef typename TSelf::TTriangleIterator TObjectIterator;
		typedef const TTriangle& TObjectReference;

		typedef typename TSelf::TPoint TPoint;
		typedef typename TSelf::TVector TVector;
		typedef typename TSelf::TValue TValue;
		typedef typename TSelf::TParam TParam;
		typedef typename TSelf::TReference TReference;
		typedef typename TSelf::TConstReference TConstReference;

		typedef void TInfo;
		
		enum { dimension = TSelf::dimension };

		static const TAabb aabb(TObjectIterator iObject) 
		{
			TAabb result;
			result += *iObject->vertices[0]; 
			result += *iObject->vertices[1];
			result += *iObject->vertices[2];
			return result;
		}

		static const bool intersect(TObjectIterator iObject, const TRay& iRay, 
			TReference oT, TParam iMinT, const TInfo* iInfo)
		{
			return iObject->intersect(iRay, oT, iMinT) == rOne;
		}

		static const bool intersects(TObjectIterator iObject, const TRay& iRay, 
			TParam iMinT, TParam iMaxT, const TInfo* iInfo)
		{
			TValue t;
			Result hit = iObject->intersect(iRay, t, iMinT);
			return hit == rOne && t < iMaxT;
		}

		static const bool contains(const TAabb& iAabb, const TPoint& iPoint) 
		{ 
			return iAabb.contains(iPoint); 
		}

		static const bool intersect(const TAabb& iAabb, const TRay& iRay, 
			TReference oT, const TParam iMinT)
		{
			return prim::intersect(iAabb, iRay, oT, iMinT) != prim::rNone;
		}

		static const TAabb join(const TAabb& iA, const TAabb& iB) { return iA + iB; }
		static const TPoint min(const TAabb& iAabb) { return iAabb.min(); }
		static const TPoint max(const TAabb& iAabb) { return iAabb.max(); }
		static const TPoint support(const TRay& iRay) {	return iRay.support(); }
		static const TVector direction(const TRay& iRay) { return iRay.direction();	}
		static const TValue coordinate(const TPoint& iPoint, size_t iAxis) { return iPoint[iAxis]; }
		static const TValue component(const TVector& iVector, size_t iAxis) { return iVector[iAxis]; }
		static const TVector reciprocal(const TVector& iVector) { return iVector.reciprocal(); }
	};

	typedef BoundingVolumeHierarchy<TTriangle, ObjectTraits> TTriangleTree;

	TTriangleTree tree_;
	TTriangles triangles_;
	TVertices vertices_;
	TNormals normals_;
	TUvs uvs_;
};

}

}

#include "triangle_mesh_3d.inl"

#define LASS_PRIM_PYOBJECT_UTIL_TRIANGLE_MESH_3D
#ifdef LASS_GUARDIAN_OF_INCLUSION_PRIM_PYOBJECT_UTIL_H
#	include "pyobject_util.h"
#endif

#endif

// EOF
