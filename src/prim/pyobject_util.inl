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

#ifdef LASS_GUARDIAN_OF_INCLUSION_PRIM_PYOBJECT_UTIL_H

namespace lass
{
namespace python
{

// --- vectors -------------------------------------------------------------------------------------

#ifdef LASS_GUARDIAN_OF_INCLUSION_PRIM_VECTOR_2D_H
#	ifndef LASS_GUARDIAN_OF_INCLUSION_PRIM_PYOBJECT_UTIL_INL_VECTOR_2D
#	define LASS_GUARDIAN_OF_INCLUSION_PRIM_PYOBJECT_UTIL_INL_VECTOR_2D

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
	if (!impl::checkSequenceSize(iValue, 2))
	{
		impl::addMessageHeader("Vector2D");
		return 1;
	}
	prim::Vector2D<T> result;

	if (pyGetSimpleObject(PySequence_Fast_GET_ITEM(iValue, 0), result.x) != 0)
	{
		impl::addMessageHeader("Vector2D: x");
		return 1;
	}
	if (pyGetSimpleObject(PySequence_Fast_GET_ITEM(iValue, 1), result.y) != 0)
	{
		impl::addMessageHeader("Vector2D: y");
		return 1;
	}

	oV = result;
	return 0;
}

#	endif
#endif



#ifdef LASS_GUARDIAN_OF_INCLUSION_PRIM_VECTOR_3D_H
#	ifndef LASS_GUARDIAN_OF_INCLUSION_PRIM_PYOBJECT_UTIL_INL_VECTOR_3D
#	define LASS_GUARDIAN_OF_INCLUSION_PRIM_PYOBJECT_UTIL_INL_VECTOR_3D

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
	if (!impl::checkSequenceSize(iValue, 3))
	{
		impl::addMessageHeader("Vector3D");
		return 1;
	}
	prim::Vector3D<T> result;

	if (pyGetSimpleObject(PySequence_Fast_GET_ITEM(iValue, 0), result.x) != 0)
	{
		impl::addMessageHeader("Vector3D: x");
		return 1;
	}
	if (pyGetSimpleObject(PySequence_Fast_GET_ITEM(iValue, 1), result.y) != 0)
	{
		impl::addMessageHeader("Vector3D: y");
		return 1;
	}
	if (pyGetSimpleObject(PySequence_Fast_GET_ITEM(iValue, 2), result.z) != 0)
	{
		impl::addMessageHeader("Vector3D: z");
		return 1;
	}

	oV = result;
	return 0;
}

#	endif
#endif



#ifdef LASS_GUARDIAN_OF_INCLUSION_PRIM_VECTOR_4D_H
#	ifndef LASS_GUARDIAN_OF_INCLUSION_PRIM_PYOBJECT_UTIL_INL_VECTOR_4D
#	define LASS_GUARDIAN_OF_INCLUSION_PRIM_PYOBJECT_UTIL_INL_VECTOR_4D

template <typename T>
PyObject* pyBuildSimpleObject(const prim::Vector4D<T>& iV)
{
	PyObject* newTuple = PyTuple_New(4);
	PyTuple_SetItem(newTuple, 0, pyBuildSimpleObject(iV.x));
	PyTuple_SetItem(newTuple, 1, pyBuildSimpleObject(iV.y));
	PyTuple_SetItem(newTuple, 2, pyBuildSimpleObject(iV.z));
	PyTuple_SetItem(newTuple, 3, pyBuildSimpleObject(iV.w));
	return newTuple;
}

template <typename T>
int pyGetSimpleObject(PyObject* iValue, prim::Vector4D<T>& oV)
{
	if (!impl::checkSequenceSize(iValue, 4))
	{
		impl::addMessageHeader("Vector4D");
		return 1;
	}
	prim::Vector4D<T> result;

	if (pyGetSimpleObject(PySequence_Fast_GET_ITEM(iValue, 0), result.x) != 0)
	{
		impl::addMessageHeader("Vector4D: x");
		return 1;
	}
	if (pyGetSimpleObject(PySequence_Fast_GET_ITEM(iValue, 1), result.y) != 0)
	{
		impl::addMessageHeader("Vector4D: y");
		return 1;
	}
	if (pyGetSimpleObject(PySequence_Fast_GET_ITEM(iValue, 2), result.z) != 0)
	{
		impl::addMessageHeader("Vector4D: z");
		return 1;
	}
	if (pyGetSimpleObject(PySequence_Fast_GET_ITEM(iValue, 3), result.z) != 0)
	{
		impl::addMessageHeader("Vector4D: w");
		return 1;
	}

	oV = result;
	return 0;
}

