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
