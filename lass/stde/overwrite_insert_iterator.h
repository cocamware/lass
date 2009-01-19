/**	@file
 *	@author Bram de Greve (bramz@users.sourceforge.net)
 *	@author Tom De Muer (tomdemuer@users.sourceforge.net)
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
 *	Copyright (C) 2004-2007 the Initial Developer.
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

#ifndef LASS_GUARDIAN_OF_INCLUSION_STDE_OVERWRITE_INSERT_ITERATOR_H
#define LASS_GUARDIAN_OF_INCLUSION_STDE_OVERWRITE_INSERT_ITERATOR_H

#include "stde_common.h"

namespace lass
{
namespace stde
{

template <typename Container>
class overwrite_insert_iterator: public std::iterator<std::output_iterator_tag, void, void, void, void>
{
public:

	typedef Container container_type;

	explicit overwrite_insert_iterator(container_type& container): 
		container_(&container),
		end_(container.begin())
	{
	}

	overwrite_insert_iterator& operator=(const overwrite_insert_iterator& other)
	{
		container_ = other.container_;
		end_ = other.end_;
		return *this;
	}

	overwrite_insert_iterator& operator=(typename Container::const_reference x)
	{
		if (end_ != container_->end())
		{
			*end_++ = x;
		}
		else
		{
			container_->push_back(x);
			end_ = container_->end();
		}
		return *this;
	}

	overwrite_insert_iterator& operator*() { return *this; }
	overwrite_insert_iterator& operator++() { return *this; }
	overwrite_insert_iterator& operator++(int) { return *this; }

	typename Container::iterator begin() { return container_->begin(); }
	typename Container::iterator end() { return end_; }
	typename Container::const_iterator begin() const { return container_->begin(); }
	typename Container::const_iterator end() const { return end_; }

private:

	Container* container_;
	typename Container::iterator end_;
};

template <typename Container>
overwrite_insert_iterator<Container> overwrite_inserter(Container& container) 
{
	return overwrite_insert_iterator<Container>(container);
}

}
}

#endif

// EOF
