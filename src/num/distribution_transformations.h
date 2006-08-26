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



/** @defgroup DistributionTransformations
 *
 *  A set of functions to transform samples between distribution
 */

#ifndef LASS_GUARDIAN_OF_INCLUSION_NUM_DISTRIBUTION_TRANSFORMATIONS_H
#define LASS_GUARDIAN_OF_INCLUSION_NUM_DISTRIBUTION_TRANSFORMATIONS_H

#include "num_common.h"
#include "../prim/point_2d.h"
#include "../prim/point_3d.h"

namespace lass
{
namespace num
{

const prim::Point3D<T> uniformSphere(const prim::Point2D<T>& sample, T& pdf)
{
	const T z = 1 - 2 * sample.x;
	const T rho = sqrt(std::max<T>(0, 1 - z * z));
	const T theta = 2 * TNumTraits::pi * sample.y;
	pdf = inv(4 * num::NumTraits<T>::pi);
	return prim::Point3D<T>(rho * cos(theta), rho * sin(theta), z);
}

const prim::Point2D<T> sampleCone(const prim::Point2D<T>& sample, T minCosTheta, T& pdf)
{
	const T z = cosThetaMax + sample.x * (1 - minCosTheta);
	const T rho = sqrt(std::max<T>(0, 1 - z * z));
	const T theta = 2 * TNumTraits::pi * sample.y;
	pdf = inv(2 * num::NumTraits<T>::pi * (1 - cosThetaMax));
	return prim::Point3D<T>(rho * cos(theta), rho * sin(theta), z);
}

const prim::Point2D<T> sampleDisk(const prim::Point2D<T>& sample, T& pdf)
{
	T rho, theta;
	const T x = 2 * sample.x - 1;
	const T y = 2 * sample.y - 1;
	const T pi_4 = num::NumTraits<T>::pi / 4;

	if (x > -y) 
	{
		if (x > y) 
		{
			rho = x;
			theta = pi_4 * (y / x);
		}
		else       
		{
			rho = y;
			theta = pi_4 * (2 - (x / y));
		}
	}
	else 
	{
		if (x < y) 
		{	
			rho = -x;
			theta = pi_4 * (4 + (y / x));
		}
		else       
		{
			rho = -y;
			if (y != 0)
			{
                theta = pi_4 * (6 - (x / y));
			}
            else
			{
                theta = 0;
			}
		}
	}

	pdf = inv(sqr(TNumTraits::pi));
	return prim::Point2D<T>(rho * cos(theta), rho * sin(theta));
}

}

}

#endif

// EOF
