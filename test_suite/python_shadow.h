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
 *	Copyright (C) 2004-2011 the Initial Developer.
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

#ifndef LASS_GUARDIAN_OF_INCLUSION_TEST_PYTHON_SHADOW_H
#define LASS_GUARDIAN_OF_INCLUSION_TEST_PYTHON_SHADOW_H

#include "test_common.h"
#include "../lass/python/python_api.h"

namespace lass
{
namespace test
{

class Spam;
typedef util::SharedPtr<Spam> TSpamPtr;

class Spam
{
public:
	virtual ~Spam();
	virtual std::string virtualWho() const;
	std::string overridenWho() const;
	std::string address() const;

	lass::python::Self<TSpamPtr> clone() const;
private:
	virtual TSpamPtr doClone() const;
};


class Ham: public Spam
{
public:
	~Ham();
	std::string virtualWho() const override;
	std::string overridenWho() const;
	static void say(const std::string& iWhat);
private:
	TSpamPtr doClone() const override;
};

class Bacon: public Ham
{
public:
	~Bacon();
	std::string virtualWho() const override;
	std::string overridenWho() const;
private:
	TSpamPtr doClone() const override;
};

class Eggs: public Spam
{
public:
	Eggs(int iNumber = 2);
	~Eggs();
	std::string virtualWho() const override;
	std::string overridenWho() const;

	int number() const;
	void setNumber(int iNumber);

private:
	TSpamPtr doClone() const override;

	int number_;
};

TSpamPtr makeSpam(const std::string& iWhat);

TSpamPtr spamToCppByPointer(const TSpamPtr& a);
bool spamToCppByCopy(Spam a, const TSpamPtr b);
bool spamToCppByConstReference(const Spam& a, TSpamPtr b);
bool spamToCppByReference(Spam& a, const util::SharedPtr<const Spam>& b);

PY_SHADOW_CLASS(LASS_DLL_EXPORT, PySpam, Spam);
PY_SHADOW_CLASS_DERIVED(LASS_DLL_EXPORT, PyHam, Ham, PySpam)
PY_SHADOW_CLASS_DERIVED(LASS_DLL_EXPORT, PyBacon, Bacon, PyHam)
PY_SHADOW_CLASS_DERIVED(LASS_DLL_EXPORT, PyEggs, Eggs, PySpam)


}

}

typedef lass::test::PySpam TPySpam;
PY_SHADOW_DOWN_CASTERS(TPySpam)

#endif
