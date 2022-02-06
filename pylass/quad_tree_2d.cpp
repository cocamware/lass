/** @file
 *  @author Bram de Greve (bramz@users.sourceforge.net)
 *  @author Tom De Muer (tomdemuer@users.sourceforge.net)
 *
 *  Distributed under the terms of the GPL (GNU Public License)
 *
 *  The LASS License:
 *
 *  Copyright 2004-2021 Bram de Greve and Tom De Muer
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

#include "pylass_common.h"
#include "quad_tree_2d.h"

namespace quad_tree
{

PY_DECLARE_CLASS(QuadTree2D)
PY_CLASS_CONSTRUCTOR_1(QuadTree2D, QuadTree2D::TSegments)
PY_CLASS_METHOD(QuadTree2D, nearestNeighbour)
PY_CLASS_METHOD(QuadTree2D, rangeSearch)
PY_CLASS_METHOD(QuadTree2D, objectCount)
PY_CLASS_METHOD(QuadTree2D, aabb)
PY_CLASS_METHOD(QuadTree2D, depth)
PY_CLASS_METHOD(QuadTree2D, averageDepth)

PY_DECLARE_CLASS(LineSegment2D)
PY_CLASS_INNER_CLASS( QuadTree2D, LineSegment2D )
PY_CLASS_CONSTRUCTOR_3(LineSegment2D, const LineSegment2D::TPoint&, const LineSegment2D::TPoint&, const TPyObjectPtr&)
PY_CLASS_MEMBER_R(LineSegment2D, tail)
PY_CLASS_MEMBER_R(LineSegment2D, head)
PY_CLASS_MEMBER_R(LineSegment2D, value)


QuadTree2D::QuadTree2D(const TSegments& iSegments):
	segments_(iSegments)
{
	tree_.reset(segments_.begin(), segments_.end());
}

std::pair<double, TLineSegment2DPtr> QuadTree2D::nearestNeighbour(const TPoint& iTarget)
{
	const TQuadTree::Neighbour nearest = tree_.nearestNeighbour(iTarget);
	if (nearest.object() == segments_.end())
	{
		LASS_THROW("no nearest neighbour for " << iTarget << " found.  Empty QuadTree?");
	}
	return std::make_pair(nearest.squaredDistance(), *nearest.object());
}

std::vector<std::pair<double, TLineSegment2DPtr> >
QuadTree2D::rangeSearch( const TPoint& target, double maxRadius, size_t maxCount)
{
	std::vector<std::pair<double, TLineSegment2DPtr> > segments;
	std::vector<TQuadTree::Neighbour> treeNeighbours( maxCount + 1 );
	auto last = tree_.rangeSearch(target, maxRadius, maxCount, treeNeighbours.begin());

	for (auto it=treeNeighbours.begin(); it!=last;++it)
		segments.push_back( std::make_pair( it->squaredDistance(), *it->object() ) );
	return segments;
}

size_t QuadTree2D::objectCount()const
{
	return tree_.objectCount();
}

const QuadTree2D::TQuadTree::TAabb&
QuadTree2D::aabb() const
{
	return tree_.aabb();
}

size_t 
QuadTree2D::depth() const
{
	return tree_.depth();
}

QuadTree2D::TQuadTree::TValue
QuadTree2D::averageDepth() const
{
	return tree_.averageDepth();
}


}
