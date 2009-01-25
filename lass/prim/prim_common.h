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



/** @namespace lass::prim
 *  @brief set of geometrical primitives
 *  @author Bram de Greve [BdG]
 *  @date 2003
 *
 *  lass::prim is the library for geometrical primitives and related constructions.  It
 *  consists of structures and classes to represent primitives as vectors, points, lines,
 *  planes, polygons etc., some policy classes that can modify the implementation of some of
 *  these primitives, and a few related enumerations.
 *
 *  All primitives are templated with an underlying value type (think of floats, doubles,
 *  fixed point, ...).  This value type can be set as by the template parameter @a T, and is
 *  typedef'ed in all primitives as @c TValue (convenient, isn't it? :).  It's the subatomic unit
 *  (proton if you like) on which more complex types are built.
 *
 *  @section overview
 *
 *  I think the best way to explore this library, is to give an overview of its components.
 *  Here we go ...
 *
 *  @subsection primitives
 *
 *  If @c TValue is the proton, then vectors are the smallest atomic units in lass::prim you
 *  can build.  Vectors represent a direction, a translation, and we have three of them
 *  in our library (with 2, 3 and 4 protons respective :):
 *
 *  - Vector2D: a two dimensional vector.
 *  - Vector3D: a three dimensional vector.
 *  - Vector4D: a four dimensional vector.
 *
 *  The above vectors are free vectors and have no position.  To express the position of
 *  a location in space, we need points.  we have two of these thingies.  It's a little
 *  unusual to distinguish between vectors and points.  Usually, only one is used for both
 *  concepts, whether they uses @e point or @e vector.  Yet, these are totally different
 *  mathematical entities.  You can add two vectors together, but you can't add two points
 *  (what would it mean?).  Transforming a vector is different that a point (a vector will
 *  only be rotated, a point will also be translated).  Points and vectors are clearly to
 *  be traited differently.  This cries for different classes or structures.  In C however, it has
 *  little use to do that, because of the lack of function overloading and strong type checking.
 *  However, here we are in C++ and the good news is: C++ does have overloading and strong
 *  type checking, so we can clearly distinguish between points and vectors in our code.
 *  And that's what we do here.  For more info on this subject, I gladly refer to [1, 2, 3].
 *  You can see the points as ions: vectors with some extra (or lacking) electrons:
 *  same number of protons (same dimension), but not completely the same thing.
 *
 *  - Point2D: a two dimensional point.
 *  - Point3D: a three dimensional point.
 *
 *  One problem of not being able to add and scale points is not being able to use barycentric
 *  combinations.  This is a linear combination of points in which the sum of all weight is
 *  equal to 1.  The result of such a combination is a point and is mathematical sensefull.
 *  In [1], I didn't find a satisfying solution to this problem, because I wanted to limit
 *  myself to eucledian entities.  To do barycentric combinations, I had to convert the points
 *  to vectors, do the combination, and the convert back to a point.  Not only this is a bit
 *  clumsy, it didn't guarantee that the sum of the weights would be 1.  In [2], I introduced
 *  homogenous points to solve this question.  And they are also introduced in here.
 *  Homogenous points are with the sole capability to do barycentric combinations on points.
 *  You can see the homogenous points as points with some extra neutrons: pretty compatible.
 *
 *  - Point2DH: a homogenous representation of a two dimensional point.
 *  - Point3DH: a homogenous representation of a three dimensional point.
 *
 *  So far for the essential building blocks of the more complex primitives.  Until now we had
 *  the atoms, now we'll build moleculs.  The simplest thing we can construct from it are
 *  axis-aligned-bounding-boxes.
 *
 *  - Aabb2D: a two dimensional axis aligned boundig box ... or a rectangle :)
 *  - Aabb3D: a three dimensional axis aligned bounding box.
 *
 *  Objects of infinite size are lines and planes.  We have them in parametric and cartesian
 *  versions.
 *
 *  - Line2D: a two dimensional line with different implementation policies, or a half plane.
 *  - Line3D: a three dimensional parametric line.
 *  - Plane3D: a three dimensional plane with different implementation policies, or a half space.
 *
 *  For the circular guys:
 *
 *  - Sphere3D: a sphere!
 *
 *  Objects of not so infinite size: line segments and rays.
 *
 *  - LineSegment2D: a two dimensional line segment
 *  - LineSegment3D: a three dimensional line segment
 *  - Ray2D: a two dimensional ray (or a half line)
 *  - Ray3D: a three dimensional ray (or a half line in 3D)
 *
 *  Polygons and stuff:
 *
 *  - SimplePolygon2D: a two dimensional convex/concave polygon
 *  - SimplePolygon3D: a three dimensional convex/concave polygon
 *  - Triangle2D: a two dimensional polygon with three vertices
 *  - Triangle3D: a three dimensional polygon with three vertices
 *
 *  @subsection policies
 *
 *  - @ref NormalizingPolicy: Normalized, Unnormalized
 *  - @ref ParameterPolicy: Bounded, Unbounded
 *  - @ref MinMaxPolicy: StrictMinMax, AutoMinMax
 *  - @ref EquationPolicy: Cartesian, Parametric, Combined
 *
 *  @subsection enumerations
 *
 *  - @ref Result
 *  - @ref Side
 *  - @ref Orientation
 *
 *  @subsection colors
 *
 *  - ColorRGBA: a four dimensional color with floating point channels red, green, blue and alpha.
 *
 *  @subsection iterators
 *
 *  These three classes are iterators over the different axes (components).  You can use
 *  this instead of directly calling the @c .x or @c .y members of a vector.  With
 *  these iterators, you can access those components through the @c operator[] .
 *
 *  - XY
 *  - XYZ
 *  - XYZW
 */




/** @namespace lass::prim::impl
 *  @brief implementation details of lass::prim
 *  @author BdG
 *  @date 2003
 *
 *  @warning nothing for the faint hearted :)
 */



#ifndef LASS_GUARDIAN_OF_INCLUSION_PRIM_PRIM_COMMON_H
#define LASS_GUARDIAN_OF_INCLUSION_PRIM_PRIM_COMMON_H



#include "../lass_common.h"

#include "../num/num_traits.h"
#include "../util/call_traits.h"

#include "result.h"
#include "side.h"

namespace lass
{
namespace prim
{

class SingularityError: public util::Exception
{
public:
	SingularityError(const std::string& msg, const std::string& loc): util::Exception(msg, loc) {}
private:
	LASS_UTIL_EXCEPTION_PRIVATE_IMPL(SingularityError)
};

}
}

#endif
