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

#include "pylass_common.h"
#include "mesh_interpolator.h"

namespace mesh_interpolator
{

PY_DECLARE_CLASS(LinearMeshInterpolator)
PY_CLASS_CONSTRUCTOR_2(LinearMeshInterpolator, const LinearMeshInterpolator::TAabb2D&, LinearMeshInterpolator::TPointInfo)
PY_CLASS_METHOD_DOC(LinearMeshInterpolator, insertSite,
	"insertSite((x, y), point_info)\n"
	"Insert a site (x, y) with a known value point_info\n")
PY_CLASS_METHOD_DOC(LinearMeshInterpolator, interpolate,
	"interpolate((x, y))\n"
	"returns the linear interpolated point_info at position (x, y)\n")
PY_CLASS_METHOD_DOC(LinearMeshInterpolator, interpolateList,
	"interpolateList([(x, y), ...])\n"
	"returns the list of linear interpolated point_info at positions [(x, y), ...]\n")
PY_CLASS_METHOD_DOC(LinearMeshInterpolator, interpolateLine,
	"interpolateList(iStartPoint,iEndPoint,dDistance)\n"
	"returns the list of linear interpolated point_info along the defined line\n")

LinearMeshInterpolator::LinearMeshInterpolator(const TAabb2D& iBox, TPointInfo iValueOnBox):
	interpolator_(iBox, iValueOnBox)
{
}



void LinearMeshInterpolator::insertSite(const TPoint2D& iPoint, TPointInfo iPointInfo)
{
	interpolator_.insertSite(iPoint, iPointInfo);
}



LinearMeshInterpolator::TPointInfo
LinearMeshInterpolator::interpolate(const TPoint2D& iQuery) const
{
	return interpolator_.interpolate(iQuery);
}

std::vector< LinearMeshInterpolator::TPointInfo >
LinearMeshInterpolator::interpolateList(const std::vector<TPoint2D>& iQuery) const
{
	std::vector<TPointInfo> result;
	result.reserve(iQuery.size());
	for (size_t i=0;i<iQuery.size();++i)
		result.push_back(interpolate(iQuery[i]));
	return result;
}

std::vector< LinearMeshInterpolator::TPointInfo >
LinearMeshInterpolator::interpolateLine(const TPoint2D& iPointS, const TPoint2D& iPointE, TValue iDx) const
{
	const TValue tX = iPointE.x - iPointS.x;
	const TValue tY = iPointE.y - iPointS.y;

	const TValue tD = distance(iPointS,iPointE);
	const TValue dx = tX/(tD/iDx);
	const TValue dy = tY/(tD/iDx);

	std::vector<TPointInfo> result;
	result.reserve(static_cast<size_t>(std::max<TValue>(tD/iDx, 0)) + 1);
	TValue x = iPointS.x;
	TValue y = iPointS.y;
	while (x<=iPointE.x)
	{
		result.push_back(interpolate(TPoint2D(x,y)));
		x += dx;
		y += dy;
	}
	return result;
}

}
