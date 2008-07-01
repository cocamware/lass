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

namespace lass
{
namespace python
{

/** @ingroup Python
 */
inline int pyGetSimpleObject_deprecated( PyObject* iValue, bool& oV )
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

/** @ingroup Python
 */
inline int pyGetSimpleObject_deprecated( PyObject* iValue, signed char& oV )
{
	return impl::pyGetSignedObject( iValue, oV );
}

/** @ingroup Python
 */
inline int pyGetSimpleObject_deprecated( PyObject* iValue, unsigned char& oV )
{
	return impl::pyGetUnsignedObject( iValue, oV );
}

/** @ingroup Python
	*/
inline int pyGetSimpleObject_deprecated( PyObject* iValue, signed short& oV )
{
	return impl::pyGetSignedObject( iValue, oV );
}

/** @ingroup Python
 */
inline int pyGetSimpleObject_deprecated( PyObject* iValue, unsigned short& oV )
{
	return impl::pyGetUnsignedObject( iValue, oV );
}

/** @ingroup Python
 */
inline int pyGetSimpleObject_deprecated( PyObject* iValue, signed int& oV )
{
	return impl::pyGetSignedObject( iValue, oV );
}

/** @ingroup Python
 */
inline int pyGetSimpleObject_deprecated( PyObject* iValue, unsigned int& oV )
{
	return impl::pyGetUnsignedObject( iValue, oV );
}

/** @ingroup Python
	*/
inline int pyGetSimpleObject_deprecated( PyObject* iValue, signed long& oV )
{
	return impl::pyGetSignedObject( iValue, oV );
}

/** @ingroup Python
 */
inline int pyGetSimpleObject_deprecated( PyObject* iValue, unsigned long& oV )
{
	return impl::pyGetUnsignedObject( iValue, oV );
}

#ifdef HAVE_LONG_LONG

/** @ingroup Python
	*/
inline int pyGetSimpleObject_deprecated( PyObject* iValue, signed PY_LONG_LONG& oV )
{
	return impl::pyGetSignedObject( iValue, oV );
}

/** @ingroup Python
 */
inline int pyGetSimpleObject_deprecated( PyObject* iValue, unsigned PY_LONG_LONG& oV )
{
	return impl::pyGetUnsignedObject( iValue, oV );
}

#endif

/** @ingroup Python
 */
inline int pyGetSimpleObject_deprecated( PyObject* iValue, float& oV )
{
	return impl::pyGetFloatObject( iValue, oV );
}

/** @ingroup Python
 */
inline int pyGetSimpleObject_deprecated( PyObject* iValue, double& oV )
{
	return impl::pyGetFloatObject( iValue, oV );
}

/** @ingroup Python
 */
inline int pyGetSimpleObject_deprecated( PyObject* iValue, long double& oV )
{
	return impl::pyGetFloatObject( iValue, oV );
}

/** @ingroup Python
 *  @deprecated
 */
/*
inline int pyGetSimpleObject( PyObject* iValue, PyObject*& oV )
{
	if ( iValue == Py_None )
		oV = NULL;
	else
	{
		oV = iValue;
		Py_INCREF( oV );
	}
	return 0;
}
*/


/** @ingroup Python
 */
template<class C>
int pyGetSimpleObject_deprecated(PyObject* iValue, util::SharedPtr<C, PyObjectStorage, PyObjectCounter>& oV)
{
	const bool isNone = (iValue == Py_None );
	if (isNone)
		oV = util::SharedPtr<C, PyObjectStorage, PyObjectCounter>();
	else
	{
		if (!PyType_IsSubtype(iValue->ob_type , &C::_lassPyType ))
		{
			PyErr_Format(PyExc_TypeError,"not castable to %s",C::_lassPyType.tp_name);
			return 1;
		}
		oV = fromNakedToSharedPtrCast<C>(iValue);
	}
	return 0;
}

/** @ingroup Python
 */
inline int pyGetSimpleObject_deprecated(
		PyObject* iValue, util::SharedPtr<PyObject, PyObjectStorage, PyObjectCounter>& oV)
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

/** @ingroup Python
 @deprecated
 */
/*
inline PyObject* pyBuildSimpleObject( PyObject* iV )
{
	if (iV==NULL)
	{
		Py_INCREF( Py_None );
		return Py_None;
	}
	Py_INCREF(iV);
	return iV;
}
*/

/** @ingroup Python
 */
template<class C>
PyObject* pyBuildSimpleObject_deprecated( const util::SharedPtr<C, PyObjectStorage, PyObjectCounter>& iV )
{
	if (!iV)
	{
		Py_INCREF( Py_None );
		return Py_None;
	}
	return fromSharedPtrToNakedCast(iV);
}

/** @ingroup Python
 */
inline PyObject* pyBuildSimpleObject_deprecated( bool iV )
{
	return PyInt_FromLong(static_cast<long>(iV));
}

/** @ingroup Python
 */
inline PyObject* pyBuildSimpleObject_deprecated( signed char iV )
{
	return PyInt_FromLong(static_cast<long>(iV));
}

/** @ingroup Python
 */
inline PyObject* pyBuildSimpleObject_deprecated( unsigned char iV )
{
	return PyInt_FromLong(static_cast<long>(iV));
}

/** @ingroup Python
 */
inline PyObject* pyBuildSimpleObject_deprecated( signed short iV )
{
	return PyInt_FromLong(static_cast<long>(iV));
}

/** @ingroup Python
 */
inline PyObject* pyBuildSimpleObject_deprecated( unsigned short iV )
{
	return PyInt_FromLong(static_cast<long>(iV));
}

/** @ingroup Python
 */
inline PyObject* pyBuildSimpleObject_deprecated( signed int iV )
{
	return PyInt_FromLong(static_cast<long>(iV));
}

/** @ingroup Python
 */
inline PyObject* pyBuildSimpleObject_deprecated( unsigned int iV )
{
	if (iV <= static_cast<unsigned int>(num::NumTraits<long>::max))
	{
		return PyInt_FromLong(static_cast<long>(iV));
	}
	else
	{
		return PyLong_FromUnsignedLong(static_cast<unsigned long>(iV));
	}
}

/** @ingroup Python
 */
inline PyObject* pyBuildSimpleObject_deprecated( signed long iV )
{
	return PyInt_FromLong(static_cast<long>(iV));
}

/** @ingroup Python
 */
inline PyObject* pyBuildSimpleObject_deprecated( unsigned long iV )
{
	if (iV <= static_cast<unsigned int>(num::NumTraits<long>::max))
	{
		return PyInt_FromLong(static_cast<long>(iV));
	}
	else
	{
		return PyLong_FromUnsignedLong(static_cast<unsigned long>(iV));
	}
}

#if HAVE_LONG_LONG

/** @ingroup Python
 */
inline PyObject* pyBuildSimpleObject_deprecated( signed PY_LONG_LONG iV )
{
	return PyLong_FromLongLong(iV);
}

/** @ingroup Python
 */
inline PyObject* pyBuildSimpleObject_deprecated( unsigned PY_LONG_LONG iV )
{
	return PyLong_FromUnsignedLongLong(iV);
}

#endif

/** @ingroup Python
 */
inline PyObject* pyBuildSimpleObject_deprecated( float iV )
{
	return PyFloat_FromDouble(static_cast<double>(iV));
}

/** @ingroup Python
 */
inline PyObject* pyBuildSimpleObject_deprecated( double iV )
{
	return PyFloat_FromDouble(iV);
}

/** @ingroup Python
 */
inline PyObject* pyBuildSimpleObject_deprecated( long double iV )
{
	return PyFloat_FromDouble(static_cast<long double>(iV));
}

/** @ingroup Python
 */
inline PyObject* pyBuildSimpleObject_deprecated( const char* iV )
{
	return PyString_FromString(iV);
}

PYEXPORTTRAITS_USINGDEPRECATED( bool )
PYEXPORTTRAITS_USINGDEPRECATED( signed int )
PYEXPORTTRAITS_USINGDEPRECATED( unsigned int )
PYEXPORTTRAITS_USINGDEPRECATED( signed char )
PYEXPORTTRAITS_USINGDEPRECATED( unsigned char )
PYEXPORTTRAITS_USINGDEPRECATED( signed short )
PYEXPORTTRAITS_USINGDEPRECATED( unsigned short)
PYEXPORTTRAITS_USINGDEPRECATED( signed long )
PYEXPORTTRAITS_USINGDEPRECATED( unsigned long )
#if HAVE_LONG_LONG
PYEXPORTTRAITS_USINGDEPRECATED( signed PY_LONG_LONG )
PYEXPORTTRAITS_USINGDEPRECATED( unsigned PY_LONG_LONG)
#endif
PYEXPORTTRAITS_USINGDEPRECATED( float )
PYEXPORTTRAITS_USINGDEPRECATED( double )
PYEXPORTTRAITS_USINGDEPRECATED( long double )

template<>
struct PyExportTraits< const char* >
{
	static PyObject* build(const char* iv) { return pyBuildSimpleObject_deprecated(iv); }
};
/*
template<>
struct PyExportTraits< size_t >
{
	static PyObject* build(const size_t& iv) { return pyBuildSimpleObject_deprecated(iv); }
	static int get(PyObject* iV, size_t& ov) { return pyGetSimpleObject_deprecated(iV,ov); }
};
*/
template<typename T>
struct PyExportTraits< util::SharedPtr<T,PyObjectStorage,PyObjectCounter> >
{
	typedef util::SharedPtr<T,PyObjectStorage,PyObjectCounter> TShrd;
	static PyObject* build(const TShrd& iv) { return pyBuildSimpleObject_deprecated(iv); }
	static int get(PyObject* iV, TShrd& ov) { return pyGetSimpleObject_deprecated(iV,ov); }
};
/*
template<>
struct PyExportTraits< util::SharedPtr<PyObject,PyObjectStorage,PyObjectCounter> >
{
	typedef util::SharedPtr<PyObject,PyObjectStorage,PyObjectCounter> TShrd;
	static PyObject* build(const TShrd& iv) { return pyBuildSimpleObject_deprecated(iv); }
	static int get(PyObject* iV, TShrd& ov) { return pyGetSimpleObject_deprecated(iV,ov); }
};
*/


// --- impl ----------------------------------------------------------------------------------------

namespace impl
{

/** @internal
 */
template <PyCFunction DispatcherAddress>
PyObject* unaryDispatcher(PyObject* iSelf)
{
	TPyObjPtr args(PyTuple_New(0));
	return DispatcherAddress(iSelf, args.get());
}

/** @internal
*/
template <PyCFunction DispatcherAddress>
PyObject* binaryDispatcher(PyObject* iSelf, PyObject* iOther)
{
	TPyObjPtr args(Py_BuildValue("(O)", iOther));
	return DispatcherAddress(iSelf, args.get());
}

/** @internal
*/
template <PyCFunction DispatcherAddress>
PyObject* ternaryDispatcher(PyObject* iSelf, PyObject* iArgs, PyObject* iKw)
{
	if (iKw)
	{
		PyErr_SetString(PyExc_TypeError, "keyword arguments are not supported");
		return 0;
	}
	return DispatcherAddress(iSelf, iArgs);
}

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

/** @internal
 */
template <typename CppClass>
inline void injectClassInModule(PyObject* iModule, const char* iClassDocumentation)
{
	std::pair< std::string, PyObject* > classForModule = prepareClassForModuleInjection<CppClass>(PyModule_GetName(iModule),iClassDocumentation);
	PyModule_AddObject(iModule, const_cast<char*>(classForModule.first.c_str()), classForModule.second);
}



/** @internal
 *//*
template <typename CppClass>
void addClassMethod(
		const char* iMethodName, const char* iDocumentation, PyCFunction iMethodDispatcher,
		unaryfunc iUnaryDispatcher, binaryfunc iBinaryDispatcher, ternaryfunc iTernaryDispatcher, 
		OverloadLink& oOverloadChain) 
{
	if (strcmp(iMethodName, "__call__") == 0)
	{
		oOverloadChain.setTernaryfunc(CppClass::Type.tp_call);
		CppClass::Type.tp_call = iTernaryDispatcher;
	}
	else if (strcmp(iMethodName, "__neg__") == 0)
	{
		if (CppClass::Type.tp_as_number == 0)
		{
			CppClass::Type.tp_as_number = new PyNumberMethods;
		}
		CppClass::Type.tp_as_number->nb_negative = iUnaryDispatcher;
	}
	else
	{
		::std::vector<PyMethodDef>::iterator i = ::std::find_if(
			CppClass::_lassPyMethods.begin(), CppClass::_lassPyMethods.end(), PyMethodEqual(iMethodName));
		if (i == CppClass::_lassPyMethods.end())
		{
			CppClass::_lassPyMethods.insert(CppClass::_lassPyMethods.begin(), createPyMethodDef(
				iMethodName, iMethodDispatcher, METH_VARARGS , iDocumentation));
			oOverloadChain.setNull();
		}
		else
		{
			LASS_ASSERT(i->ml_flags == METH_VARARGS);
			oOverloadChain.setPyCFunction(i->ml_meth);
			i->ml_meth = iMethodDispatcher;
			if (i->ml_doc == 0)
			{
				i->ml_doc = const_cast<char*>(iDocumentation);
			}
		};
	}
}
*/


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
		CppClass::_lassPyStatics.push_back(createStaticMember(iMethodName, iDocumentation, descr));
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
	CppClass::_lassPyStatics.push_back(createStaticMember(iName, 0, pyExportTraitBuild(iValue)));
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
		iInnerClassName, iDocumentation, reinterpret_cast<PyObject*>(&InnerCppClass::_lassPyType),
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
	if (PyInt_Check(iValue))
	{
		long temp = PyInt_AS_LONG(iValue);
		return pyNumericCast( temp, oV );
	}
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
	if (PyInt_Check(iValue))
	{
		long temp = PyInt_AS_LONG(iValue);
		oV = static_cast<Float>( temp );
		return 0;
	}
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
}
}

// EOF
