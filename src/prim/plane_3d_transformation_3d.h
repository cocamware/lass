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

#ifndef LASS_GUARDIAN_OF_INCLUSION_PRIM_PLANE_3D_TRANSFORMATION_3D_H
#define LASS_GUARDIAN_OF_INCLUSION_PRIM_PLANE_3D_TRANSFORMATION_3D_H

#include "prim_common.h"
#include "plane_3d.h"
#include "transformation_3d.h"

namespace lass
{
namespace prim
{


/** apply transformation to cartesian plane
 *  @relates Transformation3D
 */
template<typename T, class NP>
Plane3D<T, Cartesian, NP> transform(
		const Plane3D<T, Cartesian, NP>& plane, const Transformation3D<T>& transformation)
{
	const std::pair<Vector3D<T>, T> result = 
		transform(std::make_pair(plane.normal(), plane.d()), transformation);
	return Plane3D<T, Cartesian, NP>(result.first, result.second);
}



/** apply transformation to parametric plane
 *  @relates Transformation3D
 */
template<typename T, class NP>
Plane3D<T, Parametric, NP> transform(
		const Plane3D<T, Parametric, NP>& plane, const Transformation3D<T>& transformation)
{
	return Plane3D<T, Parametric, NP>(
		transform(plane.support()), transform(plane.directionU()), transform(plane.directionV()));
}



/** apply transformation to combined plane
 *  @relates Transformation3D
 */
template<typename T, class NP>
Plane3D<T, Combined, NP> transform(
		const Plane3D<T, Combined, NP>& plane, const Transformation3D<T>& transformation)
{
	return Plane3D<T, Combined, NP>(
		transform(plane.support()),	transform(plane.directionU()), transform(plane.directionV()));
}

}
}

#endif

// EOF
