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
#include "triangle_3d.h"
#include "impl/intersect_triangle_3d.h"
#include "../stde/iterator_range.h"
#include "../num/impl/matrix_solve.h"
#include "../spat/default_object_traits.h"
#include "../spat/split_heuristics.h"

namespace lass
{
namespace prim
{

struct IndexTriangle
{
	size_t vertices[3];
	size_t normals[3];
	size_t uvs[3];

	size_t size() const { return 3; }
	static size_t null() { return static_cast<size_t>(-1); }
};

LASS_DLL bool LASS_CALL operator==(const IndexTriangle& a, const IndexTriangle& b);

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
		typedef typename TPoint::TVector TVector;

		const TPoint* vertices[3];
		const TVector* normals[3];
		const TUv* uvs[3];
		Triangle* others[3]; /**< triangle on other side of vertices k,k+1 */
		unsigned creaseLevel[3];   /**< crease level of side k,k+1 */
		size_t attribute;

		Result intersect(const TRay& ray, TReference t, TParam tMin = 0, IntersectionContext* context = 0) const;
		size_t side(const TPoint* v) const;
	};

	typedef IntersectionContext TIntersectionContext;
	typedef Triangle TTriangle;
	typedef IndexTriangle TIndexTriangle;

	// we need to use std::vector as we're going to rely on some pointer arithmetic ...
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
	TriangleMesh3D(const VertexInputRange& vertices, const IndexTriangleInputRange& triangles);
	template <typename VertexInputRange, typename NormalInputRange, typename UvInputRange, typename IndexTriangleInputRange>
	TriangleMesh3D(const VertexInputRange& vertices, const NormalInputRange& normals, const UvInputRange& uvs, const IndexTriangleInputRange& triangles);
	TriangleMesh3D(const TriangleMesh3D& other);

	TriangleMesh3D& operator=(const TriangleMesh3D& other);

	const TTriangles& triangles() const;
	const TVertices& vertices() const;
	const TNormals& normals() const;
	const TUvs& uvs() const;
	template <typename OutputIterator> 
	OutputIterator indexTriangles(OutputIterator triangles) const;

	const TAabb aabb() const;
	const TValue area() const;

	void smoothNormals();
	void flatFaces();
	void loopSubdivision(unsigned level);
	void autoSew();
	void autoCrease(unsigned level);
	void autoCrease(unsigned level, TParam maxAngleInRadians);

	Result intersect(const TRay& ray, TTriangleIterator& triangle, TReference t, TParam tMin = 0, IntersectionContext* context = 0) const;
	bool intersects(const TRay& ray, TParam tMin, TParam tMax) const;

	void swap(TSelf& other);
	
private:

	typedef spat::DefaultAabbRayTraits<TAabb, TRay> TAabbRayTraits;
	
	struct TriangleTraits: public TAabbRayTraits
	{
		typedef typename TSelf::TTriangle TObject;
		typedef typename TSelf::TTriangleIterator TObjectIterator;
		typedef const TTriangle& TObjectReference;
		typedef void TInfo;
		
		typedef typename TAabbRayTraits::TAabb TAabb;
		typedef typename TAabbRayTraits::TRay TRay;
		typedef typename TAabbRayTraits::TPoint TPoint;
		typedef typename TAabbRayTraits::TVector TVector;
		typedef typename TAabbRayTraits::TValue TValue;
		typedef typename TAabbRayTraits::TParam TParam;
		typedef typename TAabbRayTraits::TReference TReference;
		typedef typename TAabbRayTraits::TConstReference TConstReference;
	
