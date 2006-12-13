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
template 
<
	typename VertexInputRange, typename NormalInputRange,
	typename UvInputRange, typename IndexTriangleInputRange
>
TriangleMesh3D<T, BHV, SH>::TriangleMesh3D(
		const VertexInputRange& iVertices, const NormalInputRange& iNormals, 
		const UvInputRange& iUvs, const IndexTriangleInputRange& iTriangles):
	vertices_(iVertices.begin(), iVertices.end()),
	normals_(iNormals.begin(), iNormals.end()),
	uvs_(iUvs.begin(), iUvs.end())
{
	const std::size_t sizeVertices = vertices_.size();
	const std::size_t sizeNormals = normals_.size();
	const std::size_t sizeUvs = uvs_.size();

	for (typename IndexTriangleInputRange::const_iterator i = iTriangles.begin(); i != iTriangles.end(); ++i)
	{
		TTriangle triangle;
		size_t numNormals = 0;
		size_t numUvs = 0;
		for (std::size_t k = 0; k < 3; ++k)
		{
			triangle.others[k] = 0;
			triangle.creaseLevel[k] = 0;

			const std::size_t vertex = i->vertices[k];
			if (vertex >= sizeVertices)
			{
				LASS_THROW("vertex index is out of range: " << static_cast<unsigned long>(vertex) 
					<< " >= " << static_cast<unsigned long>(sizeVertices));
			}
			triangle.vertices[k] = &vertices_[vertex];

			const std::size_t normal = i->normals[k];
			if (normal != IndexTriangle::null())
			{				
				if (normal >= sizeNormals)
				{
					LASS_THROW("normal index is out of range: " << static_cast<unsigned long>(normal) 
						<< " >= " << static_cast<unsigned long>(sizeNormals));
				}
				triangle.normals[k] = &normals_[normal];
				++numNormals;
			}
			else
			{
				triangle.normals[k] = 0;
			}

			const std::size_t uv = i->uvs[k];
			if (uv != IndexTriangle::null())
			{
				if (uv >= sizeUvs)
				{
					LASS_THROW("uv index is out of range: " << static_cast<unsigned long>(uv) 
						<< " >= " << static_cast<unsigned long>(sizeUvs));
				}
				triangle.uvs[k] = &uvs_.begin()[uv];
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
OutputIterator TriangleMesh3D<T, BHV, SH>::indexTriangles(OutputIterator oIndices) const
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
		*oIndices++ = indexTriangle;
	}
	return oIndices;
}



template <typename T, template <typename, typename, typename> class BHV, typename SH>
const typename TriangleMesh3D<T, BHV, SH>::TAabb
TriangleMesh3D<T, BHV, SH>::aabb() const
{
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



template <typename T, template <typename, typename, typename> class BHV, typename SH>
void TriangleMesh3D<T, BHV, SH>::smoothNormals(TParam iMaxAngleInRadians)
{
	const std::size_t numTriangles = triangles_.size();
	const std::size_t numVertices = vertices_.size();
	const TValue minDot = num::cos(iMaxAngleInRadians);

	// first pass: determine possible vertex normals
	//
	TNormals faceNormals(numTriangles);
	TNormals vertexNormals(numVertices);
	for (std::size_t i = 0; i < numTriangles; ++i)
	{
		const Triangle& triangle = triangles_[i];

		bool isDegenerated = false;
		for (std::size_t prevK = 2, k = 0; k < 3; prevK = k++)
		{
			isDegenerated |= (*triangle.vertices[k] == *triangle.vertices[prevK]);
		}
		if (isDegenerated)
		{
			faceNormals[i] = TVector();
			continue;
		}

		const TVector edges[3] =
		{
			(*triangle.vertices[1] - *triangle.vertices[0]).normal(),
			(*triangle.vertices[2] - *triangle.vertices[1]).normal(),
			(*triangle.vertices[0] - *triangle.vertices[2]).normal()
		};
		
		const TVector n = cross(edges[0], edges[1]).normal();
		faceNormals[i] = n;

		if (!n.isZero() && !n.isNaN())
		{
			for (std::size_t prevK = 2, k = 0; k < 3; prevK = k++)
			{
				const std::size_t j = triangle.vertices[k] - &vertices_.front();
				vertexNormals[j] += n * num::acos(-dot(edges[prevK], edges[k]));
			}
		}
	}
	stde::for_each_r(vertexNormals, std::mem_fun_ref(&TVector::normalize));

	// 2nd pass: determine wether to use vertex or face normal
	//
	std::vector<std::size_t> usedVertexNormals(vertices_.size(), IndexTriangle::null());
	std::vector<std::size_t> usedFaceNormals(triangles_.size(), IndexTriangle::null());
	std::size_t normalCount = 0;
	for (std::size_t i = 0; i < numTriangles; ++i)
	{
		const Triangle& triangle = triangles_[i];
		for (std::size_t k = 0; k < 3; ++k)
		{
			const std::size_t j = triangle.vertices[k] - &vertices_.front();
			if (dot(faceNormals[i], vertexNormals[j]) >= minDot)
			{
				if (usedVertexNormals[j] == IndexTriangle::null())
				{
					usedVertexNormals[j] = normalCount++;
				}
			}
			else
			{
				if (usedFaceNormals[i] == IndexTriangle::null())
				{
					usedFaceNormals[i] = normalCount++;
				}
			}
		}
	}

	// 2nd pass and a half: collect selected normals in normals_
	normals_.resize(normalCount);
	for (std::size_t i = 0; i < numTriangles; ++i)
	{
		if (usedFaceNormals[i] != IndexTriangle::null())
		{
			normals_[usedFaceNormals[i]] = faceNormals[i];
		}
	}
	for (std::size_t i = 0; i < numVertices; ++i)
	{
		if (usedVertexNormals[i] != IndexTriangle::null())
		{
			normals_[usedVertexNormals[i]] = vertexNormals[i];
		}
	}

	// 3rd pass: assign pointers to normals!
	//
	for (std::size_t i = 0; i < numTriangles; ++i)
	{
		Triangle& triangle = triangles_[i];
		for (std::size_t k = 0; k < 3; ++k)
		{
			const std::size_t j = triangle.vertices[k] - &vertices_.front();
			if (dot(faceNormals[i], vertexNormals[j]) >= minDot)
			{
				LASS_ASSERT(usedVertexNormals[j] != IndexTriangle::null());
				triangle.normals[k] = &normals_[usedVertexNormals[j]];
			}
			else
			{
				LASS_ASSERT(usedFaceNormals[i] != IndexTriangle::null());
				triangle.normals[k] = &normals_[usedFaceNormals[i]];
			}
		}
	}
}



/** remove all normals, so that faces become flat
 */
template <typename T, template <typename, typename, typename> class BHV, typename SH>
void TriangleMesh3D<T, BHV, SH>::flatFaces()
{
	normals_.clear();
	for (TTriangles::iterator triangle = triangles_.begin(); triangle != triangles_.end(); ++triangle)
	{
		std::fill(triangle->normals, triangle->normals + 3, static_cast<TVector*>(0));
	}
}



template <typename T, template <typename, typename, typename> class BHV, typename SH>
void TriangleMesh3D<T, BHV, SH>::loopSubdivision(unsigned level)
{
#pragma LASS_TODO("refactor this function [Bramz]")
	if (triangles_.empty())
	{
		return;
	}

	while (level > 0)
	{
		const size_t numTriangles = triangles_.size();
		const size_t numVertices = vertices_.size();

		// step I: first, of each vertex, we need to find an adjacent triangle
		//
		typedef std::vector<Triangle*> TVertexTriangles;
		TVertexTriangles vertexTriangles(numVertices, 0);
		const TPoint* const firstVertex = &vertices_[0];
		for (size_t i = 0; i < numTriangles; ++i)
		{
			Triangle& triangle = triangles_[i];
			for (size_t k = 0; k < 3; ++k)
			{
				const size_t j = triangle.vertices[k] - firstVertex;
				vertexTriangles[j] = &triangle;
			}
		}

		// step II: compute new even vertices
		//	
		LASS_ASSERT((3 * numTriangles + numBoundaryEdges_) % 2 == 0);
		const size_t numEvenVertices = (3 * numTriangles + numBoundaryEdges_) / 2;
		typedef std::vector<const TPoint*> TRing;
		TRing ring;
		TRing creases;
		TVertices newVertices;
		newVertices.reserve(numVertices + numEvenVertices);
		for (size_t i = 0; i < numVertices; ++i)
		{
			const TPoint& vertex = vertices_[i];
			Triangle* const vertexTriangle = vertexTriangles[i];
			Triangle* triangle = vertexTriangle;
			if (!triangle)
			{
				newVertices.push_back(vertex);
				continue;
			}
			stde::overwrite_insert_iterator<TRing> last(ring);
			stde::overwrite_insert_iterator<TRing> lastCrease(creases);
			do
			{
				const size_t k = triangle->side(&vertex);
				LASS_ASSERT(k < 3);
				const size_t kCcw = (k + 2) % 3;
				const TPoint* neighbour = triangle->vertices[kCcw];
				if (triangle->creaseLevel[kCcw] > 0)
				{
					LASS_ASSERT(triangle->others[kCcw]);
					*lastCrease++ = neighbour;
					//--triangle->creaseLevel[kCcw];
				}
				triangle = triangle->others[kCcw];
				if (triangle)
				{
					*last++ = neighbour;
				}
				else
				{
					// boundary/crease edge, turn back to other "end", and use special formula.
					const TPoint* neighbour2 = 0;
					triangle = vertexTriangle;
					while (triangle) //
					{
						const size_t k = triangle->side(&vertex);
						LASS_ASSERT(k < 3);
						const size_t kCw = (k + 1) % 3;
						neighbour2 = triangle->vertices[kCw];
						if (triangle->creaseLevel[kCw] > 0)
						{
							LASS_ASSERT(triangle->others[kCw]);
							*lastCrease++ = neighbour2;
							--triangle->creaseLevel[kCw];
						}
						triangle = triangle->others[k];
					}
					// boundary edges count as 2 creases, so continue only if no other creases.
					if (lastCrease.get() == creases.begin())
					{
						const TVector newPos = .75f * vertex.position() + 
							.125f * (neighbour->position() + neighbour2->position());
						LASS_ASSERT(newVertices.size() < newVertices.capacity());
						newVertices.push_back(TPoint(newPos));
					}
					else
					{
						// more than three "creases", don't move!
						LASS_ASSERT(newVertices.size() < newVertices.capacity());
						newVertices.push_back(vertex);
					}
				}
			}
			while (triangle && triangle != vertexTriangle);
			if (triangle)
			{
				const size_t nCreases = lastCrease.get() - creases.begin();
				TVector newPos = vertex.position();
				switch (nCreases)
				{
				case 0:
					{
						// interior vertex
						const size_t n = last.get() - ring.begin();
						const TValue beta = n == 6 ? 
							.0625f : (.625f - num::sqr(.375f + .25f * num::cos(2 * TNumTraits::pi / n))) / n;
						newPos *= (1 - n * beta);
						for (size_t i = 0; i < n; ++i)
						{
							newPos += beta * ring[i]->position();
						}
					}
					break;
				case 2:
					// same thing as boundary edge
					newPos *= .75f;
					newPos += .125f * (creases[0]->position() + creases[1]->position());
					break;
				default:
					{
						int a = 5;
					}
					break;
				}
				LASS_ASSERT(newVertices.size() < newVertices.capacity());
				newVertices.push_back(TPoint(newPos));
			}
		}

		// III. now the fun starts, compute new stuff ...
		//
		//            2				- original vertex numbers are on outside
		//			  *				- new vertex numbers are on inside
		//           /2\			- triangle numbers are between braces (in the middle)
		//          /   \		    - as always everything is counter clockwise
		//         / (2) \			- number of edges (for 'others'): same number as it tail.
		//        /0     1\         - triangle (3) is called the oddTriangle
		//       *---------*
		//      /2\2     1/2\
		//     /   \ (3) /   \
		//    / (0) \   / (1) \
		//   /0     1\0/0     1\
		//  *---------*---------*
		// 0                     1
		//
		TTriangles newTriangles(4 * numTriangles);
		const Triangle* const firstTriangle = &triangles_[0];
		for (size_t i = 0; i < numTriangles; ++i)
		{
			Triangle& triangle = triangles_[i];
			Triangle* const newTris = &newTriangles[4 * i];

			// III.1 compute odd vertrices
			//
			Triangle& oddTriangle = newTris[3];
			const TPoint* b = triangle.vertices[1];
			const TPoint* c = triangle.vertices[2];
			for (size_t k = 0; k < 3; ++k)
			{
				const TPoint* const a = triangle.vertices[k];
				if (oddTriangle.vertices[k] == 0)
				{
					LASS_ASSERT(newVertices.size() < newVertices.capacity());
					if (Triangle* const other = triangle.others[k])
					{
						const size_t otherK = other->side(a);
						LASS_ASSERT(otherK < 3);
						LASS_ASSERT(other->vertices[(otherK + 2) % 3] == b);
						LASS_ASSERT(triangle.creaseLevel[k] == other->creaseLevel[(otherK + 2) % 3]);
						if (triangle.creaseLevel[k] > 0)
						{
							--triangle.creaseLevel[k];
							--other->creaseLevel[(otherK + 2) % 3];
							newVertices.push_back(TPoint(.5f * (a->position() + b->position())));
						}
						else
						{
							const TPoint* const d = other->vertices[(otherK + 1) % 3];
							newVertices.push_back(TPoint(
								.375f * (a->position() + b->position()) + 
								.125f * (c->position() + d->position())));
						}
						Triangle& otherOddTriangle = newTriangles[4 * (other - firstTriangle) + 3];
						otherOddTriangle.vertices[(otherK + 2) % 3] = &newVertices.back();
					}
					else
					{
						LASS_ASSERT(triangle.creaseLevel[k] == 0);
						newVertices.push_back(TPoint(.5f * (a->position() + b->position())));
					}
					oddTriangle.vertices[k] = &newVertices.back();
				}			
				b = c;
				c = a;
			}

			// III.2 reconnect some things
			//
			size_t k1 = 1;
			size_t k2 = 2;
			for (size_t k0 = 0; k0 < 3; ++k0)
			{
				const TPoint* const vertex0 = triangle.vertices[k0];
				Triangle& newTri = newTris[k0];
				newTri.vertices[k0] = &newVertices[vertex0 - firstVertex];
				newTri.vertices[k1] = oddTriangle.vertices[k0];
				newTri.vertices[k2] = oddTriangle.vertices[k2];
				oddTriangle.others[k2] = &newTri;
				newTri.others[k1] = &oddTriangle;
				if (const Triangle* other0 = triangle.others[k0])
				{
					const size_t otherK = other0->side(vertex0);
					LASS_ASSERT(otherK < 3);
					Triangle& otherTri = newTriangles[4 * (other0 - firstTriangle) + otherK];
					LASS_ASSERT(otherTri.others[(otherK + 2) % 3] == 0 || otherTri.others[(otherK + 2) % 3] == &newTri);
					newTri.others[k0] = &otherTri;
				}
				if (const Triangle* other2 = triangle.others[k2])
				{
					const size_t otherK = other2->side(vertex0);
					LASS_ASSERT(otherK < 3);
					Triangle& otherTri = newTriangles[4 * (other2 - firstTriangle) + otherK];
					LASS_ASSERT(otherTri.others[otherK] == 0 || otherTri.others[otherK] == &newTri);
					newTri.others[k2] = &otherTri;
				}
				newTri.creaseLevel[k0] = triangle.creaseLevel[k0];
				newTri.creaseLevel[k2] = triangle.creaseLevel[k2];
				k1 = k2;
				k2 = k0;
			}
		}

		// IV. recompute UVs and normals
		//
		TNormals newNormals(normals_);
		TUvs newUvs(uvs_);
		newNormals.reserve(normals_.size() + 3 * numTriangles);
		newUvs.reserve(uvs_.size() + 3 * numTriangles);
		const TVector* firstNormal = !normals_.empty() ? &normals_.front() : 0;
		const TUv* firstUv = !uvs_.empty() ? &uvs_.front() : 0;
		for (size_t i = 0; i < numTriangles; ++i)
		{
			const Triangle& triangle = triangles_[i];
			Triangle* newTris = &newTriangles[4 * i];
			Triangle& oddTriangle = newTris[3];
			if (triangle.normals[0])
			{
				LASS_ASSERT(triangle.normals[1] && triangle.normals[2]);
				size_t k1 = 1;
				size_t k2 = 2;
				for (size_t k0 = 0; k0 < 3; ++k0)
				{
					// set new pointer of even Normal
					newTris[k0].normals[k0] = &newNormals[triangle.normals[k0] - firstNormal];
					// if odd normal doesn't exist yet, compute
					if (oddTriangle.normals[k0] == 0)
					{
						const TVector a = *triangle.normals[k0];
						const TVector b = *triangle.normals[k1];
						LASS_ASSERT(newNormals.size() < newNormals.capacity());
						newNormals.push_back((.5f * (a + b)).normal());
						oddTriangle.normals[k0] = &newNormals.back();
						// if other triangle uses same UVs, copy to odd triangle over there.
						if (const Triangle* const other = triangle.others[k0])
						{
							const size_t otherK0 = other->side(triangle.vertices[k0]);
							LASS_ASSERT(otherK0 < 3);
							const size_t otherK2 = (otherK0 + 2) % 3;
							if (triangle.normals[k0] == other->normals[otherK0] && 
								triangle.normals[k1] == other->normals[otherK2])
							{
								Triangle& otherOddTriangle = 
									newTriangles[4 * (other - firstTriangle) + 3];
								otherOddTriangle.normals[otherK2] = oddTriangle.normals[k0];
							}
						}
					}
					newTris[k0].normals[k1] = oddTriangle.normals[k0];
					newTris[k1].normals[k0] = oddTriangle.normals[k0];
					k1 = k2;
					k2 = k0;
				}
			}
			if (triangle.uvs[0])
			{
				LASS_ASSERT(triangle.uvs[1] && triangle.uvs[2]);
				size_t k1 = 1;
				size_t k2 = 2;
				for (size_t k0 = 0; k0 < 3; ++k0)
				{
					// set new pointer of even UV
					newTris[k0].uvs[k0] = &newUvs[triangle.uvs[k0] - firstUv];
					// if odd UV doesn't exist yet, compute
					if (oddTriangle.uvs[k0] == 0)
					{
						const TUv a = *triangle.uvs[k0];
						const TUv b = *triangle.uvs[k1];
						LASS_ASSERT(newUvs.size() < newUvs.capacity());
						newUvs.push_back(TUv(.5f * (a.position() + b.position())));
						oddTriangle.uvs[k0] = &newUvs.back();
						// if other triangle uses same UVs, copy to odd triangle over there.
						if (const Triangle* const other = triangle.others[k0])
						{
							const size_t otherK0 = other->side(triangle.vertices[k0]);
							LASS_ASSERT(otherK0 < 3);
							const size_t otherK2 = (otherK0 + 2) % 3;
							if (triangle.uvs[k0] == other->uvs[otherK0] && 
								triangle.uvs[k1] == other->uvs[otherK2])
							{
								Triangle& otherOddTriangle = 
									newTriangles[4 * (other - firstTriangle) + 3];
								otherOddTriangle.uvs[otherK2] = oddTriangle.uvs[k0];
							}
						}
					}
					newTris[k0].uvs[k1] = oddTriangle.uvs[k0];
					newTris[k1].uvs[k0] = oddTriangle.uvs[k0];
					k1 = k2;
					k2 = k0;
				}
			}
		}

		vertices_.swap(newVertices);
		normals_.swap(newNormals);
		uvs_.swap(newUvs);
		triangles_.swap(newTriangles);
		numBoundaryEdges_ *= 2;

		--level;
	}
	tree_.reset(triangles_.begin(), triangles_.end());

	std::ofstream test("test.obj");
	for (TVertices::const_iterator v = vertices_.begin(); v != vertices_.end(); ++v)
	{
		test << "v";
		for (size_t k = 0; k < 3; ++k)
		{
			test << " " << (*v)[k];
		}
		test << "\n";
	}
	const TPoint* const firstVertex = &vertices_[0];
	for (TTriangles::const_iterator t = triangles_.begin(); t != triangles_.end(); ++t)
	{
		test << "f";
		for (size_t k = 0; k < 3; ++k)
		{
			test << " " << (t->vertices[k] - firstVertex + 1);
		}
		test << "\n";
	}

}



/** automatically sews (unconnected) triangles by comparing geometrical vertices and normals
 */
template <typename T, template <typename, typename, typename> class BHV, typename SH>
void TriangleMesh3D<T, BHV, SH>::autoSew()
{
	struct Edge
	{
		Triangle* triangle;
		size_t k1;
		size_t k2;
		Edge(Triangle* iTriangle, size_t iK1, size_t iK2): 
			triangle(iTriangle), k1(iK1), k2(iK2)
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
		const bool operator<(const Edge& iOther) const
		{
			for (size_t i = 0; i < size_; ++i)
			{
				if (x_[i] < iOther.x_[i]) return true;
				if (x_[i] > iOther.x_[i]) return false;
			}
			return false;
		}
	private:
		enum { size_ = 6 };
		TValue x_[size_];
	};
	typedef std::vector<Edge> TEdges;

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
				edges.push_back(Edge(&triangle, k1, k2));
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
				const Edge bait(&triangle, k2, k1); // reversed edge
				const TEdges::const_iterator haul = 
					std::lower_bound(edges.begin(), edges.end(), bait);
				if (haul != edges.end() && !(bait < *haul) && haul->triangle->others[haul->k1] == 0)
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
						const TPoint* const v = triangle.vertices[s == 1 ? k1 : k2];
						const TPoint* const otherV = other->vertices[k];
						LASS_ASSERT(*v == *otherV);
						if (v != otherV)
						{
							do 
							{
								LASS_ASSERT(other->vertices[k] == otherV);
								other->vertices[k] = v;
								other = other->others[(k + (s == 1 ? 0 : 2)) % 3];
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



/** automatically assign crease levels to edges with discontinued normals
 */
template <typename T, template <typename, typename, typename> class BHV, typename SH>
void TriangleMesh3D<T, BHV, SH>::autoCrease(unsigned level)
{
	const size_t numTriangles = triangles_.size();
	for (size_t i = 0; i < numTriangles; ++i)
	{
		Triangle& triangle = triangles_[i];
		for (size_t k1 = 2, k2 = 0; k2 < 3; k1 = k2++)
		{
			triangle.creaseLevel[k1] = 0;
			if (Triangle* other = triangle.others[k1])
			{
				const size_t otherK1 = other->side(triangle.vertices[k1]);
				const size_t otherK2 = (otherK1 + 2) % 3;
				LASS_ASSERT(otherK1 < 3 && otherK2 == other->side(triangle.vertices[k2]));
				const bool hasNormals =
					triangle.normals[k1] && triangle.normals[k2] && 
					other->normals[otherK1] && other->normals[otherK2];
				const bool isCrease = !hasNormals ||
					*triangle.normals[k1] != *other->normals[otherK1] ||
					*triangle.normals[k2] != *other->normals[otherK2];
				triangle.creaseLevel[k1] = isCrease ? level : 0;
			}
		}
	}
}



template <typename T, template <typename, typename, typename> class BHV, typename SH>
const Result TriangleMesh3D<T, BHV, SH>::intersect(const TRay& iRay, TTriangleIterator& oTriangle,
		TReference oT, TParam iMinT, IntersectionContext* oContext) const
{
	TValue t;
	const TTriangleIterator triangle =  tree_.intersect(iRay, t, iMinT);
	if (triangle == triangles_.end())
	{
		return rNone;
	}
	oTriangle = triangle;
	oT = t;
	return rOne;
}



template <typename T, template <typename, typename, typename> class BHV, typename SH>
const bool TriangleMesh3D<T, BHV, SH>::intersects(const TRay& iRay, TParam iMinT, TParam iMaxT) const
{
	return tree_.intersects(iRay, iMinT, iMaxT);
}



template <typename T, template <typename, typename, typename> class BHV, typename SH>
void TriangleMesh3D<T, BHV, SH>::swap(TSelf& ioOther)
{
	tree_.swap(ioOther.tree_);
	triangles_.swap(ioOther.trtriangles_ee_);
	vertices_.swap(ioOther.vertices_);
	normals_.swap(ioOther.normals_);
	uvs_.swap(ioOther.uvs_);
}



template <typename T, template <typename, typename, typename> class BHV, typename SH>
const Result TriangleMesh3D<T, BHV, SH>::Triangle::intersect(const TRay& iRay, 
		TReference oT, TParam iMinT, IntersectionContext* oContext) const
{
	LASS_ASSERT(vertices[0] && vertices[1] && vertices[2]);
	const TPoint point0 = *vertices[0];
	const TVector dPoint_dR = *vertices[1] - point0;
	const TVector dPoint_dS = *vertices[2] - point0;

	TValue r, s, t;
	const Result hit = impl::intersectTriangle3D(
		point0, dPoint_dR, dPoint_dS, iRay.support(), iRay.direction(), r, s, t, iMinT);
	if (hit != rOne)
	{
		return hit;
	}

	oT = t;

	if (oContext)
	{
		TUv dRs_dU(1, 0);
		TUv dRs_dV(0, 1);
		if (uvs[0])
		{
			LASS_ASSERT(uvs[1] && uvs[2]);
			const TUv uv0 = *uvs[0];
			const typename TUv::TVector dUv_dR = *uvs[1] - uv0;
			const typename TUv::TVector dUv_dS = *uvs[2] - uv0;
			oContext->uv = uv0 + r * dUv_dR + s * dUv_dS;
		
			const TValue matrix[4] = { dUv_dR.x, dUv_dS.x, dUv_dR.y, dUv_dS.y };
			TValue solution[4] = { 1, 0, 0, 1 };
			num::impl::cramer2<TValue>(matrix, solution, solution + 4);
			dRs_dU = TUv(solution[0], solution[2]);
			dRs_dV = TUv(solution[1], solution[3]);
			oContext->dPoint_dU = dPoint_dR * dRs_dU.x + dPoint_dS * dRs_dU.y;
			oContext->dPoint_dV = dPoint_dR * dRs_dV.x + dPoint_dS * dRs_dV.y;
		}
		else
		{
			oContext->uv = TUv(r, s);
			oContext->dPoint_dU = dPoint_dR;
			oContext->dPoint_dV = dPoint_dS;
		}

		oContext->geometricNormal = cross(dPoint_dR, dPoint_dS).normal();
		if (normals[0])
		{
			LASS_ASSERT(normals[1] && normals[2]);
			const TVector normal0 = *normals[0];
			const TVector dNormal_dR = *normals[1] - normal0;
			const TVector dNormal_dS = *normals[2] - normal0;
			oContext->normal = (normal0 + r * dNormal_dR + s * dNormal_dS).normal();
			oContext->dNormal_dU = dNormal_dR * dRs_dU.x + dNormal_dS * dRs_dU.y;
			oContext->dNormal_dV = dNormal_dR * dRs_dV.x + dNormal_dS * dRs_dV.y;
		}
		else
		{
			oContext->normal = oContext->geometricNormal;
			oContext->dNormal_dU = TVector();
			oContext->dNormal_dV = TVector();
		}
	}
	return rOne;
}



template <typename T, template <typename, typename, typename> class BHV, typename SH> inline
const size_t TriangleMesh3D<T, BHV, SH>::Triangle::side(const TPoint* v) const 
{ 
	return vertices[0] == v ? 0 : (vertices[1] == v ? 1 : (vertices[2] == v ? 2 : size_t(-1))); 
}



// --- private -------------------------------------------------------------------------------------

template <typename T, template <typename, typename, typename> class BHV, typename SH>
void TriangleMesh3D<T, BHV, SH>::connectTriangles()
{
	struct Edge
	{
		Triangle* triangle;
		const TPoint* tail;
		const TPoint* head;
		Edge(Triangle* iTriangle, const TPoint* iTail, const TPoint* iHead): 
			triangle(iTriangle), tail(iTail), head(iHead) {}
		bool operator<(const Edge& iOther) const
		{
			return tail < iOther.tail || (tail == iOther.tail && head < iOther.head);
		}
	};
	typedef std::vector<Edge> TEdges;

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
			edges.push_back(Edge(&triangle, triangle.vertices[k1], triangle.vertices[k2]));
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
			const Edge bait(0, triangle.vertices[k2], triangle.vertices[k1]); // reversed edge
			const TEdges::const_iterator haul = std::lower_bound(edges.begin(), edges.end(), bait);
			if (haul != edges.end() && !(bait < *haul))
			{
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



// --- free ----------------------------------------------------------------------------------------



}

}

#endif

// EOF
