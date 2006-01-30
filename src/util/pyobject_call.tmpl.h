/**
*   @author Bram de Greve (bramz@users.sourceforge.net)
*   @author Tom De Muer (tomdemuer@users.sourceforge.net)
*
*   Distributed under the terms of the GPL (GNU Public License)
*
*   The LASS License:
*
*   Copyright 2004-2006 Bram de Greve and Tom De Muer
*
*   LASS is free software; you can redistribute it and/or modify
*   it under the terms of the GNU General Public License as published by
*   the Free Software Foundation; either version 2 of the License, or
*   (at your option) any later version.
*
*   This program is distributed in the hope that it will be useful,
*   but WITHOUT ANY WARRANTY; without even the implied warranty of
*   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
*   GNU General Public License for more details.
*
*   You should have received a copy of the GNU General Public License
*   along with this program; if not, write to the Free Software
*   Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
*


 *  @date 2004
 */

#ifndef LASS_GUARDIAN_OF_INCLUSION_UTIL_PYOBJECT_CALL_H
#define LASS_GUARDIAN_OF_INCLUSION_UTIL_PYOBJECT_CALL_H

#include "util_common.h"

#define PY_CALLABLE_FILEMETHOD( function )\
	PY_CALLABLE_FILEMETHOD_EX( function, LASS_CONCATENATE(py, function), LASS_STRINGIFY(function) )



#define PY_CALLABLE_FILEMETHOD_EX( function, pyFunction, pyName )\
	inline PyObject* pyFunction( PyObject* iIgnore, PyObject* iArgs )\
	{\
		return lass::python::impl::pyCallFunction( iArgs, function );\
	}\
	PY_EXPORTFILEMETHOD( pyFunction, pyName )



#define PY_CALLABLE_METHOD( Object, method )\
	PY_CALLABLE_METHOD_EX( Object, method, LASS_CONCATENATE(py, LASS_CONCATENATE(Object, method)),\
		LASS_STRINGIFY(method) )



#define PY_CALLABLE_METHOD_EX( Object, method, pyFunction, pyName )\
	inline PyObject* pyFunction( PyObject* iObject, PyObject* iArgs )\
	{\
		Object* object = static_cast<Object*>(iObject);\
		return lass::python::impl::PyCallMethod<Object>::call( iArgs, object, Object::method );\
	}\
	EXECUTE( Object::Methods.insert(Object::Methods.begin(),\
		lass::python::createPyMethodDef(pyName, (PyCFunction) pyFunction, PY_NEWARGS, NULL)); )



#include "pyobject_call.inl"

#endif

// EOF
