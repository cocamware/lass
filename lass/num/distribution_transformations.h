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

/** @ingroup DistributionTransformations
 */
template <typename T>
const prim::Point3D<T> uniformSphere(const prim::Point2D<T>& sample, T& pdf)
{
	const T z = 1 - 2 * sample.x;
	const T rho = sqrt(std::max<T>(0, 1 - z * z));
	const T theta = 2 * num::NumTraits<T>::pi * sample.y;
	pdf = inv(4 * num::NumTraits<T>::pi);
	return prim::Point3D<T>(rho * cos(theta), rho * sin(theta), z);
}

/** @ingroup DistributionTransformations
 */
template <typename T>
const prim::Point3D<T> uniformHemisphere(const prim::Point2D<T>& sample, T& pdf)
{
	const T z = sample.x;
	const T rho = sqrt(std::max<T>(0, 1 - z * z));
	const T theta = 2 * num::NumTraits<T>::pi * sample.y;
	pdf = inv(2 * num::NumTraits<T>::pi);
	return prim::Point3D<T>(rho * cos(theta), rho * sin(theta), z);
}

/** @ingroup DistributionTransformations
 */
template <typename T>
const prim::Point3D<T> uniformCone(const prim::Point2D<T>& sample, T minCosTheta, T& pdf)
{
	const T z = minCosTheta + sample.x * (1 - minCosTheta);
	const T rho = sqrt(std::max<T>(0, 1 - z * z));
	const T theta = 2 * NumTraits<T>::pi * sample.y;
	pdf = inv(2 * NumTraits<T>::pi * (1 - minCosTheta));
	return prim::Point3D<T>(rho * cos(theta), rho * sin(theta), z);
}

/** @ingroup DistributionTransformations
 */
template <typename T>
const prim::Point2D<T> uniformDisk(const prim::Point2D<T>& sample, T& pdf)
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

	pdf = inv(NumTraits<T>::pi);
	return prim::Point2D<T>(rho * cos(theta), rho * sin(theta));
}

/** @ingroup DistributionTransformations
 */
template <typename T>
const prim::Point3D<T> cosineHemisphere(const prim::Point2D<T>& sample, T& pdf)
{
	const prim::Point2D<T> xy = uniformDisk(sample, pdf);
	const T z = num::sqrt(std::max(T(), 1 - xy.position().squaredNorm()));
	pdf *= z;
	return prim::Point3D<T>(xy.x, xy.y, z);
}

}

}

#endif

// EOF
