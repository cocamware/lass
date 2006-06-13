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
 *  You should have received iA copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */


#ifndef LASS_GUARDIAN_OF_INCLUSION_NUM_FLOATING_POINT_CONSISTENCY_H
#define LASS_GUARDIAN_OF_INCLUSION_NUM_FLOATING_POINT_CONSISTENCY_H

#include "num_common.h"
#include "../util/call_traits.h"

namespace lass
{
namespace num
{

template <typename T>
class Consistent
{
public:
	typedef Consistent<T> TSelf;
	typedef typename util::CallTraits<T>::TValue TValue;
	typedef typename util::CallTraits<T>::TParam TParam;
	typedef typename util::CallTraits<T>::TReference TReference;
	typedef typename util::CallTraits<T>::TConstReference TConstReference;
	typedef num::NumTraits<T> TNumTraits;

	Consistent(TParam iT): 
		t_(iT) 
	{
	}

	TParam value() const 
	{ 
		return t_; 
	}

	TSelf& operator+=(const TSelf& iOther) 
	{ 
		t_ += iOther.t_; 
		return *this; 
	}
	TSelf& operator-=(const TSelf& iOther) 
	{ 
		t_ -= iOther.t_; 
		return *this; 
	}
	TSelf& operator*=(const TSelf& iOther) 
	{ 
		t_ *= iOther.t_; 
		return *this; 
	}
	TSelf& operator/=(const TSelf& iOther)
	{ 
		t_ /= iOther.t_; 
		return *this; 
	}

private:
	volatile TValue t_;
};




template <typename T> inline
Consistent<T> operator+(const Consistent<T>& iA, const Consistent<T>& iB)  
{ 
	Consistent<T> result(iA); 
	result += iB; 
	return result; 
}

template <typename T> inline
Consistent<T> operator-(const Consistent<T>& iA, const Consistent<T>& iB) 
{
	Consistent<T> result(iA);
	result -= iB;
	return result;
}

template <typename T> inline
Consistent<T> operator*(const Consistent<T>& iA, const Consistent<T>& iB) 
{
	Consistent<T> result(iA);
	result *= iB;
	return result;
}

template <typename T> inline
Consistent<T> operator/(const Consistent<T>& iA, const Consistent<T>& iB) 
{
	Consistent<T> result(iA);
	result /= iB;
	return result;
}

template <typename T> inline
bool operator==(const Consistent<T>& iA, const Consistent<T>& iB) 
{
	return iA.value() == iB.value();
}

template <typename T> inline
bool operator!=(const Consistent<T>& iA, const Consistent<T>& iB) 
{
	return !(iA == iB);
}

template <typename T> inline
bool operator<(const Consistent<T>& iA, const Consistent<T>& iB) 
{
	return iA.value() < iB.value();
}

template <typename T> inline
bool operator>(const Consistent<T>& iA, const Consistent<T>& iB) 
{
	return iB < iA;
}

template <typename T> inline
bool operator<=(const Consistent<T>& iA, const Consistent<T>& iB) 
{
	return !(iB < iA);
}

template <typename T> inline
bool operator>=(const Consistent<T>& iA, const Consistent<T>& iB) 
{
	return !(iA < iB);
}




template <typename T> inline
Consistent<T> operator+(const Consistent<T>& iA, const T& iB)  
{ 
	Consistent<T> result(iA); 
	result += iB; 
	return result; 
}

template <typename T> inline
Consistent<T> operator-(const Consistent<T>& iA, const T& iB) 
{
	Consistent<T> result(iA);
	result -= iB;
	return result;
}

template <typename T> inline
Consistent<T> operator*(const Consistent<T>& iA, const T& iB) 
{
	Consistent<T> result(iA);
	result *= iB;
	return result;
}

template <typename T> inline
Consistent<T> operator/(const Consistent<T>& iA, const T& iB) 
{
	Consistent<T> result(iA);
	result /= iB;
	return result;
}

template <typename T> inline
bool operator==(const Consistent<T>& iA, const T& iB) 
{
	Consistent<T> b(iB);
	return iA == b;
}

template <typename T> inline
bool operator!=(const Consistent<T>& iA, const T& iB) 
{
	return !(iA == iB);
}

template <typename T> inline
bool operator<(const Consistent<T>& iA, const T& iB) 
{
	Consistent<T> b(iB);
	return iA < b;
}

template <typename T> inline
bool operator>(const Consistent<T>& iA, const T& iB) 
{
	return iB < iA;
}

template <typename T> inline
bool operator<=(const Consistent<T>& iA, const T& iB) 
{
	return !(iB < iA);
}

template <typename T> inline
bool operator>=(const Consistent<T>& iA, const T& iB) 
{
	return !(iA < iB);
}




template <typename T> inline
Consistent<T> operator+(const T& iA, const Consistent<T>& iB)  
{ 
	Consistent<T> result(iA); 
	result += iB; 
	return result; 
}

template <typename T> inline
Consistent<T> operator-(const T& iA, const Consistent<T>& iB) 
{
	Consistent<T> result(iA);
	result -= iB;
	return result;
}

template <typename T> inline
Consistent<T> operator*(const T& iA, const Consistent<T>& iB) 
{
	Consistent<T> result(iA);
	result *= iB;
	return result;
}

template <typename T> inline
Consistent<T> operator/(const T& iA, const Consistent<T>& iB) 
{
	Consistent<T> result(iA);
	result /= iB;
	return result;
}

template <typename T> inline
bool operator==(const T& iA, const Consistent<T>& iB) 
{
	Consistent<T> a(iA);
	return a == iB;
}

template <typename T> inline
bool operator!=(const T& iA, const Consistent<T>& iB) 
{
	return !(iA == iB);
}

template <typename T> inline
bool operator<(const T& iA, const Consistent<T>& iB) 
{
	Consistent<T> a(iA);
	return a < iB;
}

template <typename T> inline
bool operator>(const T& iA, const Consistent<T>& iB) 
{
	return iB < iA;
}

template <typename T> inline
bool operator<=(const T& iA, const Consistent<T>& iB) 
{
	return !(iB < iA);
}

template <typename T> inline
bool operator>=(const T& iA, const Consistent<T>& iB) 
{
	return !(iA < iB);
}





template <typename T> inline
Consistent<T> abs(const Consistent<T>& iV)
{
	return num::abs(iV.value());
}

template <typename T> inline
Consistent<T> inv(const Consistent<T>& iV)
{
	return num::inv(iV.value());
}

template <typename T> inline
Consistent<T> sqrt(const Consistent<T>& iV)
{
	return num::sqrt(iV.value());
}

template <typename T> inline
Consistent<T> pow(const Consistent<T>& iV, const Consistent<T>& iPow)
{
	return num::pow(iV.value(), iPow.value());
}

template <typename T> inline
Consistent<T> exp(const Consistent<T>& iV)
{
	return num::exp(iV.value());
}

template <typename T> inline
Consistent<T> log(const Consistent<T>& iV)
{
	return num::log(iV.value());
}

template <typename T> inline
Consistent<T> cos(const Consistent<T>& iV)
{
	return num::cos(iV.value());
}

template <typename T> inline
Consistent<T> sin(const Consistent<T>& iV)
{
	return num::sin(iV.value());
}

template <typename T> inline
Consistent<T> tan(const Consistent<T>& iV)
{
	return num::tan(iV.value());
}

template <typename T> inline
Consistent<T> acos(const Consistent<T>& iV)
{
	return num::acos(iV.value());
}

template <typename T> inline
Consistent<T> asin(const Consistent<T>& iV)
{
	return num::asin(iV.value());
}

template <typename T> inline
Consistent<T> atan(const Consistent<T>& iV)
{
	return num::atan(iV.value());
}

template <typename T> inline
Consistent<T> atan2(const Consistent<T>& iX, const Consistent<T>& iY)
{
	return num::atan2(iX.value(), iY.value());
}

template <typename T> inline
Consistent<T> floor(const Consistent<T>& iV)
{
	return num::floor(iV.value());
}

template <typename T> inline
Consistent<T> ceil(const Consistent<T>& iV)
{
	return num::ceil(iV.value());
}

template <typename T> inline
Consistent<T> round(const Consistent<T>& iV)
{
	return num::round(iV.value());
}

template <typename T> inline
Consistent<T> fractional(const Consistent<T>& iV)
{
	return num::fractional(iV.value());
}

template <typename T> inline
Consistent<T> div(const Consistent<T>& iV, const Consistent<T>& iMod)
{
	return num::div(iV.value(), iMod.value());
}

template <typename T> inline
Consistent<T> mod(const Consistent<T>& iV, const Consistent<T>& iMod)
{
	return num::mod(iV.value(), iMod.value());
}


#if 0
template <typename T>
struct NumTraits< Consistent<T> >
{
private:
	typedef NumTraits<T> TBaseTraits;

public:
	typedef Consistent<T> selfType;	/**< our own type */
	typedef T   baseType;			/**< is the base type where selfType is based on */
	typedef T   intervalType;		/**< type of the support, useful for distributions */

