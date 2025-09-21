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
 *	Copyright (C) 2004-2025 the Initial Developer.
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
#include "no_none.h"
#include "maybe_none.h"
#include "self.h"
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
	using NoNone [[deprecated("lass::python::NoNone<T> instead.")]] = lass::python::NoNone<T>;

	LASS_PYTHON_DLL PyObject* buildStringImpl(const char* s, size_t n);
	LASS_PYTHON_DLL PyObject* buildStringImpl(const wchar_t* s, size_t n);
#if LASS_HAVE_STD_U8STRING
#if __cpp_lib_char8_t
	LASS_PYTHON_DLL PyObject* buildStringImpl(const char8_t* s, size_t n);
#endif
#endif
	LASS_PYTHON_DLL PyObject* buildStringImpl(const char16_t* s, size_t n);
	LASS_PYTHON_DLL PyObject* buildStringImpl(const char32_t* s, size_t n);
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
	constexpr static const char* py_typing = "T | None";

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
	constexpr static const char* py_typing = "Any";

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

/** std::unique_ptr assumes shadow types
*  @ingroup Python
*/
template <typename T, typename Deleter>
struct PyExportTraits< std::unique_ptr<T, Deleter> >
{
	constexpr static const char* py_typing = "T | None";

	typedef impl::ShadowTraits<typename ShadoweeTraits<T>::TShadow> TShadowTraits;
	static PyObject* build(std::unique_ptr<T, Deleter>&& value)
	{
		if (!value)
		{
			Py_RETURN_NONE;
		}
		return fromSharedPtrToNakedCast(TShadowTraits::buildObject(std::move(value)));
	}
};

/** std::shared_ptr assumes shadow types.
 *  @ingroup Python
 */
template <typename T>
struct PyExportTraits< std::shared_ptr<T> >
{
	constexpr static const char* py_typing = "T | None";

	typedef impl::ShadowTraits<typename ShadoweeTraits<T>::TShadow> TShadowTraits;
	typedef std::shared_ptr<T> TPtr;
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
			value.reset();
			return 0;
		}
		return TShadowTraits::getObject(obj, value);
	}
};

// --- void ptrs ------------------------------------------------------------------------------------

/** @ingroup Python
 *  @internal
 */
template <>
struct PyExportTraits<void*>
{
	constexpr static const char* py_typing = "Any"; // should be CapsuleType | None instead?

	LASS_PYTHON_DLL static PyObject* build(void* value);
	LASS_PYTHON_DLL static int get(PyObject* obj, void*& value);
};



/** @ingroup Python
 *  @internal
 */
template <>
struct PyExportTraits<std::nullptr_t>
{
	constexpr static const char* py_typing = "None";

	LASS_PYTHON_DLL static PyObject* build(std::nullptr_t value);
	LASS_PYTHON_DLL static int get(PyObject* obj, std::nullptr_t& value);
};



// --- NoNone --------------------------------------------------------------------------------------

/** Helper class to create PyExportTraits for NoNone wrapped types.
 *
 *  NoNone wrapped types are used to ensure that:
 *  - No `None` values can be passed from Python to C++ (which would be translated to a `nullptr`),
 *  - No `nullptr` values can be returned to Python (which would be translated to a `None`).
 *
 *  Use this helper to create PyExportTraits for your own NoNone wrapped types by inheriting from it.
 *
 *  ```
 *  template <typename T, template <typename, typename> class S, typename C>
 *  struct PyExportTraits< NoNone< util::SharedPtr<T, S, C> > >: public PyExportTraitsNoNone< util::SharedPtr<T, S, C> >
 *  {
 *      constexpr static const char* py_typing = "T"; // optional
 *  };
 *  ```
 *
 *  Built-in specializations are provided for raw pointers `T*`, util::SharedPtr<T>, and `std::shared_ptr<T>`.
 *
 *  @note If you use `typename T` as the main template parameter for your actual type, then the
 *  Python type will automatically be deduced. If not, or if you still see a `T | None` type-hint,
 *  you can can override the `py_typing` member to specify the type-hint you want.
 *
 *  @ingroup Python
 *  @sa NoNone
 *  @sa PyExportTraits<NoNone<T*>>
 *  @sa PyExportTraits<NoNone<util::SharedPtr<T,S,C>>>
 *  @sa PyExportTraits<NoNone<std::shared_ptr<T>>>
 */
template <typename T>
struct PyExportTraitsNoNone
{
	constexpr static const char* py_typing = "T";

