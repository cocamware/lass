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



template<typename C>
interval<C>::interval(typename util::CallTraits<baseType>::TParam iValue)
{
	v[0] = v[1] = iValue;
}


template<typename C>
interval<C>::interval(typename util::CallTraits<baseType>::TParam iInf,typename util::CallTraits<baseType>::TParam iSup)
{
	v[0] = iInf;
	v[1] = iSup;
}

template<typename C>
interval<C>& interval<C>::operator=(const interval<C>& i)
{
	v[0] = i.v[0];
	v[1] = i.v[1];
	return *this;
}

template<typename C>
const interval<C>&  interval<C>::operator+() const
{
	return *this;
}

template<typename C>
void interval<C>::set(typename util::CallTraits<C>::TParam iInf,typename util::CallTraits<C>::TParam iSup)
{
	v[0] = iInf;
	v[1] = iSup;
}

template<typename C>
bool interval<C>::isEmpty() const
{
	return v[0]>v[1];
}


template<typename C>
bool interval<C>::isSingleton() const
{
	return v[0]==v[1];
}


template<typename C>
bool interval<C>::contains(typename util::CallTraits<baseType>::TParam ix)  const
{
	return (ix>=v[0]) && (ix<=v[1]);
}

template<typename C>
C&  interval<C>::operator[](int i)
{
	return v[i];
}

template<typename C>
C   interval<C>::operator[](int i) const
{
	return v[i];
}

template<typename C>
C   interval<C>::inf() const
{
	return v[0];
}

template<typename C>
C   interval<C>::sup() const
{
	return v[1];
}

template<typename C>
C&  interval<C>::inf()
{
	return v[0];
}

template<typename C>
C&  interval<C>::sup()
{
	return v[1];
}

template<typename C>
C   interval<C>::mid() const
{
	return static_cast<C>(0.5)*(v[0]+v[1]);
}

template<typename C>
C   interval<C>::diam()const
{
	return v[1]-v[0];
}

template<typename C>
C   interval<C>::ratio() const
{
	if (v[0]!=static_cast<C>(0.0))
		return (v[1]/v[0]);
	else
		return 0.0;
}

template<typename C>
bool interval<C>::operator> (const interval<C>& i) const
{
	return v[0]>i.v[1];
}

template<typename C>
bool interval<C>::operator< (const interval<C>& i) const
{
	return v[1]<i.v[0];
}

template<typename C>
bool interval<C>::operator==(const interval<C>& i) const
{
	return (v[0]==i.v[0]) && (v[1]==i.v[1]);
}

template<typename C>
bool interval<C>::operator!=(const interval<C>& i) const
{
	return !(*this==i);
}

template<typename C>
bool interval<C>::operator>=(const interval<C>& i) const
{
	return (v[0]>=i.v[0]) && (v[1]>=i.v[1]);
}

template<typename C>
bool interval<C>::operator<=(const interval<C>& i) const
{
	return (v[0]<=i.v[0]) && (v[1]<=i.v[1]);
}

template<typename C>
bool interval<C>::pe(const interval<C>& i) const
{
	return ((v[0]<i.v[0]) && (v[1]>i.v[0])) || ((v[0]<i.v[1])&&(v[1]>i.v[1]));
}

template<typename C>
bool interval<C>::pne(const interval<C>& i) const
{
	return *this!=i;
}

template<typename C>
bool interval<C>::pg(const interval<C>& i) const
{
	return v[1]>i.v[0];
}

template<typename C>
bool interval<C>::pge(const interval<C>& i) const
{
	return v[1]>=i.v[0];
}

template<typename C>
bool interval<C>::pl(const interval<C>& i) const
{
	return v[0]<i.v[1];
}

template<typename C>
bool interval<C>::ple(const interval<C>& i) const
{
	return v[0]>=i.v[1];
}

template<typename C>
std::ostream& operator<<( std::ostream& os, const interval<C>& iV )
{
	os << "<" << iV.inf() << "," << iV.sup() << ">";
	return os;
}

template<typename C> std::string str( const interval<C>& iV )
{
	return "<"+ str(iV.inf()) +  "," + str(iV.sup()) + ">";
}


