/** @file
 *  @author Bram de Greve (bramz@users.sourceforge.net)
 *  @author Tom De Muer (tomdemuer@users.sourceforge.net)
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

#include "pyobject_plus.h"
#include "../meta/is_derived.h"
#include <set>

namespace lass
{
namespace python
{

namespace impl
{

class LASS_DLL PyShadowBaseCommon: public PyObjectPlus
{
protected:
	PyShadowBaseCommon();
	PyShadowBaseCommon(const PyShadowBaseCommon& iOther);
	~PyShadowBaseCommon();
	PyShadowBaseCommon& operator=(const PyShadowBaseCommon& iOther);	
};

template <class CppBase>
class PyShadowBaseTracked : public PyShadowBaseCommon
{
public:
	PyShadowBaseTracked(CppBase* iObject=0, bool iTrack=false) : cppObject_(iObject)
	{
		if (iTrack)
			PyShadowBaseTracked<CppBase>::trackShadow(this);
	}
	virtual ~PyShadowBaseTracked()
	{
		PyShadowBaseTracked<CppBase>::forgetShadow(this);
	}
	class AutomaticObjectInvalidator
	{
	public:
		AutomaticObjectInvalidator() {}
		virtual ~AutomaticObjectInvalidator()
		{
			PyShadowBaseTracked<CppBase>::invalidateBase(static_cast<CppBase*>(this));
		}
	};
	static std::set<PyShadowBaseTracked<CppBase>*>* shadows_;
	static bool invalidateBase(CppBase* iBase)
	{
		if (!shadows_)
			return true;
		typename std::set<PyShadowBaseTracked<CppBase>*>::iterator it = shadows_->begin();
		for (;it!=shadows_->end();++it)
		{
			if ((*it)->cppObject_==iBase)
			{
				(*it)->cppObject_ = NULL;		// invalidate
				Py_INCREF(Py_None);
				*(static_cast<PyObject*>(*it)) = *Py_None;
			}
		}
		return true;
	}
	static bool trackShadow(PyShadowBaseTracked<CppBase>* iShadow)
	{
		if (!shadows_)
			shadows_ = new std::set<PyShadowBaseTracked<CppBase>*>;
		if (shadows_)
			shadows_->insert(iShadow);
		return true;
	}
	static bool forgetShadow(PyShadowBaseTracked<CppBase>* iShadow)
	{
		if (shadows_)
			shadows_->erase(iShadow);
		return true;
	}
protected:
	CppBase* cppObject_;
};

template <class CppBase> std::set<PyShadowBaseTracked<CppBase>*>* PyShadowBaseTracked<CppBase>::shadows_;


template <class CppBase, bool weak = false>
class PyShadowBase: public PyShadowBaseTracked<CppBase>
{
public:
	virtual ~PyShadowBase()
	{
		if (!weak && this->ownership_ == oOwner) 
		{
			delete this->cppObject_; 
			this->cppObject_ = 0;
		}
	}
	CppBase* cppObject() const
	{
		return this->cppObject_;
	}
	const CppBase* constCppObject() const
	{
		return this->cppObject_;
	}
	static bool doTracking;

protected:
	enum Ownership
	{
		oOwner,
		oBorrowed
	};
	PyShadowBase(CppBase* iCppObject, Ownership iOwnership):
		PyShadowBaseTracked<CppBase>(iCppObject,doTracking),
		ownership_(iOwnership)
	{
		this->cppObject_ = iCppObject;
	}
private:
	Ownership ownership_;
};
template <class CppBase, bool weak> bool PyShadowBase<CppBase,weak>::doTracking = false;
}

template <typename CppBase>
struct ShadowObjectInvalidator
{
	typedef typename impl::PyShadowBaseTracked<CppBase>::AutomaticObjectInvalidator Type;
};

template<typename CppBase>
void invalidateShadowingObject(CppBase* iPointerReferenceToInvalidate)
{
	impl::PyShadowBaseTracked<CppBase>::invalidateBase(iPointerReferenceToInvalidate);
}

namespace impl
{

/** a weak shadow class NEVER EVER owns the object, USE AT YOUR OWN RISK!  
*	Consult your local Lass::Python guru to gain insight when the use of this
*	class is appropriate.  A rule of thumb is that any properly designed 
*	C++ interface should never be exposed using weak shadow objects.
*   For your own safety, use weak shadow objects always in conjunction with the
*	automatic object invalidator.  This at least assures that when you access 
*	a weak shadow object from within Python that you don't get a dereference of 
*	a dangling pointer.  You will notice that in Python a "C++ deleted" weak
*	shadow object is transformed into None.
 */
