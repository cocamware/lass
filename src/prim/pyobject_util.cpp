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

#include "lass_common.h"
#include "pyobject_util.h"
#include "triangle_mesh_3d.h"

namespace lass
{
namespace python
{
namespace impl
{

/** @ingroup Python
 *  @internal
 */
PyObject* buildIndexVertex(size_t iVertex, size_t iNormal, size_t iUv)
{
	python::PyObjectPtr<PyObject>::Type tuple;
	
	LASS_ASSERT(iVertex != prim::IndexTriangle::null());
	if (iNormal == prim::IndexTriangle::null())
	{
		if (iUv == prim::IndexTriangle::null())
		{
			tuple = python::makeTuple(iVertex);
		}
		else
		{
			tuple = python::makeTuple(iVertex, python::fromNakedToSharedPtrCast<PyObject>(Py_None), iUv);
		}
	}
	else
	{
		if (iUv == prim::IndexTriangle::null())
		{
			tuple = python::makeTuple(iVertex, iNormal);
		}
		else
		{
			tuple = python::makeTuple(iVertex, iNormal, iUv);
		}
	}
	return python::fromSharedPtrToNakedCast(tuple);
}

/** @ingroup Python
 *  @internal
 */
int getIndexVertex(PyObject* iIndices, size_t& oVertex, size_t& oNormal, size_t& oUv)
{
	size_t vertex = prim::IndexTriangle::null();
	size_t normal = prim::IndexTriangle::null();
	size_t uv = prim::IndexTriangle::null();

	TPyObjPtr tuple(PySequence_Fast(iIndices, "expected a sequence (tuple, list, ...)"));
	if (!tuple)
	{
		return 1;
	}
	const Py_ssize_t size = PySequence_Fast_GET_SIZE(tuple.get());
	if (size == -1)
	{
		return 1;
	}
	if (size == 0 || size > 3)
	{
		PyErr_SetString(PyExc_TypeError, "is not (v [, vn [, vt]])");
		return 1;
	}
	PyObject** objects = PySequence_Fast_ITEMS(tuple.get());
	if (pyGetSimpleObject(objects[0], vertex) != 0)
	{
		impl::addMessageHeader("v");
		return 1;
	}
	if (size > 1 && objects[1] != Py_None)
	{
		if (pyGetSimpleObject(objects[1], normal) != 0)
		{
			impl::addMessageHeader("vn");
			return 1;
		}
	}
	if (size > 2 && objects[2] != Py_None)
	{
		if (pyGetSimpleObject(objects[2], uv) != 0)
		{
			impl::addMessageHeader("vt");
			return 1;
		}
	}
	oVertex = vertex;
	oNormal = normal;
	oUv = uv;
	return 0;
}


}
}
}

// EOF
