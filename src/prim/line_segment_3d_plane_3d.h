/** @file
 *  @author Bram de Greve (bramz@users.sourceforge.net)
 *  @author Tom De Muer (tomdemuer@users.sourceforge.net)
 *
 *  Distributed under the terms of the GPL (GNU Public License)
 *
 *  The LASS License:
 *
 *  Copyright 2004 Bram de Greve and Tom De Muer
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

#ifndef LASS_GUARDIAN_OF_INCLUSION_PRIM_LINE_SEGMENT_3D_PLANE_3D_H
#define LASS_GUARDIAN_OF_INCLUSION_PRIM_LINE_SEGMENT_3D_PLANE_3D_H

#include "prim_common.h"
#include "line_segment_3d.h"
#include "plane_3d.h"

namespace lass
{
namespace prim
{

template<typename T, class EP, class NP, class PP>
Result intersect(const Plane3D<T, EP, NP>& iPlane,
				 const LineSegment3D<T, PP>& iSegment,
				 T& oT, const T& iMinT = T());

template <typename T, class EP, class NP, class PP>
LineSegment3D<T, PP> reflect(const Plane3D<T, EP, NP>& iPlane, const LineSegment3D<T, PP>& iSegment);

template <typename T, class EP, class NP, class PP>
LineSegment3D<T, PP> project(const Plane3D<T, EP, NP>& iPlane, const LineSegment3D<T, PP>& iSegment);

}

}

#include "line_segment_3d_plane_3d.inl"

#endif

// EOF