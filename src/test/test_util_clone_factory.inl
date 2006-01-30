/** @file
 *  @internal
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



#ifndef LASS_GUARDIAN_OF_INCLUSION_TEST_TEST_UTIL_CLONE_FACTORY_INL
#define LASS_GUARDIAN_OF_INCLUSION_TEST_TEST_UTIL_CLONE_FACTORY_INL

#include "test_common.h"

#include "../util/clone_factory.h"

namespace lass
{
namespace test
{
namespace clone_factory
{
	class Base
	{
	public:
		Base(const std::string& iName): name_(iName) {}
		virtual ~Base() {}
		virtual std::string who() const { return std::string("Base ") + name_; }
		virtual Base* clone() const { return new Base(*this); }
	protected:
		std::string name_;
	};

	class Foo: public Base
	{
	public:
		Foo(const std::string& iName): Base(iName) {}
		virtual std::string who() const { return std::string("Foo ") + name_; }
		virtual Base* clone() const { return new Foo(*this); }
	};

	class Bar: public Base
	{
	public:
		Bar(const std::string& iName): Base(iName) {}
		virtual std::string who() const { return std::string("Bar ") + name_; }
		virtual Base* clone() const { return new Bar(*this); }
	};

	Base* clone(const Base& iPrototype) { return iPrototype.clone(); }
}

void testUtilCloneFactory()
{
	typedef util::CloneFactory<clone_factory::Base, std::string> TFactory;
	typedef std::auto_ptr<clone_factory::Base> TBasePtr;

	TFactory factory(clone_factory::clone);
	factory.subscribe("Joe", TBasePtr(new clone_factory::Base("Joe")));
	factory.subscribe("Moe", TBasePtr(new clone_factory::Foo("Moe")));
	factory.subscribe("Doe", TBasePtr(new clone_factory::Bar("Doe")));

	TBasePtr a(factory.make("Joe"));
	LASS_TEST_CHECK_EQUAL(a->who(), "Base Joe");
	TBasePtr b(factory.make("Moe"));
	LASS_TEST_CHECK_EQUAL(b->who(), "Foo Moe");
	TBasePtr c(factory.make("Doe"));
	LASS_TEST_CHECK_EQUAL(c->who(), "Bar Doe");
}

}

}

#endif

// EOF
