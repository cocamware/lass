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
#include "overload_link.h"
#include "py_tuple.h"

namespace lass
{
namespace python
{
namespace impl
{

OverloadLink::OverloadLink()
{
	setNull();
}

void OverloadLink::setNull()
{
	signature_ = sNull;
}

void OverloadLink::setPyCFunction(PyCFunction iOverload)
{
	signature_ = iOverload ? sPyCFunction : sNull;
	pyCFunction_ = iOverload;
}

void OverloadLink::setBinaryfunc(binaryfunc iOverload)
{
	signature_ = iOverload ? sBinary : sNull;
	binaryfunc_ = iOverload;
}

void OverloadLink::setTernaryfunc(ternaryfunc iOverload)
{
	signature_ = iOverload ? sTernary : sNull;
	ternaryfunc_ = iOverload;
}
void OverloadLink::setSsizeArgfunc(ssizeargfunc iOverload)
{
	signature_ = iOverload ? sSsizeArg : sNull;
	ssizeargfunc_ = iOverload;
}
void OverloadLink::setSsizeSsizeArgfunc(ssizessizeargfunc iOverload)
{
	signature_ = iOverload ? sSsizeSsizeArg : sNull;
	ssizessizeargfunc_ = iOverload;
}

void OverloadLink::setSsizeObjArgProcfunc(ssizeobjargproc iOverload)
{
	signature_ = iOverload ? sSsizeObjArg : sNull;
	ssizeobjargproc_ = iOverload;
}
void OverloadLink::setSsizeSsizeObjArgProcfunc(ssizessizeobjargproc iOverload)
{
	signature_ = iOverload ? sSsizeSsizeObjArg : sNull;
	ssizessizeobjargproc_ = iOverload;
}
void OverloadLink::setObjObjProcfunc(objobjproc iOverload)
{
	signature_ = iOverload ? sObjObjArg : sNull;
	objobjproc_ = iOverload;
}
void OverloadLink::setObjObjArgProcfunc(objobjargproc iOverload)
{
	signature_ = iOverload ? sObjObjArg : sNull;
	objobjargproc_ = iOverload;
}
void OverloadLink::setArgKwfunc(ternaryfunc iOverload)
{
	signature_ = iOverload ? sArgKw : sNull;
	ternaryfunc_ = iOverload;
}

bool OverloadLink::operator ()(PyObject* iSelf, PyObject* iArgs, PyObject*& oResult) const
{
	if (signature_ == sNull)
	{
		return false;
	}
	PyObject* temp = call(iSelf, iArgs);
	if (PyErr_Occurred() && (PyErr_ExceptionMatches(PyExc_TypeError) || PyErr_ExceptionMatches(PyExc_NotImplementedError)))
	{
		PyErr_Clear();
		Py_XDECREF(temp);
		return false;
	}
	oResult = temp;
	return true;
}

PyObject* OverloadLink::call(PyObject* iSelf, PyObject* iArgs) const
{
	switch (signature_)
	{
	case sNull:
		return 0;

	case sPyCFunction:
		return pyCFunction_(iSelf, iArgs);

	case sBinary:
		{
			TPyObjPtr arg;
			if (decodeTuple(iArgs, arg) != 0)
			{
				return 0;
			}
			return binaryfunc_(iSelf, arg.get());
		}

	case sTernary:
		{
			TPyObjPtr arg1, arg2;
			if (decodeTuple(iArgs, arg1, arg2) != 0)
			{
				return 0;
			}
			return ternaryfunc_(iSelf, arg1.get(), arg2.get());
		}

	case sSsizeArg:
		{
			Py_ssize_t size1;
			if (decodeTuple(iArgs, size1) != 0)
			{
				return 0;
			}
			return ssizeargfunc_(iSelf, size1 );
		}

	case sSsizeSsizeArg:
		{
			Py_ssize_t size1;
			Py_ssize_t size2;
			if (decodeTuple(iArgs, size1, size2) != 0)
			{
				return 0;
			}
			return ssizessizeargfunc_(iSelf, size1, size2 );
		}
	
	case sSsizeObjArg:
		{
			Py_ssize_t size1;
			TPyObjPtr obj1;
			if (decodeTuple(iArgs, size1, obj1) != 0)
			{
				return 0;
			}
			return pyBuildSimpleObject(ssizeobjargproc_(iSelf,size1,obj1.get()));
		}

	case sSsizeSsizeObjArg:
		{
			Py_ssize_t size1;
			Py_ssize_t size2;
			TPyObjPtr obj1;
			if (decodeTuple(iArgs, size1, size2, obj1) != 0)
			{
				return 0;
			}
			return pyBuildSimpleObject(ssizessizeobjargproc_(iSelf,size1,size2,obj1.get()));
		}

	case sObjObj:
		{
			TPyObjPtr obj1;
			if (decodeTuple(iArgs, obj1) != 0)
			{
				return 0;
			}
			return pyBuildSimpleObject(objobjproc_(iSelf,obj1.get()));		
		}

	case sArgKw:
		return ternaryfunc_(iSelf, iArgs, 0);

	default:
		PyErr_SetString(PyExc_AssertionError, "OverloadChain: invalid signature type");
		return 0;
	};
}

}
}
}

// EOF