	enum
	{
		isDistribution = TBaseTraits::isDistribution,	/**< true for distribution like types */
		isIntegral = TBaseTraits::isIntegral,		/**< true for integral types, ie supporting ++ and -- fi */
		isNative = TBaseTraits::isNative,       /**< true for the native types, ie char, int , ... */
		isSigned = TBaseTraits::isSigned,		/**< true for signed types */
		hasInfinity = TBaseTraits::hasInfinity,    /**< true for types having infinity */
		hasNaN = TBaseTraits::hasNaN			/**< true for types have the concept of Not a Number */
	};

	static const int   memorySize;      /**< memory footprint */
	static const std::string name() { return "Consistent<" + TBaseTraits::name() + ">" ; }   /**< name of the selfType */

	static const T  one = TBaseTraits::one;                /**< definition of one */
	static const T  zero = TBaseTraits::zero;               /**< definition of zero */

	/* some constants */
	static const T  sNaN = TBaseTraits::sNaN;               /**< signaling Nan, see http://research.microsoft.com/~hollasch/cgindex/coding/ieeefloat.html */
	static const T  qNaN = TBaseTraits::qNaN ;               /**< quiet NaN, see http://research.microsoft.com/~hollasch/cgindex/coding/ieeefloat.html */
	static const T  epsilon = TBaseTraits::epsilon;            /**< the smallest positive value such that one + epsilon != one*/
	static const T infinity = TBaseTraits::infinity;            /**< infinity */
	static const T min = TBaseTraits::min;                 /**< the smallest value representable */
	static const T  max = TBaseTraits::max;                /**< the largest positive value */
	static const T minStrictPositive = TBaseTraits::minStrictPositive;   /**< the smallest strictly positive value */
	static const T pi = TBaseTraits::pi;
	static const T e = TBaseTraits::e;
	static const T sqrt2 = TBaseTraits::sqrt2;
	static const T sqrtPi = TBaseTraits::sqrtPi;
};
#endif 

template<class T> inline
bool isNaN( const Consistent<T>& iV )
{
	return num::isNaN(iV.value());
}

template <typename T, typename Char, typename Traits>
std::basic_ostream<Char, Traits>& operator<<(std::basic_ostream<Char, Traits>& stream, const Consistent<T>& v)
{
	stream << v.value();
	return stream;
}

template <typename T, typename Char, typename Traits>
std::basic_istream<Char, Traits>& operator>>(std::basic_istream<Char, Traits>& stream, Consistent<T>& v)
{
	T temp;
	stream >> temp;
	v = Consistent<T>(temp);
	return stream;
}

}

}

#endif
