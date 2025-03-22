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
 *	Copyright (C) 2022-2025 the Initial Developer.
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

#pragma once

#include "python_common.h"
#include "export_traits.h"

#if LASS_HAVE_STD_FILESYSTEM

#include <filesystem>

namespace lass
{
namespace python
{

/** @ingroup Python
 *
 *  Accepts bytes, str or objects that implement the os.PathLike interface (having __fspath__())
 *  If conversion is needed from bytes to str (on Windows) or from str to bytes (on POSIX),
 *  then it is performed using sys.getfilesystemencoding() (which is usually UTF-8, but not always).
 * 
 *  Always converts them to str instances.
 */
template <>
struct PyExportTraits<std::filesystem::path>
{
	constexpr static const char* py_typing = "str | bytes | os.PathLike";

	LASS_PYTHON_DLL static PyObject* build(const std::filesystem::path& v);
	LASS_PYTHON_DLL static int get(PyObject* obj, std::filesystem::path& v);
};

}
}

#endif

// EOF
