/**	@file
 *	@author Bram de Greve (bramz@users.sourceforge.net)
 *	@author Tom De Muer (tomdemuer@users.sourceforge.net)
 *
 *	Distributed under the terms of the GPL (GNU Public License)
 *
 * 	The LASS License:
 *
 *	Copyright 2004 Bram de Greve and Tom De Muer
 *
 *	LASS is free software; you can redistribute it and/or modify
 *	it under the terms of the GNU General Public License as published by
 *	the Free Software Foundation; either version 2 of the License, or
 *	(at your option) any later version.
 *
 *	This program is distributed in the hope that it will be useful,
 *	but WITHOUT ANY WARRANTY; without even the implied warranty of
 *	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *	GNU General Public License for more details.
 *
 *	You should have received a copy of the GNU General Public License
 *	along with this program; if not, write to the Free Software
 *	Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 *
 *	$g
 */



/** @class lass::util::CloneFactory
 *  @brief object creation by cloning prototypes
 *  @author Bram de Greve [BdG]
 *
 *  A clone factory is much like an object factory (lass::util::ObjectFactory), except that it will
 *  clone existings object rather than creating new ones from the ground up using maker functions.
 *
 *  A clone factory creates objects of the same hierarchy, i.e. objects that have a common base
 *  class.  The base class is an @e abstract product and the derived classes are @e concrete
 *  products.  The factory let you decide what concrete product to create at @e runtime by
 *  using a specific @e identifier.  These identifiers can be integers or strings or any
 *  type you can use in a std::map.  
 *
 *  Before you can create any products, you must @e subscribe @e prototypes and @e cloner methods
 *	to the factory.  A @e prototype is an object instance that will be cloned by the @e cloner 
 *  function.  Together, a pair of a @e prototype and a @e cloner function provides the 
 *  functionality of one @e maker function in the ObjectFactory.  
 *
 *	Again, all this @a cloners must have the same signature and should return a pointer to the 
 *  abstract product.  Their first argument will be a reference to the prototype being cloned.
 *  The following arguments will be filled by the @a iP1, @a iP2, @a iP3, ... that are provided to
 *  the make() method of the clone factory.
 *
 *  @code
 *	class Base
 *	{
 *	public:
 *		Base(const std::string& iName): name_(iName) {}
 *		virtual std::string who() const { return std::string("Base ") + name_; }
 *		virtual Base* clone() const { return new Base(*this); }
 *	protected:
 *		std::string name_;
 *	};
 *
 *	class Foo: public Base
 *	{
 *	public:
 *		Foo(const std::string& iName): Base(iName) {}
 *		virtual std::string who() const { return std::string("Foo ") + name_; }
 *		virtual Base* clone() const { return new Foo(*this); }
 *	};
 *
 *	class Bar: public Base
 *	{
 *	public:
 *		Bar(const std::string& iName): Base(iName) {}
 *		virtual std::string who() const { return std::string("Bar ") + name_; }
 *		virtual Base* clone() const { return new Bar(*this); }
 *	};
 *
 *	Base* clone(const Base& iPrototype) { return iPrototype.clone(); }
 *
  * typedef util::CloneFactory<clone_factory::Base, std::string> TFactory;
 *	typedef std::auto_ptr<clone_factory::Base> TBasePtr;
 *
 *	TFactory factory(clone_factory::clone);
 *	factory.subscribe("Joe", TBasePtr(new clone_factory::Base("Joe")));
 *	factory.subscribe("Moe", TBasePtr(new clone_factory::Foo("Moe")));
 *	factory.subscribe("Doe", TBasePtr(new clone_factory::Bar("Doe")));
 *
 *	TBasePtr a(factory.make("Joe"));
 *	LASS_COUT << a->who() << "\n"; // Base Joe
 *	TBasePtr b(factory.make("Moe"));
 *	LASS_COUT << b->who() << "\n"; // Foo Moe
 *	TBasePtr c(factory.make("Doe"));
 *	LASS_COUT << c->who() << "\n"; // Bar Doe
 *  @endcode
 */

#ifndef LASS_GUARDIAN_OF_INCLUSION_UTIL_CLONE_FACTORY_H
#define LASS_GUARDIAN_OF_INCLUSION_UTIL_CLONE_FACTORY_H

#include "util_common.h"
#include "shared_ptr.h"

namespace lass
{
namespace util
{

template 
<
	class AbstractProduct,
	class IdentifierType,
	class ProductCloner = AbstractProduct*(*)(const AbstractProduct&)
>
class CloneFactory
{
public:

	typedef AbstractProduct TAbstractProduct;
	typedef IdentifierType TIdentifier;
	typedef ProductCloner TProductCloner;

	/** create a clone factory by specifying its cloner function @a iCloner.
	 */
	CloneFactory(typename CallTraits<ProductCloner>::TParam iCloner):
		cloner_(iCloner)
	{
	}

	/** register a concrete product to the CloneFactory by a @a iIdentifier that will identify
	 *  the product, and an @a iPrototype that will be cloned.
	 */
	bool subscribe(typename CallTraits<IdentifierType>::TParam iIdentifier,
				   std::auto_ptr<AbstractProduct> iPrototype)
	{
		return prototypes_.insert(TPrototypes::value_type(iIdentifier, iPrototype)).second;
	}

	/** unregister a concrete product by its @a iIdentifier
	 */
	bool unsubscribe(typename CallTraits<IdentifierType>::TParam iIdentifier)
	{
		return prototypes_.erase(iIdentifier) == 1;
	}

	/** Create a new concrete product by cloning the prototype
	 *  only to be used if maker don't want any parameters.
	 */
	AbstractProduct* make(typename CallTraits<IdentifierType>::TParam iIdentifier)
	{
		typename TPrototypes::const_iterator i = prototypes_.find(iIdentifier);
		if (i == prototypes_.end())
		{
			LASS_THROW("Unknown Product identifier '" << iIdentifier 
				<< "' passed to CloneFactory.");
		}
		return cloner_(*i->second);
	}
	$[
	/** create a new concrete product by cloning the prototype with $x additional parameter(s).
	 *  only to be used if it can be mapped on @a iCloner function.
	 */
	template <$(typename P$x)$>
	AbstractProduct* make(typename CallTraits<IdentifierType>::TParam iIdentifier, 
						  $(P$x& iP$x)$)
	{
		typename TPrototypes::const_iterator i = prototypes_.find(iIdentifier);
		if (i == prototypes_.end())
		{
			LASS_THROW("Unknown Product identifier '" << iIdentifier 
				<< "' passed to CloneFactory.");
		}
		return cloner_(*i->second, $(iP$x)$);
	}
	]$

private:

	typedef SharedPtr<AbstractProduct> TPrototypePtr;
	typedef std::map<IdentifierType, TPrototypePtr> TPrototypes;

	TPrototypes prototypes_;
	TProductCloner cloner_;
};



}

}

#endif

// EOF
