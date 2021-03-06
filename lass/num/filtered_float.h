/**	@file
 *	@author Bram de Greve (bram@cocamware.com)
 *	@author Tom De Muer (tom@cocamware.com)
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
 *	Copyright (C) 2004-2011 the Initial Developer.
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
		t_(t), e_(TNumTraits::epsilon*t)
	{
	}

	explicit FilteredFloat(TParam t, TParam e): 
		t_(t), e_(e)
	{
	}


	FilteredFloat(const TSelf& other):
		t_(other.t_), e_(other.e_)
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
		return TSelf(-t_,-e_);
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
		LASS_ENFORCE(isValid());
		return *this; 
	}
	TSelf& operator-=(const TSelf& other) 
	{ 
		T x = t_ - other.t_;
		T bv = t_-x;
		T av = x+bv;
		T br = bv-other.t_;
		T ar = t_-av;
		t_ = x; 
		e_ = ar+br;
		LASS_ENFORCE(isValid());
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
		LASS_ENFORCE(isValid());
		return *this; 
	}
	TSelf& operator/=(const TSelf& other)
	{ 
// TODO: We need to rederive the forward error for this operation!
// https://sourceforge.net/tracker2/?func=detail&aid=2517783&group_id=118315&atid=680768
		TSelf newOther(T(1)/other.value(),0.0);
		this->operator *=(newOther);
		LASS_ENFORCE(isValid());
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

	bool isValid() const
	{
		return num::abs(static_cast<T>(t_)) >= num::abs(static_cast<T>(e_));
	}

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
	//return a.value()+a.error() < b.value()-b.error();;
	LASS_ENFORCE( (a.value()+b.error() < b.value()-a.error()) == (a.value()<b.value()));
	return a.value()<b.value();
	/*
	if (a.value()==b.value())
	{
		return a.error() < b.error();
	}
	return a.value() < b.value();
	*/
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
FilteredFloat<T> atan2(const FilteredFloat<T>& y, const FilteredFloat<T>& x)
{
	return num::atan2(y.value(), x.value());
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
