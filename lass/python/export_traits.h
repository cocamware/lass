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
 *	Copyright (C) 2004-2011 the Initial Developer.
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

#ifndef LASS_GUARDIAN_OF_INCLUSION_PYTHON_EXPORT_TRAITS_H
#define LASS_GUARDIAN_OF_INCLUSION_PYTHON_EXPORT_TRAITS_H

#include "python_common.h"
#include "shadowee_traits.h"
#include "exception.h"
#include "pyobject_ptr.h"
#include "../num/num_cast.h"
#include "../util/wchar_support.h"

namespace lass
{
namespace python
{
namespace impl
{
	template <typename T> struct ShadowTraits;

	template <typename In, typename Out>
	int pyNumCast(In in, Out& out)
	{
		try
		{
			out = num::numCast<Out>(in);
		}
		catch (const std::exception& error)
		{
			PyErr_SetString(PyExc_TypeError, error.what());
			return 1;
		}
		return 0;
	}

	template <typename T>
	class NoNone
	{
	public:
		NoNone(const T& x = T()): value_(x) {}
		T& reference() { return value_; }
		const T& reference() const { return value_; }
	private:
		T value_;
	};
}

/** by copy, general case assumes shadow type or PyObjectPlus based type.
 *	@ingroup Python
 */
template <typename T> 
struct PyExportTraits
{
	typedef impl::ShadowTraits<typename ShadoweeTraits<T>::TShadow> TShadowTraits; 
	static PyObject* build(const T& v)
	{
		return fromSharedPtrToNakedCast(TShadowTraits::buildObject(v));
	}
	static int get(PyObject* obj, T& v)
	{
		return TShadowTraits::getObject(obj, v);
	}
};

/** constant objects can only be build.
 *	@ingroup Python
 */
template <typename T>
struct PyExportTraits<const T>
{
	static PyObject* build(const T& v)
	{
		return PyExportTraits<T>::build(v);
	}
};


// --- pointers ------------------------------------------------------------------------------------

template <typename T>
struct PyExportTraits< T* >
{
	typedef impl::ShadowTraits<typename ShadoweeTraits<T>::TShadow> TShadowTraits; 
	static PyObject* build(T* value)
	{
		if (!value)
		{
			Py_RETURN_NONE;
		}
		return fromSharedPtrToNakedCast(TShadowTraits::buildObject(value));
	}
	static int get(PyObject* obj, T*& value)
	{
		if (obj == Py_None)
		{
			value = 0;
			return 0;
		}
		return TShadowTraits::getObject(obj, value);
	}
};

/** SharedPtr assumes shadow types or PyObjectPlus types.
 *  @ingroup Python
 */
template <typename T, template <typename, typename> class S, typename C>
struct PyExportTraits< util::SharedPtr<T, S, C> >
{
	typedef impl::ShadowTraits<typename ShadoweeTraits<T>::TShadow> TShadowTraits; 
	typedef util::SharedPtr<T, S, C> TPtr;
	static PyObject* build(const TPtr& value)
	{
		if (!value)
		{
			Py_RETURN_NONE;
		}
		return fromSharedPtrToNakedCast(TShadowTraits::buildObject(value));
	}
	static int get(PyObject* obj, TPtr& value)
	{
		if (obj == Py_None)
		{
			value = TPtr();
			return 0;
		}
		return TShadowTraits::getObject(obj, value);
	}
};

/** @ingroup Python
 */
template <>
struct PyExportTraits<TPyObjPtr>
{
	static PyObject* build(const TPyObjPtr& v)
	{
		if (!v)
		{
			Py_RETURN_NONE;
		}
		return fromSharedPtrToNakedCast(v);
	}
	static int get(PyObject* obj, TPyObjPtr& v)
	{
		if (obj == Py_None)
		{
			v = TPyObjPtr();
		}
		v = fromNakedToSharedPtrCast<PyObject>(obj);
		return 0;
	}
};

/** @ingroup Python
 *  @internal
 */
/*
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
			if (!PyType_IsSubtype(iValue->ob_type , T::_lassPyClassDef.type() ))
			{
				PyErr_Format(PyExc_TypeError,"not castable to %s",T::_lassPyClassDef.name() );
				return 1;
			}
			oV = fromNakedToSharedPtrCast<T>(iValue);
		}
		return 0;
	}
};
*/

/** auto_ptr assumes shadow types
 *  @ingroup Python
 */
template <typename T>
struct PyExportTraits< std::auto_ptr<T> >
{
	typedef impl::ShadowTraits<typename ShadoweeTraits<T>::TShadow> TShadowTraits; 
	typedef typename ShadoweeTraits<T>::TPointerTraits::TPtr TPtr;
	static PyObject* build(std::auto_ptr<T>& v)
	{
		if (!v.get())
		{
			Py_RETURN_NONE;
		}
		return fromSharedPtrToNakedCast(TShadowTraits::buildObject(v));
	}
};

// --- void ptrs ------------------------------------------------------------------------------------

/** @ingroup Python
 *  @internal
 */
template <>
struct PyExportTraits<void*>
{
	static PyObject* build(void* value)
	{
		if (!value)
		{
			Py_RETURN_NONE;
		}
#if PY_VERSION_HEX < 0x03010000 // < 3.1
		return PyCObject_FromVoidPtr(value, 0);
#else
		return PyCapsule_New(value, 0, 0);
#endif
	}
	static int get(PyObject* obj, void*& value)
	{
		if (obj == Py_None)
		{
			value = 0;
			return 0;
		}
#if PY_VERSION_HEX < 0x03010000 // < 3.1
		if (!PyCObject_Check(obj))
		{
			PyErr_SetString(PyExc_TypeError, "does not evaluate to a void*");
			return 1;
		}
		value = PyCObject_AsVoidPtr(obj);
		return 0;
#else
		if (!PyCapsule_CheckExact(obj))
		{
			PyErr_SetString(PyExc_TypeError, "does not evaluate to a void*");
			return 1;
		}
		void* v = PyCapsule_GetPointer(obj, 0);
		if (!v)
		{
			return 1;
		}
		value = v;
		return 0;
#endif
	}
};

/** NoNone refuses None as value.
 *  @ingroup Python
 */
template <typename T>
struct PyExportTraits< impl::NoNone<T> >
{
	static int get(PyObject* obj, impl::NoNone<T>& value)
	{
		if (obj == Py_None)
		{
			PyErr_SetString(PyExc_TypeError, "argument must be not be None");
			return 1;
		}
		return PyExportTraits<T>::get(obj, value.reference());
	}
};


// --- booleans ------------------------------------------------------------------------------------

/** @ingroup Python
 *  @internal
 */
template <>
struct PyExportTraits<bool>
{
	static PyObject* build(bool v)
	{
		if (v)
		{
			Py_RETURN_TRUE;
		}
		else
		{
			Py_RETURN_FALSE;
		}
	}
	static int get(PyObject* obj, bool& v)
	{
		int result = PyObject_IsTrue(obj);
		if (result == -1)
		{
			PyErr_SetString(PyExc_TypeError, "does not evaluate to a boolean");
			return 1;
		}
		v = (result != 0);
		return 0;
	}
};


// --- signed integers -----------------------------------------------------------------------------

/** Helper class to create PyExportTraits for signed integers
 *  @ingroup Python
 */
template <typename Integer>
struct PyExportTraitsSigned
{
	LASS_META_ASSERT(sizeof(Integer) <= sizeof(long), integer_should_fit_in_long);
	static PyObject* build(Integer v)
	{
#if PY_MAJOR_VERSION < 3
		return PyInt_FromLong(v);
#else
		return PyLong_FromLong(v);
#endif
	}
	static int get(PyObject* obj, Integer& v)
	{
#if PY_MAJOR_VERSION < 3
		if (PyInt_Check(obj))
		{
			return impl::pyNumCast(PyInt_AS_LONG(obj), v);
		}
#endif
		if (PyLong_Check(obj))
		{
#if HAVE_LONG_LONG
			const PY_LONG_LONG x = PyLong_AsLongLong(obj);
#else
			const long x = PyLong_AsLong(obj);
#endif
			if (PyErr_Occurred())
			{
				PyErr_Format(PyExc_TypeError, "not a %s: overflow", num::NumTraits<Integer>::name().c_str());
				return 1;
			}
			return impl::pyNumCast(x, v);
		}
		PyErr_SetString(PyExc_TypeError, "not an integer");
		return 1;
	}
};

/** @ingroup Python
 */
template <>
struct PyExportTraits<signed char>: PyExportTraitsSigned<signed char>
{
};

/** @ingroup Python
 */
template <>
struct PyExportTraits<signed short>: PyExportTraitsSigned<signed short>
{
};

/** @ingroup Python
 */
template <>
struct PyExportTraits<signed int>: PyExportTraitsSigned<signed int>
{
};

/** @ingroup Python
 */
template <>
struct PyExportTraits<signed long>: PyExportTraitsSigned<signed long>
{
};



// --- unsigned integers ---------------------------------------------------------------------------

/** Helper class to create PyExportTraits for unsigned integers
 *  @ingroup Python
 */
template <typename Integer>
struct PyExportTraitsUnsigned
{
	LASS_META_ASSERT(sizeof(Integer) <= sizeof(unsigned long), integer_should_fit_in_unsigned_long);
	static PyObject* build(Integer v)
	{
#if PY_MAJOR_VERSION < 3
		const long x = static_cast<long>(v);
		if (x >= 0)
		{
			return PyInt_FromLong(v);
		}
#endif
		return PyLong_FromUnsignedLong(v);
	}
	static int get(PyObject* obj, Integer& v)
	{
#if PY_MAJOR_VERSION < 3
		if (PyInt_Check(obj))
		{
			const long x = PyInt_AS_LONG(obj);
			if (x < 0)
			{
				std::ostringstream buffer;
				buffer << "not a " << num::NumTraits<Integer>::name() << ": negative: " << x << " < 0";
				PyErr_SetString(PyExc_TypeError, buffer.str().c_str());
				return 1;
			}
			return impl::pyNumCast(static_cast<unsigned long>(x), v);
		}
#endif
		if (PyLong_Check(obj))
		{
#if HAVE_LONG_LONG
			const unsigned PY_LONG_LONG x = PyLong_AsUnsignedLongLong(obj);
#else
			const unsigned long x = PyLong_AsUnsignedLong(obj);
#endif
			if (PyErr_Occurred())
			{
				PyErr_Format(PyExc_TypeError, "not a %s: overflow", num::NumTraits<Integer>::name().c_str());
				return 1;
			}
			return impl::pyNumCast(x, v);
		}
		PyErr_SetString(PyExc_TypeError, "not an integer");
		return 1;
	}
};

/** @ingroup Python
 */
template <>
struct PyExportTraits<unsigned char>: PyExportTraitsUnsigned<unsigned char>
{
};

/** @ingroup Python
 */
template <>
struct PyExportTraits<unsigned short>: PyExportTraitsUnsigned<unsigned short>
{
};

/** @ingroup Python
 */
template <>
struct PyExportTraits<unsigned int>: PyExportTraitsUnsigned<unsigned int>
{
};

/** @ingroup Python
 */
template <>
struct PyExportTraits<unsigned long>: PyExportTraitsUnsigned<unsigned long>
{
};

// --- long long -----------------------------------------------------------------------------------

#ifdef HAVE_LONG_LONG

/** @ingroup Python
 */
template <>
struct PyExportTraits<signed PY_LONG_LONG>
{
	static PyObject* build(signed PY_LONG_LONG v)
	{
		return PyLong_FromLongLong(v);
	}
	static int get(PyObject* obj, signed PY_LONG_LONG& v)
	{
#if PY_MAJOR_VERSION < 3
		if (PyInt_Check(obj))
		{
			v = PyInt_AS_LONG(obj);
			return 0;
		}
#endif
		if (PyLong_Check(obj))
		{
			v = PyLong_AsLongLong(obj);
			return 0;
		}
		PyErr_SetString(PyExc_TypeError, "not an integer");
		return 1;		
	}
};

/** @ingroup Python
 */
template <>
struct PyExportTraits<unsigned PY_LONG_LONG>
{
	static PyObject* build(unsigned PY_LONG_LONG v)
	{
		return PyLong_FromUnsignedLongLong(v);
	}
	static int get(PyObject* obj, unsigned PY_LONG_LONG& v)
	{
#if PY_MAJOR_VERSION < 3
		if (PyInt_Check(obj))
		{
			const long x = PyInt_AS_LONG(obj);
			if (x < 0)
			{
				std::ostringstream buffer;
				buffer << "not a unsigned long long: negative: " << x << " < 0";
				PyErr_SetString(PyExc_TypeError, buffer.str().c_str());
				return 1;
			}
			v = x;
			return 0;
		}
#endif
		if (PyLong_Check(obj))
		{
			v = PyLong_AsUnsignedLongLong(obj);
			return 0;
		}
		PyErr_SetString(PyExc_TypeError, "not an integer");
		return 1;	
	}
};

#endif



// --- floating point numbers ----------------------------------------------------------------------

/** Helper class to create PyExportTraits for floating point numbers
 *  @ingroup Python
 */
template <typename Float>
struct PyExportTraitsFloat
{
	static PyObject* build(Float v)
	{
		return PyFloat_FromDouble(v);
	}
	static int get(PyObject* obj, Float& v)
	{
		if (PyFloat_Check(obj))
		{
			return impl::pyNumCast(PyFloat_AS_DOUBLE(obj), v);
		}
#if PY_MAJOR_VERSION < 3
		if (PyInt_Check(obj))
		{
			v = static_cast<Float>(PyInt_AS_LONG(obj));
			return 0;
		}
#endif
		if (PyLong_Check(obj))
		{
			const double x = PyLong_AsDouble(obj);
			if (PyErr_Occurred())
			{
				PyErr_Format(PyExc_TypeError, "not a %s: overflow", num::NumTraits<Float>::name().c_str());
				return 1;
			}
			return impl::pyNumCast(x, v);
		}
		PyErr_SetString(PyExc_TypeError, "not a float or integer");
		return 1;
	}
};

/** @ingroup Python
 */
template <>
struct PyExportTraits<float>: PyExportTraitsFloat<float>
{
};

/** @ingroup Python
 */
template <>
struct PyExportTraits<double>: PyExportTraitsFloat<double>
{
};

/** @ingroup Python
 */
template <>
struct PyExportTraits<long double>: PyExportTraitsFloat<long double>
{
};



// --- complex numbers -----------------------------------------------------------------------------

/** @ingroup Python
 */
template <typename T>
struct PyExportTraits< std::complex<T> >
{
	static PyObject* build(const std::complex<T>& v)
	{
		return PyComplex_FromDoubles(
			static_cast<double>(v.real()), 
			static_cast<double>(v.imag()));
	}
	static int get(PyObject* obj, std::complex<T>& v)
	{
		T re, im;
		if (pyGetSimpleObject(obj, re) == 0)
		{
			v = std::complex<T>(re, 0);
			return 0;
		}
		PyErr_Clear();
		if (!PyComplex_Check(obj))
		{
			PyErr_SetString(PyExc_TypeError, "not a complex number");
			return 1;
		}
		if (impl::pyNumCast(PyComplex_RealAsDouble(obj), re) != 0)
		{
			return 1;
		}
		if (impl::pyNumCast(PyComplex_ImagAsDouble(obj), im) != 0)
		{
			return 1;
		}
		v = std::complex<T>(re, im);
		return 0;
	}
};



// --- strings -------------------------------------------------------------------------------------

/** @ingroup Python
 */
template <>
struct PyExportTraits<const char*>
{
	static PyObject* build(const char* v)
	{
#if PY_MAJOR_VERSION < 3
		return PyString_FromString(v);
#else
		return PyUnicode_FromString(v);
#endif
	}
};


/** @ingroup Python
 */
template <size_t N>
struct PyExportTraits<const char [N]>: PyExportTraits<const char*>
{
};

/** @ingroup Python
 */
template <size_t N>
struct PyExportTraits<char [N]>: PyExportTraits<const char*>
{
};

/** @ingroup Python
 */
template <>
struct PyExportTraits<std::string>
{
	static PyObject* build(const std::string& v)
	{
		return PyExportTraits<std::string::const_pointer>::build(v.c_str());
	}
	static int get(PyObject* obj, std::string& v)
	{
#if PY_MAJOR_VERSION < 3
		if (PyString_Check(obj))
		{
			// explicitly copying into data as some the std::string(char*) assumes a zero terminated C-str
			// which is too limiting
			v.resize( PyString_GET_SIZE(obj), '\0' );
			memcpy(&v[0], PyString_AS_STRING(obj), v.size());
			return 0;
		}
#endif
		if (!PyUnicode_Check(obj))
		{
			PyErr_SetString(PyExc_TypeError, "not a string");
			return 1;
		}
		TPyObjPtr s(PyUnicode_AsUTF8String(obj));
		if (!s)
		{
			return 1;
		}
#if PY_MAJOR_VERSION < 3
		v.resize( PyString_GET_SIZE(s.get()), '\0' );
		memcpy(&v[0], PyString_AS_STRING(s.get()), v.size());
#else
		v.resize( PyBytes_GET_SIZE(s.get()), '\0' );
		memcpy(&v[0], PyBytes_AS_STRING(s.get()), v.size());
#endif
		return 0;
	}
};


/** @ingroup Python
 */
template <>
struct PyExportTraits<const wchar_t*>
{
	static PyObject* build(const wchar_t* v)
	{
		return PyUnicode_FromWideChar(v, wcslen(v) );
	}
};


/** @ingroup Python
 */
template <size_t N>
struct PyExportTraits<const wchar_t [N]>: PyExportTraits<const wchar_t*>
{
};

/** @ingroup Python
 */
template <size_t N>
struct PyExportTraits<wchar_t [N]>: PyExportTraits<const wchar_t*>
{
};

/** @ingroup Python
 */
template <>
struct PyExportTraits<std::wstring>
{
	static PyObject* build(const std::wstring& v)
	{
		return PyUnicode_FromWideChar( v.data(), v.length() );
	}
	static int get(PyObject* obj, std::wstring& v)
	{
#if PY_MAJOR_VERSION < 3
#	if LASS_HAVE_WCHAR_SUPPORT
		if (PyString_Check(obj))
		{
			std::string utf8;
			if ( PyExportTraits<std::string>::get( obj, utf8 ) != 0 )
			{
				return 1;
			}
			v = util::utf8ToWchar( utf8 );
			return 0;
		}
#	endif
#endif
		if (!PyUnicode_Check(obj))
		{
			PyErr_SetString(PyExc_TypeError, "not a string");
			return 1;
		}
#if PY_VERSION_HEX < 0x03020000 // < 3.2
		Py_ssize_t n = PyUnicode_GET_SIZE(obj); // assumes sizeof(Py_UNICODE) == sizeof(wchar_t)
#else
		Py_ssize_t n = PyUnicode_AsWideChar(obj, 0, 0) - 1; // takes care of UTF-16 and UTF-32 conversions
#endif
		LASS_ASSERT(n >= 0);
		if (n == 0)
		{
			v = std::wstring();
			return 0;
		}
		std::wstring tmp(static_cast<size_t>(n), '\0');
		if (PyUnicode_AsWideChar(obj, &tmp[0], n) == -1)
		{
			return 1;
		}
		v.swap(tmp);
		return 0;
	}
};


// --- pairs ---------------------------------------------------------------------------------------

/** @ingroup Python
 *  std::pair translates to a tuple by copy.
 */
template <typename T1, typename T2>
struct PyExportTraits< std::pair<T1, T2> >
{
	static PyObject* build(const std::pair<T1, T2>& v)
	{
		return fromSharedPtrToNakedCast(makeTuple(v.first, v.second));
	}
	static int get(PyObject* obj, std::pair<T1, T2>& v)
	{
		if (decodeTuple(obj, v.first, v.second) != 0)
		{
			impl::addMessageHeader("pair");
			return 1;
		}
		return 0;
	}
};

}
}

#endif

// EOF
