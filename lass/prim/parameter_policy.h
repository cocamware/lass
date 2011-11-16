/**	@file
 *	@author Bram de Greve (bram@cocamware.com)
 *	@author Tom De Muer (tom@cocamware.com)
 *
 *	*** BEGIN LICENSE INFORMATION ***
 *	
 *	The contents of this file are subject to the Common Public Attribution License 
 *	Version 1.0 (the "License"); you may not use this file except in compliance with 
 *	the License. You may obtain a copy of the License at 
 *	http://lass.sourceforge.net/cpal-license. The License is based on the 
 *	Mozilla Public License Version 1.1 but Sections 14 and 15 have been added to cover 
 *	use of software over a computer network and provide for limited attribution for 
 *	the Original Developer. In addition, Exhibit A has been modified to be consistent 
 *	with Exhibit B.
 *	
 *	Software distributed under the License is distributed on an "AS IS" basis, WITHOUT 
 *	WARRANTY OF ANY KIND, either express or implied. See the License for the specific 
 *	language governing rights and limitations under the License.
 *	
 *	The Original Code is LASS - Library of Assembled Shared Sources.
 *	
 *	The Initial Developer of the Original Code is Bram de Greve and Tom De Muer.
 *	The Original Developer is the Initial Developer.
 *	
 *	All portions of the code written by the Initial Developer are:
 *	Copyright (C) 2004-2011 the Initial Developer.
 *	All Rights Reserved.
 *	
 *	Contributor(s):
 *
 *	Alternatively, the contents of this file may be used under the terms of the 
 *	GNU General Public License Version 2 or later (the GPL), in which case the 
 *	provisions of GPL are applicable instead of those above.  If you wish to allow use
 *	of your version of this file only under the terms of the GPL and not to allow 
 *	others to use your version of this file under the CPAL, indicate your decision by 
 *	deleting the provisions above and replace them with the notice and other 
 *	provisions required by the GPL License. If you do not delete the provisions above,
 *	a recipient may use your version of this file under either the CPAL or the GPL.
 *	
 *	*** END LICENSE INFORMATION ***
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

#include "prim_common.h"
#include "../util/enforcer.h"



namespace lass
{

namespace prim
{

/** @ingroup ParameterPolicy
 */
class ParameterError: public util::ExceptionMixin<ParameterError>
{
public:
	ParameterError(const std::string& msg, const std::string& loc): util::ExceptionMixin<ParameterError>(msg, loc) {}
	~ParameterError() throw() {}
};



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
			LASS_THROW_EX(ParameterError, "parameter '" << iT << "' outside valid range ["
				<< iMin << ", " << iMax << "].");
		}
	}

	template <class T>
	static void enforceRange(const T& iT, const T& iMin)
	{
		if (!(iT >= iMin))
		{
			LASS_THROW_EX(ParameterError, "parameter '" << iT << "' outside valid range ["
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

