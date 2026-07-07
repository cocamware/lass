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
 *	Copyright (C) 2026 the Initial Developer.
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

namespace lass::python
{

/** @defgroup PythonSubscript Subscript Protocol
 *  @brief Support for index- and slice-based access, assignment and deletion.
 *
 *  To support `obj[key]` subscription on an exported class, assign your `__getitem__`, `__setitem__` and `__delitem__`
 *  overloads to one of two families of special slots:
 *
 *  | operation    | sequence protocol (adjusted index)               | mapping protocol (raw key) |
 *  |--------------|--------------------------------------------------|----------------------------|
 *  | `v = obj[i]` | `methods::seq_getitem_` (= `methods::_getitem_`) | `methods::map_getitem_`    |
 *  | `obj[i] = v` | `methods::seq_setitem_` (= `methods::_setitem_`) | `methods::map_setitem_`    |
 *  | `del obj[i]` | n/a                                              | `methods::map_delitem_`    |
 *
 *
 *  ### Integer indices only
 * 
 *  Assign a `getitem(Py_ssize_t)` method to `methods::_getitem_`. Python adjusts negative indices automatically before
 *  calling your method, so that `obj[-1]` will be adjusted to `size() - 1`. Your method only needs to check bounds,
 *  and raise an `IndexError` if the index is out of range.
 * 
 *  Default iteration `for x in obj` works automatically through this slot.
 * 
 *  ```
 *  std::string myvector_seq_getitem(const MyVector& self, Py_ssize_t index)
 *  {
 *      if (index < 0 || std::cmp_greater_equal(index, self.size()))
 *      {
 *          throw lass::python::PythonException(PyExc_IndexError, "index out of range");
 *      }
 *      return self.at(static_cast<size_t>(index));
 *  }
 *  PY_CLASS_FREE_METHOD_NAME(MyVector, myvector_seq_getitem, methods::seq_getitem_)
 *  ```
 *
 *  If your method already throws a `std::out_of_range` exception for out of bound indices, like `std::vector<T>::at`,
 *  this will be automatically translated to an `IndexError` in Python.
 * 
 *  If the index uses a type other than `Py_ssize_t`, you may get an `OverflowError` instead of `IndexError` if an
 *  index doesn't fit the type. In particular, with `size_t` you will get an `OverflowError` for negative numbers.
 *  To be fully compliant, roll your own bounds checks as above. But if the `OverflowError` is acceptable, the
 *  method shrinks to:
 * 
 *  ```
 *  std::string myvector_seq_getitem(const MyVector& self, size_t index)
 *  {
 *      return self.at(index);
 *  }
 *  PY_CLASS_FREE_METHOD_NAME(MyVector, myvector_seq_getitem, methods::seq_getitem_)
 *  ```
 *
 *
 *  ### Slices
 * 
 *  `slice` arguments can only be received through the mapping protocol, as a Slice parameter.
 *  Assign a `getslice(Slice)` method to `methods::map_getitem_`.
 * 
 *  ```
 *  std::vector<std::string> myvector_getslice(const MyVector& self, Slice slice)
 *  {
 *      auto sliceLength = slice.adjustIndices(self.size());
 *      auto index = slice.start;
 *      std::vector<std::string> result;
 *      result.reserve(sliceLength);
 *      for (Py_ssize_t i = 0; i < sliceLength; ++i)
 *      {
 *          result.push_back(self.at(static_cast<size_t>(index)));
 *          index += slice.step;
 *      }
 *      return result;
 *  }
 *  PY_CLASS_FREE_METHOD_NAME(MyVector, myvector_getslice, methods::map_getitem_)
 *  ```
 * 
 *  @note This has an important consequence: once a class has any `__getitem__` method on the `methods::map_getitem_`
 *        slot, Python ignores the `methods::seq_getitem_` for subscription. Move the integer overload to
 *        `methods::map_getitem_` too, but Python will no longer automatically adjust negative indices! Use
 *        `adjustIndex()` to manually adjust the negative indices; as a bonus, it will also raise `IndexError` for
 *        out-of-range indices.
 *
 *  ```
 *  std::string myvector_map_getitem(const MyVector& self, Py_ssize_t index)
 *  {
 *      index = adjustIndex(index, self.size());
 *      return self.at(static_cast<size_t>(index));
 *  }
 *  PY_CLASS_FREE_METHOD_NAME(MyVector, myvector_map_getitem, methods::map_getitem_)
 *  ```
 *
 *
 *  ### Iteration
 * 
 *  Without a `__getitem__` on the `methods::seq_getitem_` slot, default iteration no longer works. You have two options:
 * 
 *  1. Register a proper `__iter__` function. See @ref PythonIterators for more details:
 * 
 *     ```
 *     PY_CLASS_FREE_METHOD_NAME(MyVector, makeMemberRangeViewFactory<MyVector>(&MyVector::begin, &MyVector::end), methods::_iter_)
 *     ```
 *
 *  2. **Also** register a `__getitem__` on `methods::seq_getitem_`. **But don't use adjustIndex() for that one:**
 *     for callers of `PySequence_GetItem(obj, index)` with `index < -len(obj)`, the index would be adjusted **twice**,
 *     potentially bringing the index in-range while it still should have been out-of-range.
 * 
 * 
 *  ### Assignment and Deletion
 * 
 *  Assignment and deletion follow the same rules through `methods::map_setitem_` and `methods::map_delitem_`. Under the
 *  hood, they share the same `Py_mp_ass_subscript` slot, but a `__setitem__` overload has key and value parameters,
 *  while a `__delitem__` overload takes only a key parameter.
 * 
 *  Putting it all together:
 * 
 *  ```
 *  PY_CLASS_FREE_METHOD_NAME(MyVector, myvector_getitem,  methods::map_getitem_)  // (Py_ssize_t) -> T, uses adjustIndex()
 *  PY_CLASS_FREE_METHOD_NAME(MyVector, myvector_getslice, methods::map_getitem_)  // (Slice) -> std::vector<T>
 *  PY_CLASS_FREE_METHOD_NAME(MyVector, myvector_setitem,  methods::map_setitem_)  // (Py_ssize_t, T), uses adjustIndex()
 *  PY_CLASS_FREE_METHOD_NAME(MyVector, myvector_setslice, methods::map_setitem_)  // (Slice, std::vector<T>)
 *  PY_CLASS_FREE_METHOD_NAME(MyVector, myvector_delitem,  methods::map_delitem_)  // (Py_ssize_t), uses adjustIndex()
 *  PY_CLASS_FREE_METHOD_NAME(MyVector, myvector_delslice, methods::map_delitem_)  // (Slice)
 *  PY_CLASS_FREE_METHOD_NAME(MyVector, makeMemberRangeViewFactory<MyVector>(&MyVector::begin, &MyVector::end), methods::_iter_)
 *  ```
 * 
 *  @ingroup Python
 *  @sa PythonIterators
 */


/** Helper type to get or return Python slice objects
 * 
 *  This helper can be used as C++ parameter type to overload `__getitem__` to support Python slice objects.
 * 
 *  See @ref PythonSubscript for more details and examples.
 * 
 *  @note Before you can use the start/stop indices, you **must** adjust them for the actual sequence length by
 *        calling adjustIndices(). This will also return the actual number of elements in the slice.
 * 
 *
 *  @ingroup PythonSubscript
 *  @sa PythonSubscript
 *  @sa adjustIndex
 *  @sa adjustIndexEx
 *  @sa PyExportTraits<Slice>
 */
struct LASS_PYTHON_DLL Slice
{
	Py_ssize_t start; ///< start index of slice. Always the index of a valid iterator after calling adjustIndices()
	Py_ssize_t stop;  ///< stop index of slice. Can be -1 if step < 0! It's not guaranteed that `stop == start + sliceLength * step`
	Py_ssize_t step;  ///< step size of slice.

