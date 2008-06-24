/**	@file
 *	@author Bram de Greve (bramz@users.sourceforge.net)
 *	@author Tom De Muer (tomdemuer@users.sourceforge.net)
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
 *	Copyright (C) 2004-2007 the Initial Developer.
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

#include "prim_common.h"
#include "pyobject_util.h"
#include "xy.h"
#include "xyz.h"
#include "xyzw.h"
#include "triangle_mesh_3d.h"
#include "color_rgba.h"
#include "../util/py_tuple.h"
#include "../util/py_stl.h"

namespace lass
{
namespace python
{

// --- axis iterators ------------------------------------------------------------------------------

PyObject* pyBuildSimpleObject_deprecated(const prim::XY& iV)
{
	return pyExportTraitBuild(std::string(1, iV.axis()));
}

int pyGetSimpleObject_deprecated(PyObject* iValue, prim::XY& oV)
{
	std::string axis;
	if (pyExportTraitGet(iValue, axis) != 0)
	{
		impl::addMessageHeader("XY");
	}

	try
	{
		oV = prim::XY(axis);
	}
	catch (util::Exception& error)
	{
		std::ostringstream buffer;
		buffer << "XY: " << error.message();
		PyErr_SetString(PyExc_TypeError, buffer.str().c_str());
		return 1;
	}
	return 0;
}

PyObject* pyBuildSimpleObject_deprecated(const prim::XYZ& iV)
{
	return pyExportTraitBuild(std::string(1, iV.axis()));
}

int pyGetSimpleObject_deprecated(PyObject* iValue, prim::XYZ& oV)
{
	std::string axis;
	if (pyExportTraitGet(iValue, axis) != 0)
	{
		impl::addMessageHeader("XYZ");
	}

	try
	{
		oV = prim::XYZ(axis);
	}
	catch (util::Exception& error)
	{
		std::ostringstream buffer;
		buffer << "XYZ: " << error.message();
		PyErr_SetString(PyExc_TypeError, buffer.str().c_str());
		return 1;
	}
	return 0;
}

PyObject* pyBuildSimpleObject_deprecated(const prim::XYZW& iV)
{
	return pyExportTraitBuild(std::string(1, iV.axis()));
}

int pyGetSimpleObject_deprecated(PyObject* iValue, prim::XYZW& oV)
{
	std::string axis;
	if (pyExportTraitGet(iValue, axis) != 0)
	{
		impl::addMessageHeader("XYZW");
	}

	try
	{
		oV = prim::XYZW(axis);
	}
	catch (util::Exception& error)
	{
		std::ostringstream buffer;
		buffer << "XYZW: " << error.message();
		PyErr_SetString(PyExc_TypeError, buffer.str().c_str());
		return 1;
	}
	return 0;
}



// --- triangle meshes -----------------------------------------------------------------------------

namespace impl
{
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

	int getIndexVertex(PyObject* iIndices, size_t& oVertex, size_t& oNormal, size_t& oUv)
	{
		size_t vertex = prim::IndexTriangle::null();
		size_t normal = prim::IndexTriangle::null();
		size_t uv = prim::IndexTriangle::null();

		typedef PyObjectPtr<PyObject>::Type TPyPtr;
		if (!PySequence_Check(iIndices))
		{
			PyErr_SetString(PyExc_TypeError, "is not a sequence");
			return 1;
		}
		const Py_ssize_t size = PySequence_Size(iIndices);
		if (size == -1)
		{
			return 1;
		}
		if (size == 0 || size > 3)
		{
			PyErr_SetString(PyExc_TypeError, "is not (v [, vn [, vt]])");
			return 1;
		}
		const TPyPtr vertexItem(PySequence_ITEM(iIndices, 0));
		LASS_ASSERT(vertexItem);
		if (pyExportTraitGet(vertexItem.get(), vertex) != 0)
		{
			impl::addMessageHeader("v");
			return 1;
		}
		if (size > 1)
		{
			const TPyPtr normalItem(PySequence_ITEM(iIndices, 1));
			LASS_ASSERT(normalItem);
			if (normalItem.get() != Py_None)
			{
				if (pyExportTraitGet(normalItem.get(), normal) != 0)
				{
					impl::addMessageHeader("vn");
					return 1;
				}
			}
		}
		if (size > 2)
		{
			const TPyPtr uvItem(PySequence_ITEM(iIndices, 2));
			LASS_ASSERT(uvItem);
			if (uvItem.get() != Py_None)
			{
				if (pyExportTraitGet(uvItem.get(), uv) != 0)
				{
					impl::addMessageHeader("vt");
					return 1;
				}
			}
		}
		oVertex = vertex;
		oNormal = normal;
		oUv = uv;
		return 0;
	}
}

PyObject* pyBuildSimpleObject_deprecated(const prim::IndexTriangle& iTriangle)
{
	typedef PyObjectPtr<PyObject>::Type TPyPtr;
	TPyPtr triangle(PyTuple_New(3));
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



int pyGetSimpleObject_deprecated(PyObject* iValue, prim::IndexTriangle& oTriangle)
{
	typedef PyObjectPtr<PyObject>::Type TPyPtr;
	prim::IndexTriangle result = {{0, 0, 0}, {0, 0, 0}, {0, 0, 0}};
	if (!impl::checkSequenceSize(iValue, 3))
	{
		impl::addMessageHeader("lass::prim::IndexTriangle");
		return 1;
	}
	for (int k = 0; k < 3; ++k)
	{
		const TPyPtr indices(PySequence_ITEM(iValue, k));
		if (!indices)
		{
			impl::addMessageHeader("lass::prim::IndexTriangle");
			return 1;
		}
		if (impl::getIndexVertex(indices.get(), result.vertices[k], result.normals[k], result.uvs[k]) != 0)
		{
			impl::addMessageHeader("lass::prim::IndexTriangle: vertex " + 
				util::stringCast<std::string>(k));
			return 1;
		}
	}
	oTriangle = result;
	return 0;
}



// --- colors --------------------------------------------------------------------------------------

PyObject* pyBuildSimpleObject_deprecated(const prim::ColorRGBA& iV)
{
	return pyExportTraitBuild(iV.vector());
}

int pyGetSimpleObject_deprecated(PyObject* iValue, prim::ColorRGBA& oV)
{
	if (!PyTuple_Check(iValue))
	{
		PyErr_SetString(PyExc_TypeError, "ColorRGBA: is not a tuple");
		return 1;
	}
	const Py_ssize_t size = PyTuple_Size(iValue);
	if (size != 3 && size != 4)
	{
		std::ostringstream buffer;
		buffer << "ColorRGBA: tuple is not of expected size 3 or 4 (size is "
			<< size << ")";
		PyErr_SetString(PyExc_TypeError, buffer.str().c_str());
		return false;
	}
	prim::ColorRGBA result;

	if (pyExportTraitGet(PySequence_Fast_GET_ITEM(iValue, 0), result.r) != 0)
	{
		impl::addMessageHeader("ColorRGBA: r");
		return 1;
	}
	if (pyExportTraitGet(PySequence_Fast_GET_ITEM(iValue, 1), result.g) != 0)
	{
		impl::addMessageHeader("ColorRGBA: g");
		return 1;
	}
	if (pyExportTraitGet(PySequence_Fast_GET_ITEM(iValue, 2), result.b) != 0)
	{
		impl::addMessageHeader("ColorRGBA: b");
		return 1;
	}
	if (size == 4)
	{
		if (pyExportTraitGet(PySequence_Fast_GET_ITEM(iValue, 3), result.a) != 0)
		{
			impl::addMessageHeader("ColorRGBA: a");
			return 1;
		}
	}

	oV = result;
	return 0;
}

}

}

// EOF
