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


#ifndef LASS_GUARDIAN_OF_INCLUSION_NUM_FILTERED_FLOAT_H
#define LASS_GUARDIAN_OF_INCLUSION_NUM_FILTERED_FLOAT_H

#include "num_common.h"
#include "../util/call_traits.h"
#include <complex>

namespace lass
{
namespace num
{

/** @class lass::num::FilteredFloat
 *  @brief a float with error analysis on it's basic operations.
 *  @author Tom De Muer [TDM]
 *
 *	For more information we refer to the work of Shewchuck, Fortune, Van Wijck on forward
 *  error analysis.
 *
 *  @arg J. R. Shewchuk 1997, "Adaptive Precision Floating-Point Arithmetic and Fast Robust 
 *  Geometric Predicates", Discrete & Computational Geometry, 18(3), 305--363
 *
 *	The filtered float here represents X with x+e, x and e non-overlapping and e the error on the
 *  previous operation.  If X+Y=Z with X=x+e and Y=y+f then error of Z is g with Z=(x+y)+g.  Thus
 *  <b>not</b> the error on (x+e)+(y+f) but only on (x+y)!
 */

template <typename T>
class FilteredFloat
{
public:
	typedef FilteredFloat< T > TSelf;
	typedef typename util::CallTraits<T>::TValue TValue;
	typedef typename util::CallTraits<T>::TParam TParam;
	typedef typename util::CallTraits<T>::TReference TReference;
	typedef typename util::CallTraits<T>::TConstReference TConstReference;
	typedef num::NumTraits< T > TNumTraits;

	FilteredFloat() : t_() 
	{
	}

	FilteredFloat(TParam t): 
		t_(t), e_(TNumTraits::zero)
	{
	}

	explicit FilteredFloat(TParam t, TParam e): 
		t_(t), e_(e)
	{
	}


	FilteredFloat(const TSelf& other):
		t_(other.t_)
	{
	}

	TParam value() const 
	{ 
		return t_; 
	}

	TParam error() const 
	{ 
		return e_; 
	}

	TSelf operator-() const
	{
		return TSelf(-t_,e_);
	}

	TSelf operator+() const
	{
		return *this;
	}

	TSelf& operator+=(const TSelf& other) 
	{ 
		T x = t_ + other.t_;
		T bv = x-t_;
		T av = x-bv;
		T br = other.t_-bv;
		T ar = t_-av;
		t_ = x; 
		e_ = ar+br; 
		return *this; 
	}
	TSelf& operator-=(const TSelf& other) 
	{ 
		*this += (-other);
		return *this; 
	}
	TSelf& operator*=(const TSelf& other) 
	{ 
		T x = t_ * other.t_;
		TSelf a = split(t_,(TNumTraits::mantisseSize+1)/2);
		TSelf b = split(other.t_,(TNumTraits::mantisseSize+1)/2);
		T err1 = x-(a.t_*b.t_);
		T err2 = err1-(a.e_*b.t_);
		T err3 = err2-(a.t_*b.e_);
		T y = (a.e_*b.e_)-err3;
		t_ = x; 
		e_ = y;
		return *this; 
	}
	TSelf& operator/=(const TSelf& other)
	{ 
#pragma LASS_TODO("We need to rederive the forward error for this operation!")
		TSelf newOther(T(1)/other.value(),0.0);
		this->operator *=(newOther);
		return *this; 
	}
	
	void swap(TSelf& other)
	{
		std::swap(t_, other.t_);
		std::swap(e_, other.e_);
	}

private:
	volatile TValue t_,		/**< the value */
					e_;		/**< the expected precision, based on forward error analysis */

