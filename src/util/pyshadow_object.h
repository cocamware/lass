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
#include "../meta/is_derived_type.h"

namespace lass
{
namespace python
{
namespace impl
{

class LASS_DLL PyShadowBaseCommon: public PyObjectPlus
{
protected:
	PyShadowBaseCommon(PyTypeObject* iType);
	PyShadowBaseCommon(const PyShadowBaseCommon& iOther);
	~PyShadowBaseCommon();
	PyShadowBaseCommon& operator=(const PyShadowBaseCommon& iOther);	
};

template <class CppBase>
class PyShadowBase: public PyShadowBaseCommon
{
public:
	virtual ~PyShadowBase()
	{
		if (ownership_ == oOwner) delete cppObject_; cppObject_ = 0;
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
	PyShadowBase(PyTypeObject* iType, CppBase* iCppObject, Ownership iOwnership):
		PyShadowBaseCommon(iType), cppObject_(iCppObject), ownership_(iOwnership)
	{
	}
private:
	CppBase* cppObject_;
	Ownership ownership_;
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

	enum { isShadow = meta::IsDerivedType<T, PyShadowBaseCommon>::value };
	typedef typename Impl<T, isShadow>::TCppClass TCppClass;

	static TCppClass* cppObject(PyObject* iPyObject)
	{
		if (!PyType_IsSubtype(iPyObject->ob_type , &T::Type ))
		{
			PyErr_Format(PyExc_TypeError,"PyObject not castable to %s", T::Type.tp_name);
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
		i_PyObjectShadowClass(const TCppClass& iByCopy):\
			t_PyObjectBase(&Type, new TCppClass(iByCopy), oOwner) {}\
		i_PyObjectShadowClass(TCppClass* iByBorrowedPointer):\
			t_PyObjectBase(&Type, iByBorrowedPointer, oBorrowed) {}\
		i_PyObjectShadowClass(std::auto_ptr<TCppClass> iBySinkedPointer):\
			t_PyObjectBase(&Type, iBySinkedPointer.get(), oOwner) { iBySinkedPointer.release(); }\
	protected:\
		i_PyObjectShadowClass(PyTypeObject* iType, TCppClass* iValue, Ownership iOwnership):\
			t_PyObjectBase(iType, iValue, iOwnership) {}\
	};

#define PY_SHADOW_CLASS(dllInterface, i_PyObjectShadowClass, t_CppClass)\
	PY_SHADOW_CLASS_EX(\
		dllInterface, i_PyObjectShadowClass, t_CppClass, ::lass::python::impl::PyShadowBase< t_CppClass >,\
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
	return new t_ShadowObject( iByCopy );\
}\
inline PyObject* pyBuildSimpleObject( t_ShadowObject::TCppClass* iByBorrowedPointer )\
{\
	return new t_ShadowObject( iByBorrowedPointer );\
}\
inline PyObject* pyBuildSimpleObject( std::auto_ptr< t_ShadowObject::TCppClass > iBySinkedPointer )\
{\
	return new t_ShadowObject( iBySinkedPointer );\
}\
}\
}



#endif

// EOF

