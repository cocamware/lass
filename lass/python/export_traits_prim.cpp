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

#include "python_common.h"
#include "export_traits_prim.h"
#include "../prim/triangle_mesh_3d.h"

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
int getIndexVertex(PyObject* object, size_t& vertex, size_t& normal, size_t& uv)
{
	FastSequence indices(object, 1, 3);
	if (!indices)
	{
		impl::addMessageHeader("is not (v [, vn [, vt]])");
		return 1;
	}
	if (pyGetSimpleObject(indices[0], vertex) != 0)
	{
		impl::addMessageHeader("v");
		return 1;
	}
	if (indices.size() > 1 && indices[1] != Py_None)
	{
		if (pyGetSimpleObject(indices[1], normal) != 0)
		{
			impl::addMessageHeader("vn");
			return 1;
		}
	}
	else
	{
		normal = prim::IndexTriangle::null();
	}
	if (indices.size() > 2 && indices[2] != Py_None)
	{
		if (pyGetSimpleObject(indices[2], uv) != 0)
		{
			impl::addMessageHeader("vt");
			return 1;
		}
	}
	else
	{
		uv = prim::IndexTriangle::null();
	}
	return 0;
}


}


PyObject* PyExportTraits<prim::IndexTriangle>::build(const prim::IndexTriangle& iTriangle)
{
	TPyObjPtr triangle(PyTuple_New(3));
	if (!triangle) return 0;
	for (int k = 0; k < 3; ++k)
	{
		PyObject* vertex = impl::buildIndexVertex(
			iTriangle.vertices[k], iTriangle.normals[k], iTriangle.uvs[k]);
		if (!vertex) return 0;
		if (PyTuple_SetItem(triangle.get(), k, vertex) != 0) return 0;
	}
	return fromSharedPtrToNakedCast(triangle);
}

int PyExportTraits<prim::IndexTriangle>::get(PyObject* obj, prim::IndexTriangle& triangle)
{
	impl::FastSequence tuple(obj, 3);
	if (!tuple)
	{
		impl::addMessageHeader("IndexTriangle");
		return 1;
	}
	for (int k = 0; k < 3; ++k)
	{
		if (impl::getIndexVertex(tuple[k], triangle.vertices[k], triangle.normals[k], triangle.uvs[k]) != 0)
		{
			std::ostringstream buffer;
			buffer << "IndexTriangle: " << (k + 1) << "th vertex";
			impl::addMessageHeader(buffer.str().c_str());
			return 1;
		}
	}
	return 0;
}

}
}

// EOF
