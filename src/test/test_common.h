/** @file
 *  @internal
 *  @author Bram de Greve (bramz@users.sourceforge.net)
 *  @author Tom De Muer (tomdemuer@users.sourceforge.net)
 *
 *  Distributed under the terms of the GPL (GNU Public License)
 *
 *  The LASS License:
 *
 *  Copyright 2004 Bram de Greve and Tom De Muer
 *
 *  LASS is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */



/** @namespace lass::test
 *  @brief namespace of lass test suite.
 */



#ifndef LASS_GUARDIAN_OF_INCLUSION_TEST_TEST_COMMON_H
#define LASS_GUARDIAN_OF_INCLUSION_TEST_TEST_COMMON_H

#include "../lass_common.h"

#include <boost/test/unit_test_suite.hpp>
#include <boost/test/test_tools.hpp>
#include <boost/test/floating_point_comparison.hpp>

#define LASS_TEST_VERSION LASS_LIB_PLATFORM "_" LASS_LIB_COMPILER LASS_LIB_DEBUG

/** compare if all components of a lass::prim vector, point or homogenous point are "close".
 *  It does what BOOST_CHECK_CLOSE does, but for more components.
 */
#define LASS_CLOSE_VECTOR(iLeft, iRight, iPersentTolerance)\
	lass::test::impl::compareVectorsAndContinue((iLeft), (iRight), (iPersentTolerance),\
												#iLeft,  #iRight, __FILE__, __LINE__)


namespace lass
{
namespace test
{
namespace impl
{

template <typename VectorType, typename PersentType>
inline bool
compareVectorsAndContinue(const VectorType& iLeft, const VectorType& iRight,
						  PersentType iTolerance,
						  const char* iLeftText, const char* iRightText,
						  const char* iFile, std::size_t iLine,
						  boost::unit_test_framework::log_level iLogLevel = boost::unit_test_framework::log_all_errors )
{
	using namespace boost::test_toolbox;

	for (unsigned i = 0; i < VectorType::dimension; ++i)
	{
		extended_predicate_value predicate(check_is_close(iLeft[i], iRight[i], iTolerance));

		if (!predicate)
		{
			return tt_detail::test_and_continue_impl(predicate, boost::wrap_stringstream().ref()
					<< "difference between one of the components of " << iLeftText << "{"
					<< tt_detail::print_helper<VectorType>( iLeft ) << "}" << " and " << iRightText << "{"
					<< tt_detail::print_helper<VectorType>( iRight ) << "}" << " exceeds "
					<< tt_detail::print_helper<PersentType>( iTolerance ) << "%",
				iFile, iLine, false, iLogLevel);
		}
	}

	return tt_detail::test_and_continue_impl(true, boost::wrap_stringstream().ref()
			<< "difference between the components of " << iLeftText << "{"
			<< tt_detail::print_helper<VectorType>( iLeft ) << "}" << " and " << iRightText << "{"
			<< tt_detail::print_helper<VectorType>( iRight ) << "} does not exceeds "
			<< tt_detail::print_helper<PersentType>( iTolerance ) << "%",
		iFile, iLine, true, iLogLevel);
}



}

}

}

#endif

// EOF
