/**	@file
 *	@author Bram de Greve (bramz@users.sourceforge.net)
 *	@author Tom De Muer (tomdemuer@users.sourceforge.net)
 *
 *	Distributed under the terms of the GPL (GNU Public License)
 *
 * 	The LASS License:
 *
 *	Copyright 2004 Bram de Greve and Tom De Muer
 *
 *	LASS is free software; you can redistribute it and/or modify
 *	it under the terms of the GNU General Public License as published by
 *	the Free Software Foundation; either version 2 of the License, or
 *	(at your option) any later version.
 *
 *	This program is distributed in the hope that it will be useful,
 *	but WITHOUT ANY WARRANTY; without even the implied warranty of
 *	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *	GNU General Public License for more details.
 *
 *	You should have received a copy of the GNU General Public License
 *	along with this program; if not, write to the Free Software
 *	Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
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
 *
 *  This is the default policy
 *
 *  NoDegenerate will not prevent a primitive to become degenerate, but it will check some 
 *  conditions will check some conditions when you try to do "unsafe" operations and throws
 *  an exception on failure.  e.g. CheckDegenerate will not allow you to get the @c edge(0)
 *  or @c vector(0) of a polygon with only one vertex.
 *
 *  However, it will not check all posible conditions.  It will avoid expensive predicates.
 *  e.g. it will avoid to test if a polygon is simple, because that's too expensive.  If you
 *  want all conditions to be checked, you should use StrictNoDegenerate
 */
struct NoDegenerate
{
	template <class T>
	static bool wrong(const T& iObj)
	{
		return !iObj;
	}

    template <class Primitive>
    static void enforceEdge(const Primitive& iPrimitive, int iIndexOfTailVertex)
    {
        if (iPrimitive.at(iIndexOfTailVertex) == iPrimitive.at(iIndexOfTailVertex + 1))
        {
            LASS_THROW("Degenerate edge detected at tail vertex '" << iIndexOfTailVertex << "'.");
        }
    }

    template <class Primitive>
    static void enforceSimple(const Primitive& iPrimitive)
    {
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
	template <class T>
	static bool wrong(const T& iObj)
	{
		return !iObj;
	}

    template <class Primitive>
    static void enforceEdge(const Primitive& iPrimitive, int iIndexOfTailVertex)
    {
        if (iPrimitive.at(iIndexOfTailVertex) == iPrimitive.at(iIndexOfTailVertex + 1))
        {
            LASS_THROW("Degenerate edge detected at tail vertex '" << iIndexOfTailVertex << "'.");
        }
    }

    template <class Primitive>
    static void enforceSimple(const Primitive& iPrimitive)
    {
        if (!iPrimtive.isSimple())
        {
            LASS_THROW("polygon is not simple.");
        }
    }
};



/** @ingroup DegeneratePolicy
 *
 *  AllowDegenerate puts the responsibility on the user.  It will assume all conditions are fullfilled
 *  and continue like nothing is wrong.  This is the "best guess" thing.
 */
struct AllowDegenerate
{
	template <class T>
	static bool wrong(const T& iObj)
	{
		return false;
	}

    template <class Primitive>
    static void enforceEdge(const Primitive& iPrimitive, int iIndexOfTailVertex)
    {
    }

    template <class Primitive>
    static void enforceSimple(const Primitive& iPrimitive)
    {
    }
};


}

}

/** enforce a primitive condition @a degeneratePredicate__ following the @a DegeneratePolicy__
 *  @ingroup DegeneratePolicy
 *
 *  If @a DegeneratePolicy__ is restricting, then the condition @a iDegeneratePredicate__ is 
 *  checked and an exception is thrown if condition is not fullfilled.  In case of non restricting
 *  degenerate policy, no exception is ever thrown.
 *
 *  @param DegeneratePolicy__ type of used degenerate policy, is used as enforcer predicate.
 *  @param degeneratePredicate__ boolean expression to check the primitive.
 *  @throw std::exception if predicate fails.
 */
#define LASS_PRIM_ENFORCE_NO_DEGENERATE(DegeneratePolicy__, noDegeneratePredicate__)\
    (*lass::util::impl::makeEnforcer<DegeneratePolicy__, lass::prim::impl::DegenerateRaiser>(\
        (noDegeneratePredicate__),\
        "Degenerate primitive detected. Condition '" LASS_STRINGIFY(noDegeneratePredicate__)\
        "' is not fullfilled in '" LASS_HERE "'"))



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
struct DegenerateRaiser
{
    template <class T> 
	static void raise(const T&, const std::string& iMessage, const char* iLocus)
    {
        LASS_THROW(iMessage << '\n' << iLocus);
    }
};

}

}

}



#endif

// EOF
