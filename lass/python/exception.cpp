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
#include "exception.h"
#include "pyobject_plus.h"

namespace lass
{
namespace python
{
namespace impl
{

void addMessageHeader(const char* header)
{
	LockGIL LASS_UNUSED(lock);
	if (!PyErr_Occurred() || !PyErr_ExceptionMatches(PyExc_TypeError))
	{
		return;
	}
	PyObject *type, *value, *traceback;
	PyErr_Fetch(&type, &value, &traceback);
	try
	{
		if (PyUnicode_Check(value))
		{
			std::string left = std::string(header) + ": ";
			TPyObjPtr pyLeft(PyUnicode_DecodeUTF8(left.data(), static_cast<Py_ssize_t>(left.length()), 0));
			PyObject* newValue = PyUnicode_Concat(pyLeft.get(), value);
			std::swap(value, newValue);
			Py_DECREF(newValue);
		}
	}
	catch (const std::exception&)
	{
	}
	PyErr_Restore(type, value, traceback);
}



void fetchAndThrowPythonException(const std::string& loc)
{
	LockGIL LASS_UNUSED(lock);
	if (!PyErr_Occurred())
	{
		LASS_THROW("internal error: fetchAndThrowPythonException called while Python exception is not set");
	}

	PyObject *tempType, *tempValue, *tempTraceback;
	PyErr_Fetch(&tempType, &tempValue, &tempTraceback);

	const TPyObjPtr type(tempType);
	const TPyObjPtr value(tempValue);
	const TPyObjPtr traceback(tempTraceback);

	throw PythonException(type, value, traceback, loc);
}

void catchPythonException(const PythonException& error)
{
	LockGIL LASS_UNUSED(lock);
	PyErr_Restore(
		fromSharedPtrToNakedCast(error.type()),
		fromSharedPtrToNakedCast(error.value()),
		fromSharedPtrToNakedCast(error.traceback()));
}

void catchLassException(const util::Exception& error)
{
	LockGIL LASS_UNUSED(lock);
	::std::ostringstream buffer;
	buffer << error.message() << "\n\n(" << error.location() << ")";
	PyErr_SetString(PyExc_Exception, buffer.str().c_str());
}

void catchStdException(const std::exception& error)
{
	LockGIL LASS_UNUSED(lock);
	PyErr_SetString(PyExc_Exception, error.what());
}

}

PythonException::PythonException(
		const TPyObjPtr& type, const TPyObjPtr& value, const TPyObjPtr& traceback, const std::string& loc):
	util::ExceptionMixin<PythonException>(extractMessage(type.get(), value.get()), loc),
	type_(type),
	value_(value),
	traceback_(traceback)
{
}

PythonException::PythonException(PyObject* type, const std::string& msg, const std::string& loc):
	util::ExceptionMixin<PythonException>(extractMessage(type) + ": " + msg, loc),
	type_(fromNakedToSharedPtrCast<PyObject>(type)),
	value_(pyBuildSimpleObject(msg)),
	traceback_(0)
{
}

PythonException::PythonException(PyObject* type, const std::string& msg):
	util::ExceptionMixin<PythonException>(extractMessage(type) + ": " + msg),
	type_(fromNakedToSharedPtrCast<PyObject>(type)),
	value_(pyBuildSimpleObject(msg)),
	traceback_(0)
{
}

const std::string PythonException::extractMessage(PyObject* type, PyObject* value)
{
	LockGIL LASS_UNUSED(lock);
	std::string message;
	const TPyObjPtr typeStr(PyObject_Str(type));
	if (!typeStr || pyGetSimpleObject(typeStr.get(), message) != 0)
	{
		message = "unknown python exception";
		PyErr_Clear();
	}
	if (value && value != Py_None)
	{
		std::string valmsg;
		const TPyObjPtr valueStr(PyObject_Str(value));
		if (!valueStr || pyGetSimpleObject(valueStr.get(), valmsg) != 0)
		{
			PyErr_Clear();
			return message;
		}
		message += ": " + valmsg;
	}
	return message;
}

}
}
