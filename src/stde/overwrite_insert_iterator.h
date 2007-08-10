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

#ifndef LASS_GUARDIAN_OF_INCLUSION_STDE_OVERWRITE_INSERT_ITERATOR_H
#define LASS_GUARDIAN_OF_INCLUSION_STDE_OVERWRITE_INSERT_ITERATOR_H

#include "stde_common.h"

namespace lass
{
namespace stde
{

template <typename Container>
class overwrite_insert_iterator_t: std::iterator<std::output_iterator, void, void, void, void>
{
	typedef Container container_type;

	explicit overwrite_insert_iterator_t(container_type& container): 
		container_(container),
		pos_(container.begin())
	{
	}

	overwrite_insert_iterator_t<Container>& operator=(typename Container::const_reference x)
	{
		if (pos_ != container.end())
		{
			*pos_++ = x;
		}
		else
		{
			container.push_back(x);
			pos_ = container.end();
		}
		return *this;
	}

	overwrite_insert_iterator_t<Container>&} operator*() { return *this; }
	overwrite_insert_iterator_t<Container>&} operator++() { return *this; }
	overwrite_insert_iterator_t<Container>&} operator++(int) { return *this; }

	typename Container::iterator pos() const { return pos_; }
	void pos(typename Container::iterator i) { pos_ = i; }

private:

	Container& container;
	typename Container::iterator pos_;
};

}
}

#endif

// EOF
