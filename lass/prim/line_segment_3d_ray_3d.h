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

#ifndef LASS_GUARDIAN_OF_INCLUSION_PRIM_LINE_SEGMENT_3D_RAY_3D_H
#define LASS_GUARDIAN_OF_INCLUSION_PRIM_LINE_SEGMENT_3D_RAY_3D_H

#include "prim_common.h"
#include "line_segment_3d.h"
#include "ray_3d.h"

namespace lass
{
namespace prim
{


template<typename T, class PP1, class NP2, class PP2>
T distance(
		const LineSegment3D<T, PP1>& lineSegment, const Ray3D<T, NP2, PP2>& ray, 
		const T& tMin = T())
{
	return num::sqrt(squaredDistance(lineSegment, ray, tMin));
}

template<typename T, class PP1, class NP2, class PP2>
T squaredDistance(
		const LineSegment3D<T, PP1>& lineSegment, const Ray3D<T, NP2, PP2>& ray, 
		const T& tMin = T())
{ 
	typedef typename LineSegment3D<T, PP1>::TValue TValue;
	typedef Point3D<TValue> TPoint;
	typedef Vector3D<TValue> TVector;

	const TPoint S1 = ray.support();
	const TVector D = ray.direction();
	const TPoint R = lineSegment.tail();
	const TVector E = lineSegment.vector();

	TVector SR = R - S1;
	const TVector N = cross(D,E);
	if(N.squaredNorm() == 0)
		return squaredDistance(ray.project(R), R);
	const TPoint S = S1 + N.project(SR);
	SR = R - S;

	TValue tRay, tSeg;

	// cramer
	if(num::abs(N.z) > num::abs(N.x) &&
	   num::abs(N.z) > num::abs(N.y))
	{
		tRay = (SR.x * E.y - SR.y * E.x) / N.z;
		tSeg = (SR.x * D.y - SR.y * D.x) / N.z;
	}
	else if(num::abs(N.x) > num::abs(N.y))
	{
		tRay = (SR.y * E.z - SR.z * E.y) / N.x;
		tSeg = (SR.y * D.z - SR.z * D.y) / N.x;
	}else
	{
		tRay = (SR.z * E.x - SR.x * E.z) / N.y;
		tSeg = (SR.z * D.x - SR.x * D.z) / N.y;	
	}

	if(tSeg > 1)
	{
		TValue tHead = ray.t(lineSegment.head());
		if(tHead > tMin)
			return squaredDistance(ray.point(tHead), lineSegment.head());
		else
		{
			tSeg = lineSegment.t(S1);
			if(tSeg < 0)
				return squaredDistance(S1, lineSegment.tail());
			if(tSeg > 1)
				return squaredDistance(S1, lineSegment.head());
			return squaredDistance(S1, lineSegment.point(tSeg));
		}
	}
	if(tSeg < 0)
	{
		TValue tTail = ray.t(R);
		if(tTail > tMin)
			return squaredDistance(ray.point(tTail), R);
		else
		{
			tSeg = lineSegment.t(S1);
			if(tSeg < 0)
                return squaredDistance(S1, lineSegment.tail());
			if(tSeg > 1)
				return squaredDistance(S1, lineSegment.head());
			return squaredDistance(S1, lineSegment.point(tSeg));
		}
	}

	if(tRay > tMin)
        return squaredDistance(ray.point(tRay), lineSegment.point(tSeg));
	
	return lineSegment.squaredDistance(S1);
}

template<typename T, class PP1, class NP2, class PP2>
T closestsPoints(
		const LineSegment3D<T, PP1>& lineSegment, const Ray3D<T, NP2, PP2>& ray,
		T &tSeg, T &tRay, const T& tMin = T())
{
	typedef typename LineSegment3D<T, PP1>::TValue TValue;
	typedef Point3D<TValue> TPoint;
	typedef Vector3D<TValue> TVector;

	const TPoint S1 = ray.support();
	const TVector D = ray.direction();
	const TPoint R = lineSegment.tail();
	const TVector E = lineSegment.vector();

	TVector SR = R - S1;
	const TVector N = cross(D,E);
	if(N.squaredNorm() == 0)
		return squaredDistance(ray.project(R), R);
	const TPoint S = S1 + N.project(SR);
	SR = R - S;

	// cramer
	if(num::abs(N.z) > num::abs(N.x) &&
	   num::abs(N.z) > num::abs(N.y))
	{
		tRay = (SR.x * E.y - SR.y * E.x) / N.z;
		tSeg = (SR.x * D.y - SR.y * D.x) / N.z;
	}
	else if(num::abs(N.x) > num::abs(N.y))
	{
		tRay = (SR.y * E.z - SR.z * E.y) / N.x;
		tSeg = (SR.y * D.z - SR.z * D.y) / N.x;
	}else
	{
		tRay = (SR.z * E.x - SR.x * E.z) / N.y;
		tSeg = (SR.z * D.x - SR.x * D.z) / N.y;	
	}

	if(tSeg > 1)
	{
		TValue tHead = ray.t(lineSegment.head());
		if(tHead > tMin)
		{
			tRay = tHead;
			tSeg = 1.0;
			return squaredDistance(ray.point(tHead), lineSegment.head());
		}
		else
		{
			tSeg = lineSegment.t(S1);
			tRay = 0.0;
			if(tSeg < 0)
			{
				tSeg = 0.0;
                return squaredDistance(S1, lineSegment.tail());
			}
			if(tSeg > 1)
			{
				tSeg = 1.0;
				return squaredDistance(S1, lineSegment.head());
			}
			return squaredDistance(S1, lineSegment.point(tSeg));
		}
	}
	if(tSeg < 0)
	{
		TValue tTail = ray.t(R);
		if(tTail > tMin)
		{
			tRay = tTail;
			return squaredDistance(ray.point(tTail), R);
		}
		else
		{
			tSeg = lineSegment.t(S1);
			tRay = 0.0;
			if(tSeg < 0)
			{
				tSeg = 0.0;
                return squaredDistance(S1, lineSegment.tail());
			}
			if(tSeg > 1)
			{
				tSeg = 1.0;
				return squaredDistance(S1, lineSegment.head());
			}
			return squaredDistance(S1, lineSegment.point(tSeg));
		}
	}

	if(tRay > tMin)
        return squaredDistance(ray.point(tRay), lineSegment.point(tSeg));
	
	tRay = 0.0;
	return lineSegment.closestsPoint(S1, tSeg);
}

}
}

#endif

// EOF
