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

// --- Spam ----------------------------------------------------------------------------------------

Spam::~Spam()
{
}

std::string Spam::who() const
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

std::string Ham::who() const
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

std::string Bacon::who() const
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

std::string Eggs::who() const
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

std::auto_ptr<Spam> makeSpam(const std::string& iWhat)
{
	std::auto_ptr<Spam> result;
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

void spamToCppByCopy(Spam iSpam)
{
	LASS_COUT << "spamToCppByCopy: " << iSpam.who() << " @ " << iSpam.address() << "\n";
}

void spamToCppByConstReference(const Spam& iSpam)
{
	LASS_COUT << "spamToCppByConstReference: " << iSpam.who() << " @ " << iSpam.address() << "\n";
}

void spamToCppByReference(Spam& iSpam)
{
	LASS_COUT << "spamToCppByReference: " << iSpam.who() << " @ " << iSpam.address() << "\n";
}

void spamToCppByPointer(Spam* iSpam)
{
	LASS_COUT << "spamToCppByPointer: " << iSpam->who() << " @ " << iSpam->address() << "\n";
}

void spamToCppByAutoPointer(std::auto_ptr<Spam> iSpam)
{
	LASS_COUT << "spamToCppByAutoPointer: " << iSpam->who() << " @ " << iSpam->address() << "\n";
}

}

}

// EOF
