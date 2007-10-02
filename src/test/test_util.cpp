/**	@file
 *	@author Bram de Greve (bramz@users.sourceforge.net)
 *	@author Tom De Muer (tomdemuer@users.sourceforge.net)
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
 *	Copyright (C) 2004-2007 the Initial Developer.
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



#include "test_common.h"
#include "test_util.h"

#include "test_util_atomic.inl"
#include "test_util_bind.inl"
#include "test_util_callback.inl"
#include "test_util_clock.inl"
#include "test_util_clone_factory.inl"
#include "test_util_dictionary.inl"
#include "test_util_exception.inl"
#include "test_util_fixed_array.inl"
#include "test_util_id_generator.inl"
#include "test_util_python.inl"
#include "test_util_string_cast.inl"
#include "test_util_thread_fun.inl"
#include "test_util_thread_pool.inl"
//#include "test_util_visitor.inl"

namespace lass
{
namespace test
{

TUnitTests testUtil()
{
	TUnitTests result;

	result.push_back(LASS_UNIT_TEST(testUtilAtomic));

	result.push_back(LASS_UNIT_TEST(testUtilBind));

	result.push_back(LASS_UNIT_TEST(testUtilPython));
	
	result.push_back(LASS_UNIT_TEST(testUtilCallback));

	result.push_back(LASS_UNIT_TEST(testUtilClock));

	result.push_back(LASS_UNIT_TEST(testUtilCloneFactory));

	result.push_back(LASS_UNIT_TEST(testUtilDictionary));

	result.push_back(LASS_UNIT_TEST(testUtilException));

	result.push_back(LASS_UNIT_TEST(testUtilFixedArray<int>));
	result.push_back(LASS_UNIT_TEST(testUtilFixedArray<float>));

	result.push_back(LASS_UNIT_TEST(testUtilIdGenerator<int>));
	result.push_back(LASS_UNIT_TEST(testUtilIdGenerator<char>));
	result.push_back(LASS_UNIT_TEST(testUtilIdGenerator<unsigned long>));

	result.push_back(LASS_UNIT_TEST(testUtilStringCast));

	result.push_back(LASS_UNIT_TEST(testUtilThreadFun));

	result.push_back(LASS_UNIT_TEST(testUtilThreadPool));

	//result.push_back(LASS_UNIT_TEST(testUtilVisitor));

	return result;
}



}

}
