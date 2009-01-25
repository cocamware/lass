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
 *	Copyright (C) 2004-2009 the Initial Developer.
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



#ifndef LASS_GUARDIAN_OF_INCLUSION_NUM_INTERVAL_H
#define LASS_GUARDIAN_OF_INCLUSION_NUM_INTERVAL_H

#include "num_common.h"
#include "num_traits.h"
#include "../util/call_traits.h"
#include "../python/pyobject_plus.h"
#include <iostream>

namespace lass
{
namespace num
{

template<typename C> class interval;

template<typename C> void inpsqr(interval<C>& i);
template<typename C> void inpsqrt(interval<C>& i);
template<typename C> void inpexp(interval<C>& i);
template<typename C> void inplog(interval<C>& i);
template<typename C> void inpnorm(interval<C>& i);
template<typename C> void inpinv(interval<C>& i);
template<typename C, typename F> interval<C> applyFunction(const interval<C>& i, F func);

/** Interval class.  Class for interval arithmetic.  The arithmetic is weak interval arithmetic: there is
*   no rounding code, to speed up things.  It should however be useful except for numerical stability
*   studies on the bitlevel.  If you wont strong interval arithmetic, look for the boost library.
* @warning DO NOT derive a class from this one which uses dynamic memory management! destructor is not virtual.
* This is to avoid the functionptrtable to be stored in the objects.  Therefor none of the methods are virtual
* @author Tom De Muer
* @date   2003
*/
template<typename C>
class interval
{
private:
	C v[2];
public:
	typedef C   baseType;

	static  const interval<C>   empty;

	inline interval() { v[0]=v[1]=C();};
	inline ~interval() {};
	inline explicit interval(typename util::CallTraits<baseType>::TParam iValue);
	inline explicit interval(typename util::CallTraits<baseType>::TParam iInf,typename util::CallTraits<baseType>::TParam iSup);
	inline interval& operator=(const interval<C>& i);

	inline const interval&  operator+() const;
	inline interval         operator-() const;

	/** direct set of both infinum and supinum */
	inline void set( typename util::CallTraits<C>::TParam iInf,typename util::CallTraits<C>::TParam iSup);
	/** returns true for the empty interval */
	inline bool isEmpty() const;
	/** return true for degenerated intervals */
	inline bool isSingleton() const;
	/** returns true when ix is in the interval */
	inline bool contains(typename util::CallTraits<baseType>::TParam ix) const;

	/** indexed access, 0 = infinum, 1 = supinum */
	inline baseType&    operator[](int i);
	/** indexed access, 0 = infinum, 1 = supinum */
	inline baseType operator[](int i) const;
	/** the infinum of the interval */
	inline baseType inf() const;
	/** the supinum of the interval */
	inline baseType sup() const;
	/** the infinum of the interval */
	inline baseType&    inf();
	/** the supinum of the interval */
	inline baseType&    sup();
	/** the middle of the interval */
	inline baseType mid() const;
	/** the difference between supinum and infinum */
	inline baseType diam()const;
	/** the ratio of the supinum over the infinum */
	inline baseType ratio() const;

	/** certainly greater than */
	inline bool operator> (const interval<C>& i) const;
	/** certainly less than */
	inline bool operator< (const interval<C>& i) const;
	/** certainly equal */
	inline bool operator==(const interval<C>& i) const;
	/** certainly not equal */
	inline bool operator!=(const interval<C>& i) const;
	/** certainly greater or equal than */
	inline bool operator>=(const interval<C>& i) const;
	/** certainly less or equal than */
	inline bool operator<=(const interval<C>& i) const;

	/** possibly equal */
	inline bool pe(const interval<C>& i) const;
	/** possibly not equal */
	inline bool pne(const interval<C>& i) const;
	/** possibly greater than */
	inline bool pg(const interval<C>& i) const;
	/** possibly greater or equal than */
	inline bool pge(const interval<C>& i) const;
	/** possibly less than */
	inline bool pl(const interval<C>& i) const;
	/** possibly less or equal than */
	inline bool ple(const interval<C>& i) const;

	/** fuzzy equal */
	baseType    fe(const interval<C>& i) const;
	/** fuzzy not equal */
	inline C    fne(const interval<C>& i) const;
	/** fuzzy greater than*/
	baseType    fg(const interval<C>& i) const;
	/** fuzzy greater or equal than*/
	baseType    fge(const interval<C>& i) const;
	/** fuzzy less than */
	baseType    fl(const interval<C>& i) const;
	/** fuzzy less or equal than*/
	baseType    fle(const interval<C>& i) const;

	inline interval&    operator+=(const interval<C>& i);
	inline interval&    operator-=(const interval<C>& i);
	inline interval&    operator*=(const interval<C>& i);
		   interval&    operator/=(const interval<C>& i);

