/** 
*   @author Bram de Greve (bramz@users.sourceforge.net)
*   @author Tom De Muer (tomdemuer@users.sourceforge.net)
*
*	Distributed under the terms of the GPL (GNU Public License)
*
* 	The LASS License:
*   
*	Copyright 2004 Bram de Greve and Tom De Muer
*
*   LASS is free software; you can redistribute it and/or modify
*   it under the terms of the GNU General Public License as published by
*   the Free Software Foundation; either version 2 of the License, or
*   (at your option) any later version.
*
*   This program is distributed in the hope that it will be useful,
*   but WITHOUT ANY WARRANTY; without even the implied warranty of
*   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
*   GNU General Public License for more details.
*
*   You should have received a copy of the GNU General Public License
*   along with this program; if not, write to the Free Software
*   Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
*
*/
// -----------------------------------------------------------------------------
/** @file vertex_3d.h
 *  @brief interface of class lass::spat::world_3d::Vertex3D
 *  @author Bram de Greve --- BDG ---
 *  @date 2002-2003
 */
// -----------------------------------------------------------------------------



/** @class lass::spat::world_3d::Vertex3D
 *  @brief A 3D Vertex
 *  @author Bram de Greve --- BDG ---
 *  @date 2002-2003
 */



#ifndef LASS_GUARDIAN_OF_INCLUSION_SPAT_WORLD_3D_VERTEX3_H
#define LASS_GUARDIAN_OF_INCLUSION_SPAT_WORLD_3D_VERTEX3_H



// --- OLD INTERFACES ----------------------------------------------------------

#include "../spat_common.h"
#include "../../prim/point_3d.h"
#include "../../util/non_copyable.h"


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

	typedef prim::Point3D<T> TPoint;

    // STRUCTORS

    Vertex3D(const TPoint& a_position);


    // METHODS

    const TPoint& position() const;

private:

    // DATA

    TPoint m_position;
};



}

}

}

#include "vertex_3d.inl"

#endif // guardian of inclusion

// --- END OF FILE vertex_3d.h -------------------------------------------------
