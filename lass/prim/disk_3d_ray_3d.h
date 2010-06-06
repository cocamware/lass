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
 *	Copyright (C) 2004-2010 the Initial Developer.
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

#ifndef LASS_GUARDIAN_OF_INCLUSION_PRIM_PARALLELOGRAM_3D_RAY_3D_H
#define LASS_GUARDIAN_OF_INCLUSION_PRIM_PARALLELOGRAM_3D_RAY_3D_H

#include "prim_common.h"
#include "disk_3d.h"
#include "ray_3d.h"
#include "../num/floating_point_consistency.h"

namespace lass
{
namespace prim
{

/** @relates lass::prim::Ray3D
 *  @relates lass::prim::Parallelogram3D
 */
template <typename T, class NP, class PP>
Result intersect(const Disk3D<T>& disk, const Ray3D<T, NP, PP>& ray, T& u, T& v, T& t, const T& tMin = T())
{
	typedef typename Disk3D<T>::TPlane TPlane;
	typedef typename Disk3D<T>::TVector TVector;
	typedef typename Disk3D<T>::TNumTraits TNumTraits;

	T tCandidate;
	const TPlane& plane = disk.plane();
	const Result result = intersect(plane, ray, tCandidate, tMin);
	if (result != rOne)
	{
		return result;
	}

	const TVector delta = ray.point(tCandidate) - disk.center();
	const T sqrDist = delta.squaredNorm();
	if (sqrDist > num::sqr(disk.radius()))
	{
		return rNone;
	}

	const T x = dot(delta, plane.directionU());
	const T y = dot(delta, plane.directionV());
	
	t = tCandidate;
	u = num::sqrt(sqrDist) / disk.radius();
	v = num::atan2(y, x) / (2 * TNumTraits::pi);
	if (v < 0)
	{
		v += TNumTraits::one;
	}
	return rOne;
}

/** @relates lass::prim::Ray3D
 *  @relates lass::prim::Parallelogram3D
 */
template <typename T, class NP, class PP>
Result intersect(const Disk3D<T>& disk, const Ray3D<T, NP, PP>& ray, T& t, const T& tMin = T())
{
	T u;
	T v;
	return intersect(disk, ray, u, v, t, tMin);
}

}

}

#endif

// EOF
