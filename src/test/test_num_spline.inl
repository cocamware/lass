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
		3.70e-007f,
		3.75e-007f,
		3.80e-007f,
		3.85e-007f,
		3.90e-007f,
		3.95e-007f,
		4.00e-007f,
		4.05e-007f,
		4.10e-007f,
		4.15e-007f,
		4.20e-007f,
		4.25e-007f,
		4.30e-007f,
		4.35e-007f,
		4.40e-007f,
		4.45e-007f,
		4.50e-007f,
		4.55e-007f,
		4.60e-007f,
		4.65e-007f,
		4.70e-007f,
		4.75e-007f,
		4.80e-007f,
		4.85e-007f,
		4.90e-007f,
		4.95e-007f,
		5.00e-007f,
		5.05e-007f,
		5.10e-007f,
		5.15e-007f,
		5.20e-007f,
		5.25e-007f,
		5.30e-007f,
		5.35e-007f,
		5.40e-007f,
		5.45e-007f,
		5.50e-007f,
		5.55e-007f,
		5.60e-007f,
		5.65e-007f,
		5.70e-007f,
		5.75e-007f,
		5.80e-007f,
		5.85e-007f,
		5.90e-007f,
		5.95e-007f,
		6.00e-007f,
		6.05e-007f,
		6.10e-007f,
		6.15e-007f,
		6.20e-007f,
		6.25e-007f,
		6.30e-007f,
		6.35e-007f,
		6.40e-007f,
		6.45e-007f,
		6.50e-007f,
		6.55e-007f,
		6.60e-007f,
		6.65e-007f,
		6.70e-007f,
		6.75e-007f,
		6.80e-007f,
		6.85e-007f,
		6.90e-007f,
		6.95e-007f,
		7.00e-007f,
		7.05e-007f,
		7.10e-007f,
		7.15e-007f,
		7.20e-007f,
		7.25e-007f,
		7.30e-007f,
		7.35e-007f,
		7.40e-007f,
		7.45e-007f,
		7.50e-007f,
		7.55e-007f,
		7.60e-007f,
		7.65e-007f,
		7.70e-007f,
		7.75e-007f,
		7.80e-007f,
	};
	const TVector3D xyz[] =
	{
		TVector3D(0.0000e000f, 0.0000e000f, 0.0000e000f),
		TVector3D(0.0000e000f, 0.0000e000f, 0.0000e000f),
		TVector3D(1.7146e003f, 0.0000e000f, 5.9997e003f),
		TVector3D(6.0010e003f, 8.5718e002f, 2.4856e004f),
		TVector3D(2.0575e004f, 2.5715e003f, 8.9996e004f),
		TVector3D(6.1725e004f, 6.8574e003f, 2.7684e005f),
		TVector3D(1.6374e005f, 1.7144e004f, 7.3711e005f),
		TVector3D(3.7206e005f, 3.8573e004f, 1.6894e006f),
		TVector3D(7.2612e005f, 7.5432e004f, 3.3376e006f),
		TVector3D(1.2053e006f, 1.2429e005f, 5.6295e006f),
		TVector3D(1.7532e006f, 1.8344e005f, 8.3353e006f),
		TVector3D(2.2692e006f, 2.5287e005f, 1.0992e007f),
		TVector3D(2.6979e006f, 3.3173e005f, 1.3315e007f),
		TVector3D(3.0665e006f, 4.2516e005f, 1.5415e007f),
		TVector3D(3.2894e006f, 5.3231e005f, 1.6862e007f),
		TVector3D(3.3151e006f, 6.4031e005f, 1.7376e007f),
		TVector3D(3.1780e006f, 7.6717e005f, 1.7098e007f),
		TVector3D(2.9405e006f, 9.1118e005f, 1.6291e007f),
		TVector3D(2.5916e006f, 1.0989e006f, 1.4960e007f),
		TVector3D(2.1784e006f, 1.3098e006f, 1.3327e007f),
		TVector3D(1.6769e006f, 1.5875e006f, 1.1293e007f),
		TVector3D(1.1342e006f, 1.8849e006f, 8.8299e006f),
		TVector3D(6.9012e005f, 2.1738e006f, 6.6177e006f),
		TVector3D(3.5235e005f, 2.5518e006f, 4.8863e006f),
		TVector3D(1.3888e005f, 2.9067e006f, 3.5596e006f),
		TVector3D(4.3722e004f, 3.3893e006f, 2.5919e006f),
		TVector3D(3.2577e004f, 3.9499e006f, 1.8728e006f),
		TVector3D(1.3202e005f, 4.5550e006f, 1.3645e006f),
		TVector3D(3.2148e005f, 5.2005e006f, 9.5996e005f),
		TVector3D(6.1210e005f, 5.8777e006f, 7.0454e005f),
		TVector3D(1.0090e006f, 6.5300e006f, 5.2026e005f),
		TVector3D(1.4831e006f, 7.0571e006f, 3.6941e005f),
		TVector3D(2.0275e006f, 7.5020e006f, 2.6142e005f),
		TVector3D(2.6079e006f, 7.9186e006f, 1.7656e005f),
		TVector3D(3.2303e006f, 8.2460e006f, 1.1742e005f),
		TVector3D(3.8715e006f, 8.4192e006f, 6.7711e004f),
		TVector3D(4.5419e006f, 8.5015e006f, 3.4284e004f),
		TVector3D(5.2817e006f, 8.5641e006f, 9.4281e003f),
		TVector3D(6.0456e006f, 8.5486e006f, 0.0000e000f),
		TVector3D(6.8051e006f, 8.4209e006f, 0.0000e000f),
		TVector3D(7.5330e006f, 8.1912e006f, 0.0000e000f),
		TVector3D(8.1545e006f, 7.8449e006f, 0.0000e000f),
		TVector3D(8.6946e006f, 7.4480e006f, 0.0000e000f),
		TVector3D(9.2098e006f, 7.0769e006f, 0.0000e000f),
		TVector3D(9.5888e006f, 6.6637e006f, 0.0000e000f),
		TVector3D(9.7242e006f, 6.1751e006f, 0.0000e000f),
		TVector3D(9.6359e006f, 5.6428e006f, 0.0000e000f),
		TVector3D(9.3367e006f, 5.0908e006f, 0.0000e000f),
		TVector3D(8.8343e006f, 4.5259e006f, 0.0000e000f),
		TVector3D(8.1502e006f, 3.9584e006f, 0.0000e000f),
		TVector3D(7.3410e006f, 3.4124e006f, 0.0000e000f),
		TVector3D(6.4717e006f, 2.9110e006f, 0.0000e000f),
		TVector3D(5.5509e006f, 2.4301e006f, 0.0000e000f),
		TVector3D(4.5873e006f, 1.9569e006f, 0.0000e000f),
		TVector3D(3.7001e006f, 1.5412e006f, 0.0000e000f),
		TVector3D(2.9465e006f, 1.2018e006f, 0.0000e000f),
		TVector3D(2.3001e006f, 9.2232e005f, 0.0000e000f),
		TVector3D(1.7514e006f, 6.9603e005f, 0.0000e000f),
		TVector3D(1.3082e006f, 5.1688e005f, 0.0000e000f),
		TVector3D(9.6188e005f, 3.7802e005f, 0.0000e000f),
		TVector3D(6.9697e005f, 2.7258e005f, 0.0000e000f),
		TVector3D(4.9637e005f, 1.9372e005f, 0.0000e000f),
		TVector3D(3.5063e005f, 1.3629e005f, 0.0000e000f),
		TVector3D(2.4518e005f, 9.5147e004f, 0.0000e000f),
		TVector3D(1.7060e005f, 6.6003e004f, 0.0000e000f),
		TVector3D(1.1831e005f, 4.6288e004f, 0.0000e000f),
		TVector3D(8.2300e004f, 3.1716e004f, 0.0000e000f),
		TVector3D(5.6581e004f, 2.2287e004f, 0.0000e000f),
		TVector3D(3.9435e004f, 1.5429e004f, 0.0000e000f),
		TVector3D(2.6576e004f, 1.0286e004f, 0.0000e000f),
		TVector3D(1.8860e004f, 6.8574e003f, 0.0000e000f),
		TVector3D(1.2859e004f, 5.1431e003f, 0.0000e000f),
		TVector3D(8.5729e003f, 3.4287e003f, 0.0000e000f),
		TVector3D(6.0010e003f, 2.5715e003f, 0.0000e000f),
		TVector3D(4.2864e003f, 1.7144e003f, 0.0000e000f),
		TVector3D(3.4291e003f, 8.5718e002f, 0.0000e000f),
		TVector3D(2.5719e003f, 8.5718e002f, 0.0000e000f),
		TVector3D(1.7146e003f, 8.5718e002f, 0.0000e000f),
		TVector3D(8.5729e002f, 0.0000e000f, 0.0000e000f),
		TVector3D(8.5729e002f, 0.0000e000f, 0.0000e000f),
		TVector3D(8.5729e002f, 0.0000e000f, 0.0000e000f),
		TVector3D(0.0000e000f, 0.0000e000f, 0.0000e000f),
		TVector3D(0.0000e000f, 0.0000e000f, 0.0000e000f),
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
