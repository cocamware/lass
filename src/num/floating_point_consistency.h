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

	Consistent(TParam t): 
		t_(t) 
	{
	}

	TParam value() const 
	{ 
		return t_; 
	}

	TSelf& operator+=(const TSelf& other) 
	{ 
		t_ += other.t_; 
		return *this; 
	}
	TSelf& operator-=(const TSelf& other) 
	{ 
		t_ -= other.t_; 
		return *this; 
	}
	TSelf& operator*=(const TSelf& other) 
	{ 
		t_ *= other.t_; 
		return *this; 
	}
	TSelf& operator/=(const TSelf& other)
	{ 
		t_ /= other.t_; 
		return *this; 
	}
	
	void swap(TSelf& other)
	{
		std::swap(t_, other.t_);
	}

private:
	volatile TValue t_;
};




template <typename T> inline
Consistent<T> operator+(const Consistent<T>& a, const Consistent<T>& b)  
{ 
	Consistent<T> result(a); 
	result += b; 
	return result; 
}

template <typename T> inline
Consistent<T> operator-(const Consistent<T>& a, const Consistent<T>& b) 
{
	Consistent<T> result(a);
	result -= b;
	return result;
}

template <typename T> inline
Consistent<T> operator*(const Consistent<T>& a, const Consistent<T>& b) 
{
	Consistent<T> result(a);
	result *= b;
	return result;
}

template <typename T> inline
Consistent<T> operator/(const Consistent<T>& a, const Consistent<T>& b) 
{
	Consistent<T> result(a);
	result /= b;
	return result;
}

template <typename T> inline
bool operator==(const Consistent<T>& a, const Consistent<T>& b) 
{
	return a.value() == b.value();
}

template <typename T> inline
bool operator!=(const Consistent<T>& a, const Consistent<T>& b) 
{
	return !(a == b);
}

template <typename T> inline
bool operator<(const Consistent<T>& a, const Consistent<T>& b) 
{
	return a.value() < b.value();
}

template <typename T> inline
bool operator>(const Consistent<T>& a, const Consistent<T>& b) 
{
	return b < a;
}

template <typename T> inline
bool operator<=(const Consistent<T>& a, const Consistent<T>& b) 
{
	return !(b < a);
}

template <typename T> inline
bool operator>=(const Consistent<T>& a, const Consistent<T>& b) 
{
	return !(a < b);
}




template <typename T> inline
Consistent<T> operator+(const Consistent<T>& a, const T& b)  
{ 
	Consistent<T> result(a); 
	result += b; 
	return result; 
}

template <typename T> inline
Consistent<T> operator-(const Consistent<T>& a, const T& b) 
{
	Consistent<T> result(a);
	result -= b;
	return result;
}

template <typename T> inline
Consistent<T> operator*(const Consistent<T>& a, const T& b) 
{
	Consistent<T> result(a);
	result *= b;
	return result;
}

template <typename T> inline
Consistent<T> operator/(const Consistent<T>& a, const T& b) 
{
	Consistent<T> result(a);
	result /= b;
	return result;
}

template <typename T> inline
bool operator==(const Consistent<T>& a, const T& b) 
{
	return a == Consistent<T>(b);
}

template <typename T> inline
bool operator!=(const Consistent<T>& a, const T& b) 
{
	return !(a == b);
}

template <typename T> inline
bool operator<(const Consistent<T>& a, const T& b) 
{
	return a < Consistent<T>(b);
}

template <typename T> inline
bool operator>(const Consistent<T>& a, const T& b) 
{
	return b < a;
}

template <typename T> inline
bool operator<=(const Consistent<T>& a, const T& b) 
{
	return !(b < a);
}

template <typename T> inline
bool operator>=(const Consistent<T>& a, const T& b) 
{
	return !(a < b);
}




template <typename T> inline
Consistent<T> operator+(const T& a, const Consistent<T>& b)  
{ 
	Consistent<T> result(a); 
	result += b; 
	return result; 
}

template <typename T> inline
Consistent<T> operator-(const T& a, const Consistent<T>& b) 
{
	Consistent<T> result(a);
	result -= b;
	return result;
}

template <typename T> inline
Consistent<T> operator*(const T& a, const Consistent<T>& b) 
{
	Consistent<T> result(a);
	result *= b;
	return result;
}

template <typename T> inline
Consistent<T> operator/(const T& a, const Consistent<T>& b) 
{
	Consistent<T> result(a);
	result /= b;
	return result;
}

template <typename T> inline
bool operator==(const T& a, const Consistent<T>& b) 
{
	return Consistent<T>(a) == b;
}

template <typename T> inline
bool operator!=(const T& a, const Consistent<T>& b) 
{
	return !(a == b);
}

template <typename T> inline
bool operator<(const T& a, const Consistent<T>& b) 
{
	return Consistent<T>(a) < b;
}

template <typename T> inline
bool operator>(const T& a, const Consistent<T>& b) 
{
	return b < a;
}

template <typename T> inline
bool operator<=(const T& a, const Consistent<T>& b) 
{
	return !(b < a);
}

template <typename T> inline
bool operator>=(const T& a, const Consistent<T>& b) 
{
	return !(a < b);
}





template <typename T> inline
Consistent<T> abs(const Consistent<T>& v)
{
	return num::abs(v.value());
}

template <typename T> inline
Consistent<T> inv(const Consistent<T>& v)
{
	return num::inv(v.value());
}

template <typename T> inline
Consistent<T> sqrt(const Consistent<T>& v)
{
	return num::sqrt(v.value());
}