#	endif
#endif



// --- points --------------------------------------------------------------------------------------

#ifdef LASS_GUARDIAN_OF_INCLUSION_PRIM_POINT_2D_H
#	ifndef LASS_GUARDIAN_OF_INCLUSION_PRIM_PYOBJECT_UTIL_INL_POINT_2D
#	define LASS_GUARDIAN_OF_INCLUSION_PRIM_PYOBJECT_UTIL_INL_POINT_2D

template <typename T>
PyObject* pyBuildSimpleObject(const prim::Point2D<T>& iV)
{
	return pyBuildSimpleObject(iV.position());
}

template <typename T>
int pyGetSimpleObject(PyObject* iValue, prim::Point2D<T>& oV)
{
	if (!impl::checkSequenceSize(iValue, 2))
	{
		impl::addMessageHeader("Point3D");
		return 1;
	}
	prim::Point2D<T> result;

	if (pyGetSimpleObject(PySequence_Fast_GET_ITEM(iValue, 0), result.x) != 0)
	{
		impl::addMessageHeader("Point3D: x");
		return 1;
	}
	if (pyGetSimpleObject(PySequence_Fast_GET_ITEM(iValue, 1), result.y) != 0)
	{
		impl::addMessageHeader("Point3D: y");
		return 1;
	}

	oV = result;
	return 0;
}

#	endif
#endif



#ifdef LASS_GUARDIAN_OF_INCLUSION_PRIM_POINT_3D_H
#	ifndef LASS_GUARDIAN_OF_INCLUSION_PRIM_PYOBJECT_UTIL_INL_POINT_3D
#	define LASS_GUARDIAN_OF_INCLUSION_PRIM_PYOBJECT_UTIL_INL_POINT_3D

template <typename T>
PyObject* pyBuildSimpleObject(const prim::Point3D<T>& iV)
{
	return pyBuildSimpleObject(iV.position());
}

template <typename T>
int pyGetSimpleObject(PyObject* iValue, prim::Point3D<T>& oV)
{
	if (!impl::checkSequenceSize(iValue, 3))
	{
		impl::addMessageHeader("Point3D");
		return 1;
	}
	prim::Point3D<T> result;

	if (pyGetSimpleObject(PySequence_Fast_GET_ITEM(iValue, 0), result.x) != 0)
	{
		impl::addMessageHeader("Point3D: x");
		return 1;
	}
	if (pyGetSimpleObject(PySequence_Fast_GET_ITEM(iValue, 1), result.y) != 0)
	{
		impl::addMessageHeader("Point3D: y");
		return 1;
	}
	if (pyGetSimpleObject(PySequence_Fast_GET_ITEM(iValue, 2), result.z) != 0)
	{
		impl::addMessageHeader("Point3D: z");
		return 1;
	}
	oV = result;
	return 0;
}

#	endif
#endif



// --- boxes ---------------------------------------------------------------------------------------

#ifdef LASS_GUARDIAN_OF_INCLUSION_PRIM_AABB_2D_H
#	ifndef LASS_GUARDIAN_OF_INCLUSION_PRIM_PYOBJECT_UTIL_INL_AABB_2D
#	define LASS_GUARDIAN_OF_INCLUSION_PRIM_PYOBJECT_UTIL_INL_AABB_2D

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
	if (!impl::checkSequenceSize(iValue, 2))
	{
		impl::addMessageHeader("Aabb2D");
		return 1;
	}

	typename TAabb::TPoint min;
	if (pyGetSimpleObject(PySequence_Fast_GET_ITEM(iValue, 0), min) != 0)
	{
		impl::addMessageHeader("Aabb2D: min");
		return 1;
	}

	typename TAabb::TPoint max;
	if (pyGetSimpleObject(PySequence_Fast_GET_ITEM(iValue, 1), max) != 0)
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

#	endif
#endif

#ifdef LASS_GUARDIAN_OF_INCLUSION_PRIM_AABB_3D_H
#	ifndef LASS_GUARDIAN_OF_INCLUSION_PRIM_PYOBJECT_UTIL_INL_AABB_3D
#	define LASS_GUARDIAN_OF_INCLUSION_PRIM_PYOBJECT_UTIL_INL_AABB_3D

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
	if (!impl::checkSequenceSize(iValue, 2))
	{
		impl::addMessageHeader("Aabb3D");
		return 1;
	}

	typename TAabb::TPoint min;
	if (pyGetSimpleObject(PySequence_Fast_GET_ITEM(iValue, 0), min) != 0)
	{
		impl::addMessageHeader("Aabb3D: min");
		return 1;
	}

	typename TAabb::TPoint max;
	if (pyGetSimpleObject(PySequence_Fast_GET_ITEM(iValue, 1), max) != 0)
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

