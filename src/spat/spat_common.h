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



/** @namespace lass::spat
 *	@brief spatial subdivisions, quadtrees, octrees, meshes in 2D and 3D, triangulators, ...
 *
 *  @section overview
 *
 *  @ref lass::spat::world_3d:: the templated version of the original @c World3 model in @c bass3
 */



#ifndef LASS_GUARDIAN_OF_INCLUSION_SPAT_SPAT_COMMON_H
#define LASS_GUARDIAN_OF_INCLUSION_SPAT_SPAT_COMMON_H




#include "../lass_common.h"

/** name of library
 */
#define LASS_LIB_SPAT LASS_LIB_PREFIX "spat" LASS_LIB_SUFFIX
//#pragma message("LASS_LIB_SPAT: " LASS_LIB_SPAT)

#if !defined(LASS_LIB_NO_AUTOMATIC_LINK)
#   pragma comment(lib, LASS_LIB_SPAT)
#endif


#endif

