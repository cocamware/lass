/** @file
 *  @internal
 *  @author Bram de Greve (bramz@users.sourceforge.net)
 *  @author Tom De Muer (tomdemuer@users.sourceforge.net)
 *
 *  Distributed under the terms of the GPL (GNU Public License)
 *
 *  The LASS License:
 *
 *  Copyright 2004 Bram de Greve and Tom De Muer
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



#ifndef LASS_GUARDIAN_OF_INCLUSION_TEST_TEST_SPAT_AABB_TREE_INL
#define LASS_GUARDIAN_OF_INCLUSION_TEST_TEST_SPAT_AABB_TREE_INL

#include "test_common.h"
#include "../spat/aabb_tree.h"
#include "../meta/select.h"
#include "../num/distribution.h"
#include "../num/random.h"
#include "../prim/aabb_2d.h"
#include "../prim/aabb_3d.h"
#include "../prim/triangle_2d.h"
#include "../prim/sphere_3d.h"

namespace lass
{
namespace test
{
namespace aabb_tree
{

template <typename T, typename RandomGenerator>
void generateObjects(const prim::Aabb2D<T>& iBound,
					 RandomGenerator& iRandom,
					 size_t iNumber,
					 prim::Triangle2D<T>* oObjects)					 
{
	typedef prim::Point2D<T> TPoint;
	for (size_t i = 0; i < iNumber; ++i)
	{
		const TPoint a = iBound.random(iRandom);
		const TPoint b = iBound.random(iRandom);
		const TPoint c = iBound.random(iRandom);
		oObjects[i] = prim::Triangle2D<T>(a, b, c);
	}
}


template <typename T, typename RandomGenerator>
void generateObjects(const prim::Aabb3D<T>& iBound,
					 RandomGenerator& iRandom,
					 size_t iNumber,
					 prim::Sphere3D<T>* oObjects)					 
{
	typedef prim::Point3D<T> TPoint;
	typedef prim::Vector3D<T> TVector;
	for (size_t i = 0; i < iNumber; ++i)
	{
		const TPoint center = iBound.random(iRandom);
		const TVector centerToBound = prim::pointwiseMin(
			center - iBound.min(), iBound.max() - center);
		const T maxRadius = std::min(centerToBound.x, centerToBound.y);
		const T radius = num::distributeUniform(iRandom, T(0), maxRadius);
		oObjects[i] = prim::Sphere3D<T>(center, radius);
	}
}

}

template <typename T, size_t dim>
void testSpatAabbTree()
{
	const T extent = T(1000);
	const size_t numberOfObjects = 1000;
	const size_t numberOfContainTests = 100; 

	// set bounds
	//
	typedef typename meta::Select
	<
		dim == 2,
		prim::Aabb2D<T>,
		prim::Aabb3D<T>
	>::Type TAabb;
	typedef typename TAabb::TPoint TPoint;
	TPoint min;
	TPoint max;
	for (size_t i = 0; i < TPoint::dimension; ++i)
	{
		min[i] = -extent;
		max[i] = extent;
	}
	const TAabb bounds(min, max);

	// random generator
	//
	num::RandomMT19937 generator;

	// create test subjects
	//
	typedef typename meta::Select
	<
		dim == 2,
		prim::Triangle2D<T>,
		prim::Sphere3D<T>
	>::Type TObject;
	TObject objects[numberOfObjects];
	aabb_tree::generateObjects(bounds, generator, numberOfObjects, objects);

	// create tree
	//
	typedef spat::AabbTree<TObject, spat::AabbTreeObjectTraits<TObject, TAabb> > TAabbTree;
	TAabbTree tree(objects, objects + numberOfObjects);

	// contain test
	//
	for (unsigned i = 0; i < numberOfContainTests; ++i)
	{
		TPoint target = bounds.random(generator);

		bool naiveContain = false;
		for (unsigned k = 1; k < numberOfObjects && !naiveContain; ++k)
		{
			naiveContain |= objects[k].contains(target);
		}

		bool treeContain = tree.contains(target);

		BOOST_CHECK_EQUAL(naiveContain, treeContain);
	}
}



}

}

#endif

// EOF
