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
 *	Copyright (C) 2024 the Initial Developer.
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

#include <lass/num/double_double.h>

#include <filesystem>
#include <fstream>

#include <quadmath.h>

using namespace lass;
using num::DoubleDouble;

constexpr DoubleDouble fromFloat128(__float128 x)
{
	const double high = static_cast<double>(x);
	const double low = static_cast<double>(x - high);
	return DoubleDouble(high, low);
}

std::ostream& operator<<(std::ostream& os, const DoubleDouble& x)
{
	return os << std::hexfloat << x.high() << ", " << x.low() << std::defaultfloat;
}

void generateNumTraits()
{
	LASS_COUT << "--- NumTraits ---" << std::endl;
	LASS_COUT << "static constexpr DoubleDouble pi { " << fromFloat128(M_PIq) << " };" << std::endl;
	LASS_COUT << "static constexpr DoubleDouble e { " << fromFloat128(M_Eq) << " };" << std::endl;
	LASS_COUT << "static constexpr DoubleDouble sqrt2 { " << fromFloat128(M_SQRT2q) << " };" << std::endl;
	LASS_COUT << "static constexpr DoubleDouble sqrtPi { " << fromFloat128(sqrtq(M_PIq)) << " };" << std::endl;
	LASS_COUT << "static constexpr DoubleDouble log2 { " << fromFloat128(M_LN2q) << " };" << std::endl;
	LASS_COUT << "---" << std::endl;
}

void generateExpTable(std::ostream& os)
{
	constexpr int r = 8;
	constexpr __float128 q = 1 << r;
	constexpr __float128 invQ = 1 / q;
	constexpr auto ddInvQ = fromFloat128(invQ);
	static_assert(ddInvQ.low() == 0.0, "double representation should be exact");

	constexpr __float128 d = 1024.0;
	constexpr __float128 c = M_LN2q;
	const __float128 c1 = roundq(c * d) / d;
	LASS_ENFORCE(static_cast<double>(c1) == c1);
	const __float128 c2 = c - c1;
	os << "extern const double expC1 = " << std::hexfloat << static_cast<double>(c1) << ";" << std::endl;
	os << "extern const DoubleDouble expC2 { " << fromFloat128(c2) << " };" << std::endl;

	const size_t n = static_cast<size_t>(ceilq(q * M_LN2q)) + 1;
	os << "extern const size_t expI_q_size = " << n << ";" << std::endl;
	os << "extern const DoubleDouble expI_q[expI_q_size] = {" << std::endl;
	for (size_t i = 0; i < n; ++i)
	{
		const __float128 x = i * invQ;
		LASS_ENFORCE(static_cast<double>(x) == x); // exact representation
		const __float128 y = expq(x);
		os << "\t{ " << fromFloat128(y) << " }, // exp(" << static_cast<double>(x) << ")" << std::endl;
	}
	os << "};" << std::endl;
}

void generateDoubleDoubleConstants()
{
	const std::filesystem::path sourceDir = LASS_SOURCE_DIR;
	const auto path = sourceDir / "lass/num/double_double_constants.cpp";

	std::ofstream os(path);
	os <<
		"#include <lass/num/double_double.h>\n"
		"\n"
		"namespace lass\n"
		"{\n"
		"namespace num\n"
		"{\n"
		"namespace impl\n"
		"{\n"
		"\n";

	generateExpTable(os);

	os <<
		"\n"
		"}\n"
		"}\n"
		"}\n";
}

int main()
{
	generateNumTraits();
	generateDoubleDoubleConstants();
	return 0;
}
