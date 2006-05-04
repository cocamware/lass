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

#ifndef LASS_GUARDIAN_OF_INCLUSION_PRIM_LINE_SEGMENT_3D_RAY_3D_INL
#define LASS_GUARDIAN_OF_INCLUSION_PRIM_LINE_SEGMENT_3D_RAY_3D_INL

#include "line_segment_3d_ray_3d.h"

namespace lass
{
namespace prim
{

template<typename T, class PP1, class NP2, class PP2>
T distance(const LineSegment3D<T, PP1>& iLineSegment, 
		   const Ray3D<T, NP2, PP2>& iRay, 
		   const T& iMinT)
{
	return num::sqrt(squaredDistance(iLineSegment, iRay, iMinT));
}

template<typename T, class PP1, class NP2, class PP2>
T squaredDistance(const LineSegment3D<T, PP1>& iLineSegment, 
 				  const Ray3D<T, NP2, PP2>& iRay, 
				  const T& iMinT) 
{ 
	typedef typename LineSegment3D<T, PP1>::TValue TValue;
	typedef Point3D<TValue> TPoint;
	typedef Vector3D<TValue> TVector;

	const TPoint S1 = iRay.support();
	const TVector D = iRay.direction();
	const TPoint R = iLineSegment.tail();
	const TVector E = iLineSegment.vector();

	TVector SR = R - S1;
	const TVector N = cross(D,E);
	if(N.squaredNorm() == 0)
		return squaredDistance(iRay.project(R), R);
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
		TValue tHead = iRay.t(iLineSegment.head());
		if(tHead > iMinT)
			return squaredDistance(iRay.point(tHead), iLineSegment.head());
		else
		{
			tSeg = iLineSegment.t(S1);
			if(tSeg < 0)
                return squaredDistance(S1, iLineSegment.tail());
			if(tSeg > 1)
				return squaredDistance(S1, iLineSegment.head());
			return squaredDistance(S1, iLineSegment.point(tSeg));
		}
	}
	if(tSeg < 0)
	{
		TValue tTail = iRay.t(R);
		if(tTail > iMinT)
			return squaredDistance(iRay.point(tTail), R);
		else
		{
			tSeg = iLineSegment.t(S1);
			if(tSeg < 0)
                return squaredDistance(S1, iLineSegment.tail());
			if(tSeg > 1)
				return squaredDistance(S1, iLineSegment.head());
			return squaredDistance(S1, iLineSegment.point(tSeg));
		}
	}

	if(tRay > iMinT)
        return squaredDistance(iRay.point(tRay), iLineSegment.point(tSeg));
	
	return iLineSegment.squaredDistance(S1);
}

template<typename T, class PP1, class NP2, class PP2>
T closestsPoints(const LineSegment3D<T, PP1>& iLineSegment,
						  const Ray3D<T, NP2, PP2>& iRay,
						  T &tSeg,
						  T &tRay,
						  const T& iMinT)
{
	typedef typename LineSegment3D<T, PP1>::TValue TValue;
	typedef Point3D<TValue> TPoint;
	typedef Vector3D<TValue> TVector;

	const TPoint S1 = iRay.support();
	const TVector D = iRay.direction();
	const TPoint R = iLineSegment.tail();
	const TVector E = iLineSegment.vector();

	TVector SR = R - S1;
	const TVector N = cross(D,E);
	if(N.squaredNorm() == 0)
		return squaredDistance(iRay.project(R), R);
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
		TValue tHead = iRay.t(iLineSegment.head());
		if(tHead > iMinT)
		{
			tRay = tHead;
			tSeg = 1.0;
			return squaredDistance(iRay.point(tHead), iLineSegment.head());
		}
		else
		{
			tSeg = iLineSegment.t(S1);
			tRay = 0.0;
			if(tSeg < 0)
			{
				tSeg = 0.0;
                return squaredDistance(S1, iLineSegment.tail());
			}
			if(tSeg > 1)
			{
				tSeg = 1.0;
				return squaredDistance(S1, iLineSegment.head());
			}
			return squaredDistance(S1, iLineSegment.point(tSeg));
		}
	}
	if(tSeg < 0)
	{
		TValue tTail = iRay.t(R);
		if(tTail > iMinT)
		{
			tRay = tTail;
			return squaredDistance(iRay.point(tTail), R);
		}
		else
		{
			tSeg = iLineSegment.t(S1);
			tRay = 0.0;
			if(tSeg < 0)
			{
				tSeg = 0.0;
                return squaredDistance(S1, iLineSegment.tail());
			}
			if(tSeg > 1)
			{
				tSeg = 1.0;
				return squaredDistance(S1, iLineSegment.head());
			}
			return squaredDistance(S1, iLineSegment.point(tSeg));
		}
	}

	if(tRay > iMinT)
        return squaredDistance(iRay.point(tRay), iLineSegment.point(tSeg));
	
	tRay = 0.0;
	return iLineSegment.closestsPoint(S1, tSeg);
}
}
}


#endif

// EOF
