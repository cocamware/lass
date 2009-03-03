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
	scNonConst,
};

/** @ingroup Python
 *  @internal
 */
class LASS_DLL ShadowBaseCommon: public PyObjectPlus
{
protected:
	ShadowBaseCommon();
	~ShadowBaseCommon();
private:
	ShadowBaseCommon(const ShadowBaseCommon& other);
	ShadowBaseCommon& operator=(const ShadowBaseCommon& other);	
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
			LASS_ASSERT(!TConstPointerTraits::isEmpty(value));
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
			PyErr_Format(PyExc_TypeError,"PyObject not castable to %s", T::_lassPyClassDef.name());
			return false;
		}
		return true;
	}

public:

	typedef typename TImpl::TCppClass TCppClass;
	typedef typename TImpl::TCppClassPtr TCppClassPtr;
	typedef typename TImpl::TConstCppClassPtr TConstCppClassPtr;
	typedef typename TImpl::TPyClassPtr TPyClassPtr;

	template <typename Ptr> static int getObject(PyObject* obj, Ptr& value)
	{
		return checkSubType(obj) ? TImpl::getObject(static_cast<T*>(obj), value) : 1;
	}
	template <typename Ptr> static TPyClassPtr buildObject(const Ptr& value)
	{
		return TImpl::buildObject(value);
	}
	static TPyClassPtr buildObject(const TCppClass& value)
	{
		TCppClassPtr p(new TCppClass(value));
		return TImpl::buildObject(p);
	}
};

template <typename ShadowType, typename DerivedMakers> 
typename ShadowType::TShadowPtr makeShadow(
		const typename ShadowType::TConstShadoweePtr& shadowee, const DerivedMakers* derivedMakers,
		impl::ShadoweeConstness constness)
{
	typedef typename ShadowType::TShadowPtr TShadowPtr;
	typedef typename ShadowType::TConstPointerTraits TConstPointerTraits;

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

}


template <typename T>
struct DefaultShadoweePointerTraits
{
	typedef util::SharedPtr<T> TPtr;
	static bool isEmpty(const TPtr& p) 
	{ 
		return p.isEmpty(); 
	}
	template <typename U> struct Rebind
	{
		typedef DefaultShadoweePointerTraits<U> Type;
	};
	template <typename U> static TPtr staticCast(const util::SharedPtr<U>& p)
	{
		return p.staticCast<T>();
	}
	template <typename U> static TPtr dynamicCast(const util::SharedPtr<U>& p)
	{
		return p.dynamicCast<T>();
	}
	template <typename U> static TPtr constCast(const util::SharedPtr<U>& p)
	{
		return p.constCast<T>();
	}
};



/** @ingroup Python
 */
template 
<
	typename ShadowType,
	typename ShadoweeType, 
	typename ParentShadowType,
	typename PointerTraits = DefaultShadoweePointerTraits<ShadoweeType>
