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

#include "python_common.h"
#include "streams.h"
#include "gil.h"

namespace lass
{
namespace python
{
namespace impl
{

SysStreamBuf::SysStreamBuf(FILE* file, const char* name): 
	file_(file),
	name_(name)
{
	// make sure you have room for one more + \0
	setp(buffer_, buffer_ + (bufferSize_ - 2));
}



SysStreamBuf::~SysStreamBuf()
{
	sync();
}


SysStreamBuf::int_type SysStreamBuf::overflow(int_type c)
{
	if (c != EOF)
	{
		// we have room for one more!
		*pptr() = static_cast<char>(c);
		pbump(1);
	}
	if (sync() != 0)
	{
		return EOF;
	}
	return c;
}


int SysStreamBuf::sync()
{
	const int n = static_cast<int>(pptr() - pbase());
	LASS_ASSERT(n >= 0 && (pbase() + n == pptr()));
	if (n == 0)
	{
		return 0;
	}
	buffer_[n] = 0;

	bool ok = false;
	if (Py_IsInitialized())
	{
		LockGIL LASS_UNUSED(lock);
		PyObject* obj = PySys_GetObject(const_cast<char*>(name_));
		if (obj)
		{
			ok = PyFile_WriteString(buffer_, obj) == 0;
			if (!ok)
			{
				PyErr_Clear();
			}
		}
	}

	if (!ok)
	{
		// fall back on the C stream
		ok = fputs(buffer_, file_) != EOF;
	}

	pbump(-n);
	return ok ? 0 : -1;
}


SysOStream::SysOStream(FILE* file, const char* name):
	std::ostream(&buffer_),
	buffer_(file, name)
{
	setf(std::ios_base::unitbuf); // disable buffering so that it gets flushed after each insertion.
}


}


impl::SysOStream sysStdout(stdout, "stdout");
impl::SysOStream sysStderr(stderr, "stderr");

}
}
