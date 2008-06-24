/**	@file
 *	@author Bram de Greve (bramz@users.sourceforge.net)
 *	@author Tom De Muer (tomdemuer@users.sourceforge.net)
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
 *	Copyright (C) 2004-2007 the Initial Developer.
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
Modulo<N, T>::Modulo(TParam iValue):
	value_(mod(iValue, N))
{
	LASS_ASSERT(static_cast<T>(N) > 0 && static_cast<T>(N) < NumTraits<T>::max);
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
	return iValue >= 0 && iValue < static_cast<T>(N);
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
