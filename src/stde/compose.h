/** @file
 *  @author Bram de Greve (bramz@users.sourceforge.net)
 *  @author Tom De Muer (tomdemuer@users.sourceforge.net)
 *
 *  Distributed under the terms of the GPL (GNU Public License)
 *
 *  The LASS License:
 *
 *  Copyright 2004-2006 Bram de Greve and Tom De Muer
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



/** @defgroup stde_compose
 *  @brief functors to compose other functors
 *
 *  see Nicolai M. Josuttis. "The C++ Standard Library: A Tutorial and Referenc".
 *
 *  Use this if you need to concatenate two functors for use with STL algorithms
 *
 *  @code
 *	std::vector<int> a;
 *	for (int i = 0; i < 10; ++i)
 *	{
 *		a.push_back(i);
 *	}
 *	std::transform(a.begin(), a.end(), std::ostream_iterator<int>(std::cout, " "),
 *		stde::compose_f_gx(
 *			std::negate<int>(),
 *			std::bind2nd(std::multiplies<int>(), 2)));
 *
 *	typedef std::pair<int, int> pair_type;
 *	std::vector< std::pair<int, int> > b;
 *	for (int i = 0; i < 10; ++i)
 *	{
 *		b.push_back(std::make_pair(10 * i, i));
 *	}
 *	std::transform(b.begin(), b.end(), std::ostream_iterator<int>(std::cout, " "),
 *		stde::compose_f_gx_hx(
 *			std::plus<int>(),
 *			stde::select_1st<pair_type>(),
 *			stde::select_2nd<pair_type>()));
 *  @endcode
 */


#ifndef LASS_GUARDIAN_OF_INCLUSION_STDE_COMPOSE_H
#define LASS_GUARDIAN_OF_INCLUSION_STDE_COMPOSE_H

#include "stde_common.h"

namespace lass
{
namespace stde
{

/** f(g(x))
 *  @ingroup stde_compose
 */
template <typename OpF, typename OpG>
struct compose_f_gx_t: public std::unary_function
	<
		typename OpG::argument_type, 
		typename OpF::result_type
	>
{
public:
	compose_f_gx_t(const OpF& f, const OpG& g): 
		f_(f), 
		g_(g) 
	{
	}
	typename OpF::result_type operator()(const typename OpG::argument_type& x)
	{
		return f_(g_(x));
	}
private:
	OpF f_;
	OpG g_;
};

/** convencience function for the compose_f_gx adapter
 *  @relates compose_f_gx_t
 */
template <typename OpF, typename OpG> inline
compose_f_gx_t<OpF, OpG> compose_f_gx(const OpF& f, const OpG& g)
{
	return compose_f_gx_t<OpF, OpG>(f, g);
}




/** f(g(x), h(x))
 *  @ingroup stde_compose
 */
template <typename OpF, typename OpG, typename OpH>
struct compose_f_gx_hx_t: public std::unary_function
	<
		typename OpG::argument_type, 
		typename OpF::result_type
	>
{
public:
	compose_f_gx_hx_t(const OpF& f, const OpG& g, const OpH& h): 
		f_(f), 
		g_(g),
		h_(h)
	{
	}
	typename OpF::result_type operator()(const typename OpG::argument_type& x)
	{
		return f_(g_(x), h_(x));
	}
private:
	OpF f_;
	OpG g_;
	OpH h_;
};

/** convencience function for the compose_f_gx_hx adapter
 *  @relates compose_f_gx_hx_t
 */
template <typename OpF, typename OpG, typename OpH> inline
compose_f_gx_hx_t<OpF, OpG, OpH> compose_f_gx_hx(const OpF& f, const OpG& g, const OpH& h)
{
	return compose_f_gx_hx_t<OpF, OpG, OpH>(f, g, h);
}




/** f(g(x, y))
 *  @ingroup stde_compose
 */
template <typename OpF, typename OpG>
struct compose_f_gxy_t: public std::binary_function
	<
		typename OpG::first_argument_type, 
		typename OpG::second_argument_type, 
		typename OpF::result_type
	>
{
public:
	compose_f_gxy_t(const OpF& f, const OpG& g): 
		f_(f), 
		g_(g) 
	{
	}
	typename OpF::result_type operator()(
		const typename OpG::argument_type& x, const typename OpG::argument_type& y)
	{
		return f_(g_(x, y));
	}
private:
	OpF f_;
	OpG g_;
};

/** convencience function for the compose_f_gxy adapter
 *  @relates compose_f_gxy_t
 */
template <typename OpF, typename OpG> inline
compose_f_gxy_t<OpF, OpG> compose_f_gxy(const OpF& f, const OpG& g)
{
	return compose_f_gxy_t<OpF, OpG>(f, g);
}




/** f(g(x), h(y))
 *  @ingroup stde_compose
 */
template <typename OpF, typename OpG, typename OpH>
struct compose_f_gx_hy_t: public std::binary_function
	<
		typename OpG::first_argument_type, 
		typename OpH::second_argument_type, 
		typename OpF::result_type
	>
{
public:
	compose_f_gx_hy_t(const OpF& f, const OpG& g, const OpH& h): 
		f_(f), 
		g_(g),
		h_(h)
	{
	}
	typename OpF::result_type operator()(
		const typename OpG::argument_type& x, const typename OpG::argument_type& y)
	{
		return f_(g_(x), h_(y));
	}
private:
	OpF f_;
	OpG g_;
	OpH h_;
};

/** convencience function for the compose_f_gx_hy adapter
 *  @relates compose_f_gx_hy_t
 */
template <typename OpF, typename OpG, typename OpH> inline
compose_f_gx_hy_t<OpF, OpG, OpH> compose_f_gx_hy(const OpF& f, const OpG& g, const OpH& h)
{
	return compose_f_gx_hy_t<OpF, OpG, OpH>(f, g, h);
}



}

}

#endif

// EOF
