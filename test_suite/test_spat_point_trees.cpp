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
 *	Copyright (C) 2004-2011 the Initial Developer.
 *	All Rights Reserved.
 *	
 *	Contributor(s):
 * *	Alternatively, the contents of this file may be used under the terms of the 
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

//#define LASS_SPAT_KD_TREE_DIAGNOSTICS

#include "test_common.h"
#include "tree_test_helpers.h"

namespace lass
{
namespace test
{

template <typename TPoint>
void testSpatPointTrees()
{
	typedef typename TPoint::TValue TValue;
	typedef typename meta::Select
	<
		meta::Bool< TPoint::dimension == 2 >,
		prim::Aabb2D<TValue>,
		prim::Aabb3D<TValue>
	>::Type TAabb;
	typedef const TPoint* TPointIterator;

	// set bounds
	//
	TPoint min;
	TPoint max;
	for (unsigned i = 0; i < TPoint::dimension; ++i)
	{
		min[i] = TValue(-1000);
		max[i] = TValue(+1000);
	}
	const TAabb bounds(min, max);

	// generate points
	//
	const unsigned n = 100000;
	num::RandomMT19937 generator;
	std::vector<TPoint> points(n);
	for (unsigned i = 0; i < n; ++i)
	{
		points[i] = bounds.random(generator);
	}

	// make single tree, and do diagnostics on itµ
	//
	typedef spat::KdTree<TPoint> TKdTree;
	TKdTree tree(&points[0], &points[0] + n);
	//tree.diagnostics();



	// make tree set
	//
	typedef spat::DefaultObjectTraits<TPoint, TAabb, meta::NullType, TPointIterator> TPointTraits;
	typedef typename meta::type_list::Make<
		TKdTree,
		spat::AabbTree<TPoint, TPointTraits>,
		spat::AabpTree<TPoint, TPointTraits>
		//spat::QuadTree<TPoint, TPointTraits> // apparently, QuadTrees are no good idea for points
	>::Type TTreeTypes;
	typedef meta::Tuple<TTreeTypes> TTrees;
	TTrees trees;
	tree_test_helpers::TreeConstructor<TPointIterator> construct(&points[0], &points[0] + n);
	meta::tuple::forEach(trees, construct);

	// VALIDATION TESTS
	//
	const unsigned nNearestTests = 1000;
	for (unsigned i = 0; i < nNearestTests; ++i)
	{
		TPoint target = bounds.random(generator);

		// naive nearest
		//
		size_t naiveNearest = 0;
		TValue naiveSqrDistance = squaredDistance(target, points[0]);
		for (unsigned k = 1; k < n; ++k)
		{
			const TValue sqrDist = squaredDistance(target, points[k]);
			if (sqrDist < naiveSqrDistance)
			{
				naiveNearest = k;
				naiveSqrDistance = sqrDist;
			}
		}

		// kd tree nearest
		//
		TPoint kdTreeNearest = tree.nearestNeighbour(target).position();

		LASS_TEST_CHECK_EQUAL(kdTreeNearest, points[naiveNearest]);

		tree_test_helpers::NearestNeighbourValidityTest<TPoint, TPointIterator> test(target, &points[naiveNearest], naiveSqrDistance);
		meta::tuple::forEach(trees, test);
	}

	// SPEED TESTS
	//
	util::Clock clock;
	util::StopWatch stopWatch(clock);

	const size_t nTargets = 1000;
	const size_t nRuns = 2;
	const TValue maxRadius = 50;
	const size_t maxCount = 10;
	typedef std::vector<TPoint> TTargets;
	TTargets targets;
	for (size_t i = 0; i < nTargets; ++i)
	{
		targets.push_back(bounds.random(generator));
	}

	LASS_COUT << "nearest neighbour tests:\n";
	tree_test_helpers::NearestNeighbourSpeedTest<TTargets> nearestNeighbourSpeedTest(
		targets, stopWatch, nRuns);
	meta::tuple::forEach(trees, nearestNeighbourSpeedTest);

	LASS_COUT << "range search tests:\n";
	tree_test_helpers::RangeSearchSpeedTest<TTargets, TValue> rangeSearchSpeedTest(
		targets, maxRadius, maxCount, stopWatch, nRuns);
	meta::tuple::forEach(trees, rangeSearchSpeedTest);	
}


TUnitTest test_spat_point_trees()
{
	TUnitTest result;
	result.push_back(LASS_TEST_CASE(testSpatPointTrees< prim::Point2D<float> >));
	result.push_back(LASS_TEST_CASE(testSpatPointTrees< prim::Point3D<float> >));
	result.push_back(LASS_TEST_CASE(testSpatPointTrees< prim::Point2D<double> >));
	result.push_back(LASS_TEST_CASE(testSpatPointTrees< prim::Point3D<double> >));
	return result;
}

}
}

// EOF
