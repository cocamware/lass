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
template <typename T, class DegeneratePolicy > class SimplePolygon2D;
template <typename T, class PlaneEquationPolicy, class PlaneNormalizingPolicy > class SimplePolygon3D;
template <typename T, typename MMP> class Aabb2D;
template <typename T, typename MMP> class Aabb3D;
template <typename T, typename PP> class LineSegment2D;
template <typename T, typename PP> class LineSegment3D;
template <typename T> class Transformation2D;
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
template <typename T, class DegeneratePolicy > PyObject* pyBuildSimpleObject(const prim::SimplePolygon2D<T, DegeneratePolicy>& iV);
template <typename T, class PlaneEquationPolicy, class PlaneNormalizingPolicy > PyObject* pyBuildSimpleObject(const prim::SimplePolygon3D<T, PlaneEquationPolicy, PlaneNormalizingPolicy>& iV);
template <typename T, typename MMP> PyObject* pyBuildSimpleObject(const prim::Aabb2D<T, MMP>& iV);
template <typename T, typename MMP> PyObject* pyBuildSimpleObject(const prim::Aabb3D<T, MMP>& iV);
template <typename T, typename PP> PyObject* pyBuildSimpleObject(const prim::LineSegment2D<T, PP>& iV);
template <typename T, typename PP> PyObject* pyBuildSimpleObject(const prim::LineSegment3D<T, PP>& iV);
template <typename T> PyObject* pyBuildSimpleObject(const prim::Transformation2D<T>& iV);
template <typename T> PyObject* pyBuildSimpleObject(const prim::Transformation3D<T>& iV);

LASS_DLL PyObject* LASS_CALL pyBuildSimpleObject(const prim::XY& iV);
LASS_DLL PyObject* LASS_CALL pyBuildSimpleObject(const prim::XYZ& iV);
LASS_DLL PyObject* LASS_CALL pyBuildSimpleObject(const prim::XYZW& iV);
LASS_DLL PyObject* LASS_CALL pyBuildSimpleObject(const prim::ColorRGBA& iV);
LASS_DLL PyObject* LASS_CALL pyBuildSimpleObject(const prim::IndexTriangle& iV);

template <typename T> int pyGetSimpleObject(PyObject* iValue, prim::Vector2D<T>& oV);
template <typename T> int pyGetSimpleObject(PyObject* iValue, prim::Vector3D<T>& oV);
template <typename T> int pyGetSimpleObject(PyObject* iValue, prim::Vector4D<T>& oV);
template <typename T> int pyGetSimpleObject(PyObject* iValue, prim::Point2D<T>& oV);
template <typename T> int pyGetSimpleObject(PyObject* iValue, prim::Point3D<T>& oV);
template <typename T, class DegeneratePolicy > int pyGetSimpleObject(PyObject* iValue, prim::SimplePolygon2D<T, DegeneratePolicy>& oV);
template <typename T, class PlaneEquationPolicy, class PlaneNormalizingPolicy > int pyGetSimpleObject(PyObject* iValue, prim::SimplePolygon3D<T, PlaneEquationPolicy, PlaneNormalizingPolicy>& oV);
template <typename T, typename MMP> int pyGetSimpleObject(PyObject* iValue, prim::Aabb2D<T, MMP>& oV);
template <typename T, typename MMP> int pyGetSimpleObject(PyObject* iValue, prim::Aabb3D<T, MMP>& oV);
template <typename T, typename PP> int pyGetSimpleObject(PyObject* iValue, prim::LineSegment2D<T, PP>& oV);
template <typename T, typename PP> int pyGetSimpleObject(PyObject* iValue, prim::LineSegment3D<T, PP>& oV);
template <typename T> int pyGetSimpleObject(PyObject* iValue, prim::Transformation2D<T>& oV);
template <typename T> int pyGetSimpleObject(PyObject* iValue, prim::Transformation3D<T>& oV);

LASS_DLL int LASS_CALL pyGetSimpleObject(PyObject* iValue, prim::XY& oV);
LASS_DLL int LASS_CALL pyGetSimpleObject(PyObject* iValue, prim::XYZ& oV);
LASS_DLL int LASS_CALL pyGetSimpleObject(PyObject* iValue, prim::XYZW& oV);
LASS_DLL int LASS_CALL pyGetSimpleObject(PyObject* iValue, prim::ColorRGBA& oV);
LASS_DLL int LASS_CALL pyGetSimpleObject(PyObject* iValue, prim::IndexTriangle& oV);

}

}

#endif

#include "pyobject_util.inl" // allow repetitive includes ...

// EOF
