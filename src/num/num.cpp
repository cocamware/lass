/**	@file
 *	@author Bram de Greve (bramz@users.sourceforge.net)
 *	@author Tom De Muer (tomdemuer@users.sourceforge.net)
 *
 *	Distributed under the terms of the GPL (GNU Public License)
 *
 * 	The LASS License:
 *
 *	Copyright 2004 Bram de Greve and Tom De Muer
 *
 *	LASS is free software; you can redistribute it and/or modify
 *	it under the terms of the GNU General Public License as published by
 *	the Free Software Foundation; either version 2 of the License, or
 *	(at your option) any later version.
 *
 *	This program is distributed in the hope that it will be useful,
 *	but WITHOUT ANY WARRANTY; without even the implied warranty of
 *	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *	GNU General Public License for more details.
 *
 *	You should have received a copy of the GNU General Public License
 *	along with this program; if not, write to the Free Software
 *	Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */



#include "num_common.h"
#include "num_traits.h"
#include "interval.h"
#include "fuzzy_functions.h"
#include "fn_sampled8.h"

#include <iostream>

const bool	testTraits = true;
const bool	testInterval = true;

template<class C>
void test_traits(C iDummyArg)
{
	std::cout << "\n";
	std::cout << "name = " << lass::num::NumTraits<C>::name() << std::endl;
	std::cout << "isIntegral  = " << lass::num::NumTraits<C>::isIntegral << std::endl;
	std::cout << "isNative  = " << lass::num::NumTraits<C>::isNative << std::endl;
	std::cout << "memorySize  = " << lass::num::NumTraits<C>::memorySize << std::endl;
	std::cout << "one  = " << lass::num::str(lass::num::NumTraits<C>::one) << std::endl;
	std::cout << "zero  = " << lass::num::str(lass::num::NumTraits<C>::zero) << std::endl;

	std::cout << "sqrt of pi = " << lass::num::str(lass::num::sqrt(  lass::num::NumTraits<C>::pi )) << std::endl;
	std::cout << "sqrtPi = " << lass::num::str(lass::num::NumTraits<C>::sqrtPi) << std::endl;

	std::cout << lass::num::str( lass::num::NumTraits<C>::e ) << std::endl;
}

template<class C>
void test_interval(C iDummyArg)
{
	std::cout << "\n";

	typedef lass::num::interval<C>	Tinterval;
	typedef lass::num::NumTraits<Tinterval>	TnumTrait;

	Tinterval	temp1;
	Tinterval	temp2;

	std::cout << "1+1 = " << TnumTrait::one + TnumTrait::one << std::endl;
	std::cout << "1-1 = " << TnumTrait::one - TnumTrait::one << std::endl;
	temp1 = TnumTrait::one + TnumTrait::one;
	std::cout << "(1+1)*(1+1) = " << temp1 * temp1 << std::endl;
	std::cout << "1/(1+1) = " << TnumTrait::one / temp1 << std::endl;

}

template<class C>
void test_fuzzy_functions(C iDummyArg)
{
	std::cout << "\n";
	
	C	temp;
	
	lass::num::createUnit(temp);
	std::cout << "unit = " << temp << std::endl;
	lass::num::createZero(temp);
	std::cout << "zero = " << temp << std::endl;

	std::cout << "entropy = " << lass::num::entropy( temp ) << std::endl;

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
		std::cout << i << "\t" << lass::num::mod(i,3) << std::endl;

	std::cin.ignore();
	return 0;
}
