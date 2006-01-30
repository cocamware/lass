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

#ifndef LASS_GUARDIAN_OF_INCLUSION_PRIM_SIMPLE_POLYGON_2D_TRIANGLE_2D_INL
#define LASS_GUARDIAN_OF_INCLUSION_PRIM_SIMPLE_POLYGON_2D_TRIANGLE_2D_INL

#include "simple_polygon_2d_triangle_2d.h"

namespace lass
{
namespace prim
{

template <typename T, class DegeneratePolicy, typename OutputIterator>
OutputIterator triangulate(const SimplePolygon2D<T, DegenerationPolicy>& iPolygon,
						   OutputIterator oTriangles)
{
	if (iPolygon.isConvex())
	{
		// this is the easy one :)
		const size_t n = iPolygon.size() - 3;
		for (size_t i = 0; i < n; ++i)
		{
			*oTriangles++ = Triangle2D<T>(iPolygon[0],iPolygon[i+1],iPolygon[i+2]);
		}
		return oTriangles;
	}

	// we implement the easiest algorithm: ear clipping
	// we try to find a non-reflex vertex and then clip it

	SimplePolygon2D<T, DegenerationPolicy> temp(iPolygon);
	while (temp.size()>3)
	{
		const int n = temp.size();
		for (int i=0;i<n;++i)
		{
			if (temp.isReflex(i))
			{
				*oTriangles++ = push_back(Triangle2D<T>(temp.at(i-1),temp[i],temp.at(i+1)));
				temp.erase(i);
				break;
			}
		}
	}
	*oTriangles++ = Triangle2D<T>(temp[0],temp[1],temp[2]);
	return oTriangles;
}

}

}

#endif

// EOF
