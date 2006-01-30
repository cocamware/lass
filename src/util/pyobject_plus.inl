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
 */

namespace lass
{
namespace python
{

/** @ingroup Python
	*/
inline int pyGetSimpleObject( PyObject* iValue, bool& oV )
{
	int result = PyObject_IsTrue(iValue);
	if (result == -1)
	{
		PyErr_SetString(PyExc_TypeError, LASS_PYTHON_ERR_MSG_ARG_NOT_BOOL);
		return 1;
	}
	oV = (result != 0);
	return 0;
}

/** @ingroup Python
	*/
inline int pyGetSimpleObject( PyObject* iValue, signed char& oV )
{
	return impl::pyGetSignedObject( iValue, oV );
}

/** @ingroup Python
	*/
inline int pyGetSimpleObject( PyObject* iValue, unsigned char& oV )
{
	return impl::pyGetUnsignedObject( iValue, oV );
}

/** @ingroup Python
	*/
inline int pyGetSimpleObject( PyObject* iValue, signed short& oV )
{
	return impl::pyGetSignedObject( iValue, oV );
}

/** @ingroup Python
	*/
inline int pyGetSimpleObject( PyObject* iValue, unsigned short& oV )
{
	return impl::pyGetUnsignedObject( iValue, oV );
}

/** @ingroup Python
	*/
inline int pyGetSimpleObject( PyObject* iValue, signed int& oV )
{
	return impl::pyGetSignedObject( iValue, oV );
}

/** @ingroup Python
	*/
inline int pyGetSimpleObject( PyObject* iValue, unsigned int& oV )
{
	return impl::pyGetUnsignedObject( iValue, oV );
}

/** @ingroup Python
	*/
inline int pyGetSimpleObject( PyObject* iValue, signed long& oV )
{
	return impl::pyGetSignedObject( iValue, oV );
}

/** @ingroup Python
	*/
inline int pyGetSimpleObject( PyObject* iValue, unsigned long& oV )
{
	return impl::pyGetUnsignedObject( iValue, oV );
}

/** @ingroup Python
	*/
inline int pyGetSimpleObject( PyObject* iValue, float& oV )
{
	return impl::pyGetFloatObject( iValue, oV );
}

/** @ingroup Python
	*/
inline int pyGetSimpleObject( PyObject* iValue, double& oV )
{
	return impl::pyGetFloatObject( iValue, oV );
}

/** @ingroup Python
	*/
inline int pyGetSimpleObject( PyObject* iValue, long double& oV )
{
	return impl::pyGetFloatObject( iValue, oV );
}

/** @ingroup Python
	*/
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

/** @ingroup Python
	*/
template<class C>
int pyGetSimpleObject( PyObject* iValue,
						util::SharedPtr<C, PyObjectStorage, PyObjectCounter>& oV )
{
	const bool isNone = (iValue == Py_None );
	if (isNone)
		oV = util::SharedPtr<C, PyObjectStorage, PyObjectCounter>();
	else
	{
		if (!PyType_IsSubtype(iValue->ob_type , &C::Type ))
		{
			PyErr_Format(PyExc_TypeError,"not castable to %s",C::Type.tp_name);
			return 1;
		}
		oV = fromPySharedPtrCast<C>(iValue);
	}
	return 0;
}

/** @ingroup Python
	*/
inline int pyGetSimpleObject( PyObject* iValue,
								util::SharedPtr<PyObject, PyObjectStorage, PyObjectCounter>& oV )
{
	const bool isNone = (iValue == Py_None );
	if (isNone)
	{
		oV = util::SharedPtr<PyObject, PyObjectStorage, PyObjectCounter>();
	}
	else
	{
		oV = fromPySharedPtrCast<PyObject>(iValue);
	}
	return 0;
}

/** @ingroup Python
	*/
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

/** @ingroup Python
	*/
template<class C>
PyObject* pyBuildSimpleObject( const util::SharedPtr<C, PyObjectStorage, PyObjectCounter>& iV )
{
	return toPySharedPtrCast(iV);
}

/** @ingroup Python
	*/
inline PyObject* pyBuildSimpleObject( bool iV )
{
	return PyInt_FromLong(static_cast<long>(iV));
}

/** @ingroup Python
	*/
inline PyObject* pyBuildSimpleObject( signed char iV )
{
	return PyInt_FromLong(static_cast<long>(iV));
}

/** @ingroup Python
	*/
inline PyObject* pyBuildSimpleObject( unsigned char iV )
{
	return PyInt_FromLong(static_cast<long>(iV));
}

/** @ingroup Python
	*/
inline PyObject* pyBuildSimpleObject( signed short iV )
{
	return PyInt_FromLong(static_cast<long>(iV));
}

/** @ingroup Python
	*/
inline PyObject* pyBuildSimpleObject( unsigned short iV )
{
	return PyInt_FromLong(static_cast<long>(iV));
}

/** @ingroup Python
	*/
inline PyObject* pyBuildSimpleObject( signed int iV )
{
	return PyInt_FromLong(static_cast<long>(iV));
}

/** @ingroup Python
	*/
inline PyObject* pyBuildSimpleObject( unsigned int iV )
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
inline PyObject* pyBuildSimpleObject( signed long iV )
{
	return PyInt_FromLong(static_cast<long>(iV));
}

/** @ingroup Python
	*/
inline PyObject* pyBuildSimpleObject( unsigned long iV )
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
inline PyObject* pyBuildSimpleObject( float iV )
{
	return PyFloat_FromDouble(static_cast<double>(iV));
}

/** @ingroup Python
	*/
inline PyObject* pyBuildSimpleObject( double iV )
{
	return PyFloat_FromDouble(iV);
}

/** @ingroup Python
	*/
inline PyObject* pyBuildSimpleObject( long double iV )
{
	return PyFloat_FromDouble(static_cast<long double>(iV));
}

/** @ingroup Python
	*/
inline PyObject* pyBuildSimpleObject( const char* iV )
{
	return PyString_FromString(iV);
}


/** @ingroup Python
 *  get value of a python object by the name of the python object rather than the pointer
 */
template <typename T>
int pyGetSimpleObjectByName(const std::string& iName, T& oV)
{
	PyObject* module = PyImport_AddModule("__main__");
	PyObject* dict = PyModule_GetDict(module);
	PyObject* temp = PyDict_GetItemString(dict, iName.c_str());
	return python::pyGetSimpleObject(temp, oV);
}


// --- impl ----------------------------------------------------------------------------------------

namespace impl
{

/** @internal
 */
template <typename CppClass>
inline void injectClassInModule(PyObject* iModule, const char* iClassDocumentation)
{
	char* shortName = CppClass::Type.tp_name; // finalizePyType will expand tp_name with module name.
	finalizePyType(CppClass::Type, *CppClass::GetParentType(), CppClass::Methods, CppClass::GetSetters,
		CppClass::Statics, PyModule_GetName(iModule), iClassDocumentation);
	PyModule_AddObject(iModule, shortName, reinterpret_cast<PyObject*>(&CppClass::Type));
}



/** @internal
 */
template <typename CppClass>
void addClassMethod(char* iMethodName, char* iDocumentation, PyCFunction iMethodDispatcher,
					PyCFunction& oOverloadChain)
{
	::std::vector<PyMethodDef>::iterator i = ::std::find_if(
		CppClass::Methods.begin(), CppClass::Methods.end(),	PyMethodEqual(iMethodName));
	if (i == CppClass::Methods.end())
	{
		CppClass::Methods.insert(CppClass::Methods.begin(), createPyMethodDef(
			iMethodName, iMethodDispatcher, METH_VARARGS , iDocumentation));
		oOverloadChain = 0;
	}
	else
	{
		oOverloadChain = i->ml_meth;
		i->ml_meth = iMethodDispatcher;
	};
}



/** @internal
 */
template <typename CppClass, typename T>
inline void addClassStaticConst(const char* iName, const T& iValue)
{
	StaticMember temp;
	temp.name = iName;
	temp.object = pyBuildSimpleObject(iValue);
	temp.parentType = 0;
	temp.methods = 0;
	temp.getSetters = 0;
	temp.statics = 0;
	temp.doc = 0;
	CppClass::Statics.push_back(temp);
}



/** @internal
 */
template <typename InnerCppClass>
inline void addClassInnerClass(std::vector<StaticMember>& oOuterStatics, 
							const char* iInnerClassName, const char* iDocumentation)
{
	StaticMember temp;
	temp.name = iInnerClassName;
	temp.object = reinterpret_cast<PyObject*>(&InnerCppClass::Type);
	temp.parentType = InnerCppClass::GetParentType();
	temp.methods = &InnerCppClass::Methods;
	temp.getSetters = &InnerCppClass::GetSetters;
	temp.statics = &InnerCppClass::Statics;
	temp.doc = iDocumentation;
	oOuterStatics.push_back(temp);
}




/** @internal
 *  helper for pyNumericCast
 */
template <bool InIsSigned> 
struct PyNumericCaster
{
	template <typename In, typename Out> static int cast( In iIn, Out& oOut )
	{
		//LASS_COUT << ">>>> PyNumericCaster<" << typeid(In).name() << ", " << typeid(Out).name() << ">\n";

		LASS_ASSERT(num::NumTraits<Out>::isSigned == true);
		LASS_ASSERT(num::NumTraits<In>::min <= num::NumTraits<Out>::min);
		if (iIn < static_cast<In>(num::NumTraits<Out>::min))
		{
			std::ostringstream buffer;
			buffer << "not a " << num::NumTraits<Out>::name() << ": underflow: "
				<< iIn << " < " << num::NumTraits<Out>::min;
			PyErr_SetString(PyExc_TypeError, buffer.str().c_str());
			return 1;
		}
		LASS_ASSERT(num::NumTraits<In>::max >= num::NumTraits<Out>::max);
		const In maxIn = static_cast<In>(num::NumTraits<Out>::max);
		const bool isOverflow = (iIn > maxIn);
		//LASS_COUT << iIn << " > " << maxIn << "? "
		//	<< (isOverflow) << " " << (iIn > maxIn) << "\n";
		if (isOverflow)
		{
			std::ostringstream buffer;
			buffer << "not a " << num::NumTraits<Out>::name() << ": overflow: "
				<< iIn << " > " << num::NumTraits<Out>::max;
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
		LASS_ASSERT(num::NumTraits<In>::max >= num::NumTraits<Out>::max);
		if (iIn > static_cast<In>(num::NumTraits<Out>::max))
		{
			std::ostringstream buffer;
			buffer << "not a " << num::NumTraits<Out>::name() << ": overflow: "
				<< iIn << " > " << num::NumTraits<Out>::max;
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
		long temp = PyLong_AsLong(iValue);
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
		unsigned long temp = PyLong_AsUnsignedLong(iValue);
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
