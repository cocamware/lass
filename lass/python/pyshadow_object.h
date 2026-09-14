/** @file
 *  @author Bram de Greve (bram@cocamware.com)
 *  @author Tom De Muer (tom@cocamware.com)
 *
 *  *** BEGIN LICENSE INFORMATION ***
 *
 *  The contents of this file are subject to the Common Public Attribution License
 *  Version 1.0 (the "License"); you may not use this file except in compliance with
 *  the License. You may obtain a copy of the License at
 *  https://lass.cocamware.com/cpal-license. The License is based on the
 *  Mozilla Public License Version 1.1 but Sections 14 and 15 have been added to cover
 *  use of software over a computer network and provide for limited attribution for
 *  the Original Developer. In addition, Exhibit A has been modified to be consistent
 *  with Exhibit B.
 *
 *  Software distributed under the License is distributed on an "AS IS" basis, WITHOUT
 *  WARRANTY OF ANY KIND, either express or implied. See the License for the specific
 *  language governing rights and limitations under the License.
 *
 *  The Original Code is LASS - Library of Assembled Shared Sources.
 *
 *  The Initial Developer of the Original Code is Bram de Greve and Tom De Muer.
 *  The Original Developer is the Initial Developer.
 *
 *  All portions of the code written by the Initial Developer are:
 *  Copyright (C) 2023-2026 the Initial Developer.
 *  All Rights Reserved.
 *
 *  Contributor(s):
 *
 *  Alternatively, the contents of this file may be used under the terms of the
 *  GNU General Public License Version 2 or later (the GPL), in which case the
 *  provisions of GPL are applicable instead of those above.  If you wish to allow use
 *  of your version of this file only under the terms of the GPL and not to allow
 *  others to use your version of this file under the CPAL, indicate your decision by
 *  deleting the provisions above and replace them with the notice and other
 *  provisions required by the GPL License. If you do not delete the provisions above,
 *  a recipient may use your version of this file under either the CPAL or the GPL.
 *
 *  *** END LICENSE INFORMATION ***
 */

/** @file
 *  @author Bram de Greve (bram@cocamware.com)
 *  @author Tom De Muer (tom@cocamware.com)
 *
 *  Distributed under the terms of the GPL (GNU Public License)
 *
 *  The LASS License:
 *
 *  Copyright 2004-2008 Bram de Greve and Tom De Muer
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
 *
 *  @note
 *      This header bundles the _EXPERIMENTAL_ code for quasi automatic
 *      wrapping of C++ object hierarchies into python shadow objects.  This
 *      code is still heavily under development and not ready for production use.
 */

#ifndef LASS_GUARDIAN_OF_INCLUSION_UTIL_PYSHADOW_OBJECT_H
#define LASS_GUARDIAN_OF_INCLUSION_UTIL_PYSHADOW_OBJECT_H

#include "python_common.h"
#include "pyobject_plus.h"
#include "shadowee_traits.h"
#include "../meta/is_derived.h"
#include <type_traits>

namespace lass
{
namespace python
{

/** @defgroup ShadowClasses Python Shadow Classes
 *  @brief Export C++ classes that don't derive from lass::python::PyObjectPlus
 *  @ingroup Python
 * 
 *  Shadow classes are LASS' wrapper classes that allow you to export C++ classes that don't
 *  derive from lass::python::PyObjectPlus to Python.
 * 
 *  They are defined by invoking two macros:
 *
 *  - PY_SHADOW_CLASS defines the shadow class. It's typical to name the shadow class by prefixing
 *    the native C++ class with Py. For example, Spam becomes PySpam. This macro can be invoked in
 *    any namespace.
 *
 *  - PY_SHADOW_CASTERS defines the ShadoweeTraits for the shadow class to make sure you can
 *    translate from the native C++ class to the shadow class and vice-versa. This must be invoked
 *    in the global namespace.
 * 
 *  Both macros are typically invoked in a header file (not necessarily the one that defines the
 *  original C++ classes), as both macros must be seen by all exports that use this class as parameter
 *  or return type.
 * 
 *  Once you have defined the Shadow class, you use the regular macros like PY_DECLARE_CLASS_NAME
 *  or PY_CLASS_METHOD to fully define the Python export for this class. But instead of using the
 *  C++ class as first argument, you use the shadow class.
 * 
 *  Here's a quick comparison between both techniques:
 * 
 *  ```cpp
 *  // Direct Python class                         | // Shadow Python class
 *                                                 |
 *  // spam.h                                      | // spam.h
 *                                                 |
 *  class Spam: public lass::python::PyObjectPlus  | class Spam
 *  {                                              | {
 *      PY_HEADER(lass::python::PyObjectPlus)      | 
 *  public:                                        | public:
 *      void method(int a, int b);                 |     void method(int a, int b);
 *  };                                             | };
 *                                                 |
 *  class Ham: public Spam                         | class Ham: public Spam
 *  {                                              | {
 *      PY_HEADER(Spam)                            | 
 *  public:                                        | public:
 *      void other(int c);                         |     void other(int c);
 *  };                                             | };
 *                                                 |
 *                                                 | PY_SHADOW_CLASS(LASS_DLL_EXPORT, PySpam, Spam)
 *                                                 | PY_SHADOW_CASTERS(PySpam)
 *                                                 |
 *                                                 | PY_SHADOW_CLASS_DERIVED(LASS_DLL_EXPORT, PyHam, Ham, PySpam)
 *                                                 | PY_SHADOW_CASTERS(PyHam)
 *                                                 |
 *  // spam.cpp                                    | // spam.cpp
 *                                                 |
 *  PY_DECLARE_CLASS(Spam)                         | PY_DECLARE_CLASS_NAME(PySpam, "Spam")
 *  PY_CLASS_METHOD(Spam, method)                  | PY_CLASS_METHOD(PySpam, method)
 *                                                 |
 *  PY_DECLARE_CLASS(Ham)                          | PY_DECLARE_CLASS_NAME(PyHam, "Ham")
 *  PY_CLASS_METHOD(Ham, other)                    | PY_CLASS_METHOD(PyHam, other)
 *  ```
 */


/** @ingroup ShadowClasses
 *  @brief ID-type to uniquely identify shadowee instances in the shadow cache
 */
using TShadoweeID = num::TuintPtr;

namespace impl
{

/** @ingroup ShadowClasses
 *  @internal
 */
enum ShadoweeConstness
{
	scConst,
	scNonConst
};

/** @ingroup ShadowClasses
 *  @internal
 */
class LASS_PYTHON_DLL ShadowBaseCommon: public PyObjectPlus
{
public:
	static TPyObjPtr findShadowObject(TShadoweeID shadoweeID, ShadoweeConstness constness);

protected:
	ShadowBaseCommon();
	~ShadowBaseCommon() override;

