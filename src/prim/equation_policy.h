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



/** @defgroup EquationPolicy EquationPolicy
 *  @brief policies to specify how some primitives are modeled and implemented.
 *  @author Bram de Greve
 *  @date 2003
 *
 *  Some primitives, like 3D planes and 2D lines, you can model in one that more way.
 *  Typically, you can use the cartesian equation or a parametric equation.  You can
 *  use these policies to select one.  A third model combines both the cartesian and
 *  the parametric equation.
 *
 *  @note These polices are only place holders, selectors.  It means they are empty
 *        classes, and will select the real model when used.
 *
 *
 *  The content of the policies are rather implementation details, clients shouldn't
 *  worry about it.
 */

/** @class lass::prim::Cartesian
 *  @ingroup EquationPolicy
 *  @brief policy for an implementation based on the cartesian equation.
 *  @author Bram de Greve [BdG]
 */

/** @class lass::prim::Parametric
 *  @ingroup EquationPolicy
 *  @brief policy for an implementation based on the parametric equation.
 *  @author Bram de Greve [BdG]
 */

/** @class lass::prim::Combined
 *  @ingroup EquationPolicy
 *  @brief policy for an implementation based on both the cartesian and parametric equation.
 *  @author Bram de Greve [BdG]
 */

#ifndef LASS_GUARDIAN_OF_INCLUSION_PRIM_EQUATION_POLICY_H
#define LASS_GUARDIAN_OF_INCLUSION_PRIM_EQUATION_POLICY_H
#pragma once

#include "prim_common.h"

namespace lass
{
namespace prim
{

struct Cartesian
{
};

struct Parametric
{
};

struct Combined
{
};


}

}

#endif

// EOF
