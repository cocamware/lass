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

#ifndef LASS_GUARDIAN_OF_INCLUSION_PRIM_AABB_2D_TRIANGLE_2D_H
#define LASS_GUARDIAN_OF_INCLUSION_PRIM_AABB_2D_TRIANGLE_2D_H

#include "prim_common.h"
#include "aabb_2d.h"
#include "triangle_2d.h"

namespace lass
{
namespace prim
{

/** determine axis aligned bounding box of a 2D triangle
 *  @relates Aabb2D
 */
template <typename T> 
Aabb2D<T> aabb(const Triangle2D<T>& triangle)
{
	Aabb2D<T, AutoMinMax> result(triangle[0], triangle[1]);
	result += triangle[2];
	return result;
}

}
}

#endif

// EOF
