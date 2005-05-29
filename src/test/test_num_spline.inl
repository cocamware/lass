/** @file
 *  @internal
 *  @author Bram de Greve (bramz@users.sourceforge.net)
 *  @author Tom De Muer (tomdemuer@users.sourceforge.net)
 *
 *  Distributed under the terms of the GPL (GNU Public License)
 *
 *  The LASS License:
 *
 *  Copyright 2004 Bram de Greve and Tom De Muer
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



#ifndef LASS_GUARDIAN_OF_INCLUSION_TEST_TEST_NUM_SPLINE_INL
#define LASS_GUARDIAN_OF_INCLUSION_TEST_TEST_NUM_SPLINE_INL

#include "test_common.h"

#include "../num/spline_linear.h"
#include "../num/spline_cubic.h"
#include "../prim/vector_3d.h"

namespace lass
{
namespace test
{
/*
template 
<
	typename ScalarType, 
	typename DataType
>
struct FixedDimensionDataTraits
{
	static size_t dimension(const DataType& iData) { return DataType::dimension; }
	static void zero(DataType& ioData, size_t iDimension) { ioData = DataType(); }
	static ScalarType get(const DataType& iData, size_t iIndex) { return iData[iIndex]; }
	static void set(DataType& ioData, size_t iIndex, ScalarType iValue) { ioData[iIndex] = iValue; }
	static void scale(DataType& ioAcc, ScalarType iScale) {	ioAcc *= iScale; }
	static void multiplyAccumulate(DataType& ioAcc, const DataType& iData, ScalarType iScale) {	ioAcc += iData * iScale; }
};
*/

