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



/** @namespace lass::num
 *  numeric types and traits.
 */


#ifndef LASS_GUARDIAN_OF_INCLUSION_NUM_COMMON_H
#define LASS_GUARDIAN_OF_INCLUSION_NUM_COMMON_H

#include "../lass_common.h"

/** name of library
 */
#define LASS_LIB_NUM LASS_LIB_PREFIX "num" LASS_LIB_SUFFIX
//#pragma message("LASS_LIB_NUM: " LASS_LIB_NUM)
#pragma comment(lib, LASS_LIB_NUM)


#include "basic_ops.h"
#include "stringify.h"

namespace lass
{

namespace num
{

#if _MSC_VER
	typedef __int8 Tint8;
	typedef unsigned __int8 Tuint8;
	typedef __int16 Tint16;
	typedef unsigned __int16 Tuint16;
	typedef __int32 Tint32;
	typedef unsigned __int32 Tuint32;
	typedef __int64 Tint64;
	typedef unsigned __int64 Tuint64;
#else
	typedef signed char Tint8;
	typedef unsigned char Tuint8;
	typedef signed short Tint16;
	typedef unsigned short Tuint16;
	typedef signed int Tint32;
	typedef unsigned int Tuint32;
	typedef signed long long Tint64;
	typedef unsigned long long Tuint64;
#endif

}

}


#endif
