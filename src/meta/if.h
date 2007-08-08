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

#ifndef LASS_GUADIAN_OF_INCLUSION_META_IF_H
#define LASS_GUADIAN_OF_INCLUSION_META_IF_H

#include "meta_common.h"
#include "bool.h"

namespace lass
{
namespace meta
{
namespace impl
{

template <typename Bool, typename TrueType, typename FalseType> struct IfImpl;

}

/**	Evaluates meta expression TrueType or FalseType based on a meta condition Condition.
 *  
 *	Depending on whether Condition is meta::True or meta::False, meta::If derives from
 *	TrueType or FalseType, thereby selecting the meta code path.
 *  
 *	meta::If can roughly be expressed as meta::Select if only the resulting Type is needed.
 *
 *	@code
 *	If<Cond, A, B>::Type <-> Select<Cond, A::Type, B::Type>::Type
 *	@endcode
 *
 *	Apart from being the shorter expression, using meta::If has the following benefits:
 *	@arg meta::It is faster. Using meta::Select requires the compiler to both evaluate A::Type and B::Type,
 *		regardless fo the outcome.  meta::If postpones the evaluation by deriving from the result.
 *	@arg meta::Select only has the Type field as result.  If other fields are needed, you should
 *		use meta::If as it derives directly from A or B.  Any field that is available in A or B
 *		is thus also available in the result.
 *
 *	meta::If however cannot be used with fundamental types as you need to be able to derive from the result.
 *	This is not really a short coming, as meta::If is designed to select meta code paths, and not to select
 *	a type.  Use meta::Select instead.
 */
template <typename Condition, typename TrueType, typename FalseType>
struct If: public If<typename Condition::Type, TrueType, FalseType> 
{
};

template <typename TrueType, typename FalseType> 
struct If<True, TrueType, FalseType>: public TrueType 
{
};

template <typename TrueType, typename FalseType> 
struct If<False, TrueType, FalseType>: public FalseType 
{
};

}
}

#endif

// EOF
