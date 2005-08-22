/** @file
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



/** @defgroup stde_select
 *  @brief functors to select element of std::pair or stde::triple
 */


#ifndef LASS_GUARDIAN_OF_INCLUSION_STDE_SELECT_H
#define LASS_GUARDIAN_OF_INCLUSION_STDE_SELECT_H

#include "stde_common.h"

namespace lass
{
namespace stde
{

/** selects first element of std::pair or stde::triple and applies supplied operator on it
 *  @ingroup stde_select
 */
template <typename Op>
struct select_1st_t: public std::unary_function<Op::argument_type, Op::result_type>
{
public:
	select_1st_t(): op_() {}
	explicit select_1st_t(Op iOp): op_(iOp) {}
	template <typename T> result_type operator()(T& iT) { return op_(iT.first); }
protected:
	Op op_;
};

/** @relates select_1st_t
 */
template <typename Op> select_1st_t<Op> select_1st(const Op& op) { return select_1st_t<Op>(op); }



/** selects second element of std::pair or stde::triple and applies supplied operator on it
 *  @ingroup stde_select
 */
template <typename Op>
struct select_2nd_t: public std::unary_function<Op::argument_type, Op::result_type>
{
public:
	select_2nd_t(): op_() {}
	explicit select_2nd_t(Op iOp): op_(iOp) {}
	template <typename T> result_type operator()(T& iT) { return op_(iT.second); }
protected:
	Op op_;
};

/** @relates select_2nd_t
 */
template <typename Op> select_2nd_t<Op> select_2nd(const Op& op) { return select_2nd_t<Op>(op); }



/** selects third element of stde::triple and applies supplied operator on it
 *  @ingroup stde_select
 */
template <typename Op>
struct select_3rd_t: public std::unary_function<Op::argument_type, Op::result_type>
{
public:
	select_3rd_t(): op_() {}
	explicit select_3rd_t(Op iOp): op_(iOp) {}
	template <typename T> result_type operator()(T& iT) { return op_(iT.third); }
protected:
	Op op_;
};

/** @relates select_3rd_t
 */
template <typename Op> select_3rd_t<Op> select_3rd(const Op& op) { return select_3rd_t<Op>(op); }



}

}

#endif

// EOF
