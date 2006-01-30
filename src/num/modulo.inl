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
Modulo<N, T>::Modulo(TParam iValue):
	value_(mod(iValue, N))
{
	LASS_ASSERT(N <= NumTraits<T>::max);
}



template <unsigned N, typename T> inline
Modulo<N, T> Modulo<N, T>::operator+() const
{
	return *this;
}



template <unsigned N, typename T>
Modulo<N, T> Modulo<N, T>::operator-() const
{
	return Modulo<N, T>(-value_);
}



template <unsigned N, typename T>
Modulo<N, T>& Modulo<N, T>::operator++()
{
	++value_;
	if (value_ == N)
	{
		value_ = 0;
	}
	LASS_ASSERT(isInRange(value_));
	return *this;
}



template <unsigned N, typename T>
Modulo<N, T>& Modulo<N, T>::operator--()
{
	if (value_ == 0)
	{
		value_ = N;
	}
	--value_;
	LASS_ASSERT(isInRange(value_));
	return *this;
}



template <unsigned N, typename T>
Modulo<N, T> Modulo<N, T>::operator++(int)
{
	Modulo<N, T> result(*this);
	++*this;
	return result;
}



template <unsigned N, typename T>
Modulo<N, T> Modulo<N, T>::operator--(int)
{
	Modulo<N, T> result(*this);
	--*this;
	return result;
}



template <unsigned N, typename T>
Modulo<N, T>& Modulo<N, T>::operator+=(const Modulo<N, T>& iOther)
{
	value_ += iOther.value_;
	if (value_ >= static_cast<T>(N))
	{
		value_ -= N;
	}
	LASS_ASSERT(isInRange(value_));
	return *this;
}



template <unsigned N, typename T>
Modulo<N, T>& Modulo<N, T>::operator-=(const Modulo<N, T>& iOther)
{
	value_ -= iOther.value_;
	if (value_ < 0)
	{
		value_ += N;
	}
	LASS_ASSERT(isInRange(value_));
	return *this;
}



template <unsigned N, typename T>
Modulo<N, T>& Modulo<N, T>::operator*=(const Modulo<N, T>& iOther)
{
	value_ *= iOther.value_;
	value_ = mod(value_, N);
	LASS_ASSERT(isInRange(value_));
	return *this;
}



template <unsigned N, typename T>
Modulo<N, T>& Modulo<N, T>::operator/=(const Modulo<N, T>& iOther)
{
	value_ /= iOther.value_;
	value_ = mod(value_, N);
	LASS_ASSERT(isInRange(value_));
	return *this;
}



template <unsigned N, typename T> inline
Modulo<N, T>::operator T() const
{
	return value_;
}



template <unsigned N, typename T> inline
typename Modulo<N, T>::TParam Modulo<N, T>::value() const
{
	return value_;
}



// --- protected -----------------------------------------------------------------------------------



// --- private -------------------------------------------------------------------------------------

template <unsigned N, typename T>
bool Modulo<N, T>::isInRange(TParam iValue) const
{
	return iValue >= 0 && iValue < N;
}



// --- free ----------------------------------------------------------------------------------------

template <unsigned N, typename T>
bool operator==(const Modulo<N, T>& iA, const Modulo<N, T>& iB)
{
	return iA.value() == iB.value();
}



template <unsigned N, typename T>
bool operator!=(const Modulo<N, T>& iA, const Modulo<N, T>& iB)
{
	return !(iA == iB);
}



template <unsigned N, typename T>
bool operator<(const Modulo<N, T>& iA, const Modulo<N, T>& iB)
{
	return iA.value() < iB.value_();
}



template <unsigned N, typename T>
bool operator>(const Modulo<N, T>& iA, const Modulo<N, T>& iB)
{
	return iB < iA;
}



template <unsigned N, typename T>
bool operator<=(const Modulo<N, T>& iA, const Modulo<N, T>& iB)
{
	return !(iB < iA);
}



template <unsigned N, typename T>
bool operator>=(const Modulo<N, T>& iA, const Modulo<N, T>& iB)
{
	return !(iA < iB);
}



template <unsigned N, typename T>
std::ostream& operator<<(std::ostream& oS, Modulo<N, T> iM)
{
	oS << iM.value();
	return oS;
}



template <unsigned N, typename T>
std::istream& operator>>(std::istream& iS, Modulo<N, T>& iM)
{
	int result;
	LASS_ENFORCE(iS) >> result;
	iM = Modulo<N, T>(result);
	return iS;
}



}

}

#endif

// EOF
