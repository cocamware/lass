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

#ifndef LASS_GUARDIAN_OF_INCLUSION_UTIL_BULK_ADD_H
#define LASS_GUARDIAN_OF_INCLUSION_UTIL_BULK_ADD_H

#include "python_common.h"
#include "module_definition.h"

/** @ingroup Python
 *	Injects a number of constants into a Python module.  The number of arguments to the macro
 *  is variable but is limited to the number specified during the build of lass.
 *  The name of the identifier in Python is the same as the one in C++.  This makes export of
 *	enums more efficient.
 *
 *  An example is:
 *		enum Foo 
 *		{
 *			the = 0x1,
 *			fan = 0x2,
 *			has = 0x5,
 *			started = 0x7
 *		};
 *			
 *		PY_MODULE_INTEGER_CONSTANTS( theFanModule, the, fan, has, started )
 *
 *
 *  @param i_module
 *		the identifier of a module declared by PY_DECLARE_MODULE
 *  @param ...
 *		the identifiers that will be injected
 */
#define PY_MODULE_INTEGER_CONSTANTS( i_module , ... ) \
	LASS_EXECUTE_BEFORE_MAIN_EX\
	( LASS_CONCATENATE( lassExecutePyModuleIntegerConstants_, i_module),\
		lass::python::impl::addIntegerConstantsToModule( i_module, #__VA_ARGS__, __VA_ARGS__); )

namespace lass
{
namespace python
{
namespace impl
{

	LASS_PYTHON_DLL std::string argument(const std::string& iValue, size_t iArg);
	
	void addIntegerConstantToModule( ModuleDefinition& iModule, long iValue, const std::string& iName );
}

}

}	// namespace lass

#include "bulk_add_integer.inl"

#endif