template <typename T> inline
Consistent<T> pow(const Consistent<T>& v, const Consistent<T>& p)
{
	return num::pow(v.value(), p.value());
}

template <typename T> inline
Consistent<T> exp(const Consistent<T>& v)
{
	return num::exp(v.value());
}

template <typename T> inline
Consistent<T> log(const Consistent<T>& v)
{
	return num::log(v.value());
}

template <typename T> inline
Consistent<T> cos(const Consistent<T>& v)
{
	return num::cos(v.value());
}

template <typename T> inline
Consistent<T> sin(const Consistent<T>& v)
{
	return num::sin(v.value());
}

template <typename T> inline
Consistent<T> tan(const Consistent<T>& v)
{
	return num::tan(v.value());
}

template <typename T> inline
Consistent<T> acos(const Consistent<T>& v)
{
	return num::acos(v.value());
}

template <typename T> inline
Consistent<T> asin(const Consistent<T>& v)
{
	return num::asin(v.value());
}

template <typename T> inline
Consistent<T> atan(const Consistent<T>& v)
{
	return num::atan(v.value());
}

template <typename T> inline
Consistent<T> atan2(const Consistent<T>& x, const Consistent<T>& y)
{
	return num::atan2(x.value(), y.value());
}

template <typename T> inline
Consistent<T> floor(const Consistent<T>& v)
{
	return num::floor(v.value());
}

template <typename T> inline
Consistent<T> ceil(const Consistent<T>& v)
{
	return num::ceil(v.value());
}

template <typename T> inline
Consistent<T> round(const Consistent<T>& v)
{
	return num::round(v.value());
}

template <typename T> inline
Consistent<T> fractional(const Consistent<T>& v)
{
	return num::fractional(v.value());
}

template <typename T> inline
Consistent<T> div(const Consistent<T>& v, const Consistent<T>& m)
{
	return num::div(v.value(), m.value());
}

template <typename T> inline
Consistent<T> mod(const Consistent<T>& v, const Consistent<T>& m)
{
	return num::mod(v.value(), m.value());
}


#define LASS_NUM_FLOATING_POINT_CONSISTENCY_DECLARE_NUMTRAITS(type)\
	template <>\
	struct NumTraits< Consistent< type > >\
	{\
	private:\
		typedef NumTraits< type > TBaseTraits;\
	public:\
		typedef Consistent< type > selfType;\
		typedef type baseType;\
		typedef type intervalType;\
		enum\
		{\
			isDistribution = TBaseTraits::isDistribution,\
			isIntegral = TBaseTraits::isIntegral,\
			isNative = TBaseTraits::isNative,\
			isSigned = TBaseTraits::isSigned,\
			hasInfinity = TBaseTraits::hasInfinity,\
			hasNaN = TBaseTraits::hasNaN\
		};\
		static const int memorySize;\
		static const std::string name() { return "Consistent<" + TBaseTraits::name() + ">" ; }\
		static const selfType one;\
		static const selfType zero;\
		static const selfType sNaN;\
		static const selfType qNaN;\
		static const selfType epsilon;\
		static const selfType infinity;\
		static const selfType min;\
		static const selfType max;\
		static const selfType minStrictPositive;\
		static const selfType pi;\
		static const selfType e;\
		static const selfType sqrt2;\
		static const selfType sqrtPi;\
	};

LASS_NUM_FLOATING_POINT_CONSISTENCY_DECLARE_NUMTRAITS(float)
LASS_NUM_FLOATING_POINT_CONSISTENCY_DECLARE_NUMTRAITS(double)

#define LASS_NUM_FLOATING_POINT_CONSISTENCY_DECLARE_NUMTRAITS_COMPLEX(type)\
	template <>\
	struct NumTraits< Consistent< type > >\
	{\
	private:\
		typedef NumTraits< type > TBaseTraits;\
	public:\
		typedef Consistent< type > selfType;\
		typedef type baseType;\
		typedef type intervalType;\
		enum\
		{\
			isDistribution = TBaseTraits::isDistribution,\
			isIntegral = TBaseTraits::isIntegral,\
			isNative = TBaseTraits::isNative,\
			isSigned = TBaseTraits::isSigned,\
			hasInfinity = TBaseTraits::hasInfinity,\
			hasNaN = TBaseTraits::hasNaN\
		};\
		static const int memorySize;\
		static const std::string name() { return "Consistent<" + TBaseTraits::name() + ">" ; }\
		static const selfType one;\
		static const selfType zero;\
		static const selfType pi;\
		static const selfType e;\
		static const selfType sqrt2;\
		static const selfType sqrtPi;\
	};

LASS_NUM_FLOATING_POINT_CONSISTENCY_DECLARE_NUMTRAITS_COMPLEX(std::complex<float>)
LASS_NUM_FLOATING_POINT_CONSISTENCY_DECLARE_NUMTRAITS_COMPLEX(std::complex<double>)

template<class T> inline
bool isNaN( const Consistent<T>& v )
{
	return num::isNaN(v.value());
}

template <typename T, typename Char, typename Traits>
std::basic_ostream<Char, Traits>& operator<<(std::basic_ostream<Char, Traits>& s, const Consistent<T>& v)
{
	s << v.value();
	return s;
}

template <typename T, typename Char, typename Traits>
std::basic_istream<Char, Traits>& operator>>(std::basic_istream<Char, Traits>& s, Consistent<T>& v)
{
	T temp;
	s >> temp;
	v = Consistent<T>(temp);
	return s;
}

}

}

namespace std
{

template <typename T>
void swap(::lass::num::Consistent<T>& a, ::lass::num::Consistent<T>& b)
{
	a.swap(b);
}

}

#endif