	/** Raise a Python `TypeError` if @a value is equal to `nullptr` */
	static PyObject* build(const NoNone<T>& value)
	{
		const T& v = static_cast<const T&>(value);
		if (v == nullptr)
		{
			PyErr_SetString(PyExc_TypeError, "value must be not be None");
			return nullptr;
		}
		return PyExportTraits<T>::build(value);
	}
	/** Raise a Python `TypeError` if @a obj is equal to `None` */
	static int get(PyObject* obj, NoNone<T>& value)
	{
		if (obj == Py_None)
		{
			PyErr_SetString(PyExc_TypeError, "argument must be not be None");
			return 1;
		}
		return PyExportTraits<T>::get(obj, value);
	}
};

/** NoNone<T*> type-hints as `T` and refuses `None` as value.
 *
 *  @ingroup Python
 *  @sa NoNone
 *  @sa PyExportTraitsNoNone
 */
template <typename T>
struct PyExportTraits< NoNone<T*> > : public PyExportTraitsNoNone<T*>
{
};

/** Type-hints NoNone<util::SharedPtr<T>> as `T` and refuses `None` as value.
 *
 *  @ingroup Python
 *  @sa NoNone
 *  @sa util::SharedPtr
 *  @sa PyExportTraitsNoNone
 */
template <typename T, template <typename, typename> class S, typename C>
struct PyExportTraits< NoNone< util::SharedPtr<T, S, C> > > : public PyExportTraitsNoNone< util::SharedPtr<T, S, C> >
{
};

/** NoNone<std::shared_ptr<T>> type-hints as `T` and refuses `None` as value.
 *
 *  @ingroup Python
 *  @sa NoNone
 *  @sa PyExportTraitsNoNone
 */
template <typename T>
struct PyExportTraits< NoNone< std::shared_ptr<T> > > : public PyExportTraitsNoNone< std::shared_ptr<T> >
{
};



// --- MaybeNone -----------------------------------------------------------------------------------

/** Helper class to create PyExportTraits for MaybeNone wrapped types.
 *
 *  MaybeNone does not provide any runtime or compile-time checks. It only serves to alter the
 *  type-hint to `T | MaybeNone` in Python, which is useful for type-checking, see [The Any Trick].
 * 
 *  MaybeNone<T> types can only be returned from C++ to Python, it doesn't make sense to pass them
 *  as function parameters to C++.
 *
 *  Use this helper to create PyExportTraits for your own MaybeNone wrapped types by inheriting from it.
 *
 *  ```
 *  template <typename T, template <typename, typename> class S, typename C>
 *  struct PyExportTraits< MaybeNone< util::SharedPtr<T, S, C> > >: public PyExportTraitsMaybeNone< util::SharedPtr<T, S, C> >
 *  {
 *      constexpr static const char* py_typing = "T | MaybeNone"; // optional
 *  };
 *  ```
 *
 *  Built-in specializations are provided for raw pointers `T*`, util::SharedPtr<T>, `std::shared_ptr<T>`,
 *  and `std::optional<T>`.
 *
 *  @note If you use `typename T` as the main template parameter for your actual type, then the
 *  Python type will automatically be deduced. If not, or if you still see a `T | None` type-hint,
 *  you can can override the `py_typing` member to specify the type-hint you want.
 * 
 *  [The Any Trick]: https://typing.python.org/en/latest/guides/writing_stubs.html#the-any-trick
 *
 *  @ingroup Python
 *  @sa MaybeNone
 *  @sa PyExportTraits<MaybeNone<T*>>
 *  @sa PyExportTraits<MaybeNone<util::SharedPtr<T,S,C>>>
 *  @sa PyExportTraits<MaybeNone<std::shared_ptr<T>>>
 *  @sa PyExportTraits<MaybeNone<std::optional<T>>>
 */
template <typename T>
struct PyExportTraitsMaybeNone
{
	constexpr static const char* py_typing = "T | MaybeNone";
	constexpr static const char* py_typing_preamble = "from _typeshed import MaybeNone";

	static PyObject* build(const MaybeNone<T>& value)
	{
		return PyExportTraits<T>::build(value);
	}
};

/** MaybeNone<T*> type-hints a type as `T | MaybeNone`
 * 
 *  @ingroup Python
 *  @sa MaybeNone
 *  @sa PyExportTraitsMaybeNone
 */
template <typename T>
struct PyExportTraits< MaybeNone<T*> > : public PyExportTraitsMaybeNone<T*>
{
};

/** MaybeNone<util::SharedPtr<T>> type-hints a type as `T | MaybeNone`
 * 
 *  @ingroup Python
 *  @sa MaybeNone
 *  @sa util::SharedPtr
 *  @sa PyExportTraitsMaybeNone
 */
template <typename T, template <typename, typename> class S, typename C>
struct PyExportTraits< MaybeNone< util::SharedPtr<T, S, C> > > : public PyExportTraitsMaybeNone< util::SharedPtr<T, S, C> >
{
};

