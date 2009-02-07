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

#ifndef LASS_GUARDIAN_OF_INCLUSION_UTIL_MODULE_DEFINITION_H
#define LASS_GUARDIAN_OF_INCLUSION_UTIL_MODULE_DEFINITION_H

#include "python_common.h"
#include "pyobject_plus.h"

namespace lass
{
namespace python
{
namespace impl
{

/** @internal 
 *  Returns a  pair<short name, pyobject pointing at the class object>
 *  We have to work around the static initializer fiasco by demanding the iModuleName instead of pealing 
 *	it from the module object (which may not be initialized yet... and then we have *kaboom*)
 */
template <typename CppClass>
inline std::pair<std::string,PyObject*> prepareClassForModuleInjection(const char* iModuleName, const char* iClassDocumentation)
{
	char* shortName = const_cast<char*>(CppClass::_lassPyClassDef.name()); // finalizePyType will expand tp_name with module name.
	finalizePyType(CppClass::_lassPyClassDef, CppClass::_lassPyGetParentType(), iModuleName, iClassDocumentation);
	return std::pair<std::string,PyObject*>(std::string(shortName), reinterpret_cast<PyObject*>(CppClass::_lassPyClassDef.type()));
}

class LASS_DLL ModuleDefinition: util::NonCopyable
{
public:
	ModuleDefinition(const char* name, const char* doc = 0);
	const char* name() const { return name_.get(); }
	void setName(const char* name);
	const char* doc() const { return doc_.get(); }
	void setDoc(const char* doc);
	PyObject* module() const { return module_; }
	void addFunctionDispatcher(PyCFunction dispatcher, const char* name, const char* doc, PyCFunction& overloadChain);
	template <typename CppClass> void injectClass(const char* doc)
	{
		std::pair<std::string, PyObject*> c = prepareClassForModuleInjection<CppClass>(name_.get(), doc);
		PyModule_AddObject(module_, const_cast<char*>(c.first.c_str()), c.second);
	}
	PyObject* inject();
private:
	typedef std::vector<PyMethodDef> TMethods;
	struct NamedObject
	{
		char* name;
		PyObject* object;
	};
	typedef std::vector<NamedObject> TObjects;
	typedef util::ScopedPtr< char, util::ArrayStorage > TScopedCString;
	TMethods methods_;
	TObjects objects_;
	TScopedCString name_;
	TScopedCString doc_;
	PyObject* module_;
#if PY_MAJOR_VERSION >= 3
	PyModuleDef def_;
#endif
	bool isInjected_;
};

}
}
}

#endif
