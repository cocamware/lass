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
 *	Copyright (C) 2004-2009 the Initial Developer.
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

#include "lass_common.h"
#include "utilities.h"
#include "exception.h"
#include "pyobject_plus.h"

namespace lass
{
namespace python
{
namespace impl
{

TPyObjPtr runString(const char *code, int start)
{
	const TPyObjPtr dict = globals();
	const TPyObjPtr result(PY_ENFORCE_POINTER(PyRun_String(code, start, dict.get(), dict.get())));
	return result;
}

}

/** @ingroup Python
 *  @brief retrieve pointer to PyObject by its name in the script.
 *  @return 
 *	new reference to PyObject @a iName or NULL if 
 *	@a iName does not exist (_without_ setting an exception!)
 */
TPyObjPtr getPyObjectByName(const std::string& iName)
{
	const TPyObjPtr dict = globals();
	const TPyObjPtr key(PY_ENFORCE_POINTER(pyBuildSimpleObject(iName)));
	PyObject* const object = PyDict_GetItem(dict.get(), key.get());
	return fromNakedToSharedPtrCast<PyObject>(object);
}



/** @ingroup Python
 */
TPyObjPtr globals()
{
	PyObject* const module = PY_ENFORCE_POINTER(PyImport_AddModule("__main__"));
	PyObject* const dict = PY_ENFORCE_POINTER(PyModule_GetDict(module));
	return fromNakedToSharedPtrCast<PyObject>(dict);
}



/** @ingroup Python
 */
void execute(const std::string& code)
{
	execute(code.c_str());
}



/** @ingroup Python
 */
void execute(const char* code)
{
	impl::runString(code, Py_file_input);
}



/** @ingroup Python
 */
TPyObjPtr evaluate(const std::string& code)
{
	return evaluate(code.c_str());
}



/** @ingroup Python
 */
TPyObjPtr evaluate(const char* code)
{
	return impl::runString(code, Py_eval_input);
}



/** os.environ[key] = value
 *  @ingroup Python
 */
void putenv(const std::string& key, const std::string& value)
{
	putenv(key.c_str(), value.c_str());
}



/** os.environ[key] = value
 *  @ingroup Python
 */
void putenv(const char* key, const char* value)
{
	const TPyObjPtr item(PY_ENFORCE_POINTER(pyBuildSimpleObject(value)));
	const TPyObjPtr os(PY_ENFORCE_POINTER(PyImport_ImportModule("os")));
	PyObject* const dict = PY_ENFORCE_POINTER(PyModule_GetDict(os.get()));
	PyObject* const os_environ = PY_ENFORCE_POINTER(PyDict_GetItemString(dict, "environ"));
	PY_ENFORCE_ZERO(PyMapping_SetItemString(os_environ, const_cast<char*>(key), item.get()));
}


}
}

// EOF
