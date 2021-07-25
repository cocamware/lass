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



/** @defgroup DegeneratePolicy DegeneratePolicy
 *  @brief policies to specify the rules for degenerate polygons and others..
 *  @author Bram de Greve
 *  @date 2003
 *
 *  Polygons and triangles can be degenerate.  e.g. when they have less than
 *  3 vertices, or some vertices are coincident, or colinear.  We provide a few
 *  policies to deal with this.
 */

#ifndef LASS_GUARDIAN_OF_INCLUSION_PRIM_DEGENERATE_POLICY_H
#define LASS_GUARDIAN_OF_INCLUSION_PRIM_DEGENERATE_POLICY_H


#include "prim_common.h"
#include "../util/enforcer.h"

namespace lass
{
namespace prim
{

/** @ingroup DegeneratePolicy
 */
class DegenerationError: public util::ExceptionMixin<DegenerationError>
{
public:
	DegenerationError(const std::string& msg, const std::string& loc): util::ExceptionMixin<DegenerationError>(msg, loc) {}
	~DegenerationError() noexcept {}
};



/** @ingroup DegeneratePolicy
 *
 *  This is the default policy
 *
 *  NoDegenerate will not prevent a primitive to become degenerate, but it will check some
 *  basic conditions when you try to do "unsafe" operations and throws
 *  an exception on failure.  e.g. CheckDegenerate will not allow you to get the @c edge(0)
 *  or @c vector(0) of a polygon with only one vertex.
 *
 *  However, it will not check all posible conditions.  It will avoid expensive predicates.
 *  e.g. it will avoid to test if a polygon is simple, because that's too expensive.  If you
 *  want all conditions to be checked, you should use StrictNoDegenerate
 */
struct NoDegenerate
{
	template <class Primitive>
	static void enforceEdge(const Primitive& iPrimitive, int iIndexOfTailVertex)
	{
		if (iPrimitive.at(iIndexOfTailVertex) == iPrimitive.at(iIndexOfTailVertex + 1))
		{
			LASS_THROW_EX(DegenerationError, "Degenerate edge detected at tail vertex '" << iIndexOfTailVertex << "'.");
		}
	}

	template <class Primitive>
	static void enforceSimple(const Primitive& /*iPrimitive*/)
	{
	}

	template <class Primitive>
	static const typename Primitive::TValue enforceNonZeroSignedArea(const Primitive& iPrimitive)
	{
		const typename Primitive::TValue result = iPrimitive.signedArea();
		if (result == Primitive::TNumTraits::zero)
		{
			LASS_THROW_EX(DegenerationError, "Area of primitive is zero.");
		}
		return result;
	}
};



/** @ingroup DegeneratePolicy
 *
 *  This is the default policy
 *
 *  StrictNoDegenerate is similar to NoDegenerate, but will check more conditions.  However, this
 *  might go wild on cpu usage (read: be very slow).  Use this if you really need to be on the safe
 *  side.
 */
struct StrictNoDegenerate
{
	template <class Primitive>
	static void enforceEdge(const Primitive& iPrimitive, int iIndexOfTailVertex)
	{
		if (iPrimitive.at(iIndexOfTailVertex) == iPrimitive.at(iIndexOfTailVertex + 1))
		{
			LASS_THROW_EX(DegenerationError, "Degenerate edge detected at tail vertex '" << iIndexOfTailVertex << "'.");
		}
	}

	template <class Primitive>
	static void enforceSimple(const Primitive& iPrimitive)
	{
		if (!iPrimitive.isSimple())
		{
			LASS_THROW_EX(DegenerationError, "polygon is not simple.");
		}
	}

	template <class Primitive>
	static const typename Primitive::TValue enforceNonZeroSignedArea(const Primitive& iPrimitive)
	{
		const typename Primitive::TValue result = iPrimitive.signedArea();
		if (result == Primitive::TNumTraits::zero)
		{
			LASS_THROW_EX(DegenerationError, "Area of primitive is zero.");
		}
		return result;
	}
};



/** @ingroup DegeneratePolicy
 *
 *  AllowDegenerate puts the responsibility on the user.  It will assume all conditions are fullfilled
 *  and continue like nothing is wrong.  This is the "best guess" thing.
 */
struct AllowDegenerate
{
	template <class Primitive>
	static void enforceEdge(const Primitive& /*iPrimitive*/, int /*iIndexOfTailVertex*/)
	{
	}

	template <class Primitive>
	static void enforceSimple(const Primitive& /*iPrimitive*/)
	{
	}

	template <class Primitive>
	static const typename Primitive::TValue enforceNonZeroSignedArea(const Primitive& iPrimitive)
	{
		return iPrimitive.signedArea();
	}
};


}

}

// --- implementation ------------------------------------------------------------------------------

namespace lass
{
namespace prim
{
namespace impl
{

/** raises exception in case primitive is degenerate and we have restricting
 *  @relates lass::prim::Bounded
 */
/*
struct DegenerateRaiser
{
	template <class T>
	static void raise(const T&, const std::string& iMessage, const char* iLocus)
	{
		LASS_THROW_EX(DegenerationError, iMessage << '\n' << iLocus);
	}
};
*/

}

}

}



#endif

// EOF
