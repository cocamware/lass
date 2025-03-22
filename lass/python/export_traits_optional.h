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
 *	Copyright (C) 2022-2025 the Initial Developer.
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

#if LASS_HAVE_STD_OPTIONAL

#include <optional>

namespace lass
{
namespace python
{

/** @ingroup Python
 *
 *  Uses None to represent a std::optional without value.
 * 
 *  If None is also a valid value for the type T (such as pointers), then it
 *  will be impossible to differentiate between an unset std::optional, and
 *  a set std::optional with a value for None.
 */
template <typename T>
struct PyExportTraits< std::optional<T> >
{
	static constexpr const char* py_typing = "T | None";

	static PyObject* build(const std::optional<T>& value)
	{
		if (!value)
		{
			Py_RETURN_NONE;
		}
		return PyExportTraits<T>::build(*value);
	}
	static int get(PyObject* obj, std::optional<T>& value)
	{
		if (obj == Py_None)
		{
			value.reset();
			return 0;
		}
		T tmp;
		if (PyExportTraits<T>::get(obj, tmp) != 0)
		{
			return 1;
		}
		value = std::move(tmp);
		return 0;
	}
};

}
}

#endif
