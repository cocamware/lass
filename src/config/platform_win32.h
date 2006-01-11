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



#ifndef LASS_GUARDIAN_OF_INCLUSION_CONFIG_PLATFORM_WIN32_H
#define LASS_GUARDIAN_OF_INCLUSION_CONFIG_PLATFORM_WIN32_H


#define LASS_PLATFORM_TYPE LASS_PLATFORM_TYPE_WIN32
#define LASS_PLATFORM "Win32"
#define LASS_LIB_PLATFORM "win32"

#define LASS_LITTLE_ENDIAN  1
#define LASS_BIG_ENDIAN 0

#define LASS_UTIL_THREAD_HAVE_WIN32

#pragma message("[LASS BUILD MSG] Build on MS Windows requires Windows 2000 or higher!")
#define _WIN32_WINNT 0x500

//
// disable min/max macros:
//
#ifdef min
#  undef min
#endif
#ifdef max
#  undef max
#endif
#ifndef NOMINMAX
#  define NOMINMAX
#endif



namespace lass
{

const int bitsPerByte = 8;

}

#endif
