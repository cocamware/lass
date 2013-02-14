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

#ifndef LASS_GUARDIAN_OF_INCLUSION_NUM_POLYNOMIAL_INL
#define LASS_GUARDIAN_OF_INCLUSION_NUM_POLYNOMIAL_INL

#include "num_common.h"
#include "polynomial.h"

namespace lass
{
namespace num
{

// --- public --------------------------------------------------------------------------------------

template <typename T>
Polynomial<T>::Polynomial()
{
}


template <typename T>
Polynomial<T>::Polynomial(TParam iScalar):
	a_(1, iScalar)
{
}



template <typename T>
Polynomial<T>::Polynomial(const TCoefficients& iCoefficients):
	a_(iCoefficients)
{
}


template <typename T>
template <typename InputIterator> 
Polynomial<T>::Polynomial(InputIterator iBegin, InputIterator iEnd):
	a_(iBegin, iEnd)
{
}



template <typename T> inline
const typename Polynomial<T>::TCoefficients& 
Polynomial<T>::coefficients() const
{
	return a_;
}



template <typename T> inline
const typename Polynomial<T>::TValue
Polynomial<T>::operator[](size_t iIndex) const
{
	LASS_ASSERT(iIndex < a_.size());
	return a_[iIndex];
}



template <typename T> inline
const typename Polynomial<T>::TValue 
Polynomial<T>::at(size_t iIndex) const
{
	return iIndex < a_.size() ? a_[iIndex] : TNumTraits::zero;
}



template <typename T>
const typename Polynomial<T>::TValue 
Polynomial<T>::operator()(TParam iX) const
{
	TValue result = TNumTraits::zero;
	TValue x = TNumTraits::one;
	const size_t n = a_.size();
	for (size_t i = 0; i < n; ++i)
	{
		result += a_[i] * x;
		x *= iX;
	}
	return result;
}



template <typename T>
const Polynomial<T>& Polynomial<T>::operator+() const
{
	return *this;
}



template <typename T>
const Polynomial<T> Polynomial<T>::operator-() const
{
	const size_t n = a_.size();
	TCoefficients result(n);
	for (size_t i = 0; i < n; ++i)
	{
		result[i] = -a_[i];
	}
	return Polynomial<T>(result);
}



template <typename T>
Polynomial<T>& Polynomial<T>::operator+=(const Polynomial<T>& iOther)
{
	const size_t n = iOther.a_.size();
	if (a_.size() < n)
	{
		a_.resize(n, TNumTraits::zero);
	}
	for (size_t i = 0; i < n; ++i)
	{
		a_[i] += iOther.a_[i];
	}
	return *this;
}



template <typename T>
Polynomial<T>& Polynomial<T>::operator-=(const Polynomial<T>& iOther)
{
	const size_t n = iOther.a_.size();
	if (a_.size() < n)
	{
		a_.resize(n, TNumTraits::zero);
	}
	for (size_t i = 0; i < n; ++i)
	{
		a_[i] -= iOther.a_[i];
	}
	return *this;
}



template <typename T>
Polynomial<T>& Polynomial<T>::operator*=(const Polynomial<T>& iOther)
{
	const size_t m = a_.size();
	const size_t n = iOther.a_.size();
	TCoefficients result(m + n - 1, TNumTraits::zero);
	for (size_t i = 0; i < m; ++i)
	{
		const TValue a = a_[i];
		for (size_t k = 0; k < n; ++k)
		{
			result[i + k] += a * iOther.a_[k];
		}
	}
	a_.swap(result);
	return *this;
}



template <typename T>
Polynomial<T>& Polynomial<T>::operator+=(TParam iScalar)
{
	if (a_.empty())
	{
		a_.resize(1, iScalar);
	}
	else
	{
		a_[0] += iScalar;
	}
	return *this;
}



template <typename T>
Polynomial<T>& Polynomial<T>::operator-=(TParam iScalar)
{
	if (a_.empty())
	{
		a_.resize(1, -iScalar);
	}
	else
	{
		a_[0] -= iScalar;
	}
	return *this;
}



template <typename T>
Polynomial<T>& Polynomial<T>::operator*=(TParam iScalar)
{
	const size_t n = a_.size();
	for (size_t i = 0; i < n; ++i)
	{
		a_[i] *= iScalar;
	}
	return *this;
}



template <typename T>
Polynomial<T>& Polynomial<T>::operator/=(TParam iScalar)
{
	const size_t n = a_.size();
	for (size_t i = 0; i < n; ++i)
	{
		a_[i] /= iScalar;
	}
	return *this;
}



template <typename T>
Polynomial<T> Polynomial<T>::derivative() const
{
	const size_t n = a_.size();
	if (n < 2)
	{
		return Polynomial<T>();
	}
	TCoefficients result(n - 1);
	for (size_t i = 1; i < n; ++i)
	{
		result[i - 1] = static_cast<typename TNumTraits::baseType>(i) * a_[i];
	}
	return Polynomial<T>(result);
}



template <typename T>
Polynomial<T> Polynomial<T>::integral() const
{
	const size_t n = a_.size();
	if (n == 0)
	{
		return Polynomial<T>();
	}
	TCoefficients result(n + 1);
	result[0] = TNumTraits::zero;
	for (size_t i = 0; i < n; ++i)
	{
		result[i + 1] = a_[i] / static_cast<typename TNumTraits::baseType>(i);
	}
	return Polynomial<T>(result);
}



template <typename T>
Polynomial<T> Polynomial<T>::pow(unsigned iPower) const
{
	Polynomial<T> result(1);
	for (unsigned i = 0; i < iPower; ++i)
	{
		result *= *this;
	}
	return result;
}



/** return size of coefficients.
 */
template <typename T>
const typename Polynomial<T>::size_type Polynomial<T>::size() const
{
	return a_.size();
}



/** return iterator to first (lowest) coefficient
 */
template <typename T>
const typename Polynomial<T>::const_iterator Polynomial<T>::begin() const
{
	return a_.begin();
}



/** return iterator to last (highest) coefficient
 */
template <typename T>
const typename Polynomial<T>::const_iterator Polynomial<T>::end() const
{
	return a_.end();
}



/** return constant polynomial 1
 */
template <typename T>
Polynomial<T> Polynomial<T>::one()
{
	static Polynomial<T> result(1);
	return result;
}



/** return linear polynomial x
 */
template <typename T>
Polynomial<T> Polynomial<T>::x()
{
	static TValue coefficients[2] = { 0, 1 };
	static Polynomial<T> result(coefficients, coefficients + 2);
	return result;
}



// --- free ----------------------------------------------------------------------------------------

template <typename T> 
bool operator==(const Polynomial<T>& iA, const Polynomial<T>& iB)
{
	typedef typename Polynomial<T>::TCoefficients TCoefficients;
	const TCoefficients& a = iA.coefficients();
	const TCoefficients& b = iB.coefficients();
	const size_t m = a.size();
	const size_t n = b.size();
	if (m != n)
	{
		return false;
	}
	for (size_t i = 0; i < n; ++i)
	{
		if (a[i] != b[i])
		{
			return false;
		}
	}
	return true;
}



template <typename T> inline 
bool operator!=(const Polynomial<T>& iA, const Polynomial<T>& iB)
{
	return !(iA == iB);
}



template <typename T> inline
Polynomial<T> operator+(const Polynomial<T>& iA, const Polynomial<T>& iB)
{
	Polynomial<T> result(iA);
	result += iB;
	return result;
}



template <typename T> inline
Polynomial<T> operator-(const Polynomial<T>& iA, const Polynomial<T>& iB)
{
	Polynomial<T> result(iA);
	result -= iB;
	return result;
}



template <typename T> inline
Polynomial<T> operator*(const Polynomial<T>& iA, const Polynomial<T>& iB)
{
	Polynomial<T> result(iA);
	result *= iB;
	return result;
}



template <typename T> inline
Polynomial<T> operator+(const T& iA, const Polynomial<T>& iB)
{
	Polynomial<T> result(iB);
	result += iA;
	return result;
}



template <typename T> inline
Polynomial<T> operator-(const T& iA, const Polynomial<T>& iB)
{
	Polynomial<T> result(-iB);
	result += iA;
	return result;
}



template <typename T> inline
Polynomial<T> operator*(const T& iA, const Polynomial<T>& iB)
{
	Polynomial<T> result(iB);
	result *= iA;
	return result;
}



template <typename T> inline
Polynomial<T> operator+(const Polynomial<T>& iA, const T& iB)
{
	Polynomial<T> result(iA);
	result += iB;
	return result;
}



template <typename T> inline
Polynomial<T> operator-(const Polynomial<T>& iA, const T& iB)
{
	Polynomial<T> result(iA);
	result -= iB;
	return result;
}



template <typename T> inline
Polynomial<T> operator*(const Polynomial<T>& iA, const T& iB)
{
	Polynomial<T> result(iA);
	result *= iB;
	return result;
}



template <typename T> inline
Polynomial<T> operator/(const Polynomial<T>& iA, const T& iB)
{
	Polynomial<T> result(iA);
	result /= iB;
	return result;
}



template <typename T, typename Char, typename Traits>
std::basic_ostream<Char, Traits>&
operator<<(std::basic_ostream<Char, Traits>& iS, const Polynomial<T>& iA)
{
	typedef typename Polynomial<T>::TCoefficients TCoefficients;
	const TCoefficients& a = iA.coefficients();
	const size_t n = a.size();
	if (n == 0)
	{
		iS << T();
		return iS;
	}
	iS << a[0];
	for (size_t i = 1; i < n; ++i)
	{
		iS << " + " << a[i] << "x^" << static_cast<unsigned long>(i);
	}
	return iS;
}



}

}

#endif

// EOF
