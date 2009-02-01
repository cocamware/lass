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

/** @ingroup Python
 *  @internal
 */
class ConstError: public util::experimental::ExceptionMixin<ConstError>
{
public:
	ConstError(const std::string& msg, const std::string& loc): 
		util::experimental::ExceptionMixin<ConstError>(msg, loc) {}
};

/** @ingroup Python
 *  @internal
 */
enum ShadowOwnership
{
	soBorrowed,
	soOwner
};

/** @ingroup Python
 *  @internal
 */
enum ShadowConstness
{
	scConst,
	scNonConst
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
template <class ShadoweeType>
class ShadowBase: public ShadowBaseCommon
{
public:
	typedef ShadoweeType TShadowee;

	virtual ~ShadowBase()
	{
		if (ownership_ == soOwner)
		{
			delete shadowee_;
		}
	}

	TShadowee* cppObject() const
	{
		if (constness_ == scConst)
		{
			LASS_THROW_EX(ConstError, "Can't get write access through a const object pointer");
		}
		return shadowee_;
	}
	const TShadowee* constCppObject() const
	{
		return shadowee_;
	}

protected:
	typedef PyObjectPlus* (*TDerivedMaker)(TShadowee*, ShadowOwnership, ShadowConstness);

	ShadowBase(TShadowee* shadowee, ShadowOwnership ownership, ShadowConstness constness): 
		shadowee_(shadowee), ownership_(ownership), constness_(constness)
	{
	}

	static void registerDerivedMaker(TDerivedMaker derivedMaker)
	{
	}

private:
	TShadowee* shadowee_;
	ShadowOwnership ownership_;
	ShadowConstness constness_;
};

/** @ingroup Python
 *  @internal
 */
template <typename ShadoweeType, typename ParentShadowType> 
struct ShadowBaseSelector 
{ 
	typedef ParentShadowType Type;
};

/** @ingroup Python
 *  @internal
 */
template <typename ShadoweeType> 
struct ShadowBaseSelector<ShadoweeType, PyObjectPlus> 
{ 
	typedef ShadowBase<ShadoweeType> Type;
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
private:

	template <typename U, bool shadow>
	struct Impl
	{
		typedef typename U::TShadowee TCppClass;
		static TCppClass* cppObject(U* pyObject)
		{
			return static_cast<TCppClass*>(pyObject->cppObject());
		};
		// this does nothing special in the general case, for specific use we
		// can override this function to only allow const calls on const shadowed objects
		static const TCppClass* constCppObject(U* pyObject)
		{
			return static_cast<const TCppClass*>(pyObject->constCppObject());
		};
		static U* pyObject(TCppClass* cppObject)
		{
			return U::make(cppObject);
		}
	};

	template <typename U>
	struct Impl<U, false>
	{
		typedef U TCppClass;
		static U* cppObject(U* pyObject)
		{
			return pyObject;
		};
		static const U* constCppObject(U* pyObject)
		{
			return pyObject;
		};
		static U* pyObject(U* cppObject)
		{
			return cppObject;
		}
	};

public:

	enum { isShadow = IsShadowClass<T>::value };
	typedef typename Impl<T, isShadow>::TCppClass TCppClass;

	typedef int (*TPyGetSimpleObjectByCopy)( PyObject* iObject, TCppClass& oByCopy );
	typedef int (*TPyGetSimpleObjectByBorrow)( PyObject* iObject, TCppClass*& oByBorrow );

	static std::vector<TPyGetSimpleObjectByCopy> byCopyGetters;
	static std::vector<TPyGetSimpleObjectByBorrow> byBorrowGetters;

	static TCppClass* cppObject(PyObject* iPyObject)
	{
		if (!PyType_IsSubtype(iPyObject->ob_type , &T::_lassPyType ))
		{
			PyErr_Format(PyExc_TypeError,"PyObject not castable to %s", T::_lassPyType.tp_name);
			return 0;
		}
		try
		{
			return Impl<T, isShadow>::cppObject(static_cast<T*>(iPyObject));
		}
		catch (ConstError&)
		{
			PyErr_SetString(PyExc_TypeError, "PyObject not writable");
			return 0;
		}
	};
	static const TCppClass* constCppObject(PyObject* iPyObject)
	{
		if (!PyType_IsSubtype(iPyObject->ob_type , &T::_lassPyType ))
		{
			PyErr_Format(PyExc_TypeError,"PyObject not castable to %s", T::_lassPyType.tp_name);
			return 0;
		}
		return Impl<T, isShadow>::constCppObject(static_cast<T*>(iPyObject));
	};
	static T* pyObject(TCppClass* iCppObject)
	{
		return Impl<T, isShadow>::pyObject(iCppObject);
	}
};

template<typename T> std::vector<typename ShadowTraits<T>::TPyGetSimpleObjectByCopy> ShadowTraits<T>::byCopyGetters;
template<typename T> std::vector<typename ShadowTraits<T>::TPyGetSimpleObjectByBorrow> ShadowTraits<T>::byBorrowGetters;

}

