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
 *	Copyright (C) 2004-2025 the Initial Developer.
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
PY_CLASS_METHOD(PySpam, clone)

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

PY_DECLARE_CLASS_NAME(PyShadowedIteratorContainer, "ShadowedIteratorContainer")
PY_CLASS_FREE_METHOD_NAME(PyShadowedIteratorContainer, python::makeContainerRangeView<ShadowedIteratorContainer>, lass::python::methods::_iter_);
PY_CLASS_FREE_METHOD_NAME(PyShadowedIteratorContainer, python::makeContainerRangeView<const ShadowedIteratorContainer>, lass::python::methods::_iter_);
PY_CLASS_STATIC_METHOD(PyShadowedIteratorContainer, make);
PY_CLASS_STATIC_METHOD(PyShadowedIteratorContainer, makeConst);

PY_DECLARE_CLASS_NAME(PyShadowedMemberIteratorContainer, "ShadowedMemberIteratorContainer")
PY_CLASS_FREE_METHOD_NAME(PyShadowedMemberIteratorContainer,
	python::makeMemberRangeViewFactory<ShadowedMemberIteratorContainer>(&ShadowedMemberIteratorContainer::myBegin, &ShadowedMemberIteratorContainer::myEnd),
	lass::python::methods::_iter_);
PY_CLASS_FREE_METHOD_NAME(PyShadowedMemberIteratorContainer,
	python::makeMemberRangeViewFactory<const ShadowedMemberIteratorContainer>(&ShadowedMemberIteratorContainer::myBeginConst, &ShadowedMemberIteratorContainer::myEndConst),
	lass::python::methods::_iter_);
PY_CLASS_STATIC_METHOD(PyShadowedMemberIteratorContainer, make);
PY_CLASS_STATIC_METHOD(PyShadowedMemberIteratorContainer, makeConst);

PY_DECLARE_CLASS_NAME(PyShadowedFreeIteratorContainer, "ShadowedFreeIteratorContainer")
PY_CLASS_FREE_METHOD_NAME(PyShadowedFreeIteratorContainer,
	python::makeFreeRangeViewFactory<ShadowedFreeIteratorContainer>(shadowedFreeIteratorContainerBegin, shadowedFreeIteratorContainerEnd),
	lass::python::methods::_iter_);
PY_CLASS_FREE_METHOD_NAME(PyShadowedFreeIteratorContainer,
	python::makeFreeRangeViewFactory<const ShadowedFreeIteratorContainer>(shadowedFreeIteratorContainerBeginConst, shadowedFreeIteratorContainerEndConst),
	lass::python::methods::_iter_);
PY_CLASS_STATIC_METHOD(PyShadowedFreeIteratorContainer, make);
PY_CLASS_STATIC_METHOD(PyShadowedFreeIteratorContainer, makeConst);

PY_DECLARE_CLASS_NAME(PyShadowedIndexContainer, "ShadowedIndexContainer")
using AtNonConst = ShadowedIndexContainer::TValue (ShadowedIndexContainer::*)(size_t);
using AtConst = ShadowedIndexContainer::TConstValue (ShadowedIndexContainer::*)(size_t) const;
PY_CLASS_FREE_METHOD_NAME(PyShadowedIndexContainer,
	python::makeIndexedRangeViewFactory<ShadowedIndexContainer>(static_cast<AtNonConst>(&ShadowedIndexContainer::operator[]), &ShadowedIndexContainer::size),
	lass::python::methods::_iter_);
PY_CLASS_FREE_METHOD_NAME(PyShadowedIndexContainer,
	python::makeIndexedRangeViewFactory<const ShadowedIndexContainer>(static_cast<AtConst>(&ShadowedIndexContainer::operator[]), &ShadowedIndexContainer::size),
	lass::python::methods::_iter_);
PY_CLASS_STATIC_METHOD(PyShadowedIndexContainer, make);
PY_CLASS_STATIC_METHOD(PyShadowedIndexContainer, makeConst);


PY_DECLARE_CLASS_NAME(PyShadowedFreeIndexContainer, "ShadowedFreeIndexContainer")
PY_CLASS_FREE_METHOD_NAME(PyShadowedFreeIndexContainer,
	python::makeFreeIndexedRangeViewFactory<ShadowedFreeIndexContainer>(shadowedFreeIndexContainerAt, shadowedFreeIndexContainerSize),
	lass::python::methods::_iter_);
