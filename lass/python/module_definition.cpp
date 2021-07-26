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
#include "module_definition.h"

namespace lass
{
namespace python
{

ModuleDefinition::ModuleDefinition(const char* name, const char* doc):
	module_(0),
	isInjected_(false)
{
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

	setName(name);
	setDoc(doc);
}

namespace experimental
{
	typedef std::unique_ptr<char[]> TScopedCString;
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

/** callback called at start of module injection
 */
void ModuleDefinition::setPreInject(const TPreInject& callback)
{
	preInject_ = callback;
}

/** callback called at end of module injection, with module as parameter
 */
void ModuleDefinition::setPostInject(const TPostInject& callback)
{
	postInject_ = callback;
}

/** new-style class addition.
 *
 *  This function adds a class definition to a table that will be injected when the module is.
 */
void ModuleDefinition::addClass(impl::ClassDefinition& classDef)
{
	LASS_ASSERT(!isInjected_);
	classes_.push_back(&classDef);
}

void ModuleDefinition::addObject(PyObject* object, const char* name)
{
	LASS_ASSERT(!isInjected_);
	NamedObject *tempObject = new NamedObject;
	experimental::assignScopedCString(tempObject->name, name);
	tempObject->object = object;
	objects_.push_back(tempObject);
}

void ModuleDefinition::addLong(long object, const char* name)
{
	LASS_ASSERT(!isInjected_);
	LongObject *tempObject = new LongObject;
	experimental::assignScopedCString(tempObject->name, name);
	tempObject->object = object;
	longObjects_.push_back(tempObject);
}

void ModuleDefinition::addString(const char* object, const char* name)
{
	LASS_ASSERT(!isInjected_);
	StringObject* tempObject = new StringObject;
	experimental::assignScopedCString(tempObject->name, name);
	experimental::assignScopedCString(tempObject->object, object);
	stringObjects_.push_back(tempObject);
}


void ModuleDefinition::addFunctionDispatcher(
		PyCFunction dispatcher, const char* name, const char* doc, PyCFunction& overloadChain)
{
	TMethods::iterator i = ::std::find_if(methods_.begin(), methods_.end(), impl::NamePredicate(name));
	if (i == methods_.end())
	{
		methods_.push_back(impl::createPyMethodDef(name, dispatcher, METH_VARARGS , doc));
		overloadChain = 0;
	}
	else
	{
		overloadChain = i->ml_meth;
		i->ml_meth = dispatcher;
	};
}

/** old-style class injection.
 *
 *  This function can injects a class in an already-injected module
 *  @deprecated
 */
void ModuleDefinition::injectClass(impl::ClassDefinition& classDef)
{
	LASS_ASSERT(isInjected_);
	const char* shortName = classDef.name(); // finalizePyType will expand tp_name with module name.
	classDef.freezeDefinition(name_.get());
	PyModule_AddObject(module_, const_cast<char*>(shortName), reinterpret_cast<PyObject*>(classDef.type()));
}

PyObject* ModuleDefinition::inject()
{
	LASS_ASSERT(!isInjected_);
	LASS_ASSERT(name_.get());
	preInject_();
	methods_.push_back(impl::createPyMethodDef(0, 0, 0, 0));
	Py_Initialize();
#if PY_VERSION_HEX < 0x03090000 // < 3.9
	PyEval_InitThreads(); // see http://stackoverflow.com/questions/8451334/why-is-pygilstate-release-throwing-fatal-python-errors
#endif
	def_.m_name = name_.get();
	def_.m_doc = doc_.get();
	def_.m_methods = &methods_[0];
	module_ = PyModule_Create(&def_);
	isInjected_ = true;
	for (TClassDefs::const_iterator def = classes_.begin(); def != classes_.end(); ++def)
	{
		injectClass(**def);
	}
	for (TObjects::const_iterator obj = objects_.begin(); obj != objects_.end(); ++obj)
	{
		PyModule_AddObject(module_, (*obj)->name.get(), (*obj)->object);
	}
	for (TLongObjects::const_iterator obj = longObjects_.begin(); obj != longObjects_.end(); ++obj)
	{
		PyModule_AddIntConstant(module_, (*obj)->name.get(), (*obj)->object);
	}
	for (TStringObjects::const_iterator obj = stringObjects_.begin(); obj != stringObjects_.end(); ++obj)
	{
		PyModule_AddStringConstant(module_, (*obj)->name.get(), (*obj)->object.get());
	}
	postInject_(module_);
	return module_;
}

}
}

// EOF
