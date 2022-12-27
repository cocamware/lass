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
 *	Copyright (C) 2004-2022 the Initial Developer.
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

#ifndef LASS_GUARDIAN_OF_INCLUSION_CONFIG_ARCHITECTURE_H
#define LASS_GUARDIAN_OF_INCLUSION_CONFIG_ARCHITECTURE_H

#include "compilers.h"

/*
 *  Finds out for which processor architecture the code is generated
 *  for.
 *     Supported definitions:
 *     i386, ARM
 */

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


#if LASS_COMPILER_TYPE == LASS_COMPILER_TYPE_GCC || LASS_COMPILER_TYPE == LASS_COMPILER_TYPE_CLANG
#	if defined(__ARM_EABI__)
#		define LASS_PROCESSOR_ARCHITECTURE_ARM
#	else
#		define LASS_PROCESSOR_ARCHITECTURE_x86
#	endif
#else
    // for now assume that non-gcc compilers will be used to generate i386 compatible code
#	define LASS_PROCESSOR_ARCHITECTURE_x86
#endif

#if defined(LASS_PROCESSOR_ARCHITECTURE_x86)
#	define LASS_PROCESSOR_ARCHITECTURE "x86"
#elif defined(LASS_PROCESSOR_ARCHITECTURE_ARM)
#	define LASS_PROCESSOR_ARCHITECTURE "arm"
#else
#	define LASS_PROCESSOR_ARCHITECTURE "unknown"
#endif

#endif

// EOF
