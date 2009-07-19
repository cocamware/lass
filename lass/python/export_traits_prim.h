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


#ifndef LASS_GUARDIAN_OF_INCLUSION_PYTHON_EXPORT_TRAITS_PRIM_H
#define LASS_GUARDIAN_OF_INCLUSION_PYTHON_EXPORT_TRAITS_PRIM_H

#include "python_common.h"
#include "pyobject_plus.h"
#include "py_tuple.h"

namespace lass
{
namespace python
{
namespace impl
{

/** @ingroup Python
 *  @internal
 */
template <typename ObjectType, typename ExportTraits, size_t dimension = ObjectType::dimension>
struct PyExportTraitsVectorPoint
{
	static PyObject* build(const ObjectType& v)
	{
		PyObject* const tuple = PyTuple_New(dimension);
		for (size_t i = 0; i < dimension; ++i)
		{
			PyTuple_SetItem(tuple, i, pyBuildSimpleObject(v[i]));
		}
		return tuple;
	}

	static int get(PyObject* obj, ObjectType& v)
	{
		const TPyObjPtr tuple = impl::checkedFastSequence(obj, dimension);
		if (!tuple)
		{
			impl::addMessageHeader(ExportTraits::className());
			return 1;
		}
		PyObject** objects = PySequence_Fast_ITEMS(tuple.get());
		ObjectType result;
		for (size_t k = 0; k < dimension; ++k)
		{
			if (!impl::decodeObject(objects[k], result[k], k + 1))
			{
				impl::addMessageHeader(ExportTraits::className());
				return 1;
			}
		}
		v = result;
		return 0;
	}
};

/** @ingroup Python
 *  @internal
 */
template < typename AabbType, typename ExportTraits >
struct PyExportTraitsPrimAabb
{
	typedef AabbType TAabb;
	static PyObject* build(const TAabb& aabb)
	{
		return fromSharedPtrToNakedCast(makeTuple(aabb.min(), aabb.max()));
	}
	static int get(PyObject* obj, TAabb& aabb)
	{
		typename TAabb::TPoint min, max;
		if (decodeTuple(obj, min, max) != 0)
		{
			impl::addMessageHeader(ExportTraits::className());
			return 1;
		}
		try
		{
			aabb = TAabb(min, max);
		}
		catch (util::Exception& error)
		{
			std::ostringstream buffer;
			buffer << ExportTraits::className() << ": " << error.message();
			PyErr_SetString(PyExc_ValueError, buffer.str().c_str());
			return 1;
		}
		return 0;
	}
};

/** @ingroup Python
 *  @internal
 */
template < typename LineSegmentType, typename ExportTraits >
struct PyExportTraitsPrimLineSegment
{
	typedef LineSegmentType TLineSegment;
	static PyObject* build(const TLineSegment& seg)
	{
		return fromSharedPtrToNakedCast(makeTuple(seg.tail(), seg.head()));
	}
	static int get(PyObject* obj, TLineSegment& seg)
	{
		typename TLineSegment::TPoint tail, head;
		if (decodeTuple(obj, tail, head) != 0)
		{
			impl::addMessageHeader(ExportTraits::className());
			return 1;
		}
		seg = TLineSegment(tail, head);
		return 0;
	}
};

/** @ingroup Python
 *  @internal
 */
template <typename TransformationType, typename ExporTraits>
struct PyExportTraitsPrimTransformation
{
	typedef TransformationType TTransformation;
	typedef typename TransformationType::TValue TValue;
	enum { size = TransformationType::dimension + 1 };

	static PyObject* build(const TTransformation& transfo)
	{
		const TValue* const values = transfo.matrix();
		PyObject* const matrix = PyTuple_New(size);
		for (int i = 0; i < size; ++i)
		{
			PyObject* const row = PyTuple_New(size);
			for (int j = 0; j < size; ++j)
			{
				PyTuple_SetItem(row, j, pyBuildSimpleObject(values[i * size + j]));
			}
			PyTuple_SetItem(matrix, i, row);
		}
		return matrix;
	}

