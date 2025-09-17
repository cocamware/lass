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

#include "python_common.h"
#include "exception.h"
#include "pyobject_plus.h"
#include "py_tuple.h"
#include <system_error>
#include <filesystem>

namespace
{

using namespace lass::python;
namespace fs = std::filesystem;

using TChar = fs::path::value_type;

void setOSErrorFromErrno(int errno_, const std::string& message, const TChar* path1 = nullptr, const int* winerr = nullptr, const TChar* path2 = nullptr)
{
	TPyObjPtr args;
	if (path2)
	{
		if (winerr)
		{
			args = makeTuple(errno_, message, path1, *winerr, path2);
		}
		else
		{
			args = makeTuple(errno_, message, path1, nullptr, path2);
		}
	}
	else if (winerr)
	{
		args = makeTuple(errno_, message, path1, *winerr);
	}
	else if (path1)
	{
		args = makeTuple(errno_, message, path1);
	}
	else
	{
		args = makeTuple(errno_, message);
	}
	// OSError_new will determine the correct subtype based on errno (or winerror)
	// So we need to retrieve the exception type from the result.
	TPyObjPtr value(PyObject_CallObject(PyExc_OSError, args.get()));
	PyObject* v = value.get();
	PyErr_SetObject((PyObject*) Py_TYPE(v), v);
}

void setOSErrorFromCode(const std::error_code& code, const TChar* path1 = nullptr, const TChar* path2 = nullptr)
{
	if (code.category() == std::generic_category())
	{
		setOSErrorFromErrno(code.value(), code.message(), path1, nullptr, path2);
	}
	else if (code.category() == std::system_category())
	{
#if LASS_PLATFORM_TYPE == LASS_PLATFORM_TYPE_WIN32
		const int winerror = code.value(); // OSError will determine errno from winerror
		setOSErrorFromErrno(0, code.message(), path1, &winerror, path2);
#else
		setOSErrorFromErrno(code.value(), code.message(), path1, nullptr, path2);
#endif
	}
	else
	{
		LASS_ASSERT(!path1 && !path2);
		PyErr_SetString(PyExc_Exception, code.message().c_str());
	}
}

}

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



void fetchAndThrowPythonException(std::string loc)
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

	throw PythonException(type, value, traceback, std::move(loc));
}



void handleException(std::exception_ptr ptr)
{
	LockGIL LASS_UNUSED(lock);
	try
	{
		std::rethrow_exception(ptr);
	}
	catch (const PythonException& error)
	{
		PyErr_Restore(
			fromSharedPtrToNakedCast(error.type()),
			fromSharedPtrToNakedCast(error.value()),
			fromSharedPtrToNakedCast(error.traceback()));
	}
	catch (const util::Exception& error)
	{
		::std::ostringstream buffer;
		buffer << error.message() << "\n\n(" << error.location() << ")";
		PyErr_SetString(PyExc_Exception, buffer.str().c_str());
	}
	catch (const std::invalid_argument& error)
	{
		PyErr_SetString(PyExc_ValueError, error.what());
	}
	catch (const std::domain_error& error)
	{
		PyErr_SetString(PyExc_ValueError, error.what());
	}
	catch (const std::length_error& error)
	{
		PyErr_SetString(PyExc_ValueError, error.what());
	}
	catch (const std::out_of_range& error)
	{
		PyErr_SetString(PyExc_IndexError, error.what());
	}
	catch (const std::range_error& error)
	{
		PyErr_SetString(PyExc_ValueError, error.what());
	}
	catch (const std::overflow_error& error)
	{
		PyErr_SetString(PyExc_OverflowError, error.what());
	}
	catch (const std::underflow_error& error)
	{
		PyErr_SetString(PyExc_ValueError, error.what());
	}
	catch (const std::bad_cast& error)
	{
		PyErr_SetString(PyExc_TypeError, error.what());
	}
	catch (const std::bad_alloc& error)
	{
		PyErr_SetString(PyExc_MemoryError, error.what());
	}
	catch (const std::filesystem::filesystem_error& error)
	{
		const TChar* path1 = error.path1().empty() ? nullptr : error.path1().c_str();
		const TChar* path2 = error.path2().empty() ? nullptr : error.path2().c_str();
		setOSErrorFromCode(error.code(), path1, path2);
	}
	catch (const std::system_error& error)
	{
		setOSErrorFromCode(error.code());
	}
	catch (const std::exception& error)
	{
		PyErr_SetString(PyExc_Exception, error.what());
	}
}

}

PythonException::PythonException(
		const TPyObjPtr& type, const TPyObjPtr& value, const TPyObjPtr& traceback, std::string loc):
	util::ExceptionMixin<PythonException>(extractMessage(type.get(), value.get()), std::move(loc)),
	type_(type),
	value_(value),
	traceback_(traceback)
{
}

PythonException::PythonException(PyObject* type, const std::string& msg, std::string loc):
	util::ExceptionMixin<PythonException>(extractMessage(type) + ": " + msg, std::move(loc)),
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



PyObject* chainErrFormat(PyObject* exception, const char* format, ...)
{
	va_list vargs;
	va_start(vargs, format);
	chainErrFormatV(exception, format, vargs);
	va_end(vargs);
	return nullptr;
}



PyObject* chainErrFormatV(PyObject* exception, const char* format, va_list vargs)
{
	LockGIL LASS_UNUSED(lock);
	if (!PyErr_Occurred())
	{
		return PyErr_FormatV(exception, format, vargs);
	}

	// fetch current exception
	PyObject *exc, *fromValue, *newValue, *traceback;
	PyErr_Fetch(&exc, &fromValue, &traceback);
	// normalize it so `fromValue` is a valid exception instance with traceback
	PyErr_NormalizeException(&exc, &fromValue, &traceback);
	if (traceback)
	{
		PyException_SetTraceback(fromValue, traceback);
		Py_DECREF(traceback); // no longer needed
	}
	Py_DECREF(exc); // no longer needed
	assert(!PyErr_Occurred());

	// create new exception with formatted message
	PyErr_FormatV(exception, format, vargs);
	// fetch and normalize (no need to set traceback)
	PyErr_Fetch(&exc, &newValue, &traceback);
	PyErr_NormalizeException(&exc, &newValue, &traceback);
	// chain the exceptions
	Py_INCREF(fromValue); // increase refcount as both setcause and setcontext will steal one.
	PyException_SetCause(newValue, fromValue);
	PyException_SetContext(newValue, fromValue);
	// restore the exception state
	PyErr_Restore(exc, newValue, traceback);

	return nullptr;
}



}
}
