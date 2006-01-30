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

namespace lass
{
namespace prim
{


template<typename T, class DegeneratePolicy>
bool triangulate(const SimplePolygon2D<T, DegenerationPolicy>& iPolygon,
				 std::vector<Triangle2D<T> >& oTriangles)
{
	oTriangles.clear();
	if (iPolygon.isConvex())
	{
		// this is the easy one :)
		int i=0;
		for (;i<iPolygon.size()-3;++i)
			oTriangles.push_back(Triangle2D<T>(iPolygon[0],iPolygon[i+1],iPolygon[i+2] ) );
		return true;
	}

	// we implement the easiest algorithm: ear clipping
	// we try to find a non-reflex vertex and then clip it

	SimplePolygon2D<T, DegenerationPolicy> temp(iPolygon);
	while (temp.size()>3)
	{
		int i=0;
		for (;i<temp.size();++i)
		{
			if (temp.isReflex(i))
			{
				oTriangles.push_back(Triangle2D<T>(temp[i-1],temp[i],temp[i+1]));
				temp.erase(i);
				break;
			}
		}
	}
	oTriangles.push_back(Triangle2D<T>(temp[0],temp[1],temp[2]));
	return true;
}

}
}