	void registerShadowee(TShadoweeID shadoweeID, ShadoweeConstness constness);
	void unregisterShadowee(TShadoweeID shadoweeID, ShadoweeConstness constness);
private:
	typedef std::pair<TShadoweeID, ShadoweeConstness> TCacheKey;

	struct CacheKeyHash
	{
		std::size_t operator()(const TCacheKey& key) const
		{
			std::size_t h1 = std::hash<TShadoweeID>()(key.first);
			std::size_t h2 = std::hash<ShadoweeConstness>()(key.second);
			return h1 ^ h2;
		}
	};

	typedef std::unordered_map<TCacheKey, ShadowBaseCommon*, CacheKeyHash> TCache;
	ShadowBaseCommon(const ShadowBaseCommon&);
	ShadowBaseCommon& operator=(const ShadowBaseCommon&);

	static TCache& cache();
};

/** @ingroup ShadowClasses
 *  @internal
 */
template <typename T>
struct IsShadowClass: public meta::IsDerived<T, ShadowBaseCommon> 
{
};

/** @ingroup ShadowClasses
 *  @internal
 */
template <typename T>
struct ShadowTraits
{
	enum { isShadow = IsShadowClass<T>::value };

private:

	template <typename U, bool shadow>
	struct Impl
	{
		typedef typename U::TShadoweePtr TCppClassPtr;
		typedef typename U::TConstShadoweePtr TConstCppClassPtr;
		typedef typename U::TShadowPtr TPyClassPtr;
		typedef typename U::TShadowee TCppClass;
		static int getObject(U* obj, TCppClassPtr& value)
		{
			typedef typename U::TPointerTraits TPointerTraits;
			const TCppClassPtr p = TPointerTraits::staticCast(obj->cppObject());
			if (TPointerTraits::isEmpty(p))
			{
				PyErr_Format(PyExc_TypeError, "PyObject is a const %s", T::_lassPyClassDef.name());
				return 1;
			}
			value = p;
			return 0;
		};
		static int getObject(U* obj, TConstCppClassPtr& value)
		{
			typedef typename U::TConstPointerTraits TConstPointerTraits;
			value = TConstPointerTraits::staticCast(obj->constCppObject());
			if (TConstPointerTraits::isEmpty(value))
			{
				PyErr_Format(PyExc_TypeError, "Trying to dereference null-PyObject of type %s", T::_lassPyClassDef.name());
				return 1;
			}
			return 0;
		};
		template <typename Ptr> static TPyClassPtr buildObject(const Ptr& value)
		{
			return U::make(value);
		}
	};

	template <typename U>
	struct Impl<U, false>
	{
		typedef typename PyObjectPtr<U>::Type TPyClassPtr;
		typedef TPyClassPtr TCppClassPtr;
		typedef typename PyObjectPtr<const U>::Type TConstCppClassPtr;
		typedef U TCppClass;
		static int getObject(U* obj, TPyClassPtr& value)
		{
			value = fromNakedToSharedPtrCast<U>(obj);
			return 0;
		};
		static int getObject(U* obj, TConstCppClassPtr& value)
		{
			value = fromNakedToSharedPtrCast<const U>(obj);
			return 0;
		};
		static const TPyClassPtr& buildObject(const TPyClassPtr& value)
		{
			return value;
		}
		// we can't create them from const cppObjects, as we can't track it as such ...
	};

	typedef Impl<T, isShadow> TImpl;

	static bool checkSubType(PyObject* obj)
	{
		LASS_ASSERT(obj);
		if (!PyType_IsSubtype(obj->ob_type , T::_lassPyClassDef.type() ))
		{
			PyErr_Format(PyExc_TypeError, "%s not castable to %s", obj->ob_type->tp_name, T::_lassPyClassDef.name());
			return false;
		}
		return true;
	}

public:

	typedef typename TImpl::TCppClass TCppClass;
	typedef typename TImpl::TCppClassPtr TCppClassPtr;
	typedef typename TImpl::TConstCppClassPtr TConstCppClassPtr;
	typedef typename TImpl::TPyClassPtr TPyClassPtr;
	typedef int (*TImplicitConverter)(PyObject* obj, TCppClassPtr&);