void testNumSpline()
{
	typedef float TScalar;
	typedef prim::Vector3D<float> TVector3D;

	/*
	const TScalar xx[] = { -13.f, -5.6f, -5.5f, 2.f, 3.14f, 7.34f,   9.f, 10.7f };
	const TScalar yy[] = {  4.8f, -34.f, -34.f, 0.f,  6.5f,  0.5f, -20.f, -15.f };
	/*/
	const TScalar w[] =
	{
		3.70e-007,
		3.75e-007,
		3.80e-007,
		3.85e-007,
		3.90e-007,
		3.95e-007,
		4.00e-007,
		4.05e-007,
		4.10e-007,
		4.15e-007,
		4.20e-007,
		4.25e-007,
		4.30e-007,
		4.35e-007,
		4.40e-007,
		4.45e-007,
		4.50e-007,
		4.55e-007,
		4.60e-007,
		4.65e-007,
		4.70e-007,
		4.75e-007,
		4.80e-007,
		4.85e-007,
		4.90e-007,
		4.95e-007,
		5.00e-007,
		5.05e-007,
		5.10e-007,
		5.15e-007,
		5.20e-007,
		5.25e-007,
		5.30e-007,
		5.35e-007,
		5.40e-007,
		5.45e-007,
		5.50e-007,
		5.55e-007,
		5.60e-007,
		5.65e-007,
		5.70e-007,
		5.75e-007,
		5.80e-007,
		5.85e-007,
		5.90e-007,
		5.95e-007,
		6.00e-007,
		6.05e-007,
		6.10e-007,
		6.15e-007,
		6.20e-007,
		6.25e-007,
		6.30e-007,
		6.35e-007,
		6.40e-007,
		6.45e-007,
		6.50e-007,
		6.55e-007,
		6.60e-007,
		6.65e-007,
		6.70e-007,
		6.75e-007,
		6.80e-007,
		6.85e-007,
		6.90e-007,
		6.95e-007,
		7.00e-007,
		7.05e-007,
		7.10e-007,
		7.15e-007,
		7.20e-007,
		7.25e-007,
		7.30e-007,
		7.35e-007,
		7.40e-007,
		7.45e-007,
		7.50e-007,
		7.55e-007,
		7.60e-007,
		7.65e-007,
		7.70e-007,
		7.75e-007,
		7.80e-007,
	};
	const TVector3D xyz[] =
	{
		TVector3D(0.0000e+000, 0.0000e+000, 0.0000e+000),
		TVector3D(0.0000e+000, 0.0000e+000, 0.0000e+000),
		TVector3D(1.7146e+003, 0.0000e+000, 5.9997e+003),
		TVector3D(6.0010e+003, 8.5718e+002, 2.4856e+004),
		TVector3D(2.0575e+004, 2.5715e+003, 8.9996e+004),
		TVector3D(6.1725e+004, 6.8574e+003, 2.7684e+005),
		TVector3D(1.6374e+005, 1.7144e+004, 7.3711e+005),
		TVector3D(3.7206e+005, 3.8573e+004, 1.6894e+006),
		TVector3D(7.2612e+005, 7.5432e+004, 3.3376e+006),
		TVector3D(1.2053e+006, 1.2429e+005, 5.6295e+006),
		TVector3D(1.7532e+006, 1.8344e+005, 8.3353e+006),
		TVector3D(2.2692e+006, 2.5287e+005, 1.0992e+007),
		TVector3D(2.6979e+006, 3.3173e+005, 1.3315e+007),
		TVector3D(3.0665e+006, 4.2516e+005, 1.5415e+007),
		TVector3D(3.2894e+006, 5.3231e+005, 1.6862e+007),
		TVector3D(3.3151e+006, 6.4031e+005, 1.7376e+007),
		TVector3D(3.1780e+006, 7.6717e+005, 1.7098e+007),
		TVector3D(2.9405e+006, 9.1118e+005, 1.6291e+007),
		TVector3D(2.5916e+006, 1.0989e+006, 1.4960e+007),
		TVector3D(2.1784e+006, 1.3098e+006, 1.3327e+007),
		TVector3D(1.6769e+006, 1.5875e+006, 1.1293e+007),
		TVector3D(1.1342e+006, 1.8849e+006, 8.8299e+006),
		TVector3D(6.9012e+005, 2.1738e+006, 6.6177e+006),
		TVector3D(3.5235e+005, 2.5518e+006, 4.8863e+006),
		TVector3D(1.3888e+005, 2.9067e+006, 3.5596e+006),
		TVector3D(4.3722e+004, 3.3893e+006, 2.5919e+006),
		TVector3D(3.2577e+004, 3.9499e+006, 1.8728e+006),
		TVector3D(1.3202e+005, 4.5550e+006, 1.3645e+006),
		TVector3D(3.2148e+005, 5.2005e+006, 9.5996e+005),
		TVector3D(6.1210e+005, 5.8777e+006, 7.0454e+005),
		TVector3D(1.0090e+006, 6.5300e+006, 5.2026e+005),
		TVector3D(1.4831e+006, 7.0571e+006, 3.6941e+005),
		TVector3D(2.0275e+006, 7.5020e+006, 2.6142e+005),
		TVector3D(2.6079e+006, 7.9186e+006, 1.7656e+005),
		TVector3D(3.2303e+006, 8.2460e+006, 1.1742e+005),
		TVector3D(3.8715e+006, 8.4192e+006, 6.7711e+004),
		TVector3D(4.5419e+006, 8.5015e+006, 3.4284e+004),
		TVector3D(5.2817e+006, 8.5641e+006, 9.4281e+003),
		TVector3D(6.0456e+006, 8.5486e+006, 0.0000e+000),
		TVector3D(6.8051e+006, 8.4209e+006, 0.0000e+000),
		TVector3D(7.5330e+006, 8.1912e+006, 0.0000e+000),
		TVector3D(8.1545e+006, 7.8449e+006, 0.0000e+000),
		TVector3D(8.6946e+006, 7.4480e+006, 0.0000e+000),
		TVector3D(9.2098e+006, 7.0769e+006, 0.0000e+000),
		TVector3D(9.5888e+006, 6.6637e+006, 0.0000e+000),
		TVector3D(9.7242e+006, 6.1751e+006, 0.0000e+000),
		TVector3D(9.6359e+006, 5.6428e+006, 0.0000e+000),
		TVector3D(9.3367e+006, 5.0908e+006, 0.0000e+000),
		TVector3D(8.8343e+006, 4.5259e+006, 0.0000e+000),
		TVector3D(8.1502e+006, 3.9584e+006, 0.0000e+000),
		TVector3D(7.3410e+006, 3.4124e+006, 0.0000e+000),
		TVector3D(6.4717e+006, 2.9110e+006, 0.0000e+000),
		TVector3D(5.5509e+006, 2.4301e+006, 0.0000e+000),
		TVector3D(4.5873e+006, 1.9569e+006, 0.0000e+000),
		TVector3D(3.7001e+006, 1.5412e+006, 0.0000e+000),
		TVector3D(2.9465e+006, 1.2018e+006, 0.0000e+000),
		TVector3D(2.3001e+006, 9.2232e+005, 0.0000e+000),
		TVector3D(1.7514e+006, 6.9603e+005, 0.0000e+000),
		TVector3D(1.3082e+006, 5.1688e+005, 0.0000e+000),
		TVector3D(9.6188e+005, 3.7802e+005, 0.0000e+000),
		TVector3D(6.9697e+005, 2.7258e+005, 0.0000e+000),
		TVector3D(4.9637e+005, 1.9372e+005, 0.0000e+000),
		TVector3D(3.5063e+005, 1.3629e+005, 0.0000e+000),
		TVector3D(2.4518e+005, 9.5147e+004, 0.0000e+000),
		TVector3D(1.7060e+005, 6.6003e+004, 0.0000e+000),
		TVector3D(1.1831e+005, 4.6288e+004, 0.0000e+000),
		TVector3D(8.2300e+004, 3.1716e+004, 0.0000e+000),
		TVector3D(5.6581e+004, 2.2287e+004, 0.0000e+000),
		TVector3D(3.9435e+004, 1.5429e+004, 0.0000e+000),
		TVector3D(2.6576e+004, 1.0286e+004, 0.0000e+000),
		TVector3D(1.8860e+004, 6.8574e+003, 0.0000e+000),
		TVector3D(1.2859e+004, 5.1431e+003, 0.0000e+000),
		TVector3D(8.5729e+003, 3.4287e+003, 0.0000e+000),
		TVector3D(6.0010e+003, 2.5715e+003, 0.0000e+000),
		TVector3D(4.2864e+003, 1.7144e+003, 0.0000e+000),
		TVector3D(3.4291e+003, 8.5718e+002, 0.0000e+000),
		TVector3D(2.5719e+003, 8.5718e+002, 0.0000e+000),
		TVector3D(1.7146e+003, 8.5718e+002, 0.0000e+000),
		TVector3D(8.5729e+002, 0.0000e+000, 0.0000e+000),
		TVector3D(8.5729e+002, 0.0000e+000, 0.0000e+000),
		TVector3D(8.5729e+002, 0.0000e+000, 0.0000e+000),
		TVector3D(0.0000e+000, 0.0000e+000, 0.0000e+000),
		TVector3D(0.0000e+000, 0.0000e+000, 0.0000e+000),
	};
	const TScalar begin = 300e-9f;
	const TScalar end = 850e-9f;
	const TScalar delta = 1e-9f;
	const TScalar integral0 = 380e-9f;
	/**/

	num::SplineLinear<TScalar, TVector3D, num::DataTraitsStaticVector<TVector3D> > linear(
		w, w + (sizeof w / sizeof TScalar), xyz);
	std::ofstream outputLinear("spline_linear.txt");
	const size_t n = static_cast<size_t>(num::floor((end - begin) / delta));
	for (unsigned i = 0; i < n; ++i)
	{
		const TScalar x = begin + i * delta;
		const TVector3D y = linear(x);
		const TVector3D dy = linear.derivative(x);
		const TVector3D ddy = linear.derivative2(x);
		const TVector3D inty = linear.integral(integral0, x);
		outputLinear << x << "\t" 
			<< y.x << "\t" << y.y << "\t" << y.z << "\t"
			<< dy.x << "\t" << dy.y << "\t" << dy.z << "\t"
			<< ddy.x << "\t" << ddy.y << "\t" << ddy.z << "\t"
			<< inty.x << "\t" << inty.y << "\t" << inty.z << "\n";
	}

	num::SplineCubic<TScalar, TVector3D, num::DataTraitsStaticVector<TVector3D> > cubic(
		w, w + (sizeof w / sizeof TScalar), xyz);
	std::ofstream outputCubic("spline_cubic.txt");
	for (unsigned i = 0; i < n; ++i)
	{
		const TScalar x = begin + i * delta;
		const TVector3D y = cubic(x);
		const TVector3D dy = cubic.derivative(x);
		const TVector3D ddy = cubic.derivative2(x);
		const TVector3D inty = cubic.integral(integral0, x);
		outputCubic << x << "\t" 
			<< y.x << "\t" << y.y << "\t" << y.z << "\t"
			<< dy.x << "\t" << dy.y << "\t" << dy.z << "\t"
			<< ddy.x << "\t" << ddy.y << "\t" << ddy.z << "\t"
			<< inty.x << "\t" << inty.y << "\t" << inty.z << "\n";
	}
}


}

}

#endif

// EOF
