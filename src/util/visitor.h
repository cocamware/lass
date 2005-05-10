/** @file
 *  @author Bram de Greve (bramz@users.sourceforge.net)
 *  @author Tom De Muer (tomdemuer@users.sourceforge.net)
 *
 *  Distributed under the terms of the GPL (GNU Public License)
 *
 *  The LASS License:
 *
 *  Copyright 2004 Bram de Greve and Tom De Muer
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
 *      <i>The Loki Library, Copyright (c) 2001 by Andrei Alexandrescu\n
 *      This code (Loki) accompanies the book:\n
 *      Alexandrescu, Andrei. "Modern C++ Design: Generic Programming and Design Patterns Applied".
 *      Copyright (c) 2001. Addison-Wesley.\n
 *      Permission to use, copy, modify, distribute and sell this software (Loki) for any purpose is
 *      hereby granted without fee, provided that the above copyright notice appear in all copies and
 *      that both that copyright notice and this permission notice appear in supporting documentation.\n
 *      The author or Addison-Wesley Longman make no representations about the suitability of this
 *      software (Loki) for any purpose. It is provided "as is" without express or implied warranty.\n</i>
 *
 *  @section documentation
 *
 *  To use the visitor pattern, you go as following.  First, you must distinguish between the
 *  @e visited hierarchy and the @e visitor hierarchy.  Let's start what must be done with the
 *  @e visited hierarchy.
 *
 *  @subsection visitables
 *
 *  Suppose, we have some classes @c Foo and @c Bar to be visited, and both are are derived of
 *  @c Spam.  You must derived @c Spam of VisitableBase, and invoke the macro
 *  LASS_UTIL_ACCEPT_VISITOR in the private part of each class of the hierarchy.
 *
 *  @code
 *  class Spam: public lass::util::VisitableBase<>
 *  {
 *  private:
 *      LASS_UTIL_ACCEPT_VISITOR
 *  };
 *
 *  class Foo: public Spam
 *  {
 *  private:
 *      LASS_UTIL_ACCEPT_VISITOR
 *  };
 *
 *  class Bar: public Spam
 *  {
 *  private:
 *      LASS_UTIL_ACCEPT_VISITOR
 *  };
 *  @endcode
 *
 *  that's easy isn't it?  LASS_UTIL_ACCEPT_VISITOR implements in each derived class the accept function
 *  that is needed to switch the visitor over the different visitables.  All it does is passing the
 *  call to the VisitableBase, but it's crucial for the pattern.  Instead of using
 *  LASS_UTIL_ACCEPT_VISITOR, you can also write your own acceptor if you want something fancier,
 *  e.g. to iterate over a list of child objects.  This acceptor should be called @c doAccept ...
 *
 *  @code
 *  class List: public Spam
 *  {
 *  public:
 *      void addChild(Spam* iChild): children_.push_back(iChild) {}
 *  private:
 *      virtual TVisitReturn doAccept(lass::util::VisitorBase& iVisitor)
 *      {
 *			visit(*this, iVisitor);
 *          for (TChilderen::iterator i = children_.begin(); i != children_.end(); ++i)
 *          {
 *              (*i)->accept(iVisitor);
 *          }
 *          visitOnExit(*this, iVisitor);
 *      }
 *
 *      typedef std::list<Spam*> TChildren;
 *      TChildren children_;
 *  };
 *  @endcode
 *
 *  @c doAccept should call both @c visit and @c visitOnExit.  The former should be called before
 *	visiting children, the latter after.
 *
 *  if @c visit or @c visitOnExit don't know how to accept the visitor (or the visitor doesn't know
 *  how to visit the visitable), the @c onUnknownVisitor function of the @e CatchAll policy is 
 *  called. @c CatchAll of VisitableBase determines what to do on undetermined visits.  By default 
 *  the policy VisitNonStrict is used, what means the call is silently ignored and you can move on
 *  with the visit. Another one is VisitStrict which will throw an exception on every unknown visit.
 *  What you want is your choice, and you can always write your own policy.
 *
 *  <i>What about TVisitReturn?  We didn't talk about that one yet, did we?  Well, by default this will
 *  be @c void, meaning the acceptors and visits don't have a return value.  However, if it suits
 *  your needs, you could make it return something interesting.  That's when you can use
 *  TVisitReturn.</i> <b>[disabled]</b>
 *
 *  @subsection visitors
 *
 *  To declare a visitor, all you have to do is to derive of VisitorBase once, and of Visitor for
 *  every class of the visitable hierarchy you want to visit.  Suppose you want to visit objects of
 *  type Foo and Bar only, the you would write:
 *
 *  @code
 *  class MyVisitor:
 *      public lass::util::VisitorBase<>,
 *      public lass::util::Visitor<Foo>,
 *      public lass::util::Visitor<Bar>
 *  {
 *  private:
 *      virtual void doVisit(Foo& iFoo) { ... }
 *      virtual void doVisit(Bar& iBar) { ... }
 *		virtual void doVisitOnExit(Bar& iBar) { ... }
 *  };
 *  @endcode
 *
 *  objects of type Spam (non derived) or any other type of the Spam hiearchy that is not Foo or Bar
 *  will be redirected to the @c onUnknownVisitor function of the @e CatchAll policy of the visitable
 *  hierarchy.  That way, you only have to pull in and write visits for the types your interested in.
 *
 *  for every @c Visitor<X> being derived of, you'll have to implement @c doVisit(X&).  This one is
 *  called by @c visit in @c doAccept.  If you don't implement it, the compiler will complain.  
 *	You can also implement @c doVisitOnExit which will be called by @c visitOnExit in @c doAccept,
 *	but this is not required by the compiler.  If you don't implement it, it will default to an 
 *	empty function.
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
 *      public lass::util::VisitorBase<>,
 *      public lass::util::Visitor<Spam>
 *  {
 *  public:
 *      int count() const { return count_; }
 *  private:
 *      virtual void doVisit(Spam& iSpam) { ++count_; }
 *      int count_;
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
 *  Foo foo;
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
 *      public lass::util::VisitorBase<>,
 *      public lass::util::Visitor<Spam>
 *  {
 *  public:
 *      int operator()(Spam& iSpam)
 *      {
 *          count_ = 0;
 *          iSpam.accept(*this);
 *          return count_;
 *      }
 *  private:
 *      virtual void doVisit(Spam& iSpam) { ++count_; }
 *      int count_;
 *  };
 *
 *  Counter counter;
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
class LASS_DLL VisitorBase
{
public:
	virtual ~VisitorBase() {}
};



/** @ingroup VisitorPattern
 *  @brief a mix-in to provides a visitor capabilities to visit a class
 */