template <class CppBase>
struct PyShadowBaseWeak
{
	typedef PyShadowBase<CppBase,true> Type;
};

template <typename T>
struct IsShadowClass: public meta::IsDerived<T, PyShadowBaseCommon> 
{
};



template <typename T>
struct ShadowTraits
{
private:

	template <typename U, bool shadow>
	struct Impl
	{
		typedef typename U::TCppClass TCppClass;
		static TCppClass* cppObject(U* iPyObject)
		{
			return static_cast<TCppClass*>(iPyObject->cppObject());
		};
		// this does nothing special in the general case, for specific use we
		// can override this function to only allow const calls on const shadowed objects
		static const TCppClass* constCppObject(U* iPyObject)
		{
			return static_cast<const TCppClass*>(iPyObject->constCppObject());
		};
		static U* pyObject(TCppClass* iCppObject)
		{
			std::auto_ptr<TCppClass> cppObject(iCppObject);
			return new U(cppObject);
		}
	};

	template <typename U>
	struct Impl<U, false>
	{
		typedef U TCppClass;
		static U* cppObject(U* iPyObject)
		{
			return iPyObject;
		};
		static const U* constCppObject(U* iPyObject)
		{
			return const_cast<U*>(iPyObject);
		};
		static U* pyObject(U* iCppObject)
		{
			return iCppObject;
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
		return Impl<T, isShadow>::cppObject(static_cast<T*>(iPyObject));
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

}

}

#define PY_SHADOW_CLASS_EX(dllInterface, i_PyObjectShadowClass, t_CppClass, t_PyObjectBase, t_PyObjectParent)\
	class dllInterface i_PyObjectShadowClass: public t_PyObjectBase\
	{\
		PY_HEADER(t_PyObjectParent)\
	public:\
		typedef t_CppClass TCppClass;\
		typedef t_PyObjectParent TPyParentClass;\
		i_PyObjectShadowClass(const TCppClass& iByCopy):\
			t_PyObjectBase(new TCppClass(iByCopy), oOwner) {}\
		i_PyObjectShadowClass(TCppClass* iByBorrowedPointer):\
			t_PyObjectBase(iByBorrowedPointer, oBorrowed) {}\
		i_PyObjectShadowClass(std::auto_ptr<TCppClass> iBySinkedPointer):\
			t_PyObjectBase(iBySinkedPointer.get(), oOwner) { iBySinkedPointer.release(); }\
	\
		typedef PyObject* (*TConstructorChainMethod)(TCppClass*);\
		static std::vector<TConstructorChainMethod>& constructors()\
		{\
			static std::vector<TConstructorChainMethod> constructors;\
			return constructors;\
		}\
		static PyObject* pyBuildSimpleObject_fromPtr(TCppClass* iPtr)\
		{\
			for (size_t i=0;i<constructors().size();++i)\
			{\
				PyObject* tempObj = constructors()[i](iPtr);\
				if ( (tempObj!=0) && (tempObj!=Py_None))\
					return tempObj;\
			}\
			return ::lass::python::impl::fixObjectType(new i_PyObjectShadowClass( iPtr ));\
		}\
	protected:\
		i_PyObjectShadowClass(TCppClass* iValue, Ownership iOwnership):\
			t_PyObjectBase(iValue, iOwnership) {}\
	};\

#define PY_SHADOW_CLASS_NOCONSTRUCTOR_EX(dllInterface, i_PyObjectShadowClass, t_CppClass, t_PyObjectBase, t_PyObjectParent)\
	class dllInterface i_PyObjectShadowClass: public t_PyObjectBase\
	{\
		PY_HEADER(t_PyObjectParent)\
	public:\
		typedef t_CppClass TCppClass;\
		typedef t_PyObjectParent TPyParentClass;\
		i_PyObjectShadowClass(TCppClass* iByBorrowedPointer):\
			t_PyObjectBase(iByBorrowedPointer, oBorrowed) {}\
		i_PyObjectShadowClass(std::auto_ptr<TCppClass> iBySinkedPointer):\
			t_PyObjectBase(iBySinkedPointer.get(), oOwner) { iBySinkedPointer.release(); }\
	\
		typedef PyObject* (*TConstructorChainMethod)(TCppClass*);\
		static std::vector<TConstructorChainMethod>& constructors()\
		{\
			static std::vector<TConstructorChainMethod> constructors;\
			return constructors;\
		}\
		static PyObject* pyBuildSimpleObject_fromPtr(TCppClass* iPtr)\
		{\
			for (size_t i=0;i<constructors().size();++i)\
			{\
				PyObject* tempObj = constructors()[i](iPtr);\
				if ( (tempObj!=0) && (tempObj!=Py_None))\
					return tempObj;\
			}\
			return ::lass::python::impl::fixObjectType(new i_PyObjectShadowClass( iPtr ));\
		}\
	protected:\
		i_PyObjectShadowClass(TCppClass* iValue, Ownership iOwnership):\
			t_PyObjectBase(iValue, iOwnership) {}\
	};\


#define PY_SHADOW_CLASS(dllInterface, i_PyObjectShadowClass, t_CppClass)\
	PY_SHADOW_CLASS_EX(\
		dllInterface, i_PyObjectShadowClass, t_CppClass, lass::python::impl::PyShadowBase< t_CppClass >,\
		lass::python::PyObjectPlus)

#define PY_SHADOW_CLASS_NOCONSTRUCTOR(dllInterface, i_PyObjectShadowClass, t_CppClass)\
	PY_SHADOW_CLASS_NOCONSTRUCTOR_EX(\
		dllInterface, i_PyObjectShadowClass, t_CppClass, lass::python::impl::PyShadowBase< t_CppClass >,\
		lass::python::PyObjectPlus)


/** a weak shadow class NEVER EVER owns the object, USE AT YOUR OWN RISK!  
*	Consult your local Lass::Python guru to gain insight when the use of this
*	class is appropriate.  A rule of thumb is that any properly designed 
*	C++ interface should never be exposed using weak shadow objects.
 */
#define PY_WEAK_SHADOW_CLASS(dllInterface, i_PyObjectShadowClass, t_CppClass)\
	PY_SHADOW_CLASS_EX(\
	dllInterface, i_PyObjectShadowClass, t_CppClass, lass::python::impl::PyShadowBaseWeak< t_CppClass >::Type ,\
		lass::python::PyObjectPlus)

#define PY_WEAK_SHADOW_CLASS_NOCONSTRUCTOR(dllInterface, i_PyObjectShadowClass, t_CppClass)\
	PY_SHADOW_CLASS_NOCONSTRUCTOR_EX(\
	dllInterface, i_PyObjectShadowClass, t_CppClass, lass::python::impl::PyShadowBaseWeak< t_CppClass >::Type ,\
		lass::python::PyObjectPlus)


/** a macro to enable automatic invalidation for a given shadow class */
#define PY_SHADOW_CLASS_ENABLE_AUTOMATIC_INVALIDATION(i_PyObjectShadowClass)\
	i_PyObjectShadowClass::doTracking = true;

#define PY_SHADOW_CLASS_DERIVED(dllInterface, i_PyObjectShadowClass, t_CppClass, t_PyObjectShadowParent)\
	PY_SHADOW_CLASS_EX(\
		dllInterface, i_PyObjectShadowClass, t_CppClass, t_PyObjectShadowParent, t_PyObjectShadowParent)

#define PY_SHADOW_CLASS_DERIVED_NOCONSTRUCTOR(dllInterface, i_PyObjectShadowClass, t_CppClass, t_PyObjectShadowParent)\
	PY_SHADOW_CLASS_NOCONSTRUCTOR_EX(\
		dllInterface, i_PyObjectShadowClass, t_CppClass, t_PyObjectShadowParent, t_PyObjectShadowParent)


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
#pragma LASS_TODO("Check if the convertor macro's don't leak any memory")

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


#define PY_SHADOW_CASTERS(t_ShadowObject)\
namespace lass\
{\
namespace python\
{\
namespace impl\
{\
template <> struct ArgumentTraits< t_ShadowObject::TCppClass >\
{\
	typedef t_ShadowObject::TCppClass TCppClass;\
	typedef PyObjectPtr<t_ShadowObject>::Type TStorage;\
	static const TCppClass& arg(const TStorage& iArg)\
	{\
		return *static_cast<TCppClass*>(iArg->cppObject());\
	}\
};\
template <> struct ArgumentTraits< const t_ShadowObject::TCppClass >\
{\
	typedef t_ShadowObject::TCppClass TCppClass;\
	typedef PyObjectPtr<t_ShadowObject>::Type TStorage;\
	static const TCppClass& arg(const TStorage& iArg)\
	{\
		return *static_cast<TCppClass*>(iArg->cppObject());\
	}\
};\
template <> struct ArgumentTraits< t_ShadowObject::TCppClass& >\
{\
	typedef t_ShadowObject::TCppClass TCppClass;\
	typedef PyObjectPtr<t_ShadowObject>::Type TStorage;\
	static TCppClass& arg(const TStorage& iArg)\
	{\
		return *static_cast<TCppClass*>(iArg->cppObject());\
	}\
};\
template <> struct ArgumentTraits< const t_ShadowObject::TCppClass& >\
{\
	typedef t_ShadowObject::TCppClass TCppClass;\
	typedef PyObjectPtr<t_ShadowObject>::Type TStorage;\
	static const TCppClass& arg(const TStorage& iArg)\
	{\
		return *static_cast<TCppClass*>(iArg->cppObject());\
	}\
};\
template <> struct ArgumentTraits< t_ShadowObject::TCppClass* >\
{\
	typedef t_ShadowObject::TCppClass TCppClass;\
	typedef PyObjectPtr<t_ShadowObject>::Type TStorage;\
	static TCppClass* arg(const TStorage& iArg)\
	{\
		return static_cast<TCppClass*>(iArg->cppObject());\
	}\
};\
template <> struct ArgumentTraits< const t_ShadowObject::TCppClass* >\
{\
	typedef t_ShadowObject::TCppClass TCppClass;\
	typedef PyObjectPtr<t_ShadowObject>::Type TStorage;\
	static const TCppClass* arg(const TStorage& iArg)\
	{\
		return static_cast<TCppClass*>(iArg->cppObject());\
	}\
};\
template <> struct ArgumentTraits< t_ShadowObject::TCppClass* const >\
{\
	typedef t_ShadowObject::TCppClass TCppClass;\
	typedef PyObjectPtr<t_ShadowObject>::Type TStorage;\
	static TCppClass* const arg(const TStorage& iArg)\
	{\
		return static_cast<TCppClass*>(iArg->cppObject());\
	}\
};\
template <> struct ArgumentTraits< const t_ShadowObject::TCppClass* const >\
{\
	typedef t_ShadowObject::TCppClass TCppClass;\
	typedef PyObjectPtr<t_ShadowObject>::Type TStorage;\
	static const TCppClass* const arg(const TStorage& iArg)\
	{\
		return static_cast<TCppClass*>(iArg->cppObject());\
	}\
};\
}\
inline PyObject* pyBuildSimpleObject_deprecated( const t_ShadowObject::TCppClass& iByCopy )\
{\
	return ::lass::python::impl::fixObjectType(new t_ShadowObject( iByCopy ));\
}\
inline PyObject* pyBuildSimpleObject_deprecated( t_ShadowObject::TCppClass* iByBorrowedPointer )\
{\
	return ::lass::python::impl::fixObjectType(new t_ShadowObject( iByBorrowedPointer ));\
}\
inline PyObject* pyBuildSimpleObject_deprecated( t_ShadowObject::TCppClass& iByBorrowedPointer )\
{\
	return pyBuildSimpleObject_deprecated(&iByBorrowedPointer);\
}\
inline PyObject* pyBuildSimpleObject_deprecated( std::auto_ptr< t_ShadowObject::TCppClass > iBySinkedPointer )\
{\
	return ::lass::python::impl::fixObjectType(new t_ShadowObject( iBySinkedPointer ));\
}\
inline int pyGetSimpleObject_deprecated( PyObject* iObject, t_ShadowObject::TCppClass& oByCopy )\
{\
	if (t_ShadowObject::TCppClass* cppObject = \
		impl::ShadowTraits< t_ShadowObject >::cppObject(iObject))\
	{\
		oByCopy = *cppObject;\
		return 0;\
	}\
	return 1;\
}\
inline int pyGetSimpleObject_deprecated( PyObject* iObject, t_ShadowObject::TCppClass*& oByBorrowedPointer )\
{\
	if (t_ShadowObject::TCppClass* cppObject = \
		impl::ShadowTraits< t_ShadowObject >::cppObject(iObject))\
	{\
		oByBorrowedPointer = cppObject;\
		return 0;\
	}\
	return 1;\
}\
template <> \
struct PyExportTraits< t_ShadowObject::TCppClass > \
{\
	static PyObject* build(const t_ShadowObject::TCppClass& iByCopy) { return pyBuildSimpleObject_deprecated(iByCopy); }\
	static int get(PyObject* iV, t_ShadowObject::TCppClass& oV) { return pyGetSimpleObject_deprecated(iV,oV); }\
};\
template <> \
struct PyExportTraits< std::auto_ptr< t_ShadowObject::TCppClass > > \
{\
	static PyObject* build(std::auto_ptr< t_ShadowObject::TCppClass > iBySinkedPointer) { return pyBuildSimpleObject_deprecated(iBySinkedPointer); }\
	static int get(PyObject* iV, t_ShadowObject::TCppClass& oV) { return pyGetSimpleObject_deprecated(iV,oV); }\
};\
}\
}


#define PY_SHADOW_DOWN_CASTERS(t_ShadowObject)\
namespace lass\
{\
namespace python\
{\
namespace impl\
{\
template <> struct ArgumentTraits< t_ShadowObject::TCppClass >\
{\
	typedef t_ShadowObject::TCppClass TCppClass;\
	typedef PyObjectPtr<t_ShadowObject>::Type TStorage;\
	static const TCppClass& arg(const TStorage& iArg)\
	{\
		if (!iArg) LASS_THROW("Null pointer reference");\
		return *static_cast<TCppClass*>(iArg->cppObject());\
	}\
};\
template <> struct ArgumentTraits< const t_ShadowObject::TCppClass >\
{\
	typedef t_ShadowObject::TCppClass TCppClass;\
	typedef PyObjectPtr<t_ShadowObject>::Type TStorage;\
	static const TCppClass& arg(const TStorage& iArg)\
	{\
		if (!iArg) LASS_THROW("Null pointer reference");\
		return *static_cast<TCppClass*>(iArg->cppObject());\
	}\
};\
template <> struct ArgumentTraits< t_ShadowObject::TCppClass& >\
{\
	typedef t_ShadowObject::TCppClass TCppClass;\
	typedef PyObjectPtr<t_ShadowObject>::Type TStorage;\
	static TCppClass& arg(const TStorage& iArg)\
	{\
		if (!iArg) LASS_THROW("Null pointer reference");\
		return *static_cast<TCppClass*>(iArg->cppObject());\
	}\
};\
template <> struct ArgumentTraits< const t_ShadowObject::TCppClass& >\
{\
	typedef t_ShadowObject::TCppClass TCppClass;\
	typedef PyObjectPtr<t_ShadowObject>::Type TStorage;\
	static const TCppClass& arg(const TStorage& iArg)\
	{\
		if (!iArg) LASS_THROW("Null pointer reference");\
		return *static_cast<TCppClass*>(iArg->cppObject());\
	}\
};\
template <> struct ArgumentTraits< t_ShadowObject::TCppClass* >\
{\
	typedef t_ShadowObject::TCppClass TCppClass;\
	typedef PyObjectPtr<t_ShadowObject>::Type TStorage;\
	static TCppClass* arg(const TStorage& iArg)\
	{\
		if (!iArg) return 0;\
		return static_cast<TCppClass*>(iArg->cppObject());\
	}\
};\
template <> struct ArgumentTraits< const t_ShadowObject::TCppClass* >\
{\
	typedef t_ShadowObject::TCppClass TCppClass;\
	typedef PyObjectPtr<t_ShadowObject>::Type TStorage;\
	static const TCppClass* arg(const TStorage& iArg)\
	{\
		if (!iArg) return 0;\
		return static_cast<TCppClass*>(iArg->cppObject());\
	}\
};\
template <> struct ArgumentTraits< t_ShadowObject::TCppClass* const >\
{\
	typedef t_ShadowObject::TCppClass TCppClass;\
	typedef PyObjectPtr<t_ShadowObject>::Type TStorage;\
	static TCppClass* const arg(const TStorage& iArg)\
	{\
		if (!iArg) return 0;\
		return static_cast<TCppClass*>(iArg->cppObject());\
	}\
};\
template <> struct ArgumentTraits< const t_ShadowObject::TCppClass* const >\
{\
	typedef t_ShadowObject::TCppClass TCppClass;\
	typedef PyObjectPtr<t_ShadowObject>::Type TStorage;\
	static const TCppClass* const arg(const TStorage& iArg)\
	{\
		if (!iArg) return 0;\
		return static_cast<TCppClass*>(iArg->cppObject());\
	}\
};\
}\
inline PyObject* pyBuildSimpleObject_deprecated( const t_ShadowObject::TCppClass& iByCopy )\
{\
	return ::lass::python::impl::fixObjectType(new t_ShadowObject( iByCopy ));\
}\
inline PyObject* pyBuildSimpleObject_deprecated( t_ShadowObject::TCppClass* iByBorrowedPointer )\
{\
	return t_ShadowObject::pyBuildSimpleObject_fromPtr( iByBorrowedPointer );\
}\
inline PyObject* pyBuildSimpleObject_deprecated( t_ShadowObject::TCppClass& iByBorrowedPointer )\
{\
	return pyBuildSimpleObject_deprecated(&iByBorrowedPointer);\
}\
inline PyObject* pyBuildSimpleObject_deprecated( std::auto_ptr< t_ShadowObject::TCppClass > iBySinkedPointer )\
{\
	return ::lass::python::impl::fixObjectType(new t_ShadowObject( iBySinkedPointer ));\
}\
inline int pyGetSimpleObject_deprecated( PyObject* iObject, t_ShadowObject::TCppClass& oByCopy )\
{\
	if (t_ShadowObject::TCppClass* cppObject = \
		impl::ShadowTraits< t_ShadowObject >::cppObject(iObject))\
	{\
		oByCopy = *cppObject;\
		return 0;\
	}\
	for (size_t i=0;i<impl::ShadowTraits< t_ShadowObject >::byCopyGetters.size();++i)\
	{\
		if (!impl::ShadowTraits< t_ShadowObject >::byCopyGetters[i](iObject, oByCopy))\
			return 0;\
	}\
	return 1;\
}\
inline int pyGetSimpleObject_deprecated( PyObject* iObject, t_ShadowObject::TCppClass*& oByBorrowedPointer )\
{\
	if (t_ShadowObject::TCppClass* cppObject = \
		impl::ShadowTraits< t_ShadowObject >::cppObject(iObject))\
	{\
		oByBorrowedPointer = cppObject;\
		return 0;\
	}\
	for (size_t i=0;i<impl::ShadowTraits< t_ShadowObject >::byBorrowGetters.size();++i)\
	{\
		if (!impl::ShadowTraits< t_ShadowObject >::byBorrowGetters[i](iObject, oByBorrowedPointer))\
			return 0;\
	}\
	return 1;\
}\
template <> \
struct PyExportTraits< t_ShadowObject::TCppClass > \
{\
	static PyObject* build(const t_ShadowObject::TCppClass& iByCopy) { return pyBuildSimpleObject_deprecated(iByCopy); }\
	static int get(PyObject* iV, t_ShadowObject::TCppClass& oV) { return pyGetSimpleObject_deprecated(iV,oV); }\
};\
template <> \
struct PyExportTraits< const t_ShadowObject::TCppClass > \
{\
	static PyObject* build(const t_ShadowObject::TCppClass& iByCopy) { return pyBuildSimpleObject_deprecated(iByCopy); }\
	static int get(PyObject* iV, t_ShadowObject::TCppClass& oV) { return pyGetSimpleObject_deprecated(iV,oV); }\
};\
template <> \
struct PyExportTraits< t_ShadowObject::TCppClass & > \
{\
	static PyObject* build(t_ShadowObject::TCppClass& iByCopy) { return pyBuildSimpleObject_deprecated(iByCopy); }\
	static int get(PyObject* iV, t_ShadowObject::TCppClass& oV) { return pyGetSimpleObject_deprecated(iV,oV); }\
};\
template <> \
struct PyExportTraits< const t_ShadowObject::TCppClass& > \
{\
	static PyObject* build(const t_ShadowObject::TCppClass& iByCopy) { return pyBuildSimpleObject_deprecated(iByCopy); }\
	static int get(PyObject* iV, t_ShadowObject::TCppClass& oV) { return pyGetSimpleObject_deprecated(iV,oV); }\
};\
template <> \
struct PyExportTraits< std::auto_ptr< t_ShadowObject::TCppClass > > \
{\
	static PyObject* build(std::auto_ptr< t_ShadowObject::TCppClass > iBySinkedPointer) { return pyBuildSimpleObject_deprecated(iBySinkedPointer); }\
	static int get(PyObject* iV, t_ShadowObject::TCppClass& oV) { return pyGetSimpleObject_deprecated(iV,oV); }\
};\
template <> \
struct PyExportTraits< t_ShadowObject::TCppClass* > \
{\
	static PyObject* build(t_ShadowObject::TCppClass* iByCopy) { return pyBuildSimpleObject_deprecated(iByCopy); }\
	static int get(PyObject* iV, t_ShadowObject::TCppClass& oV) { return pyGetSimpleObject_deprecated(iV,oV); }\
};\
template <> \
struct PyExportTraits< const t_ShadowObject::TCppClass* > \
{\
	static PyObject* build(const t_ShadowObject::TCppClass* iByCopy) { return pyBuildSimpleObject_deprecated(*iByCopy); }\
	static int get(PyObject* iV, t_ShadowObject::TCppClass& oV) { return pyGetSimpleObject_deprecated(iV,oV); }\
};\
PY_CLASS_CONSTRUCTOR_BYCOPY_IMPL( t_ShadowObject) \
inline PyObject* LASS_CONCATENATE_3(pyBuildSimpleObject,t_ShadowObject,FromParent) ( \
	lass::meta::Select< \
		lass::meta::IsSame<t_ShadowObject::TPyParentClass, lass::python::PyObjectPlus>, \
			t_ShadowObject::TCppClass , \
			t_ShadowObject::TPyParentClass::TCppClass>::Type \
	* iByBorrowedPointer)\
{\
	if (dynamic_cast<t_ShadowObject::TCppClass*>(iByBorrowedPointer))\
	{\
		t_ShadowObject::TCppClass* downCastedObject = dynamic_cast<t_ShadowObject::TCppClass*>(iByBorrowedPointer);\
		return lass::python::pyBuildSimpleObject_deprecated(downCastedObject);\
	}\
	return 0;\
}\
template <typename THasRealPyParent,typename TDummy>\
struct ParentClassInjectorSelector_##t_ShadowObject\
{\
	static void doStuff() {}\
};\
template <typename TDummy> \
struct ParentClassInjectorSelector_##t_ShadowObject <lass::meta::False::Type, TDummy>\
{\
	static void doStuff()\
	{\
		TDummy::constructors().push_back(& LASS_CONCATENATE_3(pyBuildSimpleObject,t_ShadowObject,FromParent));	\
	}\
};\
typedef ParentClassInjectorSelector_##t_ShadowObject < lass::meta::IsSame<t_ShadowObject::TPyParentClass, lass::python::PyObjectPlus>::Type, t_ShadowObject::TPyParentClass > SelectorFor_##t_ShadowObject;\
LASS_EXECUTE_BEFORE_MAIN_EX( t_ShadowObject, SelectorFor_##t_ShadowObject::doStuff(); )\
}\
}

#define PY_SHADOW_DOWN_CASTERS_NOCONSTRUCTOR(t_ShadowObject)\
namespace lass\
{\
namespace python\
{\
namespace impl\
{\
template <> struct ArgumentTraits< t_ShadowObject::TCppClass >\
{\
	typedef t_ShadowObject::TCppClass TCppClass;\
	typedef PyObjectPtr<t_ShadowObject>::Type TStorage;\
	static const TCppClass& arg(const TStorage& iArg)\
	{\
		if (!iArg) LASS_THROW("Null pointer reference");\
		return *static_cast<TCppClass*>(iArg->cppObject());\
	}\
};\
template <> struct ArgumentTraits< const t_ShadowObject::TCppClass >\
{\
	typedef t_ShadowObject::TCppClass TCppClass;\
	typedef PyObjectPtr<t_ShadowObject>::Type TStorage;\
	static const TCppClass& arg(const TStorage& iArg)\
	{\
		if (!iArg) LASS_THROW("Null pointer reference");\
		return *static_cast<TCppClass*>(iArg->cppObject());\
	}\
};\
template <> struct ArgumentTraits< t_ShadowObject::TCppClass& >\
{\
	typedef t_ShadowObject::TCppClass TCppClass;\
	typedef PyObjectPtr<t_ShadowObject>::Type TStorage;\
	static TCppClass& arg(const TStorage& iArg)\
	{\
		if (!iArg) LASS_THROW("Null pointer reference");\
		return *static_cast<TCppClass*>(iArg->cppObject());\
	}\
};\
template <> struct ArgumentTraits< const t_ShadowObject::TCppClass& >\
{\
	typedef t_ShadowObject::TCppClass TCppClass;\
	typedef PyObjectPtr<t_ShadowObject>::Type TStorage;\
	static const TCppClass& arg(const TStorage& iArg)\
	{\
		if (!iArg) LASS_THROW("Null pointer reference");\
		return *static_cast<TCppClass*>(iArg->cppObject());\
	}\
};\
template <> struct ArgumentTraits< t_ShadowObject::TCppClass* >\
{\
	typedef t_ShadowObject::TCppClass TCppClass;\
	typedef PyObjectPtr<t_ShadowObject>::Type TStorage;\
	static TCppClass* arg(const TStorage& iArg)\
	{\
		if (!iArg) return 0;\
		return static_cast<TCppClass*>(iArg->cppObject());\
	}\
};\
template <> struct ArgumentTraits< const t_ShadowObject::TCppClass* >\
{\
	typedef t_ShadowObject::TCppClass TCppClass;\
	typedef PyObjectPtr<t_ShadowObject>::Type TStorage;\
	static const TCppClass* arg(const TStorage& iArg)\
	{\
		if (!iArg) return 0;\
		return static_cast<TCppClass*>(iArg->cppObject());\
	}\
};\
template <> struct ArgumentTraits< t_ShadowObject::TCppClass* const >\
{\
	typedef t_ShadowObject::TCppClass TCppClass;\
	typedef PyObjectPtr<t_ShadowObject>::Type TStorage;\
	static TCppClass* const arg(const TStorage& iArg)\
	{\
		if (!iArg) return 0;\
		return static_cast<TCppClass*>(iArg->cppObject());\
	}\
};\
template <> struct ArgumentTraits< const t_ShadowObject::TCppClass* const >\
{\
	typedef t_ShadowObject::TCppClass TCppClass;\
	typedef PyObjectPtr<t_ShadowObject>::Type TStorage;\
	static const TCppClass* const arg(const TStorage& iArg)\
	{\
		if (!iArg) return 0;\
		return static_cast<TCppClass*>(iArg->cppObject());\
	}\
};\
}\
inline PyObject* pyBuildSimpleObject_deprecated( const t_ShadowObject::TCppClass& iByBorrowedPointer )\
{\
	return t_ShadowObject::pyBuildSimpleObject_fromPtr( const_cast<t_ShadowObject::TCppClass*>(&iByBorrowedPointer) );\
}\
inline PyObject* pyBuildSimpleObject_deprecated( t_ShadowObject::TCppClass* iByBorrowedPointer )\
{\
	return t_ShadowObject::pyBuildSimpleObject_fromPtr( iByBorrowedPointer );\
}\
inline PyObject* pyBuildSimpleObject_deprecated( t_ShadowObject::TCppClass& iByBorrowedPointer )\
{\
	return pyBuildSimpleObject_deprecated(&iByBorrowedPointer);\
}\
inline PyObject* pyBuildSimpleObject_deprecated( std::auto_ptr< t_ShadowObject::TCppClass > iBySinkedPointer )\
{\
	return ::lass::python::impl::fixObjectType(new t_ShadowObject( iBySinkedPointer ));\
}\
inline int pyGetSimpleObject_deprecated( PyObject* iObject, t_ShadowObject::TCppClass*& oByBorrowedPointer )\
{\
	if (t_ShadowObject::TCppClass* cppObject = \
		impl::ShadowTraits< t_ShadowObject >::cppObject(iObject))\
	{\
		oByBorrowedPointer = cppObject;\
		return 0;\
	}\
	return 1;\
}\
template <> \
struct PyExportTraits< t_ShadowObject::TCppClass* > \
{\
	static PyObject* build(const t_ShadowObject::TCppClass* iByCopy) { return pyBuildSimpleObject_deprecated(*iByCopy); }\
	static int get(PyObject* iV, t_ShadowObject::TCppClass* oV) { return pyGetSimpleObject_deprecated(iV,oV); }\
};\
template <> \
struct PyExportTraits< std::auto_ptr< t_ShadowObject::TCppClass > > \
{\
	static PyObject* build(std::auto_ptr< t_ShadowObject::TCppClass > iBySinkedPointer) { return pyBuildSimpleObject_deprecated(iBySinkedPointer); }\
};\
inline PyObject* LASS_CONCATENATE_3(pyBuildSimpleObject,t_ShadowObject,FromParent) ( \
	lass::meta::Select< \
		lass::meta::IsSame< t_ShadowObject::TPyParentClass, lass::python::PyObjectPlus>, \
			t_ShadowObject::TCppClass , \
			t_ShadowObject::TPyParentClass::TCppClass>::Type \
	* iByBorrowedPointer)\
{\
	if (dynamic_cast< t_ShadowObject::TCppClass*>(iByBorrowedPointer))\
	{\
		t_ShadowObject::TCppClass* downCastedObject = dynamic_cast<t_ShadowObject::TCppClass*>(iByBorrowedPointer);\
		return lass::python::pyBuildSimpleObject_deprecated(downCastedObject);\
	}\
	return 0;\
}\
template <typename THasRealPyParent>\
struct ParentClassInjectorSelector_##t_ShadowObject\
{\
	static void doStuff()\
	{\
		THasRealPyParent::constructors().push_back(& LASS_CONCATENATE_3(pyBuildSimpleObject,t_ShadowObject,FromParent));	\
	}\
};\
template <> \
struct ParentClassInjectorSelector_##t_ShadowObject < lass::python::PyObjectPlus >\
{\
	static void doStuff() {}\
};\
typedef ParentClassInjectorSelector_##t_ShadowObject < t_ShadowObject::TPyParentClass > SelectorFor_##t_ShadowObject;\
LASS_EXECUTE_BEFORE_MAIN_EX( t_ShadowObject, SelectorFor_##t_ShadowObject::doStuff(); )\
}\
}

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


