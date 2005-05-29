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

void testNumSpline()
{
	typedef float TScalar;
	typedef prim::Vector3D<float> TData;

	/*
	const TScalar xx[] = { -13.f, -5.6f, -5.5f, 2.f, 3.14f, 7.34f,   9.f, 10.7f };
	const TScalar yy[] = {  4.8f, -34.f, -34.f, 0.f,  6.5f,  0.5f, -20.f, -15.f };
	/*/
	const TScalar xx[] = {365e-9f, 370e-9f, 375e-9f, 380e-9f, 385e-9f, 390e-9f, 395e-9f, 400e-9f, 405e-9f, 410e-9f, 415e-9f, 420e-9f, 425e-9f, 430e-9f, 435e-9f, 440e-9f, 445e-9f, 450e-9f, 455e-9f, 460e-9f, 465e-9f, 470e-9f, 475e-9f, 480e-9f, 485e-9f, 490e-9f, 495e-9f, 500e-9f, 505e-9f, 510e-9f, 515e-9f, 520e-9f, 525e-9f, 530e-9f, 535e-9f, 540e-9f, 545e-9f, 550e-9f, 555e-9f, 560e-9f, 565e-9f, 570e-9f, 575e-9f, 580e-9f, 585e-9f, 590e-9f, 595e-9f, 600e-9f, 605e-9f, 610e-9f, 615e-9f, 620e-9f, 625e-9f, 630e-9f, 635e-9f, 640e-9f, 645e-9f, 650e-9f, 655e-9f, 660e-9f, 665e-9f, 670e-9f, 675e-9f, 680e-9f, 685e-9f, 690e-9f, 695e-9f, 700e-9f, 705e-9f, 710e-9f, 715e-9f, 720e-9f, 725e-9f, 730e-9f, 735e-9f, 740e-9f, 745e-9f, 750e-9f, 755e-9f, 760e-9f, 765e-9f, 770e-9f, 775e-9f, 780e-9f, 785e-9f};
	const TData yy[] = {TData(), TData(), TData(), TData(0.0002, 0.0000, 0.0007), TData(0.0007, 0.0001, 0.0029), TData(0.0024, 0.0003, 0.0105), TData(0.0072, 0.0008, 0.0323), TData(0.0191, 0.0020, 0.0860), TData(0.0434, 0.0045, 0.1971), TData(0.0847, 0.0088, 0.3894), TData(0.1406, 0.0145, 0.6568), TData(0.2045, 0.0214, 0.9725), TData(0.2647, 0.0295, 1.2825), TData(0.3147, 0.0387, 1.5535), TData(0.3577, 0.0496, 1.7985), TData(0.3837, 0.0621, 1.9673), TData(0.3867, 0.0747, 2.0273), TData(0.3707, 0.0895, 1.9948), TData(0.3430, 0.1063, 1.9007), TData(0.3023, 0.1282, 1.7454), TData(0.2541, 0.1528, 1.5549), TData(0.1956, 0.1852, 1.3176), TData(0.1323, 0.2199, 1.0302), TData(0.0805, 0.2536, 0.7721), TData(0.0411, 0.2977, 0.5701), TData(0.0162, 0.3391, 0.4153), TData(0.0051, 0.3954, 0.3024), TData(0.0038, 0.4608, 0.2185), TData(0.0154, 0.5314, 0.1592), TData(0.0375, 0.6067, 0.1120), TData(0.0714, 0.6857, 0.0822), TData(0.1177, 0.7618, 0.0607), TData(0.1730, 0.8233, 0.0431), TData(0.2365, 0.8752, 0.0305), TData(0.3042, 0.9238, 0.0206), TData(0.3768, 0.9620, 0.0137), TData(0.4516, 0.9822, 0.0079), TData(0.5298, 0.9918, 0.0040), TData(0.6161, 0.9991, 0.0011), TData(0.7052, 0.9973, 0.0000), TData(0.7938, 0.9824, 0.0000), TData(0.8787, 0.9556, 0.0000), TData(0.9512, 0.9152, 0.0000), TData(1.0142, 0.8689, 0.0000), TData(1.0743, 0.8256, 0.0000), TData(1.1185, 0.7774, 0.0000), TData(1.1343, 0.7204, 0.0000), TData(1.1240, 0.6583, 0.0000), TData(1.0891, 0.5939, 0.0000), TData(1.0305, 0.5280, 0.0000), TData(0.9507, 0.4618, 0.0000), TData(0.8563, 0.3981, 0.0000), TData(0.7549, 0.3396, 0.0000), TData(0.6475, 0.2835, 0.0000), TData(0.5351, 0.2283, 0.0000), TData(0.4316, 0.1798, 0.0000), TData(0.3437, 0.1402, 0.0000), TData(0.2683, 0.1076, 0.0000), TData(0.2043, 0.0812, 0.0000), TData(0.1526, 0.0603, 0.0000), TData(0.1122, 0.0441, 0.0000), TData(0.0813, 0.0318, 0.0000), TData(0.0579, 0.0226, 0.0000), TData(0.0409, 0.0159, 0.0000), TData(0.0286, 0.0111, 0.0000), TData(0.0199, 0.0077, 0.0000), TData(0.0138, 0.0054, 0.0000), TData(0.0096, 0.0037, 0.0000), TData(0.0066, 0.0026, 0.0000), TData(0.0046, 0.0018, 0.0000), TData(0.0031, 0.0012, 0.0000), TData(0.0022, 0.0008, 0.0000), TData(0.0015, 0.0006, 0.0000), TData(0.0010, 0.0004, 0.0000), TData(0.0007, 0.0003, 0.0000), TData(0.0005, 0.0002, 0.0000), TData(0.0004, 0.0001, 0.0000), TData(0.0003, 0.0001, 0.0000), TData(0.0002, 0.0001, 0.0000), TData(0.0001, 0.0000, 0.0000), TData(0.0001, 0.0000, 0.0000), TData(0.0001, 0.0000, 0.0000), TData(0.0000, 0.0000, 0.0000), TData(0.0000, 0.0000, 0.0000), TData(0.0000, 0.0000, 0.0000), };
	const TScalar begin = 300e-9f;
	const TScalar end = 850e-9f;
	const TScalar delta = 1e-9f;
	const TScalar integral0 = begin;
	/**/

	num::SplineLinear<TScalar, TData, FixedDimensionDataTraits<TScalar, TData> > linear(xx, xx + (sizeof xx / sizeof TScalar), yy);
	std::ofstream outputLinear("spline_linear.txt");
	const size_t n = static_cast<size_t>(num::floor((end - begin) / delta));
	for (unsigned i = 0; i < n; ++i)
	{
		const TScalar x = begin + i * delta;
		const TData y = linear(x);
		const TData dy = linear.derivative(x);
		const TData ddy = linear.derivative2(x);
		const TData inty = linear.integral(integral0, x);
		outputLinear << x << "\t" 
			<< y.x << "\t" << y.y << "\t" << y.z << "\t"
			<< dy.x << "\t" << dy.y << "\t" << dy.z << "\t"
			<< ddy.x << "\t" << ddy.y << "\t" << ddy.z << "\t"
			<< inty.x << "\t" << inty.y << "\t" << inty.z << "\n";
	}

	num::SplineCubic<TScalar, TData, FixedDimensionDataTraits<TScalar, TData> > cubic(xx, xx + (sizeof xx / sizeof TScalar), yy);
	std::ofstream outputCubic("spline_cubic.txt");
	for (unsigned i = 0; i < n; ++i)
	{
		const TScalar x = begin + i * delta;
		const TData y = cubic(x);
		const TData dy = cubic.derivative(x);
		const TData ddy = cubic.derivative2(x);
		const TData inty = cubic.integral(integral0, x);
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
