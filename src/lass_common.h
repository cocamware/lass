/** @file
 *  @author Bram de Greve (bramz@users.sourceforge.net)
 *  @author Tom De Muer (tomdemuer@users.sourceforge.net)
 *
 *  Distributed under the terms of the GPL (GNU Public License)
 *
 *  The LASS License:
 *
 *  Copyright 2004 Bram de Greve and Tom De Muer
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



/** @namespace lass
 *  @brief Library for Assembled Shared Sources
 */



/** @mainpage lass: Library for Assembled Shared Sources
 *  @author Bram de Greve (bramz@users.sourceforge.net)
 *  @author Tom De Muer (tomdemuer@users.sourceforge.net)
 *
 *  - lass::frb: fuzzy rule bases and systems for inference
 *  - lass::gis: wrappers around GDAL to comply with prim, etc.
 *  - lass::io: streams, binary streams, vrmlstreams, ...
 *  - lass::meta: a meta programming library for your cunning plans.
 *  - lass::num: numeric primitives and functions
 *  - lass::prim: geometric primitives and basic functionality
 *  - lass::spat: spatial subdivisions, quadtrees, octrees, meshes in 2D and 3D, triangulators, ...
 *  - lass::stde: std extensions, ...
 *  - lass::util: general utility, debug facilities, ...
 *
 *  Distributed under the terms of the GPL (GNU Public License)
 *
 *  The LASS License:
 *
 *  Copyright 2004 Bram de Greve and Tom De Muer
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



#ifndef LASS_GUARDIAN_OF_INCLUSION_LASS_COMMON_H
#define LASS_GUARDIAN_OF_INCLUSION_LASS_COMMON_H

#include "config/config.h"

// --- Here we'll define some stuff on the name of libraries lass creates ---

/** @def LASS_LIB_DEBUG
 *  @brief part of the library name that indicates if it is a release or debug build.
 */
#ifdef _DEBUG
#	define LASS_LIB_DEBUG "_d"
#else
#	define LASS_LIB_DEBUG ""
#endif

/** @def LASS_LIB_PREFIX
 *  @brief prefix of all library names that will be made
 */
#define LASS_LIB_PREFIX "lass_"

/** @def LASS_LIB_SUFFIX
 *  @brief suffix of all library names that will be made, indicates platform, compiler and build.
 */
#define LASS_LIB_SUFFIX "_" LASS_LIB_PLATFORM "_" LASS_LIB_COMPILER LASS_LIB_DEBUG ".lib"

/** @def LASS_DLL
 *  @brief DLL interface: import or export symbols?  or neither?
 */
#if defined(LASS_BUILD_DLL)
#	define LASS_DLL LASS_DLL_EXPORT
#elif defined(LASS_USE_DLL)
#	define LASS_DLL LASS_DLL_IMPORT
#else
#	define LASS_DLL LASS_DLL_EXPORT
#endif

/** @def LASS_LIB_AUTO_LINK
 *  @brief if defined, try to automatically link the static libraries
 */
#if defined(LASS_LIB_AUTO_LINK)
#	undef LASS_LIB_AUTO_LINK
#endif
#if LASS_COMPILER_TYPE == LASS_COMPILER_TYPE_MSVC || LASS_COMPILER_TYPE == LASS_COMPILER_TYPE_INTEL
#	if !defined(LASS_USE_DLL) && !defined(LASS_BUILD_DLL)
#		define LASS_LIB_AUTO_LINK
#	endif
#	if defined(LASS_USE_DLL)
#		pragma comment(lib, LASS_LIB_PREFIX LASS_LIB_PLATFORM "_" LASS_LIB_COMPILER LASS_LIB_DEBUG ".lib")
#	endif
#endif



// --- frequently used STL mumbo jumbo ---

#include <typeinfo>
#include <exception>
#include <stdexcept>

#include <utility>
#include <functional>
#include <memory>

#include <string>

#include <locale>

#include <vector>
#include <list>
#include <deque>
#include <map>
#include <set>

#include <iterator>

#include <algorithm>

#include <complex>
#include <numeric>

#include <iostream>
#include <fstream>
#include <sstream>
#include <iomanip>



// --- common lass stuff ---

#include "util/common_macros.h"
#include "util/exception.h"
#include "util/enforcer.h"
#include "io/io_fwd.h"
#include "io/proxy_man.h"
#include "stde/extended_io.h"



// --- the name of the game ---

//#pragma LASS_NOTE("LASS_PLATFORM: " LASS_PLATFORM)
//#pragma LASS_NOTE("LASS_COMPILER: " LASS_COMPILER)


#endif
