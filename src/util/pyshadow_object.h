/**	@file
 *	@author Bram de Greve (bramz@users.sourceforge.net)
 *	@author Tom De Muer (tomdemuer@users.sourceforge.net)
 *
 *	Distributed under the terms of the GPL (GNU Public License)
 *
 * 	The LASS License:
 *
 *	Copyright 2004 Bram de Greve and Tom De Muer
 *
 *	LASS is free software; you can redistribute it and/or modify
 *	it under the terms of the GNU General Public License as published by
 *	the Free Software Foundation; either version 2 of the License, or
 *	(at your option) any later version.
 *
 *	This program is distributed in the hope that it will be useful,
 *	but WITHOUT ANY WARRANTY; without even the implied warranty of
 *	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *	GNU General Public License for more details.
 *
 *	You should have received a copy of the GNU General Public License
 *	along with this program; if not, write to the Free Software
 *	Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 *
 *	@note 
 *		This header bundles the _EXPERIMENTAL_ code for quasi automatic 
 *		wrapping of C++ object hierarchies into python shadow objects.  This
 *		code is still heavily under development and not ready for production use.
 */

#ifndef LASS_GUARDIAN_OF_INCLUSION_UTIL_PYSHADOW_OBJECT_H
#define LASS_GUARDIAN_OF_INCLUSION_UTIL_PYSHADOW_OBJECT_H

#include "pyobject_plus.h"
#include "../meta/is_derived_type.h"

namespace lass
{
namespace python
{

template<typename T, typename P  >
class PyObjectPlusPlus : public P
{
	PY_HEADER( P );

	typedef T	TPyClassSelf;
	typedef P	TPyClassParent;
	typedef PyObjectPlusPlus<T,P>	TSelf;

public:
    virtual std::string	pyRepr(void) { return "PyObjectPlusPlus object at " + util::stringCast<std::string>(this); }
	virtual std::string	pyStr(void) { return "PyObjectPlusPlus object string at " + util::stringCast<std::string>(this); }

	PyObjectPlusPlus(): P() { /*ob_type = &Type; _Py_NewReference();*/ }
	virtual ~PyObjectPlusPlus() {}
};
/*
template<typename T, typename P  >
class PyShadowObject : public PyObjectPlusPlus<T,P>
{
public:
	typedef T	TCppClas;
	T*	value_;

	PyShadowObject() : value_( new T() ) {}
	operator T() { return *value_; }
	virtual ~PyShadowObject() {}
};
/**/

namespace impl
{

class PyShadowBase: public PyObjectPlus
{
public:
    virtual ~PyShadowBase() 
    { 
        if (ownership_ == oOwner) delete cppObject__; cppObject__ = 0; 
    }
    void* cppObject() const 
    { 
        return cppObject__; 
    }
protected:
    enum Ownership 
    { 
        oOwner, 
        oBorrowed 
    };
    PyShadowBase(PyTypeObject* iType, void* iCppObject, Ownership iOwnership): 
        PyObjectPlus(iType), cppObject__(iCppObject), ownership_(iOwnership) 
    {
    }
private:
    void* cppObject__;
    Ownership ownership_;
};

template< typename T>	
struct ShadowTraits 
{
private:

    template <typename U, bool shadow>
    struct Impl
    {
        typedef U TCppClass;
        static U* cppObject(PyObject* iPyObject) 
        { 
            return static_cast<U*>(iPyObject);
        };
        static U* pyObject(U* iCppObject)
        {
            return iCppObject;
        }
    };

    template <typename U>
    struct Impl<U, true>
    {
        typedef typename U::TCppClass TCppClass;
        static TCppClass* cppObject(PyObject* iPyObject) 
        { 
            return static_cast<TCppClass*>(static_cast<U*>(iPyObject)->cppObject()); 
        };
        static U* pyObject(TCppClass* iCppObject)
        {
            std::auto_ptr<TCppClass> cppObject(iCppObject);
            return new U(cppObject);
        }
    };

public:

    enum { isShadow = meta::IsDerivedType<T, PyShadowBase>::value };
    typedef typename Impl<T, isShadow>::TCppClass TCppClass;

    static TCppClass* cppObject(PyObject* iPyObject) 
    { 
        return Impl<T, isShadow>::cppObject(iPyObject); 
    };
    static T* pyObject(TCppClass* iCppObject)
    {
        return Impl<T, isShadow>::pyObject(iCppObject);
    }
};

}

}

}

#define PY_SHADOW_CLASS_IMPL(PyObjectShadowClass__, CppClass__, PyObjectBase__, PyObjectParent__, PyClassName__)\
    class PyObjectShadowClass__: public PyObjectBase__\
    {\
        PY_HEADER(PyObjectParent__)\
    public:\
        typedef CppClass__ TCppClass;\
        PyObjectShadowClass__(const TCppClass& iByCopy):\
            PyObjectBase__(&Type, new TCppClass(iByCopy), oOwner) {}\
        PyObjectShadowClass__(TCppClass* iByBorrowedPointer):\
            PyObjectBase__(&Type, iByBorrowedPointer, oBorrowed) {}\
        PyObjectShadowClass__(std::auto_ptr<TCppClass> iBySinkedPointer):\
            PyObjectBase__(&Type, iBySinkedPointer.get(), oOwner) { iBySinkedPointer.release(); }\
    protected:\
        PyObjectShadowClass__(PyTypeObject* iType, TCppClass* iValue, Ownership iOwnership):\
            PyObjectBase__(iType, iValue, iOwnership) {}\
    };\

