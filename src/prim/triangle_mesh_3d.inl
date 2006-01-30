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

namespace lass
{
namespace prim
{

// --- public --------------------------------------------------------------------------------------

template <typename T, template <typename, typename> class BHV>
template <typename VertexInputRange, typename NormalInputRange,
	typename UvInputRange, typename IndexTriangleInputRange>
TriangleMesh3D<T, BHV>::TriangleMesh3D(const VertexInputRange& iVertices, 
									   const NormalInputRange& iNormals,
									   const UvInputRange& iUvs, 
									   const IndexTriangleInputRange& iTriangles):
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
		for (std::size_t k = 0; k < dimension; ++k)
		{
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
			}
			else
			{
				triangle.uvs[k] = 0;
			}
		}
		triangles_.push_back(triangle);
	}

	tree_.reset(triangles_.begin(), triangles_.end());
}



template <typename T, template <typename, typename> class BHV>
const typename TriangleMesh3D<T, BHV>::TTriangles&
TriangleMesh3D<T, BHV>::triangles() const
{
	return triangles_;
}



template <typename T, template <typename, typename> class BHV>
const typename TriangleMesh3D<T, BHV>::TVertices&
TriangleMesh3D<T, BHV>::vertices() const
{
	return vertices_;
}



template <typename T, template <typename, typename> class BHV>
const typename TriangleMesh3D<T, BHV>::TNormals&
TriangleMesh3D<T, BHV>::normals() const
{
	return normals_;
}



template <typename T, template <typename, typename> class BHV>
const typename TriangleMesh3D<T, BHV>::TUvs&
TriangleMesh3D<T, BHV>::uvs() const
{
	return uvs_;
}



template <typename T, template <typename, typename> class BHV>
const typename TriangleMesh3D<T, BHV>::TAabb
TriangleMesh3D<T, BHV>::aabb() const
{
	return tree_.aabb();
}



template <typename T, template <typename, typename> class BHV>
const typename TriangleMesh3D<T, BHV>::TValue
TriangleMesh3D<T, BHV>::area() const
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



template <typename T, template <typename, typename> class BHV>
void TriangleMesh3D<T, BHV>::smoothNormals(TParam iMaxAngleInRadians)
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
				std::size_t j = triangle.vertices[k] - &vertices_[0];
				vertexNormals[j] += n * num::acos(-dot(edges[prevK], edges[k]));
			}
		}
	}
	stde::for_each_r(vertexNormals, std::mem_fun_ref(TVector::normalize));

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
			std::size_t j = triangle.vertices[k] - &vertices_[0];
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
	for (std::size_t j = 0; j < numVertices; ++j)
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
			std::size_t j = triangle.vertices[k] - &vertices_[0];
			if (dot(faceNormals[i], vertexNormals[j]) >= minDot)
			{
				LASS_ASSERT(usedVertexNormals[j] != IndexTriangle::null())
				triangle.normals[k] = &normals_[usedVertexNormals[j]];
			}
			else
			{
				LASS_ASSERT(usedFaceNormals[i] != IndexTriangle::null())
				triangle.normals[k] = &normals_[usedFaceNormals[i]];
			}
		}
	}
}



template <typename T, template <typename, typename> class BHV>
const Result TriangleMesh3D<T, BHV>::intersect(const TRay& iRay, TTriangleIterator& oTriangle,
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



template <typename T, template <typename, typename> class BHV>
const bool TriangleMesh3D<T, BHV>::intersects(const TRay& iRay, TParam iMinT, TParam iMaxT) const
{
	return tree_.intersects(iRay, iMinT, iMaxT);
}



template <typename T, template <typename, typename> class BHV>
const Result TriangleMesh3D<T, BHV>::Triangle::intersect(const TRay& iRay, 
														 TReference oT, 
														 TParam iMinT, 
														 IntersectionContext* oContext) const
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




// --- private -------------------------------------------------------------------------------------


// --- free ----------------------------------------------------------------------------------------



}

}

#endif

// EOF
