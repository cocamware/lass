/**	@file
 *	@author Bram de Greve (bramz@users.sourceforge.net)
 *	@author Tom De Muer (tomdemuer@users.sourceforge.net)
 *
 *	Distributed under the terms of the GPL (GNU Public License)
 *
 * 	The LASS License:
 *
 *	Copyright 2004 Bram de Greve and Tom De Muer
 *
 *	LASS is free software; you can redistribute it and/or modify
 *	it under the terms of the GNU General Public License as published by
 *	the Free Software Foundation; either version 2 of the License, or
 *	(at your option) any later version.
 *
 *	This program is distributed in the hope that it will be useful,
 *	but WITHOUT ANY WARRANTY; without even the implied warranty of
 *	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *	GNU General Public License for more details.
 *
 *	You should have received a copy of the GNU General Public License
 *	along with this program; if not, write to the Free Software
 *	Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */



/** @class lass::spat::world_3d::Vertex3D
 *  @brief A 3D Vertex
 *  @author Bram de Greve --- BDG ---
 *  @date 2002-2003
 */

#ifndef LASS_GUARDIAN_OF_INCLUSION_SPAT_WORLD_3D_VERTEX3_H
#define LASS_GUARDIAN_OF_INCLUSION_SPAT_WORLD_3D_VERTEX3_H

#include "../spat_common.h"
#include "../../prim/point_3d.h"

namespace lass
{
namespace spat
{
namespace world_3d
{

template <typename T>
class Vertex3D: public util::NonCopyable
{
public:

	typedef Vector3D<T> TSelf;

	typedef prim::Point3D<T> TPoint;

    Vertex3D(const TPoint& iPosition);

    const TPoint& position() const;

private:

    TPoint position_;
};



}

}

}

#include "vertex_3d.inl"

#endif

// EOF