#define PY_SHADOW_CLASS_EX(PyObjectShadowClass__, CppClass__, PyClassName__)\
    PY_SHADOW_CLASS_IMPL(\
        PyObjectShadowClass__, CppClass__, ::lass::python::impl::PyShadowBase,\
        ::lass::python::PyObjectPlus, PyClassName__)

#define PY_SHADOW_CLASS(PyObjectShadowClass__, CppClass__)\
    PY_SHADOW_CLASS_EX(PyObjectShadowClass__, CppClass__, LASS_STRINGIFY(CppClass__))

#define PY_SHADOW_CLASS_DERIVED_EX(PyObjectShadowClass__, CppClass__, PyObjectShadowParent__, PyClassName__)\
    PY_SHADOW_CLASS_IMPL(\
        PyObjectShadowClass__, CppClass__, PyObjectShadowParent__, PyObjectShadowParent__,\
        PyClassName__)

#define PY_SHADOW_CLASS_DERIVED(PyObjectShadowClass__, CppClass__, PyObjectShadowParent__)\
    PY_SHADOW_CLASS_DERIVED_EX(\
        PyObjectShadowClass__, CppClass__, PyObjectShadowParent__, LASS_STRINGIFY(CppClass__))

/*
#define PY_SHADOW_CLASS_METHOD_EX( Object, method, pyFunction, pyName, pyDoc )\
	inline PyObject* pyFunction( PyObject* iObject, PyObject* iArgs )\
	{\
		Object* object = static_cast<Object*>(iObject);\
		return lass::python::impl::PyCallMethod<Object ::TCppClass>::call( iArgs,  ((Object ::RealObject*) (*object)), Object ::RealObject:: method );\
	}\
	EXECUTE( Object::Methods.insert(Object::Methods.begin(),\
		lass::python::createPyMethodDef(pyName, (PyCFunction) pyFunction, METH_VARARGS , pyDoc)); )
*/

#define PY_SHADOW_CASTERS(ShadowObject__)\
namespace lass\
{\
namespace python\
{\
namespace impl\
{\
template <> struct ArgumentTraits<ShadowObject__::TCppClass>\
{\
    typedef ShadowObject__::TCppClass TCppClass;\
    typedef PyObject* TStorage;\
    static const TCppClass& arg(PyObject* iArg)\
    {\
        return *static_cast<TCppClass*>(static_cast<PyShadowBase*>(iArg)->cppObject());\
    }\
};\
template <> struct ArgumentTraits<const ShadowObject__::TCppClass>\
{\
    typedef ShadowObject__::TCppClass TCppClass;\
    typedef PyObject* TStorage;\
    static const TCppClass& arg(PyObject* iArg)\
    {\
        return *static_cast<TCppClass*>(static_cast<PyShadowBase*>(iArg)->cppObject());\
    }\
};\
template <> struct ArgumentTraits<ShadowObject__::TCppClass&>\
{\
    typedef ShadowObject__::TCppClass TCppClass;\
    typedef PyObject* TStorage;\
    static TCppClass& arg(PyObject* iArg)\
    {\
        return *static_cast<TCppClass*>(static_cast<PyShadowBase*>(iArg)->cppObject());\
    }\
};\
template <> struct ArgumentTraits<const ShadowObject__::TCppClass&>\
{\
    typedef ShadowObject__::TCppClass TCppClass;\
    typedef PyObject* TStorage;\
    static const TCppClass& arg(PyObject* iArg)\
    {\
        return *static_cast<TCppClass*>(static_cast<PyShadowBase*>(iArg)->cppObject());\
    }\
};\
template <> struct ArgumentTraits<ShadowObject__::TCppClass*>\
{\
    typedef ShadowObject__::TCppClass TCppClass;\
    typedef PyObject* TStorage;\
    static TCppClass* arg(PyObject* iArg)\
    {\
        return static_cast<TCppClass*>(static_cast<PyShadowBase*>(iArg)->cppObject());\
    }\
};\
template <> struct ArgumentTraits<const ShadowObject__::TCppClass*>\
{\
    typedef ShadowObject__::TCppClass TCppClass;\
    typedef PyObject* TStorage;\
    static const TCppClass* arg(PyObject* iArg)\
    {\
        return static_cast<TCppClass*>(static_cast<PyShadowBase*>(iArg)->cppObject());\
    }\
};\
template <> struct ArgumentTraits<ShadowObject__::TCppClass* const>\
{\
    typedef ShadowObject__::TCppClass TCppClass;\
    typedef PyObject* TStorage;\
    static TCppClass* const arg(PyObject* iArg)\
    {\
        return static_cast<TCppClass*>(static_cast<PyShadowBase*>(iArg)->cppObject());\
    }\
};\
template <> struct ArgumentTraits<const ShadowObject__::TCppClass* const>\
{\
    typedef ShadowObject__::TCppClass TCppClass;\
    typedef PyObject* TStorage;\
    static const TCppClass* const arg(PyObject* iArg)\
    {\
        return static_cast<TCppClass*>(static_cast<PyShadowBase*>(iArg)->cppObject());\
    }\
};\
}\
inline PyObject* pyBuildSimpleObject( const ShadowObject__::TCppClass& iByCopy )\
{\
    return new ShadowObject__( iByCopy );\
}\
inline PyObject* pyBuildSimpleObject( ShadowObject__::TCppClass* iByBorrowedPointer )\
{\
    return new ShadowObject__( iByBorrowedPointer );\
}\
inline PyObject* pyBuildSimpleObject( std::auto_ptr<ShadowObject__::TCppClass> iBySinkedPointer )\
{\
    return new ShadowObject__( iBySinkedPointer );\
}\
}\
}



#endif

// EOF

