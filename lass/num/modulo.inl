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
 *	Copyright (C) 2004-2024 the Initial Developer.
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



#ifndef LASS_GUARDIAN_OF_INCLUSION_NUM_MODULO_INL
#define LASS_GUARDIAN_OF_INCLUSION_NUM_MODULO_INL

#include "num_common.h"
#include "modulo.h"
#include "basic_ops.h"



namespace lass
{
namespace num
{

// --- public --------------------------------------------------------------------------------------

template <unsigned N, typename T>
constexpr Modulo<N, T>::Modulo():
	value_(0)
{
}



template <unsigned N, typename T>
constexpr Modulo<N, T>::Modulo(TParam value):
	value_(static_cast<TValue>(mod(value, N)))
{
}



template <unsigned N, typename T> inline
constexpr Modulo<N, T> Modulo<N, T>::operator+() const
{
	return *this;
}



template <unsigned N, typename T>
constexpr Modulo<N, T> Modulo<N, T>::operator-() const
{
	return Modulo<N, T>(
		value_ == 0 ? 0 : static_cast<T>(N) - value_,
		false);
}



template <unsigned N, typename T>
constexpr Modulo<N, T>& Modulo<N, T>::operator++()
{
	value_ = value_ + 1 == static_cast<T>(N) ? 0 : value_ + 1;
	LASS_ASSERT(isInRange(value_));
	return *this;
}



template <unsigned N, typename T>
constexpr Modulo<N, T>& Modulo<N, T>::operator--()
{
	value_ = value_ == 0 ? static_cast<T>(N) - 1 : value_ - 1;
	LASS_ASSERT(isInRange(value_));
	return *this;
}



template <unsigned N, typename T>
constexpr Modulo<N, T> Modulo<N, T>::operator++(int)
{
	Modulo<N, T> result(*this);
	++*this;
	return result;
}



template <unsigned N, typename T>
constexpr Modulo<N, T> Modulo<N, T>::operator--(int)
{
	Modulo<N, T> result(*this);
	--*this;
	return result;
}



template <unsigned N, typename T>
constexpr Modulo<N, T>& Modulo<N, T>::operator+=(const Modulo<N, T>& other)
{
	value_ += other.value_;
	if (value_ >= static_cast<T>(N))
	{
		value_ -= N;
	}
	LASS_ASSERT(isInRange(value_));
	return *this;
}



template <unsigned N, typename T>
constexpr Modulo<N, T>& Modulo<N, T>::operator-=(const Modulo<N, T>& other)
{
	value_ -= other.value_;
	if (value_ < 0)
	{
		value_ += N;
	}
	LASS_ASSERT(isInRange(value_));
	return *this;
}



template <unsigned N, typename T>
constexpr Modulo<N, T>& Modulo<N, T>::operator*=(const Modulo<N, T>& other)
{
	*this = TSelf(value_ * other.value_);
	return *this;
}



/*
template <unsigned N, typename T>
constexpr Modulo<N, T>& Modulo<N, T>::operator/=(const Modulo<N, T>& other)
{
	value_ /= other.value_;
	value_ = mod(value_, N);
	LASS_ASSERT(isInRange(value_));
	return *this;
}
*/


// --- protected -----------------------------------------------------------------------------------



// --- private -------------------------------------------------------------------------------------

/** private constructor bypassing mod
 */
template <unsigned N, typename T>
constexpr Modulo<N, T>::Modulo(TParam value, bool):
	value_(value)
{
	LASS_ASSERT(isInRange(value));
}



template <unsigned N, typename T>
constexpr bool Modulo<N, T>::isInRange(TParam value) const
{
	return value >= 0 && value < static_cast<T>(N);
}



// --- free ----------------------------------------------------------------------------------------

template <unsigned N, typename T>
constexpr bool operator==(const Modulo<N, T>& a, const Modulo<N, T>& b)
{
	return a.value() == b.value();
}



template <unsigned N, typename T>
constexpr bool operator!=(const Modulo<N, T>& a, const Modulo<N, T>& b)
{
	return !(a == b);
}



template <unsigned N, typename T>
constexpr bool operator<(const Modulo<N, T>& a, const Modulo<N, T>& b)
{
	return a.value() < b.value();
}



template <unsigned N, typename T>
constexpr bool operator>(const Modulo<N, T>& a, const Modulo<N, T>& b)
{
	return b < a;
}



template <unsigned N, typename T>
constexpr bool operator<=(const Modulo<N, T>& a, const Modulo<N, T>& b)
{
	return !(b < a);
}



template <unsigned N, typename T>
constexpr bool operator>=(const Modulo<N, T>& a, const Modulo<N, T>& b)
{
	return !(a < b);
}



template <unsigned N, typename T>
std::ostream& operator<<(std::ostream& stream, const Modulo<N, T>& a)
{
	stream << a.value();
	return stream;
}



template <unsigned N, typename T>
std::istream& operator>>(std::istream& stream, Modulo<N, T>& a)
{
	T result;
	LASS_ENFORCE(stream) >> result;
	a = Modulo<N, T>(result);
	return stream;
}



}

}

#endif

// EOF
