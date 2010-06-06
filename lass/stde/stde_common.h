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
 *	Copyright (C) 2004-2010 the Initial Developer.
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



/** @namespace lass::stde
 *  @brief lass extensions to the standard library
 *
 *  This namespace contains extensions to the standard library (@c std namespace), provided
 *  by the lass library.  In contrary to the rest of the @c lass library, these extensions
 *  will follow the codestyle of the standard library and @c boost.
 *
 *  Although these are extensions to the standard library, we've choosen to not
 *  incorporate these in the @c std namespace.  This to avoid conflicts with whatever
 *  is inside that @c std namespace (@c boost has its own namespace too).
 *
 *  And yes, @c stde stands for <i>standard extensions</i>
 *
 *  These are the extensions we offer:
 *
 *  - @ref stde_delete: functors to delete objects in pointer sequence.
 *  - @ref extended_algorithme: additional STL algorithms
 *  - @ref extended_io: implements stream operators for @c std::pair and STL containers.
 *  - @ref extended_string: implements additional @c std::basic_string functionaly.
 *  - @ref extended_iterator: implements additional iterator functionaly like @c next and @c prior.
 *  - @ref stde_select: functors to to select members of std::pair or stde::triple.
 *  - slist: implements a single linked list, much like the one of SGI (and in STLport).
 *  - small_object_allocator: an std compliant allocator using the allocator from util::SmallObject
 *  - triple: a std::pair with tree elements
 *
 *  @note Currently, this libary is headers only.  This means no static library needs to be
 *        build.  Hence, the stde project is skipped by the build and the automatic link
 *        to the lib is commented. [BdG]
 */

#ifndef LASS_GUARDIAN_OF_INCLUSION_STDE_STDE_COMMON_H
#define LASS_GUARDIAN_OF_INCLUSION_STDE_STDE_COMMON_H

#include "../lass_common.h"

#endif

