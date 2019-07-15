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

#if LASS_HAVE_STD_AUTO_PTR
/** auto_ptr assumes shadow types
 *  @ingroup Python
 */
template <typename T>
struct PyExportTraits< std::auto_ptr<T> >
{
	typedef impl::ShadowTraits<typename ShadoweeTraits<T>::TShadow> TShadowTraits; 
	static PyObject* build(std::auto_ptr<T>& v)
	{
		if (!v.get())
		{
			Py_RETURN_NONE;
		}
		return fromSharedPtrToNakedCast(TShadowTraits::buildObject(v));
	}
};
#endif

#if LASS_HAVE_STD_UNIQUE_PTR
/** auto_ptr assumes shadow types
*  @ingroup Python
*/
template <typename T>
struct PyExportTraits< std::unique_ptr<T> >
{
	typedef impl::ShadowTraits<typename ShadoweeTraits<T>::TShadow> TShadowTraits;
	static PyObject* build(std::unique_ptr<T>&& v)
	{
		if (!v.get())
		{
			Py_RETURN_NONE;
		}
		return fromSharedPtrToNakedCast(TShadowTraits::buildObject(std::move(v)));
	}
};
#endif

// --- void ptrs ------------------------------------------------------------------------------------

/** @ingroup Python
 *  @internal
 */
template <>
struct PyExportTraits<void*>
{
	LASS_PYTHON_DLL static PyObject* build(void* value);
	LASS_PYTHON_DLL static int get(PyObject* obj, void*& value);
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
	LASS_PYTHON_DLL static PyObject* build(bool v);
	LASS_PYTHON_DLL static int get(PyObject* obj, bool& v);
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
			return PyInt_FromLong(x);
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
	LASS_PYTHON_DLL static PyObject* build(signed PY_LONG_LONG v);
	LASS_PYTHON_DLL static int get(PyObject* obj, signed PY_LONG_LONG& v);
};

/** @ingroup Python
 */
template <>
struct PyExportTraits<unsigned PY_LONG_LONG>
{
	LASS_PYTHON_DLL static PyObject* build(unsigned PY_LONG_LONG v);
	LASS_PYTHON_DLL static int get(PyObject* obj, unsigned PY_LONG_LONG& v);
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
#ifdef Py_LIMITED_API
		const double x = PyFloat_AsDouble(obj);
		if (PyErr_Occurred())
		{
			return 1;
		}
		return impl::pyNumCast(x, v);
#else
		if (PyFloat_Check(obj))
		{
			return impl::pyNumCast(PyFloat_AS_DOUBLE(obj), v);
		}
#	if PY_MAJOR_VERSION < 3
		if (PyInt_Check(obj))
		{
			v = static_cast<Float>(PyInt_AS_LONG(obj));
			return 0;
		}
#	endif
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
#endif
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
		if (PyExportTraits<T>::get(obj, re) == 0)
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
	LASS_PYTHON_DLL static PyObject* build(const char* v);
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
	LASS_PYTHON_DLL static PyObject* build(const std::string& v);
	LASS_PYTHON_DLL static int get(PyObject* obj, std::string& v);
};


/** @ingroup Python
 */
template <>
struct PyExportTraits<const wchar_t*>
{
	LASS_PYTHON_DLL static PyObject* build(const wchar_t* v);
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
	LASS_PYTHON_DLL static PyObject* build(const std::wstring& v);
	LASS_PYTHON_DLL static int get(PyObject* obj, std::wstring& v);
};

}
}

#endif

// EOF
