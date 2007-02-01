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



#ifndef LASS_GUARDIAN_OF_INCLUSION_CONFIG_PLATFORMS_H
#define LASS_GUARDIAN_OF_INCLUSION_CONFIG_PLATFORMS_H

#define LASS_PLATFORM_TYPE_WIN32 1
#define LASS_PLATFORM_TYPE_LINUX 2
#define LASS_PLATFORM_TYPE_CYGWIN 3
#define LASS_PLATFORM_TYPE_BSD 4

#if defined(linux) || defined(__linux) || defined(__linux__)
#	include "platform_linux.h"
#elif defined(__CYGWIN__)
#	include "platform_cygwin.h"
#elif defined(_WIN32) || defined(__WIN32__) || defined(WIN32)
#	include "platform_win32.h"
#elif defined(__FreeBSD__)
#	include "platform_bsd.h"
#else
#	error "Unknown platform - please configure and report the results to the LASS team"
#endif



#if defined(_M_IA64) || defined(_LP64) || defined(__LP64__)
#	define LASS_ADDRESS_SIZE 64
	// HACK: Currently, we assume that all 64 bit platforms actually only use 48 bits
#	define LASS_ACTUAL_ADDRESS_SIZE 48
#else
#	define LASS_ADDRESS_SIZE 32
#endif

#if !defined(LASS_ACTUAL_ADDRESS_SIZE)
#	define LASS_ACTUAL_ADDRESS_SIZE LASS_ADDRESS_SIZE
#endif



#ifndef LASS_LIB_PLATFORM
#	define LASS_LIB_PLATFORM "default"
#endif

#endif
