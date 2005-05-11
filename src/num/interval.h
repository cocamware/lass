/** @file
 *  @author Bram de Greve (bramz@users.sourceforge.net)
 *  @author Tom De Muer (tomdemuer@users.sourceforge.net)
 *
 *  Distributed under the terms of the GPL (GNU Public License)
 *
 *  The LASS License:
 *
 *  Copyright 2004 Bram de Greve and Tom De Muer
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



#ifndef LASS_GUARDIAN_OF_INCLUSION_NUM_INTERVAL_H
#define LASS_GUARDIAN_OF_INCLUSION_NUM_INTERVAL_H

#include "num_common.h"
#include "num_traits.h"
#include "../util/call_traits.h"
#include "../util/pyobject_plus.h"
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

	inline interval() {};
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

	template<typename C,typename f> friend interval<C> applyFunction(const interval<C>& iV, f func );

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

#ifndef LASS_NO_TEMPLATE_PARTIAL_SPECIALIZATION
// num traits partial specialisation
template< class C >
struct NumTraits< interval<C> >
{
	typedef interval<C> selfType;
	typedef C           baseType;
	typedef C           intervalType;

	static const bool  isDistribution;
	static const bool  isIntegral;
	static const bool  isNative;
	static const bool  isSigned;
	static const bool  hasInfinity;
	static const int   memorySize;
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


template<class C> const int     NumTraits<interval<C> >::memorySize = sizeof(interval<C>);
template<class C> const bool    NumTraits<interval<C> >::isIntegral = false;
template<class C> const bool    NumTraits<interval<C> >::isNative = false;
template<class C> const bool    NumTraits<interval<C> >::isSigned = true;
template<class C> const bool    NumTraits<interval<C> >::hasInfinity = false;
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

#else
// The "poor mans broken compiler" workaround
#pragma LASS_NOTE( "NumTraits for Interval is only specialized for interval<float> and interval<double>" )

template<>
struct NumTraits< interval<float> >
{
	typedef interval<float> selfType;
	typedef float           baseType;
	typedef float           intervalType;

	static const bool  isDistribution;
	static const bool  isIntegral;
	static const bool  isNative;
	static const bool  isSigned;
	static const bool  hasInfinity;
	static const int   memorySize;
	static const std::string name();

	static const interval<float>    one;
	static const interval<float>    zero;

	static const interval<float> epsilon;
	static const interval<float> min;
	static const interval<float> max;
	static const interval<float> minStrictPositive;
	static const interval<float> pi;
	static const interval<float> e;
	static const interval<float> sqrt2;
	static const interval<float> sqrtPi;
};


template<>
struct NumTraits< interval<double> >
{
	typedef interval<double>    selfType;
	typedef double          baseType;
	typedef double          intervalType;

	static const bool  isDistribution;
	static const bool  isIntegral;
	static const bool  isNative;
	static const bool  isSigned;
	static const bool  hasInfinity;
	static const int   memorySize;
	static const std::string name();

	static const interval<double>   one;
	static const interval<double>   zero;

	static const interval<double> epsilon;
	static const interval<double> min;
	static const interval<double> max;
	static const interval<double> minStrictPositive;
	static const interval<double> pi;
	static const interval<double> e;
	static const interval<double> sqrt2;
	static const interval<double> sqrtPi;
};

#endif

#include "interval.inl"

}

namespace python
{
	template<class C> int pyGetSimpleObject( PyObject* iValue, lass::num::interval<C>& oV )
	{
		std::pair<C,C> temp;
		int r=pyGetSimpleObject(iValue, temp);
		if (r==0)
		{
			oV.set(temp.first , temp.second);
		}
		return r;
	}
	template<class C> PyObject* pyBuildSimpleObject( const lass::num::interval<C>& iV )
	{
		return pyBuildSimpleObject( std::pair<C,C>(iV.inf(),iV.sup()) );
	}

}

}


#endif
