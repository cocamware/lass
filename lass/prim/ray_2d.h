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
 *	Copyright (C) 2004-2011 the Initial Developer.
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


/** @class lass::prim::Ray2D
 *  @brief 2D Ray
 *  @author Bram de Greve [BdG]
 *  @date 2003
 */



#ifndef LASS_GUARDIAN_OF_INCLUSION_PRIM_RAY_2D_H
#define LASS_GUARDIAN_OF_INCLUSION_PRIM_RAY_2D_H




#include "prim_common.h"
#include "normalizing_policy.h"
#include "parameter_policy.h"
#include "point_2d.h"



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
class LASS_SIMD_ALIGN Ray2D
{
public:

	typedef Ray2D<T, NormalizingPolicy, ParameterPolicy> TSelf;
	typedef NormalizingPolicy TNormalizingPolicy;
	typedef ParameterPolicy TParameterPolicy;

	typedef Point2D<T> TPoint;
	typedef typename TPoint::TVector TVector;

	typedef typename TPoint::TValue TValue;
	typedef typename TPoint::TParam TParam;
	typedef typename TPoint::TReference TReference;
	typedef typename TPoint::TConstReference TConstReference;
	typedef typename TPoint::TNumTraits TNumTraits;

	enum { dimension = TPoint::dimension }; /**< number of dimensions */

	template <typename U> struct Rebind
	{
		typedef Ray2D<U, NormalizingPolicy, ParameterPolicy> Type;
	};

	// STRUCTORS

	Ray2D();
	Ray2D(const TPoint& iSupport, const TVector& iDirection);
	Ray2D(const TPoint& iSupport, const TPoint& iLookAt);

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

	Side classify(const TPoint& iPoint) const;

	bool isValid() const;

private:

	TPoint support_;
	TVector direction_;
};



template<typename T, class NP, class PP>
std::ostream& operator<<(std::ostream& oOStream, const Ray2D<T, NP, PP>& iB);

template<typename T, class NP, class PP>
io::XmlOStream& operator<<(io::XmlOStream& oOStream, const Ray2D<T, NP, PP>& iB);



}

}

#include "ray_2d.inl"

#ifdef LASS_GUARDIAN_OF_INCLUSION_PRIM_AABB_2D_H
#	include "aabb_2d_ray_2d.h"
#endif

#ifdef LASS_GUARDIAN_OF_INCLUSION_PRIM_LINE_2D_H
#	include "line_2d_ray_2d.h"
#endif

#ifdef LASS_GUARDIAN_OF_INCLUSION_PRIM_LINE_SEGMENT_2D_H
#	include "line_segment_2d_ray_2d.h"
#endif

#ifdef LASS_GUARDIAN_OF_INCLUSION_PRIM_SIMPLE_POLYGON_2D_H
#	include "ray_2d_simple_polygon_2d.h"
#endif

#ifdef LASS_GUARDIAN_OF_INCLUSION_PRIM_TRIANGLE_2D_H
#	include "ray_2d_triangle_2d.h"
#endif

#endif
