/**	@file
 *	@author Bram de Greve (bramz@users.sourceforge.net)
 *	@author Tom De Muer (tomdemuer@users.sourceforge.net)
 *
 *	Distributed under the terms of the GPL (GNU Public License)
 *
 * 	The LASS License:
 *
 *	Copyright 2004 Bram de Greve and Tom De Muer
 *
 *	LASS is free software; you can redistribute it and/or modify
 *	it under the terms of the GNU General Public License as published by
 *	the Free Software Foundation; either version 2 of the License, or
 *	(at your option) any later version.
 *
 *	This program is distributed in the hope that it will be useful,
 *	but WITHOUT ANY WARRANTY; without even the implied warranty of
 *	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *	GNU General Public License for more details.
 *
 *	You should have received a copy of the GNU General Public License
 *	along with this program; if not, write to the Free Software
 *	Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */

#include "util_common.h"
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

    typedef python::PyObjectPtr<PyObject>::Type TPyPtr;
    TPyPtr type(tempType);
    TPyPtr value(tempValue);
    TPyPtr traceback(tempTraceback);

    std::ostringstream buffer;
    if (PyErr_GivenExceptionMatches(type.get(), PyExc_TypeError))
    {
        buffer << "Python function does not match signature of C++ callback.  It complained with: ";
    }
    else
    {
        buffer << "Python function in callback failed: "; 
    }

    TPyPtr typeStr(PyObject_Str(type.get()));
    buffer << (typeStr ? PyString_AsString(typeStr.get()) : "unknown python exception");

    TPyPtr valueStr(value.get() == Py_None ? 0 : PyObject_Str(value.get()));
    if (valueStr)
    {
        buffer << ": '" << PyString_AsString(valueStr.get()) << "'";
    }

    return buffer.str();
}

}
}
}
