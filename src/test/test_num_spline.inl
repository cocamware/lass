/**	@file
 *	@author Bram de Greve (bramz@users.sourceforge.net)
 *	@author Tom De Muer (tomdemuer@users.sourceforge.net)
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
 *	Copyright (C) 2004-2007 the Initial Developer.
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



#ifndef LASS_GUARDIAN_OF_INCLUSION_TEST_TEST_NUM_SPLINE_INL
#define LASS_GUARDIAN_OF_INCLUSION_TEST_TEST_NUM_SPLINE_INL

#include "test_common.h"

#include "../num/spline_bezier_path.h"
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

	std::string mantissa = splitted[0];
	LASS_ASSERT(mantissa.length() == iDigitsMantissa + 2);
	const std::string negativeZero = "-0." + std::string(iDigitsMantissa - 1, '0');
	LASS_ASSERT(negativeZero.length() == iDigitsMantissa + 2);
	if (mantissa == negativeZero)
	{
		mantissa[0] = '+';
	}
	
	const int e = util::stringCast<int>(splitted[1]);
	buffer.str("");
	buffer << std::setw(iDigitsExponent + 1) << std::showpos << std::internal 
		<< std::setfill('0') << e;
	std::string exponent = buffer.str();
	LASS_ASSERT(exponent.length() == iDigitsExponent + 1);
	if (exponent[0] == '0')
	{
		LASS_ASSERT(e == 0);
		exponent[0] = '+';
	}

	const std::string result = mantissa + "e" + exponent;
	LASS_ASSERT(result.length() == iDigitsMantissa + iDigitsExponent + 4);

	return result;
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

	TestStream patternLinear("spline_linear.pattern", TestStream::asForbidSaving);
	num::SplineLinear<TScalar, TVector3D, num::DataTraitsStaticVector<TVector3D> > linear(
		frequencies.begin(), frequencies.end(), responses.begin());
	num_spline::testSpline(linear, patternLinear, begin, end, delta, integral0);

	TestStream patternCubic("spline_cubic.pattern",	TestStream::asForbidSaving);
	num::SplineCubic<TScalar, TVector3D, num::DataTraitsStaticVector<TVector3D> > cubic(
		frequencies.begin(), frequencies.end(), responses.begin());
	num_spline::testSpline(cubic, patternCubic, begin, end, delta, integral0);

	TestStream patternBezierPath("spline_bezier_path.pattern", TestStream::asAllowSaving);
	num::SplineBezierPath<TScalar, TVector3D, num::DataTraitsStaticVector<TVector3D> > bezierPath(
		frequencies.begin(), frequencies.end(), responses.begin());
	num_spline::testSpline(bezierPath, patternBezierPath, begin, end, delta, integral0);
}


}

}

#endif

// EOF
