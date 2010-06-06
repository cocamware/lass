/**	@file
 *	@author Bram de Greve (bram@cocamware.com)
 *	@author Tom De Muer (tom@cocamware.com)
 *
 *	*** BEGIN LICENSE INFORMATION ***
 *	
 *	The contents of this file are subject to the Common Public Attribution License 
 *	Version 1.0 (the "License"); you may not use this file except in compliance with 
 *	the License. You may obtain a copy of the License at 
 *	http://lass.sourceforge.net/cpal-license. The License is based on the 
 *	Mozilla Public License Version 1.1 but Sections 14 and 15 have been added to cover 
 *	use of software over a computer network and provide for limited attribution for 
 *	the Original Developer. In addition, Exhibit A has been modified to be consistent 
 *	with Exhibit B.
 *	
 *	Software distributed under the License is distributed on an "AS IS" basis, WITHOUT 
 *	WARRANTY OF ANY KIND, either express or implied. See the License for the specific 
 *	language governing rights and limitations under the License.
 *	
 *	The Original Code is LASS - Library of Assembled Shared Sources.
 *	
 *	The Initial Developer of the Original Code is Bram de Greve and Tom De Muer.
 *	The Original Developer is the Initial Developer.
 *	
 *	All portions of the code written by the Initial Developer are:
 *	Copyright (C) 2004-2010 the Initial Developer.
 *	All Rights Reserved.
 *	
 *	Contributor(s):
 *
 *	Alternatively, the contents of this file may be used under the terms of the 
 *	GNU General Public License Version 2 or later (the GPL), in which case the 
 *	provisions of GPL are applicable instead of those above.  If you wish to allow use
 *	of your version of this file only under the terms of the GPL and not to allow 
 *	others to use your version of this file under the CPAL, indicate your decision by 
 *	deleting the provisions above and replace them with the notice and other 
 *	provisions required by the GPL License. If you do not delete the provisions above,
 *	a recipient may use your version of this file under either the CPAL or the GPL.
 *	
 *	*** END LICENSE INFORMATION ***
 */



#ifndef LASS_GUARDIAN_OF_INCLUSION_CONFIG_PLATFORMS_H
#define LASS_GUARDIAN_OF_INCLUSION_CONFIG_PLATFORMS_H

#define LASS_PLATFORM_TYPE_WIN32 1
#define LASS_PLATFORM_TYPE_LINUX 2
#define LASS_PLATFORM_TYPE_CYGWIN 3
#define LASS_PLATFORM_TYPE_BSD 4
#define LASS_PLATFORM_TYPE_SOLARIS 5 

#if defined(linux) || defined(__linux) || defined(__linux__)
#	include "platform_linux.h"
#elif defined(__CYGWIN__)
#	include "platform_cygwin.h"
#elif defined(_WIN32) || defined(__WIN32__) || defined(WIN32)
#	include "platform_win32.h"
#elif defined(__FreeBSD__)
#	include "platform_bsd.h"
#elif defined(__sun) && defined(__SVR4)
#	include "platform_solaris.h"
#else
#	error "Unknown platform - please configure and report the results to the LASS team"
#endif



#if defined(_M_IA64) || defined(_M_X64) || defined(_LP64) || defined(__LP64__) || defined(__x86_64)
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
