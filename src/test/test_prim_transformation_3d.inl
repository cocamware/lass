/**	@file
 *  @internal
 *	@author Bram de Greve (bramz@users.sourceforge.net)
 *	@author Tom De Muer (tomdemuer@users.sourceforge.net)
 *
 *	Distributed under the terms of the GPL (GNU Public License)
 *
 * 	The LASS License:
 *
 *	Copyright 2004 Bram de Greve and Tom De Muer
 *
 *	LASS is free software; you can redistribute it and/or modify
 *	it under the terms of the GNU General Public License as published by
 *	the Free Software Foundation; either version 2 of the License, or
 *	(at your option) any later version.
 *
 *	This program is distributed in the hope that it will be useful,
 *	but WITHOUT ANY WARRANTY; without even the implied warranty of
 *	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *	GNU General Public License for more details.
 *
 *	You should have received a copy of the GNU General Public License
 *	along with this program; if not, write to the Free Software
 *	Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */



#ifndef LASS_GUARDIAN_OF_INCLUSION_TEST_TEST_PRIM_TRANSFORMATION_3D_INL
#define LASS_GUARDIAN_OF_INCLUSION_TEST_TEST_PRIM_TRANSFORMATION_3D_INL

#include "test_common.h"

#include "../prim/transformation_3d.h"
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
				BOOST_CHECK(num::abs(a[i] - b[i]) < epsilon);
				if (num::abs(a[i] - b[i]) >= epsilon)
				{
					LASS_COUT << "i:" << i << "\ta[i]:" << a[i] << "\tb[i]" << b[i] << "\n";
					LASS_COUT << testTransfo << "\n";
				}
            }
        }
        catch (...)
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
}   

}

}

#endif
