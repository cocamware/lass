/**	@file
 *	@author Bram de Greve (bramz@users.sourceforge.net)
 *	@author Tom De Muer (tomdemuer@users.sourceforge.net)
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
 *	Copyright (C) 2004-2007 the Initial Developer.
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



/** @struct lass::prim::impl::Line2DImpl
 *  @brief binder of equation policy to lass::prim::Line2D implementation
 *  @author Bram de Greve [BdG]
 */

#ifndef LASS_GUARDIAN_OF_INCLUSION_PRIM_IMPL_LINE_2D_IMPL_H
#define LASS_GUARDIAN_OF_INCLUSION_PRIM_IMPL_LINE_2D_IMPL_H

#include "../prim_common.h"
#include "../../num/floating_point_comparison.h"
#include "line_2d_cartesian.h"
#include "line_2d_parametric.h"
#include "line_2d_combined.h"

namespace lass
{
namespace prim
{
namespace impl
{

// --- partial specialization ----------------------------------------------------------------------

#if !defined(LASS_NO_TEMPLATE_PARTIAL_SPECIALIZATION)

/** @internal
 *  Selects the cartesian equation as the default implemantation of a Line2D class.
 */
template <typename T, class EquationPolicy, class NormalizingPolicy>
struct Line2DImpl
{
	typedef Line2DCartesian<T, NormalizingPolicy> Type;
};

/** @internal
 *  specialization for the parametric equation as the default implemantation of a Line2D class.
 */
template <typename T, class NormalizingPolicy>
struct Line2DImpl<T, Parametric, NormalizingPolicy>
{
	typedef Line2DParametric<T, NormalizingPolicy> Type;
};

/** @internal
 *  specialization for the combined equation as the default implemantation of a Line2D class.
 */
template <typename T, class NormalizingPolicy>
struct Line2DImpl<T, Combined, NormalizingPolicy>
{
	typedef Line2DCombined<T, NormalizingPolicy> Type;
};



// --- no partial specialization -------------------------------------------------------------------

#else

/** @internal
 *  default is cartesian equation ...
 */
template <class EquationPolicy>
struct Line2DImplBinder
{
	template <typename T, class NormalizingPolicy>
	struct Bind
	{
		typedef Line2DCartesian<T, NormalizingPolicy> Type;
	};
};

/** @internal
 *  ...  Furthermore we have the parametric implementation ...
 */
template <>
struct Line2DImplBinder<Parametric>
{
	template <typename T, class NormalizingPolicy>
	struct Bind
	{
		typedef Line2DParametric<T, NormalizingPolicy> Type;
	};
};

/** @internal
 *  ... and the combined implementation.
 */
template <>
struct Line2DImplBinder<Combined>
{
	template <typename T, class NormalizingPolicy>
	struct Bind
	{
		typedef Line2DCombined<T, NormalizingPolicy> Type;
	};
};

/** @internal
 *  Selects the implemantation of a Line2D class.
 */
template <typename T, class EquationPolicy, class NormalizingPolicy>
struct Line2DImpl
{
	typedef typename Line2DImplBinder<EquationPolicy>::Bind<T, NormalizingPolicy>::Type Type;
};

#endif

}

}

}

#endif

// EOF