PY_CLASS_FREE_METHOD_NAME(PyShadowedFreeIndexContainer,
	python::makeFreeIndexedRangeViewFactory<const ShadowedFreeIndexContainer>(shadowedFreeIndexContainerAtConst, shadowedFreeIndexContainerSize),
	lass::python::methods::_iter_);
PY_CLASS_STATIC_METHOD(PyShadowedFreeIndexContainer, make);
PY_CLASS_STATIC_METHOD(PyShadowedFreeIndexContainer, makeConst);


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

lass::python::Self<TSpamPtr> Spam::clone() const
{
	auto result = doClone();
	const Spam* cloned = result.get();
	LASS_ENFORCE(typeid(*cloned) == typeid(*this));
	return result;
}

TSpamPtr Spam::doClone() const
{
	return TSpamPtr(new Spam(*this));
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

TSpamPtr Ham::doClone() const
{
	return TSpamPtr(new Ham(*this));
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

TSpamPtr Bacon::doClone() const
{
	return TSpamPtr(new Bacon(*this));
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

TSpamPtr Eggs::doClone() const
{
	return TSpamPtr(new Eggs(*this));
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

// --- ShadowedIteratorContainer ------------------------------------------------------------------

ShadowedIteratorContainer::ShadowedIteratorContainer(TItems items) :
	items_(std::move(items))
{
}


ShadowedIteratorContainer::TIterator ShadowedIteratorContainer::begin()
{
	return items_.begin();
}


ShadowedIteratorContainer::TIterator ShadowedIteratorContainer::end()
{
	return items_.end();
}


ShadowedIteratorContainer::TConstIterator ShadowedIteratorContainer::begin() const
{
	return TConstIterator(items_.begin());
}


ShadowedIteratorContainer::TConstIterator ShadowedIteratorContainer::end() const
{
	return TConstIterator(items_.end());
}


python::NoNone<util::SharedPtr<ShadowedIteratorContainer>> ShadowedIteratorContainer::make(TItems items)
{
	return util::SharedPtr<ShadowedIteratorContainer>(new ShadowedIteratorContainer(items));
}


python::NoNone<util::SharedPtr<const ShadowedIteratorContainer>> ShadowedIteratorContainer::makeConst(TItems items)
{
	return util::SharedPtr<const ShadowedIteratorContainer>(new ShadowedIteratorContainer(items));
}



// --- ShadowedMemberIteratorContainer ------------------------------------------------------------------

ShadowedMemberIteratorContainer::ShadowedMemberIteratorContainer(TItems items) :
	items_(std::move(items))
{
}


ShadowedMemberIteratorContainer::TIterator ShadowedMemberIteratorContainer::myBegin()
{
	return items_.begin();
}


ShadowedMemberIteratorContainer::TIterator ShadowedMemberIteratorContainer::myEnd()
{
	return items_.end();
}


ShadowedMemberIteratorContainer::TConstIterator ShadowedMemberIteratorContainer::myBeginConst() const
{
	return TConstIterator(items_.begin());
}


ShadowedMemberIteratorContainer::TConstIterator ShadowedMemberIteratorContainer::myEndConst() const
{
	return TConstIterator(items_.end());
}


python::NoNone<util::SharedPtr<ShadowedMemberIteratorContainer>> ShadowedMemberIteratorContainer::make(TItems items)
{
	return util::SharedPtr<ShadowedMemberIteratorContainer>(new ShadowedMemberIteratorContainer(std::move(items)));
}


python::NoNone<util::SharedPtr<const ShadowedMemberIteratorContainer>> ShadowedMemberIteratorContainer::makeConst(TItems items)
{
	return util::SharedPtr<const ShadowedMemberIteratorContainer>(new ShadowedMemberIteratorContainer(std::move(items)));
}



// --- ShadowedFreeIteratorContainer ------------------------------------------------------------------

ShadowedFreeIteratorContainer::ShadowedFreeIteratorContainer(TItems items) :
	items_(std::move(items))
{
}


python::NoNone<util::SharedPtr<ShadowedFreeIteratorContainer>> ShadowedFreeIteratorContainer::make(TItems items)
{
	return util::SharedPtr<ShadowedFreeIteratorContainer>(new ShadowedFreeIteratorContainer(std::move(items)));
}


python::NoNone<util::SharedPtr<const ShadowedFreeIteratorContainer>> ShadowedFreeIteratorContainer::makeConst(TItems items)
{
	return util::SharedPtr<const ShadowedFreeIteratorContainer>(new ShadowedFreeIteratorContainer(std::move(items)));
}


ShadowedFreeIteratorContainer::TIterator shadowedFreeIteratorContainerBegin(ShadowedFreeIteratorContainer& self)
{
	return self.items_.begin();
}


ShadowedFreeIteratorContainer::TIterator shadowedFreeIteratorContainerEnd(ShadowedFreeIteratorContainer& self)
{
	return self.items_.end();
}


ShadowedFreeIteratorContainer::TConstIterator shadowedFreeIteratorContainerBeginConst(const ShadowedFreeIteratorContainer& self)
{
	return ShadowedFreeIteratorContainer::TConstIterator(self.items_.begin());
}


ShadowedFreeIteratorContainer::TConstIterator shadowedFreeIteratorContainerEndConst(const ShadowedFreeIteratorContainer& self)
{
	return ShadowedFreeIteratorContainer::TConstIterator(self.items_.end());
}



// --- ShadowedIndexContainer ------------------------------------------------------------------

ShadowedIndexContainer::ShadowedIndexContainer(TItems items) :
	items_(std::move(items))
{
}


ShadowedIndexContainer::TValue
ShadowedIndexContainer::operator[](size_t index)
{
	if (index >= items_.size())
	{
		LASS_THROW("Index out of bounds: " << index << " >= " << items_.size());
	}
	return items_[index];
}


ShadowedIndexContainer::TConstValue
ShadowedIndexContainer::operator[](size_t index) const
{
	if (index >= items_.size())
	{
		LASS_THROW("Index out of bounds: " << index << " >= " << items_.size());
	}
	return items_[index].reference().constCast<const Spam>();
}


size_t ShadowedIndexContainer::size() const
{
	return items_.size();
}


python::NoNone<util::SharedPtr<ShadowedIndexContainer>> ShadowedIndexContainer::make(TItems items)
{
	return util::SharedPtr<ShadowedIndexContainer>(new ShadowedIndexContainer(std::move(items)));
}


python::NoNone<util::SharedPtr<const ShadowedIndexContainer>> ShadowedIndexContainer::makeConst(TItems items)
{
	return util::SharedPtr<const ShadowedIndexContainer>(new ShadowedIndexContainer(std::move(items)));
}



// --- ShadowedFreeIndexContainer ------------------------------------------------------------------

ShadowedFreeIndexContainer::ShadowedFreeIndexContainer(TItems items) :
	items_(std::move(items))
{
}


ShadowedFreeIndexContainer::TValue
shadowedFreeIndexContainerAt(ShadowedFreeIndexContainer& self, size_t index)
{
	if (index >= self.items_.size())
	{
		LASS_THROW("Index out of bounds: " << index << " >= " << self.items_.size());
	}
	return self.items_[index];
}


ShadowedFreeIndexContainer::TConstValue
shadowedFreeIndexContainerAtConst(const ShadowedFreeIndexContainer& self, size_t index)
{
	if (index >= self.items_.size())
	{
		LASS_THROW("Index out of bounds: " << index << " >= " << self.items_.size());
	}
	return self.items_[index].reference().constCast<const Spam>();
}


size_t shadowedFreeIndexContainerSize(const ShadowedFreeIndexContainer& self)
{
	return self.items_.size();
}


python::NoNone<util::SharedPtr<ShadowedFreeIndexContainer>> ShadowedFreeIndexContainer::make(TItems items)
{
	return util::SharedPtr<ShadowedFreeIndexContainer>(new ShadowedFreeIndexContainer(std::move(items)));
}


python::NoNone<util::SharedPtr<const ShadowedFreeIndexContainer>> ShadowedFreeIndexContainer::makeConst(TItems items)
{
	return util::SharedPtr<const ShadowedFreeIndexContainer>(new ShadowedFreeIndexContainer(std::move(items)));
}


}

}

// EOF
