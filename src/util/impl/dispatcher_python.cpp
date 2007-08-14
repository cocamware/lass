/**	@file
 *	@author Bram de Greve (bramz@users.sourceforge.net)
 *	@author Tom De Muer (tomdemuer@users.sourceforge.net)
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
 *	Copyright (C) 2004-2007 the Initial Developer.
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

#include "util_common.h"
#include "../pyobject_plus.h"
#include "dispatcher_python.h"

namespace lass
{
namespace util
{
namespace impl
{

std::string fetchException()
{
	if (!PyErr_Occurred())
	{
		return "internal error: fetchException called while Python exception is not set";
	}

	PyObject *tempType, *tempValue, *tempTraceback;
	PyErr_Fetch(&tempType, &tempValue, &tempTraceback);

	python::TPyObjPtr type(tempType);
	python::TPyObjPtr value(tempValue);
	python::TPyObjPtr traceback(tempTraceback);

	std::ostringstream buffer;
	if (PyErr_GivenExceptionMatches(type.get(), PyExc_TypeError))
	{
		buffer << "Python function does not match signature of C++ callback.  It complained with: ";
	}
	else
	{
		buffer << "Python function in callback failed: ";
	}

	python::TPyObjPtr typeStr(PyObject_Str(type.get()));
	buffer << (typeStr ? PyString_AsString(typeStr.get()) : "unknown python exception");

	python::TPyObjPtr valueStr(value.get() == Py_None ? 0 : PyObject_Str(value.get()));
	if (valueStr)
	{
		buffer << ": '" << PyString_AsString(valueStr.get()) << "'";
	}

	return buffer.str();
}

}
}
}
