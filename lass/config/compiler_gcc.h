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
 *	Copyright (C) 2004-2024 the Initial Developer.
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



#ifndef LASS_GUARDIAN_OF_INCLUSION_CONFIG_COMPILER_GCC_H
#define LASS_GUARDIAN_OF_INCLUSION_CONFIG_COMPILER_GCC_H

#define LASS_COMPILER_VERSION (__GNUC__ * 10000 + __GNUC_MINOR__ * 100 + __GNUC_PATCHLEVEL__)

#define LASS_COMPILER "GNU C++ version " __VERSION__
#define LASS_LIB_COMPILER "gcc" LASS_STRINGIFY(__GNUC__) LASS_STRINGIFY(__GNUC_MINOR__) LASS_STRINGIFY(__GNUC_PATCHLEVEL__)

// use GCC style inline assembly
#define LASS_HAVE_INLINE_ASSEMBLY_GCC 1

#if !defined(__CHAR_UNSIGNED__)
#	define LASS_CHAR_IS_SIGNED 1
#endif

#define LASS_DLL_IMPORT
#define LASS_DLL_EXPORT
#define LASS_DLL_LOCAL __attribute__ ((visibility ("hidden")))
#define LASS_CALL
#define LASS_NO_INLINE
#define LASS_PER_THREAD __thread
#define LASS_ALIGN(n) __attribute__((aligned(n)))
#define LASS_ALIGNED(x, n) x LASS_ALIGN(n)

#endif
