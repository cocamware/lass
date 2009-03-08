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
#include "module_definition.h"

namespace lass
{
namespace python
{
namespace impl
{

ModuleDefinition::ModuleDefinition(const char* name, const char* doc):
	module_(0),
	isInjected_(false)
{
#if PY_MAJOR_VERSION >= 3 
	PyModuleDef def = {
		PyModuleDef_HEAD_INIT,
		0, /* m_name */
		0, /* m_doc */
		0, /* m_size */
		0, /* m_methods */
		0, /* m_reload */
		0, /* m_traverse */
		0, /* m_clear */
		0, /* m_free */
	};
	def_ = def;
#endif
	setName(name);
	setDoc(doc);
}

namespace experimental
{
	typedef util::ScopedPtr< char, util::ArrayStorage > TScopedCString;
	void assignScopedCString(TScopedCString& out, const char* in)
	{
		if (in)
		{
			const size_t n = strlen(in) + 1;
			TScopedCString temp(new char[n]);
			memcpy(temp.get(), in, n);
			out.swap(temp);
		}
		else
		{
			out.reset();
		}
	}
}

void ModuleDefinition::setName(const char* name)
{
	experimental::assignScopedCString(name_, name);
}

void ModuleDefinition::setDoc(const char* doc)
{
	experimental::assignScopedCString(doc_, doc);
}

void ModuleDefinition::addFunctionDispatcher(
		PyCFunction dispatcher, const char* name, const char* doc, PyCFunction& overloadChain)
{
	TMethods::iterator i = ::std::find_if(methods_.begin(), methods_.end(), PyMethodEqual(name));
	if (i == methods_.end())
	{
		methods_.push_back(createPyMethodDef(name, dispatcher, METH_VARARGS , doc));
		overloadChain = 0;
	}
	else
	{
		overloadChain = i->ml_meth;
		i->ml_meth = dispatcher;
	};
}

void ModuleDefinition::injectClass(ClassDefinition& classDef)
{
	const char* shortName = classDef.name(); // finalizePyType will expand tp_name with module name.
	classDef.freezeDefinition(name_.get());
	PyModule_AddObject(module_, const_cast<char*>(shortName), reinterpret_cast<PyObject*>(classDef.type()));
}

PyObject* ModuleDefinition::inject()
{
	LASS_ASSERT(!isInjected_);
	LASS_ASSERT(name_.get());
	methods_.push_back(impl::createPyMethodDef(0, 0, 0, 0));
	Py_Initialize();
#if PY_MAJOR_VERSION < 3
	module_ = Py_InitModule3(name_.get(), &methods_[0], doc_.get());
#else
	def_.m_name = name_.get();
	def_.m_doc = doc_.get();
	def_.m_methods = &methods_[0];
	module_ = PyModule_Create(&def_);
#endif
	for (TObjects::const_iterator obj = objects_.begin(); obj != objects_.end(); ++obj)
	{
		PyModule_AddObject(module_, obj->name, obj->object);
	}
	isInjected_ = true;
	return module_;
}

}
}
}

// EOF