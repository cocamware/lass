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

#ifndef LASS_GUARDIAN_OF_INCLUSION_PRIM_AABB_3D_TRANSFORMATION_3D_H
#define LASS_GUARDIAN_OF_INCLUSION_PRIM_AABB_3D_TRANSFORMATION_3D_H

#include "prim_common.h"
#include "aabb_3d.h"
#include "transformation_3d.h"

namespace lass
{
namespace prim
{

/** apply transformation to axis aligned bounding box
 *  @relates Transformation3D
 */
template<typename T, class MMP>
Aabb3D<T, MMP> transform(const Aabb3D<T, MMP>& subject, const Transformation3D<T>& transformation)
{
	typedef Aabb3D<T, MMP> TAabb;
	typedef typename TAabb::TPoint TPoint;
	typedef typename TAabb::TVector TVector;

	if (subject.isEmpty())
	{
		return subject;
	}

	const TVector delta = subject.size();
	const TPoint p0 = transform(subject.min(), transformation);
	const TPoint px = p0 + transform(TVector(delta.x, 0, 0), transformation);
	const TVector dy = transform(TVector(0, delta.y, 0), transformation);
	const TVector dz = transform(TVector(0, 0, delta.z), transformation);

	TAabb result = Aabb3D<T, AutoMinMax>(p0, px);
	result += (p0 + dy);
	result += (px + dy);
	result += (p0 + dz);
	result += (px + dz);
	const TVector dyz = dy + dz;
	result += (p0 + dyz);
	result += (px + dyz);

	return result;
}

}
}

#endif

// EOF