		static const TAabb objectAabb(TObjectIterator triangle) 
		{
			TAabb result;
			result += *(triangle->vertices[0]); 
			result += *(triangle->vertices[1]);
			result += *(triangle->vertices[2]);
			return result;
		}
		static bool objectIntersect(TObjectIterator triangle, const TRay& ray, TReference t, TParam tMin, const TInfo*)
		{
			return triangle->intersect(ray, t, tMin) == rOne;
		}
		static bool objectIntersects(TObjectIterator triangle, const TRay& ray, TParam tMin, TParam tMax, const TInfo*)
		{
			TValue t;
			const Result hit = triangle->intersect(ray, t, tMin);
			return hit == rOne && t < tMax;
		}
		static bool objectIntersects(TObjectIterator triangle, const TAabb& aabb, const TInfo*)
		{
			const prim::Triangle3D<T> temp(
				*(triangle->vertices[0]), 
				*(triangle->vertices[1]),
				*(triangle->vertices[2]));
			return prim::intersects(temp, aabb);
		}
	};

	typedef BoundingVolumeHierarchy<TTriangle, TriangleTraits, SplitHeuristics> TTriangleTree;
	
	struct LogicalEdge
	{
		Triangle* triangle;
		const TPoint* tail;
		const TPoint* head;
		LogicalEdge(Triangle* triangle, const TPoint* tail, const TPoint* head): 
			triangle(triangle), tail(tail), head(head) {}
		bool operator==(const LogicalEdge& other) const
		{
			return tail == other.tail && head == other.head;
		}
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
		bool operator<(const PositionalEdge& other) const
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

	class HalfEdge
	{
	public:
		typedef typename TPoint::TVector TVector;

		HalfEdge(Triangle* triangle, size_t edge): triangle_(triangle), edge_(edge) { LASS_ASSERT(edge < 3); }
		Triangle* triangle() const { return triangle_; }
		size_t edge() const { return edge_; }

		const TPoint* vertex() const { return triangle_->vertices[edge_]; }
		const TVector* normal() const { return triangle_->normals[edge_]; }
		unsigned creaseLevel() const { return triangle_->creaseLevel[edge_]; }
		void setNormal(const TVector* normal) { triangle_->normals[edge_] = normal; }
		const TVector vector() const { return *oNext().vertex() - *vertex(); }

		HalfEdge oPrev() const { return HalfEdge(triangle_, (edge_ + 2) % 3); }
		HalfEdge oNext() const { return HalfEdge(triangle_, (edge_ + 1) % 3); }
		HalfEdge rPrev() const { return sym().oNext(); }
		HalfEdge rNext() const { return oPrev().sym(); }
		HalfEdge sym() const 
		{
			Triangle* other = triangle_->others[edge_];
			if (!other || other->others[0] == triangle_)
			{
				return HalfEdge(other, 0);
			}
			if (other->others[1] == triangle_)
			{
				return HalfEdge(other, 1);
			}
			LASS_ASSERT(other->others[2] == triangle_);
			return HalfEdge(other, 2);
		}

		bool operator!() const { return !triangle_; }
		operator num::SafeBool() const { return num::safeBool(triangle_ != 0); }
	private:
		Triangle* triangle_;
		size_t edge_;
	};

	typedef std::vector<const Triangle*> TVertexTriangles;
	typedef std::vector<TPoint> TVertexRing;
	typedef std::vector<TVector> TNormalRing;
	typedef std::vector<TUv> TUvRing;

	template <typename IndexTriangleInputRange> 
	void buildMesh(const IndexTriangleInputRange& triangles);
	void connectTriangles();
	void findVertexTriangles(TVertexTriangles& vertexTriangles) const;
	void findVertexRing(const TPoint& vertex, const Triangle* vertexTriangle, TVertexRing& ring, TVertexRing& creases, TNormalRing& normals, TUvRing& uvs) const;
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

#define LASS_PRIM_HAVE_PY_EXPORT_TRAITS_TRIANGLE_MESH_3D
#ifdef LASS_GUARDIAN_OF_INCLUSION_UTIL_PYOBJECT_PLUS_H
#	include "../python/export_traits_prim.h"
#endif

#ifdef LASS_GUARDIAN_OF_INCLUSION_PRIM_HALF_EDGE_MESH_3D_H
#	include "half_edge_mesh_3d_triangle_mesh_3d.h"
#endif

#endif

// EOF
