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
 *	Copyright (C) 2004-2011 the Initial Developer.
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



/** @class lass::util::CloneFactory
 *  @brief object creation by cloning prototypes
 *  @author Bram de Greve [Bramz]
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
 *  to the factory.  A @e prototype is an object instance that will be cloned by the @e cloner
 *  function.  Together, a pair of a @e prototype and a @e cloner function provides the
 *  functionality of one @e maker function in the ObjectFactory.
 *
 *  Again, all this @a cloners must have the same signature and should return a pointer to the
 *  abstract product.  Their first argument will be a reference to the prototype being cloned.
 *  The following arguments will be filled by the @a iP1, @a iP2, @a iP3, ... that are provided to
 *  the make() method of the clone factory.
 *
 *  @code
 *  class Base
 *  {
 *  public:
 *      Base(const std::string& iName): name_(iName) {}
 *      virtual std::string who() const { return std::string("Base ") + name_; }
 *      virtual Base* clone() const { return new Base(*this); }
 *  protected:
 *      std::string name_;
 *  };
 *
 *  class Foo: public Base
 *  {
 *  public:
 *      Foo(const std::string& iName): Base(iName) {}
 *      virtual std::string who() const { return std::string("Foo ") + name_; }
 *      virtual Base* clone() const { return new Foo(*this); }
 *  };
 *
 *  class Bar: public Base
 *  {
 *  public:
 *      Bar(const std::string& iName): Base(iName) {}
 *      virtual std::string who() const { return std::string("Bar ") + name_; }
 *      virtual Base* clone() const { return new Bar(*this); }
 *  };
 *
 *  Base* clone(const Base& iPrototype) { return iPrototype.clone(); }
 *
  * typedef util::CloneFactory<Base, std::string> TFactory;
 *  typedef std::auto_ptr<Base> TBasePtr;
 *
 *  TFactory factory(clone);
 *  factory.subscribe("Joe", TBasePtr(new Base("Joe")));
 *  factory.subscribe("Moe", TBasePtr(new Foo("Moe")));
 *  factory.subscribe("Doe", TBasePtr(new Bar("Doe")));
 *
 *  TBasePtr a(factory.make("Joe"));
 *  LASS_COUT << a->who() << "\n"; // Base Joe
 *  TBasePtr b(factory.make("Moe"));
 *  LASS_COUT << b->who() << "\n"; // Foo Moe
 *  TBasePtr c(factory.make("Doe"));
 *  LASS_COUT << c->who() << "\n"; // Bar Doe
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

#if LASS_HAVE_CPP_STD_11
	/** register a concrete product to the CloneFactory by a @a iIdentifier that
	 *  will identify the product, and an @a iPrototype that will be cloned.
	*/
	bool subscribe(
		typename CallTraits<IdentifierType>::TParam iIdentifier,
		std::unique_ptr<AbstractProduct> iPrototype)
	{
		return prototypes_.insert(typename TPrototypes::value_type(
			iIdentifier, std::move(iPrototype))).second;
	}
#endif

#if LASS_HAVE_STD_AUTO_PTR
	/** register a concrete product to the CloneFactory by a @a iIdentifier that
	 *  will identify the product, and an @a iPrototype that will be cloned.
	 */
	bool subscribe(
		typename CallTraits<IdentifierType>::TParam iIdentifier,
		std::auto_ptr<AbstractProduct> iPrototype)
	{
		return prototypes_.insert(typename TPrototypes::value_type(
			iIdentifier, iPrototype)).second;
	}
#endif

	/** unregister a concrete product by its @a iIdentifier
	 */
	bool unsubscribe(typename CallTraits<IdentifierType>::TParam iIdentifier)
	{
		return prototypes_.erase(iIdentifier) == 1;
	}

	/** Create a new concrete product by cloning the prototype
	 *  only to be used if maker don't want any parameters.
	 */
	AbstractProduct* make(typename CallTraits<IdentifierType>::TParam iIdentifier) const
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
						  $(P$x& iP$x)$) const
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