	static int get(PyObject* obj, TTransformation& transfo)
	{
		TPyObjPtr tuple = impl::checkedFastSequence(obj, size);
		if (!tuple)
		{
			impl::addMessageHeader(ExporTraits::className());
			return 1;
		}
		PyObject** rows = PySequence_Fast_ITEMS(tuple.get());
		TValue values[size * size];
		for (int i = 0; i < size; ++i)
		{
			TPyObjPtr row = impl::checkedFastSequence(rows[i], size);
			if (!row)
			{
				std::ostringstream buffer;
				buffer << ExporTraits::className() << ": row " << i;
				impl::addMessageHeader(buffer.str());
				return 1;
			}
			PyObject** objects = PySequence_Fast_ITEMS(row.get());
			for (int j = 0; j < size; ++j)
			{
				if (pyGetSimpleObject(objects[j], values[size * i + j]) != 0)
				{
					std::ostringstream buffer;
					buffer << ExporTraits::className() << ": row " << i << ", column " << j;
					impl::addMessageHeader(buffer.str());
					return 1;
				}
			}
		}
		transfo = TTransformation(values, values + size * size);
		return 0;
	}
};

/** @ingroup Python
 *  @internal
 */
template <typename PolygonType, typename ExportTraits>
struct PyExportTraitsPrimSimplePolygon
{
	typedef PolygonType TPolygon;
	typedef typename PolygonType::TPoint TPoint;
	static PyObject* build(const TPolygon& poly)
	{
		std::vector<TPoint> points;
		for (unsigned i = 0; i < poly.size(); ++i)
			points.push_back(poly[i]);
		return pyBuildSimpleObject( points );
	}
	static int get(PyObject* obj, TPolygon& poly)
	{
		std::vector<TPoint> points;
		if (pyGetSimpleObject(obj, points) != 0)
		{
			impl::addMessageHeader(ExportTraits::className());
			return 1;
		}
		poly = TPolygon(points.begin(), points.end());
		return 0;
	}
};

/** @ingroup Python
 *  @internal
 */
template <typename AxisType, typename ExportTraits>
struct PyExportTraitsPrimAxis
{
	typedef AxisType TAxis;
	static PyObject* build(const TAxis& axis)
	{
		return pyBuildSimpleObject(std::string(1, axis.axis()));
	}
	static int get(PyObject* obj, TAxis& axis)
	{
		std::string s;
		if (pyGetSimpleObject(obj, s) != 0)
		{
			impl::addMessageHeader(ExportTraits::className());
			return 1;
		}
		try
		{
			axis = TAxis(s);
		}
		catch (util::Exception& error)
		{
			std::ostringstream buffer;
			buffer << ExportTraits::className() << ": " << error.message();
			PyErr_SetString(PyExc_ValueError, buffer.str().c_str());
			return 1;
		}
		return 0;
	}
};

LASS_DLL PyObject* LASS_CALL buildIndexVertex(size_t vertex, size_t normal, size_t uv);
LASS_DLL int LASS_CALL getIndexVertex(PyObject* iIndices, size_t& vertex, size_t& normal, size_t& uv);

}
}
}

#endif

