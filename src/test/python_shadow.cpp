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
