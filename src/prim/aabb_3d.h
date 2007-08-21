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



/** @class lass::prim::Aabb3D
 *  @brief your momma's axis aligned bounding box.
 *  @author Bram de Greve [BdG]
 *  @date 2003
 *
 *  An AABB (Axis Aligned Bounding Box) is a rectangular box of a given dimension
 *  (in this case a 3D box), that is often used as a simple bounding volume of another
 *  primitive or data structure.
 *
 *  <i>"A form of a bounding box where the box is aligned to the axis therefore only
 *  two points in space are needed to define it. AABB's are much faster to use, and
 *  take up less memory, but are very limited in the sense that they can only be
 *  aligned to the axis."</i>, http://www.gamedev.net/dict/term.asp?TermID=525
 *
 *  The way an AABB handles its minima and maxima can be set by the @c MinMaxPolicy.
 *  On policy StrictPolicy will enforce you to use correct minima and maxima, and on
 *  any suspicious behaviour, it will throw an exception.  The other policy AutoPolicy
 *  will try to correct misbehaviour without your notice.  For more information on
 *  these policies, I refer to the documentation compagning these policies.
 */



#ifndef LASS_GUARDIAN_OF_INCLUSION_PRIM_AABB_3D_H
#define LASS_GUARDIAN_OF_INCLUSION_PRIM_AABB_3D_H



#include "min_max_policy.h"
#include "point_3dh.h"


namespace lass
{

namespace prim
{

template
<
	typename T,
	class MinMaxPolicy = StrictMinMax
>
class Aabb3D
{
public:

	typedef Aabb3D<T, MinMaxPolicy> TSelf;
	typedef MinMaxPolicy TMinMaxPolicy;

	typedef Point3D<T> TPoint;
	typedef Point3DH<T> TPointH;
	typedef typename TPoint::TVector TVector;

	typedef typename TPoint::TValue TValue;
	typedef typename TPoint::TParam TParam;
	typedef typename TPoint::TReference TReference;
	typedef typename TPoint::TConstReference TConstReference;
	typedef typename TPoint::TNumTraits TNumTraits;

	enum { dimension = TPoint::dimension }; /**< number of dimensions of vector */

	template <typename U> struct Rebind
	{
		typedef Aabb3D<U, MinMaxPolicy> Type;
	};

	Aabb3D();
	Aabb3D(const TPoint& iMin, const TPoint& iMax);
	explicit Aabb3D(const TPoint& iPoint);
	template <class MMP2> Aabb3D(const Aabb3D<T, MMP2>& iOther);

	const TPoint& min() const;
	const TPoint& max() const;
	void setMin(const TPoint& iMin);
	void setMax(const TPoint& iMax);

	template <class MMP2> TSelf& operator=(const Aabb3D<T, MMP2>& iOther);

	TSelf& operator+=(const TPoint& iPoint);
	template<class MMP2> TSelf& operator+=(const Aabb3D<T, MMP2>& iOther);
	void grow(TParam iDistance);
	void scale(TParam iScale);

	const TPointH center() const;
	const TVector size() const;
	const TValue area() const;
	const TValue volume() const;

	const Side classify(const TPoint& iPoint) const;
	const bool contains(const TPoint& iPoint) const;
	template <class MMP2> const bool contains(const Aabb3D<T, MMP2>& iOther) const;
	template <class MMP2> const bool intersects(const Aabb3D<T, MMP2>& iOther) const;
	template <class MMP2> const bool collides(const Aabb3D<T, MMP2>& iOther) const;

	template <class RandomGenerator> const TPoint random(RandomGenerator& ioRandom) const;

	void clear();
	const bool isEmpty() const;
	const bool isValid() const;

	template <typename MMP2> void swap(Aabb3D<T, MMP2>& iOther);

private:

	TPoint min_;
	TPoint max_;
};

template <typename T, class MMPa, class MMPb>
const Aabb3D<T, MMPa> operator+(const Aabb3D<T, MMPa>& iA, const Aabb3D<T, MMPb>& iB);

template <typename T, class MMP>
const Aabb3D<T, MMP> operator+(const Aabb3D<T, MMP>& iA, const Point3D<T>& iB);

template <typename T, class MMP>
const Aabb3D<T, MMP> operator+(const Point3D<T>& iA, const Aabb3D<T, MMP>& iB);

template <typename T, class MMP>
T distance(const Aabb3D<T, MMP>& iA, const Point3D<T>& iB);

template <typename T, class MMPa, class MMPb>
T distance(const Aabb3D<T, MMPa>& iA, const Aabb3D<T, MMPb>& iB);

template <typename T, class MMPa, class MMPb, class MMPr>
Result intersect(const Aabb3D<T, MMPa>& iA, const Aabb3D<T, MMPb>& iB, Aabb3D<T, MMPr>& oResult);

template <typename T, class MMP>
std::ostream& operator<<(std::ostream& ioOStream, const Aabb3D<T, MMP>& iAabb);

template <typename T, class MMP>
io::XmlOStream& operator<<(io::XmlOStream& ioOStream, const Aabb3D<T, MMP>& iAabb);

}

}

#include "aabb_3d.inl"
#include "pyobject_util.inl"

#define LASS_PRIM_PYOBJECT_UTIL_AABB_3D
#ifdef LASS_GUARDIAN_OF_INCLUSION_PRIM_PYOBJECT_UTIL_H
#	include "pyobject_util.h"
#endif

#ifdef LASS_GUARDIAN_OF_INCLUSION_PRIM_PARALLELOGRAM_3D_H
#	include "aabb_3d_parallelogram_3d.h"
#endif

#ifdef LASS_GUARDIAN_OF_INCLUSION_PRIM_RAY_3D_H
#	include "aabb_3d_ray_3d.h"
#endif

#ifdef LASS_GUARDIAN_OF_INCLUSION_PRIM_SIMPLE_POLYGON_3D_H
#	include "aabb_3d_simple_polygon_3d.h"
#endif

#ifdef LASS_GUARDIAN_OF_INCLUSION_PRIM_SPHERE_3D_H
#	include "aabb_3d_sphere_3d.h"
#endif

#ifdef LASS_GUARDIAN_OF_INCLUSION_PRIM_TRIANGLE_3D_H
#	include "aabb_3d_triangle_3d.h"
#endif

#ifdef LASS_GUARDIAN_OF_INCLUSION_PRIM_TRANSFORMATION_3D_H
#	include "aabb_3d_transformation_3d.h"
#endif

#endif

// EOF