	template <typename Ptr> static int getObject(PyObject* obj, Ptr& value)
	{
		if (obj == Py_None)
		{
			value = Ptr();
			return 0;
		}
		if (PyType_IsSubtype(obj->ob_type , T::_lassPyClassDef.type()))
		{
			return TImpl::getObject(static_cast<T*>(obj), value);
		}
		TCppClassPtr p;
		if (tryImplicitConverters(obj, p) != 0)
		{
			return 1;
		}
		value = p;
		return 0;
	}
	static int getObject(PyObject* obj, TCppClass& value)
	{
		if (obj == Py_None)
		{
			PyErr_Format(PyExc_TypeError, "None not castable to %s", T::_lassPyClassDef.name());
			return 1;
		}
		TConstCppClassPtr p;
		if (obj->ob_type == T::_lassPyClassDef.type())
		{
			if (TImpl::getObject(static_cast<T*>(obj), p) != 0)
			{
				return 1;
			}
		}
		else
		{
			TCppClassPtr p2;
			if (tryImplicitConverters(obj, p2) != 0)
			{
				return 1;
			}
			p = p2;
		}
		try
		{
			value = *p;
		}
		LASS_PYTHON_CATCH_AND_RETURN_EX(1)
		return 0;
	}
	template <typename Ptr> static TPyClassPtr buildObject(const Ptr& value)
	{
		return TImpl::buildObject(value);
	}
	static TPyClassPtr buildObject(const TCppClass& value)
	{
		TCppClassPtr p(new TCppClass(value));
		return buildObject(p);
	}
	template <typename Deleter>
	static TPyClassPtr buildObject(std::unique_ptr<TCppClass, Deleter>&& value)
	{
		return buildObject(TCppClassPtr(std::move(value)));
	}
	static void addConverter(TImplicitConverter converter)
	{
		TImplicitConverterList* converters = implicitConverters();
		converters->push_back(converter);
	}

private:	
	typedef std::vector<TImplicitConverter> TImplicitConverterList;
	static TImplicitConverterList* implicitConverters_;

	static int tryImplicitConverters(PyObject* obj, TCppClassPtr& p)
	{
		const TImplicitConverterList* converters = implicitConverters();
		if (converters)
		{
			for (typename TImplicitConverterList::const_iterator i = converters->begin(); i != converters->end(); ++i)
			{
				if ((*i)(obj, p) == 0)
				{
					return 0;
				}
				PyErr_Clear();
			}
		}
		PyErr_Format(PyExc_TypeError, "%s not convertable to %s", obj->ob_type->tp_name, T::_lassPyClassDef.name());
		return 1;
	}

	static TImplicitConverterList* implicitConverters()
	{
		void*& slot = T::_lassPyClassDef.implicitConvertersSlot_;
		if (!slot)
		{
			slot = new TImplicitConverterList;
		}
		return static_cast<TImplicitConverterList*>(slot);
	}
};

//template <typename T> typename ShadowTraits<T>::TImplicitConverterList* ShadowTraits<T>::implicitConverters_ = 0;

/** @ingroup ShadowClasses
 *  @internal
 */
template <typename ShadowType, typename DerivedMakers> 
typename ShadowType::TShadowPtr makeShadow(
		const typename ShadowType::TConstShadoweePtr& shadowee, const DerivedMakers* derivedMakers,
		impl::ShadoweeConstness constness)
{
	typedef typename ShadowType::TShadowPtr TShadowPtr;
	typedef typename ShadowType::TConstPointerTraits TConstPointerTraits;

	LASS_ASSERT(!TConstPointerTraits::isEmpty(shadowee));
	if (const auto p = impl::ShadowBaseCommon::findShadowObject(TConstPointerTraits::id(shadowee), constness))
	{
		LASS_ASSERT(PyObject_IsInstance(p.get(), reinterpret_cast<PyObject*>(ShadowType::_lassPyClassDef.type())));
		return p.template staticCast<ShadowType>();
	}
	if (derivedMakers)
	{
		for (typename DerivedMakers::const_iterator i = derivedMakers->begin(); i != derivedMakers->end(); ++i)
		{
			if (const TShadowPtr p = (*i)(shadowee, constness))
			{
				return p;
			}
		}
	}
	return TShadowPtr(impl::fixObjectType(new ShadowType(shadowee, constness)));
}

/** @ingroup ShadowClasses
 *  @internal
 */
template <typename Makers, typename Maker> void registerMaker(Makers*& makers, Maker maker)
{
	if (!makers)
	{
		makers = new Makers;
	}
	makers->push_back(maker);
}

/** @ingroup ShadowClasses
 *  @internal
 */
template <typename DestPyType, typename SourceCppType>
int defaultConvertor(PyObject* object, typename lass::python::impl::ShadowTraits<DestPyType>::TCppClassPtr& p)
{
	typedef typename lass::python::impl::ShadowTraits<DestPyType>::TCppClass TCppClass;
	typedef typename lass::python::impl::ShadowTraits<DestPyType>::TCppClassPtr TPtr;
	SourceCppType source;
	if (pyGetSimpleObject(object, source) != 0)
	{
		return 1;
	}
	p = TPtr(new TCppClass(source));
	return 0;
}

}


/** @addtogroup ShadowClasses
 *  
 *  @par Pointer-traits
 * 
 *  Shadow-classes need a pointer type to store their shadowee instances. This is
 *  defined by the pointer traits chosen for the shadow class.
 * 
 *  By default, the SharedPointerTraits is used with util::SharedPtr as shadowee pointer,
 *  but you can specify a custom one using PY_SHADOW_CLASS_PTRTRAITS.
 * 
 *  Available pointer traits are:
 *  - SharedPointerTraits
 *  - NakedPointerTraits
 *  - StdSharedPointerTraits
 * 
 *  To make your custom pointer traits, copy the pattern of these three.
 * 
 *  @note `id` must return an ID that uniquely identifies a shadowee instance for as long
 *  as it is alive and at least one shadow pointer references it (when it's removed from
 *  the shadow cache). Return 0 to opt-out from the shadow cache.
 */

/** @ingroup ShadowClasses
 *  @brief Pointer-traits for Python Shadow classes that use lass::util::SharedPtr for storage
 */
template <typename T, template <typename, typename> class S = util::ObjectStorage, typename C = util::DefaultCounter>
struct SharedPointerTraits
{
	/** Pointer to shadowee type */
	typedef util::SharedPtr<T, S, C> TPtr;
	
