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

#ifndef LASS_GUARDIAN_OF_INCLUSION_PRIM_RAY_3D_TRANSFORMATION_3D_H
#define LASS_GUARDIAN_OF_INCLUSION_PRIM_RAY_3D_TRANSFORMATION_3D_H

#include "prim_common.h"
#include "ray_3d.h"
#include "transformation_3d.h"

namespace lass
{
namespace prim
{

namespace impl
{
	template <typename NP> struct RayParameterRescaler;

	template <>
	struct RayParameterRescaler<prim::Unnormalized>
	{
		// unnormalized directions don't alter parameter: same t is same point
		template <typename T, typename VectorType> 
		static void rescale(T& /*tRay*/, const VectorType& /*unnormalizedNewDirection*/) 
		{ 
		}
	};

	template <>
	struct RayParameterRescaler<prim::Normalized>
	{
		// normalized ray's compress parameters because of renormalisation of direction.
		// to get same point, scale parameter by norm before renormalisation.
		template <typename T, typename VectorType> 
		static void rescale(T& tRay, const VectorType& unnormalizedNewDirection) 
		{
			tRay *= unnormalizedNewDirection.norm();
		}
	};
}



/** apply transformation to ray
 *  @relates Transformation3D
 */
template<typename T, class NP, class PP>
Ray3D<T, NP, PP> transform(
		const Ray3D<T, NP, PP>& subject, const Transformation3D<T>& transformation)
{
	return Ray3D<T, NP, PP>(
		transform(subject.support(), transformation),
		transform(subject.direction(), transformation));
}



/** apply transformation to ray, and rescale a parameter to represent same point
 *
 *  @param tRay
 *		- INPUT & OUTPUT
 *		- if direction gets renormalized, then a point on ray is no longer represented
 *		  by same parameter.  Adjust parameter @a tRay to fix this.
 *		- If you fill in 1 as @a tRay, you can use the result to
 *		  rescale multiple parameters by multiplying it.
 *		- <tt>transform(ray, transf).point(t') == transform(ray.point(t), transf)</tt>
 *
 *  @relates Transformation3D
 */
template<typename T, class NP, class PP>
Ray3D<T, NP, PP> transform(
		const Ray3D<T, NP, PP>& subject, const Transformation3D<T>& transformation, T& tRay)
{
	typedef Ray3D<T, NP, PP> TRay;
	const typename TRay::TPoint support = transform(subject.support(), transformation);
	const typename TRay::TVector direction = transform(subject.direction(), transformation);
	impl::RayParameterRescaler<NP>::rescale(tRay, direction);
	return Ray3D<T, NP, PP>(support, direction);;
}

}
}

#endif

// EOF
