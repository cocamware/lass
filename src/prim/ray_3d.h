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

#ifdef LASS_GUARDIAN_OF_INCLUSION_PRIM_LINE_SEGMENT_3D_H
#	include "line_segment_3d_ray_3d.h"
#endif

#endif

// EOF
