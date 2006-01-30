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



#ifndef LASS_GUARDIAN_OF_INCLUSION_TEST_TEST_SPAT_KD_TREE_INL
#define LASS_GUARDIAN_OF_INCLUSION_TEST_TEST_SPAT_KD_TREE_INL

#define LASS_SPAT_KD_TREE_DIAGNOSTICS

#include "test_common.h"
#include "../spat/kd_tree.h"
#include "../meta/select.h"
#include "../num/random.h"
#include "../prim/aabb_2d.h"
#include "../prim/aabb_3d.h"

namespace lass
{
namespace test
{

template <typename TPoint>
void testSpatKdTree()
{
	typedef spat::KdTree<TPoint> TKdTree;
	typedef typename TPoint::TValue TValue;

	typedef typename meta::Select
	<
		TPoint::dimension == 2,
		prim::Aabb2D<TValue>,
		prim::Aabb3D<TValue>
	>::Type TAabb;

	TPoint min;
	TPoint max;
	for (unsigned i = 0; i < TPoint::dimension; ++i)
	{
		min[i] = TValue(-1000);
		max[i] = TValue(+1000);
	}
	const TAabb bounds(min, max);

	const unsigned n = 1000;
	num::RandomMT19937 generator;
	TPoint points[n];
	for (unsigned i = 0; i < n; ++i)
	{
		points[i] = bounds.random(generator);
	}

	TKdTree tree(points, points + n);
	tree.diagnostics();

	const unsigned nNearestTests = 100;
	for (unsigned i = 0; i < nNearestTests; ++i)
	{
		TPoint target = bounds.random(generator);

		// naive nearest
		//
		TPoint naiveNearest = points[0];
		TValue naiveSqrDistance = squaredDistance(target, naiveNearest);
		for (unsigned k = 1; k < n; ++k)
		{
			const TValue sqrDist = squaredDistance(target, points[k]);
			if (sqrDist < naiveSqrDistance)
			{
				naiveNearest = points[k];
				naiveSqrDistance = sqrDist;
			}
		}

		// kd tree nearest
		TPoint kdTreeNearest = tree.nearestNeighbour(target).position();

		LASS_TEST_CHECK_EQUAL(kdTreeNearest, naiveNearest);
	}
}



}

}

#endif

// EOF
