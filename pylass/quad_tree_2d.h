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

#ifndef KD_TREE_GUARDIAN_OF_INCLUSION_QUAD_TREE_2D_H
#define KD_TREE_GUARDIAN_OF_INCLUSION_QUAD_TREE_2D_H

#include "pylass_common.h"
#include "vertex_2d.h"
#include "line_segment_2d.h"
#include <lass/prim/aabb_2d.h>
#include <lass/prim/ray_2d.h>
#include <lass/spat/quad_tree.h>
#include <lass/spat/split_heuristics.h>

namespace quad_tree
{

	using namespace lass;


class LineSegment2DTraits
{
public:
	typedef std::vector<TLineSegment2DPtr>::const_iterator TObjectIterator;
	typedef TLineSegment2DPtr::TReference TObjectReference;
	typedef TLineSegment2DPtr::TPointee TLineSegment2D;
	typedef TLineSegment2D::TLineSegment::TPoint TPoint;
    typedef TLineSegment2D::TLineSegment::TValue TValue;
    typedef TLineSegment2D::TLineSegment::TParam TParam;
    typedef TLineSegment2D::TLineSegment::TReference TReference;
    typedef TLineSegment2D::TLineSegment::TConstReference TConstReference;
    enum { dimension = TPoint::dimension };

	typedef prim::Aabb2D<TPoint::TValue> TAabb;
	typedef prim::Vector2D<TPoint::TValue> TVector;
	typedef prim::Ray2D<TPoint::TValue> TRay;
	typedef TPyObjectPtr TInfo;

	static TAabb aabbEmpty() { TAabb temp; temp.clear(); return temp; }
	static TAabb aabbJoin( const TAabb& a, const TAabb& b ) { return a+b; }
	static TAabb objectAabb( TObjectIterator i ) { TAabb temp( (*i)->head() ); temp += (*i)->tail(); return temp; }
	static const TPoint& aabbMin( const TAabb& a ) { return a.min(); }
	static const TPoint& aabbMax( const TAabb& a ) { return a.max(); }
	static TAabb aabbMake(const TPoint& a, const TPoint& b) { return TAabb(a,b); }
	static bool aabbContains(const TAabb& a, const TAabb& b ) { return a.contains(b); }
	static TValue objectSquaredDistance(TObjectIterator i, const TPoint& p, const TInfo* /*info*/)  {return lass::prim::distance( (*i)->segment(), p ); }
	static void coord( TPoint& p, size_t which, TValue newValue ) { p[which] = newValue; }
	static TValue coord( const TPoint& p, size_t which )  { return p[which]; }

	static bool objectIntersects(TObjectIterator i, const TAabb& a, const TInfo* /*info*/) 
	{ 
		if (a.contains( (*i)->head() ) ) 
			return true;
		if (a.contains( (*i)->tail() ) ) 
			return true;

		bool intersects = false;
		TPoint lowLeft( a.min().x, a.min().y );
		TPoint lowRight( a.max().x, a.min().y );
		TPoint upLeft( a.min().x, a.max().y );
		TPoint upRight( a.max().x, a.max().y );

		TLineSegment2D::TLineSegment segment1( lowLeft, lowRight );
		intersects = lass::prim::intersects( (*i)->segment(), segment1 );
		if (intersects)
			return true;

		TLineSegment2D::TLineSegment segment2( lowRight, upRight );
		intersects |= lass::prim::intersects( (*i)->segment(), segment2 );
		if (intersects)
			return true;

		TLineSegment2D::TLineSegment segment3( upRight, upLeft);
		intersects |= lass::prim::intersects( (*i)->segment(), segment3 );
		if (intersects)
			return true;

		TLineSegment2D::TLineSegment segment4( upLeft, lowLeft);
		intersects |= lass::prim::intersects( (*i)->segment(), segment4 );
		return intersects;
	}
};

class QuadTree2D: public python::PyObjectPlus
{
	PY_HEADER(python::PyObjectPlus)
public:
	typedef LineSegment2D::TPoint TPoint;
	typedef std::vector<TLineSegment2DPtr> TSegments;
	typedef spat::QuadTree<LineSegment2D, LineSegment2DTraits, spat::DefaultSplitHeuristics> TQuadTree;

	QuadTree2D(const TSegments& iSegments);
	std::pair<double, TLineSegment2DPtr> nearestNeighbour(const TPoint& iTarget);
	std::vector<std::pair<double, TLineSegment2DPtr> >	rangeSearch( const TPoint& target, double maxRadius, size_t maxCount);

	size_t objectCount() const;
	const TQuadTree::TAabb& aabb() const;

	/** depth. Returns the depth of the tree */
	size_t depth() const;
	TQuadTree::TValue averageDepth() const;

private:

	TSegments segments_;
	TQuadTree tree_;
};

}

#endif

// EOF
