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



/** @class lass::num::Modulo
 *  @brief a integer number class using modulo arithmetic
 *  @author Bram de Greve [BdG]
 *
 *  @warning under development!
 *
 *  @bug what happens if the temporary result of an operation does not fit in the range of T?
 */

#ifndef LASS_GUARDIAN_OF_INCLUSION_NUM_MODULO_H
#define LASS_GUARDIAN_OF_INCLUSION_NUM_MODULO_H

#include "num_common.h"
#include "../util/call_traits.h"

namespace lass
{
namespace num
{

template <unsigned N, typename T = int>
class Modulo
{
public:

	typedef Modulo<N, T> TSelf;

	typedef typename util::CallTraits<T>::TValue TValue;
	typedef typename util::CallTraits<T>::TParam TParam;

	template <typename U>
	struct Rebind
	{
		typedef Modulo<N, U> Type;
	};

	Modulo(TParam iValue = 0);

	Modulo<N, T> operator+() const;
	Modulo<N, T> operator-() const;

	Modulo<N, T>& operator++();
	Modulo<N, T>& operator--();
	Modulo<N, T> operator++(int);
	Modulo<N, T> operator--(int);

	Modulo<N, T>& operator+=(const Modulo<N, T>& iB);
	Modulo<N, T>& operator-=(const Modulo<N, T>& iB);
	Modulo<N, T>& operator*=(const Modulo<N, T>& iB);
	Modulo<N, T>& operator/=(const Modulo<N, T>& iB);

	operator T() const;
	TParam value() const;

private:

	bool isInRange(TParam iValue) const;

	TValue value_;
};

template <unsigned N, typename T>
bool operator==(const Modulo<N, T>& iA, const Modulo<N, T>& iB);

template <unsigned N, typename T>
bool operator!=(const Modulo<N, T>& iA, const Modulo<N, T>& iB);

template <unsigned N, typename T>
bool operator<(const Modulo<N, T>& iA, const Modulo<N, T>& iB);

template <unsigned N, typename T>
bool operator>(const Modulo<N, T>& iA, const Modulo<N, T>& iB);

template <unsigned N, typename T>
bool operator<=(const Modulo<N, T>& iA, const Modulo<N, T>& iB);

template <unsigned N, typename T>
bool operator>=(const Modulo<N, T>& iA, const Modulo<N, T>& iB);

template <unsigned N, typename T>
std::ostream& operator<<(std::ostream& iS, const Modulo<N, T>& iM);

template <unsigned N, typename T>
std::istream& operator>>(std::istream& iS, Modulo<N, T>& iM);

}

}

#include "modulo.inl"

#endif

// EOF
