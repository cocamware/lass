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



/** @struct lass::prim::impl::Plane3DImpl
 *  @brief binder of equation policy to lass::prim::Plane3D implementation
 *  @author Bram de Greve [BdG]
 */

#ifndef LASS_GUARDIAN_OF_INCLUSION_PRIM_IMPL_PLANE_3D_IMPL_H
#define LASS_GUARDIAN_OF_INCLUSION_PRIM_IMPL_PLANE_3D_IMPL_H

#include "../prim_common.h"
#include "plane_3d_impl_detail.h"
#include "plane_3d_cartesian.h"
#include "plane_3d_parametric.h"
#include "plane_3d_combined.h"

namespace lass
{
namespace prim
{
namespace impl
{

// --- partial specialization ----------------------------------------------------------------------

#if !defined(LASS_NO_TEMPLATE_PARTIAL_SPECIALIZATION)

/** @internal
 *  Selects the cartesian equation as the default implemantation of a Plane3D class.
 */
template <typename T, class EquationPolicy, class NormalizingPolicy>
struct Plane3DImpl
{
    typedef Plane3DCartesian<T, NormalizingPolicy> Type;
};

/** @internal
 *  specialization for the parametric equation as the default implemantation of a Plane3D class.
 */
template <typename T, class NormalizingPolicy>
struct Plane3DImpl<T, Parametric, NormalizingPolicy>
{
    typedef Plane3DCartesian<T, NormalizingPolicy> Type;
};

/** @internal
 *  specialization for the combined equation as the default implemantation of a Plane3D class.
 */
template <typename T, class NormalizingPolicy>
struct Plane3DImpl<T, Combined, NormalizingPolicy>
{
    typedef Plane3DCombined<T, NormalizingPolicy> Type;
};



// --- no partial specialization -------------------------------------------------------------------

#else

/** @internal
 *  default is cartesian equation ...
 */
template <class EquationPolicy>
struct Plane3DImplBinder
{
    template <typename T, class NormalizingPolicy>
    struct Bind
    {
        typedef Plane3DCartesian<T, NormalizingPolicy> Type;
    };
};

/** @internal
 *  ...  Furthermore we have the parametric implementation ... 
 */
template <>
struct Plane3DImplBinder<Parametric>
{
    template <typename T, class NormalizingPolicy>
    struct Bind
    {
        typedef Plane3DParametric<T, NormalizingPolicy> Type;
    };
};

/** @internal
 *  ... and the combined implementation.
 */
template <>
struct Plane3DImplBinder<Combined>
{
    template <typename T, class NormalizingPolicy>
    struct Bind
    {
        typedef Plane3DCombined<T, NormalizingPolicy> Type;
    };
};

/** @internal
 *  Selects the implemantation of a Plane3D class.
 */
template <typename T, class EquationPolicy, class NormalizingPolicy>
struct Plane3DImpl
{
    typedef typename Plane3DImplBinder<EquationPolicy>::Bind<T, NormalizingPolicy>::Type Type;
};

#endif


}

}

}

#endif

// EOF
