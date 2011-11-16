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
