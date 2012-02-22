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
 *	Copyright (C) 2004-2011 the Initial Developer.
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

#include "test_common.h"

#include "../lass/prim/triangle_mesh_3d.h"
#include "../lass/spat/aabb_tree.h"
#include "../lass/stde/iterator_range.h"
#include "../lass/stde/empty_range.h"
#include "../lass/stde/extended_cstring.h"
#include "../lass/io/file_attribute.h"

namespace lass
{
namespace test
{

namespace
{

typedef prim::TriangleMesh3D<double, spat::AabbTree, spat::DefaultSplitHeuristics> TMesh;
typedef TMesh::TPoint TPoint;
typedef TMesh::TVector TVector;
typedef TMesh::TUv TUv;
typedef TMesh::TValue TValue;
typedef TMesh::TNumTraits TNumTraits;

void perfectSphere(TPoint& vertex, TVector* normal1, TVector* normal2, TUv* uv1, TUv* uv2, const size_t* attr1, const size_t* attr2)
{
	// don't trust the uv coordinates. they get twisted around the poles.
	// renormalize vertex, and recalculate uv, but take care of the 2pi-0 seem
	const TVector normal = vertex.position().normal();
	vertex = TPoint(normal);
	if (normal1) *normal1 = normal;
	if (normal2) *normal2 = normal;

	LASS_ASSERT( !(normal.x == 0 && normal.y == 0) ); // not gonna call this at the poles.
	const TValue v = std::min( num::acos( normal.z ) / TNumTraits::pi, TNumTraits::one );
	if ( normal.x == 0 )
	{
		// we're on the seem.  u must be 0 or 1, depending on what side your on.  u already has an interpolated value, just round it to the integer.
		if (uv1) *uv1 = TUv(num::round(uv1->x), v);
		if (uv2) *uv2 = TUv(num::round(uv2->x), v);
	}
	else
	{
		const TValue u = num::fractional( num::atan2( normal.y, normal.x ) / (2 * TNumTraits::pi) + 1); // RANGE ???
		if (uv1) *uv1 = TUv(u, v);
		if (uv2) *uv2 = TUv(u, v);
	}
}

struct ObjIndex
{
	ObjIndex(size_t i): cIndex(i) {}
	size_t cIndex;
};
std::ostream& operator<<(std::ostream& stream, ObjIndex i)
{
	if ( i.cIndex != prim::IndexTriangle::null() )
	{
		stream << (i.cIndex + 1);
	}
	return stream;
}



template <typename MeshType>
void writeObj(const MeshType& mesh, const std::string& path)
{
	std::ofstream file(path.c_str());

	for (typename MeshType::TVertexIterator i = mesh.vertices().begin(), end = mesh.vertices().end(); i != end; ++i)
	{
		file << "v " << i->x << " " << i->y << " " << i->z << "\n";
	}
	for (typename MeshType::TNormalIterator i = mesh.normals().begin(), end = mesh.normals().end(); i != end; ++i)
	{
		file << "vn " << i->x << " " << i->y << " " << i->z << "\n";
	}
	for (typename MeshType::TUvIterator i = mesh.uvs().begin(), end = mesh.uvs().end(); i != end; ++i)
	{
		file << "vt " << i->x << " " << i->y << "\n";
	}

	typedef std::vector<typename MeshType::TIndexTriangle> TTriangles;
	TTriangles triangles;
	mesh.indexTriangles(std::back_inserter(triangles));
	for (typename TTriangles::const_iterator i = triangles.begin(), end = triangles.end(); i != end; ++i)
	{
		file << "f";
		for (size_t k = 0; k < 3; ++k)
		{
			file << " " << ObjIndex(i->vertices[k]) << "/" << ObjIndex(i->normals[k]) << "/" << ObjIndex(i->uvs[k]);
		}
		file << "\n";
	}
}

void testPrimTriangleMesh3D()
{
	// we'll start with an octahedron.
	// be carefull with the seem 2pi-0 though!  
	// Both sides must have different uv coordinates, otherwise interpolation is a wreck.
	// therefore, we have 6 verts, but 15 uvs!

	TPoint verts[6] = {
		TPoint(1, 0, 0), // 0
		TPoint(0, 1, 0), // 1
		TPoint(-1, 0, 0), // 2
		TPoint(0, -1, 0), // 3
		TPoint(0, 0, 1), // 4
		TPoint(0, 0, -1) // 5
	};
	TUv uvs[13] = { 
		TUv(0, 0), // 0
		TUv(.25, 0), // 1
		TUv(.5, 0), // 2
		TUv(.75, 0), // 3

		TUv(0, .5), // 4
		TUv(.25, .5), // 5
		TUv(.5, .5), // 6
		TUv(.75, .5), // 7
		TUv(1, .5), // 8

		TUv(0, 1), // 9
		TUv(.25, 1), // 10
		TUv(.5, 1), // 11
		TUv(.75, 1), // 12
	};

	const size_t null = prim::IndexTriangle::null();
	prim::IndexTriangle triangles[8] = {
		{ { 0, 1, 4 }, { null, null, null }, { 4, 5, 0 } },
		{ { 1, 2, 4 }, { null, null, null }, { 5, 6, 1 } },
		{ { 2, 3, 4 }, { null, null, null }, { 6, 7, 2 } },
		{ { 3, 0, 4 }, { null, null, null }, { 7, 8, 3 } }, 
		{ { 0, 5, 1 }, { null, null, null }, { 4, 9, 5 } },
		{ { 1, 5, 2 }, { null, null, null }, { 5, 10, 6 } }, 
		{ { 2, 5, 3 }, { null, null, null }, { 6, 11, 7 } },  
		{ { 3, 5, 0 }, { null, null, null }, { 7, 12, 8 } },  
		};

	TMesh mesh( stde::range(verts), stde::empty_range<TVector>(), stde::range(uvs), stde::range(triangles) );
	writeObj(mesh, io::fileJoinPath(test::outputDir(), "triangle_mesh_base.obj"));



	TMesh loopMesh = mesh;
	for (unsigned level = 0; level < 4; ++level)
	{
		loopMesh.loopSubdivision(1);
		writeObj(loopMesh, io::fileJoinPath(test::outputDir(), stde::safe_format("triangle_mesh_loop_%d.obj", level)));
	}

	TMesh snapMesh = mesh;
	for (unsigned level = 0; level < 4; ++level)
	{
		snapMesh.subdivisionWithLimitSurface(1, perfectSphere);
		writeObj(snapMesh, io::fileJoinPath(test::outputDir(), stde::safe_format("triangle_mesh_snap_%d.obj", level)));
	}

	TMesh adaptiveMesh = mesh;
	TMesh::TTriangleIterators selected;
	selected.push_back(adaptiveMesh.triangles().begin());
	for (unsigned level = 0; level < 5; ++level)
	{
		adaptiveMesh.subdivisionWithLimitSurface(1, perfectSphere, selected);
		writeObj(adaptiveMesh, io::fileJoinPath(test::outputDir(), stde::safe_format("triangle_mesh_snap_adaptive_%d.obj", level)));
	}
}

}

TUnitTest test_prim_triangle_mesh()
{
	return TUnitTest(1, LASS_TEST_CASE(testPrimTriangleMesh3D));
}

}
}