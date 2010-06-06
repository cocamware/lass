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
 *	Copyright (C) 2004-2010 the Initial Developer.
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

class LASS_DLL PythonException: public util::ExceptionMixin<PythonException>
{
public:
	PythonException(const TPyObjPtr& type, const TPyObjPtr& value, const TPyObjPtr& traceback, const std::string& loc);
	PythonException(PyObject* type, const std::string& value, const std::string& loc = "no location");
	~PythonException() throw();
	const python::TPyObjPtr& type() const;
	const python::TPyObjPtr& value() const;
	const python::TPyObjPtr& traceback() const;
private:
	python::TPyObjPtr type_;
	python::TPyObjPtr value_;
	python::TPyObjPtr traceback_;
	static const std::string extractMessage(PyObject* type, PyObject* value = 0);
};

namespace impl
{
	LASS_DLL void LASS_CALL addMessageHeader(const char* header);
	LASS_DLL void LASS_CALL fetchAndThrowPythonException(const std::string& loc = "");
	LASS_DLL void LASS_CALL catchPythonException(const PythonException& error);
	LASS_DLL void LASS_CALL catchLassException(const util::Exception& error);
	LASS_DLL void LASS_CALL catchStdException(const std::exception& error);

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
	catch (const ::lass::python::PythonException& error)\
	{\
		::lass::python::impl::catchPythonException(error);\
		return v_errorReturnValue;\
	}\
	catch (const ::lass::util::Exception& error)\
	{\
		::lass::python::impl::catchLassException(error);\
		return v_errorReturnValue;\
	}\
	catch (::std::exception& error)\
	{\
		::lass::python::impl::catchStdException(error);\
		return v_errorReturnValue;\
	}

#define LASS_PYTHON_CATCH_AND_RETURN\
	LASS_PYTHON_CATCH_AND_RETURN_EX(0)

#define PY_ENFORCE_POINTER(pointer)\
		*LASS_UTIL_IMPL_MAKE_ENFORCER(\
		::lass::util::impl::TruePredicate,\
		::lass::python::impl::PythonFetchRaiser,\
		(pointer), \
		int(0), \
		"'" LASS_STRINGIFY(pointer) "' in " LASS_HERE)

#define PY_ENFORCE_ZERO(expression)\
		*LASS_UTIL_IMPL_MAKE_ENFORCER(\
		::lass::util::impl::EqualPredicate,\
		::lass::python::impl::PythonFetchRaiser,\
		(expression), \
		int(0), \
		"'" LASS_STRINGIFY(expression) "' in " LASS_HERE)

#define PY_ENFORCE_NOTZERO(expression)\
		*LASS_UTIL_IMPL_MAKE_ENFORCER(\
		::lass::util::impl::UnequalPredicate,\
		::lass::python::impl::PythonFetchRaiser,\
		(expression), \
		int(0), \
		"'" LASS_STRINGIFY(expression) "' in " LASS_HERE)
#endif
