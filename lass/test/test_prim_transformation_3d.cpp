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

#include "test_common.h"

#include "../prim/transformation_3d.h"
#include "../prim/transformation_2d.h"
#include "../num/random.h"
#include "../num/distribution.h"
/*
#include "../prim/aabb_3d.h"
#include "../util/stop_watch.h"
*/
namespace lass
{
namespace test
{

template 
<
	typename T
> 
void testPrimTransformation3D()
{
	const T epsilon = static_cast<T>(1e-2);

	typedef prim::Transformation3D<T> TTransformation;
	typedef prim::Point3D<T> TPoint;
	typedef prim::Vector3D<T> TVector;
	typedef num::NumTraits<T> TNumTraits;

	TTransformation identity;
	TTransformation transfo;

	num::RandomMT19937 random;
	num::DistributionUniform<T, num::RandomMT19937> distribution(random);

	for (unsigned k = 0; k < 100; ++k)
	{
		T mat[16];
		std::generate(mat, mat + 16, distribution);
		transfo = TTransformation(mat, mat + 16);

		try
		{
			TTransformation invTransfo = transfo.inverse();
			TTransformation testTransfo = concatenate(transfo, invTransfo);
			const T* a = testTransfo.matrix();
			const T* b = identity.matrix();
			for (unsigned i = 0; i < 16; ++i)
			{
				LASS_TEST_CHECK(num::abs(a[i] - b[i]) < epsilon);
				if (num::abs(a[i] - b[i]) >= epsilon)
				{
					LASS_COUT << "i:" << i << "\ta[i]:" << a[i] << "\tb[i]" << b[i] << "\n";
					LASS_COUT << testTransfo << "\n";
				}
			}
		}
		catch (const prim::SingularityError&)
		{
		}
	}
/*
	util::Clock clock;
	util::StopWatch stopWatch(clock);

	stopWatch.start();
	stopWatch.stop();
	stopWatch.reset();
	stopWatch.start();
	const double correction = -stopWatch.stop();

	prim::Aabb3D<T, prim::UncheckedMinMax> aabb = prim::Aabb3D<T, prim::AutoMinMax>(
		prim::Point3D<T>(distribution(), distribution(), distribution()),
		prim::Point3D<T>(distribution(), distribution(), distribution()));
	T mat[16];
	std::generate(mat, mat + 16, distribution);
	transfo = TTransformation(mat, mat + 16);

	const unsigned n = 1000000;
	stopWatch.reset();
	stopWatch.start();
	for (unsigned k = 0; k < n; ++k)
	{
		transform(aabb, transfo);
	}
	LASS_COUT << "time: " << ((stopWatch.stop() + correction) / n) << "\n";
*/

	// test concatenation

	TTransformation A = TTransformation::rotation('x', TNumTraits::pi / 2);
	TTransformation B = TTransformation::rotation('y', TNumTraits::pi / 2);
	TTransformation C = concatenate(A, B);
	TVector v(0, 0, 1);
	LASS_TEST_CHECK_CLOSE_ARRAY(transform(transform(v, A), B), transform(v, C), epsilon, 3);
}   



TUnitTest test_prim_transformation_3d()
{
	TUnitTest result;
	result.push_back(LASS_TEST_CASE(testPrimTransformation3D<float>));
	result.push_back(LASS_TEST_CASE(testPrimTransformation3D<double>));
	return result;
}

}

}
