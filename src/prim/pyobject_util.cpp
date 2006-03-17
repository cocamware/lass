/** @file
 *  @author Bram de Greve (bramz@users.sourceforge.net)
 *  @author Tom De Muer (tomdemuer@users.sourceforge.net)
 *
 *  Distributed under the terms of the GPL (GNU Public License)
 *
 *  The LASS License:
 *
 *  Copyright 2004-2006 Bram de Greve and Tom De Muer
 *
 *  LASS is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */

#include "prim_common.h"
#include "pyobject_util.h"
#include "xy.h"
#include "xyz.h"
#include "xyzw.h"
#include "triangle_mesh_3d.h"
#include "color_rgba.h"

namespace lass
{
namespace python
{

// --- axis iterators ------------------------------------------------------------------------------

PyObject* pyBuildSimpleObject(const prim::XY& iV)
{
	return pyBuildSimpleObject(std::string(1, iV.axis()));
}

int pyGetSimpleObject(PyObject* iValue, prim::XY& oV)
{
	std::string axis;
	if (pyGetSimpleObject(iValue, axis) != 0)
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

PyObject* pyBuildSimpleObject(const prim::XYZ& iV)
{
	return pyBuildSimpleObject(std::string(1, iV.axis()));
}

int pyGetSimpleObject(PyObject* iValue, prim::XYZ& oV)
{
	std::string axis;
	if (pyGetSimpleObject(iValue, axis) != 0)
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

PyObject* pyBuildSimpleObject(const prim::XYZW& iV)
{
	return pyBuildSimpleObject(std::string(1, iV.axis()));
}

int pyGetSimpleObject(PyObject* iValue, prim::XYZW& oV)
{
	std::string axis;
	if (pyGetSimpleObject(iValue, axis) != 0)
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
	int getIndexVertex(PyObject* iIndices, size_t& oVertex, size_t& oNormal, size_t& oUv)
	{
		typedef PyObjectPtr<PyObject>::Type TPyPtr;
		if (!PySequence_Check(iIndices))
		{
			PyErr_SetString(PyExc_TypeError, "is not a sequence");
			return 1;
		}
		const int size = PySequence_Size(iIndices);
		if (size == -1)
		{
			return 1;
		}
		if (size == 0 || size > 3)
		{
			PyErr_SetString(PyExc_TypeError, "is not (v [, vn [, vt]])");
			return 1;
		}
		const TPyPtr vertex(PySequence_ITEM(iIndices, 0));
		if (!vertex)
		{
			return 1;
		}
		if (pyGetSimpleObject(vertex.get(), oVertex) != 0)
		{
			impl::addMessageHeader("v");
			return 1;
		}
		if (size > 1)
		{
			const TPyPtr normal(PySequence_ITEM(iIndices, 1));
			if (!normal)
			{
				return 1;
			}
			if (normal.get() != Py_None)
			{
				if (pyGetSimpleObject(normal.get(), oNormal) != 0)
				{
					impl::addMessageHeader("vn");
					return 1;
				}
			}
			else
			{
				oNormal = prim::IndexTriangle::null();
			}
		}
		if (size > 2)
		{
			const TPyPtr uv(PySequence_ITEM(iIndices, 2));
			if (!uv)
			{
				return 1;
			}
			if (uv.get() != Py_None)
			{
				if (pyGetSimpleObject(uv.get(), oUv) != 0)
				{
					impl::addMessageHeader("vt");
					return 1;
				}
			}
			else
			{
				oUv = prim::IndexTriangle::null();
			}
		}
		return 0;
	}
}

int pyGetSimpleObject(PyObject* iValue, prim::IndexTriangle& oTriangle)
{
	typedef PyObjectPtr<PyObject>::Type TPyPtr;
	prim::IndexTriangle result = { 0, 0, 0, 0, 0, 0, 0, 0, 0 };
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

PyObject* pyBuildSimpleObject(const prim::ColorRGBA& iV)
{
	return pyBuildSimpleObject(iV.vector());
}

int pyGetSimpleObject(PyObject* iValue, prim::ColorRGBA& oV)
{
	if (!PyTuple_Check(iValue))
	{
		PyErr_SetString(PyExc_TypeError, "ColorRGBA: is not a tuple");
		return 1;
	}
	const int size = PyTuple_Size(iValue);
	if (size != 3 && size != 4)
	{
		std::ostringstream buffer;
		buffer << "ColorRGBA: tuple is not of expected size 3 or 4 (size is "
			<< size << ")";
		PyErr_SetString(PyExc_TypeError, buffer.str().c_str());
		return false;
	}
	prim::ColorRGBA result;

	if (pyGetSimpleObject(PySequence_Fast_GET_ITEM(iValue, 0), result.r) != 0)
	{
		impl::addMessageHeader("ColorRGBA: r");
		return 1;
	}
	if (pyGetSimpleObject(PySequence_Fast_GET_ITEM(iValue, 1), result.g) != 0)
	{
		impl::addMessageHeader("ColorRGBA: g");
		return 1;
	}
	if (pyGetSimpleObject(PySequence_Fast_GET_ITEM(iValue, 2), result.b) != 0)
	{
		impl::addMessageHeader("ColorRGBA: b");
		return 1;
	}
	if (size == 4)
	{
		if (pyGetSimpleObject(PySequence_Fast_GET_ITEM(iValue, 3), result.a) != 0)
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