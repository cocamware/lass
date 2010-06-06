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


#ifndef LASS_GUARDIAN_OF_INCLUSION_UTIL_IMPL_ENVIRONMENT_H
#define LASS_GUARDIAN_OF_INCLUSION_UTIL_IMPL_ENVIRONMENT_H

#include "util_common.h"

namespace lass
{
namespace util
{
namespace impl
{

LASS_DLL const std::string LASS_CALL lass_getenv(const std::string& iName);
LASS_DLL void LASS_CALL lass_putenv(const std::string& iName, const std::string& iValue);

template <typename T>
struct Environment
{
	static const T get(const std::string& iName)
	{
		return stringCast<T>(lass_getenv(iName));
	}
	static void put(const std::string& iName, typename CallTraits<T>::TParam iValue)
	{
		lass_putenv(iName, stringCast<std::string>(iValue));
	}
};

template <>
struct Environment<std::string>
{
	static const std::string get(const std::string& iName)
	{
		return lass_getenv(iName);
	}
	static void put(const std::string& iName, const std::string& iValue)
	{
		lass_putenv(iName, iValue);
	}
};

}

template <typename T>
const T getEnvironment(const std::string& iName)
{	
	return impl::Environment<typename CallTraits<T>::TValue>::get(iName);
}

template <typename T>
void putEnvironment(const std::string& iName, const T& iValue)
{
	impl::Environment<typename CallTraits<T>::TValue>::put(iName, iValue);
}

}
}

#endif

// EOF
