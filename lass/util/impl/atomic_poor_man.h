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

#define LASS_UTIL_ATOMIC_HAVE_POOR_MANS_IMPL

namespace lass
{
namespace util
{
namespace impl
{

class LASS_DLL PoorMansGlobalAtomicLock
{
public:
	PoorMansGlobalAtomicLock();
	~PoorMansGlobalAtomicLock();
};

template <size_t byteSize>
struct AtomicOperations
{
	LASS_META_ASSERT(byteSize == 1 || byteSize == 2 || byteSize == 4 || byteSize == 8, bytesize_should_be_1_2_4_or_8);
	
	template <typename T>
	static bool compareAndSwap(volatile T& dest, T expectedValue, T newValue)
	{
		PoorMansGlobalAtomicLock LASS_UNUSED(lock);
		const T old = dest;
		if (old != expectedValue)
		{
			return false;
		}
		dest = newValue;
		return true;
	}

	template <typename T1, typename T2> inline 
	static bool LASS_CALL compareAndSwap(
			volatile T1& dest1, T1 expected1, T2 expected2, T1 new1, T2 new2)
	{
		PoorMansGlobalAtomicLock LASS_UNUSED(lock);
		volatile T2& dest2 = *reinterpret_cast<volatile T2*>((&dest1) + 1);
		if (dest1 != expected1 || dest2 != expected2)
		{
			return false;
		}
		dest1 = new1;
		dest2 = new2;
		return true;
	}

	template <typename T> inline
	static void LASS_CALL increment(volatile T& value)
	{
		PoorMansGlobalAtomicLock LASS_UNUSED(lock);
		++value;
	}

	template <typename T> inline
	static void LASS_CALL decrement(volatile T& value)
	{
		PoorMansGlobalAtomicLock LASS_UNUSED(lock);
		--value;
	}
};

}
}
}

// EOF
