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



/** @class lass::prim::Ray3D
 *  @brief 3D Ray
 *  @author Bram de Greve [BdG]
 *  @date 2003
 */



#ifndef LASS_GUARDIAN_OF_INCLUSION_PRIM_RAY_3D_H
#define LASS_GUARDIAN_OF_INCLUSION_PRIM_RAY_3D_H




#include "prim_common.h"
#include "normalizing_policy.h"
#include "parameter_policy.h"
#include "point_3d.h"



namespace lass
{

namespace prim
{

template
<
	typename T,
	class NormalizingPolicy = Normalized,
	class ParameterPolicy = Bounded
>
class Ray3D
{
public:

	typedef Ray3D<T, NormalizingPolicy, ParameterPolicy> TSelf;
	typedef NormalizingPolicy TNormalizingPolicy;
	typedef ParameterPolicy TParameterPolicy;

	typedef Point3D<T> TPoint;
	typedef typename TPoint::TVector TVector;

	typedef typename TPoint::TValue TValue;
	typedef typename TPoint::TParam TParam;
	typedef typename TPoint::TReference TReference;
	typedef typename TPoint::TConstReference TConstReference;
	typedef typename TPoint::TNumTraits TNumTraits;

	enum { dimension = TPoint::dimension }; /**< number of dimensions */

	template <typename U> struct Rebind
	{
		typedef Ray3D<U, NormalizingPolicy, ParameterPolicy> Type;
	};

	// STRUCTORS

	Ray3D();
	Ray3D(const TPoint& iSupport, const TVector& iDirection);
	Ray3D(const TPoint& iSupport, const TVector& iNormalizedDirection, IsAlreadyNormalized);
	Ray3D(const TPoint& iSupport, const TPoint& iLookAt);
	template <typename NP2, typename PP2> Ray3D(const Ray3D<T, NP2, PP2>& iOther);

	// METHODS

	const TPoint& support() const;
	TPoint& support();

	const TVector& direction() const;
	void setDirection(const TVector& iDirection);
	void lookAt(const TPoint& iLookAt);

	const TPoint point(TParam a_t) const;
	const TValue t(const TPoint& iPoint) const;

	const TVector project(const TVector& iVector) const;
	const TVector reject(const TVector& iVector) const;
	const TVector reflect(const TVector& iVector) const;
	const TPoint project(const TPoint& iPoint) const;
	const TVector reject(const TPoint& iPoint) const;
	const TPoint reflect(const TPoint& iPoint) const;

	const bool isValid() const;

private:

	TPoint support_;
	TVector direction_;
};



template<typename T, class NP, class PP>
std::ostream& operator<<(std::ostream& oOStream, const Ray3D<T, NP, PP>& iB);

template<typename T, class NP, class PP>
io::XmlOStream& operator<<(io::XmlOStream& oOStream, const Ray3D<T, NP, PP>& iB);



}

}

#include "ray_3d.inl"

#ifdef LASS_GUARDIAN_OF_INCLUSION_PRIM_AABB_3D_H
#	include "aabb_3d_ray_3d.h"
#endif

#ifdef LASS_GUARDIAN_OF_INCLUSION_PRIM_PARALLELOGRAM_3D_H
#	include "parallelogram_3d_ray_3d.h"
#endif

#ifdef LASS_GUARDIAN_OF_INCLUSION_PRIM_PLANE_3D_H
#	include "plane_3d_ray_3d.h"
#endif

#ifdef LASS_GUARDIAN_OF_INCLUSION_PRIM_SIMPLE_POLYGON_3D_H
#	include "ray_3d_simple_polygon_3d.h"
#endif

#ifdef LASS_GUARDIAN_OF_INCLUSION_PRIM_SPHERE_3D_H
#	include "ray_3d_sphere_3d.h"
#endif

#ifdef LASS_GUARDIAN_OF_INCLUSION_PRIM_TRANSFORMATION_3D_H
#	include "ray_3d_transformation_3d.h"
#endif

#ifdef LASS_GUARDIAN_OF_INCLUSION_PRIM_TRIANGLE_3D_H
#	include "ray_3d_triangle_3d.h"
#endif

#ifdef LASS_GUARDIAN_OF_INCLUSION_PRIM_CAPSULE_3D_H
#	include "capsule_3d_ray_3d.h"
#endif

#ifdef LASS_GUARDIAN_OF_INCLUSION_PRIM_LINE_SEGMENT_3D_H
#	include "line_segment_3d_ray_3d.h"
#endif

#endif