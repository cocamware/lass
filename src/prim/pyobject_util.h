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

#ifdef LASS_PRIM_PYTHON_EXTENSION_VECTOR_2D
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
    prim::Vector2D<T> result;
	int error = !PyTuple_Check(iValue) || PyTuple_GET_SIZE(iValue) != 2;
	if (!error)
	{
		error = pyGetSimpleObject(PyTuple_GetItem(iValue, 0), result.x) ||
				pyGetSimpleObject(PyTuple_GetItem(iValue, 1), result.y);
	}
	else
	{
		PyErr_BadArgument();
		return error;
	}

	oV = result;
	return error;
}

#   endif
#endif



#ifdef LASS_PRIM_PYTHON_EXTENSION_VECTOR_3D
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
    prim::Vector3D<T> result;
	int error = !PyTuple_Check(iValue) || PyTuple_GET_SIZE(iValue) != 3;
	if (!error)
	{
		error = pyGetSimpleObject(PyTuple_GetItem(iValue, 0), result.x) ||
		        pyGetSimpleObject(PyTuple_GetItem(iValue, 1), result.y) ||
				pyGetSimpleObject(PyTuple_GetItem(iValue, 2), result.z);
	}
	else
	{
		PyErr_BadArgument();
		return error;
	}

	oV = result;
	return error;
}

#   endif
#endif



#ifdef LASS_PRIM_PYTHON_EXTENSION_VECTOR_4D
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
    prim::Vector4D<T> result;
	int error = !PyTuple_Check(iValue) || PyTuple_GET_SIZE(iValue) != 4;
	if (!error)
	{
		error = pyGetSimpleObject(PyTuple_GetItem(iValue, 0), result.x) ||
				pyGetSimpleObject(PyTuple_GetItem(iValue, 1), result.y) ||
				pyGetSimpleObject(PyTuple_GetItem(iValue, 2), result.z) ||
				pyGetSimpleObject(PyTuple_GetItem(iValue, 3), result.w);
	}
	else
	{
		PyErr_BadArgument();
		return error;
	}

	oV = result;
	return error;
}

#   endif
#endif



// --- points --------------------------------------------------------------------------------------

#ifdef LASS_PRIM_PYTHON_EXTENSION_POINT_2D
#   ifndef LASS_GUARDIAN_OF_INCLUSION_PRIM_PYOBJECT_UTIL_H_POINT_2D
#   define LASS_GUARDIAN_OF_INCLUSION_PRIM_PYOBJECT_UTIL_H_POINT_2D

template <typename T> 
PyObject* pyBuildSimpleObject(const prim::Point2D<T>& iV)
{
	return pyBuildSimpleObject(iV.position);
}

template <typename T>
int pyGetSimpleObject(PyObject* iValue, prim::Point2D<T>& oV)
{
    return pyGetSimpleObject(iValue, oV.position);
}

#   endif
#endif



#ifdef LASS_PRIM_PYTHON_EXTENSION_POINT_3D
#   ifndef LASS_GUARDIAN_OF_INCLUSION_PRIM_PYOBJECT_UTIL_H_POINT_3D
#   define LASS_GUARDIAN_OF_INCLUSION_PRIM_PYOBJECT_UTIL_H_POINT_3D

template <typename T> 
PyObject* pyBuildSimpleObject(const prim::Point3D<T>& iV)
{
	return pyBuildSimpleObject(iV.position);
}

template <typename T>
int pyGetSimpleObject(PyObject* iValue, prim::Point3D<T>& oV)
{
    return pyGetSimpleObject(iValue, oV.position);
}

#   endif
#endif



// --- boxes ---------------------------------------------------------------------------------------

#ifdef LASS_PRIM_PYTHON_EXTENSION_AABB_2D
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
	TAabb::TPoint min;
	TAabb::TPoint max;
	int error = !PyTuple_Check(iValue) || PyTuple_GET_SIZE(iValue) != 2;
	if (!error)
	{
		
		error = pyGetSimpleObject(PyTuple_GetItem(iValue, 0), min) ||
				pyGetSimpleObject(PyTuple_GetItem(iValue, 1), max);
	}
	else
	{
		PyErr_BadArgument();
		return error;
	}

	oV = TAabb(min, max);
	return error;
}

#   endif
#endif

#ifdef LASS_PRIM_PYTHON_EXTENSION_AABB_3D
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
	TAabb::TPoint min;
	TAabb::TPoint max;
	int error = !PyTuple_Check(iValue) || PyTuple_GET_SIZE(iValue) != 2;
	if (!error)
	{
		
		error = pyGetSimpleObject(PyTuple_GetItem(iValue, 0), min) ||
				pyGetSimpleObject(PyTuple_GetItem(iValue, 1), max);
	}
	else
	{
		PyErr_BadArgument();
		return error;
	}

	oV = TAabb(min, max);
	return error;
}

#   endif
#endif



// --- line segments -------------------------------------------------------------------------------

#ifdef LASS_PRIM_PYTHON_EXTENSION_LINE_SEGMENT_2D
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
	TLineSegment::TPoint tail;
	TLineSegment::TPoint head;
	int error = !PyTuple_Check(iValue) || PyTuple_GET_SIZE(iValue) != 2;
	if (!error)
	{
		
		error = pyGetSimpleObject(PyTuple_GetItem(iValue, 0), tail) ||
				pyGetSimpleObject(PyTuple_GetItem(iValue, 1), head);
	}
	else
	{
		PyErr_BadArgument();
		return error;
	}

	oV = TLineSegment(tail, head);
	return error;
}

#   endif
#endif

#ifdef LASS_PRIM_PYTHON_EXTENSION_LINE_SEGMENT_3D
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
	TLineSegment::TPoint tail;
	TLineSegment::TPoint head;
	int error = !PyTuple_Check(iValue) || PyTuple_GET_SIZE(iValue) != 3;
	if (!error)
	{
		
		error = pyGetSimpleObject(PyTuple_GetItem(iValue, 0), tail) ||
				pyGetSimpleObject(PyTuple_GetItem(iValue, 1), head);
	}
	else
	{
		PyErr_BadArgument();
		return error;
	}

	oV = TLineSegment(tail, head);
	return error;
}

#   endif
#endif

// --- axis iterators ------------------------------------------------------------------------------

#ifdef LASS_PRIM_PYTHON_EXTENSION_XY
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
	const int error = pyGetSimpleObject(iValue, axis);
	if (!error)
	{
		oV = prim::XY(axis);
	}
	return error;
}

#   endif
#endif

#ifdef LASS_PRIM_PYTHON_EXTENSION_XYZ
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
	const int error = pyGetSimpleObject(iValue, axis);
	if (!error)
	{
		oV = prim::XYZ(axis);
	}
	return error;
}

#   endif
#endif

#ifdef LASS_PRIM_PYTHON_EXTENSION_XYZW
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
	const int error = pyGetSimpleObject(iValue, axis);
	if (!error)
	{
		oV = prim::XYZW(axis);
	}
	return error;
}

#   endif
#endif

}

}

// EOF
