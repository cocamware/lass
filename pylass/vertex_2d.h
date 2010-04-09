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

#ifndef KD_TREE_GUARDIAN_OF_INCLUSION_VERTEX_2D_H
#define KD_TREE_GUARDIAN_OF_INCLUSION_VERTEX_2D_H

#include "pylass_common.h"
#include <lass/prim/point_2d.h>

namespace kd_tree
{

using namespace lass;
typedef python::PyObjectPtr<PyObject>::Type TPyObjectPtr;

class Vertex2D: public python::PyObjectPlus
{
	PY_HEADER(python::PyObjectPlus)
public:

	typedef lass::prim::Point2D<double> TPoint;

	Vertex2D(const TPoint& iPosition, const TPyObjectPtr& iValue);
	~Vertex2D();

	const TPoint& position() const;
	const TPyObjectPtr& value() const;

private:

	TPoint position_;
	TPyObjectPtr value_;
};

typedef util::SharedPtr<Vertex2D, python::PyObjectStorage, python::PyObjectCounter> TVertex2DPtr;

}

#endif

// EOF
