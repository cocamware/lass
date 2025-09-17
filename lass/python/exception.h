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

#ifndef LASS_GUARDIAN_OF_INCLUSION_PYTHON_EXCEPTION_H
#define LASS_GUARDIAN_OF_INCLUSION_PYTHON_EXCEPTION_H

#include "python_common.h"
#include "pyobject_ptr.h"

namespace lass
{
namespace python
{


/** @defgroup PythonExceptions Python Exceptions
 *  @brief Handling Python exceptions in C++, and translating C++ exceptions to Python.
 *
 *  This module provides utilities for converting C++ exceptions into Python exceptions,
 *  and for handling Python exceptions in C++ code.
 * 
 *  Its main part is the class `PythonException` that represents a Python exception in C++ code.
 *  Additionally, it provides functions and macros to facilitate raising and handling Python exceptions.
 * 
 *  @ingroup Python
 */


/** C++ exception type that holds a Python exception
 *
 *  This class is used to propagate Python exceptions through C++ code. Can be used to both
 *  - handle exceptions originating from Python,
 *  - raise new Python exceptions from C++ code.
 * 
 *  @ingroup PythonExceptions
 */
class PythonException: public util::ExceptionMixin<PythonException>
{
public:
	LASS_PYTHON_DLL PythonException(const TPyObjPtr& type, const TPyObjPtr& value, const TPyObjPtr& traceback, std::string loc);
	LASS_PYTHON_DLL PythonException(PyObject* type, const std::string& msg, std::string loc);
	LASS_PYTHON_DLL PythonException(PyObject* type, const std::string& msg);

	~PythonException() noexcept {}
	const python::TPyObjPtr& type() const { return type_; }
	const python::TPyObjPtr& value() const { return value_; }
	const python::TPyObjPtr& traceback() const { return traceback_; }
private:
	static LASS_PYTHON_DLL const std::string LASS_CALL extractMessage(PyObject* type, PyObject* value = 0);

	python::TPyObjPtr type_;
	python::TPyObjPtr value_;
	python::TPyObjPtr traceback_;
};


namespace impl
{
	/** Prepend a message to the current Python exception value
	 *
	 *  This function adds a custom header to the current Python exception message,
	 *  if one is set. This can be useful for providing additional context about
	 *  the error.
	 * 
	 *  If no exception is currently set, i.e. `PyErr_Occurred()` returns false, then
	 *  this function does nothing.
	 *
	 *  @param header The header to add to the exception message.
	 *  @note Only works for exceptions whose value is a string. If the value is not a string,
	 * 	  the function does nothing.
	 *  @ingroup PythonExceptions
	 */
	LASS_PYTHON_DLL void LASS_CALL addMessageHeader(const char* header);



	/** Fetch the current Python exception and throw it as a C++ PythonException
	 * 
	 *  @ingroup PythonExceptions
	 */
	LASS_PYTHON_DLL void LASS_CALL fetchAndThrowPythonException [[noreturn]] (std::string loc = "");



	/** Handle a C++ exception by raising an Python exception
	 * 
	 *  This function translates a C++ exception into a Python exception.
	 *  It supports `PythonException`, `util::Exception`, and standard C++ exceptions.
	 *  It also supports `std::filesystem::filesystem_error` and `std::system_error`
	 *  by translating the error code to an appropriate OSError subclass.
	 * 
	 *  If the exception type is not recognized, it will be translated to a generic
	 *  `Exception` with the what() message of the C++ exception.
	 * 
	 *  @param error The exception pointer to handle. This is typically obtained
	 *     using `std::current_exception()`.
	 * 
	 *  @ingroup PythonExceptions
	 */
	LASS_PYTHON_DLL void LASS_CALL handleException(std::exception_ptr error);



	/** Raiser type for enforcers that raises a Python exception
	 * 
	 *  This can be used as a building block for Python-related enforcers.
	 *  It's used by the macros `PY_ENFORCE_POINTER()`, `PY_ENFORCE_ZERO()`, and `PY_ENFORCE_NOTZERO()`.
	 * 
	 *  If a Python error is set, it will be fetched and thrown as a `PythonException`.
	 *  Otherwise, a new `PythonException` of type `AssertionError` will be thrown.
	 * 
	 *  @throws ::lass::python::PythonException
	 *  @ingroup PythonExceptions Enforcers
	 */
	struct PythonFetchRaiser
	{
 		template <typename T, typename C>
		static void raise(const T&, const C&, const std::string& message, const char* locus)
		{
			LockGIL LASS_UNUSED(lock);
			if (PyErr_Occurred())
			{
				fetchAndThrowPythonException(locus);
			}
			if (!message.empty())
			{
				throw PythonException(PyExc_AssertionError, message, locus);
			}
			throw PythonException(PyExc_AssertionError, locus, locus);
		}
	};
}
}
}


#define LASS_PYTHON_CATCH_AND_RETURN_EX(v_errorReturnValue)\
	catch (const ::std::exception&)\
	{\
		::lass::python::impl::handleException(std::current_exception());\
		return v_errorReturnValue;\
	}

#define LASS_PYTHON_CATCH_AND_RETURN\
	LASS_PYTHON_CATCH_AND_RETURN_EX(0)



/** Enforce that a Python pointer is not null
 * 
 *  @throws ::lass::python::PythonException
 *  @sa ::lass::python::impl::PythonFetchRaiser
 *  @ingroup PythonExceptions
 *  @sa Enforcers
 */
#define PY_ENFORCE_POINTER(pointer)\
		*LASS_UTIL_IMPL_MAKE_ENFORCER(\
		::lass::util::impl::TruePredicate,\
		::lass::python::impl::PythonFetchRaiser,\
		(pointer), \
		int(0), \
		"'" LASS_STRINGIFY(pointer) "' in " LASS_HERE)



/** Enforce that an expression evaluates to zero
 * 
 *  @throws ::lass::python::PythonException
 *  @sa ::lass::python::impl::PythonFetchRaiser
 *  @ingroup PythonExceptions
 *  @sa Enforcers
 */
#define PY_ENFORCE_ZERO(expression)\
		*LASS_UTIL_IMPL_MAKE_ENFORCER(\
		::lass::util::impl::EqualPredicate,\
		::lass::python::impl::PythonFetchRaiser,\
		(expression), \
		int(0), \
		"'" LASS_STRINGIFY(expression) "' in " LASS_HERE)



/** Enforce that an expression evaluates to not zero
 *
 *  @throws ::lass::python::PythonException
 *  @sa ::lass::python::impl::PythonFetchRaiser
 *  @ingroup PythonExceptions
 *  @sa Enforcers
 */
#define PY_ENFORCE_NOTZERO(expression)\
		*LASS_UTIL_IMPL_MAKE_ENFORCER(\
		::lass::util::impl::UnequalPredicate,\
		::lass::python::impl::PythonFetchRaiser,\
		(expression), \
		int(0), \
		"'" LASS_STRINGIFY(expression) "' in " LASS_HERE)

#endif
