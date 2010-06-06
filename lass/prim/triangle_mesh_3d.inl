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



#ifndef LASS_GUARDIAN_OF_INCLUSION_PRIM_SIMPLE_TRIANGLE_MESH_3D_INL
#define LASS_GUARDIAN_OF_INCLUSION_PRIM_SIMPLE_TRIANGLE_MESH_3D_INL

#include "prim_common.h"
#include "triangle_mesh_3d.h"
#include "../stde/range_algorithm.h"
#include "../stde/extended_iterator.h"
#include "point_3dh.h"
#include "point_2dh.h"
#include <algorithm>

namespace lass
{
namespace prim
{

// --- public --------------------------------------------------------------------------------------

template <typename T, template <typename, typename, typename> class BHV, typename SH>
TriangleMesh3D<T, BHV, SH>::TriangleMesh3D(const SH& heuristics):
	tree_(heuristics)
{
}



template <typename T, template <typename, typename, typename> class BHV, typename SH>
template 
<
	typename VertexInputRange, typename IndexTriangleInputRange
>
TriangleMesh3D<T, BHV, SH>::TriangleMesh3D(
		const VertexInputRange& vertices, const IndexTriangleInputRange& triangles, const SH& heuristics):
	tree_(heuristics),
	vertices_(vertices.begin(), vertices.end()),
	normals_(),
	uvs_()
{
	buildMesh(triangles);
}



template <typename T, template <typename, typename, typename> class BHV, typename SH>
template 
<
	typename VertexInputRange, typename NormalInputRange,
	typename UvInputRange, typename IndexTriangleInputRange
>
TriangleMesh3D<T, BHV, SH>::TriangleMesh3D(
		const VertexInputRange& vertices, const NormalInputRange& normals, 
		const UvInputRange& uvs, const IndexTriangleInputRange& triangles,
		const SH& heuristics):
	tree_(heuristics),
	vertices_(vertices.begin(), vertices.end()),
	normals_(normals.begin(), normals.end()),
	uvs_(uvs.begin(), uvs.end())
{
	buildMesh(triangles);
}



template <typename T, template <typename, typename, typename> class BHV, typename SH>
TriangleMesh3D<T, BHV, SH>::TriangleMesh3D(const TriangleMesh3D& other):
	tree_(static_cast<const SH&>(other.tree_)),
	vertices_(other.vertices_),
	normals_(other.normals_),
	uvs_(other.uvs_)
{
	std::vector<IndexTriangle> triangles;
	other.indexTriangles(std::back_inserter(triangles));
	buildMesh(triangles);
}



template <typename T, template <typename, typename, typename> class BHV, typename SH>
TriangleMesh3D<T, BHV, SH>& TriangleMesh3D<T, BHV, SH>::operator=(const TriangleMesh3D& other)
{
	TriangleMesh3D temp(other);
	swap(temp);
	return *this;
}


template <typename T, template <typename, typename, typename> class BHV, typename SH>
const typename TriangleMesh3D<T, BHV, SH>::TTriangles&
TriangleMesh3D<T, BHV, SH>::triangles() const
{
	return triangles_;
}



template <typename T, template <typename, typename, typename> class BHV, typename SH>
const typename TriangleMesh3D<T, BHV, SH>::TVertices&
TriangleMesh3D<T, BHV, SH>::vertices() const
{
	return vertices_;
}



template <typename T, template <typename, typename, typename> class BHV, typename SH>
const typename TriangleMesh3D<T, BHV, SH>::TNormals&
TriangleMesh3D<T, BHV, SH>::normals() const
{
	return normals_;
}



template <typename T, template <typename, typename, typename> class BHV, typename SH>
const typename TriangleMesh3D<T, BHV, SH>::TUvs&
TriangleMesh3D<T, BHV, SH>::uvs() const
{
	return uvs_;
}



template <typename T, template <typename, typename, typename> class BHV, typename SH>
template <typename OutputIterator> 
OutputIterator TriangleMesh3D<T, BHV, SH>::indexTriangles(OutputIterator triangles) const
{
	typename TTriangles::const_iterator triangle;
	for (triangle = triangles_.begin(); triangle != triangles_.end(); ++triangle)
	{
		TIndexTriangle indexTriangle;
		for (std::size_t k = 0; k < 3; ++k)
		{
			LASS_ASSERT(triangle->vertices[k]);
			indexTriangle.vertices[k] = triangle->vertices[k] - &vertices_.front();
			indexTriangle.normals[k] = triangle->normals[k] ?
				triangle->normals[k] - &normals_.front() : IndexTriangle::null();
			indexTriangle.uvs[k] = triangle->uvs[k] ?
				triangle->uvs[k] - &uvs_.front() : IndexTriangle::null();
		}
		*triangles++ = indexTriangle;
	}
	return triangles;
}



template <typename T, template <typename, typename, typename> class BHV, typename SH>
const typename TriangleMesh3D<T, BHV, SH>::TAabb
TriangleMesh3D<T, BHV, SH>::aabb() const
{
	LASS_ASSERT(tree_.isEmpty() == triangles_.empty());
	return tree_.aabb();
}



template <typename T, template <typename, typename, typename> class BHV, typename SH>
const typename TriangleMesh3D<T, BHV, SH>::TValue
TriangleMesh3D<T, BHV, SH>::area() const
{
	TValue result = 0;

	typename TTriangles::const_iterator triangle;
	for (triangle = triangles_.begin(); triangle != triangles_.end(); ++triangle)
	{
		const TVector a = *triangle->vertices[1] - *triangle->vertices[0];
		const TVector b = *triangle->vertices[2] - *triangle->vertices[0];
		const TVector n = cross(a, b);
		result += n.norm() / 2;
	}

	return result;
}

namespace impl
{
	template <typename HalfEdgeType>
	typename HalfEdgeType::TVector computeWeightedNormal(const HalfEdgeType& edge)
	{
		typedef typename HalfEdgeType::TVector TVector;
		const TVector a = edge.vector();
		const TVector b = -edge.oPrev().vector();
		const TVector n = cross(a, b);
		if (n.isZero() || n.isNaN())
		{
			return TVector();
		}
		return n.normal() * num::acos(dot(a.normal(), b.normal()));
	}
}


template <typename T, template <typename, typename, typename> class BHV, typename SH>
void TriangleMesh3D<T, BHV, SH>::smoothNormals()
{
	const size_t numTriangles = triangles_.size();

	for (size_t i = 0; i < numTriangles; ++i)
	{
		std::fill_n(triangles_[i].normals, 3, static_cast<TVector*>(0));
	}
	normals_.clear();
	normals_.reserve(3 * triangles_.size()); // let's waste some memory, we must make sure it's never going to reallocate!

	for (size_t i = 0; i < numTriangles; ++i)
	{
		Triangle& triangle = triangles_[i];
		for (size_t k = 0; k < 3; ++k)
		{
			HalfEdge edge(&triangle, k);
			if (edge.normal())
			{
				continue;
			}

			// start a fresh normal.
			LASS_ENFORCE(normals_.size() < normals_.capacity());
			normals_.push_back(impl::computeWeightedNormal(edge));
			TVector& normal = normals_.back();
			edge.setNormal(&normal);

			// rotate left, until you hit a crease, an edge with a normal already. 
			// No need to check if you hit yourself, as you have a normal already.
			for (HalfEdge left = edge.rNext(); left && !left.normal() && !left.creaseLevel(); left = left.rNext())
			{
				normal += impl::computeWeightedNormal(left);
				left.setNormal(&normal);
			}

			// rotate right, until ...
			for (HalfEdge right = edge.rPrev(); right && !right.normal() && !right.oPrev().creaseLevel(); right = right.rPrev())
			{
				normal += impl::computeWeightedNormal(right);
				right.setNormal(&normal);
			}

			normal.normalize();
		}
	}
}



/** remove all normals, so that faces become flat
 */
template <typename T, template <typename, typename, typename> class BHV, typename SH>
void TriangleMesh3D<T, BHV, SH>::flatFaces()
{
	normals_.clear();
	for (typename TTriangles::iterator triangle = triangles_.begin(); 
		triangle != triangles_.end(); ++triangle)
	{
		std::fill(triangle->normals, triangle->normals + 3, static_cast<TVector*>(0));
	}
}


template <typename T, template <typename, typename, typename> class BHV, typename SH>
void TriangleMesh3D<T, BHV, SH>::loopSubdivision(unsigned level)
{
	if (triangles_.empty())
	{
		return;
	}

	while (level-- > 0)
	{
		// step I: subdivide triangles with simple interpolation
		//
		subdivide();

		// copying should "reduce" the extra reserve we don't need
		TVertices newVertices = vertices_;
		TNormals newNormals = normals_;
		TUvs newUvs = uvs_;
		const TPoint* const firstVertex = &vertices_[0];
		const TVector* const firstNormal = &normals_[0];
		const TUv* const firstUv = &uvs_[0];

		// step II: smooth mesh
		//
		//*
		TVertexTriangles vertexTriangles;
		findVertexTriangles(vertexTriangles);
		TVertexRing ring;
		TVertexRing creases;
		TNormalRing normalRing;
		TUvRing uvRing;
		const size_t numVertices = vertices_.size();
		for (size_t i = 0; i < numVertices; ++i)
		{
			if (const Triangle* const vertexTriangle = vertexTriangles[i])
			{
				const TPoint& vertex = vertices_[i];	
				findVertexRing(vertex, vertexTriangle, ring, creases, normalRing, uvRing);
				const size_t nRing = ring.size();
				const size_t nCreases = creases.size();
				LASS_ASSERT(nRing >= 2);
				if (nCreases == 2)
				{
					// crease or boundary (boundary edges count as creases)
					newVertices[i] = TPoint(.75f * vertex.position() + .125f * (creases[0].position() + creases[1].position()));
				}
				else if (nRing > 2 && nCreases == 0)
				{
					// interior vertex
					const TValue beta = nRing == 6 ? 
						.125f : 2 * (.625f - num::sqr(.375f + .25f * num::cos(2 * TNumTraits::pi / nRing))) / nRing;
					TVector newVertex = (1 - nRing * beta) * vertex.position();
					for (size_t j = 0; j < nRing; ++j)
					{
						newVertex += beta * ring[j].position();
					}
					newVertices[i] = TPoint(newVertex);
					if (!uvRing.empty())
					{
						LASS_ASSERT(uvRing.size() == nRing);
						const size_t k = vertexTriangle->side(&vertex);
						LASS_ASSERT(k < 3 && vertexTriangle->uvs[k]);
						const TUv& uv = *vertexTriangle->uvs[k];
						typename TUv::TVector newUv = (1 - nRing * beta) * uv.position();
						for (size_t j = 0; j < nRing; ++j)
						{
							newUv += beta * uvRing[j].position();
						}
						newUvs[&uv - firstUv] = TUv(newUv);
					}
				}
			}
		}
		/**/

		// step III: adjust pointers and decrease crease levels
		//
		for (typename TTriangles::iterator triangle = triangles_.begin();
			triangle != triangles_.end(); ++triangle)
		{
			for (size_t k = 0; k < 3; ++k)
			{
				triangle->vertices[k] = &newVertices[triangle->vertices[k] - firstVertex];
				triangle->normals[k] = triangle->normals[k] ? &newNormals[triangle->normals[k] - firstNormal] : 0;
				triangle->uvs[k] = triangle->uvs[k] ? &newUvs[triangle->uvs[k] - firstUv] : 0;
				triangle->creaseLevel[k] = triangle->creaseLevel[k] > 0 ? triangle->creaseLevel[k] - 1 : 0;
			}
		}
		vertices_.swap(newVertices);
		normals_.swap(newNormals);
		uvs_.swap(newUvs);
	}
	tree_.reset(triangles_.begin(), triangles_.end());
}
	


/** automatically sews (unconnected) triangles by comparing geometrical vertices and normals
 */
template <typename T, template <typename, typename, typename> class BHV, typename SH>
void TriangleMesh3D<T, BHV, SH>::autoSew()
{
	typedef std::vector<PositionalEdge> TEdges;

	// I. make list of all boundary edges
	//
	const size_t numTriangles = triangles_.size();
	TEdges edges;
	edges.reserve(numBoundaryEdges_);
	for (size_t i = 0; i < numTriangles; ++i)
	{
		Triangle& triangle = triangles_[i];
		for (size_t k1 = 2, k2 = 0; k2 < 3; k1 = k2++)
		{
			if (triangle.others[k1] == 0)
			{
				edges.push_back(PositionalEdge(&triangle, k1, k2));
			}
		}
	}
	LASS_ASSERT(edges.size() == numBoundaryEdges_);
	std::sort(edges.begin(), edges.end());

	// II. go over all boundary edges, and search one to sew with ...
	//
	for (size_t i = 0; i < numTriangles; ++i)
	{
		Triangle& triangle = triangles_[i];
		for (size_t k1 = 2, k2 = 0; k2 < 3; k1 = k2++)
		{
			if (triangle.others[k1] == 0)
			{
				const PositionalEdge bait(&triangle, k2, k1); // reversed edge
				const typename TEdges::const_iterator haul = 
					std::lower_bound(edges.begin(), edges.end(), bait);
				if (haul != edges.end() && !(bait < *haul) && 
					haul->triangle->others[haul->k1] == 0)
				{
					triangle.others[k1] = haul->triangle;
					haul->triangle->others[haul->k1] = &triangle;
					LASS_ASSERT(numBoundaryEdges_ >= 2);
					numBoundaryEdges_ -= 2;
					for (size_t s = 1; s <= 2; ++s)
					{
						// turn right/left, and replace all otherV by v
						Triangle* other = haul->triangle;
						size_t k = s == 1 ? haul->k2 : haul->k1;
						const TPoint* const v = 
							triangle.vertices[s == 1 ? k1 : k2];
						const TPoint* const otherV = other->vertices[k];
						LASS_ASSERT(*v == *otherV);
						if (v != otherV)
						{
							do 
							{
								LASS_ASSERT(other->vertices[k] == otherV);
								other->vertices[k] = v;
								other = other->others[
									(k + (s == 1 ? 0 : 2)) % 3];
								k = other ? other->side(otherV) : 3;
							}
							while (other && k < 3);
						}
					}
				}
			}
		}
	}
}


namespace impl
{
	template <typename TriangleType>
	typename TriangleType::TVector computeFaceNormal(const TriangleType& triangle)
	{
		typedef typename TriangleType::TVector TVector;
		const TVector a = *triangle.vertices[1] - *triangle.vertices[0];
		const TVector b = *triangle.vertices[2] - *triangle.vertices[0];
		const TVector n = cross(a, b);
		if (n.isZero() || n.isNaN())
		{
			return TVector();
		}
		return n.normal();
	}
}

/** automatically assign crease levels to edges with discontinued normals
 */
template <typename T, template <typename, typename, typename> class BHV, typename SH>
void TriangleMesh3D<T, BHV, SH>::autoCrease(unsigned level, TParam maxAngleInRadians)
{
	const TValue minDot = num::cos(maxAngleInRadians);
	const size_t numTriangles = triangles_.size();
	for (size_t i = 0; i < numTriangles; ++i)
	{
		Triangle& triangle = triangles_[i];
		const TVector faceNormal = impl::computeFaceNormal(triangle);
		for (size_t k1 = 2, k2 = 0; k2 < 3; k1 = k2++)
		{
			triangle.creaseLevel[k1] = level; // it's a crease by default, smooth it under conditions.

			Triangle* other = triangle.others[k1];
			if (!other)
			{
				continue;
			}
			const size_t otherK1 = other->side(triangle.vertices[k1]);
			const size_t otherK2 = (otherK1 + 2) % 3;
			LASS_ASSERT(otherK1 < 3 && otherK2 == other->side(triangle.vertices[k2]));
			const TVector otherNormal = impl::computeFaceNormal(*other);

			const TVector a1 = (triangle.normals[k1] ? *triangle.normals[k1] : faceNormal).normal();
			const TVector a2 = (triangle.normals[k2] ? *triangle.normals[k2] : faceNormal).normal();
			const TVector b1 = (other->normals[otherK1] ? *other->normals[otherK1] : otherNormal).normal();
			const TVector b2 = (other->normals[otherK2] ? *other->normals[otherK2] : otherNormal).normal();

			if (maxAngleInRadians == 0)
			{
				if (a1 == b1 && a2 == b2)
				{
					triangle.creaseLevel[k1] = 0;
				}
			}
			else
			{
				if (dot(a1, b1) >= minDot && dot(a2, b2) >= minDot)
				{
					triangle.creaseLevel[k1] = 0;
				}
			}
		}
	}
}



template <typename T, template <typename, typename, typename> class BHV, typename SH>
Result TriangleMesh3D<T, BHV, SH>::intersect(const TRay& ray, TTriangleIterator& triangle,
		TReference t, TParam tMin, IntersectionContext* context) const
{
	LASS_ASSERT(tree_.isEmpty() == triangles_.empty());
	const TTriangleIterator candidate =  tree_.intersect(ray, t, tMin);
	if (candidate == triangles_.end())
	{
		return rNone;
	}
	triangle = candidate;
	
	if (context)
	{
		TValue temp;
		const Result LASS_UNUSED(r) = triangle->intersect(ray, temp, tMin, context);
		LASS_ASSERT(r == rOne && t == temp);
	}

	return rOne;
}



template <typename T, template <typename, typename, typename> class BHV, typename SH>
bool TriangleMesh3D<T, BHV, SH>::intersects(const TRay& ray, TParam tMin, TParam tMax) const
{
	LASS_ASSERT(tree_.isEmpty() == triangles_.empty());
	return tree_.intersects(ray, tMin, tMax);
}



template <typename T, template <typename, typename, typename> class BHV, typename SH>
void TriangleMesh3D<T, BHV, SH>::swap(TSelf& other)
{
	tree_.swap(other.tree_);
	triangles_.swap(other.triangles_);
	vertices_.swap(other.vertices_);
	normals_.swap(other.normals_);
	uvs_.swap(other.uvs_);
	std::swap(numBoundaryEdges_, other.numBoundaryEdges_);
}



// --- Triangle ------------------------------------------------------------------------------------

template <typename T, template <typename, typename, typename> class BHV, typename SH>
Result TriangleMesh3D<T, BHV, SH>::Triangle::intersect(const TRay& ray, 
		TReference t, TParam tMin, IntersectionContext* context) const
{
	LASS_ASSERT(vertices[0] && vertices[1] && vertices[2]);
	const TPoint point0 = *vertices[0];
	const TVector dPoint_dR = *vertices[1] - point0;
	const TVector dPoint_dS = *vertices[2] - point0;

	TValue r, s;
	const Result hit = impl::intersectTriangle3D(
		point0, dPoint_dR, dPoint_dS, ray.support(), ray.direction(), r, s, t, tMin);
	if (hit != rOne)
	{
		return hit;
	}

	if (context)
	{
		TUv dRs_dU(1, 0);
		TUv dRs_dV(0, 1);
		if (uvs[0])
		{
			LASS_ASSERT(uvs[1] && uvs[2]);
			const TUv uv0 = *uvs[0];
			const typename TUv::TVector dUv_dR = *uvs[1] - uv0;
			const typename TUv::TVector dUv_dS = *uvs[2] - uv0;
			context->uv = uv0 + r * dUv_dR + s * dUv_dS;
		
			const TValue matrix[4] = { dUv_dR.x, dUv_dS.x, dUv_dR.y, dUv_dS.y };
			TValue solution[4] = { 1, 0, 0, 1 };
			num::impl::cramer2<TValue>(matrix, solution, solution + 4);
			dRs_dU = TUv(solution[0], solution[2]);
			dRs_dV = TUv(solution[1], solution[3]);
			context->dPoint_dU = dPoint_dR * dRs_dU.x + dPoint_dS * dRs_dU.y;
			context->dPoint_dV = dPoint_dR * dRs_dV.x + dPoint_dS * dRs_dV.y;
		}
		else
		{
			context->uv = TUv(r, s);
			context->dPoint_dU = dPoint_dR;
			context->dPoint_dV = dPoint_dS;
		}

		context->geometricNormal = cross(dPoint_dR, dPoint_dS).normal();
		if (normals[0])
		{
			LASS_ASSERT(normals[1] && normals[2]);
			const TVector normal0 = *normals[0];
			const TVector dNormal_dR = *normals[1] - normal0;
			const TVector dNormal_dS = *normals[2] - normal0;
			context->normal = (normal0 + r * dNormal_dR + s * dNormal_dS).normal();
			context->dNormal_dU = dNormal_dR * dRs_dU.x + dNormal_dS * dRs_dU.y;
			context->dNormal_dV = dNormal_dR * dRs_dV.x + dNormal_dS * dRs_dV.y;
		}
		else
		{
			context->normal = context->geometricNormal;
			context->dNormal_dU = TVector();
			context->dNormal_dV = TVector();
		}
	}
	return rOne;
}



template <typename T, template <typename, typename, typename> class BHV, typename SH> inline
size_t TriangleMesh3D<T, BHV, SH>::Triangle::side(const TPoint* vertex) const 
{ 
	return vertices[0] == vertex ? 0 : (vertices[1] == vertex ? 1 : (vertices[2] == vertex ? 2 : size_t(-1))); 
}



// --- private -------------------------------------------------------------------------------------

template <typename T, template <typename, typename, typename> class BHV, typename SH>
template <typename IndexTriangleInputRange>
void TriangleMesh3D<T, BHV, SH>::buildMesh(const IndexTriangleInputRange& triangles)
{
	const std::size_t sizeVertices = vertices_.size();
	const std::size_t sizeNormals = normals_.size();
	const std::size_t sizeUvs = uvs_.size();

	for (typename IndexTriangleInputRange::const_iterator i = triangles.begin(); i != triangles.end(); ++i)
	{
		TTriangle triangle;
		size_t numNormals = 0;
		size_t numUvs = 0;
		for (std::size_t k = 0; k < 3; ++k)
		{
			triangle.others[k] = 0;
			triangle.creaseLevel[k] = 0;

			const std::size_t vertex = i->vertices[k];
			triangle.vertices[k] = &vertices_[LASS_ENFORCE_INDEX(vertex, sizeVertices)];

			const std::size_t normal = i->normals[k];
			if (normal != IndexTriangle::null())
			{				
				triangle.normals[k] = &normals_[LASS_ENFORCE_INDEX(normal, sizeNormals)];
				++numNormals;
			}
			else
			{
				triangle.normals[k] = 0;
			}

			const std::size_t uv = i->uvs[k];
			if (uv != IndexTriangle::null())
			{
				triangle.uvs[k] = &uvs_.begin()[LASS_ENFORCE_INDEX(uv, sizeUvs)];
				++numUvs;
			}
			else
			{
				triangle.uvs[k] = 0;
			}
		}

		if (!(numNormals == 0 || numNormals == 3))
		{
			LASS_THROW("Each triangle must have either zero or three normals vectors");
		}
		if (!(numUvs == 0 || numUvs == 3))
		{
			LASS_THROW("Each triangle must have either zero or three uv coordinates");
		}

		triangles_.push_back(triangle);
	}

	connectTriangles();
	tree_.reset(triangles_.begin(), triangles_.end());
}



template <typename T, template <typename, typename, typename> class BHV, typename SH>
void TriangleMesh3D<T, BHV, SH>::findVertexTriangles(TVertexTriangles& vertexTriangles) const
{
	TVertexTriangles result(vertices_.size(), 0);
	const TPoint* const firstVertex = &vertices_[0];
	for (typename TTriangles::const_iterator i = triangles_.begin(); i != triangles_.end(); ++i)
	{
		const Triangle& triangle = *i;
		for (size_t k = 0; k < 3; ++k)
		{
			const size_t j = triangle.vertices[k] - firstVertex;
			LASS_ASSERT(j < result.size());
			result[j] = &triangle;
		}
	}
	vertexTriangles.swap(result);
}



template <typename T, template <typename, typename, typename> class BHV, typename SH>
void TriangleMesh3D<T, BHV, SH>::connectTriangles()
{
	typedef std::vector<LogicalEdge> TEdges;

	numBoundaryEdges_ = 0;
	const size_t numTriangles = triangles_.size();
	if (numTriangles == 0)
	{
		return;
	}

	// step 1: build list of edges and sort.
	//
	TEdges edges;
	edges.reserve(3 * numTriangles);
	for (size_t i = 0; i < numTriangles; ++i)
	{
		Triangle& triangle = triangles_[i];
		for (std::size_t k1 = 2, k2 = 0; k2 < 3; k1 = k2++)
		{
			edges.push_back(LogicalEdge(
				&triangle, triangle.vertices[k1], triangle.vertices[k2]));
		}
	}
	std::sort(edges.begin(), edges.end());

	// step 2: build topological information of triangles
	//
	for (size_t i = 0; i < numTriangles; ++i)
	{
		Triangle& triangle = triangles_[i];
		for (std::size_t k1 = 2, k2 = 0; k2 < 3; k1 = k2++)
		{
			const LogicalEdge bait(0, triangle.vertices[k2], triangle.vertices[k1]); // reversed edge
			const typename TEdges::const_iterator haul = std::lower_bound(edges.begin(), edges.end(), bait);
			if (haul != edges.end() && *haul == bait)
			{
				if (stde::next(haul) != edges.end() && *stde::next(haul) == bait)
				{
					std::ostringstream buffer;
					buffer << "multiple half edges detected from " << *bait.tail << " to " << *bait.head << ". Triangles:\n";
					const typename TEdges::const_iterator last = std::upper_bound(edges.begin(), edges.end(), bait);
					for (typename TEdges::const_iterator e = haul; e != last; ++e)
					{
						const Triangle& t = *e->triangle;
						buffer << *t.vertices[0] << ", " << *t.vertices[1] << ", " << *t.vertices[2] << "\n";
					}
					LASS_THROW(buffer.str());
				}
				triangle.others[k1] = haul->triangle;
			}
			else
			{
				triangle.others[k1] = 0;
				++numBoundaryEdges_;
			}
		}
	}
}



template <typename T, template <typename, typename, typename> class BHV, typename SH>
void TriangleMesh3D<T, BHV, SH>::findVertexRing(
		const TPoint& vertex, const Triangle* vertexTriangle, TVertexRing& ring, 
		TVertexRing& creases, TNormalRing& normals, TUvRing& uvs) const
{
	ring.resize(0); // normaly, this should not shrink memory (*)
	creases.resize(0);
	normals.resize(0);
	uvs.resize(0);

	bool hasUvRing = true;
	const TUv* vertexUv = 0;
	const Triangle* triangle = vertexTriangle;
	do
	{
		const size_t k = triangle->side(&vertex);
		LASS_ASSERT(k < 3);
		const size_t kCcw = (k + 2) % 3;
		const TPoint& neighbour = *triangle->vertices[kCcw];
		const Triangle* const other = triangle->others[kCcw];
		if (triangle->creaseLevel[kCcw] > 0 || !other)
		{
			//LASS_ASSERT(triangle->others[kCcw]);
			creases.push_back(neighbour);
		}

		if (!vertexUv)
		{
			vertexUv = triangle->uvs[k];
		}
		hasUvRing &= vertexUv != 0 && vertexUv == triangle->uvs[k];
		if (hasUvRing)
		{
			LASS_ASSERT(triangle->uvs[kCcw]);
			uvs.push_back(*triangle->uvs[kCcw]);
		}

		if (!other) 
		{
			// we've hit a boundary edge, turn back to other "end".
			// ring will only consist of that one + this neighbour
			//
			const Triangle* triangle2 = vertexTriangle;
			const TPoint* neighbour2 = 0;
			while (triangle2)
			{
				const size_t k = triangle2->side(&vertex);
				LASS_ASSERT(k < 3);
				const size_t kCw = (k + 1) % 3;
				neighbour2 = triangle2->vertices[kCw];
				const Triangle* other2 = triangle2->others[k];
				if (triangle2->creaseLevel[kCw] > 0 || !other2)
				{
					//LASS_ASSERT(triangle2->others[kCw]);
					creases.push_back(*neighbour2);
				}
				triangle2 = other2;
			}
			ring.resize(1, *neighbour2);
			hasUvRing = false;
		}
		ring.push_back(neighbour);
		triangle = other;
	}
	while (triangle && triangle != vertexTriangle);

	if (!hasUvRing)
	{
		uvs.resize(0);
	}
	// (*) so says the standard, as as we all know, _every_ compiler follows the standard, don't they?
}



/** subdivide every triangle in 4 new ones by splitting the edges
 *  @warning tree_ will be reset and must be recreated manually after calling this function!
 */
template <typename T, template <typename, typename, typename> class BHV, typename SH>
void TriangleMesh3D<T, BHV, SH>::subdivide()
{
	if (triangles_.empty())
	{
		return;
	}

	const size_t numTriangles = triangles_.size();
	const size_t numVertices = vertices_.size();
	const size_t numUvs = uvs_.size();
	LASS_ASSERT((3 * numTriangles + numBoundaryEdges_) % 2 == 0);
	const size_t numOddVertices = (3 * numTriangles + numBoundaryEdges_) / 2;

	TTriangles newTriangles(4 * numTriangles);
	TVertices newVertices = vertices_;
	TNormals newNormals = normals_;
	TUvs newUvs = uvs_;
	newVertices.reserve(numVertices + numOddVertices);
	//newNormals.reserve(numNormals + 3 * numTriangles);
	newUvs.reserve(numUvs + 3 * numTriangles);

	const Triangle* const firstTriangle = &triangles_[0];
	const TPoint* const firstVertex = &vertices_[0];
	const TVector* const firstNormal = &normals_[0];
	const TUv* const firstUv = &uvs_[0];

	/*
	            2             - original vertex numbers are on outside
	            *             - new vertex numbers are on inside
	           /2\            - triangle numbers are between braces: (#)
	          /   \           - as always everything is counter clockwise
	         / (2) \          - number of edges (for 'others'): same number as it tail.
	        /0     1\         - triangle (3) is called the oddTriangle
	       *---------* 
	      /2\2     1/2\ 
	     /   \ (3) /   \ 
	    / (0) \   / (1) \ 
	   /0     1\0/0     1\ 
	  *---------*---------* 
	 0                     1
	*/
	for (size_t i = 0; i < numTriangles; ++i)
	{
		const Triangle& triangle = triangles_[i];
		Triangle* const newTris = &newTriangles[4 * i];
		Triangle& oddTriangle = newTris[3];
	
		// compute new vertices, normals and Uvs, and store in odd triangle
		//
		for (size_t k0 = 2, k1 = 0; k1 < 3; k0 = k1++)
		{
			const Triangle* const other = triangle.others[k0];
			Triangle* const oddOther = other ? &newTriangles[4 * (other - firstTriangle) + 3] : 0;
			const size_t h0 = other ? other->side(triangle.vertices[k1]) : size_t(-1);
			const size_t h1 = other ? other->side(triangle.vertices[k0]) : size_t(-1);
			LASS_ASSERT(h0 < 3 && h1 == (h0 + 1) % 3 || other == 0);

			if (!oddTriangle.vertices[k0])
			{	
				LASS_ASSERT(newVertices.size() < newVertices.capacity());
				newVertices.push_back(TPoint(.5 * (triangle.vertices[k0]->position() + triangle.vertices[k1]->position())));
				oddTriangle.vertices[k0] = &newVertices.back();
				if (other)
				{
					oddOther->vertices[h0] = &newVertices.back();
				}
			}
			if (triangle.normals[k0] && !oddTriangle.normals[k0])
			{
				LASS_ASSERT(triangle.normals[k1]);
				//LASS_ASSERT(newNormals.size() < newNormals.capacity());
				newNormals.push_back(.5 * (*triangle.normals[k0] + *triangle.normals[k1]));
				oddTriangle.normals[k0] = &newNormals.back();
				if (other && triangle.normals[k0] == other->normals[h1] && triangle.normals[k1] == other->normals[h0])
				{
					oddOther->normals[h0] = &newNormals.back();
				}
			}		
			if (triangle.uvs[k0] && !oddTriangle.uvs[k0])
			{
				LASS_ASSERT(newUvs.size() < newUvs.capacity());
				LASS_ASSERT(triangle.uvs[k1]);
				newUvs.push_back(TUv(.5 * (triangle.uvs[k0]->position() + triangle.uvs[k1]->position())));
				oddTriangle.uvs[k0] = &newUvs.back();
				if (other && triangle.uvs[k0] == other->uvs[h1] && triangle.uvs[k1] == other->uvs[h0])
				{
					oddOther->uvs[h0] = &newUvs.back();
				}
			}		
			oddTriangle.others[k0] = &newTris[k1];
			oddTriangle.creaseLevel[k0] = 0;
		}

		// connect other triangles
		//
		for (size_t k0 = 0, k1 = 1, k2 = 2; k0 < 3; k1 = k2, k2 = k0, ++k0)
		{
			Triangle& newTriangle = newTris[k0];
			newTriangle.vertices[k0] = &newVertices[triangle.vertices[k0] - firstVertex];
			newTriangle.vertices[k1] = oddTriangle.vertices[k0];
			newTriangle.vertices[k2] = oddTriangle.vertices[k2];
			newTriangle.normals[k0] = triangle.normals[k0] ? &newNormals[triangle.normals[k0] - firstNormal] : 0;
			newTriangle.normals[k1] = oddTriangle.normals[k0];
			newTriangle.normals[k2] = oddTriangle.normals[k2];
			newTriangle.uvs[k0] = triangle.uvs[k0] ? &newUvs[triangle.uvs[k0] - firstUv] : 0;
			newTriangle.uvs[k1] = oddTriangle.uvs[k0];
			newTriangle.uvs[k2] = oddTriangle.uvs[k2];
			if (const Triangle* other0 = triangle.others[k0])
			{
				const size_t j = other0 - firstTriangle;
				const size_t h = other0->side(triangle.vertices[k0]);
				LASS_ASSERT(h < 3);
				Triangle& newOther = newTriangles[4 * j + h];
				LASS_ASSERT(newOther.others[(h + 2) % 3] == 0 || newOther.others[(h + 2) % 3] == &newTriangle);
				newTriangle.others[k0] = &newOther;
			}
			newTriangle.others[k1] = &oddTriangle;
			if (const Triangle* other2 = triangle.others[k2])
			{
				const size_t j = other2 - firstTriangle;
				const size_t h = other2->side(triangle.vertices[k0]);
				LASS_ASSERT(h < 3);
				Triangle& newOther = newTriangles[4 * j + h];
				LASS_ASSERT(newOther.others[h] == 0 || newOther.others[h] == &newTriangle);
				newTriangle.others[k2] = &newOther;
			}
			newTriangle.creaseLevel[k0] = triangle.creaseLevel[k0];
			newTriangle.creaseLevel[k1] = 0;
			newTriangle.creaseLevel[k2] = triangle.creaseLevel[k2];
		}
	}

	triangles_.swap(newTriangles);
	vertices_.swap(newVertices);
	normals_.swap(newNormals);
	uvs_.swap(newUvs);
	tree_.reset();
	numBoundaryEdges_ *= 2;
}



// --- free ----------------------------------------------------------------------------------------



}

}

#endif

// EOF
