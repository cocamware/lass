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

#ifndef LASS_GUARDIAN_OF_INCLUSION_UTIL_VISITOR_H
#define LASS_GUARDIAN_OF_INCLUSION_UTIL_VISITOR_H

namespace lass
{
namespace util
{

class BadVisit: public Exception
{
public:
	BadVisit(const std::string& msg, const std::string& loc): Exception(msg, loc) {}
private:
	LASS_UTIL_EXCEPTION_PRIVATE_IMPL(BadVisit)
};



class LASS_DLL VisitorBase
{
public:
	virtual ~VisitorBase();
protected:
	VisitorBase();
};



template <typename T>
class Visitor
{
public:
	void preVisit(T& visitable)
	{
		doPreVisit(visitable);
	}
	void postVisit(T& visitable)
	{
		doPostVisit(visitable);
	}
private:
	virtual void doPreVisit(T& visitable) = 0;
	virtual void doPostVisit(T& visitable) {}
};



struct ThrowOnUnknownVisit
{
	template <typename T> static void onUnknownPreVisit(VisitorBase& visitor, T& visitable)
	{
		LASS_THROW_EX(BadVisit, "unkown previsit on type '" << typeid(T).name() << "'.");
	}
	template <typename T> static void onUnknownPostVisit(VisitorBase& visitor, T& visitable)
	{
		LASS_THROW_EX(BadVisit, "unkown postvisit on type '" << typeid(T).name() << "'.");
	}
};


struct IgnoreUnknownVisit
{
	template <typename T> static void onUnknownPreVisit(VisitorBase& visitor, T& visitable) {}
	template <typename T> static void onUnknownPostVisit(VisitorBase& visitor, T& visitable) {}
};



template <typename CatchAll = ThrowOnUnknownVisit>
class VisitableBase
{
public:
	virtual ~VisitableBase() {}

	void accept(VisitorBase& visitor) { doAccept(visitor); }

protected:

	template <typename T>
	void preAccept(VisitorBase& visitor, T& visitable)
	{
		if (Visitor<T>* p = dynamic_cast<Visitor<T>*>(&visitor))
		{
			p->preVisit(visitable);
		}
		else
		{
			CatchAll::onUnknownPreVisit(visitor, visitable);
		}
	}

	template <typename T>
	void postAccept(VisitorBase& visitor, T& visitable)
	{
		if (Visitor<T>* p = dynamic_cast<Visitor<T>*>(&visitor))
		{
			p->postVisit(visitable);
		}
		else
		{
			CatchAll::onUnknownPostVisit(visitor, visitable);
		}
	}

private:

	virtual void doAccept(VisitorBase& visitor) = 0;
};

#define LASS_UTIL_VISITOR_DO_ACCEPT\
	void doAccept(::lass::util::VisitorBase& visitor)\
	{\
		preAccept(visitor, *this);\
		postAccept(visitor, *this);\
	}

}
}

#endif

// EOF
