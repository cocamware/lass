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

#ifndef LASS_GUARDIAN_OF_INCLUSION_TEST_PYTHON_SHADOW_H
#define LASS_GUARDIAN_OF_INCLUSION_TEST_PYTHON_SHADOW_H

#include "test_common.h"
#include "../util/pyobject_plus.h"

namespace lass
{
namespace test
{

class Spam
{
public:
    virtual ~Spam();
    virtual std::string who() const;
    std::string address() const;
};

class Ham: public Spam
{
public:
    virtual ~Ham();
    virtual std::string who() const;
    static void say(const std::string& iWhat);
};

class Bacon: public Ham
{
public:
    virtual ~Bacon();
    virtual std::string who() const;
};

class Eggs: public Spam
{
public:
    Eggs(int iNumber = 2);
    virtual ~Eggs();
    virtual std::string who() const;
    
    int number() const;
    void setNumber(int iNumber);
    
private:
    int number_;
};

std::auto_ptr<Spam> makeSpam(const std::string& iWhat);

void spamToCppByCopy(Spam iSpam);
void spamToCppByConstReference(const Spam& iSpam);
void spamToCppByReference(Spam& iSpam);
void spamToCppByPointer(Spam* iSpam);
void spamToCppByAutoPointer(std::auto_ptr<Spam> iSpam);

}

}

#endif
