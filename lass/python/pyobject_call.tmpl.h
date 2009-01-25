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
/**
*   @author Bram de Greve (bram@cocamware.com)
*   @author Tom De Muer (tom@cocamware.com)
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

#include "python_common.h"

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
	EXECUTE( Object::_lassPyMethods.insert(Object::_lassPyMethods.begin(),\
		lass::python::createPyMethodDef(pyName, (PyCFunction) pyFunction, PY_NEWARGS, NULL)); )



#include "pyobject_call.inl"

#endif

// EOF