/** @ingroup Python
 *	@internal
 */
template 
<
	typename ShadowType,
	typename ShadoweeType, 
	typename ParentShadowType = PyObjectPlus
>
class ShadowClass: public impl::ShadowBaseSelector<ShadoweeType, ParentShadowType>::Type
{
	typedef typename impl::ShadowBaseSelector<ShadoweeType, ParentShadowType>::Type TBase;
public:
	typedef ShadowType TShadow;
	typedef ShadoweeType TShadowee;
	typedef ParentShadowType TParentShadow;
	typedef typename TBase::TShadowee TParentShadowee;

	static TShadow* make(TShadowee* shadowee)
	{
		return ShadowClass::make(shadowee, impl::soBorrowed, impl::scNonConst);
	}
	static TShadow* make(const TShadowee* shadowee)
	{
		return ShadowClass::make(const_cast<TShadowee*>(shadowee), impl::soBorrowed, impl::scConst);
	}
	static TShadow* make(std::auto_ptr<TShadowee>& shadowee)
	{
		TShadow* p = ShadowClass::make(shadowee.get(), impl::soOwner, impl::scNonConst);
		shadowee.release();
		return p;
	}
	static void registerWithParent()
	{
		TBase::registerDerivedMaker(ShadowClass::makeParent);
	}

protected:
	typedef TShadow* (*TDerivedMaker)(TShadowee*, impl::ShadowOwnership, impl::ShadowConstness);

	ShadowClass(TShadowee* shadowee, impl::ShadowOwnership ownership, impl::ShadowConstness constness):
		TBase(shadowee, ownership, constness)
	{
	}

	static void registerDerivedMaker(TDerivedMaker derivedMaker)
	{
		if (!derivedMakers_)
		{
			derivedMakers_ = new TDerivedMakers;
		}
		derivedMakers_->push_back(derivedMaker);
	}

private:
	typedef std::vector<TDerivedMaker> TDerivedMakers;

	static TShadow* make(TShadowee* shadowee, impl::ShadowOwnership ownership, impl::ShadowConstness constness)
	{
		if (derivedMakers_)
		{
			for (typename TDerivedMakers::const_iterator i = derivedMakers_->begin(); i != derivedMakers_->end(); ++i)
			{
				if(TShadow* p = (*i)(shadowee, ownership, constness))
				{
					return p;
				}
			}
		}
		return impl::fixObjectType(new TShadow(shadowee, ownership, constness));
	}
	static TParentShadow* makeParent(TParentShadowee* shadowee, impl::ShadowOwnership ownership, impl::ShadowConstness constness)
	{
		if (TShadowee* p = dynamic_cast<TShadowee*>(shadowee))
		{
			return ShadowClass::make(p, ownership, constness);
		}
		return 0;
	}

	static TDerivedMakers* derivedMakers_;
};

template <typename S, typename T, typename P> typename ShadowClass<S, T, P>::TDerivedMakers* ShadowClass<S, T, P>::derivedMakers_ = 0;

}

}

/** @ingroup Python
 */
#define PY_SHADOW_CLASS_EX(dllInterface, i_PyObjectShadowClass, t_CppClass, t_PyObjectBase, t_PyObjectParent) \
	class dllInterface i_PyObjectShadowClass: public ::lass::python::ShadowClass<i_PyObjectShadowClass, t_CppClass, t_PyObjectParent> \
	{ \
		PY_HEADER(t_PyObjectParent) \
	public: \
		i_PyObjectShadowClass(TShadowee* shadowee, ::lass::python::impl::ShadowOwnership ownership, ::lass::python::impl::ShadowConstness constness): \
			::lass::python::ShadowClass<i_PyObjectShadowClass, t_CppClass, t_PyObjectParent>(shadowee, ownership, constness) \
		{ \
		} \
	}; \
	LASS_EXECUTE_BEFORE_MAIN_EX( \
		LASS_CONCATENATE(lassPythonImplRegisterShadowWithParent_, i_PyObjectShadowClass), \
		i_PyObjectShadowClass::registerWithParent(); \
		)

/** @ingroup Python
 */
#define PY_SHADOW_CLASS(dllInterface, i_PyObjectShadowClass, t_CppClass)\
	PY_SHADOW_CLASS_EX(dllInterface, i_PyObjectShadowClass, t_CppClass, ::lass::python::PyObjectPlus, ::lass::python::PyObjectPlus)

#define PY_SHADOW_CLASS_DERIVED(dllInterface, i_PyObjectShadowClass, t_CppClass, t_PyObjectShadowParent)\
	PY_SHADOW_CLASS_EX(dllInterface, i_PyObjectShadowClass, t_CppClass, t_PyObjectShadowParent, t_PyObjectShadowParent)

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


