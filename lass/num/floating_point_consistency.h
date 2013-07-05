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


#ifndef LASS_GUARDIAN_OF_INCLUSION_NUM_FLOATING_POINT_CONSISTENCY_H
#define LASS_GUARDIAN_OF_INCLUSION_NUM_FLOATING_POINT_CONSISTENCY_H

#include "num_common.h"
#include "../util/call_traits.h"
#include <complex>

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

	Consistent() : t_() 
	{
	}

	Consistent(TParam t): 
		t_(t) 
	{
	}

	Consistent(const TSelf& other):
		t_(other.t_)
	{
	}

	TParam value() const 
	{ 
		return t_; 
	}

	TSelf operator-() const
	{
		return TSelf(-t_);
	}

	TSelf operator+() const
	{
		return *this;
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
	return Consistent<T>(a.value() + b.value()); 
}

template <typename T> inline
Consistent<T> operator-(const Consistent<T>& a, const Consistent<T>& b) 
{
	return Consistent<T>(a.value() - b.value()); 
}

template <typename T> inline
Consistent<T> operator*(const Consistent<T>& a, const Consistent<T>& b) 
{
	return Consistent<T>(a.value() * b.value()); 
}

template <typename T> inline
Consistent<T> operator/(const Consistent<T>& a, const Consistent<T>& b) 
{
	return Consistent<T>(a.value() / b.value()); 
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
	return Consistent<T>(a.value() + b); 
}

template <typename T> inline
Consistent<T> operator-(const Consistent<T>& a, const T& b) 
{
	return Consistent<T>(a.value() - b); 
}

template <typename T> inline
Consistent<T> operator*(const Consistent<T>& a, const T& b) 
{
	return Consistent<T>(a.value() * b); 
}

template <typename T> inline
Consistent<T> operator/(const Consistent<T>& a, const T& b) 
{
	return Consistent<T>(a.value() / b); 
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
	return Consistent<T>(a + b.value()); 
}

template <typename T> inline
Consistent<T> operator-(const T& a, const Consistent<T>& b) 
{
	return Consistent<T>(a - b.value()); 
}

template <typename T> inline
Consistent<T> operator*(const T& a, const Consistent<T>& b) 
{
	return Consistent<T>(a * b.value()); 
}

template <typename T> inline
Consistent<T> operator/(const T& a, const Consistent<T>& b) 
{
	return Consistent<T>(a / b.value()); 
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
/*
template <typename T> inline
Consistent<T> sqrt(const Consistent<T>& v)
{
	return num::sqrt(v.value());
}
*/
template <typename T> inline
Consistent<T> sqrt(Consistent<T> v)
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
Consistent<T> atan2(const Consistent<T>& y, const Consistent<T>& x)
{
	return num::atan2(y.value(), x.value());
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
	struct LASS_DLL NumTraits< Consistent< type > >\
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
LASS_NUM_FLOATING_POINT_CONSISTENCY_DECLARE_NUMTRAITS(long double)

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
