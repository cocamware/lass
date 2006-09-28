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



/** @class lass::num::Polynomial
 *  @brief an univariate polynomial.
 *  @author Bram de Greve [BdG]
 *
 *  <i>A polynomial is a mathematical expression involving a sum of powers in one or more variables
 *  multiplied by coefficients.</i>,
 *  Eric W. Weisstein. "Polynomial." From MathWorld--A Wolfram Web Resource. 
 *  http://mathworld.wolfram.com/Polynomial.html
 */

#ifndef LASS_GUARDIAN_OF_INCLUSION_NUM_POLYNOMIAL_H
#define LASS_GUARDIAN_OF_INCLUSION_NUM_POLYNOMIAL_H

#include "num_common.h"
#include "num_traits.h"

namespace lass
{
namespace num
{

template <typename T>
class Polynomial
{
public:

	typedef Polynomial<T> TSelf;
	typedef NumTraits<T> TNumTraits;
	typedef typename util::CallTraits<T>::TValue TValue;
	typedef typename util::CallTraits<T>::TParam TParam;
	typedef typename util::CallTraits<T>::TReference TReference;
	typedef typename util::CallTraits<T>::TConstReference TConstReference;
	
	typedef std::vector<TValue> TCoefficients;
	typedef typename TCoefficients::size_type size_type;
	typedef typename TCoefficients::const_iterator const_iterator;

	Polynomial();
	explicit Polynomial(TParam iScalar);
	explicit Polynomial(const TCoefficients& iCoefficients);
	template <typename InputIterator> Polynomial(InputIterator iBegin, InputIterator iEnd);

	const TCoefficients& coefficients() const;
	const TValue operator[](size_t iIndex) const;
	const TValue at(size_t iIndex) const;

	const TValue operator()(TParam iX) const;

	const Polynomial<T>& operator+() const;
	const Polynomial<T> operator-() const;

	Polynomial<T>& operator+=(const Polynomial<T>& iOther);
	Polynomial<T>& operator-=(const Polynomial<T>& iOther);
	Polynomial<T>& operator*=(const Polynomial<T>& iOther);

	Polynomial<T>& operator+=(TParam iScalar);
	Polynomial<T>& operator-=(TParam iScalar);
	Polynomial<T>& operator*=(TParam iScalar);
	Polynomial<T>& operator/=(TParam iScalar);

	Polynomial<T> derivative() const;
	Polynomial<T> integral() const;
	Polynomial<T> pow(unsigned iPower) const;

	const size_type size() const;
	const const_iterator begin() const;
	const const_iterator end() const;

	static Polynomial<T> one();
	static Polynomial<T> x();

private:

	TCoefficients a_;
};

template <typename T> bool operator==(const Polynomial<T>& iA, const Polynomial<T>& iB);
template <typename T> bool operator!=(const Polynomial<T>& iA, const Polynomial<T>& iB);

template <typename T> Polynomial<T> operator+(const Polynomial<T>& iA, const Polynomial<T>& iB);
template <typename T> Polynomial<T> operator-(const Polynomial<T>& iA, const Polynomial<T>& iB);
template <typename T> Polynomial<T> operator*(const Polynomial<T>& iA, const Polynomial<T>& iB);

template <typename T> Polynomial<T> operator+(const T& iA, const Polynomial<T>& iB);
template <typename T> Polynomial<T> operator-(const T& iA, const Polynomial<T>& iB);
template <typename T> Polynomial<T> operator*(const T& iA, const Polynomial<T>& iB);

template <typename T> Polynomial<T> operator+(const Polynomial<T>& iA, const T& iB);
template <typename T> Polynomial<T> operator-(const Polynomial<T>& iA, const T& iB);
template <typename T> Polynomial<T> operator*(const Polynomial<T>& iA, const T& iB);
template <typename T> Polynomial<T> operator/(const Polynomial<T>& iA, const T& iB);

template <typename T, typename Char, typename Traits>
std::basic_ostream<Char, Traits>&
operator<<(std::basic_ostream<Char, Traits>& iS, const Polynomial<T>& iA);


}
}

#include "polynomial.inl"

#endif

// EOF
