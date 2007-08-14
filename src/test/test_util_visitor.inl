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
	class DocElement: public util::VisitableBase<util::IgnoreUnknownVisit>
	{
		LASS_UTIL_VISITOR_DO_ACCEPT
	};

	class Paragraph: public DocElement
	{
		LASS_UTIL_VISITOR_DO_ACCEPT
	};

	class Invisible: public DocElement
	{
		LASS_UTIL_VISITOR_DO_ACCEPT
	};

	class List: public DocElement
	{
	public:
		typedef util::SharedPtr<DocElement> TDocElementPtr;
		typedef std::list<TDocElementPtr> TDocElements;

		void add(std::auto_ptr<DocElement> iChild) { children_.push_back(iChild); }

	private:

		void doAccept(util::VisitorBase& ioVisitor)
		{
			preAccept(ioVisitor, *this);
			for (TDocElements::iterator i = children_.begin(); i != children_.end(); ++i)
			{
				(*i)->accept(ioVisitor);
			}
			postAccept(ioVisitor, *this);
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

		TestVisitor(): docElements_(0), paragraphs_(0), lists_(0) {}

		int docElements_;
		int paragraphs_;
		int lists_;

	private:
		void doPreVisit(DocElement&) { ++docElements_; }
		void doPreVisit(Paragraph&) { ++paragraphs_; }
		void doPreVisit(List&) { ++lists_; }
		void doPostVisit(List&) { LASS_COUT << "visit on exit\n"; }
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

	LASS_TEST_CHECK_EQUAL(visitor.docElements_, 1);
	LASS_TEST_CHECK_EQUAL(visitor.paragraphs_, 3);
	LASS_TEST_CHECK_EQUAL(visitor.lists_, 2);
}



}

}

#endif

// EOF
