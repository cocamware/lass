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

#include "test_common.h"

#include "../lass/util/clone_factory.h"

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
		std::string who() const override { return std::string("Foo ") + name_; }
		Base* clone() const override { return new Foo(*this); }
	};

	class Bar: public Base
	{
	public:
		Bar(const std::string& iName): Base(iName) {}
		std::string who() const override { return std::string("Bar ") + name_; }
		Base* clone() const override { return new Bar(*this); }
	};

	Base* clone(const Base& iPrototype) { return iPrototype.clone(); }
}

void testUtilCloneFactory()
{
	typedef util::CloneFactory<clone_factory::Base, std::string> TFactory;

#if LASS_HAVE_CPP_STD_11
	typedef std::unique_ptr<clone_factory::Base> TBasePtr;
#elif LASS_HAVE_STD_AUTO_PTR
	typedef std::auto_ptr<clone_factory::Base> TBasePtr;
#else
#	error "Must have either std::auto_ptr or std::unique_ptr"
#endif

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


TUnitTest test_util_clone_factory()
{
	return TUnitTest(1, LASS_TEST_CASE(testUtilCloneFactory));
}

}

}

// EOF
