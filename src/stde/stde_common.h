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
 *  - @ref extended_io: implements stream operators for @c std::pair and STL containers.
 *  - @ref extended_string: implements additional @c std::basic_string functionaly.
 *  - @ref extended_iterator: implements additional iterator functionaly like @c next and @c prior.
 *  - @ref slist: implements a single linked list, much like the one of SGI (and in STLport).
 *  - @ref small_object_allocator: an std compliant allocator using the allocator from util::SmallObject
 *
 *  @note Currently, this libary is headers only.  This means no static library needs to be
 *        build.  Hence, the stde project is skipped by the build and the automatic link
 *        to the lib is commented. [BdG]
 */



#ifndef LASS_GUARDIAN_OF_INCLUSION_STDE_STDE_COMMON_H
#define LASS_GUARDIAN_OF_INCLUSION_STDE_STDE_COMMON_H




#include "../lass_common.h"

// Since no static lib is build, we shouldn't try to link to it.
// Uncomment this if this changes [BdG]
//
///** name of library
// */
//#define LASS_LIB_STDE LASS_LIB_PREFIX "stde" LASS_LIB_SUFFIX
//#if !defined(LASS_LIB_NO_AUTOMATIC_LINK)
//#   pragma comment(lib, LASS_LIB_STDE)
//#endif



#endif