	/** Rebind pointer traits to other shadowee type */
	template <typename U> struct Rebind
	{
		typedef SharedPointerTraits<U, S, C> Type;
	};

	/** util::SharedPtr already handles reference counts */
	static void acquire(const TPtr&) {}
	/** util::SharedPtr already handles reference counts */
	static void release(const TPtr&) {}

	/** Return true when storing a nullptr */
	static bool isEmpty(const TPtr& p)  
	{ 
		return p.isEmpty(); 
	}
	/** Get the raw pointer to the shadowee */
	static T* get(const TPtr& p) 
	{ 
		return p.get(); 
	}

	/** Convert shadowee pointer to ID for shadow cache */
	static TShadoweeID id(const TPtr& p) 
	{
		return reinterpret_cast<TShadoweeID>(p.get());
	}

	/** Perform static cast on shadowee pointer */
	template <typename U> static TPtr staticCast(const util::SharedPtr<U, S, C>& p) 
	{
		return p.template staticCast<T>();
	}
	/** Perform dynamic cast on shadowee pointer */
	template <typename U> static TPtr dynamicCast(const util::SharedPtr<U, S, C>& p) 
	{
		return p.template dynamicCast<T>();
	}
	/** Perform const cast on shadowee pointer */
	template <typename U> static TPtr constCast(const util::SharedPtr<U, S, C>& p)
	{
		return p.template constCast<T>();
	}
};


/** @ingroup ShadowClasses
 *  @brief Pointer-traits for Python Shadow classes that use raw `*` pointers for storage
 * 
 *  @warning The NakedPointerTraits does not govern the lifetime of the shadowee, so you
 *           must make sure that it outlives every Python reference!
 */
template <typename T>
struct NakedPointerTraits
{
	/** Pointer to shadowee type */
	typedef T* TPtr;

	/** Rebind pointer traits to other shadowee type */
	template <typename U> struct Rebind
	{
		typedef NakedPointerTraits<U> Type;
	};

	/** Raw pointers have no ownership rules */
	static void acquire(TPtr) {} 
	/** Raw pointers have no ownership rules */
	static void release(TPtr) {}

	/** Return true when storing a nullptr */
	static bool isEmpty(TPtr p)  
	{ 
		return p == 0; 
	}
	/** Get the raw pointer to the shadowee */
	static T* get(TPtr p)
	{ 
		return p; 
	}

	/** Convert shadowee pointer to ID for shadow cache */
	static TShadoweeID id(TPtr p)
	{
		return reinterpret_cast<TShadoweeID>(p);
	}

	/** Perform static cast on shadowee pointer */
	template <typename U> static TPtr staticCast(U* p)
	{
		return static_cast<TPtr>(p);
	}
	/** Perform dynamic cast on shadowee pointer */
	template <typename U> static TPtr dynamicCast(U* p)
	{
		return dynamic_cast<TPtr>(p);
	}
	/** Perform const cast on shadowee pointer */
	template <typename U> static TPtr constCast(U* p)
	{
		return const_cast<TPtr>(p);
	}
};


/** @ingroup ShadowClasses
 *  @brief Pointer-traits for Python Shadow classes that use `std::shared_ptr` for storage
 */
template <typename T>
struct StdSharedPointerTraits
{
	/** Pointer to shadowee type */
	typedef std::shared_ptr<T> TPtr;

	/** Rebind pointer traits to other shadowee type */
	template <typename U> struct Rebind
	{
		typedef StdSharedPointerTraits<U> Type;
	};

	/** std::shared_ptr already handles reference counts */
	static void acquire(const TPtr&) {}
	/** std::shared_ptr already handles reference counts */
	static void release(const TPtr&) {}
	
	/** Return true when storing a nullptr */
	static bool isEmpty(const TPtr& p)
	{
		return !p;
	}
	/** Get the raw pointer to the shadowee */
	static T* get(const TPtr& p)
	{
		return p.get();
	}

	/** Convert shadowee pointer to ID for shadow cache */
	static TShadoweeID id(TPtr p)
	{
		return reinterpret_cast<TShadoweeID>(p.get());
	}

