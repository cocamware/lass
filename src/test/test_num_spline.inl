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
#include "../stde/extended_string.h"
namespace lass
{
namespace test
{

namespace num_spline
{

std::string doubleToString(double iX, int iDigitsMantissa, int iDigitsExponent)
{
	std::ostringstream buffer;
	buffer << std::scientific << std::showpos << std::setprecision(iDigitsMantissa - 1) << iX;

	const std::vector<std::string> splitted = stde::split(buffer.str(), std::string("e"));
	LASS_ASSERT(splitted.size() == 2);

	const std::string mantissa = splitted[0];
	const int exponent = util::stringCast<int>(splitted[1]);
	LASS_ASSERT(mantissa.length() == iDigitsMantissa + 2);

	buffer.str("");
	buffer << mantissa << "e" << std::setw(iDigitsExponent + 1) << std::showpos << std::internal 
		<< std::setfill('0') << exponent;
	LASS_ASSERT(buffer.str().length() == iDigitsMantissa + iDigitsExponent + 4);

	return buffer.str();
}

template <typename SplineType, typename T> 
void testSpline(const SplineType& iSpline, test::TestStream& ioPattern, 
		T iBegin, T iEnd, T iDelta, T iIntegral0)
{
	typedef T TScalar;
	typedef prim::Vector3D<T> TVector3D;

	const size_t n = static_cast<size_t>(num::floor((iEnd - iBegin) / iDelta));
	for (unsigned i = 0; i < n; ++i)
	{
		const TScalar f = iBegin + i * iDelta;
		const TVector3D xyz = iSpline(f);
		const TVector3D dXyz = iSpline.derivative(f);
		const TVector3D ddXyz = iSpline.derivative2(f);
		const TVector3D intXyz = iSpline.integral(iIntegral0, f);
		ioPattern << doubleToString(f, 3, 3) << "\t"
			<< doubleToString(xyz.x, 5, 3) << "\t" 
			<< doubleToString(xyz.y, 5, 3) << "\t" 
			<< doubleToString(xyz.z, 5, 3) << "\t"
			<< doubleToString(dXyz.x, 5, 3) << "\t" 
			<< doubleToString(dXyz.y, 5, 3) << "\t" 
			<< doubleToString(dXyz.z, 5, 3) << "\t"
			<< doubleToString(ddXyz.x, 5, 3) << "\t" 
			<< doubleToString(ddXyz.y, 5, 3) << "\t" 
			<< doubleToString(ddXyz.z, 5, 3) << "\t"
			<< doubleToString(intXyz.x, 5, 3) << "\t" 
			<< doubleToString(intXyz.y, 5, 3) << "\t" 
			<< doubleToString(intXyz.z, 5, 3) << "\n";
		LASS_TEST_CHECK(ioPattern.matchPattern());
	}
}

}

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
	num::SplineLinear<TScalar, TVector3D, num::DataTraitsStaticVector<TVector3D> > linear(
		frequencies.begin(), frequencies.end(), responses.begin());
	num_spline::testSpline(linear, patternLinear, begin, end, delta, integral0);

	TestStream patternCubic("spline_cubic.pattern",	TestStream::asAllowSaving);
	num::SplineCubic<TScalar, TVector3D, num::DataTraitsStaticVector<TVector3D> > cubic(
		frequencies.begin(), frequencies.end(), responses.begin());
	num_spline::testSpline(cubic, patternCubic, begin, end, delta, integral0);
}


}

}

#endif

// EOF
