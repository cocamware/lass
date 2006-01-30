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



/** @class lass::util::ObjectFactory
 *  @brief
 *  @author Bram de Greve [Bramz]
 *
 *  ObjectFactory is implemented after the example of Alexandrescu's loki::Factory [1].  An
 *  object factory creates objects of the same hierarchy, i.e. objects that have a common base
 *  class.  The base class is an @a abstract product and the derived classes are @a concrete
 *  products.  The factory let you decide what concrete product to create at @a runtime by
 *  using a specific @a identifier.  These identifiers can be integers or strings or any
 *  type you can use in a std::map.  Before you can create any products, you must @a subscribe
 *  maker functions to the factory.  All this makers have the same signature and should
 *  return a pointer to the abstract product.  It are these makers that create new instances
 *  of concrete products on the heap, and you should have one for each product.
 *
 *  @code
 *  class Base;
 *  class ProductA: public Base;
 *  class ProductB: public Base;
 *
 *  Base* makeA() { return new ProductA; }
 *  Base* makeB() { return new ProductB; }
 *
 *  ObjectFactory<Base, int, Base* (*) ()> factory;
 *  factory.subscribe(1, makeA);
 *  factory.subscribe(2, makeB);
 *
 *  Base* a = factory.make(1); // makes a object of type ProductA
 *  @endcode
 *
 *
 *
 *  @section Remarks
 *
 *  - the make function is overloaded to take 0 to $x (templated) parameters next to the
 *    identifier.  The identifier (that is always present and always comes first) is used to select
 *    the concrete product to be made.  In fact, it selects the product maker to be used to create
 *    a new object.  The extra templated parameters are then passed to that maker and the maker has
 *    to be able to except them.  However, that doesn't mean the maker also has to accept 0 to
 *    $x templated parameters.  No, it means that you only should use those instances of
 *    the make function that can be fitted on the maker.  i.e. if the maker only excepts two
 *    floats, you shouldn't be calling make with three strings, it doens't fit.  See it as following
 *    the make function is a broad interface that is fitted on a smaller interface, and you should
 *    only use that part that can be fitted.  What happens if you violate this?  You'll get
 *    compile time errors on the make, i guess :)
 *
 *  @b Reference:
 *  -# ALEXANDRESCU A. (2001), <i>Modern C++ Design: Generic Programming and Design Patterns
 *     applied</i>, C++ in depth series, Addison-Wesley.
 *  -# GAMMA E., HELM R., JOHNSON R. & VLISSIDES J. (1995), <i>Design Patters: Elements of
 *     Reusable Object-Oriented Software</i>, Addison-Wesley.
 *
 *  <b>original code by Andrei Alexandrescu:</b>\n
 *  The Loki Library, Copyright (c) 2001 by Andrei Alexandrescu\n
 *  This code (Loki) accompanies the book:\n
 *  Alexandrescu, Andrei. "Modern C++ Design: Generic Programming and Design Patterns Applied".
 *  Copyright (c) 2001. Addison-Wesley.\n
 *  Permission to use, copy, modify, distribute and sell this software (Loki) for any purpose is
 *  hereby granted without fee, provided that the above copyright notice appear in all copies and
 *  that both that copyright notice and this permission notice appear in supporting documentation.\n
 *  The author or Addison-Wesley Longman make no representations about the suitability of this
 *  software (Loki) for any purpose. It is provided "as is" without express or implied warranty.\n
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
	typename AbstractProduct,
	typename IdentifierType,
	typename ProductPointer = AbstractProduct*,
	typename ProductMaker = ProductPointer(*)()
>
class ObjectFactory
{
public:

	/** register a concrete product to the ObjectFactory by a iIdentifier that will identify
	 *  the product and a iMaker that can create new products.
	 */
	bool subscribe(typename CallTraits<IdentifierType>::TParam iIdentifier,
				   typename CallTraits<ProductMaker>::TParam iMaker)
	{
		return makers_.insert(std::make_pair(iIdentifier, iMaker)).second;
	}

	/** unregister a concrete product.
	 */
	bool unsubscribe(typename CallTraits<IdentifierType>::TParam iIdentifier)
	{
		return makers_.erase(iIdentifier) == 1;
	}

	/** create a new concrete product by calling its maker.
	 *  only to be used if maker don't want any parameters.
	 */
	ProductPointer make(typename CallTraits<IdentifierType>::TParam iIdentifier) const
	{
		typename TMakers::const_iterator mit = makers_.find(iIdentifier);
		if (mit == makers_.end())
		{
			LASS_THROW("Unknown Product identifier '" << iIdentifier
				<< "' passed to ObjectFactory.");
		}
		return (mit->second)();
	}
	$[
	/** create a new concrete product by calling its maker with $x parameter(s).
	 *  only to be used if it can be mapped on maker function.
	 */
	template <$(typename P$x)$>
	ProductPointer make(typename CallTraits<IdentifierType>::TParam iIdentifier,
						  $(P$x& iP$x)$) const
	{
		typename TMakers::const_iterator mit = makers_.find(iIdentifier);
		if (mit == makers_.end())
		{
			LASS_THROW("Unknown Product identifier '" << iIdentifier
				<< "' passed to ObjectFactory.");
		}
		return (mit->second)($(iP$x)$);
	}
	]$

private:

	typedef std::map<IdentifierType, ProductMaker> TMakers;
	TMakers makers_;
};



}

}

#endif

// EOF
