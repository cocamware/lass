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

namespace lass
{
namespace python
{

namespace impl
{

enum ShadoweeConstness
{
	scConst,
	scNonConst
};

/** @ingroup Python
 *  @internal
 */
class LASS_PYTHON_DLL ShadowBaseCommon: public PyObjectPlus
{
protected:
	ShadowBaseCommon();
	~ShadowBaseCommon();
private:
	ShadowBaseCommon(const ShadowBaseCommon&);
	ShadowBaseCommon& operator=(const ShadowBaseCommon&);	
};

/** @ingroup Python
 *  @internal
 */
template <typename T>
struct IsShadowClass: public meta::IsDerived<T, ShadowBaseCommon> 
{
};

/** @ingroup Python
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

template <typename ShadowType, typename DerivedMakers> 
typename ShadowType::TShadowPtr makeShadow(
		const typename ShadowType::TConstShadoweePtr& shadowee, const DerivedMakers* derivedMakers,
		impl::ShadoweeConstness constness)
{
	typedef typename ShadowType::TShadowPtr TShadowPtr;
	typedef typename ShadowType::TConstPointerTraits LASS_UNUSED(TConstPointerTraits);

	LASS_ASSERT(!TConstPointerTraits::isEmpty(shadowee));
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

template <typename Makers, typename Maker> void registerMaker(Makers*& makers, Maker maker)
{
	if (!makers)
	{
		makers = new Makers;
	}
	makers->push_back(maker);
}

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


template <typename T, template <typename, typename> class S = util::ObjectStorage, typename C = util::DefaultCounter>
struct SharedPointerTraits
{
	typedef util::SharedPtr<T, S, C> TPtr;
	template <typename U> struct Rebind
	{
		typedef SharedPointerTraits<U, S, C> Type;
	};
	static void acquire(const TPtr&) {} // TPtr already handles ownership, so nothing to acquire.
	static void release(const TPtr&) {}
	static bool isEmpty(const TPtr& p) 
	{ 
		return p.isEmpty(); 
	}
	static T* get(const TPtr& p) 
	{ 
		return p.get(); 
	}
	template <typename U> static TPtr staticCast(const util::SharedPtr<U, S, C>& p)
	{
		return p.template staticCast<T>();
	}
	template <typename U> static TPtr dynamicCast(const util::SharedPtr<U, S, C>& p)
	{
		return p.template dynamicCast<T>();
	}
	template <typename U> static TPtr constCast(const util::SharedPtr<U, S, C>& p)
	{
		return p.template constCast<T>();
	}
};


template <typename T>
struct NakedPointerTraits
{
	typedef T* TPtr;
	template <typename U> struct Rebind
	{
		typedef NakedPointerTraits<U> Type;
	};
	static void acquire(TPtr) {} // no ownership rules, so nothing to acquire.
	static void release(TPtr) {}
	static bool isEmpty(TPtr p) 
	{ 
		return p == 0; 
	}
	static T* get(TPtr p) 
	{ 
		return p; 
	}
	template <typename U> static TPtr staticCast(U* p)
	{
		return static_cast<TPtr>(p);
	}
	template <typename U> static TPtr dynamicCast(U* p)
	{
		return dynamic_cast<TPtr>(p);
	}
	template <typename U> static TPtr constCast(U* p)
	{
		return const_cast<TPtr>(p);
	}
};


template <typename T>
struct StdSharedPointerTraits
{
	typedef std::shared_ptr<T> TPtr;
	template <typename U> struct Rebind
	{
		typedef StdSharedPointerTraits<U> Type;
	};
	static void acquire(const TPtr&) {} // TPtr already handles ownership, so nothing to acquire.
	static void release(const TPtr&) {}
	static bool isEmpty(const TPtr& p)
	{
		return !p;
	}
	static T* get(const TPtr& p)
	{
		return p.get();
	}
	template <typename U> static TPtr staticCast(const std::shared_ptr<U>& p)
	{
		return std::static_pointer_cast<T>(p);
	}
	template <typename U> static TPtr dynamicCast(const std::shared_ptr<U>& p)
	{
		return std::dynamic_pointer_cast<T>(p);
	}
	template <typename U> static TPtr constCast(const std::shared_ptr<U>& p)
	{
		return std::const_pointer_cast<T>(p);
	}
};


/** @ingroup Python
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
	}
	~ShadowClass()
	{
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

}

}

/** @ingroup Python
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

/** @ingroup Python
 */
#define PY_SHADOW_CLASS_PTRTRAITS(dllInterface, i_PyObjectShadowClass, t_CppClass, pointerTraits)\
	PY_SHADOW_CLASS_EX(dllInterface, i_PyObjectShadowClass, t_CppClass, ::lass::python::PyObjectPlus, pointerTraits < t_CppClass > )

/** @ingroup Python
 */
#define PY_SHADOW_CLASS(dllInterface, i_PyObjectShadowClass, t_CppClass)\
	PY_SHADOW_CLASS_EX(dllInterface, i_PyObjectShadowClass, t_CppClass, ::lass::python::PyObjectPlus, ::lass::python::SharedPointerTraits< t_CppClass >)

#define PY_SHADOW_CLASS_DERIVED(dllInterface, i_PyObjectShadowClass, t_CppClass, t_PyObjectShadowParent)\
	PY_SHADOW_CLASS_EX(dllInterface, i_PyObjectShadowClass, t_CppClass, t_PyObjectShadowParent, t_PyObjectShadowParent::TPointerTraits::Rebind< t_CppClass >::Type )

/** @ingroup Python
 *  @deprecated 
 */
#define PY_SHADOW_CLASS_NOCONSTRUCTOR_EX(dllInterface, i_PyObjectShadowClass, t_CppClass, t_PyObjectBase, t_PyObjectParent)\
	PY_SHADOW_CLASS_EX(dllInterface, i_PyObjectShadowClass, t_CppClass, t_PyObjectBase, t_PyObjectParent)

/** @ingroup Python
 *  @deprecated 
 */
#define PY_SHADOW_CLASS_NOCONSTRUCTOR(dllInterface, i_PyObjectShadowClass, t_CppClass)\
	PY_SHADOW_CLASS(dllInterface, i_PyObjectShadowClass, t_CppClass)

/** @ingroup Python
 *  @deprecated 
 */
#define PY_WEAK_SHADOW_CLASS(dllInterface, i_PyObjectShadowClass, t_CppClass)\
	PY_SHADOW_CLASS(dllInterface, i_PyObjectShadowClass, t_CppClass)

/** @ingroup Python
 *  @deprecated 
 */
#define PY_WEAK_SHADOW_CLASS_NOCONSTRUCTOR(dllInterface, i_PyObjectShadowClass, t_CppClass)\
	PY_SHADOW_CLASS(dllInterface, i_PyObjectShadowClass, t_CppClass)

/** @ingroup Python
 *  @deprecated 
 */
#define PY_SHADOW_CLASS_ENABLE_AUTOMATIC_INVALIDATION(i_PyObjectShadowClass)

/** @ingroup Python
 *  @deprecated 
 */
#define PY_SHADOW_CLASS_DERIVED_NOCONSTRUCTOR(dllInterface, i_PyObjectShadowClass, t_CppClass, t_PyObjectShadowParent)\
	PY_SHADOW_CLASS_DERIVED(dllInterface, i_PyObjectShadowClass, t_CppClass, t_PyObjectShadowParent)




/** @ingroup Python
 */
#define PY_SHADOW_CASTERS(t_ShadowObject)\
namespace lass \
{ \
namespace python \
{ \
	template <> struct ShadoweeTraits< t_ShadowObject::TShadowee >: meta::True \
	{ \
		typedef t_ShadowObject TShadow; \
		/*typedef impl::ShadowTraits< t_ShadowObject > TShadowTraits;*/ \
		typedef t_ShadowObject::TPointerTraits TPointerTraits; \
	}; \
} \
} \
/**/



/** @ingroup Python
 *  @deprecated 
 */
#define PY_SHADOW_DOWN_CASTERS(t_ShadowObject)\
	PY_SHADOW_CASTERS(t_ShadowObject)

/** @ingroup Python
 *  @deprecated 
 */
#define PY_SHADOW_DOWN_CASTERS_NOCONSTRUCTOR(t_ShadowObject)\
	PY_SHADOW_CASTERS(t_ShadowObject)




#define PY_CLASS_CONVERTOR_EX( t_ShadowObject, v_conversionFunction, i_uniqueName )\
	LASS_EXECUTE_BEFORE_MAIN_EX( LASS_CONCATENATE( lassPyClassConverter_, i_uniqueName ),\
		lass::python::impl::ShadowTraits< t_ShadowObject >::addConverter( v_conversionFunction );\
	)

#define PY_CLASS_CONVERTOR( i_ShadowObject, t_sourceType )\
	PY_CLASS_CONVERTOR_EX( i_ShadowObject, (::lass::python::impl::defaultConvertor< i_ShadowObject, t_sourceType >) , i_ShadowObject );



#endif

// EOF


