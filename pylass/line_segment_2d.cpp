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
#include "line_segment_2d.h"

namespace quad_tree
{


LineSegment2D::LineSegment2D(const TPoint& iHead, const TPoint& iTail, const TPyObjectPtr& iValue):
	segment_( TLineSegment(iHead, iTail) ),
	value_(iValue)
{
}

LineSegment2D::~LineSegment2D()
{
}

const LineSegment2D::TPoint& LineSegment2D::head() const
{
	return segment_.head();
}

const LineSegment2D::TPoint& LineSegment2D::tail() const
{
	return segment_.tail();
}

const LineSegment2D::TLineSegment& LineSegment2D::segment() const
{
	return segment_;
}

const TPyObjectPtr& LineSegment2D::value() const
{
	return value_;
}

}
