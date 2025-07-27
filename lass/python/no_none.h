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
 *	Copyright (C) 2025 the Initial Developer.
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

#include "../config/config.h"

namespace lass::python
{

/**	Wrapper to prevent `None` values being passed to and from Python.
 *
 *  Use this class to wrap a value of type `T` that could be `None` in Python (for example,
 *  a pointer type), but you want to ensure that the value is always valid and not `None`.
 *
 *  ```
 *  using ShaderPtr = std::shared_ptr<Shader>;
 *  NoNone<ShaderPtr> subtract(const NoNone<ShaderPtr>& a, const NoNone<ShaderPtr>& b)
 *  {
 *      return ShaderPtr(new SubtractShaders(a, b));
 *  }
 *  PY_MODULE_FUNCTION_DOC(shader_graph, subtract, "Subtract two shaders")
 *  ```
 *
 *  Works in both directions: from and to Python. In case a `None` value is passed or
 *  returned, it will raise a Python `TypeError` instead.
 * 
 *  @note This class does **ONLY** prevents `None` values from crossing the Python/C++
 *  boundary. It does **NOT** provide any other runtime or compile-time checks. You can
 *  happily put a `nullptr` in a `NoNone<T>` as long as you don't pass it to Python.
 *
 *  Built-in support for NoNone is provided for raw pointers `T*`, util::SharedPtr<T>,
 *  and `std::shared_ptr<T>`. To support your own types, you must specialize PyExportTraits:
 *  see PyExportTraitsNoNone for more information.
 * 
 *	@ingroup Python
 *  @sa PyExportTraitsNoNone
 *  @sa PyExportTraits<NoNone<T*>>
 *  @sa PyExportTraits<NoNone<util::SharedPtr<T,S,C>>>
 *  @sa PyExportTraits<NoNone<std::shared_ptr<T>>>
 */
template <typename T>
class NoNone
{
public:
	NoNone(const T& x = T()): value_(x) {}

	T& reference() { return value_; } ///< Return reference to wrapped value
	const T& reference() const { return value_; } ///< Return reference to wrapped value

	operator T&() { return value_; }
	operator const T&() const { return value_; }
	auto operator->() const { return value_.operator->(); }
	auto operator->() { return value_.operator->(); }
	auto& operator*() const { return value_.operator*(); }
	auto& operator*() { return value_.operator*(); }

	bool operator!() const { return !value_; }
	explicit operator bool() const { return static_cast<bool>(value_); }

#if LASS_CPP_STANDARD >= 20
	auto operator<=>(const NoNone& other) const = default;
#endif

private:
	T value_;
};

#if LASS_CPP_STANDARD < 20

template <typename T> bool operator==(const NoNone<T>& a, const NoNone<T>& b) { return a.reference() == b.reference(); }
template <typename T> bool operator!=(const NoNone<T>& a, const NoNone<T>& b) { return a.reference() != b.reference(); }
template <typename T> bool operator<(const NoNone<T>& a, const NoNone<T>& b) { return a.reference() < b.reference(); }
template <typename T> bool operator>(const NoNone<T>& a, const NoNone<T>& b) { return a.reference() > b.reference(); }
template <typename T> bool operator<=(const NoNone<T>& a, const NoNone<T>& b) { return a.reference() <= b.reference(); }
template <typename T> bool operator>=(const NoNone<T>& a, const NoNone<T>& b) { return a.reference() >= b.reference(); }
template <typename T> bool operator==(const NoNone<T>& a, std::nullptr_t b) { return a.reference() == b; }
template <typename T> bool operator!=(const NoNone<T>& a, std::nullptr_t b) { return a.reference() != b; }
template <typename T> bool operator==(std::nullptr_t a, const NoNone<T>& b) { return a == b.reference(); }
template <typename T> bool operator!=(std::nullptr_t a, const NoNone<T>& b) { return a != b.reference(); }

#endif

}
