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



#include "num_common.h"
#include "num_traits.h"
#include "interval.h"
#include "fuzzy_functions.h"
#include "fn_sampled8.h"

#include <iostream>

const bool  testTraits = true;
const bool  testInterval = true;

template<class C>
void test_traits(C iDummyArg)
{
	LASS_COUT << "\n";
	LASS_COUT << "name = " << lass::num::NumTraits<C>::name() << std::endl;
	LASS_COUT << "isIntegral  = " << lass::num::NumTraits<C>::isIntegral << std::endl;
	LASS_COUT << "isNative  = " << lass::num::NumTraits<C>::isNative << std::endl;
	LASS_COUT << "memorySize  = " << lass::num::NumTraits<C>::memorySize << std::endl;
	LASS_COUT << "one  = " << lass::num::str(lass::num::NumTraits<C>::one) << std::endl;
	LASS_COUT << "zero  = " << lass::num::str(lass::num::NumTraits<C>::zero) << std::endl;

	LASS_COUT << "sqrt of pi = " << lass::num::str(lass::num::sqrt(  lass::num::NumTraits<C>::pi )) << std::endl;
	LASS_COUT << "sqrtPi = " << lass::num::str(lass::num::NumTraits<C>::sqrtPi) << std::endl;

	LASS_COUT << lass::num::str( lass::num::NumTraits<C>::e ) << std::endl;
}

template<class C>
void test_interval(C iDummyArg)
{
	LASS_COUT << "\n";

	typedef lass::num::interval<C>  Tinterval;
	typedef lass::num::NumTraits<Tinterval> TnumTrait;

	Tinterval   temp1;
	Tinterval   temp2;

	LASS_COUT << "1+1 = " << TnumTrait::one + TnumTrait::one << std::endl;
	LASS_COUT << "1-1 = " << TnumTrait::one - TnumTrait::one << std::endl;
	temp1 = TnumTrait::one + TnumTrait::one;
	LASS_COUT << "(1+1)*(1+1) = " << temp1 * temp1 << std::endl;
	LASS_COUT << "1/(1+1) = " << TnumTrait::one / temp1 << std::endl;

}

template<class C>
void test_fuzzy_functions(C iDummyArg)
{
	LASS_COUT << "\n";

	C   temp;

	lass::num::createUnit(temp);
	LASS_COUT << "unit = " << temp << std::endl;
	lass::num::createZero(temp);
	LASS_COUT << "zero = " << temp << std::endl;

	LASS_COUT << "entropy = " << lass::num::entropy( temp ) << std::endl;

}

int main(int argc, char* argv[])
{
	if ( testTraits )
	{
		test_traits<float>(0);
		test_traits<double>(0);
		test_traits< lass::num::interval<float> >( lass::num::interval<float>(0.f) );
		test_traits< lass::num::interval<double> >( lass::num::interval<double>(0.f) );
	}

	if ( testInterval )
	{
		test_interval<float>(0);
	}

	test_fuzzy_functions<float>(0);
	test_traits< lass::num::FNSampled8 >( lass::num::FNSampled8(0.f) );

	for (int i=-5;i<5;++i)
		LASS_COUT << i << "\t" << lass::num::mod(i,3) << std::endl;

	std::cin.ignore();
	return 0;
}