	TSelf split(TParam a, int s)
	{
		static const T f = float((2<<s) + 1);
		T c = f*a;
		T abig = c-a;
		T ahi = c-abig;
		T alo = a-ahi;
		return TSelf(ahi,alo);
	}
};



template <typename T> inline
FilteredFloat<T> operator+(const FilteredFloat<T>& a, const FilteredFloat<T>& b)  
{ 
	FilteredFloat<T> result(a); 
	result += b; 
	return result; 
}

template <typename T> inline
FilteredFloat<T> operator-(const FilteredFloat<T>& a, const FilteredFloat<T>& b) 
{
	FilteredFloat<T> result(a);
	result -= b;
	return result;
}

template <typename T> inline
FilteredFloat<T> operator*(const FilteredFloat<T>& a, const FilteredFloat<T>& b) 
{
	FilteredFloat<T> result(a);
	result *= b;
	return result;
}

template <typename T> inline
FilteredFloat<T> operator/(const FilteredFloat<T>& a, const FilteredFloat<T>& b) 
{
	FilteredFloat<T> result(a);
	result /= b;
	return result;
}

template <typename T> inline
bool operator==(const FilteredFloat<T>& a, const FilteredFloat<T>& b) 
{
	return (a.value() == b.value() && a.error() == b.error());
}

template <typename T> inline
bool operator!=(const FilteredFloat<T>& a, const FilteredFloat<T>& b) 
{
	return !(a == b);
}

template <typename T> inline
bool operator<(const FilteredFloat<T>& a, const FilteredFloat<T>& b) 
{
	if (a.value()==b.value())
	{
		return a.error() < b.error();
	}
	return a.value() < b.value();
}

template <typename T> inline
bool operator>(const FilteredFloat<T>& a, const FilteredFloat<T>& b) 
{
	return b < a;
}

template <typename T> inline
bool operator<=(const FilteredFloat<T>& a, const FilteredFloat<T>& b) 
{
	return !(b < a);
}

template <typename T> inline
bool operator>=(const FilteredFloat<T>& a, const FilteredFloat<T>& b) 
{
	return !(a < b);
}




template <typename T> inline
FilteredFloat<T> operator+(const FilteredFloat<T>& a, const T& b)  
{ 
	FilteredFloat<T> result(a); 
	result += b; 
	return result; 
}

template <typename T> inline
FilteredFloat<T> operator-(const FilteredFloat<T>& a, const T& b) 
{
	FilteredFloat<T> result(a);
	result -= b;
	return result;
}

template <typename T> inline
FilteredFloat<T> operator*(const FilteredFloat<T>& a, const T& b) 
{
	FilteredFloat<T> result(a);
	result *= b;
	return result;
}

template <typename T> inline
FilteredFloat<T> operator/(const FilteredFloat<T>& a, const T& b) 
{
	FilteredFloat<T> result(a);
	result /= b;
	return result;
}

template <typename T> inline
bool operator==(const FilteredFloat<T>& a, const T& b) 
{
	return a == FilteredFloat<T>(b);
}

template <typename T> inline
bool operator!=(const FilteredFloat<T>& a, const T& b) 
{
	return !(a == b);
}

template <typename T> inline
bool operator<(const FilteredFloat<T>& a, const T& b) 
{
	return a < FilteredFloat<T>(b);
}

template <typename T> inline
bool operator>(const FilteredFloat<T>& a, const T& b) 
{
	return b < a;
}

template <typename T> inline
bool operator<=(const FilteredFloat<T>& a, const T& b) 
{
	return !(b < a);
}

template <typename T> inline
bool operator>=(const FilteredFloat<T>& a, const T& b) 
{
	return !(a < b);
}




template <typename T> inline
FilteredFloat<T> operator+(const T& a, const FilteredFloat<T>& b)  
{ 
	FilteredFloat<T> result(a); 
	result += b; 
	return result; 
}

template <typename T> inline
FilteredFloat<T> operator-(const T& a, const FilteredFloat<T>& b) 
{
	FilteredFloat<T> result(a);
	result -= b;
	return result;
}

template <typename T> inline
FilteredFloat<T> operator*(const T& a, const FilteredFloat<T>& b) 
{
	FilteredFloat<T> result(a);
	result *= b;
	return result;
}

template <typename T> inline
FilteredFloat<T> operator/(const T& a, const FilteredFloat<T>& b) 
{
	FilteredFloat<T> result(a);
	result /= b;
	return result;
}

template <typename T> inline
bool operator==(const T& a, const FilteredFloat<T>& b) 
{
	return FilteredFloat<T>(a) == b;
}

template <typename T> inline
bool operator!=(const T& a, const FilteredFloat<T>& b) 
{
	return !(a == b);
}

template <typename T> inline
bool operator<(const T& a, const FilteredFloat<T>& b) 
{
	return FilteredFloat<T>(a) < b;
}

template <typename T> inline
bool operator>(const T& a, const FilteredFloat<T>& b) 
{
	return b < a;
}

template <typename T> inline
bool operator<=(const T& a, const FilteredFloat<T>& b) 
{
	return !(b < a);
}

template <typename T> inline
bool operator>=(const T& a, const FilteredFloat<T>& b) 
{
	return !(a < b);
}





template <typename T> inline
FilteredFloat<T> abs(const FilteredFloat<T>& v)
{
	return num::abs(v.value());
}

template <typename T> inline
FilteredFloat<T> inv(const FilteredFloat<T>& v)
{
	return num::inv(v.value());
}

template <typename T> inline
FilteredFloat<T> sqrt(const FilteredFloat<T>& v)
{
	return num::sqrt(v.value());
}

template <typename T> inline
FilteredFloat<T> pow(const FilteredFloat<T>& v, const FilteredFloat<T>& p)
{
	return num::pow(v.value(), p.value());
}

template <typename T> inline
FilteredFloat<T> exp(const FilteredFloat<T>& v)
{
	return num::exp(v.value());
}

template <typename T> inline
FilteredFloat<T> log(const FilteredFloat<T>& v)
{
	return num::log(v.value());
}

template <typename T> inline
FilteredFloat<T> cos(const FilteredFloat<T>& v)
{
	return num::cos(v.value());
}

template <typename T> inline
FilteredFloat<T> sin(const FilteredFloat<T>& v)
{
	return num::sin(v.value());
}

template <typename T> inline
FilteredFloat<T> tan(const FilteredFloat<T>& v)
{
	return num::tan(v.value());
}

template <typename T> inline
FilteredFloat<T> acos(const FilteredFloat<T>& v)
{
	return num::acos(v.value());
}

template <typename T> inline
FilteredFloat<T> asin(const FilteredFloat<T>& v)
{
	return num::asin(v.value());
}

template <typename T> inline
FilteredFloat<T> atan(const FilteredFloat<T>& v)
{
	return num::atan(v.value());
}

template <typename T> inline
FilteredFloat<T> atan2(const FilteredFloat<T>& x, const FilteredFloat<T>& y)
{
	return num::atan2(x.value(), y.value());
}

template <typename T> inline
FilteredFloat<T> floor(const FilteredFloat<T>& v)
{
	return num::floor(v.value());
}

template <typename T> inline
FilteredFloat<T> ceil(const FilteredFloat<T>& v)
{
	return num::ceil(v.value());
}

template <typename T> inline
FilteredFloat<T> round(const FilteredFloat<T>& v)
{
	return num::round(v.value());
}

template <typename T> inline
FilteredFloat<T> fractional(const FilteredFloat<T>& v)
{
	return num::fractional(v.value());
}

template <typename T> inline
FilteredFloat<T> div(const FilteredFloat<T>& v, const FilteredFloat<T>& m)
{
	return num::div(v.value(), m.value());
}

template <typename T> inline
FilteredFloat<T> mod(const FilteredFloat<T>& v, const FilteredFloat<T>& m)
{
	return num::mod(v.value(), m.value());
}


#define LASS_NUM_FILTERED_FLOAT_DECLARE_NUMTRAITS(type)\
	template <>\
	struct NumTraits< FilteredFloat< type > >\
	{\
	private:\
		typedef NumTraits< type > TBaseTraits;\
	public:\
		typedef FilteredFloat< type > selfType;\
		typedef type baseType;\
		typedef type intervalType;\
		enum\
		{\
			isDistribution = TBaseTraits::isDistribution,\
			isIntegral = TBaseTraits::isIntegral,\
			isNative = false,\
			isSigned = TBaseTraits::isSigned,\
			hasInfinity = TBaseTraits::hasInfinity,\
			hasNaN = TBaseTraits::hasNaN\
		};\
		static const int memorySize;\
		static const std::string name() { return "FilteredFloat<" + TBaseTraits::name() + ">" ; }\
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

LASS_NUM_FILTERED_FLOAT_DECLARE_NUMTRAITS(float)
LASS_NUM_FILTERED_FLOAT_DECLARE_NUMTRAITS(double)
LASS_NUM_FILTERED_FLOAT_DECLARE_NUMTRAITS(long double)

template<class T> inline
bool isNaN( const FilteredFloat<T>& v )
{
	return num::isNaN(v.value());
}

template <typename T, typename Char, typename Traits>
std::basic_ostream<Char, Traits>& operator<<(std::basic_ostream<Char, Traits>& s, const FilteredFloat<T>& v)
{
	s << v.value();
	return s;
}

template <typename T, typename Char, typename Traits>
std::basic_istream<Char, Traits>& operator>>(std::basic_istream<Char, Traits>& s, FilteredFloat<T>& v)
{
	T temp;
	s >> temp;
	v = FilteredFloat<T>(temp);
	return s;
}

}

}

namespace std
{

template <typename T>
void swap(::lass::num::FilteredFloat<T>& a, ::lass::num::FilteredFloat<T>& b)
{
	a.swap(b);
}

}

#endif
