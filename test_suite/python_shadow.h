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
typedef util::SharedPtr<const Spam> TConstSpamPtr;

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



template <typename IteratorType>
class ConstSpamIteratorAdapter
{
public:
	using iterator_category = typename std::iterator_traits<IteratorType>::iterator_category;
	using value_type = python::NoNone<TConstSpamPtr>;
	using difference_type = typename std::iterator_traits<IteratorType>::difference_type;
	using pointer = void;
	using reference = void;

	ConstSpamIteratorAdapter(IteratorType iterator):
		iterator_(iterator) 
	{
	}
	~ConstSpamIteratorAdapter() = default;
	ConstSpamIteratorAdapter& operator++()
	{
		iterator_++;
		return *this;
	}
	ConstSpamIteratorAdapter operator++(int)
	{
		ConstSpamIteratorAdapter retval = *this;
		++(*this);
		return retval;
	}
	bool operator==(const ConstSpamIteratorAdapter& other) const
	{
		return iterator_ == other.iterator_;
	}
	bool operator!=(const ConstSpamIteratorAdapter& other) const
	{
		return !(*this == other);
	}
	value_type operator*() const
	{
		return value_type(iterator_->reference());
	}
private:
	IteratorType iterator_;
};


class ShadowedIteratorContainer
{
public:
	using TValue = python::NoNone<TSpamPtr>;
	using TConstValue = python::NoNone<TConstSpamPtr>;
	using TItems = std::vector<TValue>;
	using TIterator = TItems::iterator;
	using TConstIterator = ConstSpamIteratorAdapter<TItems::const_iterator>;
	ShadowedIteratorContainer(TItems items);
	TIterator begin();
	TIterator end();
	TConstIterator begin() const;
	TConstIterator end() const;
	static python::NoNone<util::SharedPtr<ShadowedIteratorContainer>> make(TItems items);
	static python::NoNone<util::SharedPtr<const ShadowedIteratorContainer>> makeConst(TItems items);
private:
	TItems items_;
};


class ShadowedMemberIteratorContainer
{
public:
	using TValue = python::NoNone<TSpamPtr>;
	using TItems = std::vector<TValue>;
	using TIterator = TItems::iterator;
	using TConstIterator = ConstSpamIteratorAdapter<TItems::const_iterator>;
	ShadowedMemberIteratorContainer(TItems items);
	TIterator myBegin();
	TIterator myEnd();
	TConstIterator myBeginConst() const;
	TConstIterator myEndConst() const;
	static python::NoNone<util::SharedPtr<ShadowedMemberIteratorContainer>> make(TItems items);
	static python::NoNone<util::SharedPtr<const ShadowedMemberIteratorContainer>> makeConst(TItems items);
private:
	TItems items_;
};


class ShadowedFreeIteratorContainer
{
public:
	using TValue = python::NoNone<TSpamPtr>;
	using TItems = std::vector<TValue>;
	using TIterator = TItems::iterator;
	using TConstIterator = ConstSpamIteratorAdapter<TItems::const_iterator>;
	ShadowedFreeIteratorContainer(TItems items);
	static python::NoNone<util::SharedPtr<ShadowedFreeIteratorContainer>> make(TItems items);
	static python::NoNone<util::SharedPtr<const ShadowedFreeIteratorContainer>> makeConst(TItems items);
private:
	friend TIterator shadowedFreeIteratorContainerBegin(ShadowedFreeIteratorContainer& self);
	friend TIterator shadowedFreeIteratorContainerEnd(ShadowedFreeIteratorContainer& self);
	friend TConstIterator shadowedFreeIteratorContainerBeginConst(const ShadowedFreeIteratorContainer& self);
	friend TConstIterator shadowedFreeIteratorContainerEndConst(const ShadowedFreeIteratorContainer& self);
	TItems items_;
};

ShadowedFreeIteratorContainer::TIterator shadowedFreeIteratorContainerBegin(ShadowedFreeIteratorContainer& self);
ShadowedFreeIteratorContainer::TIterator shadowedFreeIteratorContainerEnd(ShadowedFreeIteratorContainer& self);
ShadowedFreeIteratorContainer::TConstIterator shadowedFreeIteratorContainerBeginConst(const ShadowedFreeIteratorContainer& self);
ShadowedFreeIteratorContainer::TConstIterator shadowedFreeIteratorContainerEndConst(const ShadowedFreeIteratorContainer& self);


class ShadowedIndexContainer
{
public:
	using TValue = python::NoNone<TSpamPtr>;
	using TConstValue = python::NoNone<TConstSpamPtr>;
	using TItems = std::vector<TValue>;
	ShadowedIndexContainer(TItems items);
	TValue operator[](size_t index);
	TConstValue operator[](size_t index) const;
	size_t size() const;
	static python::NoNone<util::SharedPtr<ShadowedIndexContainer>> make(TItems items);
	static python::NoNone<util::SharedPtr<const ShadowedIndexContainer>> makeConst(TItems items);
private:
	TItems items_;
};


class ShadowedFreeIndexContainer
{
public:
	using TValue = python::NoNone<TSpamPtr>;
	using TConstValue = python::NoNone<TConstSpamPtr>;
	using TItems = std::vector<TValue>;
	ShadowedFreeIndexContainer(TItems items);
	static python::NoNone<util::SharedPtr<ShadowedFreeIndexContainer>> make(TItems items);
	static python::NoNone<util::SharedPtr<const ShadowedFreeIndexContainer>> makeConst(TItems items);
private:
	friend size_t shadowedFreeIndexContainerSize(const ShadowedFreeIndexContainer& self);
	friend TValue shadowedFreeIndexContainerAt(ShadowedFreeIndexContainer& self, size_t index);
	friend TConstValue shadowedFreeIndexContainerAtConst(const ShadowedFreeIndexContainer& self, size_t index);
	TItems items_;
};

size_t shadowedFreeIndexContainerSize(const ShadowedFreeIndexContainer& self);
ShadowedFreeIndexContainer::TValue shadowedFreeIndexContainerAt(ShadowedFreeIndexContainer& self, size_t index);
ShadowedFreeIndexContainer::TConstValue shadowedFreeIndexContainerAtConst(const ShadowedFreeIndexContainer& self, size_t index);


PY_SHADOW_CLASS(LASS_DLL_EXPORT, PyShadowedIteratorContainer, ShadowedIteratorContainer);
PY_SHADOW_CLASS(LASS_DLL_EXPORT, PyShadowedMemberIteratorContainer, ShadowedMemberIteratorContainer);
PY_SHADOW_CLASS(LASS_DLL_EXPORT, PyShadowedFreeIteratorContainer, ShadowedFreeIteratorContainer);
PY_SHADOW_CLASS(LASS_DLL_EXPORT, PyShadowedIndexContainer, ShadowedIndexContainer);
PY_SHADOW_CLASS(LASS_DLL_EXPORT, PyShadowedFreeIndexContainer, ShadowedFreeIndexContainer);

}

}

PY_SHADOW_CASTERS(lass::test::PySpam)
PY_SHADOW_CASTERS(lass::test::PyShadowedIteratorContainer)
PY_SHADOW_CASTERS(lass::test::PyShadowedMemberIteratorContainer)
PY_SHADOW_CASTERS(lass::test::PyShadowedFreeIteratorContainer)
PY_SHADOW_CASTERS(lass::test::PyShadowedIndexContainer)
PY_SHADOW_CASTERS(lass::test::PyShadowedFreeIndexContainer)

#endif
