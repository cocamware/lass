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
		impl::addMessageHeader("Point2D");
		return 1;
	}
	prim::Point2D<T> result;

	if (pyGetSimpleObject(PySequence_Fast_GET_ITEM(iValue, 0), result.x) != 0)
	{
		impl::addMessageHeader("Point2D: x");
		return 1;
	}
	if (pyGetSimpleObject(PySequence_Fast_GET_ITEM(iValue, 1), result.y) != 0)
	{
		impl::addMessageHeader("Point2D: y");
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

#ifdef LASS_GUARDIAN_OF_INCLUSION_PRIM_TRANSFORMATION_2D_H
#	ifndef LASS_GUARDIAN_OF_INCLUSION_PRIM_PYOBJECT_UTIL_INL_TRANSFORMATION_2D
#	define LASS_GUARDIAN_OF_INCLUSION_PRIM_PYOBJECT_UTIL_INL_TRANSFORMATION_2D

template <typename T>
PyObject* pyBuildSimpleObject(const prim::Transformation2D<T>& iV)
{
	const T* const v = iV.matrix();
	PyObject* const matrix = PyTuple_New(3);
	for (unsigned i = 0; i < 3; ++i)
	{
		PyObject* const row = PyTuple_New(3);
		for (unsigned j = 0; j < 3; ++j)
		{
			PyTuple_SetItem(row, j, pyBuildSimpleObject(v[i * 3 + j]));
		}
		PyTuple_SetItem(matrix, i, row);
	}
	return matrix;
}



template <typename T>
int pyGetSimpleObject(PyObject* iValue, prim::Transformation2D<T>& oV)
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
			if (pyGetSimpleObject(PySequence_Fast_GET_ITEM(row, j), values[3 * i + j]) != 0)
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


// --- simplepolygons -----------------------------------------------------------------------------

#ifdef LASS_GUARDIAN_OF_INCLUSION_PRIM_SIMPLE_POLYGON_2D_H
#	ifndef LASS_GUARDIAN_OF_INCLUSION_PRIM_PYOBJECT_UTIL_INL_SIMPLE_POLYGON_2D
#	define LASS_GUARDIAN_OF_INCLUSION_PRIM_PYOBJECT_UTIL_INL_SIMPLE_POLYGON_2D

template <typename T, class DegeneratePolicy>
PyObject* pyBuildSimpleObject(const prim::SimplePolygon2D<T, DegeneratePolicy>& iV)
{
	std::vector< prim::Point2D<T> > points;
	for (unsigned i = 0; i < iV.size(); ++i)
		points.push_back(iV[i]);
	return pyBuildSimpleObject( points );
}

template <typename T, class DegeneratePolicy>
int pyGetSimpleObject(PyObject* iValue, prim::SimplePolygon2D<T, DegeneratePolicy>& oV)
{
	std::vector< prim::Point2D<T> > points;
	int rv = pyGetSimpleObject(iValue, points);
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
PyObject* pyBuildSimpleObject(const prim::SimplePolygon3D<T, PlaneEquationPolicy, PlaneNormalizingPolicy>& iV)
{
	std::vector< prim::Point3D<T> > points;
	for (unsigned i = 0; i < iV.size(); ++i)
		points.push_back(iV[i]);
	return pyBuildSimpleObject( points );
}

template <typename T, class PlaneEquationPolicy, class PlaneNormalizingPolicy>
int pyGetSimpleObject(PyObject* iValue, prim::SimplePolygon3D<T, PlaneEquationPolicy, PlaneNormalizingPolicy>& oV)
{
	std::vector< prim::Point3D<T> > points;
	int rv = pyGetSimpleObject(iValue, oV);
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
