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

namespace lass
{
namespace meta
{

/** meta true
 *  @relates Bool
 */
struct True
{
	char dummy;
	typedef char Type;
	enum { value = true };
};

/** meta false
 *  @relates Bool
 */
struct False
{
	char lotsOfDummies[256];
	typedef struct { char dummy[2]; } Type;
	enum { value = false };
};

template <bool flag>
struct Bool
{
	typedef True Type;
};

template <>
struct Bool<false>
{
	typedef False Type;
};

}

}

#endif

// EOF
