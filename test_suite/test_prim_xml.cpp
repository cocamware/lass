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

#include "test_common.h"

#include "../lass/prim/aabb_2d.h"
#include "../lass/prim/aabb_3d.h"
#include "../lass/prim/line_2d.h"
#include "../lass/prim/line_3d.h"
#include "../lass/prim/line_segment_2d.h"
#include "../lass/prim/line_segment_3d.h"
#include "../lass/prim/plane_3d.h"
#include "../lass/prim/point_2d.h"
#include "../lass/prim/point_2dh.h"
#include "../lass/prim/point_3d.h"
#include "../lass/prim/point_3dh.h"
#include "../lass/prim/ray_2d.h"
#include "../lass/prim/ray_3d.h"
#include "../lass/prim/simple_polygon_2d.h"
#include "../lass/prim/simple_polygon_3d.h"
#include "../lass/prim/sphere_3d.h"
#include "../lass/prim/triangle_2d.h"
#include "../lass/prim/triangle_3d.h"
#include "../lass/prim/vector_2d.h"
#include "../lass/prim/vector_3d.h"
#include "../lass/prim/vector_4d.h"
#include "../lass/io/xml_o_file.h"

namespace lass
{
namespace test
{

void testPrimXml()
{
	using namespace prim;

	io::XmlOFile xml("prim.xml", "root");

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

	Vector3D<float> n(0.f, 0.f, 1.f);
	SimplePolygon3D<float> p3(Plane3D<float>(n, -1.f));
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
}

TUnitTest test_prim_xml()
{
	return TUnitTest(1, LASS_TEST_CASE(testPrimXml));
}

}

}

// EOF
