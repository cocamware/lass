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

#include "python_common.h"
#include "export_traits_filesystem.h"

#if LASS_HAVE_STD_FILESYSTEM

namespace lass
{
namespace python
{

PyObject* PyExportTraits<std::filesystem::path>::build(const std::filesystem::path& v)
{
#if LASS_PLATFORM_TYPE == LASS_PLATFORM_TYPE_WIN32
	// On Windows, paths use wchar_t: we can directly return it as unicode
	static_assert(std::is_same_v<std::filesystem::path::value_type, wchar_t>);
	TPyObjPtr path{ PyExportTraits<std::wstring>::build(v.native()) };
#else
	// On Linux, paths use char: we decode them to unicode
	static_assert(std::is_same_v<std::filesystem::path::value_type, char>);
	const std::string& s = v.native();
	if (s.size() > PY_SSIZE_T_MAX)
	{
		PyErr_SetString(PyExc_OverflowError, "input too long");
		return 0;
	}
	TPyObjPtr path{ PyUnicode_DecodeFSDefaultAndSize(s.data(), static_cast<Py_ssize_t>(s.size())) };
#endif

	static TPyObjPtr pathType;
	if (!pathType)
	{
		TPyObjPtr pathlib(PyImport_ImportModule("pathlib"));
		pathType.reset(PyObject_GetAttrString(pathlib.get(), "Path"));
	}

	return PyObject_CallFunctionObjArgs(pathType.get(), path.get(), nullptr);
}


int PyExportTraits<std::filesystem::path>::get(PyObject* obj, std::filesystem::path& v)
{
	TPyObjPtr path(PyOS_FSPath(obj));
	if (!path)
	{
		return 1;
	}
#if LASS_PLATFORM_TYPE == LASS_PLATFORM_TYPE_WIN32
	// On Windows, paths use wchar_t: decode bytes to unicode and get as wstring.
	static_assert(std::is_same_v<std::filesystem::path::value_type, wchar_t>);
	PyObject* strObj = nullptr;
	if (!PyUnicode_FSDecoder(obj, &strObj))
	{
		return 1;
	}
	std::wstring str;
	const int ret = PyExportTraits<std::wstring>::get(strObj, str);
	Py_DECREF(strObj);
	if (ret == 0)
	{
		v = std::move(str);
	}
	return ret;
#else
	// On POSIX, paths use char: encode unicode to bytes and get as string
	static_assert(std::is_same_v<std::filesystem::path::value_type, char>);
	PyObject* bytesObj = nullptr;
	if (!PyUnicode_FSConverter(obj, &bytesObj))
	{
		return 1;
	}
	std::string bytes;
	const int ret = PyExportTraits<std::string>::get(bytesObj, bytes);
	Py_DECREF(bytesObj);
	if (ret == 0)
	{
		v = std::move(bytes);
	}
	return ret;
#endif
}

}
}

#endif

// EOF
