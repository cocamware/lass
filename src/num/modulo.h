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



/** @class lass::num::Modulo
 *  @brief a integer number class using modulo arithmetic
 *  @author Bram de Greve [BdG]
 *
 *  @warning under development!
 *
 *  @bug what happens if the temporary result of an operation does not fit in the range of T?
 */

#ifndef LASS_GUARDIAN_OF_INCLUSION_NUM_MODULO_H
#define LASS_GUARDIAN_OF_INCLUSION_NUM_MODULO_H

#include "num_common.h"
#include "../util/call_traits.h"

namespace lass
{
namespace num
{

template <unsigned N, typename T = int>
class Modulo
{
public:

	typedef Modulo<N, T> TSelf;

	typedef typename util::CallTraits<T>::TValue TValue;
	typedef typename util::CallTraits<T>::TParam TParam;

	template <typename U>
	struct Rebind
	{
		typedef Modulo<N, U> Type;
	};

	Modulo(TParam iValue = 0);

	Modulo<N, T> operator+() const;
	Modulo<N, T> operator-() const;

	Modulo<N, T>& operator++();
	Modulo<N, T>& operator--();
	Modulo<N, T> operator++(int);
	Modulo<N, T> operator--(int);

	Modulo<N, T>& operator+=(const Modulo<N, T>& iB);
	Modulo<N, T>& operator-=(const Modulo<N, T>& iB);
	Modulo<N, T>& operator*=(const Modulo<N, T>& iB);
	Modulo<N, T>& operator/=(const Modulo<N, T>& iB);

	operator T() const;
	TParam value() const;

private:

	bool isInRange(TParam iValue) const;

	TValue value_;
};

template <unsigned N, typename T>
bool operator==(const Modulo<N, T>& iA, const Modulo<N, T>& iB);

template <unsigned N, typename T>
bool operator!=(const Modulo<N, T>& iA, const Modulo<N, T>& iB);

template <unsigned N, typename T>
bool operator<(const Modulo<N, T>& iA, const Modulo<N, T>& iB);

template <unsigned N, typename T>
bool operator>(const Modulo<N, T>& iA, const Modulo<N, T>& iB);

template <unsigned N, typename T>
bool operator<=(const Modulo<N, T>& iA, const Modulo<N, T>& iB);

template <unsigned N, typename T>
bool operator>=(const Modulo<N, T>& iA, const Modulo<N, T>& iB);

template <unsigned N, typename T>
std::ostream& operator<<(std::ostream& iS, const Modulo<N, T>& iM);

template <unsigned N, typename T>
std::istream& operator>>(std::istream& iS, Modulo<N, T>& iM);

}

}

#include "modulo.inl"

#endif

// EOF