namespace impl
{

template<typename C>    C  numMin4(const C& k1,const C& k2,const C& k3,const C& k4)
{
	C   tmpmin;

	tmpmin = k2;
	if (k1<k2)
		tmpmin = k1;
	if (k3<tmpmin)
		tmpmin = k3;
	if (k4<tmpmin)
		tmpmin = k4;
	return tmpmin;
}

template<typename C>    C  numMax4(const C& k1,const C& k2,const C& k3,const C& k4)
{
	C   tmpmax;

	tmpmax = k2;
	if (k1>k2)
		tmpmax = k1;
	if (k3>tmpmax)
		tmpmax = k3;
	if (k4>tmpmax)
		tmpmax = k4;
	return tmpmax;
}

template<typename C> __inline   void  numMinMax(const C& k1,const C& k2,C& amin,C& amax)
{
	if (k1<k2)
	{
		amin = k1;
		amax = k2;
	}
	else
	{
		amin = k2;
		amax = k1;
	}
}

template<typename C>    __inline    void  numMin(const C& k1,const C& k2,C& amin)
{
	if (k1<k2)
		amin = k1;
	else
		amin = k2;
}

template<typename C>    inline void  numMax(const C& k1,const C& k2,C& amax)
{
	if (k2<k1)
		amax = k1;
	else
		amax = k2;
}

}

template<typename C> interval<C>    operator+(const interval<C>& i1,const interval<C>& i2);
template<typename C> interval<C>    operator-(const interval<C>& i1,const interval<C>& i2);
template<typename C> interval<C>    operator*(const interval<C>& i1,const interval<C>& i2);
template<typename C> interval<C>    operator/(const interval<C>& i1,const interval<C>& i2);

template<typename C>
interval<C> interval<C>::operator-() const
{
	return interval<C>(-v[1],-v[0]);
}

template<typename C>
interval<C>&    interval<C>::operator+=(const interval<C>& i)
{
	v[0] += i.v[0];
	v[1] += i.v[1];
	return *this;
}

template<typename C>
interval<C>&    interval<C>::operator-=(const interval<C>& i)
{
	v[0] -= i.v[1];
	v[1] -= i.v[0];
	return *this;
}

template<typename C>
interval<C>&    interval<C>::operator*=(const interval<C>& i)
{
	C   m1,M1,m2,M2;
	impl::numMinMax(v[0]*i.v[0],v[0]*i.v[1],m1,M1);
	impl::numMinMax(v[1]*i.v[0],v[1]*i.v[1],m2,M2);
	impl::numMin(m1,m2,v[0]);
	impl::numMax(M1,M2,v[1]);
	return *this;
}


template<typename C>
interval<C>&    interval<C>::operator/=(const interval<C>& i)
{
	if ((i.v[0]<=0.0) && (i.v[1]>0.0))
		LASS_THROW("division by zero");
	C   m1,M1,m2,M2;
	m1 = NumTraits<C>::one / i.v[0];
	m2 = NumTraits<C>::one / i.v[1];

	impl::numMinMax(v[0]*m1,v[0]*m1,m1,M1);
	impl::numMinMax(v[1]*m2,v[1]*m2,m2,M2);
	impl::numMin(m1,m2,v[0]);
	impl::numMax(M1,M2,v[1]);
	return *this;
}


template<typename C>
interval<C>&    interval<C>::operator+=(typename util::CallTraits<baseType>::TParam s)
{
	v[0] += s;
	v[1] += s;
	return *this;
}

template<typename C>
interval<C>&    interval<C>::operator-=(typename util::CallTraits<baseType>::TParam s)
{
	v[0] -= s;
	v[1] -= s;
	return *this;
}

template<typename C>
interval<C>&    interval<C>::operator*=(typename util::CallTraits<baseType>::TParam s)
{
	v[0] *= s;
	v[1] *= s;

	if (s<0.0)
		std::swap(v[0],v[1]);

	return *this;
}

template<typename C>
interval<C>&    interval<C>::operator/=(typename util::CallTraits<baseType>::TParam s)
{
	C   t = static_cast<C>(1.0)/s;
	v[0] *= t;
	v[1] *= t;

	if (s<0.0)
		std::swap(v[0],v[1]);

	return *this;
}

template<typename C> inline
interval<C> operator+(const interval<C>& i1,const interval<C>& i2)
{
	interval<C> t(i1);
	t+=i2;
	return t;
}

template<typename C> inline
interval<C> operator-(const interval<C>& i1,const interval<C>& i2)
{
	interval<C> t(i1);
	t-=i2;
	return t;
}

template<typename C> inline
interval<C> operator*(const interval<C>& i1,const interval<C>& i2)
{
	interval<C> t(i1);
	t*=i2;
	return t;
}

