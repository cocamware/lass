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
 *	Copyright (C) 2004-2023 the Initial Developer.
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

#ifndef LASS_GUARDIAN_OF_INCLUSION_UTIL_VISITOR_H
#define LASS_GUARDIAN_OF_INCLUSION_UTIL_VISITOR_H

#include "util_common.h"

namespace lass
{
namespace util
{

class BadVisit: public ExceptionMixin<BadVisit>
{
public:
	BadVisit(std::string msg, std::string loc): ExceptionMixin<BadVisit>(std::move(msg), std::move(loc)) {}
	~BadVisit() noexcept {}
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
protected:
	virtual ~Visitor() {}
private:
	virtual void doPreVisit(T& visitable) = 0;
	virtual void doPostVisit(T& /*visitable*/) {}
};



struct ThrowOnUnknownVisit
{
	template <typename T> static void onUnknownPreVisit(VisitorBase& /*visitor*/, T& /*visitable*/)
	{
		LASS_THROW_EX(BadVisit, "unkown previsit on type '" << typeid(T).name() << "'.");
	}
	template <typename T> static void onUnknownPostVisit(VisitorBase& /*visitor*/, T& /*visitable*/)
	{
		LASS_THROW_EX(BadVisit, "unkown postvisit on type '" << typeid(T).name() << "'.");
	}
};


struct IgnoreUnknownVisit
{
	template <typename T> static void onUnknownPreVisit(VisitorBase& /*visitor*/, T& /*visitable*/) {}
	template <typename T> static void onUnknownPostVisit(VisitorBase& /*visitor*/, T& /*visitable*/) {}
};



template <typename CatchAll = ThrowOnUnknownVisit>
class VisitableBase
{
public:
	virtual ~VisitableBase() {}

	void accept(VisitorBase& visitor) { doAccept(visitor); }

	template <typename T>
	static void preAccept(VisitorBase& visitor, T& visitable)
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
	static void postAccept(VisitorBase& visitor, T& visitable)
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
	void doAccept(::lass::util::VisitorBase& visitor) override\
	{\
		preAccept(visitor, *this);\
		postAccept(visitor, *this);\
	}

}
}

#endif

// EOF
