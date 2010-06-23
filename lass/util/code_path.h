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



#ifndef LASS_GUARDIAN_OF_INCLUSION_UTIL_CODE_PATH_H
#define LASS_GUARDIAN_OF_INCLUSION_UTIL_CODE_PATH_H

#include "util_common.h"

#ifdef WIN32		// this is windows only for now

namespace lass
{
namespace util
{
	/* Rationale: use a meta-function that does profiling on two versions of the same
	*  functionality and eventually picks one.  This could be labeled dynamic optimizing,
	*  but the programmer has to develop the different incarnations of the algorithm.
	*
	*  A good use is for two different versions of already heavily optimized functions
	*  that perform differently on different (cache) architectures.
	*/


namespace impl
{
	__int64 cycleCount()
	{      
		__int64 cycles;
		_asm rdtsc; // won't work on 486 or below - only pentium or above
		_asm lea ebx,cycles;
		_asm mov [ebx],eax;
		_asm mov [ebx+4],edx;
		return cycles;
	}
}

template struct CodePath<typename SelectionPolicy>
{
	template <typename Function> 
	CodePath(Function iFunction1, Function iFunction2)
	{
		SelectionPolicy::initialize(iFunction1, iFunction2);
	}
};

}
}

#endif

#endif

// EOF
