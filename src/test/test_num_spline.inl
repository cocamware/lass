/** @file
 *  @internal
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



#ifndef LASS_GUARDIAN_OF_INCLUSION_TEST_TEST_NUM_SPLINE_INL
#define LASS_GUARDIAN_OF_INCLUSION_TEST_TEST_NUM_SPLINE_INL

#include "test_common.h"

#include "../num/spline_linear.h"
#include "../num/spline_cubic.h"
#include "../prim/vector_3d.h"
#include "../io/file_attribute.h"

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
	typedef double TScalar;
	typedef prim::Vector3D<double> TVector3D;

	std::vector<TScalar> frequencies;
	std::vector<TVector3D> responses;
	std::ifstream splineIn(io::fileJoinPath(test::workPath(), "spline.in").c_str());
	if (!splineIn)
	{
		LASS_THROW("Could not find 'spline.in'.");
	}
	try
	{
		std::string buffer;
		while (std::getline(LASS_ENFORCE_STREAM(splineIn), buffer, '\n'))
		{
			std::istringstream line(buffer);
			TScalar f;
			TVector3D xyz;
			line >> f >> xyz.x >> xyz.y >> xyz.z;
			frequencies.push_back(f);
			responses.push_back(xyz);
		}
	}
	catch (std::exception& error)
	{
		LASS_THROW("Reading 'spline.in' failed: " << error.what());
	}

	const TScalar begin = 300e-9f;
	const TScalar end = 850e-9f;
	const TScalar delta = 1e-9f;
	const TScalar integral0 = 380e-9f;

	TestStream patternLinear("spline_linear.pattern", TestStream::asAllowSaving);
	patternLinear << std::scientific;

	num::SplineLinear<TScalar, TVector3D, num::DataTraitsStaticVector<TVector3D> > linear(
		frequencies.begin(), frequencies.end(), responses.begin());
	const size_t n = static_cast<size_t>(num::floor((end - begin) / delta));
	for (unsigned i = 0; i < n; ++i)
	{
		const TScalar x = begin + i * delta;
		const TVector3D y = linear(x);
		const TVector3D dy = linear.derivative(x);
		const TVector3D ddy = linear.derivative2(x);
		const TVector3D inty = linear.integral(integral0, x);
		patternLinear << std::setprecision(2) << x << "\t" << std::setprecision(4)
			<< y.x << "\t" << y.y << "\t" << y.z << "\t"
			<< dy.x << "\t" << dy.y << "\t" << dy.z << "\t"
			<< ddy.x << "\t" << ddy.y << "\t" << ddy.z << "\t"
			<< inty.x << "\t" << inty.y << "\t" << inty.z << "\n";
		LASS_TEST_CHECK(patternLinear.matchPattern());
	}

	TestStream patternCubic("spline_cubic.pattern",	TestStream::asAllowSaving);
	patternCubic << std::scientific;

	num::SplineCubic<TScalar, TVector3D, num::DataTraitsStaticVector<TVector3D> > cubic(
		frequencies.begin(), frequencies.end(), responses.begin());
	for (unsigned i = 0; i < n; ++i)
	{
		const TScalar x = begin + i * delta;
		const TVector3D y = cubic(x);
		const TVector3D dy = cubic.derivative(x);
		const TVector3D ddy = cubic.derivative2(x);
		const TVector3D inty = cubic.integral(integral0, x);
		patternCubic << std::setprecision(2) << x << "\t" << std::setprecision(4)
			<< y.x << "\t" << y.y << "\t" << y.z << "\t"
			<< dy.x << "\t" << dy.y << "\t" << dy.z << "\t"
			<< ddy.x << "\t" << ddy.y << "\t" << ddy.z << "\t"
			<< inty.x << "\t" << inty.y << "\t" << inty.z << "\n";
		LASS_TEST_CHECK(patternCubic.matchPattern());
	}
}


}

}

#endif

// EOF
