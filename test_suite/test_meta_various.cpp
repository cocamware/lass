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
 *	Copyright (C) 2004-2011 the Initial Developer.
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

#include "../lass/meta/is_same.h"
#include "../lass/meta/is_convertible.h"
#include "../lass/meta/is_derived.h"
#include "../lass/meta/is_member.h"

namespace lass
{
namespace test
{

class Spam {};
class Ham: public Spam 
{
public:
	int nonConstMember() { return 0; }
	int constMember() const { return 0; }
	void nonConstMethodWithLotsOfArguments( int, double, int, std::string, Ham& ) {}
};

void testMetaIsConvertible()
{
	// These might cause some compiler warnings about floats being converted to ints
	// and non-POD type objects being passed through ... (that will cause runtime crashes)
	// this is actually OK, it means the stuff actually works.
	//
	// I unfortunately do not a know how to suppress the warnings on gcc, in a LOCAL way.
	// If anyone knows, please let me know =)  I'm not interested in global ways to suppress
	// the warnings as that would be dangerous (we wouldn't see it when it really matters).
	//
	// [Bramz]

	// LASS_TEST_CHECK_EQUAL(bool(meta::IsConvertible<float, int>::value), true);
	LASS_TEST_CHECK_EQUAL(bool(meta::IsConvertible<int, float>::value), true);

	LASS_TEST_CHECK_EQUAL(bool(meta::IsConvertible<std::string, const char*>::value), false);
	LASS_TEST_CHECK_EQUAL(bool(meta::IsConvertible<const char*, std::string>::value), true);

	LASS_TEST_CHECK_EQUAL(bool(meta::IsConvertible<float*, int*>::value), false);
	LASS_TEST_CHECK_EQUAL(bool(meta::IsConvertible<float*, void*>::value), true);

	LASS_TEST_CHECK_EQUAL(bool(meta::IsConvertible<const float*, float*>::value), false);
	LASS_TEST_CHECK_EQUAL(bool(meta::IsConvertible<float*, const float*>::value), true);

	LASS_TEST_CHECK_EQUAL(bool(meta::IsConvertible<Spam, int>::value), false);
	LASS_TEST_CHECK_EQUAL(bool(meta::IsConvertible<int, Spam>::value), false);
	LASS_TEST_CHECK_EQUAL(bool(meta::IsConvertible<Spam, Ham>::value), false);

	LASS_TEST_CHECK_EQUAL(bool(meta::IsConvertible<Spam*, Ham*>::value), false);
	LASS_TEST_CHECK_EQUAL(bool(meta::IsConvertible<Ham*, Spam*>::value), true);
}



void testMetaIsDerivedType()
{
	LASS_TEST_CHECK_EQUAL(bool(meta::IsDerived<float, int>::value), false);
	LASS_TEST_CHECK_EQUAL(bool(meta::IsDerivedStrict<float, int>::value), false);

	LASS_TEST_CHECK_EQUAL(bool(meta::IsDerived<float, float>::value), true);
	LASS_TEST_CHECK_EQUAL(bool(meta::IsDerivedStrict<float, float>::value), false);

	LASS_TEST_CHECK_EQUAL(bool(meta::IsDerived<Spam, void>::value), false);
	LASS_TEST_CHECK_EQUAL(bool(meta::IsDerivedStrict<Spam, void>::value), false);

	LASS_TEST_CHECK_EQUAL(bool(meta::IsDerived<void, void>::value), false); // shouldn't this be true?
	LASS_TEST_CHECK_EQUAL(bool(meta::IsDerivedStrict<void, void>::value), false);

	LASS_TEST_CHECK_EQUAL(bool(meta::IsDerived<Ham, Spam>::value), true);
	LASS_TEST_CHECK_EQUAL(bool(meta::IsDerivedStrict<Ham, Spam>::value), true);

	LASS_TEST_CHECK_EQUAL(bool(meta::IsDerived<Spam, Spam>::value), true);
	LASS_TEST_CHECK_EQUAL(bool(meta::IsDerivedStrict<Spam, Spam>::value), false);

	LASS_TEST_CHECK_EQUAL(bool(meta::IsDerived<Spam, Ham>::value), false);
	LASS_TEST_CHECK_EQUAL(bool(meta::IsDerivedStrict<Spam, Ham>::value), false);

	LASS_TEST_CHECK_EQUAL(bool(meta::IsDerived<Ham*, Spam*>::value), false);
	LASS_TEST_CHECK_EQUAL(bool(meta::IsDerivedStrict<Ham*, Spam*>::value), false);
}

void testMetaIsMember()
{
	typedef int (Ham::*THamNonConstMember)(void) ;
	typedef int (Ham::*THamConstMember)(void) const;
	LASS_TEST_CHECK_EQUAL(bool(meta::IsMember< THamConstMember >::value), true);
	LASS_TEST_CHECK_EQUAL(bool(meta::IsMember< THamNonConstMember >::value), true);
	LASS_TEST_CHECK_EQUAL(bool(meta::IsConstMember< THamConstMember >::value), true);
	LASS_TEST_CHECK_EQUAL(bool(meta::IsConstMember< THamNonConstMember >::value), false);

	typedef int (Ham::*TNonConstMethodWithLotsOfArguments)(int, double, int, std::string, Ham&);
	LASS_TEST_CHECK_EQUAL(bool(meta::isMember( &Ham::nonConstMethodWithLotsOfArguments )), true);
	LASS_TEST_CHECK_EQUAL(bool(meta::isConstMember( &Ham::nonConstMethodWithLotsOfArguments )), false);
	LASS_TEST_CHECK_EQUAL(bool(meta::IsMember< TNonConstMethodWithLotsOfArguments >::value), true);
}

TUnitTest test_meta_various()
{
	TUnitTest result;
	result.push_back(LASS_TEST_CASE(testMetaIsConvertible));
	result.push_back(LASS_TEST_CASE(testMetaIsDerivedType));
	result.push_back(LASS_TEST_CASE(testMetaIsMember));
	return result;
}

}

}

// EOF