#	endif
#endif



// --- line segments -------------------------------------------------------------------------------

#ifdef LASS_GUARDIAN_OF_INCLUSION_PRIM_LINE_SEGMENT_2D_H
#	ifndef LASS_GUARDIAN_OF_INCLUSION_PRIM_PYOBJECT_UTIL_INL_LINE_SEGMENT_2D
#	define LASS_GUARDIAN_OF_INCLUSION_PRIM_PYOBJECT_UTIL_INL_LINE_SEGMENT_2D

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
	if (!impl::checkSequenceSize(iValue, 2))
	{
		impl::addMessageHeader("LineSegment2D");
		return 1;
	}

	typename TLineSegment::TPoint tail;
	if (pyGetSimpleObject(PySequence_Fast_GET_ITEM(iValue, 0), tail) != 0)
	{
		impl::addMessageHeader("LineSegment2D: tail");
		return 1;
	}

	typename TLineSegment::TPoint head;
	if (pyGetSimpleObject(PySequence_Fast_GET_ITEM(iValue, 1), head) != 0)
	{
		impl::addMessageHeader("LineSegment2D: head");
		return 1;
	}

	oV = TLineSegment(tail, head);
	return 0;
}

#	endif
#endif

#ifdef LASS_GUARDIAN_OF_INCLUSION_PRIM_LINE_SEGMENT_3D_H
#	ifndef LASS_GUARDIAN_OF_INCLUSION_PRIM_PYOBJECT_UTIL_INL_LINE_SEGMENT_3D
#	define LASS_GUARDIAN_OF_INCLUSION_PRIM_PYOBJECT_UTIL_INL_LINE_SEGMENT_3D

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
	if (!impl::checkSequenceSize(iValue, 2))
	{
		impl::addMessageHeader("LineSegment3D");
		return 1;
	}

	typename TLineSegment::TPoint tail;
	if (pyGetSimpleObject(PySequence_Fast_GET_ITEM(iValue, 0), tail) != 0)
	{
		impl::addMessageHeader("LineSegment3D: tail");
		return 1;
	}

	typename TLineSegment::TPoint head;
	if (pyGetSimpleObject(PySequence_Fast_GET_ITEM(iValue, 1), head) != 0)
	{
		impl::addMessageHeader("LineSegment3D: head");
		return 1;
	}

	oV = TLineSegment(tail, head);
	return 0;
}

#	endif
#endif

// --- transformations -----------------------------------------------------------------------------

#ifdef LASS_GUARDIAN_OF_INCLUSION_PRIM_TRANSFORMATION_3D_H
#	ifndef LASS_GUARDIAN_OF_INCLUSION_PRIM_PYOBJECT_UTIL_INL_TRANSFORMATION_3D
#	define LASS_GUARDIAN_OF_INCLUSION_PRIM_PYOBJECT_UTIL_INL_TRANSFORMATION_3D

template <typename T>
PyObject* pyBuildSimpleObject(const prim::Transformation3D<T>& iV)
{
	const T* const v = iV.matrix();
	PyObject* const matrix = PyTuple_New(4);
	for (unsigned i = 0; i < 4; ++i)
	{
		PyObject* const row = PyTuple_New(4);
		for (unsigned j = 0; j < 4; ++j)
		{
			PyTuple_SetItem(row, j, pyBuildSimpleObject(v[i * 4 + j]));
		}
		PyTuple_SetItem(matrix, i, row);
	}
	return matrix;
}



template <typename T>
int pyGetSimpleObject(PyObject* iValue, prim::Transformation3D<T>& oV)
{
	if (!impl::checkSequenceSize(iValue, 4))
	{
		impl::addMessageHeader("Transformation3D");
		return 1;
	}
	T values[16];
	for (unsigned i = 0; i < 4; ++i)
	{
		PyObject* row = PySequence_Fast_GET_ITEM(iValue, i);
		if (!impl::checkSequenceSize(row, 4))
		{
			impl::addMessageHeader("Transformation3D: row " + util::stringCast<std::string>(i));
			return 1;
		}
		for (unsigned j = 0; j < 4; ++j)
		{
			if (pyGetSimpleObject(PySequence_Fast_GET_ITEM(row, j), values[4 * i + j]) != 0)
			{
				impl::addMessageHeader("Transformation3D: row " + util::stringCast<std::string>(i) +
					", column " + util::stringCast<std::string>(j));
				return 1;
			}
		}
	}

	oV = prim::Transformation3D<T>(values, values + 16);
	return 0;
}

#	endif
#endif

}

}

#endif

// EOF