	/** Adjust start and stop for actual sequence length, returning slice length
	 *
	 *  @param sequenceLength actual length of the sequence to be sliced
	 *  @return `sliceLength`, actual number of elements in slice
	 *
	 *  @note Unlike `PySlice_AdjustIndices`, `start` will never be -1 (that case, which only happens when
	 *        `sliceLength == 0` and `step < 0`, is normalized to `start = stop = 0`).
	 *
	 *  @pre `sequenceLength >= 0`
	 *  @pre `step != 0`
	 *  @post `start >= 0 && start <= sequenceLength`
	 *  @post `stop >= -1 && stop <= sequenceLength`
	 */
	Py_ssize_t adjustIndices(Py_ssize_t sequenceLength);
};



/** Helper to adjust negative sequence indices
 * 
 *  Use adjustIndex() to adjust the raw index argument of functions that are assigned to `methods::map_getitem_`,
 *  `methods::map_setitem_`, or `methods::map_delitem_`. Negative indices will be adjusted to start counting from the
 *  end of the sequence.
 * 
 *  If the adjusted index is out of range of the sequence, a C++ PythonException exception will
 *  be thrown, containing a Python `IndexError` exception.
 * 
 *  See @ref PythonSubscript for more details.
 * 
 *  @note Only use in methods on the `methods::map_*` slots: the sequence slots receive already-adjusted indices.
 *
 *  @param[in] index the raw index to be adjusted
 *  @param[in] sequenceLength size of the sequence to adjust the index for
 *  @return the adjusted index
 *  @throw PythonException with an `IndexError` if adjusted index is outside [0, sequenceLength)
 *
 *  @pre `sequenceLength >= 0`
 *
 *  @ingroup PythonSubscript
 *  @sa PythonSubscript
 *  @sa Slice
 *  @sa adjustIndexEx
 */
LASS_PYTHON_DLL Py_ssize_t adjustIndex(Py_ssize_t index, Py_ssize_t sequenceLength);



/** Helper to adjust negative sequence indices
 * 
 *  Similar to `adjustIndex()`, but modifies `index` in-place, and returns false if the adjusted
 *  index is out of range.
 * 
 *  See @ref PythonSubscript for more details.
 *
 *  @note Most user code should use adjustIndex().
 *
 *  @note Only use in methods on the `methods::map_*` slots: the sequence slots receive already-adjusted indices.
 *
 *  @param[in,out] index pointer to the raw index to be adjusted
 *  @param[in] sequenceLength size of the sequence to adjust the index for
 *  @return
 *      - true if the adjusted index is within range;
 *      - false if the adjusted index is out of range: an `IndexError` Python exception will be set.
 *
 *  @pre `index != nullptr`
 *  @pre `sequenceLength >= 0`
 *  @post On failure, `*index` is unchanged
 *
 *  @ingroup PythonSubscript
 *  @sa PythonSubscript
 *  @sa Slice
 *  @sa adjustIndex
 */
LASS_PYTHON_DLL bool adjustIndexEx(Py_ssize_t *index, Py_ssize_t sequenceLength);

}

// EOF
