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
 *	Copyright (C) 2004-2010 the Initial Developer.
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

/** @defgroup Python
 *  @brief interface library to Python
 */

#ifndef LASS_GUARDIAN_OF_INCLUSION_UTIL_CONTAINER_H
#define LASS_GUARDIAN_OF_INCLUSION_UTIL_CONTAINER_H

#include "python_common.h"
#include "pyiteratorrange.h"
#include "../util/string_cast.h"
#include "../util/shared_ptr.h"
#include "../stde/extended_algorithm.h"
#include "../meta/type_traits.h"

#include <vector>
#include <list>
#include <deque>
#include <map>
#include <typeinfo>

namespace lass
{
namespace python
{
namespace impl
{

template <typename Container>
struct ContainerTraitsBase
{
	typedef Container container_type;
	typedef typename Container::value_type value_type;
	typedef typename Container::const_iterator const_iterator;
	typedef typename Container::iterator iterator;
	static size_t size(const container_type& c)
	{
		return c.size();
	}
	static const const_iterator begin(const container_type& c) 
	{ 
		return c.begin();
	}
	static const iterator begin(container_type& c) 
	{ 
		return c.begin();
	}
	template <typename It> 
	static const It next(It it, Py_ssize_t i = 1) 
	{ 
		std::advance(it, i); 
		return it;
	}
	template <typename InputIt> 
	static void insert(container_type& c, iterator i, InputIt first, InputIt last)
	{
		c.insert(i, first, last);
	}
	static void erase(container_type& c, iterator i)
	{
		c.erase(i);
	}
	static void erase(container_type& c, iterator first, iterator last)
	{
		c.erase(first, last);
	}
	static void clear(container_type& c)
	{
		c.clear();
	}
	static void reserve(container_type& /*c*/, size_t /*n*/) 
	{
	}
	static const util::SharedPtr<container_type> copy(const container_type& c) 
	{ 
		return util::SharedPtr<container_type>(new container_type(c)); 
	}
	static void inplace_repeat(container_type& c, Py_ssize_t n)
	{
		stde::inplace_repeat_c(c, n);
	}
};

template <typename Container>
struct ContainerTraits: ContainerTraitsBase<Container>
{
};

template <typename T, typename A>
struct ContainerTraits< std::vector<T, A> >: ContainerTraitsBase< std::vector<T, A> >
{
	typedef typename ContainerTraitsBase< std::vector<T, A> >::container_type container_type;
	static void reserve( container_type& c, size_t n ) 
	{ 
		c.reserve(n); 
	}	
};


class LASS_PYTHON_DLL ContainerImplBase
{
public:
	virtual ~ContainerImplBase() {};
	virtual bool clear() = 0;
	virtual Py_ssize_t length() const = 0;
	virtual PyObject* items() const = 0;
	virtual const TPyObjPtr asNative() const = 0;
	virtual const std::type_info& type() const = 0;
	virtual void* raw(bool writable) = 0;

	const std::string repr() const;
};


template <typename Container, typename Base> 
class ContainerImpl : public Base
{
public:
	typedef Container TContainer;
	typedef util::SharedPtr<Container> TContainerPtr;
	typedef util::SharedPtr<const Container> TConstContainerPtr;
	typedef ContainerTraits<Container> TContainerTraits;

	bool clear()
	{
		if (!checkWritable())
		{
			return false;
		}
		TContainerTraits::clear(*container_);
		return true;
	}
	Py_ssize_t length() const
	{
		const Py_ssize_t size = static_cast<Py_ssize_t>(TContainerTraits::size(*container_));
		LASS_ASSERT(size >= 0);
		return size;
	}
	PyObject* items() const
	{
		return new PyIteratorRange(container_->begin(), container_->end());
	}
	const std::type_info& type() const 
	{ 
		return typeid(TContainerPtr); 
	}
	void* raw(bool writable)
	{ 
		if (writable && readOnly_)
		{
			return 0;
		}
		return &container_; 
	}
protected:
	ContainerImpl(const TContainerPtr& container, bool readOnly = false): 
		container_(container), 
		readOnly_(readOnly)
	{
		LASS_ASSERT(container_);
	}
	bool checkWritable() const
	{
		if (readOnly_)
		{
			LockGIL LASS_UNUSED(lock);
			PyErr_SetString(PyExc_TypeError, "Container is read-only");
			return false;
		}
		return true;
	}
	const TContainer& container() const
	{
		return *container_;
	}
	TContainer& container()
	{
		return *container_;
	}
	const typename TContainerTraits::const_iterator begin() const
	{
		return TContainerTraits::begin(*container_);
	}
	const typename TContainerTraits::iterator begin()
	{
		return TContainerTraits::begin(*container_);
	}
	template <typename It> static const It next(It it, Py_ssize_t i)
	{
		return TContainerTraits::next(it, i);
	}
private:
	util::SharedPtr<Container> container_;
	bool readOnly_;
};


template 
<
	typename ShadowType,
	typename ConcreteTraits
>
struct ShadowTraitsContainer
{
	typedef typename PyObjectPtr<ShadowType>::Type TPyClassPtr;
	typedef ShadowType TCppClass;
	typedef TPyClassPtr TCppClassPtr;
	typedef typename PyObjectPtr<const ShadowType>::Type TConstCppClassPtr;

	template <typename Container> static int getObject(PyObject* obj, util::SharedPtr<Container>& container)
	{
		typedef typename meta::TypeTraits<Container>::TNonConst TNonConstContainer;
		util::SharedPtr<TNonConstContainer> temp;
		if (ConcreteTraits::getObjectImpl(obj, temp, false) != 0)
		{
			return 1;
		}
		container = temp.template constCast<Container>();
		return 0;
	}
	template <typename Container> static int getObject(PyObject* obj, Container& container)
	{
		util::SharedPtr<Container> temp;
		if (ConcreteTraits::getObjectImpl(obj, temp, false) != 0)
		{
			return 1;
		}
		container = *temp;
		return 0;
	}
	static int getObject(PyObject* obj, TPyClassPtr& shadow)
	{
		shadow = fromNakedToSharedPtrCast<ShadowType>(obj);
		return 0;
	}
	static int getObject(PyObject* obj, TConstCppClassPtr& shadow)
	{
		shadow = fromNakedToSharedPtrCast<const ShadowType>(obj);
		return 0;
	}

	template <typename T> static TPyClassPtr buildObject(const T& value)
	{
		return TPyClassPtr(new ShadowType(value));
	}
	static TPyClassPtr buildObject(const TPyClassPtr& shadow)
	{
		return shadow;
	}
};

}
}
}

#endif
