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
 *	Copyright (C) 2004-2009 the Initial Developer.
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


TUnitTest test_util_visitor()
{
	return TUnitTest(1, LASS_TEST_CASE(testUtilVisitor));
}


}

}

// EOF
