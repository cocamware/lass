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
 */

/** @defgroup VisitorPattern Visitor
 *  @brief support for visitor pattern
 *  @author Bram de Greve
 *  @author (original code: Andrei Alexandrescu, 2001) 
 *  @date 2004
 *
 *  This module is an implementation of the acyclic visitor pattern as described by Alexandrescu [1].
 *  It uses RTTI to visit all elements (what means some slow down on performance), but has the 
 *  benefit of a very low dependency between the visitor and the visited hierarchy.
 *
 *  @paroriginal code by Andrei Alexandrescu:
 *		<i>The Loki Library, Copyright (c) 2001 by Andrei Alexandrescu\n
 *		This code (Loki) accompanies the book:\n
 *		Alexandrescu, Andrei. "Modern C++ Design: Generic Programming and Design Patterns Applied". 
 *		Copyright (c) 2001. Addison-Wesley.\n
 *		Permission to use, copy, modify, distribute and sell this software (Loki) for any purpose is 
 *		hereby granted without fee, provided that the above copyright notice appear in all copies and 
 *		that both that copyright notice and this permission notice appear in supporting documentation.\n
 *		The author or Addison-Wesley Longman make no representations about the suitability of this 
 *		software (Loki) for any purpose. It is provided "as is" without express or implied warranty.\n</i>
 *
 *	@section documentation
 *
 *  To use the visitor pattern, you go as following.  First, you must distinguish between the
 *  @e visited hierarchy and the @e visitor hierarchy.  Let's start what must be done with the
 *  @e visited hierarchy.
 *
 *  @subsection visitables
 *
 *  Suppose, we have some classes @c Foo and @c Bar to be visited, and both are are derived of 
 *  @c Spam.  You must derived @c Spam of VisitableBase, and invoke the macro
 *  LASS_UTIL_ACCEPT_VISITOR in the public part of each class of the hierarchy.
 * 
 *  @code
 *  class Spam: public lass::util::VisitableBase<>
 *  {
 *  public:
 *		LASS_UTIL_ACCEPT_VISITOR
 *  };
 *
 *  class Foo: public Spam
 *  {
 *	public:
 *		LASS_UTIL_ACCEPT_VISITOR
 *  };
 *
 *  class Bar: public Spam
 *  {
 *	public:
 *		LASS_UTIL_ACCEPT_VISITOR
 *  };
 *  @endcode
 *
 *  that's easy isn't it?  LASS_UTIL_ACCEPT_VISITOR implements in each derived class the accept function
 *  that is needed to switch the visitor over the different visitables.  All it does is passing the
 *  call to the VisitableBase, but it's crucial for the pattern.  Instead of using 
 *  LASS_UTIL_ACCEPT_VISITOR, you can also write your own acceptor if you want something fancier,
 *  e.g. to iterate over a list of child objects ...
 *
 *  @code
 *  class List: public Spam
 *  {
 *	public:
 *		void addChild(Spam* iChild): children_.push_back(iChild) {}
 *      virtual TVisitReturn accept(lass::util::VisitorBase& iVisitor)
 *		{
 *			for (TChilderen::iterator i = children_.begin(); i != children_.end(); ++i)
 *			{
 *				(*i)->accept(iVisitor);
 *			}
 *			return doAccept(doAccept(*this, iVisitor);
 *		}
 *	private:
 *		typedef std::list<Spam*> TChildren;
 *		TChildren children_;
 *	};
 *  @endcode
 *
 *  if @c doAccept doesn't know how to accept the visitor (or the visitor doesn't know how to visit
 *  the visitable), the @c onUnknownVisitor function of the @e CatchAll policy is called.  CatchAll
 *  of VisitableBase determines what to do on undetermined visits.  By default the policy
 *  VisitNonStrict is used, what means the call is silently ignored and you can move on with the visit.
 *  Another one is VisitStrict which will throw an exception on every unknown visit.  What you want is
 *  your choice, and you can always write your own policy.
 *
 *  What about TVisitReturn?  We didn't talk about that one yet, did we?  Well, by default this will
 *  be @c void, meaning the acceptors and visits don't have a return value.  However, if it suits your
 *  needs, you could make it return something interesting.  That's when you can use TVisitReturn.
 *
 *  @subsection visitors
 *
 *  To declare a visitor, all you have to do is to derive of VisitorBase once, and of Visitor for
 *  every class of the visitable hierarchy you want to visit.  Suppose you want to visit objects of
 *  type Foo and Bar only, the you would write:
 *
 *  @code
 *  class MyVisitor:
 *		public lass::util::VisitorBase<>,
 *		public lass::util::Visitor<Foo>,
 *		public lass::util::Visitor<Bar>
 *	{
 *		virtual void visit(Foo& iFoo) { ... }
 *		virtual void visit(Bar& iBar) { ... }
 *  };
 *  @endcode
 *
 *  objects of type Spam (non derived) or any other type of the Spam hiearchy that is not Foo or Bar
 *  will be redirected to the @c onUnknownVisitor function of the @e CatchAll policy of the visitable
 *  hierarchy.  That way, you only have to pull in and write visits for the types your interested in.
 *
 *  Notice that while we didn't tell how to visit a List object, the visitor will still iterate over
 *  children of the list, since that's done in the @c accept function and has nothing to do with the
 *  visitor being able to visit a List or not.
 *
 *  If you don't declare visits for all types, it will go for a best match if possible.  e.g. if you're
 *  only interested in counting the number of objects in the visitable hierarchy regardless of their
 *  subtype, you can get away with writing a visitor that only visits the Spam type.  All subtypes
 *  will be handled by this best match visit.
 *
 *  @code
 *  class Counter:
 *		public lass::util::VisitorBase<>,
 *		public lass::util::Visitor<Spam>
 *	{
 *  public:
 *		virtual void visit(Spam& iSpam) { ++count_; }
 *      int count() const { return count_; }
 *  private:
 *		int count_;
 *  };
 *  @endcode.
 *
 *  @section calling
 *
 *  So, how do you visit a hierarchy?  Well, we didn't really tell yet, but it's very simple:
 *  
 *  @code
 *  visitable.accept(visitor);
 *  @endcode
 *
 *  This single line of code will iterate over the visitable object tree and will let the visitor
 *  try to visit each node.  e.g. for the counter, it would be like the following ...
 *
 *  @code
 *  List list;
 *	Foo foo;
 *  list.addChild(&foo);
 *  Bar bar;
 *  list.addChild(&bar);
 *
 *  Counter counter;
 *  list.accept(counter);
 *  std::cout << counter.count(); // output: 3
 *  @endcode
 *
 *  Of course, you could make things a little fancier by writing a nice function call operator in the
 *  Counter class:
 *
 *  @code
 *  class Counter:
 *		public lass::util::VisitorBase<>,
 *		public lass::util::Visitor<Spam>
 *	{
 *  public:
 *		virtual void visit(Spam& iSpam) { ++count_; }
 *		int operator()(Spam& iSpam)
 *		{
 *			count_ = 0;
 *			iSpam.accept(*this);
 *			return count_;
 *		}
 *  private:
 *		int count_;
 *  };
 *
 *	Counter counter;
 *  std::cout << counter(list);
 *  @endcode.
 */

