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



#ifndef LASS_GUARDIAN_OF_INCLUSION_CONFIG_PLATFORM_BSD_H
#define LASS_GUARDIAN_OF_INCLUSION_CONFIG_PLATFORM_BSD_H

#define LASS_PLATFORM_TYPE LASS_PLATFORM_TYPE_BSD
#define LASS_LIB_PLATFORM "bsd"

#define LASS_PLATFORM_BSD_STR(x) LASS_PLATFORM_BSD_STR2(x)
#define LASS_PLATFORM_BSD_STR2(x) #x

#ifdef __FreeBSD__
#	define LASS_PLATFORM "FreeBSD" LASS_PLATFORM_BSD_STR(__FreeBSD__)
#else
#	define LASS_PLATFORM "bsd"
#endif

#define LASS_LITTLE_ENDIAN  1
#define LASS_BIG_ENDIAN 0

#define LASS_UTIL_THREAD_HAVE_POSIX
#define LASS_NUM_BASIC_OPS_USE_BUILTIN_LONG_DOUBLE

namespace lass
{

const int bitsPerByte = 8;

}

#endif