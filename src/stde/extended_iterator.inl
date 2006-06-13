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



#ifndef LASS_GUARDIAN_OF_INCLUSION_STDE_EXTENDED_ITERATOR_INL
#define LASS_GUARDIAN_OF_INCLUSION_STDE_EXTENDED_ITERATOR_INL

#include "stde_common.h"
#include "extended_iterator.h"

#if LASS_COMPILER_TYPE == LASS_COMPILER_TYPE_MSVC
#	pragma warning(push)
#	pragma warning(disable: 4244)
#endif

namespace lass
{
namespace stde
{

/** @ingroup extended_iterator
 */
template <typename Iterator>
Iterator next(Iterator iterator)
{
	++iterator;
	return iterator;
}

/** @ingroup extended_iterator
 */
template <typename Iterator>
Iterator prior(Iterator iterator)
{
	--iterator;
	return iterator;
}

/** @ingroup extended_iterator
 */
template <typename Iterator, typename Distance>
Iterator next(Iterator iterator, Distance distance)
{
	std::advance(iterator, distance);
	return iterator;
}

/** @ingroup extended_iterator
 */
template <typename Iterator, typename Distance>
Iterator prior(Iterator iterator, Distance distance)
{
	std::advance(iterator, -distance);
	return iterator;
}


}

}

#if LASS_COMPILER_TYPE == LASS_COMPILER_TYPE_MSVC
#	pragma warning(pop)
#endif

#endif

// EOF
