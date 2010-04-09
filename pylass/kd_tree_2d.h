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

#ifndef KD_TREE_GUARDIAN_OF_INCLUSION_KD_TREE_2D_H
#define KD_TREE_GUARDIAN_OF_INCLUSION_KD_TREE_2D_H

#include "pylass_common.h"
#include "vertex_2d.h"
#include <lass/spat/kd_tree.h>

namespace kd_tree
{

	using namespace lass;

struct Vertex2DTraits
{
	typedef std::vector<TVertex2DPtr>::const_iterator TObjectIterator;
	typedef TVertex2DPtr::TReference TObjectReference;
	typedef TVertex2DPtr::TPointee TVertex;
	typedef TVertex::TPoint TPoint;
    typedef TPoint::TValue TValue;
    typedef TPoint::TParam TParam;
    typedef TPoint::TReference TReference;
    typedef TPoint::TConstReference TConstReference;
    enum { dimension = TPoint::dimension };

    static const TPoint& position(const TObjectIterator iObject) { return (*iObject)->position(); }
};

class KdTree2D: public python::PyObjectPlus
{
	PY_HEADER(python::PyObjectPlus)
public:
	typedef Vertex2D::TPoint TPoint;
	typedef std::vector<TVertex2DPtr> TVertices;
	typedef spat::KdTree<TVertex2DPtr, Vertex2DTraits> TTree;

	KdTree2D(const TVertices& iVertices);
	TVertex2DPtr nearestNeighbour(const TPoint& iTarget);
	TVertices rangeSearch(const Vertex2D::TPoint& iCenter,  TTree::TParam iMaxRadius, size_t iMaxCount) const;

private:

	TVertices vertices_;
	TTree tree_;
};

}

#endif

// EOF