	/** Perform static cast on shadowee pointer */
	template <typename U> static TPtr staticCast(const std::shared_ptr<U>& p)
	{
		return std::static_pointer_cast<T>(p);
	}
	/** Perform dynamic cast on shadowee pointer */
	template <typename U> static TPtr dynamicCast(const std::shared_ptr<U>& p)
	{
		return std::dynamic_pointer_cast<T>(p);
	}
	/** Perform const cast on shadowee pointer */
	template <typename U> static TPtr constCast(const std::shared_ptr<U>& p)
	{
		return std::const_pointer_cast<T>(p);
	}
};



/** @ingroup ShadowClasses
 *  @internal
 */
template 
<
	typename ShadowType,
	typename ShadoweeType, 
	typename ParentShadowType,
	typename PointerTraits = SharedPointerTraits<ShadoweeType>
>
class ShadowClass: public ParentShadowType
{
public:
	typedef ShadoweeType TShadowee;
	typedef ShadowType TShadow;
	typedef ParentShadowType TParentShadow;
	typedef typename PointerTraits::template Rebind<ShadoweeType>::Type TPointerTraits;
	typedef typename PointerTraits::template Rebind<const ShadoweeType>::Type TConstPointerTraits;
	typedef typename TPointerTraits::TPtr TShadoweePtr;
	typedef typename TConstPointerTraits::TPtr TConstShadoweePtr;
	typedef typename PyObjectPtr<ShadowType>::Type TShadowPtr;

	static TShadowPtr make(const TShadoweePtr& shadowee)
	{
		return impl::makeShadow<ShadowType>(shadowee, derivedMakers_, impl::scNonConst);
	}
	static TShadowPtr make(const TConstShadoweePtr& shadowee)
	{
		return impl::makeShadow<ShadowType>(shadowee, derivedMakers_, impl::scConst);
	}
	static void registerWithParent()
	{
		ParentShadowType::registerDerivedMaker(ShadowClass::makeParent);
	}

protected:
	typedef TShadowPtr (*TDerivedMaker)(const TConstShadoweePtr&, impl::ShadoweeConstness);

	ShadowClass(const TConstShadoweePtr& shadowee, impl::ShadoweeConstness constness): 
		ParentShadowType(shadowee, constness)
	{
	}
	static void registerDerivedMaker(TDerivedMaker derivedMaker)
	{
		impl::registerMaker(derivedMakers_, derivedMaker);
	}

private:
	typedef std::vector<TDerivedMaker> TDerivedMakers;

	typedef typename TParentShadow::TConstShadoweePtr TParentConstShadoweePtr;
	typedef typename TParentShadow::TShadowPtr TParentShadowPtr;

	static TParentShadowPtr makeParent(const TParentConstShadoweePtr& shadowee, impl::ShadoweeConstness constness)
	{
		const TConstShadoweePtr p = TConstPointerTraits::dynamicCast(shadowee);
		if (TConstPointerTraits::isEmpty(p))
		{
			return TParentShadowPtr();
		}
		return impl::makeShadow<ShadowType>(p, derivedMakers_, constness);
	}

	static TDerivedMakers* derivedMakers_;
};

template <typename S, typename T, typename P, typename PT> 
typename ShadowClass<S, T, P, PT>::TDerivedMakers* ShadowClass<S, T, P, PT>::derivedMakers_ = 0;



/** @ingroup ShadowClasses
 *  @internal
 */
template
<
	typename ShadowType,
	typename ShadoweeType, 
	typename PointerTraits
>
class ShadowClass<ShadowType, ShadoweeType, PyObjectPlus, PointerTraits>: public impl::ShadowBaseCommon
{
public:
	typedef ShadoweeType TShadowee;
	typedef ShadowType TShadow;
	typedef typename PointerTraits::template Rebind<ShadoweeType>::Type TPointerTraits;
	typedef typename PointerTraits::template Rebind<const ShadoweeType>::Type TConstPointerTraits;
	typedef typename TPointerTraits::TPtr TShadoweePtr;
	typedef typename TConstPointerTraits::TPtr TConstShadoweePtr;
	typedef typename PyObjectPtr<ShadowType>::Type TShadowPtr;

	const TShadoweePtr cppObject() const
	{
		if (constness_ == impl::scConst)
		{
			return TShadoweePtr();
		}
		return TPointerTraits::constCast(shadowee_);
	}
	const TConstShadoweePtr& constCppObject() const
	{
		return shadowee_;
	}
	static TShadowPtr make(const TShadoweePtr& shadowee)
	{
		return impl::makeShadow<ShadowType>(shadowee, derivedMakers_, impl::scNonConst);
	}
	static TShadowPtr make(const TConstShadoweePtr& shadowee)
	{
		return impl::makeShadow<ShadowType>(shadowee, derivedMakers_, impl::scConst);
	}
	static void registerWithParent()
	{
	}
protected:
	typedef TShadowPtr (*TDerivedMaker)(const TConstShadoweePtr&, impl::ShadoweeConstness);
	ShadowClass(const TConstShadoweePtr& shadowee, impl::ShadoweeConstness constness): 
		shadowee_(shadowee),
		constness_(constness)
	{
		TConstPointerTraits::acquire(shadowee_);
		impl::ShadowBaseCommon::registerShadowee(TConstPointerTraits::id(shadowee_), constness_);
	}
	~ShadowClass()
	{
		impl::ShadowBaseCommon::unregisterShadowee(TConstPointerTraits::id(shadowee_), constness_);
		TConstPointerTraits::release(shadowee_);
	}
	static void registerDerivedMaker(TDerivedMaker derivedMaker)
	{
		impl::registerMaker(derivedMakers_, derivedMaker);
	}
private:
	typedef std::vector<TDerivedMaker> TDerivedMakers;

