/**	@file
 *  @internal
 *	@author Bram de Greve (bramz@users.sourceforge.net)
 *	@author Tom De Muer (tomdemuer@users.sourceforge.net)
 *
 *	Distributed under the terms of the GPL (GNU Public License)
 *
 * 	The LASS License:
 *
 *	Copyright 2004 Bram de Greve and Tom De Muer
 *
 *	LASS is free software; you can redistribute it and/or modify
 *	it under the terms of the GNU General Public License as published by
 *	the Free Software Foundation; either version 2 of the License, or
 *	(at your option) any later version.
 *
 *	This program is distributed in the hope that it will be useful,
 *	but WITHOUT ANY WARRANTY; without even the implied warranty of
 *	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *	GNU General Public License for more details.
 *
 *	You should have received a copy of the GNU General Public License
 *	along with this program; if not, write to the Free Software
 *	Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */



#ifndef LASS_GUARDIAN_OF_INCLUSION_TEST_TEST_PRIM_XML_INL
#define LASS_GUARDIAN_OF_INCLUSION_TEST_TEST_PRIM_XML_INL

#include "test_common.h"

#include "../prim/aabb_2d.h"
#include "../prim/aabb_3d.h"
#include "../prim/line_2d.h"
#include "../prim/line_3d.h"
#include "../prim/line_segment_2d.h"
#include "../prim/line_segment_3d.h"
#include "../prim/plane_3d.h"
#include "../prim/point_2d.h"
#include "../prim/point_2dh.h"
#include "../prim/point_3d.h"
#include "../prim/point_3dh.h"
#include "../prim/ray_2d.h"
#include "../prim/ray_3d.h"
#include "../prim/simple_polygon_2d.h"
#include "../prim/simple_polygon_3d.h"
#include "../prim/sphere_3d.h"
#include "../prim/triangle_2d.h"
#include "../prim/triangle_3d.h"
#include "../prim/vector_2d.h"
#include "../prim/vector_3d.h"
#include "../prim/vector_4d.h"
#include "../io/xml_o_file.h"

namespace lass
{
namespace test
{

void testPrimXml()
{
	using namespace prim;

	io::XmlOFile xml("prim.xml");
	xml << "<test>" << std::endl;

	xml << Aabb2D<float>(Point2D<float>(1.f, 2.f), Point2D<float>(4.f, 5.f));
	xml << Aabb3D<float>(Point3D<float>(1.f, 2.f, 3.f), Point3D<float>(4.f, 5.f, 6.f));

	xml << Line2D<float, Parametric>(Point2D<float>(1.f, 2.f), Vector2D<float>(4.f, 5.f));
	xml << Line3D<float>(Point3D<float>(1.f, 2.f, 3.f), Vector3D<float>(4.f, 5.f, 6.f));

	xml << LineSegment2D<float>(Point2D<float>(1.f, 2.f), Point2D<float>(4.f, 5.f));
	xml << LineSegment3D<float>(Point3D<float>(1.f, 2.f, 3.f), Point3D<float>(4.f, 5.f, 6.f));

	xml << Plane3D<float>(Vector3D<float>(1.f, 2.f, 3.f), 4.f);

	xml << Point2D<float>(1.f, 2.f);
	xml << Point2DH<float>(1.f, 2.f, 3.f);
	xml << Point3D<float>(1.f, 2.f, 3.f);
	xml << Point3DH<float>(1.f, 2.f, 3.f, 4.f);

	xml << Ray2D<float>(Point2D<float>(1.f, 2.f), Vector2D<float>(4.f, 5.f));
	xml << Ray3D<float>(Point3D<float>(1.f, 2.f, 3.f), Vector3D<float>(4.f, 5.f, 6.f));

	SimplePolygon2D<float> p2;
	p2.add(Point2D<float>(0.f, 0.f));
	p2.add(Point2D<float>(1.f, 0.f));
	p2.add(Point2D<float>(1.f, 1.f));
	p2.add(Point2D<float>(0.f, 1.f));
	xml << p2;

	SimplePolygon3D<float> p3(Plane3D<float>(Vector3D<float>(0.f, 0.f, 1.f), -1.f));
	p3.add(Point3D<float>(0.f, 0.f, 1.f));
	p3.add(Point3D<float>(1.f, 0.f, 1.f));
	p3.add(Point3D<float>(1.f, 1.f, 1.f));
	p3.add(Point3D<float>(0.f, 1.f, 1.f));
	xml << p3;

	xml << Sphere3D<float>(Point3D<float>(1.f, 2.f, 3.f), 4.f);
	
	xml << Triangle2D<float>(Point2D<float>(1.f, 2.f), Point2D<float>(1.f, 8.f), Point2D<float>(7.f, 8.f));
	xml << Triangle3D<float>(Point3D<float>(1.f, 2.f, 3.f), Point3D<float>(1.f, 8.f, 6.f), Point3D<float>(7.f, 8.f, 9.f));

	xml << Vector2D<float>(1.f, 2.f);
	xml << Vector3D<float>(1.f, 2.f, 3.f);
	xml << Vector4D<float>(1.f, 2.f, 3.f, 4.f);

	xml << "</test>" << std::endl;
}



}

}

#endif

// EOF
