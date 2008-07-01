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

#ifdef LASS_GUARDIAN_OF_INCLUSION_PRIM_PYOBJECT_UTIL_H

#include "../util/py_tuple.h"

namespace lass
{
namespace python
{

// --- vectors -------------------------------------------------------------------------------------

#ifdef LASS_GUARDIAN_OF_INCLUSION_PRIM_VECTOR_2D_H
#	ifndef LASS_GUARDIAN_OF_INCLUSION_PRIM_PYOBJECT_UTIL_INL_VECTOR_2D
#	define LASS_GUARDIAN_OF_INCLUSION_PRIM_PYOBJECT_UTIL_INL_VECTOR_2D

template <typename T>
PyObject* pyBuildSimpleObject_deprecated(const prim::Vector2D<T>& iV)
{
	return fromSharedPtrToNakedCast(makeTuple(iV.x, iV.y));
}

template <typename T>
int pyGetSimpleObject_deprecated(PyObject* iValue, prim::Vector2D<T>& oV)
{
	T x, y;
	if (decodeTuple(iValue, x, y) != 0)
	{
		impl::addMessageHeader("Vector2D");
		return 1;
	}
	oV = prim::Vector2D<T>(x, y);
	return 0;
}

#	endif
#endif



#ifdef LASS_GUARDIAN_OF_INCLUSION_PRIM_VECTOR_3D_H
#	ifndef LASS_GUARDIAN_OF_INCLUSION_PRIM_PYOBJECT_UTIL_INL_VECTOR_3D
#	define LASS_GUARDIAN_OF_INCLUSION_PRIM_PYOBJECT_UTIL_INL_VECTOR_3D

template <typename T>
PyObject* pyBuildSimpleObject_deprecated(const prim::Vector3D<T>& iV)
{
	return fromSharedPtrToNakedCast(makeTuple(iV.x, iV.y, iV.z));
}

template <typename T>
int pyGetSimpleObject_deprecated(PyObject* iValue, prim::Vector3D<T>& oV)
{
	T x, y, z;
	if (decodeTuple(iValue, x, y, z) != 0)
	{
		impl::addMessageHeader("Vector3D");
		return 1;
	}
	oV = prim::Vector3D<T>(x, y, z);
	return 0;
}

#	endif
#endif


#ifdef LASS_GUARDIAN_OF_INCLUSION_PRIM_VECTOR_4D_H
#	ifndef LASS_GUARDIAN_OF_INCLUSION_PRIM_PYOBJECT_UTIL_INL_VECTOR_4D
#	define LASS_GUARDIAN_OF_INCLUSION_PRIM_PYOBJECT_UTIL_INL_VECTOR_4D

template <typename T>
PyObject* pyBuildSimpleObject_deprecated(const prim::Vector4D<T>& iV)
{
	return fromSharedPtrToNakedCast(makeTuple(iV.x, iV.y, iV.z, iV.w));
}

template <typename T>
int pyGetSimpleObject_deprecated(PyObject* iValue, prim::Vector4D<T>& oV)
{
	T x, y, z, w;
	if (decodeTuple(iValue, x, y, z, w) != 0)
	{
		impl::addMessageHeader("Vector4D");
		return 1;
	}
	oV = prim::Vector4D<T>(x, y, z, w);
	return 0;
}

#	endif
#endif



// --- points --------------------------------------------------------------------------------------

#ifdef LASS_GUARDIAN_OF_INCLUSION_PRIM_POINT_2D_H
#	ifndef LASS_GUARDIAN_OF_INCLUSION_PRIM_PYOBJECT_UTIL_INL_POINT_2D
#	define LASS_GUARDIAN_OF_INCLUSION_PRIM_PYOBJECT_UTIL_INL_POINT_2D

template <typename T>
PyObject* pyBuildSimpleObject_deprecated(const prim::Point2D<T>& iV)
{
	return pyExportTraitBuild(iV.position());
}

template <typename T>
int pyGetSimpleObject_deprecated(PyObject* iValue, prim::Point2D<T>& oV)
{
	T x, y;
	if (decodeTuple(iValue, x, y) != 0)
	{
		impl::addMessageHeader("Point2D");
		return 1;
	}
	oV = prim::Point2D<T>(x, y);
	return 0;
}

#	endif
#endif



#ifdef LASS_GUARDIAN_OF_INCLUSION_PRIM_POINT_3D_H
#	ifndef LASS_GUARDIAN_OF_INCLUSION_PRIM_PYOBJECT_UTIL_INL_POINT_3D
#	define LASS_GUARDIAN_OF_INCLUSION_PRIM_PYOBJECT_UTIL_INL_POINT_3D

template <typename T>
PyObject* pyBuildSimpleObject_deprecated(const prim::Point3D<T>& iV)
{
	return pyExportTraitBuild(iV.position());
}

template <typename T>
int pyGetSimpleObject_deprecated(PyObject* iValue, prim::Point3D<T>& oV)
{
	T x, y, z;
	if (decodeTuple(iValue, x, y, z) != 0)
	{
		impl::addMessageHeader("Point3D");
		return 1;
	}
	oV = prim::Point3D<T>(x, y, z);
	return 0;
}

#	endif
#endif



// --- boxes ---------------------------------------------------------------------------------------

#ifdef LASS_GUARDIAN_OF_INCLUSION_PRIM_AABB_2D_H
#	ifndef LASS_GUARDIAN_OF_INCLUSION_PRIM_PYOBJECT_UTIL_INL_AABB_2D
#	define LASS_GUARDIAN_OF_INCLUSION_PRIM_PYOBJECT_UTIL_INL_AABB_2D

template <typename T, class MMP>
PyObject* pyBuildSimpleObject_deprecated(const prim::Aabb2D<T, MMP>& iV)
{
	return fromSharedPtrToNakedCast(makeTuple(iV.min(), iV.max()));
}

template <typename T, class MMP>
int pyGetSimpleObject_deprecated(PyObject* iValue, prim::Aabb2D<T, MMP>& oV)
{
	typedef prim::Aabb2D<T, MMP> TAabb;
	
	typename TAabb::TPoint min, max;
	if (decodeTuple(iValue, min, max) != 0)
	{
		impl::addMessageHeader("Aabb2D");
		return 1;
	}

	try
	{
		oV = TAabb(min, max);
	}
	catch (prim::MinMaxError& error)
	{
		std::ostringstream buffer;
		buffer << "Aabb2D: " << error.message();
		PyErr_SetString(PyExc_ValueError, buffer.str().c_str());
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
PyObject* pyBuildSimpleObject_deprecated(const prim::Aabb3D<T, MMP>& iV)
{
	return fromSharedPtrToNakedCast(makeTuple(iV.min(), iV.max()));
}

template <typename T, class MMP>
int pyGetSimpleObject_deprecated(PyObject* iValue, prim::Aabb3D<T, MMP>& oV)
{
	typedef prim::Aabb3D<T, MMP> TAabb;
	
	typename TAabb::TPoint min, max;
	if (decodeTuple(iValue, min, max) != 0)
	{
		impl::addMessageHeader("Aabb3D");
		return 1;
	}

	try
	{
		oV = TAabb(min, max);
	}
	catch (prim::MinMaxError& error)
	{
		std::ostringstream buffer;
		buffer << "Aabb3D: " << error.message();
		PyErr_SetString(PyExc_ValueError, buffer.str().c_str());
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
PyObject* pyBuildSimpleObject_deprecated(const prim::LineSegment2D<T, PP>& iV)
{
	return fromSharedPtrToNakedCast(makeTuple(iV.tail(), iV.head()));
}

template <typename T, class PP>
int pyGetSimpleObject_deprecated(PyObject* iValue, prim::LineSegment2D<T, PP>& oV)
{
	typedef prim::LineSegment2D<T, PP> TLineSegment;

	typename TLineSegment::TPoint tail, head;
	if (decodeTuple(iValue, tail, head) != 0)
	{
		impl::addMessageHeader("LineSegment2D");
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
PyObject* pyBuildSimpleObject_deprecated(const prim::LineSegment3D<T, PP>& iV)
{
	return fromSharedPtrToNakedCast(makeTuple(iV.tail(), iV.head()));
}

template <typename T, class PP>
int pyGetSimpleObject_deprecated(PyObject* iValue, prim::LineSegment3D<T, PP>& oV)
{
	typedef prim::LineSegment3D<T, PP> TLineSegment;

	typename TLineSegment::TPoint tail, head;
	if (decodeTuple(iValue, tail, head) != 0)
	{
		impl::addMessageHeader("LineSegment3D");
		return 1;
	}
	oV = TLineSegment(tail, head);
	return 0;
}

#	endif
#endif

// --- transformations -----------------------------------------------------------------------------

#ifdef LASS_GUARDIAN_OF_INCLUSION_PRIM_TRANSFORMATION_2D_H
#	ifndef LASS_GUARDIAN_OF_INCLUSION_PRIM_PYOBJECT_UTIL_INL_TRANSFORMATION_2D
#	define LASS_GUARDIAN_OF_INCLUSION_PRIM_PYOBJECT_UTIL_INL_TRANSFORMATION_2D

template <typename T>
PyObject* pyBuildSimpleObject_deprecated(const prim::Transformation2D<T>& iV)
{
	const T* const v = iV.matrix();
	PyObject* const matrix = PyTuple_New(3);
	for (unsigned i = 0; i < 3; ++i)
	{
		PyObject* const row = PyTuple_New(3);
		for (unsigned j = 0; j < 3; ++j)
		{
			PyTuple_SetItem(row, j, pyExportTraitBuild(v[i * 3 + j]));
		}
		PyTuple_SetItem(matrix, i, row);
	}
	return matrix;
}



template <typename T>
int pyGetSimpleObject_deprecated(PyObject* iValue, prim::Transformation2D<T>& oV)
{
	if (!impl::checkSequenceSize(iValue, 3))
	{
		impl::addMessageHeader("Transformation2D");
		return 1;
	}
	T values[9];
	for (unsigned i = 0; i < 3; ++i)
	{
		PyObject* row = PySequence_Fast_GET_ITEM(iValue, i);
		if (!impl::checkSequenceSize(row, 3))
		{
			impl::addMessageHeader("Transformation2D: row " + util::stringCast<std::string>(i));
			return 1;
		}
		for (unsigned j = 0; j < 3; ++j)
		{
			if (pyExportTraitGet(PySequence_Fast_GET_ITEM(row, j), values[3 * i + j]) != 0)
			{
				impl::addMessageHeader("Transformation2D: row " + util::stringCast<std::string>(i) +
					", column " + util::stringCast<std::string>(j));
				return 1;
			}
		}
	}

	oV = prim::Transformation2D<T>(values, values + 9);
	return 0;
}

#	endif
#endif

#ifdef LASS_GUARDIAN_OF_INCLUSION_PRIM_TRANSFORMATION_3D_H
#	ifndef LASS_GUARDIAN_OF_INCLUSION_PRIM_PYOBJECT_UTIL_INL_TRANSFORMATION_3D
#	define LASS_GUARDIAN_OF_INCLUSION_PRIM_PYOBJECT_UTIL_INL_TRANSFORMATION_3D

template <typename T>
PyObject* pyBuildSimpleObject_deprecated(const prim::Transformation3D<T>& iV)
{
	const T* const v = iV.matrix();
	PyObject* const matrix = PyTuple_New(4);
	for (unsigned i = 0; i < 4; ++i)
	{
		PyObject* const row = PyTuple_New(4);
		for (unsigned j = 0; j < 4; ++j)
		{
			PyTuple_SetItem(row, j, pyExportTraitBuild(v[i * 4 + j]));
		}
		PyTuple_SetItem(matrix, i, row);
	}
	return matrix;
}



template <typename T>
int pyGetSimpleObject_deprecated(PyObject* iValue, prim::Transformation3D<T>& oV)
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
			if (pyExportTraitGet(PySequence_Fast_GET_ITEM(row, j), values[4 * i + j]) != 0)
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


// --- simplepolygons -----------------------------------------------------------------------------

#ifdef LASS_GUARDIAN_OF_INCLUSION_PRIM_SIMPLE_POLYGON_2D_H
#	ifndef LASS_GUARDIAN_OF_INCLUSION_PRIM_PYOBJECT_UTIL_INL_SIMPLE_POLYGON_2D
#	define LASS_GUARDIAN_OF_INCLUSION_PRIM_PYOBJECT_UTIL_INL_SIMPLE_POLYGON_2D

template <typename T, class DegeneratePolicy>
PyObject* pyBuildSimpleObject_deprecated(const prim::SimplePolygon2D<T, DegeneratePolicy>& iV)
{
	std::vector< prim::Point2D<T> > points;
	for (unsigned i = 0; i < iV.size(); ++i)
		points.push_back(iV[i]);
	return pyExportTraitBuild( points );
}

template <typename T, class DegeneratePolicy>
int pyGetSimpleObject_deprecated(PyObject* iValue, prim::SimplePolygon2D<T, DegeneratePolicy>& oV)
{
	std::vector< prim::Point2D<T> > points;
	int rv = pyExportTraitGet(iValue, points);
	if (rv)
	{
		impl::addMessageHeader("SimplePolygon2D");
		return 1;
	}
	oV = prim::SimplePolygon2D<T, DegeneratePolicy>(points.begin(), points.end());
	return 0;
}

#	endif
#endif


#ifdef LASS_GUARDIAN_OF_INCLUSION_PRIM_SIMPLE_POLYGON_3D_H
#	ifndef LASS_GUARDIAN_OF_INCLUSION_PRIM_PYOBJECT_UTIL_INL_SIMPLE_POLYGON_3D
#	define LASS_GUARDIAN_OF_INCLUSION_PRIM_PYOBJECT_UTIL_INL_SIMPLE_POLYGON_3D

template <typename T, class PlaneEquationPolicy, class PlaneNormalizingPolicy>
PyObject* pyBuildSimpleObject_deprecated(const prim::SimplePolygon3D<T, PlaneEquationPolicy, PlaneNormalizingPolicy>& iV)
{
	std::vector< prim::Point3D<T> > points;
	for (unsigned i = 0; i < iV.size(); ++i)
		points.push_back(iV[i]);
	return pyExportTraitBuild( points );
}

template <typename T, class PlaneEquationPolicy, class PlaneNormalizingPolicy>
int pyGetSimpleObject_deprecated(PyObject* iValue, prim::SimplePolygon3D<T, PlaneEquationPolicy, PlaneNormalizingPolicy>& oV)
{
	std::vector< prim::Point3D<T> > points;
	int rv = pyExportTraitGet(iValue, oV);
	if (rv)
	{
		impl::addMessageHeader("SimplePolygon3D");
		return 1;
	}
	oV = prim::SimplePolygon3D<T, PlaneEquationPolicy, PlaneNormalizingPolicy>(points.begin(), points.end());
	return 0;
}

#	endif
#endif

}

}

#endif

// EOF
