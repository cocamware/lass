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

#ifndef LASS_GUARDIAN_OF_INCLUSION_UTIL_OBJECT_FACTORY_H
#define LASS_GUARDIAN_OF_INCLUSION_UTIL_OBJECT_FACTORY_H

#include "util_common.h"

namespace lass
{
namespace util
{

template
<
	typename ObjectType,
	typename KeyType,
	typename PointerType = ObjectType*,
	typename MakerType = PointerType(*)()
>
class ObjectFactory
{
public:

	typedef ObjectType TObject;
	typedef KeyType TKey;
	typedef PointerType TPointer;
	typedef MakerType TMaker;

	void subscribe(typename CallTraits<TKey>::TParam key, typename CallTraits<TMaker>::TParam maker)
	{
		makers_[key] = maker;
	}

	TPointer make(typename CallTraits<TKey>::TParam key) const
	{
		const typename TMakers::const_iterator i = makers_.find(key);
		if (i == makers_.end())
		{
			LASS_THROW_EX(KeyError, "no product maker registered by key '" << key << "'");
		}
		return (i->second)();
	}
	$[
	template <$(typename P$x)$>
	TPointer make(typename CallTraits<TKey>::TParam key, $(const P$x p$x)$) const
	{
		const typename TMakers::const_iterator i = makers_.find(key);
		if (i == makers_.end())
		{
			LASS_THROW_EX(KeyError, "no product maker registered by key '" << key << "'");
		}
		return (i->second)($(p$x)$);
	}
	]$

private:

	typedef std::map<TKey, TMaker> TMakers;

	TMakers makers_;	
};

}
}

#endif

// EOF

