/**	@file
 *  @internal
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
 */



#ifndef LASS_GUARDIAN_OF_INCLUSION_TEST_TEST_UTIL_VISITOR_INL
#define LASS_GUARDIAN_OF_INCLUSION_TEST_TEST_UTIL_VISITOR_INL

#include "test_common.h"
#include "../util/visitor.h"



namespace lass
{
namespace test
{
namespace util_visitor
{
	class DocElement: public util::VisitableBase<>
	{
	public:
		LASS_UTIL_ACCEPT_VISITOR
	};

	class Paragraph: public DocElement
	{
	public:
		LASS_UTIL_ACCEPT_VISITOR
	};

	class Invisible: public DocElement
	{
	public:
		LASS_UTIL_ACCEPT_VISITOR
	};

	class List: public DocElement
	{
	public:
		typedef util::SharedPtr<DocElement> TDocElementPtr;
		typedef std::list<TDocElementPtr> TDocElements;

		void add(std::auto_ptr<DocElement> iChild) { children_.push_back(iChild); }

		void accept(util::VisitorBase& ioVisitor)
		{
			doAccept(*this, ioVisitor);
			for (TDocElements::iterator i = children_.begin(); i != children_.end(); ++i)
			{
				(*i)->accept(ioVisitor);
			}
		}

	private:
		TDocElements children_;
	};		

	struct TestVisitor:
		public util::VisitorBase, // required
		public util::Visitor<DocElement>,
		public util::Visitor<Paragraph>,
		public util::Visitor<List>
	{
	public:
		void visit(DocElement&) { ++docElements_; }
		void visit(Paragraph&) { ++paragraphs_; }
		void visit(List&) { ++lists_; }

		TestVisitor(): docElements_(0), paragraphs_(0), lists_(0) {}

		int docElements_;
		int paragraphs_;
		int lists_;
	};
}

void testUtilVisitor()
{
	using namespace util_visitor;

	typedef std::auto_ptr<DocElement> TElement;

	List root;
	root.add(TElement(new DocElement));
	root.add(TElement(new Paragraph));
	root.add(TElement(new Invisible));
	root.add(TElement(new Paragraph));

	std::auto_ptr<List> sub(new List);
	sub->add(TElement(new Paragraph));
	sub->add(TElement(new Invisible));
	root.add(TElement(sub.release()));

	TestVisitor visitor;
	root.accept(visitor);

	BOOST_CHECK_EQUAL(visitor.docElements_, 1);
	BOOST_CHECK_EQUAL(visitor.paragraphs_, 3);
	BOOST_CHECK_EQUAL(visitor.lists_, 2);
}



}

}

#endif

// EOF
