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
 *	Copyright (C) 2004-2008 the Initial Developer.
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
namespace impl
{
	LASS_DLL const std::string exceptionExtractMessage(const TPyObjPtr& type, const TPyObjPtr& value);
	LASS_DLL void LASS_CALL fetchAndThrowPythonException(const std::string& loc = "");
}

class PythonException: public util::Exception
{
public:
	PythonException(
			const TPyObjPtr& type, const TPyObjPtr& value, const TPyObjPtr& traceback, 
			const std::string& loc):
		util::Exception(impl::exceptionExtractMessage(type, value), loc),
		type_(type),
		value_(value),
		traceback_(traceback)
	{
	}
	~PythonException() throw() {}
	const python::TPyObjPtr& type() const { return type_; } 
	const python::TPyObjPtr& value() const { return value_; } 
	const python::TPyObjPtr& traceback() const { return traceback_; } 
private:
	LASS_UTIL_EXCEPTION_PRIVATE_IMPL(PythonException)
	python::TPyObjPtr type_;
	python::TPyObjPtr value_;
	python::TPyObjPtr traceback_;
};

}
}

#endif