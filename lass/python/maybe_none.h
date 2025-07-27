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

/**	Wrapper to type-hint return values in Python that maybe `None` but not likely.
 *
 *	Use this class to wrap a type T that could be None in Python (for example, a pointer type),
 *	but it is not likely to be None and forcing the user to expliclitly check for `None` can
 *	more painful than helpful. Annotating the type as `T | MaybeNone` instead of `T | None`
 *	will make static type-checkers like mypy ignore the `None` case, while still allowing
 *	the user to check for `None` if they want to. See [The Any Trick] for more information.
 *	
 *	@note This class does **NOT** prevent `None` values from being passed or returned,
 *	and it does **NOT** provide any runtime or compile-time checks whatsoever.
 *	It only alters the type-hint in Python for static type-checkers.
 *
 *	MaybeNone<T> types can only be returned from C++ to Python, it doesn't make sense to
 *  have them as function parameters in C++.
 * 
 *  Built-in support for MaybeNone is provided for raw pointers `T*`, util::SharedPtr<T>,
 *  `std::shared_ptr<T>`, and `std::optional<T>`.
 *  To support your own types, you must specialize PyExportTraits, see PyExportTraitsMaybeNone
 *  for more information.
 *
 *  [The Any Trick]: https://typing.python.org/en/latest/guides/writing_stubs.html#the-any-trick
 *
 *	@ingroup Python
 *  @sa PyExportTraitsMaybeNone
 *  @sa PyExportTraits<MaybeNone<T*>>
 *  @sa PyExportTraits<MaybeNone<util::SharedPtr<T,S,C>>>
 *  @sa PyExportTraits<MaybeNone<std::shared_ptr<T>>>
 *  @sa PyExportTraits<MaybeNone<std::optional<T>>>
 *  @sa NoNone
 */
template <typename T>
class MaybeNone
{
public:
	MaybeNone(const T& x = T()): value_(x) {}
	MaybeNone(std::nullptr_t): value_(nullptr) {}

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
	auto operator<=>(const MaybeNone& other) const = default;
#endif

private:
	T value_;
};

#if LASS_CPP_STANDARD < 20

template <typename T> bool operator==(const MaybeNone<T>& a, const MaybeNone<T>& b) { return a.reference() == b.reference(); }
template <typename T> bool operator!=(const MaybeNone<T>& a, const MaybeNone<T>& b) { return a.reference() != b.reference(); }
template <typename T> bool operator<(const MaybeNone<T>& a, const MaybeNone<T>& b) { return a.reference() < b.reference(); }
template <typename T> bool operator>(const MaybeNone<T>& a, const MaybeNone<T>& b) { return a.reference() > b.reference(); }
template <typename T> bool operator<=(const MaybeNone<T>& a, const MaybeNone<T>& b) { return a.reference() <= b.reference(); }
template <typename T> bool operator>=(const MaybeNone<T>& a, const MaybeNone<T>& b) { return a.reference() >= b.reference(); }
template <typename T> bool operator==(const MaybeNone<T>& a, std::nullptr_t b) { return a.reference() == b; }
template <typename T> bool operator!=(const MaybeNone<T>& a, std::nullptr_t b) { return a.reference() != b; }
template <typename T> bool operator==(std::nullptr_t a, const MaybeNone<T>& b) { return a == b.reference(); }
template <typename T> bool operator!=(std::nullptr_t a, const MaybeNone<T>& b) { return a != b.reference(); }

#endif

}
