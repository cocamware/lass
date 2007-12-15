/** @file
 *  @author Bram de Greve (bramz@users.sourceforge.net)
 *  @author Tom De Muer (tomdemuer@users.sourceforge.net)
 *
 *  Distributed under the terms of the GPL (GNU Public License)
 *
 *  The LASS License:
 *
 *  Copyright 2004-2006 Bram de Greve and Tom De Muer
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
		if (!weak && ownership_ == oOwner) 
		{
			delete this->cppObject_; 
			this->cppObject_ = 0;
		}
	}
	CppBase* cppObject() const
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
		ownership_(iOwnership), PyShadowBaseTracked<CppBase>(iCppObject,doTracking)
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
		static U* pyObject(U* iCppObject)
		{
			return iCppObject;
		}
	};

public:

	enum { isShadow = IsShadowClass<T>::value };
	typedef typename Impl<T, isShadow>::TCppClass TCppClass;

	static TCppClass* cppObject(PyObject* iPyObject)
	{
		if (!PyType_IsSubtype(iPyObject->ob_type , &T::_lassPyType ))
		{
			PyErr_Format(PyExc_TypeError,"PyObject not castable to %s", T::_lassPyType.tp_name);
			return 0;
		}
		return Impl<T, isShadow>::cppObject(static_cast<T*>(iPyObject));
	};
	static T* pyObject(TCppClass* iCppObject)
	{
		return Impl<T, isShadow>::pyObject(iCppObject);
	}
};

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


#define PY_SHADOW_CLASS(dllInterface, i_PyObjectShadowClass, t_CppClass)\
	PY_SHADOW_CLASS_EX(\
		dllInterface, i_PyObjectShadowClass, t_CppClass, ::lass::python::impl::PyShadowBase< t_CppClass >,\
		::lass::python::PyObjectPlus)

/** a weak shadow class NEVER EVER owns the object, USE AT YOUR OWN RISK!  
*	Consult your local Lass::Python guru to gain insight when the use of this
*	class is appropriate.  A rule of thumb is that any properly designed 
*	C++ interface should never be exposed using weak shadow objects.
 */
#define PY_WEAK_SHADOW_CLASS(dllInterface, i_PyObjectShadowClass, t_CppClass)\
	PY_SHADOW_CLASS_EX(\
	dllInterface, i_PyObjectShadowClass, t_CppClass, ::lass::python::impl::PyShadowBaseWeak< t_CppClass >::Type ,\
		::lass::python::PyObjectPlus)

/** a macro to enable automatic invalidation for a given shadow class */
#define PY_SHADOW_CLASS_ENABLE_AUTOMATIC_INVALIDATION(i_PyObjectShadowClass)\
	i_PyObjectShadowClass::doTracking = true;

#define PY_SHADOW_CLASS_DERIVED(dllInterface, i_PyObjectShadowClass, t_CppClass, t_PyObjectShadowParent)\
	PY_SHADOW_CLASS_EX(\
		dllInterface, i_PyObjectShadowClass, t_CppClass, t_PyObjectShadowParent, t_PyObjectShadowParent)


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
inline PyObject* pyBuildSimpleObject( const t_ShadowObject::TCppClass& iByCopy )\
{\
	return ::lass::python::impl::fixObjectType(new t_ShadowObject( iByCopy ));\
}\
inline PyObject* pyBuildSimpleObject( t_ShadowObject::TCppClass* iByBorrowedPointer )\
{\
	return ::lass::python::impl::fixObjectType(new t_ShadowObject( iByBorrowedPointer ));\
}\
inline PyObject* pyBuildSimpleObject( t_ShadowObject::TCppClass& iByBorrowedPointer )\
{\
	return pyBuildSimpleObject(&iByBorrowedPointer);\
}\
inline PyObject* pyBuildSimpleObject( std::auto_ptr< t_ShadowObject::TCppClass > iBySinkedPointer )\
{\
	return ::lass::python::impl::fixObjectType(new t_ShadowObject( iBySinkedPointer ));\
}\
inline int pyGetSimpleObject( PyObject* iObject, t_ShadowObject::TCppClass& oByCopy )\
{\
	if (t_ShadowObject::TCppClass* cppObject = \
		impl::ShadowTraits< t_ShadowObject >::cppObject(iObject))\
	{\
		oByCopy = *cppObject;\
		return 0;\
	}\
	return 1;\
}\
inline int pyGetSimpleObject( PyObject* iObject, t_ShadowObject::TCppClass*& oByBorrowedPointer )\
{\
	if (t_ShadowObject::TCppClass* cppObject = \
		impl::ShadowTraits< t_ShadowObject >::cppObject(iObject))\
	{\
		oByBorrowedPointer = cppObject;\
		return 0;\
	}\
	return 1;\
}\
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
inline PyObject* pyBuildSimpleObject( const t_ShadowObject::TCppClass& iByCopy )\
{\
	return ::lass::python::impl::fixObjectType(new t_ShadowObject( iByCopy ));\
}\
inline PyObject* pyBuildSimpleObject( t_ShadowObject::TCppClass* iByBorrowedPointer )\
{\
	return t_ShadowObject::pyBuildSimpleObject_fromPtr( iByBorrowedPointer );\
}\
inline PyObject* pyBuildSimpleObject( t_ShadowObject::TCppClass& iByBorrowedPointer )\
{\
	return pyBuildSimpleObject(&iByBorrowedPointer);\
}\
inline PyObject* pyBuildSimpleObject( std::auto_ptr< t_ShadowObject::TCppClass > iBySinkedPointer )\
{\
	return ::lass::python::impl::fixObjectType(new t_ShadowObject( iBySinkedPointer ));\
}\
inline int pyGetSimpleObject( PyObject* iObject, t_ShadowObject::TCppClass& oByCopy )\
{\
	if (t_ShadowObject::TCppClass* cppObject = \
		impl::ShadowTraits< t_ShadowObject >::cppObject(iObject))\
	{\
		oByCopy = *cppObject;\
		return 0;\
	}\
	return 1;\
}\
inline int pyGetSimpleObject( PyObject* iObject, t_ShadowObject::TCppClass*& oByBorrowedPointer )\
{\
	if (t_ShadowObject::TCppClass* cppObject = \
		impl::ShadowTraits< t_ShadowObject >::cppObject(iObject))\
	{\
		oByBorrowedPointer = cppObject;\
		return 0;\
	}\
	return 1;\
}\
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
		return lass::python::pyBuildSimpleObject(downCastedObject);\
	}\
	return 0;\
}\
template <typename THasRealPyParent>\
struct ParentClassInjectorSelector_##t_ShadowObject\
{\
	static void doStuff()\
	{\
		t_ShadowObject::TPyParentClass::constructors().push_back(& LASS_CONCATENATE_3(pyBuildSimpleObject,t_ShadowObject,FromParent));	\
	}\
};\
template <> \
struct ParentClassInjectorSelector_##t_ShadowObject <typename lass::meta::True::Type>\
{\
	static void doStuff() {}\
};\
typedef ParentClassInjectorSelector_##t_ShadowObject < lass::meta::IsSame<t_ShadowObject::TPyParentClass, lass::python::PyObjectPlus>::Type > SelectorFor_##t_ShadowObject;\
LASS_EXECUTE_BEFORE_MAIN_EX( t_ShadowObject, SelectorFor_##t_ShadowObject::doStuff(); )\
}\
}


#endif

// EOF


