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
 *	Copyright (C) 2022 the Initial Developer.
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

#pragma once

#include "python_common.h"
#include "export_traits.h"

#if LASS_HAVE_STD_VARIANT

#include <variant>

namespace lass
{
namespace python
{

/** std::monostate always maps to None
 *
 *  @ingroup Python
 */
template <>
struct PyExportTraits<std::monostate>
{
	static PyObject* build(std::monostate)
	{
		Py_RETURN_NONE;
	}
	static int get(PyObject* obj, std::monostate&)
	{
		if (obj != Py_None)
		{
			PyErr_SetString(PyExc_TypeError, "Not None");
			return 1;
		}
		return 0;
	}
};


namespace impl
{

template <typename Variant, typename ...T> struct PyExportTraitsVariantImpl;

template <typename Variant, typename Head, typename ...Tail>
struct PyExportTraitsVariantImpl<Variant, Head, Tail...>
{
	static int get(PyObject* obj, Variant& v)
	{
		Head head;
		if (PyExportTraits<Head>::get(obj, head) == 0)
		{
			v = std::move(head);
			return 0;
		}
		PyErr_Clear();
		return PyExportTraitsVariantImpl<Variant, Tail...>::get(obj, v);
	}
};

template <typename Variant, typename ...Tail>
struct PyExportTraitsVariantImpl<Variant, bool, Tail...>
{
	// bools get special treatment.
	// 
	// Normally, bools accept anything that evaluates to bool, and anything that is
	// true-ish will evaluate to true. This requires to put them at the end of the
	// alternative list, but then you would never be able to differentiate between
	// bool and ints, as bools are ints too ...
	//
	// If bool is part of a variant, we only accept them strictly as bools, so you can
	// put them first in your list.
	static int get(PyObject* obj, Variant& v)
	{
		if (PyBool_Check(obj))
		{
			v = obj == Py_True
				? true
				: false;
			return 0;
		}
		return PyExportTraitsVariantImpl<Variant, Tail...>::get(obj, v);
	}
};

template <typename Variant>
struct PyExportTraitsVariantImpl<Variant>
{
	static int get(PyObject* /*obj*/, Variant& /*v*/)
	{
		PyErr_SetString(PyExc_TypeError, "Unable to cast to variant");
		return 1;
	}
};

}


/** Exports std::variant using the PyExportTraits of individual types.
 *
 *  std::monostate is always mapped to None
 *
 *  When getting an std::variant from Python, the order of the alternative types
 *  is important. Each alternative will be tried from left to right, and the
 *  first match will determine the std::variant alternative type.
 *
 *  Here are some possible pitfalls to consider:
 *
 *  - None will always match to std::monostate. If None is also a valid value
 *    for any of the following alternatives, those types will never be considered
 *    (as std::monostate must be the first alternative).
 * 
 *  - Integers are valid floats, and floats are valid complex numbers. If complex
 *    or a floating point is followed by an integer, then an integer will always
 *    match the complex or floating point. For example, the following will not
 *    have the desired outcome:
 *
 *        std::variant<std::complex<float>, double, signed int>
 *
 *    Better is to reverse the order:
 *
 *        std::variant<signed int, double, std::complex<float>>
 * 
 *  - A Python str is also a sequence of str. If a std::variant has a container
 *    of std::string preceding std::string itself as alternative, then a Python
 *    string will always match the container. For example, the following will not
 *    have the desired outcome:
 *
 *        std::variant<std::vector<std::string>, std::string>
 *
 *    Better is to put the std::string first:
 *
 *        std::variant<std::string, std::vector<std::string>>
 * 
 *    NOTE: std::pair and std::tuple are also sequences in Python. So
 *    std::pair<std::string, std::string> will match any two character string.
 *
 *  @ingroup Python
 */
template <typename... T>
struct PyExportTraits< std::variant<T...> >
{
	using TVariant = std::variant<T...>;

	static PyObject* build(const TVariant& v)
	{
		return std::visit([](auto&& arg) -> PyObject* {
			using U = std::decay_t<decltype(arg)>;
			return PyExportTraits<U>::build(arg);
		}, v);
	}

	static int get(PyObject* obj, TVariant& v)
	{
		return impl::PyExportTraitsVariantImpl<TVariant, T...>::get(obj, v);
	}
};

}
}

#endif