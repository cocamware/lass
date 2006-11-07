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

#ifndef LASS_GUARDIAN_OF_INCLUSION_PRIM_IMPL_AABB_SLAB_H
#define LASS_GUARDIAN_OF_INCLUSION_PRIM_IMPL_AABB_SLAB_H

#include "../prim_common.h"
#include "../../num/floating_point_consistency.h"

namespace lass
{
namespace prim
{
namespace impl
{

/** @internal
 */
template <typename T> inline
bool interectSlab(const T& iMin, const T& iMax, const T& iSupport, const T& iDirection, T& ioTNear, T& ioTFar)
{
	typedef num::Consistent<T> TConsistent;
	
	if (iDirection == num::NumTraits<T>::zero)
	{
		return iSupport >= iMin && iSupport <= iMax;
	}
	
	const T invDirection = num::inv(iDirection);
	TConsistent tNear = (iMin - iSupport) * invDirection;
	TConsistent tFar = (iMax - iSupport) * invDirection;
	if (tFar < tNear)
	{
		std::swap(tNear, tFar);
	}

	if (tNear > ioTFar || tFar < ioTNear)
	{
		return false;
	}
	ioTNear = std::max(ioTNear, tNear.value());
	ioTFar = std::min(ioTFar, tFar.value());
	return true;
}

}

}

}

#endif

// EOF
