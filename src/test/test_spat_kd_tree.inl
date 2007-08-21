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
		meta::Bool< TPoint::dimension == 2 >,
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
