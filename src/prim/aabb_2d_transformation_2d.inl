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

#ifndef LASS_GUARDIAN_OF_INCLUSION_PRIM_AABB_2D_TRANSFORMATION_2D_INL
#define LASS_GUARDIAN_OF_INCLUSION_PRIM_AABB_2D_TRANSFORMATION_2D_INL
#pragma once

#include "prim_common.h"
#include "aabb_2d_transformation_2d.h"

namespace lass
{
namespace prim
{

/** apply transformation to axis aligned bounding box
 *  @relates Transformation3D
 */
template<typename T, class MMP>
Aabb2D<T, MMP> transform(const Aabb2D<T, MMP>& iSubject, 
						 const Transformation2D<T>& iTransformation)
{
	typedef Aabb2D<T, MMP> TAabb;
	typedef typename TAabb::TPoint TPoint;
	typedef typename TAabb::TVector TVector;

	const TVector delta = iSubject.size();
	const TPoint p0 = transform(iSubject.min(), iTransformation);
	const TPoint px = p0 + transform(TVector(delta.x, 0), iTransformation);
	const TVector dy = transform(TVector(0, delta.y), iTransformation);

	TAabb result(p0, px);
	result += (p0 + dy);
	result += (px + dy);
	return result;
}

}

}

#endif
