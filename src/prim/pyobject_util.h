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


#ifndef LASS_GUARDIAN_OF_INCLUSION_PRIM_PYOBJECT_UTIL_H
#define LASS_GUARDIAN_OF_INCLUSION_PRIM_PYOBJECT_UTIL_H
#include "../util/pyobject_util.h"
#endif

namespace lass
{
namespace python
{

// --- vectors -------------------------------------------------------------------------------------

#ifdef LASS_PRIM_PYOBJECT_UTIL_VECTOR_2D
#   ifndef LASS_GUARDIAN_OF_INCLUSION_PRIM_PYOBJECT_UTIL_H_VECTOR_2D
#   define LASS_GUARDIAN_OF_INCLUSION_PRIM_PYOBJECT_UTIL_H_VECTOR_2D

template <typename T> 
PyObject* pyBuildSimpleObject(const prim::Vector2D<T>& iV)
{
	PyObject* newTuple = PyTuple_New(2);
	PyTuple_SetItem(newTuple, 0, pyBuildSimpleObject(iV.x));
	PyTuple_SetItem(newTuple, 1, pyBuildSimpleObject(iV.y));
	return newTuple;
}

template <typename T>
int pyGetSimpleObject(PyObject* iValue, prim::Vector2D<T>& oV)
{
	if (!impl::checkTupleSize(iValue, 2))
	{
		impl::addMessageHeader("Vector2D");
		return 1;
	}
    prim::Vector2D<T> result;

	if (pyGetSimpleObject(PyTuple_GetItem(iValue, 0), result.x) != 0)
	{
		impl::addMessageHeader("Vector2D: x");
		return 1;
	}
	if (pyGetSimpleObject(PyTuple_GetItem(iValue, 1), result.y) != 0)
	{
		impl::addMessageHeader("Vector2D: y");
		return 1;
	}

	oV = result;
	return 0;
}

#   endif
#endif



#ifdef LASS_PRIM_PYOBJECT_UTIL_VECTOR_3D
#   ifndef LASS_GUARDIAN_OF_INCLUSION_PRIM_PYOBJECT_UTIL_H_VECTOR_3D
#   define LASS_GUARDIAN_OF_INCLUSION_PRIM_PYOBJECT_UTIL_H_VECTOR_3D

template <typename T> 
PyObject* pyBuildSimpleObject(const prim::Vector3D<T>& iV)
{
	PyObject* newTuple = PyTuple_New(3);
	PyTuple_SetItem(newTuple, 0, pyBuildSimpleObject(iV.x));
	PyTuple_SetItem(newTuple, 1, pyBuildSimpleObject(iV.y));
	PyTuple_SetItem(newTuple, 2, pyBuildSimpleObject(iV.z));
	return newTuple;
}

template <typename T>
int pyGetSimpleObject(PyObject* iValue, prim::Vector3D<T>& oV)
{
	if (!impl::checkTupleSize(iValue, 3))
	{
		impl::addMessageHeader("Vector3D");
		return 1;
	}
    prim::Vector3D<T> result;

	if (pyGetSimpleObject(PyTuple_GetItem(iValue, 0), result.x) != 0)
	{
		impl::addMessageHeader("Vector3D: x");
		return 1;
	}
	if (pyGetSimpleObject(PyTuple_GetItem(iValue, 1), result.y) != 0)
	{
		impl::addMessageHeader("Vector3D: y");
		return 1;
	}
	if (pyGetSimpleObject(PyTuple_GetItem(iValue, 2), result.z) != 0)
	{
		impl::addMessageHeader("Vector3D: z");
		return 1;
	}

	oV = result;
	return 0;
}

#   endif
#endif



#ifdef LASS_PRIM_PYOBJECT_UTIL_VECTOR_4D
#   ifndef LASS_GUARDIAN_OF_INCLUSION_PRIM_PYOBJECT_UTIL_H_VECTOR_4D
#   define LASS_GUARDIAN_OF_INCLUSION_PRIM_PYOBJECT_UTIL_H_VECTOR_4D

template <typename T> 
PyObject* pyBuildSimpleObject(const prim::Vector4D<T>& iV)
{
	PyObject* newTuple = PyTuple_New(3);
	PyTuple_SetItem(newTuple, 0, pyBuildSimpleObject(iV.x));
	PyTuple_SetItem(newTuple, 1, pyBuildSimpleObject(iV.y));
	PyTuple_SetItem(newTuple, 2, pyBuildSimpleObject(iV.z));
	PyTuple_SetItem(newTuple, 3, pyBuildSimpleObject(iV.w));
	return newTuple;
}

template <typename T>
int pyGetSimpleObject(PyObject* iValue, prim::Vector4D<T>& oV)
{
	if (!impl::checkTupleSize(iValue, 4))
	{
		impl::addMessageHeader("Vector4D");
		return 1;
	}
    prim::Vector4D<T> result;

	if (pyGetSimpleObject(PyTuple_GetItem(iValue, 0), result.x) != 0)
	{
		impl::addMessageHeader("Vector4D: x");
		return 1;
	}
	if (pyGetSimpleObject(PyTuple_GetItem(iValue, 1), result.y) != 0)
	{
		impl::addMessageHeader("Vector4D: y");
		return 1;
	}
	if (pyGetSimpleObject(PyTuple_GetItem(iValue, 2), result.z) != 0)
	{
		impl::addMessageHeader("Vector4D: z");
		return 1;
	}
	if (pyGetSimpleObject(PyTuple_GetItem(iValue, 3), result.z) != 0)
	{
		impl::addMessageHeader("Vector4D: w");
		return 1;
	}

	oV = result;
	return 0;
}

#   endif
#endif



// --- points --------------------------------------------------------------------------------------

#ifdef LASS_PRIM_PYOBJECT_UTIL_POINT_2D
#   ifndef LASS_GUARDIAN_OF_INCLUSION_PRIM_PYOBJECT_UTIL_H_POINT_2D
#   define LASS_GUARDIAN_OF_INCLUSION_PRIM_PYOBJECT_UTIL_H_POINT_2D

template <typename T> 
PyObject* pyBuildSimpleObject(const prim::Point2D<T>& iV)
{
	return pyBuildSimpleObject(iV.position());
}

template <typename T>
int pyGetSimpleObject(PyObject* iValue, prim::Point2D<T>& oV)
{
	if (!impl::checkTupleSize(iValue, 2))
	{
		impl::addMessageHeader("Point3D");
		return 1;
	}
    prim::Point2D<T> result;

	if (pyGetSimpleObject(PyTuple_GetItem(iValue, 0), result.x) != 0)
	{
		impl::addMessageHeader("Point3D: x");
		return 1;
	}
	if (pyGetSimpleObject(PyTuple_GetItem(iValue, 1), result.y) != 0)
	{
		impl::addMessageHeader("Point3D: y");
		return 1;
	}

	oV = result;
	return 0;
}

#   endif
#endif



#ifdef LASS_PRIM_PYOBJECT_UTIL_POINT_3D
#   ifndef LASS_GUARDIAN_OF_INCLUSION_PRIM_PYOBJECT_UTIL_H_POINT_3D
#   define LASS_GUARDIAN_OF_INCLUSION_PRIM_PYOBJECT_UTIL_H_POINT_3D

template <typename T> 
PyObject* pyBuildSimpleObject(const prim::Point3D<T>& iV)
{
	return pyBuildSimpleObject(iV.position());
}

template <typename T>
int pyGetSimpleObject(PyObject* iValue, prim::Point3D<T>& oV)
{
	if (!impl::checkTupleSize(iValue, 3))
	{
		impl::addMessageHeader("Point3D");
		return 1;
	}
    prim::Point3D<T> result;

	if (pyGetSimpleObject(PyTuple_GetItem(iValue, 0), result.x) != 0)
	{
		impl::addMessageHeader("Point3D: x");
		return 1;
	}
	if (pyGetSimpleObject(PyTuple_GetItem(iValue, 1), result.y) != 0)
	{
		impl::addMessageHeader("Point3D: y");
		return 1;
	}
	if (pyGetSimpleObject(PyTuple_GetItem(iValue, 2), result.z) != 0)
	{
		impl::addMessageHeader("Point3D: z");
		return 1;
	}

	oV = result;
	return 0;
}

#   endif
#endif



// --- boxes ---------------------------------------------------------------------------------------

#ifdef LASS_PRIM_PYOBJECT_UTIL_AABB_2D
#   ifndef LASS_GUARDIAN_OF_INCLUSION_PRIM_PYOBJECT_UTIL_H_AABB_2D
#   define LASS_GUARDIAN_OF_INCLUSION_PRIM_PYOBJECT_UTIL_H_AABB_2D

template <typename T, class MMP> 
PyObject* pyBuildSimpleObject(const prim::Aabb2D<T, MMP>& iV)
{
	PyObject* newTuple = PyTuple_New(2);
	PyTuple_SetItem(newTuple, 0, pyBuildSimpleObject(iV.min()));
	PyTuple_SetItem(newTuple, 1, pyBuildSimpleObject(iV.max()));
	return newTuple;
}

template <typename T, class MMP>
int pyGetSimpleObject(PyObject* iValue, prim::Aabb2D<T, MMP>& oV)
{
	typedef prim::Aabb2D<T, MMP> TAabb;
	if (!impl::checkTupleSize(iValue, 2))
	{
		impl::addMessageHeader("Aabb2D");
		return 1;
	}

	TAabb::TPoint min;
    if (pyGetSimpleObject(PyTuple_GetItem(iValue, 0), min) != 0)
	{
		impl::addMessageHeader("Aabb2D: min");
		return 1;
	}

	TAabb::TPoint max;
    if (pyGetSimpleObject(PyTuple_GetItem(iValue, 1), max) != 0)
	{
		impl::addMessageHeader("Aabb2D: max");
		return 1;
	}

	try
	{
		oV = TAabb(min, max);
	}
	catch (util::Exception& error)
	{
		std::ostringstream buffer;
		buffer << "Aabb2D: " << error.message();
		PyErr_SetString(PyExc_TypeError, buffer.str().c_str());
		return 1;
	}
	return 0;
}

#   endif
#endif

#ifdef LASS_PRIM_PYOBJECT_UTIL_AABB_3D
#   ifndef LASS_GUARDIAN_OF_INCLUSION_PRIM_PYOBJECT_UTIL_H_AABB_3D
#   define LASS_GUARDIAN_OF_INCLUSION_PRIM_PYOBJECT_UTIL_H_AABB_3D

template <typename T, class MMP> 
PyObject* pyBuildSimpleObject(const prim::Aabb3D<T, MMP>& iV)
{
	PyObject* newTuple = PyTuple_New(2);
	PyTuple_SetItem(newTuple, 0, pyBuildSimpleObject(iV.min()));
	PyTuple_SetItem(newTuple, 1, pyBuildSimpleObject(iV.max()));
	return newTuple;
}

template <typename T, class MMP>
int pyGetSimpleObject(PyObject* iValue, prim::Aabb3D<T, MMP>& oV)
{
	typedef prim::Aabb3D<T, MMP> TAabb;
	if (!impl::checkTupleSize(iValue, 2))
	{
		impl::addMessageHeader("Aabb3D");
		return 1;
	}

	typename TAabb::TPoint min;
    if (pyGetSimpleObject(PyTuple_GetItem(iValue, 0), min) != 0)
	{
		impl::addMessageHeader("Aabb3D: min");
		return 1;
	}

	typename TAabb::TPoint max;
    if (pyGetSimpleObject(PyTuple_GetItem(iValue, 1), max) != 0)
	{
		impl::addMessageHeader("Aabb3D: max");
		return 1;
	}

	try
	{
		oV = TAabb(min, max);
	}
	catch (util::Exception& error)
	{
		std::ostringstream buffer;
		buffer << "Aabb3D: " << error.message();
		PyErr_SetString(PyExc_TypeError, buffer.str().c_str());
		return 1;
	}
	return 0;
}

#   endif
#endif



// --- line segments -------------------------------------------------------------------------------

#ifdef LASS_PRIM_PYOBJECT_UTIL_LINE_SEGMENT_2D
#   ifndef LASS_GUARDIAN_OF_INCLUSION_PRIM_PYOBJECT_UTIL_H_LINE_SEGMENT_2D
#   define LASS_GUARDIAN_OF_INCLUSION_PRIM_PYOBJECT_UTIL_H_LINE_SEGMENT_2D

template <typename T, class PP> 
PyObject* pyBuildSimpleObject(const prim::LineSegment2D<T, PP>& iV)
{
	PyObject* newTuple = PyTuple_New(2);
	PyTuple_SetItem(newTuple, 0, pyBuildSimpleObject(iV.tail()));
	PyTuple_SetItem(newTuple, 1, pyBuildSimpleObject(iV.head()));
	return newTuple;
}

template <typename T, class PP>
int pyGetSimpleObject(PyObject* iValue, prim::LineSegment2D<T, PP>& oV)
{
	typedef prim::LineSegment2D<T, PP> TLineSegment;
	if (!impl::checkTupleSize(iValue, 2))
	{
		impl::addMessageHeader("LineSegment2D");
		return 1;
	}

	TLineSegment::TPoint tail;
    if (pyGetSimpleObject(PyTuple_GetItem(iValue, 0), tail) != 0)
	{
		impl::addMessageHeader("LineSegment2D: tail");
		return 1;
	}

	TLineSegment::TPoint head;
    if (pyGetSimpleObject(PyTuple_GetItem(iValue, 1), head) != 0)
	{
		impl::addMessageHeader("LineSegment2D: head");
		return 1;
	}

	oV = TLineSegment(tail, head);
	return 0;
}

#   endif
#endif

#ifdef LASS_PRIM_PYOBJECT_UTIL_LINE_SEGMENT_3D
#   ifndef LASS_GUARDIAN_OF_INCLUSION_PRIM_PYOBJECT_UTIL_H_LINE_SEGMENT_3D
#   define LASS_GUARDIAN_OF_INCLUSION_PRIM_PYOBJECT_UTIL_H_LINE_SEGMENT_3D

template <typename T, class PP> 
PyObject* pyBuildSimpleObject(const prim::LineSegment3D<T, PP>& iV)
{
	PyObject* newTuple = PyTuple_New(2);
	PyTuple_SetItem(newTuple, 0, pyBuildSimpleObject(iV.tail()));
	PyTuple_SetItem(newTuple, 1, pyBuildSimpleObject(iV.head()));
	return newTuple;
}

template <typename T, class PP>
int pyGetSimpleObject(PyObject* iValue, prim::LineSegment3D<T, PP>& oV)
{
	typedef prim::LineSegment3D<T, PP> TLineSegment;
	if (!impl::checkTupleSize(iValue, 2))
	{
		impl::addMessageHeader("LineSegment3D");
		return 1;
	}

	TLineSegment::TPoint tail;
    if (pyGetSimpleObject(PyTuple_GetItem(iValue, 0), tail) != 0)
	{
		impl::addMessageHeader("LineSegment3D: tail");
		return 1;
	}

	TLineSegment::TPoint head;
    if (pyGetSimpleObject(PyTuple_GetItem(iValue, 1), head) != 0)
	{
		impl::addMessageHeader("LineSegment3D: head");
		return 1;
	}

	oV = TLineSegment(tail, head);
	return 0;
}

#   endif
#endif

// --- axis iterators ------------------------------------------------------------------------------

#ifdef LASS_PRIM_PYOBJECT_UTIL_XY
#   ifndef LASS_GUARDIAN_OF_INCLUSION_PRIM_PYOBJECT_UTIL_H_XY
#   define LASS_GUARDIAN_OF_INCLUSION_PRIM_PYOBJECT_UTIL_H_XY

inline
PyObject* pyBuildSimpleObject(prim::XY iV)
{
	return pyBuildSimpleObject(std::string(1, iV.axis()));
}

inline
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

#   endif
#endif

#ifdef LASS_PRIM_PYOBJECT_UTIL_XYZ
#   ifndef LASS_GUARDIAN_OF_INCLUSION_PRIM_PYOBJECT_UTIL_H_XYZ
#   define LASS_GUARDIAN_OF_INCLUSION_PRIM_PYOBJECT_UTIL_H_XYZ

inline
PyObject* pyBuildSimpleObject(prim::XYZ iV)
{
	return pyBuildSimpleObject(std::string(1, iV.axis()));
}

inline
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

#   endif
#endif

#ifdef LASS_PRIM_PYOBJECT_UTIL_XYZW
#   ifndef LASS_GUARDIAN_OF_INCLUSION_PRIM_PYOBJECT_UTIL_H_XYZW
#   define LASS_GUARDIAN_OF_INCLUSION_PRIM_PYOBJECT_UTIL_H_XYZW

inline
PyObject* pyBuildSimpleObject(prim::XYZW iV)
{
	return pyBuildSimpleObject(std::string(1, iV.axis()));
}

inline
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

#   endif
#endif



// --- colors --------------------------------------------------------------------------------------

#ifdef LASS_PRIM_PYOBJECT_UTIL_COLOR_RGBA
#   ifndef LASS_GUARDIAN_OF_INCLUSION_PRIM_PYOBJECT_UTIL_H_COLOR_RGBA
#   define LASS_GUARDIAN_OF_INCLUSION_PRIM_PYOBJECT_UTIL_H_COLOR_RGBA

inline
PyObject* pyBuildSimpleObject(const prim::ColorRGBA& iV)
{
	return pyBuildSimpleObject(iV.vector());
}

inline
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

	if (pyGetSimpleObject(PyTuple_GetItem(iValue, 0), result.r()) != 0)
	{
		impl::addMessageHeader("ColorRGBA: r");
		return 1;
	}
	if (pyGetSimpleObject(PyTuple_GetItem(iValue, 1), result.g()) != 0)
	{
		impl::addMessageHeader("ColorRGBA: g");
		return 1;
	}
	if (pyGetSimpleObject(PyTuple_GetItem(iValue, 2), result.b()) != 0)
	{
		impl::addMessageHeader("ColorRGBA: b");
		return 1;
	}
	if (size == 4)
	{
		if (pyGetSimpleObject(PyTuple_GetItem(iValue, 3), result.a()) != 0)
		{
			impl::addMessageHeader("ColorRGBA: a");
			return 1;
		}
	}

	oV = result;
	return 0;
}

#	endif
#endif

}

}

// EOF