>
class ShadowClass: public ParentShadowType
{
public:
	typedef ShadoweeType TShadowee;
	typedef ShadowType TShadow;
	typedef ParentShadowType TParentShadow;
	typedef typename PointerTraits::Rebind<ShadoweeType>::Type TPointerTraits;
	typedef typename PointerTraits::Rebind<const ShadoweeType>::Type TConstPointerTraits;
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
	typedef typename PointerTraits::Rebind<ShadoweeType>::Type TPointerTraits;
	typedef typename PointerTraits::Rebind<const ShadoweeType>::Type TConstPointerTraits;
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
	class dllInterface i_PyObjectShadowClass: \
		public ::lass::python::ShadowClass<i_PyObjectShadowClass, t_CppClass, t_PyObjectParent, t_PointerTraits> \
	{ \
		PY_HEADER(t_PyObjectParent) \
	public: \
		i_PyObjectShadowClass(const TConstShadoweePtr& shadowee, ::lass::python::impl::ShadoweeConstness constness): \
			::lass::python::ShadowClass<i_PyObjectShadowClass, t_CppClass, t_PyObjectParent, t_PointerTraits>(shadowee, constness) \
		{ \
		} \
	}; \
	/* this should be removed from here as it will be in headers! */ \
	LASS_EXECUTE_BEFORE_MAIN_EX( \
		LASS_CONCATENATE(lassPythonImplRegisterShadowWithParent_, i_PyObjectShadowClass), \
		i_PyObjectShadowClass::registerWithParent(); \
		)

/** @ingroup Python
 */
#define PY_SHADOW_CLASS_PTRTRAITS(dllInterface, i_PyObjectShadowClass, t_CppClass, t_PointerTraits)\
	PY_SHADOW_CLASS_EX(dllInterface, i_PyObjectShadowClass, t_CppClass, ::lass::python::PyObjectPlus, t_PointerTraits)

/** @ingroup Python
 */
#define PY_SHADOW_CLASS(dllInterface, i_PyObjectShadowClass, t_CppClass)\
	PY_SHADOW_CLASS_EX(dllInterface, i_PyObjectShadowClass, t_CppClass, ::lass::python::PyObjectPlus, ::lass::python::DefaultShadoweePointerTraits< t_CppClass >)

#define PY_SHADOW_CLASS_DERIVED(dllInterface, i_PyObjectShadowClass, t_CppClass, t_PyObjectShadowParent)\
	PY_SHADOW_CLASS_EX(dllInterface, i_PyObjectShadowClass, t_CppClass, t_PyObjectShadowParent, t_PyObjectShadowParent::TPointerTraits::Rebind<t_CppClass>::Type )

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


/** a macro that enables shadow classes to have conversion (coercion) operations defined on construction 
*	Say you have following situation:
*		class A 
*		{ 
*			A(int a) {} 
*		};
*		class B
*		{
*			B(const A& iA) {}
*		};
*	You may whish to create an object B with int as constructor argument, without having proper custom pyGetSimpleObject
*	this will not work unless you use the PY_CLASS_CONVERTOR macro to let the conversion routines know which possibilities 
*	may be tried before giving up and throwing a not-casteable exception.
*/
/*
#define PY_CLASS_CONSTRUCTOR_BYCOPY_IMPL( t_ShadowObject )\
inline int pyGetSimpleObject(PyObject* iValue, lass::util::SharedPtr<t_ShadowObject, lass::python::PyObjectStorage, lass::python::PyObjectCounter>& oV)\
{\
	const bool isNone = (iValue == Py_None );\
	if (isNone)\
		oV = lass::util::SharedPtr< t_ShadowObject, lass::python::PyObjectStorage, lass::python::PyObjectCounter>();\
	else\
	{\
		if (!PyType_IsSubtype(iValue->ob_type , & t_ShadowObject::_lassPyType ))\
		{\
			for (size_t i=0;i<lass::python::impl::ShadowTraits< t_ShadowObject >::byCopyGetters.size();++i)\
			{\
				lass::python::impl::ShadowTraits< t_ShadowObject >::TCppClass* newCopy = new lass::python::impl::ShadowTraits< t_ShadowObject >::TCppClass();\
				if (!lass::python::impl::ShadowTraits< t_ShadowObject >::byCopyGetters[i](iValue, *newCopy))\
				{\
					typedef lass::util::SharedPtr<t_ShadowObject, lass::python::PyObjectStorage, lass::python::PyObjectCounter> TSharedPtr;\
					TSharedPtr newOv(new t_ShadowObject (newCopy));\
					oV.swap(newOv);\
					PyErr_Clear();\
					return 0;\
				}\
			}\
			PyErr_Format(PyExc_TypeError,"not castable to %s", t_ShadowObject::_lassPyType.tp_name);\
			return 1;\
		}\
		oV = lass::python::fromNakedToSharedPtrCast< t_ShadowObject >(iValue);\
	}\
	return 0;\
}
*/

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

/*

The insert before main does not seem to work, probably initialization order problem, so we inject at runtime!

#define PY_CLASS_CONVERTOR_IMPL( t_ShadowObject, i_conversionFunction, i_dispatcher, i_typeOfConvertor )\
	namespace lass { namespace python { namespace impl {\
	void i_dispatcher()\
	{\
	impl::ShadowTraits< t_ShadowObject >::##i_typeOfConvertor##.push_back( i_conversionFunction);\
	}\
	LASS_EXECUTE_BEFORE_MAIN_EX( t_ShadowObjectConversionInjector, i_dispatcher(); )\
	} } }

#define PY_CLASS_CONVERTOR_BYCOPY( t_ShadowObject, i_conversionFunction )\
	PY_CLASS_CONVERTOR_IMPL( t_ShadowObject, i_conversionFunction , LASS_UNIQUENAME(insertConversion), byCopyGetters )

#define PY_CLASS_CONVERTOR_BYBORROW( t_ShadowObject, i_conversionFunction )\
	PY_CLASS_CONVERTOR_IMPL( t_ShadowObject, i_conversionFunction , LASS_UNIQUENAME(insertConversion), byBorrowGetters )
*/

#define PY_CLASS_CONVERTOR( t_ShadowObject,  i_conversionFunction ) \
	lass::python::impl::ShadowTraits< t_ShadowObject >::byCopyGetters.push_back( & i_conversionFunction );



#endif

// EOF


