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
 *	Copyright (C) 2004-2009 the Initial Developer.
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

namespace lass
{
namespace python
{

// --- impl ----------------------------------------------------------------------------------------

namespace impl
{

// --- ModuleDefinition ----------------------------------------------------------------------------

/** @internal
 */
template <typename CppClass>
void ModuleDefinition::injectClass(const char* doc)
{
	std::pair<std::string, PyObject*> c = 
		prepareClassForModuleInjection<CppClass>(name_.get(), doc);
	PyModule_AddObject(module_, const_cast<char*>(c.first.c_str()), c.second);
}




/** @internal
 */
template <typename T, PyCFunction dispatcher> struct FunctionTypeDispatcher
{
	static PyObject* fun(PyObject* self, PyObject* args) { return dispatcher(self,args); }
	typedef PyCFunction OverloadType;
};
/** @internal
 */
template <PyCFunction DispatcherAddress> struct FunctionTypeDispatcher<lass::python::impl::UnarySlot ,DispatcherAddress>
{
	static PyObject* fun(PyObject* iSelf)
	{
		TPyObjPtr args(PyTuple_New(0));
		return DispatcherAddress(iSelf, args.get());
	}
};
/** @internal
 */
template <PyCFunction DispatcherAddress> struct FunctionTypeDispatcher<lass::python::impl::BinarySlot ,DispatcherAddress>
{
	static PyObject* fun(PyObject* iSelf, PyObject* iOther)
	{
		TPyObjPtr args(Py_BuildValue("(O)", iOther));
		return DispatcherAddress(iSelf, args.get());
	}
};
/** @internal
 */
template <PyCFunction DispatcherAddress> struct FunctionTypeDispatcher<lass::python::impl::TernarySlot ,DispatcherAddress>
{
	static PyObject* fun(PyObject* iSelf, PyObject* iArgs, PyObject* iKw)
	{
		if (iKw)
		{
			PyErr_SetString(PyExc_TypeError, "keyword arguments are not supported");
			return 0;
		}
		return DispatcherAddress(iSelf, iArgs);
	}
};
/** @internal
*/
template <PyCFunction DispatcherAddress> struct FunctionTypeDispatcher<lass::python::impl::SsizeArgSlot ,DispatcherAddress>
{
	static PyObject* fun(PyObject* iSelf, Py_ssize_t iSize) 
	{
		TPyObjPtr args(Py_BuildValue("(n)", iSize));
		return DispatcherAddress(iSelf,args.get());
	}
};
/** @internal
*/
template <PyCFunction DispatcherAddress> struct FunctionTypeDispatcher<lass::python::impl::SsizeSsizeArgSlot ,DispatcherAddress>
{
	static PyObject* fun(PyObject* iSelf, Py_ssize_t iSize, Py_ssize_t iSize2)
	{
		TPyObjPtr args(Py_BuildValue("(n,n)", iSize, iSize2));
		return DispatcherAddress(iSelf,args.get());
	}
};
/** @internal
 */
template <PyCFunction DispatcherAddress> struct FunctionTypeDispatcher<lass::python::impl::LenSlot ,DispatcherAddress>
{
	static Py_ssize_t fun(PyObject* iSelf) 
	{
		TPyObjPtr args(Py_BuildValue("()"));
		TPyObjPtr temp(DispatcherAddress(iSelf,args.get()));
		if (!temp)
			return -1;
		Py_ssize_t result;
		if (pyGetSimpleObject(temp.get(), result) != 0)
			return -1;
		return result;
	}
};
/** @internal
 */
template <PyCFunction DispatcherAddress> struct FunctionTypeDispatcher<lass::python::impl::SsizeObjArgSlot ,DispatcherAddress>
{
	static int fun(PyObject * iSelf, Py_ssize_t iSize, PyObject * iOther)
	{
		TPyObjPtr args(Py_BuildValue("(n,O)",iSize,iOther));
		TPyObjPtr temp(DispatcherAddress(iSelf,args.get()));
		return temp ? 0 : -1;
	}
};
/** @internal
 */
template <PyCFunction DispatcherAddress> struct FunctionTypeDispatcher<lass::python::impl::SsizeSsizeObjArgSlot ,DispatcherAddress>
{
	static int fun(PyObject * iSelf, Py_ssize_t iSize, Py_ssize_t iSize2, PyObject * iOther)
	{
		TPyObjPtr args(Py_BuildValue("(n,n,O)",iSize,iSize2,iOther));
		TPyObjPtr temp(DispatcherAddress(iSelf,args.get()));
		return temp ? 0 : -1;
	}
};
/** @internal
 */
template <PyCFunction DispatcherAddress> struct FunctionTypeDispatcher<lass::python::impl::ObjObjSlot ,DispatcherAddress>
{
	static int fun(PyObject * iSelf, PyObject * iOther)
	{
		TPyObjPtr args(Py_BuildValue("(O)",iOther));
		TPyObjPtr temp(DispatcherAddress(iSelf,args.get()));
		return temp ? 0 : -1;
	}
};
/** @internal
 */
template <PyCFunction DispatcherAddress> struct FunctionTypeDispatcher<lass::python::impl::ObjObjArgSlot ,DispatcherAddress>
{
	static int fun(PyObject * iSelf, PyObject * iOther, PyObject* iOther2)
	{
		TPyObjPtr args(Py_BuildValue("(O,O)",iOther,iOther2));
		TPyObjPtr temp(DispatcherAddress(iSelf,args.get()));
		return temp ? 0 : -1;
	}
};
/** @internal
*/
template <PyCFunction DispatcherAddress> struct FunctionTypeDispatcher<lass::python::impl::IterSlot ,DispatcherAddress>
{
	static PyObject* fun(PyObject* iSelf)
	{
		TPyObjPtr args(PyTuple_New(0));
		return DispatcherAddress(iSelf, args.get());
	}
};
/** @internal
*/
template <PyCFunction DispatcherAddress> struct FunctionTypeDispatcher<lass::python::impl::IterNextSlot ,DispatcherAddress>
{
	static PyObject* fun(PyObject* iSelf)
	{
		TPyObjPtr args(PyTuple_New(0));
		return DispatcherAddress(iSelf, args.get());
	}
};
/////////////////////////////////////////////



/** @internal 
 *  Returns a  pair<short name, pyobject pointing at the class object>
 *  We have to work around the static initializer fiasco by demanding the iModuleName instead of pealing 
 *	it from the module object (which may not be initialized yet... and then we have *kaboom*)
 */
template <typename CppClass>
inline std::pair<std::string,PyObject*> prepareClassForModuleInjection(const char* iModuleName, const char* iClassDocumentation)
{
	char* shortName = const_cast<char*>(CppClass::_lassPyType.tp_name); // finalizePyType will expand tp_name with module name.
	finalizePyType(CppClass::_lassPyType, *CppClass::_lassPyGetParentType(), CppClass::_lassPyMethods, CppClass::_lassPyGetSetters,
		CppClass::_lassPyStatics, iModuleName, iClassDocumentation);
	return std::pair<std::string,PyObject*>(std::string(shortName), reinterpret_cast<PyObject*>(&CppClass::_lassPyType));
}

/** @intenal
 */
template <typename CppClass>
void addClassStaticMethod(
		const char* iMethodName, const char* iDocumentation,
		PyCFunction iMethodDispatcher, PyCFunction& oOverloadChain)
{
#if PY_VERSION_HEX >= 0x02030000 // >= 2.3
	::std::vector<PyMethodDef>::iterator i = ::std::find_if(
		CppClass::_lassPyMethods.begin(), CppClass::_lassPyMethods.end(), PyMethodEqual(iMethodName));
	if (i == CppClass::_lassPyMethods.end())
	{
		CppClass::_lassPyMethods.insert(CppClass::_lassPyMethods.begin(), createPyMethodDef(
			iMethodName, iMethodDispatcher, METH_VARARGS | METH_STATIC, iDocumentation));
		oOverloadChain = 0;
	}
	else
	{
		LASS_ASSERT(i->ml_flags == (METH_VARARGS | METH_CLASS));
		oOverloadChain = i->ml_meth;
		i->ml_meth = iMethodDispatcher;
		if (i->ml_doc == 0)
		{
			i->ml_doc = const_cast<char*>(iDocumentation);
		}
	}
#else
	TStaticMembers::iterator i = ::std::find_if(
		CppClass::_lassPyStatics.begin(), CppClass::_lassPyStatics.end(), StaticMemberEqual(iMethodName));
	if (i == CppClass::_lassPyStatics.end())
	{
		PyMethodDef* methodDef(new PyMethodDef(createPyMethodDef(
			iMethodName, iMethodDispatcher, METH_VARARGS, iDocumentation)));
		PyObject* cFunction = PyCFunction_New(methodDef, 0);
		PyObject* descr = PyStaticMethod_New(cFunction);
		CppClass::_lassPyStatics.push_back(createStaticMember(
			iMethodName, iDocumentation, staticMemberHelperObject(descr)));
		oOverloadChain = 0;
	}
	else
	{
		PyObject* descr = i->object;
		LASS_ASSERT(descr && PyObject_IsInstance(
			descr, reinterpret_cast<PyObject*>(&PyStaticMethod_Type)));		
		PyObject* cFunction = PyStaticMethod_Type.tp_descr_get(descr, 0, 0);
		LASS_ASSERT(cFunction && PyObject_IsInstance(
			cFunction, reinterpret_cast<PyObject*>(&PyCFunction_Type)));
		PyMethodDef* methodDef = reinterpret_cast<PyCFunctionObject*>(cFunction)->m_ml;
		LASS_ASSERT(methodDef && methodDef->ml_flags == METH_VARARGS);
		oOverloadChain = methodDef->ml_meth;
		methodDef->ml_meth = iMethodDispatcher;
		if (methodDef->ml_doc == 0)
		{
			methodDef->ml_doc = const_cast<char*>(iDocumentation);
		}
		i->doc = methodDef->ml_doc;
	}	
#endif
}	


/** @internal
 */
template <typename CppClass, typename T>
void addClassStaticConst(const char* iName, const T& iValue)
{
	LASS_ASSERT(std::count_if(
		CppClass::_lassPyStatics.begin(), CppClass::_lassPyStatics.end(), StaticMemberEqual(iName)) == 0);
	CppClass::_lassPyStatics.push_back(createStaticMember(iName, 0, staticMemberHelperObject(iValue)));
}



/** @internal
 */
template <typename InnerCppClass>
inline void addClassInnerClass(
		TStaticMembers& oOuterStatics, const char* iInnerClassName, const char* iDocumentation)
{
	LASS_ASSERT(std::count_if(InnerCppClass::_lassPyStatics.begin(), InnerCppClass::_lassPyStatics.end(), 
		StaticMemberEqual(iInnerClassName)) == 0);
	oOuterStatics.push_back(createStaticMember(
		iInnerClassName, iDocumentation, staticMemberHelperType(&InnerCppClass::_lassPyType),
		InnerCppClass::_lassPyGetParentType(), &InnerCppClass::_lassPyMethods, &InnerCppClass::_lassPyGetSetters, 
		&InnerCppClass::_lassPyStatics));
}


/** @internal
 *  helper for pyNumericCast
 */
template <bool InIsSigned> 
struct PyNumericCaster
{
	template <typename In, typename Out> static int cast( In iIn, Out& oOut )
	{
		LASS_ASSERT(num::NumTraits<Out>::isSigned == true);
#if LASS_COMPILER_TYPE == LASS_COMPILER_TYPE_INTEL
		volatile Out min = num::NumTraits<Out>::min;
		volatile Out max = num::NumTraits<Out>::max;
#else
		const Out min = num::NumTraits<Out>::min;
		const Out max = num::NumTraits<Out>::max;
#endif
		if (iIn < min)
		{
			std::ostringstream buffer;
			buffer << "not a " << num::NumTraits<Out>::name() << ": underflow: "
				<< iIn << " < " << min;
			PyErr_SetString(PyExc_TypeError, buffer.str().c_str());
			return 1;
		}
		if (iIn > max)
		{
			std::ostringstream buffer;
			buffer << "not a " << num::NumTraits<Out>::name() << ": overflow: "
				<< iIn << " > " << max;
			PyErr_SetString(PyExc_TypeError, buffer.str().c_str());
			return 1;
		}
		oOut = static_cast<Out>(iIn);
		return 0;
	}
};

/** @internal
 *  helper for pyNumericCast
 */
template <> 
struct PyNumericCaster<false> // In is unsigned
{
	template <typename In, typename Out> static int cast( In iIn, Out& oOut )
	{
		LASS_ASSERT(num::NumTraits<Out>::isSigned == false);
#if LASS_COMPILER_TYPE == LASS_COMPILER_TYPE_INTEL
		volatile Out max = num::NumTraits<Out>::max;
#else
		const Out max = num::NumTraits<Out>::max;
#endif
		if (iIn > max)
		{
			std::ostringstream buffer;
			buffer << "not a " << num::NumTraits<Out>::name() << ": overflow: "
				<< iIn << " > " << max;
			PyErr_SetString(PyExc_TypeError, buffer.str().c_str());
			return 1;
		}
		oOut = static_cast<Out>(iIn);
		return 0;
	}
};



/** @internal
 *  casts one numerical value to another with range checking.
 *  implementation detail.
 *  @note range of In should fully contain range of Out.
 *  @note In and Out should both be signed or unsigned.
 */
template <typename In, typename Out>
int pyNumericCast(In iIn, Out& oOut)
{
	return PyNumericCaster< num::NumTraits<In>::isSigned >::cast( iIn, oOut );
}



/** @internal
 *  convert a PyObject to an signed integer with range checking.
 *  implementation detail.
 */
template <typename Integer>
int pyGetSignedObject( PyObject* iValue, Integer& oV )
{
#if PY_MAJOR_VERSION < 3
	if (PyInt_Check(iValue))
	{
		long temp = PyInt_AS_LONG(iValue);
		return pyNumericCast( temp, oV );
	}
#endif
	if (PyLong_Check(iValue))
	{
#if HAVE_LONG_LONG
		PY_LONG_LONG temp = PyLong_AsLongLong(iValue);
#else
		long temp = PyLong_AsLong(iValue);
#endif
		if (PyErr_Occurred())
		{
			PyErr_Format(PyExc_TypeError, "not a %s: overflow",
				num::NumTraits<Integer>::name().c_str());
			return 1;
		}
		return pyNumericCast( temp, oV );
	}
	PyErr_Format(PyExc_TypeError, "not a %s", num::NumTraits<Integer>::name().c_str());
	return 1;
}

/** @internal
 *  convert a PyObject to an unsigned integer with range checking.
 *  implementation detail.
 */
template <typename Integer>
int pyGetUnsignedObject( PyObject* iValue, Integer& oV )
{
#if PY_MAJOR_VERSION < 3
	if (PyInt_Check(iValue))
	{
		long temp = PyInt_AS_LONG(iValue);
		if (temp < 0)
		{
			std::ostringstream buffer;
			buffer << "not a " << num::NumTraits<Integer>::name() << ": negative: "
				<< temp << " < 0";
			PyErr_SetString(PyExc_TypeError, buffer.str().c_str());
			return 1;
		}
		return pyNumericCast( static_cast<unsigned long>(temp), oV );
	}
#endif
	if (PyLong_Check(iValue))
	{
#if HAVE_LONG_LONG
		unsigned PY_LONG_LONG temp = PyLong_AsUnsignedLongLong(iValue);
#else
		unsigned long temp = PyLong_AsUnsignedLong(iValue);
#endif
		if (PyErr_Occurred())
		{
			PyErr_Format(PyExc_TypeError, "not a %s: overflow",
				num::NumTraits<Integer>::name().c_str());
			return 1;
		}
		return pyNumericCast( temp, oV );
	}
	PyErr_Format(PyExc_TypeError, "not a %s", num::NumTraits<Integer>::name().c_str());
	return 1;
}

/** @internal
 *  convert a PyObject to a floating point value with range checking.
 *  implementation detail.
 */
template <typename Float>
int pyGetFloatObject( PyObject* iValue, Float& oV )
{
	if (PyFloat_Check(iValue))
	{
		double temp = PyFloat_AS_DOUBLE(iValue);
		return pyNumericCast( temp, oV );
	}
#if PY_MAJOR_VERSION < 3
	if (PyInt_Check(iValue))
	{
		long temp = PyInt_AS_LONG(iValue);
		oV = static_cast<Float>( temp );
		return 0;
	}
#endif
	if (PyLong_Check(iValue))
	{
		double temp = PyLong_AsDouble(iValue);
		if (PyErr_Occurred())
		{
			PyErr_Format(PyExc_TypeError, "not a %s: overflow",
				num::NumTraits<Float>::name().c_str());
			return 1;
		}
		return pyNumericCast( temp, oV );
	}
	PyErr_Format(PyExc_TypeError, "not a %s", num::NumTraits<Float>::name().c_str());
	return 1;
}

}

/** @ingroup Python
 *  @internal
 */
template <>
struct PyExportTraits<bool>
{
	static PyObject* build(bool iV)
	{
		return pyBuildSimpleObject(static_cast<long>(iV));
	}
	static int get( PyObject* iValue, bool& oV )
	{
		int result = PyObject_IsTrue(iValue);
		if (result == -1)
		{
			PyErr_SetString(PyExc_TypeError, "does not evaluate to a boolean");
			return 1;
		}
		oV = (result != 0);
		return 0;
	}
};

/** @ingroup Python
 *  @internal
 */
template <>
struct PyExportTraits<signed char>
{
	static PyObject* build( signed char iV )
	{
		return pyBuildSimpleObject(static_cast<long>(iV));
	}
	static int get( PyObject* iValue, signed char& oV )
	{
		return impl::pyGetSignedObject( iValue, oV );
	}
};

/** @ingroup Python
 *  @internal
 */
template <>
struct PyExportTraits<unsigned char>
{
	static PyObject* build( unsigned char iV )
	{
		return pyBuildSimpleObject(static_cast<long>(iV));
	}
	static int get( PyObject* iValue, unsigned char& oV )
	{
		return impl::pyGetUnsignedObject( iValue, oV );
	}
};

/** @ingroup Python
 *  @internal
 */
template <>
struct PyExportTraits<signed short>
{
	static PyObject* build( signed short iV )
	{
		return pyBuildSimpleObject(static_cast<long>(iV));
	}
	static int get( PyObject* iValue, signed short& oV )
	{
		return impl::pyGetSignedObject( iValue, oV );
	}
};

/** @ingroup Python
 *  @internal
 */
template <>
struct PyExportTraits<unsigned short>
{
	inline PyObject* build( unsigned short iV )
	{
		return pyBuildSimpleObject(static_cast<long>(iV));
	}
	static int get( PyObject* iValue, unsigned short& oV )
	{
		return impl::pyGetUnsignedObject( iValue, oV );
	}
};

/** @ingroup Python
 *  @internal
 */
template <>
struct PyExportTraits<signed int>
{
	static PyObject* build( signed int iV )
	{
		return pyBuildSimpleObject(static_cast<long>(iV));
	}
	static int get( PyObject* iValue, signed int& oV )
	{
		return impl::pyGetSignedObject( iValue, oV );
	}
};

/** @ingroup Python
 *  @internal
 */
template <>
struct PyExportTraits<unsigned int>
{
	static PyObject* build( unsigned int iV )
	{
		return PyLong_FromUnsignedLong(static_cast<unsigned long>(iV));
	}
	static int get( PyObject* iValue, unsigned int& oV )
	{
		return impl::pyGetUnsignedObject( iValue, oV );
	}
};

/** @ingroup Python
 *  @internal
 */
template <>
struct PyExportTraits<signed long>
{
	static PyObject* build( signed long iV )
	{
#if PY_MAJOR_VERSION < 3
		return PyInt_FromLong(iV);
#else
		return PyLong_FromLong(iV);
#endif
	}
	static int get( PyObject* iValue, signed long& oV )
	{
		return impl::pyGetSignedObject( iValue, oV );
	}
};

/** @ingroup Python
 *  @internal
 */
template <>
struct PyExportTraits<unsigned long>
{
	static PyObject* build(unsigned long iV)
	{
		return PyLong_FromUnsignedLong(iV);
	}
	static int get( PyObject* iValue, unsigned long& oV )
	{
		return impl::pyGetUnsignedObject( iValue, oV );
	}
};

#ifdef HAVE_LONG_LONG

/** @ingroup Python
 *  @internal
 */
template <>
struct PyExportTraits<signed PY_LONG_LONG>
{
	static PyObject* build( signed PY_LONG_LONG iV )
	{
		return PyLong_FromLongLong(iV);
	}
	static int get( PyObject* iValue, signed PY_LONG_LONG& oV )
	{
		return impl::pyGetSignedObject( iValue, oV );
	}
};

/** @ingroup Python
 *  @internal
 */
template <>
struct PyExportTraits<unsigned PY_LONG_LONG>
{
	static PyObject* build( unsigned PY_LONG_LONG iV )
	{
		return PyLong_FromUnsignedLongLong(iV);
	}
	static int get( PyObject* iValue, unsigned PY_LONG_LONG& oV )
	{
		return impl::pyGetUnsignedObject( iValue, oV );
	}
};

#endif

/** @ingroup Python
 *  @internal
 */
template <>
struct PyExportTraits<float>
{
	static PyObject* build( float iV )
	{
		return PyFloat_FromDouble(static_cast<double>(iV));
	}
	static int get( PyObject* iValue, float& oV )
	{
		return impl::pyGetFloatObject( iValue, oV );
	}
};

/** @ingroup Python
 *  @internal
 */
template <>
struct PyExportTraits<double>
{
	static PyObject* build( double iV )
	{
		return PyFloat_FromDouble(iV);
	}
	static int get( PyObject* iValue, double& oV )
	{
		return impl::pyGetFloatObject( iValue, oV );
	}
};

/** @ingroup Python
 *  @internal
 */
template <>
struct PyExportTraits<long double>
{
	static PyObject* build( long double iV )
	{
		return PyFloat_FromDouble(static_cast<double>(iV));
	}
	static int get( PyObject* iValue, long double& oV )
	{
		return impl::pyGetFloatObject( iValue, oV );
	}
};

/** @ingroup Python
 *  @internal
 */
template <>
struct PyExportTraits<const char*>
{
	static PyObject* build( const char* iV )
	{
#if PY_MAJOR_VERSION < 3
		return PyString_FromString(iV);
#else
		return PyUnicode_FromString(iV);
#endif
	}
};

/** @ingroup Python
 *  @internal
 */
template <size_t N>
struct PyExportTraits<const char [N]>
{
	static PyObject* build( const char iV[N] )
	{
		return pyBuildSimpleObject(&iV[0]);
	}
};

/** @ingroup Python
 *  @internal
 */
template <typename T>
struct PyExportTraits< util::SharedPtr<T, PyObjectStorage, PyObjectCounter> >
{
	static PyObject* build( const util::SharedPtr<T, PyObjectStorage, PyObjectCounter>& iV )
	{
		if (!iV)
		{
			Py_RETURN_NONE;
		}
		return fromSharedPtrToNakedCast(iV);
	}
	static int get(PyObject* iValue, util::SharedPtr<T, PyObjectStorage, PyObjectCounter>& oV)
	{
		const bool isNone = (iValue == Py_None );
		if (isNone)
		{
			oV = util::SharedPtr<T, PyObjectStorage, PyObjectCounter>();
		}
		else
		{
			if (!PyType_IsSubtype(iValue->ob_type , &T::_lassPyType ))
			{
				PyErr_Format(PyExc_TypeError,"not castable to %s",T::_lassPyType.tp_name);
				return 1;
			}
			oV = fromNakedToSharedPtrCast<T>(iValue);
		}
		return 0;
	}
};

/** @ingroup Python
 *  @internal
 */
template <>
struct PyExportTraits< util::SharedPtr<PyObject, PyObjectStorage, PyObjectCounter> >
{
	static PyObject* build( const util::SharedPtr<PyObject, PyObjectStorage, PyObjectCounter>& iV )
	{
		if (!iV)
		{
			Py_RETURN_NONE;
		}
		return fromSharedPtrToNakedCast(iV);
	}
	static int get(PyObject* iValue, util::SharedPtr<PyObject, PyObjectStorage, PyObjectCounter>& oV)
	{
		const bool isNone = (iValue == Py_None );
		if (isNone)
		{
			oV = util::SharedPtr<PyObject, PyObjectStorage, PyObjectCounter>();
		}
		else
		{
			oV = fromNakedToSharedPtrCast<PyObject>(iValue);
		}
		return 0;
	}
};



}
}

// EOF
