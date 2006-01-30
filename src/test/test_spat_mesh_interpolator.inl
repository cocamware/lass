/** @file
 *  @internal
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



#include "test_common.h"
#include "test_spat.h"

#include "../spat/mesh_interpolator.h"
#include "../io/matlab_o_stream.h"

namespace lass
{
namespace test
{
	void doTestMeshInterpolator()
	{
		typedef prim::Aabb2D<double> TAabb2D;
		typedef prim::Point2D<double> TPoint2D;
		typedef spat::LinearMeshInterpolator<double, double>    TLinearMeshInterpolator;
		TAabb2D aabb(TPoint2D(0,0),TPoint2D(100,100));
		TLinearMeshInterpolator interpolator(aabb, 0);

		for (double y=10;y<90;y+=20)
		{
			for (double x=10;x<90;x+=20)
			{
				interpolator.insertSite(TPoint2D(x, y), x*y);
			}
		}

		std::ofstream meshIO("meshinterp.txt");
		for (double y=10;y<90;y+=2)
		{
			for (double x=10;x<90;x+=2)
			{
				double value = interpolator.interpolate(TPoint2D(x, y));
				meshIO << value << "\t";
			}
			meshIO << std::endl;
		}
		meshIO.close();
	}
}
}
