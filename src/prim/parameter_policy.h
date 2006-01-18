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



/** @defgroup ParameterPolicy ParameterPolicy
 *  @brief policies to specify the rules for parameters without an inifite range.
 *  @author Bram de Greve
 *  @date 2003
 *
 *  Of some primitives that can use a parameter to locate a point on its surface (like
 *  Line3D, Plane3D, ...), some will not have points for all possible values of the
 *  parameter (like Ray3D, LineSegment3D, ...).  For a Ray3D, only the parameters t >= 0
 *  are valid.  For LineSegment3D, only 0 <= t <= 1 is valid.
 */





#ifndef LASS_GUARDIAN_OF_INCLUSION_PRIM_PARAMETER_POLICY_H
#define LASS_GUARDIAN_OF_INCLUSION_PRIM_PARAMETER_POLICY_H
#pragma once

#include "prim_common.h"
#include "../util/enforcer.h"



namespace lass
{

namespace prim
{

/** @ingroup ParameterPolicy
 *  @brief Parameters supplied to functions must be in the range of the primitive.
 *  @author Bram de Greve [BdG]
 *  @date 2003
 *
 *  An example: of a ray (origin + t * direction), only values t >= 0 are points of the ray.
 *  Here all parameters t supplied to functions of the ray must be positive.  If not,
 *  an exception is thrown.
 */
struct Bounded
{
	template <class T>
	static void enforceRange(const T& iT, const T& iMin, const T& iMax)
	{
		if (!(iT >= iMin && iT <= iMax))
		{
			LASS_THROW("parameter '" << iT << "' outside valid range ["
				<< iMin << ", " << iMax << "].");
		}
	}

	template <class T>
	static void enforceRange(const T& iT, const T& iMin)
	{
		if (!(iT >= iMin))
		{
			LASS_THROW("parameter '" << iT << "' outside valid range ["
				<< iMin << ", +inf].");
		}
	}
};



/** @ingroup ParameterPolicy
 *  @brief Parameters supplied to functions can go out of the range of the primitive.
 *  @author Bram de Greve [BdG]
 *  @date 2003
 *
 *  An example: of a ray (origin + t * direction), only values t >= 0 are points of the ray.
 *  Here also negative values for t will be accepted by functions of the ray, as long as
 *  no other conditions are broken.  However, it is no longer garanteed that results make sense.
 *  i.e. with this policy, Ray3D<>::point will also return a point for t < 0, but it won't be
 *  a point of the ray.  Though, it can be a usefull policy if we need to be able to get
 *  points beyond the origin of the ray.
 */
struct Unbounded
{
	template <typename T>
	static void enforceRange(const T& /*iT*/, const T& /*iMin*/, const T& /*iMax*/)
	{
	}

	template <typename T>
	static void enforceRange(const T& /*iT*/, const T& /*iMin*/)
	{
	}
};

}

}

#endif

// EOF

