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
#include "python_shadow.h"

namespace lass
{
namespace test
{

PY_DECLARE_CLASS_NAME(PySpam, "Spam")
PY_CLASS_METHOD(PySpam, virtualWho)
PY_CLASS_METHOD(PySpam, overridenWho)
PY_CLASS_MEMBER_R(PySpam, address)

PY_DECLARE_CLASS_NAME(PyHam, "Ham")
PY_CLASS_CONSTRUCTOR_0(PyHam)
PY_CLASS_METHOD(PyHam, overridenWho)
PY_CLASS_STATIC_METHOD(PyHam, say)

PY_DECLARE_CLASS_NAME(PyBacon, "Bacon")
PY_CLASS_CONSTRUCTOR_0(PyBacon)
PY_CLASS_METHOD(PyBacon, overridenWho)

PY_DECLARE_CLASS_NAME(PyEggs, "Eggs")
PY_CLASS_CONSTRUCTOR_1(PyEggs, int)
PY_CLASS_METHOD(PyEggs, overridenWho)
PY_CLASS_MEMBER_RW(PyEggs, number, setNumber)

// --- Spam ----------------------------------------------------------------------------------------

Spam::~Spam()
{
}

std::string Spam::virtualWho() const
{
	return "Spam";
}

std::string Spam::overridenWho() const
{
	return "Spam";
}

std::string Spam::address() const
{
	return util::stringCast<std::string>(const_cast<Spam*>(this));
}

// --- Ham -----------------------------------------------------------------------------------------

Ham::~Ham()
{
}

std::string Ham::virtualWho() const
{
	return "Ham";
}

std::string Ham::overridenWho() const
{
	return "Ham";
}

void Ham::say(const std::string& iWhat)
{
	LASS_COUT << "Ham & " << iWhat << "\n";
}

// --- Bacon ---------------------------------------------------------------------------------------

Bacon::~Bacon()
{
}

std::string Bacon::virtualWho() const
{
	return "Bacon";
}

std::string Bacon::overridenWho() const
{
	return "Bacon";
}

// --- Eggs ----------------------------------------------------------------------------------------

Eggs::Eggs(int iNumber):
	Spam(),
	number_(iNumber)
{
}

Eggs::~Eggs()
{
}

std::string Eggs::virtualWho() const
{
	return util::stringCast<std::string>(number_) + " Eggs";
}

std::string Eggs::overridenWho() const
{
	return util::stringCast<std::string>(number_) + " Eggs";
}

int Eggs::number() const
{
	return number_;
}

void Eggs::setNumber(int iNumber)
{
	number_ = iNumber;
}

// --- Free ----------------------------------------------------------------------------------------

TSpamPtr makeSpam(const std::string& iWhat)
{
	TSpamPtr result;
	if (iWhat == "Spam")
	{
		result.reset(new Spam);
	}
	else if (iWhat == "Ham")
	{
		result.reset(new Ham);
	}
	else if (iWhat == "Bacon")
	{
		result.reset(new Bacon);
	}
	else if (iWhat == "Eggs")
	{
		result.reset(new Eggs);
	}
	else
	{
		LASS_THROW("no spam '" << iWhat << "' available.");
	}
	return result;
}

TSpamPtr spamToCppByPointer(const TSpamPtr& a)
{
	return a;
}

bool spamToCppByCopy(Spam a, const TSpamPtr b)
{
	return &a == b.get();
}

bool spamToCppByConstReference(const Spam& a, TSpamPtr b)
{
	return &a == b.get();
}

bool spamToCppByReference(Spam& a, const util::SharedPtr<const Spam>& b)
{
	return &a == b.get();
}

}

}

// EOF