/** MaybeNone<std::shared_ptr<T>> type-hints a type as `T | MaybeNone`
 * 
 *  @ingroup Python
 *  @sa MaybeNone
 *  @sa PyExportTraitsMaybeNone
 */
template <typename T>
struct PyExportTraits< MaybeNone< std::shared_ptr<T> > > : public PyExportTraitsMaybeNone< std::shared_ptr<T> >
{
};



// --- Self --------------------------------------------------------------------------------------

/** Self<T> type-hints as `Self`.
 *
 *  @ingroup Python
 *  @sa Self
 */
template <typename T>
struct PyExportTraits< Self<T> > : public PyExportTraits<T>
{
	constexpr static const char* py_typing = "Self";
#if PY_VERSION_HEX < 0x030b0000 // < 3.11
	constexpr static const char* py_typing_preamble = "from typing_extensions import Self";
#else
	constexpr static const char* py_typing_preamble = "from typing import Self";
#endif
};



// --- booleans ------------------------------------------------------------------------------------

/** @ingroup Python
 *  @internal
 */
template <>
struct PyExportTraits<bool>
{
	constexpr static const char* py_typing = "bool";

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
	constexpr static const char* py_typing = "int";

	LASS_META_ASSERT(sizeof(Integer) <= sizeof(long), integer_should_fit_in_long);
	static PyObject* build(Integer v)
	{
		return PyLong_FromLong(v);
	}
	static int get(PyObject* obj, Integer& v)
	{
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
	constexpr static const char* py_typing = "int";

	LASS_META_ASSERT(sizeof(Integer) <= sizeof(unsigned long), integer_should_fit_in_unsigned_long);
	static PyObject* build(Integer v)
	{
		return PyLong_FromUnsignedLong(v);
	}
	static int get(PyObject* obj, Integer& v)
	{
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
	constexpr static const char* py_typing = "int";

	LASS_PYTHON_DLL static PyObject* build(signed PY_LONG_LONG v);
	LASS_PYTHON_DLL static int get(PyObject* obj, signed PY_LONG_LONG& v);
};

/** @ingroup Python
 */
template <>
struct PyExportTraits<unsigned PY_LONG_LONG>
{
	constexpr static const char* py_typing = "int";

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
	constexpr static const char* py_typing = "float";
	constexpr static const char* py_typing_param = "_Float";
	constexpr static const char* py_typing_preamble =
		"from typing import SupportsFloat, SupportsIndex\n"
		"type _Float = float | SupportsFloat | SupportsIndex\n";

	static PyObject* build(Float v)
	{
		return PyFloat_FromDouble(v);
	}
	static int get(PyObject* obj, Float& v)
	{
#if LASS_USE_OLD_EXPORTRAITS_FLOAT
		if (PyFloat_Check(obj))
		{
			return impl::pyNumCast(PyFloat_AS_DOUBLE(obj), v);
		}
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
#else
		double x;
		if (PyFloat_CheckExact(obj))
		{
			x = PyFloat_AS_DOUBLE(obj);
		}
		else if (PyLong_Check(obj))
		{
			x = PyLong_AsDouble(obj);
			if (x == -1.0 && PyErr_Occurred())
			{
				return 1;
			}
		}
		else
		{
			x = PyFloat_AsDouble(obj);
			if (x == -1.0 && PyErr_Occurred())
			{
				return 1;
			}
		}
		v = static_cast<Float>(x);
		return 0;
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
	constexpr static const char* py_typing = "complex";
	constexpr static const char* py_typing_param = "_Complex";
	constexpr static const char* py_typing_preamble = 
		"from typing import SupportsComplex, SupportsFloat, SupportsIndex\n"
		"type _Complex = complex | SupportsComplex | SupportsFloat | SupportsIndex\n";

	static PyObject* build(const std::complex<T>& v)
	{
		return PyComplex_FromDoubles(
			static_cast<double>(v.real()), 
			static_cast<double>(v.imag()));
	}
	static int get(PyObject* obj, std::complex<T>& v)
	{
#if LASS_USE_OLD_EXPORTRAITS_COMPLEX
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
#else
		Py_complex c = PyComplex_AsCComplex(obj);
		if (c.real == -1.0 && PyErr_Occurred())
		{
			return 1;
		}
		v = std::complex<T>(static_cast<T>(c.real), static_cast<T>(c.imag));
		return 0;
#endif
	}
};



// --- strings -------------------------------------------------------------------------------------

/** @ingroup Python
 */
template <typename T>
struct PyExportTraits<std::basic_string_view<T>>
{
	constexpr static const char* py_typing = "str";

	static PyObject* build(std::basic_string_view<T> v)
	{
		return impl::buildStringImpl(v.data(), v.size());
	}
};


/** @ingroup Python
 */
template <>
struct PyExportTraits<const char*>
{
	constexpr static const char* py_typing = "str | None";

	LASS_PYTHON_DLL static PyObject* build(const char* v);
};


/** @ingroup Python
 */
template <size_t N>
struct PyExportTraits<const char [N]>
{
	static PyObject* build(const char* v)
	{
		static_assert(N > 1, "N should include the null-terminator");
		return impl::buildStringImpl(v, N - 1);
	}
};


/** @ingroup Python
 */
template <size_t N>
struct PyExportTraits<char [N]> : PyExportTraits<const char [N]>
{
};


/** @ingroup Python
 */
template <>
struct PyExportTraits<std::string>
{
	constexpr static const char* py_typing = "str";

	LASS_PYTHON_DLL static PyObject* build(const std::string& v);
	LASS_PYTHON_DLL static int get(PyObject* obj, std::string& v);
};


/** @ingroup Python
 */
template <>
struct PyExportTraits<const wchar_t*>
{
	constexpr static const char* py_typing = "str | None";

	LASS_PYTHON_DLL static PyObject* build(const wchar_t* v);
};


/** @ingroup Python
 */
template <size_t N>
struct PyExportTraits<const wchar_t [N]>
{
	static PyObject* build(const wchar_t* v)
	{
		return impl::buildStringImpl(v, N);
	}
};


/** @ingroup Python
 */
template <size_t N>
struct PyExportTraits<wchar_t [N]>: PyExportTraits<const wchar_t [N]>
{
};


/** @ingroup Python
 */
template <>
struct PyExportTraits<std::wstring>
{
	constexpr static const char* py_typing = "str";

	LASS_PYTHON_DLL static PyObject* build(const std::wstring& v);
	LASS_PYTHON_DLL static int get(PyObject* obj, std::wstring& v);
};


#if LASS_HAVE_STD_U8STRING
#if __cpp_lib_char8_t

/** @ingroup Python
 */
template <>
struct PyExportTraits<const char8_t*>
{
	constexpr static const char* py_typing = "str | None";

	LASS_PYTHON_DLL static PyObject* build(const char8_t* v);
};


/** @ingroup Python
 */
template <size_t N>
struct PyExportTraits<const char8_t[N]>
{
	static PyObject* build(const char8_t* v)
	{
		return impl::buildStringImpl(v, N);
	}
};


/** @ingroup Python
 */
template <size_t N>
struct PyExportTraits<char8_t[N]> : PyExportTraits<const char8_t[N]>
{
};


/** @ingroup Python
 */
template <>
struct PyExportTraits<std::u8string>
{
	constexpr static const char* py_typing = "str";

	LASS_PYTHON_DLL static PyObject* build(const std::u8string& v);
	LASS_PYTHON_DLL static int get(PyObject* obj, std::u8string& v);
};

#endif
#endif


/** @ingroup Python
 */
template <>
struct PyExportTraits<const char16_t*>
{
	constexpr static const char* py_typing = "str | None";

	LASS_PYTHON_DLL static PyObject* build(const char16_t* v);
};


/** @ingroup Python
 */
template <size_t N>
struct PyExportTraits<const char16_t[N]>
{
	static PyObject* build(const char16_t* v)
	{
		return impl::buildStringImpl(v, N);
	}
};


/** @ingroup Python
 */
template <size_t N>
struct PyExportTraits<char16_t[N]> : PyExportTraits<const char16_t[N]>
{
};


/** @ingroup Python
 */
template <>
struct PyExportTraits<std::u16string>
{
	constexpr static const char* py_typing = "str";

	LASS_PYTHON_DLL static PyObject* build(const std::u16string& v);
	LASS_PYTHON_DLL static int get(PyObject* obj, std::u16string& v);
};


/** @ingroup Python
 */
template <>
struct PyExportTraits<const char32_t*>
{
	constexpr static const char* py_typing = "str | None";

	LASS_PYTHON_DLL static PyObject* build(const char32_t* v);
};


/** @ingroup Python
 */
template <size_t N>
struct PyExportTraits<const char32_t[N]>
{
	static PyObject* build(const char32_t* v)
	{
		return impl::buildStringImpl(v, N);
	}
};


/** @ingroup Python
 */
template <size_t N>
struct PyExportTraits<char32_t[N]> : PyExportTraits<const char32_t[N]>
{
};


/** @ingroup Python
 */
template <>
struct PyExportTraits<std::u32string>
{
	constexpr static const char* py_typing = "str";

	LASS_PYTHON_DLL static PyObject* build(const std::u32string& v);
	LASS_PYTHON_DLL static int get(PyObject* obj, std::u32string& v);
};


}
}

#endif

// EOF