	static TDerivedMakers* derivedMakers_;
	TConstShadoweePtr shadowee_;
	impl::ShadoweeConstness constness_;
};

template <typename S, typename T, typename PT> 
typename ShadowClass<S, T, PyObjectPlus, PT>::TDerivedMakers* ShadowClass<S, T, PyObjectPlus, PT>::derivedMakers_ = 0;



/** @ingroup ShadowClasses
 *  @brief Helper to get the pointer type holding the shadowee in a shadow object
 * 
 *  @tparam ShadoweeType native C++ class, either the shadowee type being wrapped by
 *                       a shadow class, or a direct exported type that derives from
 *                       lass::python::PyObjectPlus.
 * 
 *  This helper gives you the correct pointer type to store a C++ class on the heap
 *  (either a shadowee type, or a direct Python class) that is compatible with the
 *  defined Python exports
 *  
 *  If @a ShadoweeType is a shadowee type, this will be the pointer type that holds
 *  the shadowee object in the shadow object, and it's defined by the pointer traits
 *  passed to PY_SHADOW_CLASS_EX() or PY_SHADOW_CLASS_PTRTRAITS()
 *  (or `SharedPointerTraits<ShadoweeType>` if you use the default PY_SHADOW_CLASS() ).
 *  Depending on the constness of the shadowee, you will either get a pointer to a
 *  non-const @a ShadoweeType or a pointer to a const @a ShadoweeType.
 * 
 *  If @a ShadoweeType is _not_ a shadowed type, but is instead a direct export and 
 *  derives directly or indirectly from lass::python::PyObjectPlus, then the pointer
 *  type is simply `lass::python::PyObjectPtr<ShadoweeType>::Type`.
 */
template <typename ShadoweeType>
using ShadoweePtr = std::conditional_t<std::is_const_v<ShadoweeType>,
	typename impl::ShadowTraits<typename ShadoweeTraits<ShadoweeType>::TShadow>::TConstCppClassPtr,
	typename impl::ShadowTraits<typename ShadoweeTraits<ShadoweeType>::TShadow>::TCppClassPtr
>;

}

}

/** @ingroup ShadowClasses
 *  @brief Declare Python shadow class with full control
 *
 *  @param dllInterface desired DLL-interface of shadow class
 *  @param i_PyObjectShadowClass Unique C++ class identifier (unqualified name) of shadow class
 *  @param t_CppClass typename of the shadowee, the native C++ class being shadowed
 *  @param t_PyObjectParent the shadow's class parent (either another shadow class or lass::python::PyObjectPlus),
 *                          this will be the base class of the Python type.
 *  @param t_PointerTraits complete type of pointer-traits to be used for shadowee storage.
 */
#define PY_SHADOW_CLASS_EX(dllInterface, i_PyObjectShadowClass, t_CppClass, t_PyObjectParent, t_PointerTraits) \
	class dllInterface i_PyObjectShadowClass : \
		public ::lass::python::ShadowClass< i_PyObjectShadowClass, t_CppClass, t_PyObjectParent, t_PointerTraits > \
	{ \
		PY_HEADER(t_PyObjectParent) \
		static void _lassPyClassRegisterHook() { registerWithParent(); } \
	public: \
		i_PyObjectShadowClass(const TConstShadoweePtr& shadowee, ::lass::python::impl::ShadoweeConstness constness): \
			::lass::python::ShadowClass< i_PyObjectShadowClass, t_CppClass, t_PyObjectParent, t_PointerTraits >(shadowee, constness) \
		{ \
		} \
	}; \
	/**/

/** @ingroup ShadowClasses
 *  @brief Declare Python shadow class with custom pointer traits
 * 
 *  The pointer-traits define how shadowee classes will be stored as pointer in the shadow class.
 *  Only specify the pointer-traits template name, the macro will add @a t_CppClass as template argument.
 * 
 *  @param dllInterface desired DLL-interface of shadow class
 *  @param i_PyObjectShadowClass Unique C++ class identifier (unqualified name) of shadow class
 *  @param t_CppClass typename of the shadowee, the native C++ class being shadowed
 *  @param pointerTraits shadowee pointer-traits template name
 */
#define PY_SHADOW_CLASS_PTRTRAITS(dllInterface, i_PyObjectShadowClass, t_CppClass, pointerTraits)\
	PY_SHADOW_CLASS_EX(dllInterface, i_PyObjectShadowClass, t_CppClass, ::lass::python::PyObjectPlus, pointerTraits < t_CppClass > )

/** @ingroup ShadowClasses
 *  @brief Declare Python shadow class with util::SharedPtr as default shadowee pointer type
 * 
 *  @param dllInterface desired DLL-interface of shadow class
 *  @param i_PyObjectShadowClass Unique C++ class identifier (unqualified name) of shadow class
 *  @param t_CppClass typename of the shadowee, the native C++ class being shadowed
 */
#define PY_SHADOW_CLASS(dllInterface, i_PyObjectShadowClass, t_CppClass)\
	PY_SHADOW_CLASS_EX(dllInterface, i_PyObjectShadowClass, t_CppClass, ::lass::python::PyObjectPlus, ::lass::python::SharedPointerTraits< t_CppClass >)

/** @ingroup ShadowClasses
 *  @brief Declare Python shadow child class with a parent
 * 
 *  Register a derived shadow class to reflect the same polymorphic class hierarchy in Python as in C++.
 *  If `Ham` derives from `Spam`, then a `SharedPtr<Spam>` pointer that contains a `Ham` instance will
 *  properly be returned as a `Ham` object in Python.
 * 
 *  Derived shadow classes use the same pointer traits as the parent class, so you don't need to
 *  specify it again.
 * 
 *  @param dllInterface desired DLL-interface of shadow class
 *  @param i_PyObjectShadowClass Unique C++ class identifier (unqualified name) of shadow class
 *  @param t_CppClass typename of the shadowee, the native C++ class being shadowed
 *  @param t_PyObjectShadowParent the parent's shadow class, this will be the base class of the Python type.
 */
