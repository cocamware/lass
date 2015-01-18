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



/** @class lass::meta::Bool
 *  @brief meta boolean type
 *  @author Bram de Greve [BdG]
 *
 *  The meta boolean type consists of two types True and False.
 *  Bool<flag> converts an C @c bool @a flag to a @e meta bools True or False
 *
 *  @code
 *  Bool<true>::Type; // resolves to True
 *  @endcode
 */

#ifndef LASS_GUARDIAN_OF_INCLUSION_META_BOOL_H
#define LASS_GUARDIAN_OF_INCLUSION_META_BOOL_H

#include "meta_common.h"

// experimental,
// Clang's -Wunused-private-field will warn that dummy_ and biggerThanTrue_ are not used.
// Simply using LASS_UNUSED won't work since GCC will then again warn that __attribute__((unused)) is ignored.
// Sigh, so, only do LASS_UNUSED if we're Clang.
// If this properly works, and we have more insight in this situation, we might move this to some more permanent location ...
#if LASS_COMPILER_TYPE == LASS_COMPILER_TYPE_CLANG
#   define LASS_EXPERIMENTAL_UNUSED_PRIVATE(x) LASS_UNUSED(x)
#else
#   define LASS_EXPERIMENTAL_UNUSED_PRIVATE(x) x
#endif

namespace lass
{
namespace meta
{

/** meta true
 *  @relates Bool
 */
class True
{
public:
	typedef True Type;
	enum { value = true };
private:
	char LASS_EXPERIMENTAL_UNUSED_PRIVATE(dummy_);
};

/** meta false
 *  @relates Bool
 */
struct False
{
public:
	typedef False Type;
	enum { value = false };
private:
	True LASS_EXPERIMENTAL_UNUSED_PRIVATE(biggerThanTrue_[2]);
};



template <bool flag> 
struct Bool: 
	public True 
{
};

template <> 
struct Bool<false>: 
	public False 
{
};

// --- logic operators -----------------------------------------------------------------------------

/** meta NOT
 *  @relates Bool
 */
template <typename Operand> struct Not: 
	public Not<typename Operand::Type> 
{
};
template <> struct Not<False>: public True {};
template <> struct Not<True>: public False {};



/** meta AND
 *  @relates Bool
 */
template <typename Operand1, typename Operand2> struct And:
	public And<typename Operand1::Type, typename Operand2::Type> 
{
};
template <> struct And<False, False>: public False {};
template <> struct And<False, True>: public False {};
template <> struct And<True, False>: public False {};
template <> struct And<True, True>: public True {};



/** meta OR
 *  @relates Bool
 */
template <typename Operand1, typename Operand2> struct Or:
	public Or<typename Operand1::Type, typename Operand2::Type> 
{
};
template <> struct Or<False, False>: public False {};
template <> struct Or<False, True>: public True {};
template <> struct Or<True, False>: public True {};
template <> struct Or<True, True>: public True {};



/** meta XOR
 *  @relates Bool
 */
template <typename Operand1, typename Operand2> struct Xor:
	public Xor<typename Operand1::Type, typename Operand2::Type> 
{
};
template <> struct Xor<False, False>: public False {};
template <> struct Xor<False, True>: public True {};
template <> struct Xor<True, False>: public True {};
template <> struct Xor<True, True>: public False {};


}

}

#endif

// EOF