	inline interval&    operator+=(typename util::CallTraits<baseType>::TParam s);
	inline interval&    operator-=(typename util::CallTraits<baseType>::TParam s);
	inline interval&    operator*=(typename util::CallTraits<baseType>::TParam s);
	inline interval&    operator/=(typename util::CallTraits<baseType>::TParam s);

	friend void inpsqr<C>(interval<C>& i);
	friend void inpsqrt<C>(interval<C>& i);
	friend void inpexp<C>(interval<C>& i);
	friend void inplog<C>(interval<C>& i);
	friend void inpnorm<C>(interval<C>& i);
	friend void inpinv<C>(interval<C>& i);

	template<typename C2, typename F> friend interval<C2> applyFunction(const interval<C2>& iV, F func );

};

template<typename C> interval<C>    operator+(const interval<C>& i1,const interval<C>& i2);
template<typename C> interval<C>    operator-(const interval<C>& i1,const interval<C>& i2);
template<typename C> interval<C>    operator*(const interval<C>& i1,const interval<C>& i2);
template<typename C> interval<C>    operator/(const interval<C>& i1,const interval<C>& i2);

template<typename C> std::ostream& operator<<( std::ostream& os, const interval<C>& iV );
template<typename C> std::string str( const interval<C>& iV );

template<typename C> const interval<C> interval<C>::empty = interval<C>(NumTraits<C>::one, NumTraits<C>::zero);

// implementation of basic_ops

template<typename C> interval<C> sqr(const interval<C>& i);
template<typename C> interval<C> sqrt(const interval<C>& i);
template<typename C> interval<C> exp(const interval<C>& i);
template<typename C> interval<C> log(const interval<C>& i);

template<typename C>    interval<C> set_union(const interval<C>& i1, const interval<C>& i2);
template<typename C>    interval<C> set_intersect(const interval<C>& i1, const interval<C>& i2);

// num traits partial specialisation

template< class C >
struct NumTraits< interval<C> >
{
	typedef interval<C> selfType;
	typedef C           baseType;
	typedef C           intervalType;

	enum
	{
		isDistribution = 1,
		isIntegral = 0,
		isNative = 0,
		isSigned = 1,
		hasInfinity = 0,
	};

	static const size_t memorySize;
	static const std::string name();

	static const interval<C>    one;
	static const interval<C>    zero;

	static const interval<C> epsilon;
	static const interval<C> min;
	static const interval<C> max;
	static const interval<C> minStrictPositive;
	static const interval<C> pi;
	static const interval<C> e;
	static const interval<C> sqrt2;
	static const interval<C> sqrtPi;
};


template<class C> const size_t NumTraits<interval<C> >::memorySize = sizeof(interval<C>);
template<class C> const std::string NumTraits<interval<C> >::name() { return std::string("interval<")+NumTraits<baseType>::name()+ std::string(">"); }

template<class C> const interval<C> NumTraits<interval<C> >::one = interval<C>( NumTraits<C>::one );
template<class C> const interval<C> NumTraits<interval<C> >::zero = interval<C>( NumTraits<C>::zero );

template<class C> const interval<C> NumTraits<interval<C> >::epsilon = interval<C>( NumTraits<C>::epsilon );
template<class C> const interval<C> NumTraits<interval<C> >::min = interval<C>( NumTraits<C>::min );
template<class C> const interval<C> NumTraits<interval<C> >::max = interval<C>( NumTraits<C>::max );
template<class C> const interval<C> NumTraits<interval<C> >::minStrictPositive = interval<C>( NumTraits<C>::minStrictPositive );
template<class C> const interval<C> NumTraits<interval<C> >::pi = interval<C>( NumTraits<C>::pi );
template<class C> const interval<C> NumTraits<interval<C> >::e = interval<C>( NumTraits<C>::e );
template<class C> const interval<C> NumTraits<interval<C> >::sqrt2 = interval<C>( NumTraits<C>::sqrt2 );
template<class C> const interval<C> NumTraits<interval<C> >::sqrtPi = interval<C>( NumTraits<C>::sqrtPi );

#include "interval.inl"

}

namespace python
{
	template<class C> int pyGetSimpleObject_deprecated( PyObject* iValue, lass::num::interval<C>& oV )
	{
		std::pair<C,C> temp;
		int r=PyExportTraits<std::pair<C,C> >::get(iValue, temp);
		if (r==0)
		{
			oV.set(temp.first , temp.second);
		}
		return r;
	}
	template<class C> PyObject* pyBuildSimpleObject_deprecated( const lass::num::interval<C>& iV )
	{
		return PyExportTraits<std::pair<C,C> >::build( std::pair<C,C>(iV.inf(),iV.sup()) );
	}
	PYEXPORTTRAITS_USINGDEPRECATED_TEMPL( lass::num::interval )

}

}


#endif
