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



#ifndef LASS_GUARDIAN_OF_INCLUSION_SPAT_WORLD_3D_OBJECT_3D_INL
#define LASS_GUARDIAN_OF_INCLUSION_SPAT_WORLD_3D_OBJECT_3D_INL

#include "../spat_common.h"
#include "object_3d.h"

namespace lass
{
namespace spat
{
namespace world_3d
{

// --- public --------------------------------------------------------------------------------------

/** Constructor.
 */
template <typename T>
Object3D<T>::Object3D(const TPoint& iPosition, THandle iHandle):
    position_(iPosition),
    handle_(iHandle)
{
}



/** Return 3D position of object.
 */
template <typename T>
const typename Object3D<T>::TPoint& 
Object3D<T>::position() const
{
    return position_;
}



/** Return handle of object.
 */
template <typename T>
const typename Object3D<T>::THandle 
Object3D<T>::handle() const
{
    return handle_;
}



// --- protected -----------------------------------------------------------------------------------



// --- private -------------------------------------------------------------------------------------



// --- free ----------------------------------------------------------------------------------------



}

}

}

#endif

// EOF
