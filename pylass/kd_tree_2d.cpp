/** @file
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

#include "pylass_common.h"
#include "kd_tree_2d.h"

namespace kd_tree
{

PY_DECLARE_CLASS(KdTree2D)
PY_CLASS_CONSTRUCTOR_1(KdTree2D, KdTree2D::TVertices)
PY_CLASS_METHOD(KdTree2D, nearestNeighbour)
PY_CLASS_METHOD(KdTree2D, rangeSearch)

PY_DECLARE_CLASS(Vertex2D)
PY_CLASS_INNER_CLASS( KdTree2D, Vertex2D )
PY_CLASS_CONSTRUCTOR_2(Vertex2D, const Vertex2D::TPoint&, const TPyObjectPtr&)
PY_CLASS_MEMBER_R(Vertex2D, position)
PY_CLASS_MEMBER_R(Vertex2D, value)


KdTree2D::KdTree2D(const TVertices& iVertices):
	vertices_(iVertices)
{
	tree_.reset(vertices_.begin(), vertices_.end());
}

TVertex2DPtr KdTree2D::nearestNeighbour(const TPoint& iTarget)
{
	const TTree::Neighbour nearest = tree_.nearestNeighbour(iTarget);
	if (nearest.object() == vertices_.end())
	{
		LASS_THROW("no nearest neighbour for " << iTarget << " found.  Empty KdTree?");
	}
	return *nearest.object();	
}

KdTree2D::TVertices KdTree2D::rangeSearch(const Vertex2D::TPoint& iCenter, TTree::TParam iMaxRadius, size_t iMaxCount) const
{
	TTree::TNeighbourhood tempNeighbourhood;
	TVertices tempVertices;
	TTree::TValue ret = tree_.rangeSearch(iCenter, iMaxRadius, iMaxCount, tempNeighbourhood);

	for (TTree::TNeighbourhood::const_iterator it=tempNeighbourhood.begin();
		it!=tempNeighbourhood.end();++it)
		tempVertices.push_back(*it->object());
	return tempVertices;
}



}
