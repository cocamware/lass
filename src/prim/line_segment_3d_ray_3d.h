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

#ifndef LASS_GUARDIAN_OF_INCLUSION_PRIM_LINE_SEGMENT_3D_RAY_3D_H
#define LASS_GUARDIAN_OF_INCLUSION_PRIM_LINE_SEGMENT_3D_RAY_3D_H

#include "prim_common.h"
#include "line_segment_3d.h"
#include "ray_3d.h"

namespace lass
{
namespace prim
{

template<typename T, class PP1, class NP2, class PP2>
T distance(const LineSegment3D<T, PP1>& iLineSegment,
		   const Ray3D<T, NP2, PP2>& iRay,
  		   const T& iMinT = T());

template<typename T, class PP1, class NP2, class PP2>
T squaredDistance(const LineSegment3D<T, PP1>& iLineSegment,
				  const Ray3D<T, NP2, PP2>& iRay,
				  const T& iMinT = T());

template<typename T, class PP1, class NP2, class PP2>
T closestsPoints(const LineSegment3D<T, PP1>& iLineSegment,
						  const Ray3D<T, NP2, PP2>& iRay,
						  T &tCilinder,
						  T &tRay,
						  const T& iMinT = T());
}
}

#include "line_segment_3d_ray_3d.inl"

#endif

// EOF