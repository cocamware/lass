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

#ifndef LASS_GUARDIAN_OF_INCLUSION_UTIL_MODULE_DEFINITION_H
#define LASS_GUARDIAN_OF_INCLUSION_UTIL_MODULE_DEFINITION_H

#include "python_common.h"
#include "pyobject_plus.h"
#include "../util/callback_0.h"
#include "../util/callback_1.h"

namespace lass
{
namespace python
{

class LASS_DLL ModuleDefinition: util::NonCopyable
{
public:
	typedef util::Callback0 TPreInject;
	typedef util::Callback1<PyObject*> TPostInject;

	ModuleDefinition(const char* name, const char* doc = 0);
	const char* name() const { return name_.get(); }
	void setName(const char* name);
	const char* doc() const { return doc_.get(); }
	void setDoc(const char* doc);
	PyObject* module() const { return module_; }

	void setPreInject(const TPreInject& callback);
	void setPostInject(const TPostInject& callback);

	void addFunctionDispatcher(PyCFunction dispatcher, const char* name, const char* doc, PyCFunction& overloadChain);
	void addClass(impl::ClassDefinition& classDef);
	void addObject(PyObject* object, const char* name);
	void addLong(long object, const char* name);
	void addString(const char* object, const char* name);

	void injectClass(impl::ClassDefinition& classDef);
	PyObject* inject();
private:
	typedef util::ScopedPtr<char, util::ArrayStorage> TScopedCString;
	typedef std::vector<impl::ClassDefinition*> TClassDefs;
	typedef std::vector<PyMethodDef> TMethods;
	struct NamedObject
	{
		TScopedCString name;
		PyObject* object;
	};
	struct LongObject
	{
		TScopedCString name;
		long object;
	};
	struct StringObject
	{
		TScopedCString name;
		TScopedCString object;
	};
	typedef std::vector<NamedObject*> TObjects;
	typedef std::vector<LongObject*> TLongObjects;
	typedef std::vector<StringObject*> TStringObjects;
	TClassDefs classes_;
	TMethods methods_;
	TObjects objects_;
	TLongObjects longObjects_;
	TStringObjects stringObjects_;
	TScopedCString name_;
	TScopedCString doc_;
	TPreInject preInject_;
	TPostInject postInject_;
	PyObject* module_;
#if PY_MAJOR_VERSION >= 3
	PyModuleDef def_;
#endif
	bool isInjected_;
};

}
}

#endif