template<typename C> inline
interval<C> operator/(const interval<C>& i1,const interval<C>& i2)
{
	interval<C> t(i1);
	t/=i2;
	return t;
}

/** fuzzy equal */
template<typename C>
C   interval<C>::fe(const interval<C>& i) const
{
	if (i.isSingleton())
	{
		if (isSingleton())
			if (v[0]==i.v[0])
				return static_cast<C>(1.0);
		return static_cast<C>(0.0);
	}
	if (i.v[0]<v[1])
	{
		return (v[1]-i.v[0])/(i.v[1]-v[0]);
	}
	return (i.v[1]-v[0])/(v[1]-i.v[0]);
}

/** fuzzy not equal */
template<typename C>
C   interval<C>::fne(const interval<C>& i) const
{
	return static_cast<C>(1.0)-fe(i);
}

/** fuzzy greater than*/
template<typename C>
C    interval<C>::fg(const interval<C>& i) const
{
	if (i.v[0]<v[1])
	{
		return (i.v[0]-v[0])/(i.v[1]-v[0]);
	}
	return (v[0]-i.v[0])/(v[1]-i.v[0]);
}

/** fuzzy greater or equal than*/
template<typename C>
C   interval<C>::fge(const interval<C>& i) const
{
	return std::max(fg(i),fe(i));
}

/** fuzzy less than */
template<typename C>
C   interval<C>::fl(const interval<C>& i) const
{
	return static_cast<C>(1.0)-fg(i);
}

/** fuzzy less or equal than*/
template<typename C>
C   interval<C>::fle(const interval<C>& i) const
{
	return std::max(fl(i),fe(i));
}

template<typename C>
void inpsqr(interval<C>& i)
{
	C   m1,M1;
	m1 = i.v[0]*i.v[0];
	M1 = i.v[1]*i.v[1];

	impl::numMinMax(m1,M1,i.v[0],i.v[1]);
	if ((i.v[0] < 0.0) && (i.v[1]>=0.0))
		i.v[0] = 0.0;
}

template<typename C>
void inpsqrt(interval<C>& i)
{
	i.v[0] = lass::num::sqrt(i.v[0]);
	i.v[1] = lass::num::sqrt(i.v[1]);
}

template<typename C>
void inpexp(interval<C>& i)
{
	i.v[0] = lass::num::exp(i.v[0]);
	i.v[1] = lass::num::exp(i.v[1]);
}

template<typename C>
void inplog(interval<C>& i)
{
	i[0] = lass::num::log(i[0]);
	i[1] = lass::num::log(i[1]);
}

template<typename C>
void inpnorm(interval<C>& i)
{
	i[0] = lass::num::norm( i[0] );
	i[1] = lass::num::norm( i[1] );
}

template<typename C>
void inpinv(interval<C>& i)
{
	i[0] = lass::num::inv( i[1] );
	i[1] = lass::num::inv( i[0] );
}


template<typename C>
interval<C> sqr(const interval<C>& i)
{
	C   m1,M1,m2,M2;
	m1 = i[0]*i[0];
	M1 = i[1]*i[1];
	impl::numMinMax(m1,M1,m2,M2);

	if (i.contains(0.0))
		m2 = 0.0;

	return num::interval<C>(m2,M2);
}

template<typename C>
interval<C> sqrt(const interval<C>& i)
{
	return interval<C>(lass::num::sqrt(i.inf()),lass::num::sqrt(i.sup()));
}

template<typename C>
interval<C> exp(const interval<C>& i)
{
	return interval<C>(lass::num::exp(i.inf()),lass::num::exp(i.sup()));
}

template<typename C>
interval<C> log(const interval<C>& i)
{
	return interval<C>(lass::num::log(i.inf()),lass::num::log(i.sup()));
}

template<typename C>
interval<C> set_union(const interval<C>& i1, const interval<C>& i2)
{
	return interval<C>( std::min(i1.inf(), i2.inf()), std::max( i1.sup(), i2.sup() ) );
}

template<typename C>
interval<C> set_intersect(const interval<C>& i1, const interval<C>& i2)
{
	return interval<C>( std::max(i1.inf(), i2.inf()), std::min( i1.sup(), i2.sup() ) );
}


template<typename C,typename f>
interval<C> applyFunction(const interval<C>& iV, f func )
{
	C t1,t2,r1,r2;
	t1 = func(iV.inf());
	t2 = func(iV.sup());
	impl::numMinMax(t1,t2,r1,r2);
	return interval<C>(r1,r2);
}
