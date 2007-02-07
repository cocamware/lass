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

#ifndef LASS_GUARDIAN_OF_INCLUSION_PRIM_SIMPLE_POLYGON_2D_TRIANGLE_2D_H
#define LASS_GUARDIAN_OF_INCLUSION_PRIM_SIMPLE_POLYGON_2D_TRIANGLE_2D_H

#include "prim_common.h"
#include "simple_polygon_2d.h"
#include "triangle_2d.h"

namespace lass
{
namespace prim
{

template <typename T, class DegeneratePolicy, typename OutputIterator>
OutputIterator triangulate(
		const SimplePolygon2D<T, DegenerationPolicy>& polygon, OutputIterator triangles)
{
	if (polygon.isConvex())
	{
		// this is the easy one :)
		const size_t n = polygon.size() - 3;
		for (size_t i = 0; i < n; ++i)
		{
			*triangles++ = Triangle2D<T>(polygon[0],polygon[i+1],polygon[i+2]);
		}
		return triangles;
	}

	// we implement the easiest algorithm: ear clipping
	// we try to find a non-reflex vertex and then clip it

#pragma LASS_FIXME("This naive implementation of ear clipping is broken, let's fix it later =) [Bramz]")
	LASS_THROW("triangulation of concave polygons is not implemented yet [Bramz]");
	//SimplePolygon2D<T, DegenerationPolicy> temp(polygon);
	//while (temp.size()>3)
	//{
	//	const int n = temp.size();
	//	for (int i=0;i<n;++i)
	//	{
	//		if (temp.isReflex(i))
	//		{
	//			*triangles++ = push_back(Triangle2D<T>(temp.at(i-1),temp[i],temp.at(i+1)));
	//			temp.erase(i);
	//			break;
	//		}
	//	}
	//}
	//*triangles++ = Triangle2D<T>(temp[0],temp[1],temp[2]);
	//return triangles;
}

}
}

#endif
