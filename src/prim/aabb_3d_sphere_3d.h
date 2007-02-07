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

#ifndef LASS_GUARDIAN_OF_INCLUSION_PRIM_AABB_3D_SPHERE_3D_H
#define LASS_GUARDIAN_OF_INCLUSION_PRIM_AABB_3D_SPHERE_3D_H

#include "prim_common.h"
#include "aabb_3d.h"
#include "sphere_3d.h"

namespace lass
{
namespace prim
{

/** @relates lass::prim::Aabb3D
 *  @relates lass::prim::Sphere3D
 */
template <typename T> 
Aabb3D<T> aabb(const Sphere3D<T>& sphere)
{
	const typename Sphere3D<T>::TPoint& c = sphere.center();
	const typename Sphere3D<T>::TValue r = sphere.radius();
	const Vector3D<T> extent(r, r, r);
	return Aabb3D<T>(c - extent, c + extent);
}



/** @relates lass::prim::Aabb3D
 *  @relates lass::prim::Sphere3D
 */
template <typename T>
Sphere3D<T> boundingSphere(const Aabb3D<T>& box)
{
	const typename Aabb3D<T>::TPoint center = box.center().affine();
	const typename Aabb3D<T>::TValue radius = box.size().norm() / 2;
	return Sphere3D<T>(center, radius);
}

}
}

#endif

// EOF