#define PY_SHADOW_CLASS_DERIVED(dllInterface, i_PyObjectShadowClass, t_CppClass, t_PyObjectShadowParent)\
	PY_SHADOW_CLASS_EX(dllInterface, i_PyObjectShadowClass, t_CppClass, t_PyObjectShadowParent, t_PyObjectShadowParent::TPointerTraits::Rebind< t_CppClass >::Type )

/** @ingroup ShadowClasses
 *  @brief Deprecated alias for PY_SHADOW_CLASS_EX
 *  @deprecated Use PY_SHADOW_CLASS_EX instead, it's a direct 1:1 replacement
 */
#define PY_SHADOW_CLASS_NOCONSTRUCTOR_EX(dllInterface, i_PyObjectShadowClass, t_CppClass, t_PyObjectBase, t_PyObjectParent)\
	PY_SHADOW_CLASS_EX(dllInterface, i_PyObjectShadowClass, t_CppClass, t_PyObjectBase, t_PyObjectParent)

/** @ingroup ShadowClasses
 *  @brief Deprecated alias for PY_SHADOW_CLASS
 *  @deprecated Use PY_SHADOW_CLASS instead, it's a direct 1:1 replacement
 */
#define PY_SHADOW_CLASS_NOCONSTRUCTOR(dllInterface, i_PyObjectShadowClass, t_CppClass)\
	PY_SHADOW_CLASS(dllInterface, i_PyObjectShadowClass, t_CppClass)

/** @ingroup ShadowClasses
 *  @brief Deprecated alias for PY_SHADOW_CLASS
 *  @deprecated Use PY_SHADOW_CLASS instead, it's a direct 1:1 replacement
 */
#define PY_WEAK_SHADOW_CLASS(dllInterface, i_PyObjectShadowClass, t_CppClass)\
	PY_SHADOW_CLASS(dllInterface, i_PyObjectShadowClass, t_CppClass)

/** @ingroup ShadowClasses
 *  @brief Deprecated alias for PY_SHADOW_CLASS
 *  @deprecated Use PY_SHADOW_CLASS instead, it's a direct 1:1 replacement
 */
#define PY_WEAK_SHADOW_CLASS_NOCONSTRUCTOR(dllInterface, i_PyObjectShadowClass, t_CppClass)\
	PY_SHADOW_CLASS(dllInterface, i_PyObjectShadowClass, t_CppClass)

/** @ingroup ShadowClasses
 *  @brief Deprecated
 *  @deprecated This call has no effect and can be removed
 */
#define PY_SHADOW_CLASS_ENABLE_AUTOMATIC_INVALIDATION(i_PyObjectShadowClass)

/** @ingroup ShadowClasses
 *  @brief Deprecated alias for PY_SHADOW_CLASS_DERIVED
 *  @deprecated Use PY_SHADOW_CLASS_DERIVED instead, it's a direct 1:1 replacement
 */
#define PY_SHADOW_CLASS_DERIVED_NOCONSTRUCTOR(dllInterface, i_PyObjectShadowClass, t_CppClass, t_PyObjectShadowParent)\
	PY_SHADOW_CLASS_DERIVED(dllInterface, i_PyObjectShadowClass, t_CppClass, t_PyObjectShadowParent)




/** @ingroup ShadowClasses
 *  @brief Define lass::python::ShadoweeTraits for shadow class
 * 
 *  This ensures that shadowee class becomes usable as parameter or return type.
 *  If `PySpam` is @a t_ShadowObject, the shadow class of `Spam`, then `Spam`,
 *  `const Spam&`, `Spam*`, `lass::util::SharedPtr<Spam>`, etc. all become
 *  usable as parameter or return types.
 * 
 *  @param t_ShadowObject fully qualified typename of Python Shadow class
 * 
 *  @note This macro **MUST** be invoked in the global namespace
 * 
 *  @note This macro **MUST** be invoked in the same header that declares the
 *        shadow class with PY_SHADOW_CLASS or similar. Every translation unit
 *        that uses the shadow class must include it.
 */
#define PY_SHADOW_CASTERS(t_ShadowObject)\
namespace lass \
{ \
namespace python \
{ \
	template <> struct ShadoweeTraits< t_ShadowObject::TShadowee >: ::lass::meta::True \
	{ \
		typedef t_ShadowObject TShadow; \
		/*typedef impl::ShadowTraits< t_ShadowObject > TShadowTraits;*/ \
		typedef t_ShadowObject::TPointerTraits TPointerTraits; \
	}; \
} \
} \
/**/



/** @ingroup ShadowClasses
 *  @brief Deprecated alias for PY_SHADOW_CASTERS
 *  @deprecated Use PY_SHADOW_CASTERS instead, it's a direct 1:1 replacement
 */
#define PY_SHADOW_DOWN_CASTERS(t_ShadowObject)\
	PY_SHADOW_CASTERS(t_ShadowObject)

/** @ingroup ShadowClasses
 *  @brief Deprecated alias for PY_SHADOW_CASTERS
 *  @deprecated Use PY_SHADOW_CASTERS instead, it's a direct 1:1 replacement
 */
#define PY_SHADOW_DOWN_CASTERS_NOCONSTRUCTOR(t_ShadowObject)\
	PY_SHADOW_CASTERS(t_ShadowObject)



