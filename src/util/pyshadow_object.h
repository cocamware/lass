/**	@file
 *	@author Bram de Greve (bramz@users.sourceforge.net)
 *	@author Tom De Muer (tomdemuer@users.sourceforge.net)
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
 *	Copyright (C) 2004-2007 the Initial Developer.
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

#ifndef LASS_GUARDIAN_OF_INCLUSION_UTIL_PYSHADOW_OBJECT_H
#define LASS_GUARDIAN_OF_INCLUSION_UTIL_PYSHADOW_OBJECT_H

#include "pyobject_plus.h"
#include "../meta/is_derived.h"

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

template <class CppBase, bool weak = false>
class PyShadowBase: public PyShadowBaseCommon
{
public:
	virtual ~PyShadowBase()
	{
		if (!weak && ownership_ == oOwner) 
		{
			delete cppObject_; 
			cppObject_ = 0;
		}
	}
	CppBase* cppObject() const
	{
		return cppObject_;
	}
protected:
	enum Ownership
	{
		oOwner,
		oBorrowed
	};
	PyShadowBase(CppBase* iCppObject, Ownership iOwnership):
		cppObject_(iCppObject), ownership_(iOwnership)
	{
	}
private:
	CppBase* cppObject_;
	Ownership ownership_;
};


/** a weak shadow class NEVER EVER owns the object, USE AT YOUR OWN RISK!  
*	Consult your local Lass::Python guru to gain insight when the use of this
*	class is appropriate.  A rule of thumb is that any properly designed 
*	C++ interface should never be exposed using weak shadow objects.
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
		static std::vector<TConstructorChainMethod> constructors;\
		static PyObject* pyBuildSimpleObject_fromPtr(TCppClass* iPtr)\
		{\
			for (size_t i=0;i<constructors.size();++i)\
			{\
				PyObject* tempObj = constructors[i](iPtr);\
				if ( (tempObj!=0) && (tempObj!=Py_None))\
					return tempObj;\
			}\
			return ::lass::python::impl::fixObjectType(new i_PyObjectShadowClass( iPtr ));\
		}\
	protected:\
		i_PyObjectShadowClass(TCppClass* iValue, Ownership iOwnership):\
			t_PyObjectBase(iValue, iOwnership) {}\
	};\
	std::vector<i_PyObjectShadowClass::TConstructorChainMethod> i_PyObjectShadowClass::constructors;


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
		t_ShadowObject::TPyParentClass::constructors.push_back(& LASS_CONCATENATE_3(pyBuildSimpleObject,t_ShadowObject,FromParent));	\
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