namespace lass
{
namespace python
{

// --- vectors -------------------------------------------------------------------------------------

#if defined(LASS_PRIM_HAVE_PY_EXPORT_TRAITS_VECTOR_2D)
#	ifndef LASS_GUARDIAN_OF_INCLUSION_PYTHON_EXPORT_TRAITS_PRIM_H_VECTOR_2D
#	define LASS_GUARDIAN_OF_INCLUSION_PYTHON_EXPORT_TRAITS_PRIM_H_VECTOR_2D

/** @ingroup Python
 *  @internal
 */
template <typename T>
struct PyExportTraits< prim::Vector2D<T> >:
	public impl::PyExportTraitsVectorPoint< prim::Vector2D<T>, PyExportTraits< prim::Vector2D<T> > >
{
	static const char* className() { return "Vector2D"; }
};

#	endif
#endif



#if defined(LASS_PRIM_HAVE_PY_EXPORT_TRAITS_VECTOR_3D)
#	ifndef LASS_GUARDIAN_OF_INCLUSION_PYTHON_EXPORT_TRAITS_PRIM_H_VECTOR_3D
#	define LASS_GUARDIAN_OF_INCLUSION_PYTHON_EXPORT_TRAITS_PRIM_H_VECTOR_3D

/** @ingroup Python
 *  @internal
 */
template <typename T>
struct PyExportTraits< prim::Vector3D<T> >:
	public impl::PyExportTraitsVectorPoint< prim::Vector3D<T>, PyExportTraits< prim::Vector3D<T> > >
{
	static const char* className() { return "Vector3D"; }
};

#	endif
#endif


#if defined(LASS_PRIM_HAVE_PY_EXPORT_TRAITS_VECTOR_4D)
#	ifndef LASS_GUARDIAN_OF_INCLUSION_PYTHON_EXPORT_TRAITS_PRIM_H_VECTOR_4D
#	define LASS_GUARDIAN_OF_INCLUSION_PYTHON_EXPORT_TRAITS_PRIM_H_VECTOR_4D

/** @ingroup Python
 *  @internal
 */
template <typename T>
struct PyExportTraits< prim::Vector4D<T> >:
	public impl::PyExportTraitsVectorPoint< prim::Vector4D<T>, PyExportTraits< prim::Vector4D<T> > >
{
	static const char* className() { return "Vector4D"; }
};

#	endif
#endif



// --- points --------------------------------------------------------------------------------------

#if defined(LASS_PRIM_HAVE_PY_EXPORT_TRAITS_POINT_2D)
#	ifndef LASS_GUARDIAN_OF_INCLUSION_PYTHON_EXPORT_TRAITS_PRIM_H_POINT_2D
#	define LASS_GUARDIAN_OF_INCLUSION_PYTHON_EXPORT_TRAITS_PRIM_H_POINT_2D

/** @ingroup Python
 *  @internal
 */
template <typename T>
struct PyExportTraits< prim::Point2D<T> >:
	public impl::PyExportTraitsVectorPoint< prim::Point2D<T>, PyExportTraits< prim::Point2D<T> > >
{
	static const char* className() { return "Point2D"; }
};

#	endif
#endif



#if defined(LASS_PRIM_HAVE_PY_EXPORT_TRAITS_POINT_3D)
#	ifndef LASS_GUARDIAN_OF_INCLUSION_PYTHON_EXPORT_TRAITS_PRIM_H_POINT_3D
#	define LASS_GUARDIAN_OF_INCLUSION_PYTHON_EXPORT_TRAITS_PRIM_H_POINT_3D

/** @ingroup Python
 *  @internal
 */
template <typename T>
struct PyExportTraits< prim::Point3D<T> >:
	public impl::PyExportTraitsVectorPoint< prim::Point3D<T>, PyExportTraits< prim::Point3D<T> > >
{
	static const char* className() { return "Point3D"; }
};

#	endif
#endif



// --- boxes ---------------------------------------------------------------------------------------

#if defined(LASS_PRIM_HAVE_PY_EXPORT_TRAITS_AABB_2D)
#	ifndef LASS_GUARDIAN_OF_INCLUSION_PYTHON_EXPORT_TRAITS_PRIM_H_AABB_2D
#	define LASS_GUARDIAN_OF_INCLUSION_PYTHON_EXPORT_TRAITS_PRIM_H_AABB_2D

/** @ingroup Python
 *  @internal
 */
template <typename T, typename MMP>
struct PyExportTraits< prim::Aabb2D<T, MMP> >:
	public impl::PyExportTraitsPrimAabb< prim::Aabb2D<T, MMP>, PyExportTraits< prim::Aabb2D<T, MMP> > >
{
	static const char* className() { return "Aabb2D"; }
};

#	endif
#endif

#if defined(LASS_PRIM_HAVE_PY_EXPORT_TRAITS_AABB_3D)
#	ifndef LASS_GUARDIAN_OF_INCLUSION_PYTHON_EXPORT_TRAITS_PRIM_H_AABB_3D
#	define LASS_GUARDIAN_OF_INCLUSION_PYTHON_EXPORT_TRAITS_PRIM_H_AABB_3D

/** @ingroup Python
 *  @internal
 */
template <typename T, typename MMP>
struct PyExportTraits< prim::Aabb3D<T, MMP> >:
	public impl::PyExportTraitsPrimAabb< prim::Aabb3D<T, MMP>, PyExportTraits< prim::Aabb3D<T, MMP> > >
{
	static const char* className() { return "Aabb3D"; }
};

#	endif
#endif



// --- line segments -------------------------------------------------------------------------------

#if defined(LASS_PRIM_HAVE_PY_EXPORT_TRAITS_LINE_SEGMENT_2D)
#	ifndef LASS_GUARDIAN_OF_INCLUSION_PYTHON_EXPORT_TRAITS_PRIM_H_LINE_SEGMENT_2D
#	define LASS_GUARDIAN_OF_INCLUSION_PYTHON_EXPORT_TRAITS_PRIM_H_LINE_SEGMENT_2D

/** @ingroup Python
 *  @internal
 */
template <typename T, typename PP>
struct PyExportTraits< prim::LineSegment2D<T, PP> >:
	public impl::PyExportTraitsPrimLineSegment< prim::LineSegment2D<T, PP>, PyExportTraits< prim::LineSegment2D<T, PP> > >
{
	static const char* className() { return "LineSegment2D"; }
};

#	endif
#endif

#if defined(LASS_PRIM_HAVE_PY_EXPORT_TRAITS_LINE_SEGMENT_3D)
#	ifndef LASS_GUARDIAN_OF_INCLUSION_PYTHON_EXPORT_TRAITS_PRIM_H_LINE_SEGMENT_3D
#	define LASS_GUARDIAN_OF_INCLUSION_PYTHON_EXPORT_TRAITS_PRIM_H_LINE_SEGMENT_3D

/** @ingroup Python
 *  @internal
 */
template <typename T, typename PP>
struct PyExportTraits< prim::LineSegment3D<T, PP> >:
	public impl::PyExportTraitsPrimLineSegment< prim::LineSegment3D<T, PP>, PyExportTraits< prim::LineSegment3D<T, PP> > >
{
	static const char* className() { return "LineSegment3D"; }
};

#	endif
#endif



// --- transformations -----------------------------------------------------------------------------

#if defined(LASS_PRIM_HAVE_PY_EXPORT_TRAITS_TRANSFORMATION_2D)
#	ifndef LASS_GUARDIAN_OF_INCLUSION_PYTHON_EXPORT_TRAITS_PRIM_H_TRANSFORMATION_2D
#	define LASS_GUARDIAN_OF_INCLUSION_PYTHON_EXPORT_TRAITS_PRIM_H_TRANSFORMATION_2D

/** @ingroup Python
 *  @internal
 */
template <typename T>
struct PyExportTraits< prim::Transformation2D<T> >: 
	public impl::PyExportTraitsPrimTransformation< prim::Transformation2D<T>, PyExportTraits< prim::Transformation2D<T> > >
{
	static const char* className() { return "Transformation2D"; }
};

#	endif
#endif

#if defined(LASS_PRIM_HAVE_PY_EXPORT_TRAITS_TRANSFORMATION_3D)
#	ifndef LASS_GUARDIAN_OF_INCLUSION_PYTHON_EXPORT_TRAITS_PRIM_H_TRANSFORMATION_3D
#	define LASS_GUARDIAN_OF_INCLUSION_PYTHON_EXPORT_TRAITS_PRIM_H_TRANSFORMATION_3D

/** @ingroup Python
 *  @internal
 */
template <typename T>
struct PyExportTraits< prim::Transformation3D<T> >: 
	public impl::PyExportTraitsPrimTransformation< prim::Transformation3D<T>, PyExportTraits< prim::Transformation3D<T> > >
{
	static const char* className() { return "Transformation3D"; }
};

#	endif
#endif



// --- simplepolygons -----------------------------------------------------------------------------

#ifndef LASS_GUARDIAN_OF_INCLUSION_PYTHON_EXPORT_TRAITS_PRIM_H_SIMPLE_POLYGON
#define LASS_GUARDIAN_OF_INCLUSION_PYTHON_EXPORT_TRAITS_PRIM_H_SIMPLE_POLYGON


#endif

#if defined(LASS_PRIM_HAVE_PY_EXPORT_TRAITS_SIMPLE_POLYGON_2D)
#	ifndef LASS_GUARDIAN_OF_INCLUSION_PYTHON_EXPORT_TRAITS_PRIM_H_SIMPLE_POLYGON_2D
#	define LASS_GUARDIAN_OF_INCLUSION_PYTHON_EXPORT_TRAITS_PRIM_H_SIMPLE_POLYGON_2D

/** @ingroup Python
 *  @internal
 */
template <typename T, typename DP>
struct PyExportTraits< prim::SimplePolygon2D<T, DP> >:
	public impl::PyExportTraitsPrimSimplePolygon< prim::SimplePolygon2D<T, DP>, PyExportTraits< prim::SimplePolygon2D<T, DP> > >
{
	static const char* className() { return "SimplePolygon2D"; }
};

#	endif
#endif


#if defined(LASS_PRIM_HAVE_PY_EXPORT_TRAITS_SIMPLE_POLYGON_3D)
#	ifndef LASS_GUARDIAN_OF_INCLUSION_PYTHON_EXPORT_TRAITS_PRIM_H_SIMPLE_POLYGON_3D
#	define LASS_GUARDIAN_OF_INCLUSION_PYTHON_EXPORT_TRAITS_PRIM_H_SIMPLE_POLYGON_3D

/** @ingroup Python
 *  @internal
 */
template <typename T, typename DP>
struct PyExportTraits< prim::SimplePolygon3D<T, DP> >:
	public impl::PyExportTraitsPrimSimplePolygon< prim::SimplePolygon3D<T, DP>, PyExportTraits< prim::SimplePolygon3D<T, DP> > >
{
	static const char* className() { return "SimplePolygon3D"; }
};

#	endif
#endif

// --- axes ----------------------------------------------------------------------------------------

#ifndef LASS_GUARDIAN_OF_INCLUSION_PYTHON_EXPORT_TRAITS_PRIM_H_AXIS
#define LASS_GUARDIAN_OF_INCLUSION_PYTHON_EXPORT_TRAITS_PRIM_H_AXIS


#endif

#if defined(LASS_PRIM_HAVE_PY_EXPORT_TRAITS_XY)
#	ifndef LASS_GUARDIAN_OF_INCLUSION_PYTHON_EXPORT_TRAITS_PRIM_H_XY
#	define LASS_GUARDIAN_OF_INCLUSION_PYTHON_EXPORT_TRAITS_PRIM_H_XY

/** @ingroup Python
 *  @internal
 */
template <>
struct PyExportTraits<prim::XY>:
	public impl::PyExportTraitsPrimAxis<prim::XY, PyExportTraits<prim::XY> >
{
	static const char* className() { return "XY"; }
};

#	endif
#endif

#if defined(LASS_PRIM_HAVE_PY_EXPORT_TRAITS_XYZ)
#	ifndef LASS_GUARDIAN_OF_INCLUSION_PYTHON_EXPORT_TRAITS_PRIM_H_XYZ
#	define LASS_GUARDIAN_OF_INCLUSION_PYTHON_EXPORT_TRAITS_PRIM_H_XYZ

/** @ingroup Python
 *  @internal
 */
template <>
struct PyExportTraits<prim::XYZ>:
	public impl::PyExportTraitsPrimAxis<prim::XYZ, PyExportTraits<prim::XYZ> >
{
	static const char* className() { return "XYZ"; }
};

#	endif
#endif

#if defined(LASS_PRIM_HAVE_PY_EXPORT_TRAITS_XYZW)
#	ifndef LASS_GUARDIAN_OF_INCLUSION_PYTHON_EXPORT_TRAITS_PRIM_H_XYZW
#	define LASS_GUARDIAN_OF_INCLUSION_PYTHON_EXPORT_TRAITS_PRIM_H_XYZW

/** @ingroup Python
 *  @internal
 */
template <>
struct PyExportTraits<prim::XYZW>:
	public impl::PyExportTraitsPrimAxis<prim::XYZW, PyExportTraits<prim::XYZW> >
{
	static const char* className() { return "XYWZ"; }
};

#	endif
#endif

// --- ColorRGBA -----------------------------------------------------------------------------------

#if defined(LASS_PRIM_HAVE_PY_EXPORT_TRAITS_COLOR_RGBA)
#	ifndef LASS_GUARDIAN_OF_INCLUSION_PYTHON_EXPORT_TRAITS_PRIM_H_COLOR_RGBA
#	define LASS_GUARDIAN_OF_INCLUSION_PYTHON_EXPORT_TRAITS_PRIM_H_COLOR_RGBA

/** @ingroup Python
 *  @internal
 */
template <>
struct PyExportTraits<prim::ColorRGBA>
{
	static PyObject* build(const prim::ColorRGBA& v)
	{
		return pyBuildSimpleObject(v.vector());
	}
	static int get(PyObject* obj, prim::ColorRGBA& v)
	{
		prim::ColorRGBA x(0, 0, 0, 1);
		if (decodeTupleMinimum(obj, 3, x.r, x.g, x.b, x.a))
		{
			impl::addMessageHeader("ColorRGBA");
			return 1;
		}
		v = x;
		return 0;
	}
};

#	endif
#endif

// --- IndexTriangle -------------------------------------------------------------------------------

#if defined(LASS_PRIM_HAVE_PY_EXPORT_TRAITS_TRIANGLE_MESH_3D)
#	ifndef LASS_GUARDIAN_OF_INCLUSION_PYTHON_EXPORT_TRAITS_PRIM_H_INDEX_TRIANGLE
#	define LASS_GUARDIAN_OF_INCLUSION_PYTHON_EXPORT_TRAITS_PRIM_H_INDEX_TRIANGLE

namespace impl
{
}

/** @ingroup Python
 *  @internal
 */
template <>
struct PyExportTraits<prim::IndexTriangle>
{
	static PyObject* build(const prim::IndexTriangle& iTriangle)
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
	static int get(PyObject* obj, prim::IndexTriangle& oTriangle)
	{
		const TPyObjPtr tuple(impl::checkedFastSequence(obj, 3));
		if (!tuple)
		{
			impl::addMessageHeader("IndexTriangle");
			return 1;
		}
		PyObject** objects = PySequence_Fast_ITEMS(tuple.get());
		prim::IndexTriangle result = {{0, 0, 0}, {0, 0, 0}, {0, 0, 0}};
		for (int k = 0; k < 3; ++k)
		{
			if (impl::getIndexVertex(objects[k], result.vertices[k], result.normals[k], result.uvs[k]) != 0)
			{
				std::ostringstream buffer;
				buffer << "IndexTriangle: " << (k + 1) << "th vertex";
				impl::addMessageHeader(buffer.str());
				return 1;
			}
		}
		oTriangle = result;
		return 0;
	}
};

#	endif
#endif

}
}

// EOF
