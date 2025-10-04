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

/** @defgroup ModuleDefinition Module Definitions
 *  @brief Defining Python modules from C++ with classes, functions, and enums.
 *
 *  This module provides helper classes and macros to define Python modules that can contain
 *  C++ classes, functions, enums, and other objects exported to Python.
 * 
 *
 *  ### Module Components
 * 
 *  A Python module can contain:
 * 
 *  - **Functions**: C++ functions exported with various signatures
 *  - **Classes**: C++ classes exported as Python classes
 *  - **Enums**: C++ enums exported as Python enum types
 *  - **Objects**: Arbitrary Python objects (constants, instances, etc.)
 *  - **Values**: Simple values like integers and strings
 *
 *
 *  ### Usage Overview
 * 
 *  To create a Python module, you typically use macros that work with ModuleDefinition:
 *  
 *  ```cpp
 *  // Define module
 *  PY_DECLARE_MODULE_NAME_DOC(mymodule, "mymodule", "My example module")
 *  
 *  // Add functions
 *  PY_MODULE_FUNCTION(mymodule, myFunction)
 *  
 *  // Add classes
 *  PY_MODULE_CLASS(mymodule, MyClass)
 *  
 *  // Add enums
 *  PY_MODULE_ENUM(mymodule, MyEnum)
 *  
 *  // Create module entrypoint for Python extension
 *  PY_MODULE_ENTRYPOINT(mymodule)
 *  ```
 * 
 *  The ModuleDefinition class is typically not used directly. Instead, use the provided
 *  macros in pyobject_macros.h that work with ModuleDefinition instances.
 *
 *  @ingroup Python
 */

class EnumDefinitionBase;

/** Definition of a Python module.
 *
 *  Holds the definition of a Python module and can create the module by calling inject().
 *  This class manages all components that make up a Python module: functions, classes,
 *  enums, and other objects.
 *
 *  The module definition accumulates all components during the program initialization
 *  phase, then creates the actual Python module when inject() is called.
 *
 *  This class is typically not used directly by user code. Use the provided
 *  macros in pyobject_macros.h that work with ModuleDefinition instances.
 *
 *  @ingroup ModuleDefinition
 */
class LASS_PYTHON_DLL ModuleDefinition: util::NonCopyable
{
public:
	/** Callback type for pre-injection hooks (called before module creation). */
	typedef util::Callback0 TPreInject;
	
	/** Callback type for post-injection hooks (called after module creation with module object). */
	typedef util::Callback1<PyObject*> TPostInject;

	/** Construct module definition with name and optional documentation.
	 *  @param name Python module name (will be copied and stored internally)
	 *  @param doc Optional module docstring (will be copied and stored internally, or nullptr)
	 */
	ModuleDefinition(const char* name, const char* doc = 0);
	/** Get the module name. */
	const char* name() const { return name_.get(); }
	
	/** Set the module name.
	 *  @param name New module name (will be copied and stored internally)
	 */
	void setName(const char* name);
	
	/** Get the module documentation string. */
	const char* doc() const { return doc_.get(); }
	
	/** Set the module documentation string.
	 *  @param doc New docstring (will be copied and stored internally, or nullptr)
	 */
	void setDoc(const char* doc);
	
	/** Get the Python module object (available after inject() has been called). */
	PyObject* module() const { return module_; }

	/** Set callback to be executed before module injection.
	 *  Useful for performing setup tasks before the module is created.
	 *  @param callback Function to call before injection
	 */
	void setPreInject(const TPreInject& callback);
	
	/** Set callback to be executed after module injection.
	 *  Useful for performing additional setup on the created module.
	 *  @param callback Function to call with the module object after injection
	 */
	void setPostInject(const TPostInject& callback);

	/** Add a function dispatcher to the module.
	 *  Used internally by function export macros to register C++ functions.
	 *  @param dispatcher Function dispatcher for overload resolution
	 *  @param name Python function name
	 *  @param doc Function documentation string
	 *  @param overloadChain Reference to overload chain for this function name
	 */
	void addFunctionDispatcher(PyCFunction dispatcher, const char* name, const char* doc, PyCFunction& overloadChain);
	
	/** Add a class definition to the module.
	 *  The class will be created and added when the module is injected.
	 *  @param classDef Class definition containing class export information
	 */
	void addClass(impl::ClassDefinition& classDef);
	
	/** Add an enum definition to the module.
	 *  The enum type will be created and added when the module is injected.
	 *  @param enumDef Enum definition containing enum export information
	 */
	void addEnum(EnumDefinitionBase* enumDef);
	
	/** Add an arbitrary Python object to the module.
	 *  The object will be added directly to the module namespace.
	 *  @param object Python object to add
	 *  @param name Name for the object in the module namespace
	 */
	void addObject(PyObject* object, const char* name);
	
	/** Add a long integer constant to the module.
	 *  @param object Long value to add as module constant
	 *  @param name Name for the constant in the module namespace
	 */
	void addLong(long object, const char* name);
	
	/** Add a string constant to the module.
	 *  @param object String value to add as module constant
	 *  @param name Name for the string in the module namespace
	 */
	void addString(const char* object, const char* name);

	/** Inject a long integer directly into an already created module.
	 *  This is for immediate injection, unlike addLong() which defers until inject().
	 *  @param name Name for the constant in the module namespace
	 *  @param value Long value to inject
	 */
	void injectLong(const char* name, long value);
	
	/** Inject a string directly into an already created module.
	 *  This is for immediate injection, unlike addString() which defers until inject().
	 *  @param name Name for the string in the module namespace
	 *  @param value String value to inject
	 */
	void injectString(const char* name, const char* value);
	
	/** Inject an arbitrary object directly into an already created module.
	 *  This is for immediate injection, unlike addObject() which defers until inject().
	 *  @param object Object to inject (will be converted to Python object)
	 *  @param name Name for the object in the module namespace
	 */
	template <typename T>
	void injectObject(T&& object, const char* name)
	{
		PyModule_AddObject(module_, name, lass::python::pyBuildSimpleObject( std::forward<T>(object) ));
	}

	/** Inject a class definition directly into an already created module.
	 *  @param classDef Class definition to inject
	 *  @return true on success, false on failure
	 */
	bool injectClass(impl::ClassDefinition& classDef);
	
	/** Create and inject the Python module with all accumulated definitions.
	 *  This method should typically not be called directly. Use module registration
	 *  macros which call this at the appropriate time during Python initialization.
	 *  @return The created Python module object
	 */
	PyObject* inject();
private:
	typedef std::unique_ptr<char[]> TScopedCString;
	typedef std::vector<impl::ClassDefinition*> TClassDefs;
	typedef std::vector<EnumDefinitionBase*> TEnumDefs;
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

	/** Implementation of module injection process. */
	PyObject* doInject();

	TClassDefs classes_;
	TEnumDefs enums_;
	TMethods methods_;
	TObjects objects_;
	TLongObjects longObjects_;
	TStringObjects stringObjects_;
	TScopedCString name_;
	TScopedCString doc_;
	TPreInject preInject_;
	TPostInject postInject_;
	PyObject* module_;
	PyModuleDef def_;
	bool isInjected_;
};

}
}

#endif