template <typename VisitableType>
class LASS_DLL Visitor
{
public:

	typedef VisitableType TVisitable;

	void visit(TVisitable& iVisited) { doVisit(iVisited); }
	void visitOnExit(TVisitable& iVisited) { doVisitOnExit(iVisited); }

private:

	virtual void doVisit(TVisitable& iVisited) = 0;
	virtual void doVisitOnExit(TVisitable& iVisited) {}
};



/** @ingroup VisitorPattern
 *  @brief catch-all policy doing nothing, default.
 *
 *  This catch-all policy will do nothing if a visitor class is not recognized, and return a
 *  value by the default constructor of @a VisitReturnType.
 *
 *  This is the default.
 */
template <typename VisitableType>
struct LASS_DLL VisitNonStrict
{
	static void onUnknownVisitor(VisitableType& iVisited, VisitorBase& iVisitor)
	{
	}
};



/** @ingroup VisitorPattern
 *  @brief catch-all policy raising an exception
 *
 *  This catch-all policy will raise an exception if a visitor class is not recognized.
 */
template <typename VisitableType>
struct LASS_DLL VisitStrict
{
	static void onUnknownVisitor(VisitableType& iVisited, VisitorBase& iVisitor)
	{
		LASS_THROW("Unacceptable visit: '" << typeid(iVisited).name() << "' can't accept '"
			<< typeid(iVisitor).name() << "' as visitor.");
	}
};



/** @ingroup VisitorPattern
 *  @brief base class of all visitable classes
 *
 *  derive of this class to make a hierarchy of visitable classes
 */
template
<
	template <typename> class CatchAll = VisitNonStrict
>
class LASS_DLL VisitableBase
{
public:

	virtual ~VisitableBase() {}
	void accept(VisitorBase& iVisitor) { doAccept(iVisitor); }

protected:

	template <typename T>
	static void visit(T& iVisited, VisitorBase& iVisitor)
	{
		if (Visitor<T>* p = dynamic_cast<Visitor<T>*>(&iVisitor))
		{
			p->visit(iVisited);
		}
		else
		{
			CatchAll<T>::onUnknownVisitor(iVisited, iVisitor);
		}
	}

	template <typename T>
	static void visitOnExit(T& iVisited, VisitorBase& iVisitor)
	{
		if (Visitor<T>* p = dynamic_cast<Visitor<T>*>(&iVisitor))
		{
			p->visitOnExit(iVisited);
		}
		else
		{
			CatchAll<T>::onUnknownVisitor(iVisited, iVisitor);
		}
	}

private:

	virtual void doAccept(VisitorBase& iVisitor) = 0;
};

}

}

/** @ingroup VisitorPattern
 *  @brief implements an acceptor in a visitable class
 *
 *  This macro should be invoked in the private part of every class in a visitable hierarchy.
 */
#define LASS_UTIL_ACCEPT_VISITOR\
	virtual void doAccept(lass::util::VisitorBase& iVisitor)\
	{\
		visit(*this, iVisitor);\
		visitOnExit(*this, iVisitor);\
	}

#endif

// EOF
