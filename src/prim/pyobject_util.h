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


#ifndef LASS_GUARDIAN_OF_INCLUSION_PRIM_PYOBJECT_UTIL_H
#define LASS_GUARDIAN_OF_INCLUSION_PRIM_PYOBJECT_UTIL_H

#include "../util/pyobject_plus.h"

namespace lass
{
namespace prim
{

template <typename T> struct Vector2D;
template <typename T> struct Vector3D;
template <typename T> struct Vector4D;
template <typename T> struct Point2D;
template <typename T> struct Point3D;
template <typename T, typename MMP> class Aabb2D;
template <typename T, typename MMP> class Aabb3D;
template <typename T, typename PP> class LineSegment2D;
template <typename T, typename PP> class LineSegment3D;
template <typename T> class Transformation3D;

class XY;
class XYZ;
class XYZW;
class ColorRGBA;
struct IndexTriangle;

}

namespace python
{

template <typename T> PyObject* pyBuildSimpleObject(const prim::Vector2D<T>& iV);
template <typename T> PyObject* pyBuildSimpleObject(const prim::Vector3D<T>& iV);
template <typename T> PyObject* pyBuildSimpleObject(const prim::Vector4D<T>& iV);
template <typename T> PyObject* pyBuildSimpleObject(const prim::Point2D<T>& iV);
template <typename T> PyObject* pyBuildSimpleObject(const prim::Point3D<T>& iV);
template <typename T, typename MMP> PyObject* pyBuildSimpleObject(const prim::Aabb2D<T, MMP>& iV);
template <typename T, typename MMP> PyObject* pyBuildSimpleObject(const prim::Aabb3D<T, MMP>& iV);
template <typename T, typename PP> PyObject* pyBuildSimpleObject(const prim::LineSegment2D<T, PP>& iV);
template <typename T, typename PP> PyObject* pyBuildSimpleObject(const prim::LineSegment3D<T, PP>& iV);
template <typename T> PyObject* pyBuildSimpleObject(const prim::Transformation3D<T>& iV);

LASS_DLL PyObject* pyBuildSimpleObject(const prim::XY& iV);
LASS_DLL PyObject* pyBuildSimpleObject(const prim::XYZ& iV);
LASS_DLL PyObject* pyBuildSimpleObject(const prim::XYZW& iV);
LASS_DLL PyObject* pyBuildSimpleObject(const prim::ColorRGBA& iV);

template <typename T> int pyGetSimpleObject(PyObject* iValue, prim::Vector2D<T>& oV);
template <typename T> int pyGetSimpleObject(PyObject* iValue, prim::Vector3D<T>& oV);
template <typename T> int pyGetSimpleObject(PyObject* iValue, prim::Vector4D<T>& oV);
template <typename T> int pyGetSimpleObject(PyObject* iValue, prim::Point2D<T>& oV);
template <typename T> int pyGetSimpleObject(PyObject* iValue, prim::Point3D<T>& oV);
template <typename T, typename MMP> int pyGetSimpleObject(PyObject* iValue, prim::Aabb2D<T, MMP>& oV);
template <typename T, typename MMP> int pyGetSimpleObject(PyObject* iValue, prim::Aabb3D<T, MMP>& oV);
template <typename T, typename PP> int pyGetSimpleObject(PyObject* iValue, prim::LineSegment2D<T, PP>& oV);
template <typename T, typename PP> int pyGetSimpleObject(PyObject* iValue, prim::LineSegment3D<T, PP>& oV);
template <typename T> int pyGetSimpleObject(PyObject* iValue, prim::Transformation3D<T>& oV);

LASS_DLL int pyGetSimpleObject(PyObject* iValue, prim::XY& oV);
LASS_DLL int pyGetSimpleObject(PyObject* iValue, prim::XYZ& oV);
LASS_DLL int pyGetSimpleObject(PyObject* iValue, prim::XYZW& oV);
LASS_DLL int pyGetSimpleObject(PyObject* iValue, prim::ColorRGBA& oV);
LASS_DLL int pyGetSimpleObject(PyObject* iValue, prim::IndexTriangle& oV);

}

}

#include "pyobject_util.inl"

#endif

// EOF
