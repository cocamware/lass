/**
*   @author Bram de Greve (bramz@users.sourceforge.net)
*   @author Tom De Muer (tomdemuer@users.sourceforge.net)
*
*   Distributed under the terms of the GPL (GNU Public License)
*
*   The LASS License:
*
*   Copyright 2004 Bram de Greve and Tom De Muer
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



/** @namespace lass::gis
 *  @brief wrappers around GDAL to comply with prim, etc.
 */



#ifndef LASS_GUARDIAN_OF_INCLUSION_GIS_GIS_COMMON_H
#define LASS_GUARDIAN_OF_INCLUSION_GIS_GIS_COMMON_H
#pragma once



#include "../lass_common.h"

/** name of library
 */
#define LASS_LIB_GIS LASS_LIB_PREFIX "gis" LASS_LIB_SUFFIX
//#pragma message("LASS_LIB_GIS: " LASS_LIB_GIS)
#pragma comment(lib, LASS_LIB_GIS)



#endif
