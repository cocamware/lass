/** 
*   @author Bram de Greve (bramz@users.sourceforge.net)
*   @author Tom De Muer (tomdemuer@users.sourceforge.net)
*
*	Distributed under the terms of the GPL (GNU Public License)
*
* 	The LASS License:
*   
*	Copyright 2004 Bram de Greve and Tom De Muer
*
*   LASS is free software; you can redistribute it and/or modify
*   it under the terms of the GNU General Public License as published by
*   the Free Software Foundation; either version 2 of the License, or
*   (at your option) any later version.
*
*   This program is distributed in the hope that it will be useful,
*   but WITHOUT ANY WARRANTY; without even the implied warranty of
*   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
*   GNU General Public License for more details.
*
*   You should have received a copy of the GNU General Public License
*   along with this program; if not, write to the Free Software
*   Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
*


 *  @date 2004
 */

#ifndef LASS_GUARDIAN_OF_INCLUSION_STDE_STRONG_CONTAINER_H
#define LASS_GUARDIAN_OF_INCLUSION_STDE_STRONG_CONTAINER_H

#include "stde_common.h"

namespace lass
{
namespace stde
{

/** strong insert operation on associative containers like set and map.
 *  source: MARRIE L., <i>Exception Safety & Containers</i>, CUJ, March 2004, 34-37.
 */
template
<
	class Container,
	typename ForwardIterator
>
void strong_insert(Container& c, ForwardIterator first, ForwardIterator last)
{
	typedef typename Container::iterator iterator_type;
	std::deque<iterator_type> rollback_temp;

	try
	{
		while (first != last)
		{
			std::pair<iterator_type, bool> result = c.insert(*first);
			if (result.second)
			{
				try
				{
					rollback_temp.push_back(result.first);
				}
				catch (...)
				{
					c.erase(result.first);
					throw; // skip to outer catch block
				}
			}
			++first;
		}
	}
	catch (...)
	{
		typedef typename std::deque<iterator_type>::iterator deque_iterator_type;
		
		// remove rollback list elements
		//
		for (deque_iterator_type i = roolback_temp.begin(); i != rollback_temp.end(); ++i)
		{
			c.erase(*i);
		}
		throw;
	}
}




}

}

#endif

// EOF
