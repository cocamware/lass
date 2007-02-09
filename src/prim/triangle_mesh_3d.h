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

	const size_t size() const { return 3; }
	static const std::size_t null() { return static_cast<std::size_t>(-1); }
};

template 
<
	typename T,
	template <typename T, typename OT, typename SH> class BoundingVolumeHierarchy,
	typename SplitHeuristics
>
class TriangleMesh3D
{
public:

	typedef TriangleMesh3D<T, BoundingVolumeHierarchy, SplitHeuristics> TSelf;

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
		typedef TriangleMesh3D<U, BoundingVolumeHierarchy, SplitHeuristics> Type;
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
		const TPoint* vertices[3];
		const TVector* normals[3];
		const TUv* uvs[3];
		Triangle* others[3]; /**< triangle on other side of vertices k,k+1 */
		unsigned creaseLevel[3];   /**< crease level of side k,k+1 */

		const Result intersect(const TRay& ray, TReference t, TParam tMin = 0,
			IntersectionContext* context = 0) const;
		const size_t side(const TPoint* v) const;
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

	TriangleMesh3D();
	template <typename VertexInputRange, typename IndexTriangleInputRange>
	TriangleMesh3D(
		const VertexInputRange& vertices, const IndexTriangleInputRange& triangles);
	template <typename VertexInputRange, typename NormalInputRange,
		typename UvInputRange, typename IndexTriangleInputRange>
	TriangleMesh3D(
		const VertexInputRange& vertices, const NormalInputRange& normals,
		const UvInputRange& uvs, const IndexTriangleInputRange& triangles);

	const TTriangles& triangles() const;
	const TVertices& vertices() const;
	const TNormals& normals() const;
	const TUvs& uvs() const;
	template <typename OutputIterator> 
		OutputIterator indexTriangles(OutputIterator triangles) const;

	const TAabb aabb() const;
	const TValue area() const;

	void smoothNormals(TParam maxAngleInRadians);
	void flatFaces();
	void loopSubdivision(unsigned level);
	void autoSew();
	void autoCrease(unsigned level);

	const Result intersect(const TRay& ray, TTriangleIterator& triangle, TReference t, 
		TParam tMin = 0, IntersectionContext* context = 0) const;
	const bool intersects(const TRay& ray, TParam tMin, TParam tMax) const;

	void swap(TSelf& other);
	
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

		static const TAabb aabb(TObjectIterator object) 
		{
			TAabb result;
			result += *object->vertices[0]; 
			result += *object->vertices[1];
			result += *object->vertices[2];
			return result;
		}

		static const bool intersect(TObjectIterator object, const TRay& ray, 
			TReference t, TParam tMin, const TInfo* info)
		{
			return object->intersect(ray, t, tMin) == rOne;
		}

		static const bool intersects(TObjectIterator object, const TRay& ray, 
			TParam tMin, TParam tMax, const TInfo* info)
		{
			TValue t;
			Result hit = object->intersect(ray, t, tMin);
			return hit == rOne && t < tMax;
		}

		static const bool contains(const TAabb& aabb, const TPoint& point) 
		{ 
			return aabb.contains(point); 
		}

		static const bool intersect(const TAabb& aabb, const TRay& ray, 
			TReference t, const TParam tMin)
		{
			return prim::intersect(aabb, ray, t, tMin) != prim::rNone;
		}

		static const TAabb emptyAabb() { return TAabb(); }
		static const TAabb join(const TAabb& a, const TAabb& b) { return a + b; }
		static const TPoint min(const TAabb& aabb) { return aabb.min(); }
		static const TPoint max(const TAabb& aabb) { return aabb.max(); }
		static const TPoint support(const TRay& ray) {	return ray.support(); }
		static const TVector direction(const TRay& ray) { return ray.direction();	}
		static const TValue coordinate(const TPoint& point, size_t axis) { return point[axis]; }
		static const TValue component(const TVector& vector, size_t axis) { return vector[axis]; }
		static const TVector reciprocal(const TVector& vector) { return vector.reciprocal(); }
	};

	typedef BoundingVolumeHierarchy<TTriangle, ObjectTraits, SplitHeuristics> TTriangleTree;
	
	struct LogicalEdge
	{
		Triangle* triangle;
		const TPoint* tail;
		const TPoint* head;
		LogicalEdge(Triangle* triangle, const TPoint* tail, const TPoint* head): 
			triangle(triangle), tail(tail), head(head) {}
		bool operator<(const LogicalEdge& other) const
		{
			return tail < other.tail || (tail == other.tail && head < other.head);
		}
	};

	struct PositionalEdge
	{
		Triangle* triangle;
		size_t k1;
		size_t k2;
		PositionalEdge(Triangle* triangle, size_t k1, size_t k2): 
			triangle(triangle), k1(k1), k2(k2)
		{
			const TPoint& v1 = *triangle->vertices[k1];
			x_[ 0] = v1.x; 
			x_[ 1] = v1.y;
			x_[ 2] = v1.z;
			const TPoint& v2 = *triangle->vertices[k2];
			x_[ 3] = v2.x;
			x_[ 4] = v2.y;
			x_[ 5] = v2.z;
		}
		const bool operator<(const PositionalEdge& other) const
		{
			for (size_t i = 0; i < size_; ++i)
			{
				if (x_[i] < other.x_[i]) return true;
				if (x_[i] > other.x_[i]) return false;
			}
			return false;
		}
	private:
		enum { size_ = 6 };
		TValue x_[size_];
	};

	typedef std::vector<const Triangle*> TVertexTriangles;
	typedef std::vector<TPoint> TVertexRing;
	typedef std::vector<TVector> TNormalRing;
	typedef std::vector<TUv> TUvRing;

	template <typename IndexTriangleInputRange> 
	void buildMesh(const IndexTriangleInputRange& triangles);
	void connectTriangles();
	void findVertexTriangles(TVertexTriangles& vertexTriangles) const;
	void findVertexRing(const TPoint& vertex, const Triangle* vertexTriangle, 
			TVertexRing& ring, TVertexRing& creases, TNormalRing& normals, TUvRing& uvs) const;
	void subdivide();

	TTriangleTree tree_;
	TTriangles triangles_;
	TVertices vertices_;
	TNormals normals_;
	TUvs uvs_;
	unsigned numBoundaryEdges_;
};

}

}

#include "triangle_mesh_3d.inl"
#include "pyobject_util.inl"

#ifdef LASS_GUARDIAN_OF_INCLUSION_PRIM_HALF_EDGE_MESH_3D_H
#	include "half_edge_mesh_3d_triangle_mesh_3d.h"
#endif

#endif

// EOF
