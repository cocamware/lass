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
 *	Copyright (C) 2004-2009 the Initial Developer.
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
#include "../lass/num/num_cast.h"

namespace lass
{
namespace test
{

void testNumNumCast()
{
	// gcc 4.1.1 doesn't seem to like stuff like signed char between the <>
	typedef signed char TSignedChar;
	typedef signed short TSignedShort;
	typedef signed int TSignedInt;
	typedef signed long TSignedLong;
	typedef unsigned char TUnsignedChar;
	typedef unsigned short TUnsignedShort;
	typedef unsigned int TUnsignedInt;
	typedef unsigned long TUnsignedLong;

	LASS_TEST_CHECK_EQUAL(num::numCast<TSignedChar>(TSignedChar(0)), 0);
	LASS_TEST_CHECK_EQUAL(num::numCast<TSignedShort>(TSignedChar(0)), 0);
	LASS_TEST_CHECK_EQUAL(num::numCast<TSignedInt>(TSignedChar(0)), 0);
	LASS_TEST_CHECK_EQUAL(num::numCast<TSignedLong>(TSignedChar(0)), 0);
	LASS_TEST_CHECK_EQUAL(num::numCast<TUnsignedChar>(TSignedChar(0)), 0);
	LASS_TEST_CHECK_EQUAL(num::numCast<TUnsignedShort>(TSignedChar(0)), 0);
	LASS_TEST_CHECK_EQUAL(num::numCast<TUnsignedInt>(TSignedChar(0)), TUnsignedInt(0));
	LASS_TEST_CHECK_EQUAL(num::numCast<TUnsignedLong>(TSignedChar(0)), TUnsignedLong(0));

	LASS_TEST_CHECK_EQUAL(num::numCast<TSignedChar>(TUnsignedLong(0)), 0);
	LASS_TEST_CHECK_EQUAL(num::numCast<TSignedShort>(TUnsignedLong(0)), 0);
	LASS_TEST_CHECK_EQUAL(num::numCast<TSignedInt>(TUnsignedLong(0)), 0);
	LASS_TEST_CHECK_EQUAL(num::numCast<TSignedLong>(TUnsignedLong(0)), 0);
	LASS_TEST_CHECK_EQUAL(num::numCast<TUnsignedChar>(TUnsignedLong(0)), 0);
	LASS_TEST_CHECK_EQUAL(num::numCast<TUnsignedShort>(TUnsignedLong(0)), 0);
	LASS_TEST_CHECK_EQUAL(num::numCast<TUnsignedInt>(TUnsignedLong(0)), TUnsignedInt(0));
	LASS_TEST_CHECK_EQUAL(num::numCast<TUnsignedLong>(TUnsignedLong(0)),  TUnsignedLong(0));

	LASS_TEST_CHECK_EQUAL(num::numCast<TSignedChar>(TUnsignedLong(127)), 127);
	LASS_TEST_CHECK_EQUAL(num::numCast<TSignedChar>(long(127)), 127);
	LASS_TEST_CHECK_EQUAL(num::numCast<TSignedChar>(long(-128)), -128);
	LASS_TEST_CHECK_THROW(num::numCast<TSignedChar>(TUnsignedLong(128)), num::BadNumCast);
	LASS_TEST_CHECK_THROW(num::numCast<TSignedChar>(long(128)), num::BadNumCast);
	LASS_TEST_CHECK_THROW(num::numCast<TSignedChar>(long(-129)), num::BadNumCast);

	LASS_TEST_CHECK_EQUAL(num::numCast<TUnsignedChar>(TUnsignedLong(255)), 255);
	LASS_TEST_CHECK_EQUAL(num::numCast<TUnsignedChar>(long(255)), 255);
	LASS_TEST_CHECK_EQUAL(num::numCast<TUnsignedChar>(long(0)), 0);
	LASS_TEST_CHECK_THROW(num::numCast<TUnsignedChar>(TUnsignedLong(256)), num::BadNumCast);
	LASS_TEST_CHECK_THROW(num::numCast<TUnsignedChar>(long(256)), num::BadNumCast);
	LASS_TEST_CHECK_THROW(num::numCast<TUnsignedChar>(long(-1)), num::BadNumCast);
}

TUnitTest test_num_num_cast()
{
	return TUnitTest(1, LASS_TEST_CASE(testNumNumCast));

}



}

}

// EOF
