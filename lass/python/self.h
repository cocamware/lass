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

/**	Wrapper to type-hint a return value as `Self`.
 *
 *  This is helpful for virtual methods that return an object of the same type as
 *  the `this` pointer. Here's an example with a clone method:
 *
 *  ```
 *  class Spam
 *  {
 *  public:
 *      Self<TSpamPtr> clone() const
 *      {
 *          auto result = doClone();
 *          const Spam* cloned = result.get();
 *          LASS_ENFORCE(typeid(*cloned) != typeid(*this));
 *          return result;
 *      }
 *  private:
 *      virtual TSpamPtr doClone() const // must be overriden by each derived class 
 *      {
 *          return TSpamPtr(new Spam(*this));
 *      }
 *  };
 *  ```
 *
 *  This will make sure the type-hint of Spam will look like:
 *  ```
 *  class Spam:
 *      def clone(self) -> Self: ...
 *  ```
 *
 *  See [typing.Self] and [PEP 673] for more info.
 * 
 *  [typing.Self]: https://docs.python.org/3/library/typing.html#typing.Self
 *  [PEP 673]: https://peps.python.org/pep-0673/
 *
 *  @ingroup Python
 *  @sa PyExportTraits<Self<T>>
 *  @sa NoNone
 *  @sa MaybeNone
 */
template <typename T>
class Self
{
public:
	Self(const T& x = T()): value_(x) {}

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

#if __cpp_impl_three_way_comparison
	auto operator<=>(const Self& other) const = default;
#endif

private:
	T value_;
};

#if !__cpp_impl_three_way_comparison

template <typename T> bool operator==(const Self<T>& a, const Self<T>& b) { return a.reference() == b.reference(); }
template <typename T> bool operator!=(const Self<T>& a, const Self<T>& b) { return a.reference() != b.reference(); }
template <typename T> bool operator<(const Self<T>& a, const Self<T>& b) { return a.reference() < b.reference(); }
template <typename T> bool operator>(const Self<T>& a, const Self<T>& b) { return a.reference() > b.reference(); }
template <typename T> bool operator<=(const Self<T>& a, const Self<T>& b) { return a.reference() <= b.reference(); }
template <typename T> bool operator>=(const Self<T>& a, const Self<T>& b) { return a.reference() >= b.reference(); }
template <typename T> bool operator==(const Self<T>& a, std::nullptr_t b) { return a.reference() == b; }
template <typename T> bool operator!=(const Self<T>& a, std::nullptr_t b) { return a.reference() != b; }
template <typename T> bool operator==(std::nullptr_t a, const Self<T>& b) { return a == b.reference(); }
template <typename T> bool operator!=(std::nullptr_t a, const Self<T>& b) { return a != b.reference(); }

#endif

}
