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
#include "../num/matrix.h"
#include "../util/stop_watch.h"

#define LASS_TEST_PRIM_TRANSFORMATION_3D_EPSILON 1e-5

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
    typedef prim::Transformation3D<T> TTransformation;
    typedef prim::Point3D<T> TPoint;
    typedef prim::Vector3D<T> TVector;
    typedef num::NumTraits<T> TNumTraits;
    typedef num::Matrix<T, num::impl::MStorage<T> > TMatrix;

    const T epsilon = T(LASS_TEST_PRIM_RAY_EPSILON);

    TTransformation identity;
    TTransformation transfo;
    TMatrix matrix(4, 4);

    num::RandomMT19937 random;
    num::DistributionUniform<T, num::RandomMT19937> distribution(random);

    T differenceTransfo = 0;
    T differenceMatrix = 0;

    unsigned succeededRuns = 0;
    for (unsigned k = 0; k < 1000; ++k)
    {
        for (unsigned i = 0; i < 4; ++i)
        {
            for (unsigned j = 0; j < 4; ++j)
            {
                matrix(i, j) = transfo(i, j) = distribution();
            }
        }

        try
        {
            TTransformation invTransfo = transfo.inverse();
            TTransformation testTransfo = transfo * invTransfo;
            for (unsigned i = 0; i < 4; ++i)
            {
                for (unsigned j = 0; j < 4; ++j)
                {
                    differenceTransfo += num::sqr(testTransfo(i, j) - identity(i, j));
                }
            }

            TMatrix invMatrix = matrix;
            invMatrix.inverse();
            TMatrix testMatrix = matrix * invMatrix;
            for (unsigned i = 0; i < 4; ++i)
            {
                for (unsigned j = 0; j < 4; ++j)
                {
                    differenceMatrix += num::sqr(testMatrix(i, j) - identity(i, j));
                }
            }

            ++succeededRuns;
        }
        catch (...)
        {
        }
    }

    LASS_COUT << "transfo diff " << num::sqrt(differenceTransfo / succeededRuns) << std::endl;
    LASS_COUT << "matrix  diff " << num::sqrt(differenceMatrix / succeededRuns) << std::endl;

    // use last matrix to do a speed test
    
    const unsigned nSpeed = 10000;
    util::Clock clock;
    util::StopWatch stopWatch(clock);

    stopWatch.start();
    stopWatch.stop();
    const util::StopWatch::TTime correction = stopWatch.time();

    stopWatch.reset();
    stopWatch.start();
    for (unsigned k = 0; k < nSpeed; ++k)
    {
        transfo.inverse();
    }
    stopWatch.stop();
    LASS_COUT << "transfo time " << ((stopWatch.time() - correction) / nSpeed) << std::endl;

    stopWatch.reset();
    stopWatch.start();
    for (unsigned k = 0; k < nSpeed; ++k)
    {
        matrix.inverse();
    }
    stopWatch.stop();
    LASS_COUT << "matrix  time " << ((stopWatch.time() - correction) / nSpeed) << std::endl;

}   

}

}

#endif