#ifndef LASS_GUARDIAN_OF_INCLUSION_UTIL_VISITOR_H
#define LASS_GUARDIAN_OF_INCLUSION_UTIL_VISITOR_H

#include "util_common.h"

namespace lass
{
namespace util
{

/** @ingroup VisitorPattern
 *  @brief base class of all visitors
 */
class VisitorBase
{
public:
	virtual ~VisitorBase() {}
};



/** @ingroup VisitorPattern
 *  @brief a mix-in to provides a visitor capabilities to visit a class
 */
template 
<
	class VisitableType,
	typename VisitReturnType = void
>
class Visitor
{
public:

	typedef VisitableType TVisitable;
	typedef VisitReturnType TVisitReturn;

	virtual TVisitReturn visit(TVisitable& iVisited) = 0;
};



/** @ingroup VisitorPattern
 *  @brief catch-all policy doing nothing, default.
 *
 *  This catch-all policy will do nothing if a visitor class is not recognized, and return a
 *  value by the default constructor of @a VisitReturnType.
 *
 *  This is the default.
 */
template
<
	class VisitableType,
	typename VisitReturnType
>	
struct VisitNonStrict
{
	static VisitReturnType onUnknownVisitor(VisitableType& iVisited, VisitorBase& iVisitor)
	{
		return VisitReturnType();
	}
};



/** @ingroup VisitorPattern
 *  @brief catch-all policy raising an exception
 *
 *  This catch-all policy will raise an exception if a visitor class is not recognized.
 */
template
<
	class VisitableType,
	typename VisitReturnType
>	
struct VisitStrict
{
	static VisitReturnType onUnknownVisitor(VisitableType& iVisited, VisitorBase& iVisitor)
	{
		LASS_THROW("Unacceptable visit: '" << typeid(iVisited).name() << "' can't accept '"
			<< typeid(iVisitor).name() << "' as visitor.");
		return VisitReturnType();
	}
};



/** @ingroup VisitorPattern
 *  @brief base class of all visitable classes
 *
 *  derive of this class to make a hierarchy of visitable classes
 */
template
<
	typename VisitReturnType = void,
	template <class, typename> class CatchAll = VisitNonStrict
>
class VisitableBase
{
public:

	typedef VisitReturnType TVisitReturn;

	virtual ~VisitableBase() {}
	virtual TVisitReturn accept(VisitorBase& iVisitor) = 0;

protected:

	template <class T>
	static TVisitReturn doAccept(T& iVisited, VisitorBase& iVisitor)
	{
		if (Visitor<T>* p = dynamic_cast<Visitor<T>*>(&iVisitor))
		{
			return p->visit(iVisited);
		}
		return CatchAll<T, VisitReturnType>::onUnknownVisitor(iVisited, iVisitor);
	}
};

}

}

/** @ingroup VisitorPattern
 *  @brief implements an acceptor in a visitable class
 *
 *  This macro should be invoked in the public part of every class in a visitable hierarchy.
 */
#define LASS_UTIL_ACCEPT_VISITOR\
	virtual TVisitReturn accept(lass::util::VisitorBase& iVisitor)\
	{\
		return doAccept(*this, iVisitor);\
	}

#endif

// EOF
