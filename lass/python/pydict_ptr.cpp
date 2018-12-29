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

#include "python_common.h"
#include "pydict_ptr.h"

namespace lass
{
namespace python
{
namespace experimental
{

PyDictPtr::PyDictPtr() :
	obj_(0)
{
}


/**
 * steals a reference to obj.
 */
PyDictPtr::PyDictPtr(PyObject* obj):
	obj_(obj)
{
	if (obj && !PyDict_Check(obj))
		throw PythonException(PyExc_TypeError, "Not a dict", LASS_HERE);
}


/**
 * returns a borrowed reference
 */
PyObject* PyDictPtr::get() const
{
	return obj_.get();
}


bool PyDictPtr::operator!() const
{
	return !obj_;
}


PyDictPtr::operator num::SafeBool() const
{
	return obj_ ? num::safeTrue : num::safeFalse;
}


void PyDictPtr::setItem(PyObject* key, PyObject* val)
{
	LockGIL LASS_UNUSED(lock);
	PY_ENFORCE_ZERO(PyDict_SetItem(get(), key, val));
}


void PyDictPtr::setItem(const TPyObjPtr& key, const TPyObjPtr& val)
{
	setItem(key.get(), val.get());
}


TPyObjPtr PyDictPtr::getItem(PyObject* key) const
{
	LockGIL LASS_UNUSED(lock);
#if PY_MAJOR_VERSION >= 3
	PyObject* const borrowed = PyDict_GetItemWithError(get(), key);
#else
	// for historical reasons, PyDict_GetItem PyDict_GetItem() suppresses all
	// errors that may occur (like hash errors, or stack depth errors)
	PyObject* const borrowed = PyDict_GetItem(get(), key);
#endif
	if (!borrowed)
	{
		if (PyErr_Occurred())
		{
			impl::fetchAndThrowPythonException(LASS_HERE);
		}
		throw PythonException(PyExc_KeyError, "Key error", LASS_HERE);
	}
	return fromNakedToSharedPtrCast<PyObject>(borrowed);
}


TPyObjPtr PyDictPtr::getItem(const TPyObjPtr& key) const
{
	return getItem(key.get());
}


void PyDictPtr::delItem(PyObject* key)
{
	LockGIL LASS_UNUSED(lock);
	if (PyDict_DelItem(get(), key) != 0)
		impl::fetchAndThrowPythonException(LASS_HERE);
}


void PyDictPtr::delItem(const TPyObjPtr& key)
{
	delItem(key.get());
}


bool PyDictPtr::contains(PyObject* key) const
{
	LockGIL LASS_UNUSED(lock);
	switch (PyDict_Contains(get(), key))
	{
	case -1:
		impl::fetchAndThrowPythonException(LASS_HERE);
		break;
	case 0:
		return false;
	case 1:
		return true;
	}
	// this should be unreachable.
	throw PythonException(PyExc_SystemError, "bad internal call.", LASS_HERE);
}


bool PyDictPtr::contains(const TPyObjPtr& key) const
{
	return contains(key.get());
}


Py_ssize_t PyDictPtr::size() const
{
	LockGIL LASS_UNUSED(lock);
	const Py_ssize_t ret = PyDict_Size(get());
	if (ret == -1)
		impl::fetchAndThrowPythonException(LASS_HERE);
	return ret;
}


}


PyObject* PyExportTraits<experimental::PyDictPtr>::build(const experimental::PyDictPtr& v)
{
	if (PyObject* const obj = v.get())
	{
		Py_INCREF(obj);
		return obj;
	}
	Py_RETURN_NONE;
}


int PyExportTraits<experimental::PyDictPtr>::get(PyObject* obj, experimental::PyDictPtr& v)
{
	if (obj == Py_None)
	{
		v = experimental::PyDictPtr(0);
	}
	if (!PyDict_Check(obj))
	{
		PyErr_SetString(PyExc_TypeError, "not a dict");
		return 1;
	}
	Py_INCREF(obj); // obj is borrowed, but PyDictPtr steals a reference
	v = experimental::PyDictPtr(obj);
	return 0;
}


}
}