/** @ingroup ShadowClasses
 *  @brief Add implicit conversion function to Python class
 * 
 *  Add a conversion function to a Python class to implicitly convert a Python object to the C++ class when it is being
 *  passed as a parameter to a C++ function
 * 
 *  By default, you need to convert types by explicitly constructing an object in Python, or you must overload the
 *  function on different types. By adding implicit convertors, they will automatically be attempted when
 *  calling functions taking the native C++ class. If conversion is successful, the function is called.
 * 
 *  The conversion function must be of the following signature: `int f(PyObject* obj, ShadoweePtr<T>& val)`.
 *  It receives a pointer to the Python object for which the conversion must be attempted.
 *  If successful, you should construct a new instance on the heap of the native C++ class, assign it to the shadowee
 *  pointer, and return 0. If failed, you should return 1, and a generic `TypeError` will be set.
 * 
 *  Conversions are attempted in the order of registration.
 * 
 *  This macro must be invoked in the same translation unit (*.cpp file) as `PY_DECLARE_CLASS_*`.
 *
 *  @param t_ShadowObject Python class (fully qualified) on which to add the convertor
 *  @param f_conversionFunction conversion function
 *  @param i_uniqueName identifier used to name the generated registration hook
 *
 *  @par Example:
 *  
 *  ```cpp
 *  class Spam
 *  {
 *  public:
 *      Spam(const std::string& s);
 *  };
 * 
 *  int spamConversion(PyObject* obj, lass::python::ShadoweePtr<Spam>& spam)
 *  {
 *      std::string s;
 *      if (::lass::python::pyGetSimpleObject(obj, s) == 0)
 *      {
 *          spam.reset(new Spam(s));
 *          return 0; // conversion succeeded
 *      }
 *      return 1; // conversion failed
 *  }
 * 
 *  PY_SHADOW_CLASS(LASS_DLL_EXPORT, PySpam, Spam)
 *  PY_SHADOW_CASTERS(PySpam)
 *  PY_DECLARE_CLASS_NAME(PySpam, "Spam")
 *  PY_CLASS_CONSTRUCTOR_1(PySpam, const std::string&)
 *  PY_CLASS_CONVERTOR_EX(PySpam, spamConversion, spam)
 * 
 *  void func(const Spam& spam);
 *  PY_MODULE_CLASS(mod, PySpam)
 *  PY_MODULE_FUNCTION(mod, func)
 *  ```
 * 
 *  ```py
 *  mod.func(mod.Spam("abc")) # explicit conversion using constructor
 *  mod.func("abc")           # implicit conversion using convertor
 *  ```
 * 
 *  @sa PY_CLASS_CONVERTOR for adding auto-convertors using the constructor
 */
#define PY_CLASS_CONVERTOR_EX( t_ShadowObject, f_conversionFunction, i_uniqueName )\
	LASS_EXECUTE_BEFORE_MAIN_EX( LASS_CONCATENATE( lassPyClassConverter_, i_uniqueName ),\
		lass::python::impl::ShadowTraits< t_ShadowObject >::addConverter( f_conversionFunction );\
	)

/** @ingroup ShadowClasses
 *  @brief Add implicit auto-conversion to Python class
 * 
 *  Add an implicit convertor from @a t_sourceType to the Python class @a i_ShadowObject.
 *  It is attempted when a Python object that is not already an instance of that class is passed
 *  to a C++ function taking the C++ class as a parameter: the object is first converted to an
 *  instance of @a t_sourceType, from which a new instance of the C++ class is constructed.
 *  This assumes the C++ class has a matching constructor.
 * 
 *  Without it, you must convert the value explicitly in Python by constructing an instance of the
 *  Python class, or the C++ function must be overloaded on @a t_sourceType.
 * 
 *  Conversions are attempted in the order of registration.
 * 
 *  This macro must be invoked in the same translation unit (*.cpp file) as `PY_DECLARE_CLASS_*`.
 * 
 *  @param i_ShadowObject Python class identifier (unqualified) on which to add the auto-convertor
 *  @param t_sourceType type from which to convert. It must be default-constructible and be
 *                      exported to Python as a class or with `PyExportTraits`.
 * 
 *  @par Example:
 *  
 *  ```cpp
 *  class Spam
 *  {
 *  public:
 *      Spam(const std::string& s);
 *  };
 * 
 *  PY_SHADOW_CLASS(LASS_DLL_EXPORT, PySpam, Spam)
 *  PY_SHADOW_CASTERS(PySpam)
 *  PY_DECLARE_CLASS_NAME(PySpam, "Spam")
 *  PY_CLASS_CONSTRUCTOR_1(PySpam, const std::string&)
 *  PY_CLASS_CONVERTOR(PySpam, std::string)
 * 
 *  void func(const Spam& spam);
 *  PY_MODULE_CLASS(mod, PySpam)
 *  PY_MODULE_FUNCTION(mod, func)
 *  ```
 * 
 *  ```py
 *  mod.func(mod.Spam("abc")) # explicit conversion using constructor
 *  mod.func("abc")           # implicit conversion using convertor
 *  ```
 *
 *  @sa PY_CLASS_CONVERTOR_EX for adding convertors with custom conversion functions
 */
#define PY_CLASS_CONVERTOR( i_ShadowObject, t_sourceType )\
	PY_CLASS_CONVERTOR_EX( i_ShadowObject, (::lass::python::impl::defaultConvertor< i_ShadowObject, t_sourceType >) , i_ShadowObject );



#endif

// EOF
