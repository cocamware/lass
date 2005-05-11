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

namespace impl
{
	// for random number generators
	#define IA 16807
	#define	IM 2147483647
	#define	AM (1.0/IM)
	#define IQ 127773
	#define	IR 2836
	#define MASK 123459876


	/** ran0.  Minimal random generator.  Taken from Numerical Recipies in C. */
	inline double	ran0(long* idnum)
	{
		long k;
		float ans;
		*idnum ^= MASK;
		k = (*idnum)/IQ;
		*idnum = IA*(*idnum-k*IQ)-IR*k;
		if (*idnum<0) *idnum += IM;
		ans = static_cast<float>(AM)*(*idnum);
		*idnum ^= MASK;
		return ans;
	}

	/** ran0.  Minimal random generator.  Taken from Numerical Recipies in C. */
	inline double	ran0(void)
	{
		static long	globalnum_idnum = 53;
		long k;
		float ans;
		globalnum_idnum ^= MASK;
		k = (globalnum_idnum)/IQ;
		globalnum_idnum = IA*(globalnum_idnum-k*IQ)-IR*k;
		if (globalnum_idnum<0) globalnum_idnum += IM;
		ans = static_cast<float>(AM)*(globalnum_idnum);
		globalnum_idnum ^= MASK;
		return ans;
	}


	/** erfcc.  Complementary Error Function Approximitation for a real number.
	*	Complementary error function as by Chebyshev; 
	*	routine is described in Numerical Recipes in C.
	*/
	template<class NTC> NTC	erfcc(const NTC& x)
	{
		SW_STATIC NTC t,z,ans;
		z=fabs(x);
		t=1.0/(1.0+0.5*z);
		ans=t*exp(-z*z-1.26551223+t*(1.00002368+t*(0.37409196+t*(0.09678418+
		t*(-0.18628806+t*(0.27886807+t*(-1.13520398+t*(1.48851587+
		t*(-0.82215223+t*0.17087277)))))))));

		return x >= 0.0 ? ans : 2.0-ans;
	};

	/** uniformdev.  Returns a uniform distributed deviate in [0,1).
	*/
	template<class NTC> NTC	uniformdev()
	{
		return NTC(ran0());
	}

	/** gausdevunit.  Returns a normally distributed deviate with zero mean and unit variance. 
	*	using the ran0() as source of the uniform deviates. The argument should be in
	*	[0,1].  Taken from Numerical Recipies in C.
	*/
	template<class NTC> NTC	gausdevunit()
	{
		NTC fac,rsq,v1,v2;
		do {
			v1 = static_cast<NTC>(2.0)*uniformdev<NTC>()-static_cast<NTC>(1.0);
			v2 = static_cast<NTC>(2.0)*uniformdev<NTC>()-static_cast<NTC>(1.0);
			rsq= v1*v1+v2*v2;
		} while (rsq >= static_cast<NTC>(1.0) || rsq == static_cast<NTC>(0.0));
		fac = static_cast<NTC>(sqrt(-static_cast<NTC>(2.0)*log(rsq)/rsq));
		return v2*fac;
	}

	/** gausdev.  Returns a normally distributed deviate with iMean mean and iStd standard deviation. 
	*	using the rand() as source of the uniform deviates. The argument should be in
	*	[0,1].  Taken from Numerical Recipies in C.
	*/
	template<class NTC> NTC	gausdev(const NTC& iMean, const NTC& iStdDev)
	{
		return iMean+iStdDev*gausdevunit<NTC>();
	}

	/** triangulardev. Returns a triangle distribute deviate based on ran0
	*/
	template<class NTC> NTC	triangulardev(const NTC& iLeft, const NTC& iMid, const NTC& iRight)
	{
		double temp = uniformdev<NTC>();

		if (temp<=0.5)
		{
			// upper side of triangle, now inverse apply cumulative
			return static_cast<NTC>(iLeft + (iMid-iLeft)*sqrt(2.0*temp));
		}
		return static_cast<NTC>(iMid + (iRight-iMid)*sqrt(2.0*(temp-0.5)));
	}

	/** trapezoidaldev. Returns a trapezoidal distribute deviate based on ran0
	*/
	template<class NTC> NTC	trapezoidaldev(const NTC& ia, const NTC& ib, const NTC& ic, const NTC& id)
	{
		if (ib == ic)
			return triangulardev(ia,ib,id);

		double temp = uniformdev<NTC>();

		double firstp = 0.5*(ib-ia);
		double secondp= ic-ib;
		double thirdp = 0.5*(id-ic);

		double norm = 1.0/(firstp+secondp+thirdp);

		firstp *= norm;
		secondp *= norm;
		thirdp *= norm;
	
		if (temp<firstp)
		{
			// upper side of triangle, now inverse apply cumulative
			return static_cast<NTC>(ia + (ib-ia)*sqrt(temp/firstp));
		}
		if (temp>(firstp+secondp))
			return static_cast<NTC>(ic + (id-ic)*sqrt(temp/thirdp));
		return static_cast<NTC>(ib + temp / secondp);
	}

}

inline void createGaussian( float& oValue, float iMean, float iStdDev)
{
	oValue = impl::gausdev(iMean,iStdDev);
}

inline void createTriangular( float& oValue,    float iLeft,
										float iMid,
										float iRight)
{
	oValue = impl::triangulardev(iLeft,iMid,iRight);
}

inline void createTrapezoidal( float& oValue,float iA,
									  float iB,
									  float iC,
									  float iD)
{
	oValue = impl::trapezoidaldev(iA,iB,iC,iD);
}

inline interval<NumTraits<float>::intervalType> support( const float& iValue )
{
	return interval<NumTraits<float>::intervalType>( iValue );
}

inline interval<NumTraits<float>::intervalType> support( const float& iValue, float iAlpha )
{
	return interval<NumTraits<float>::intervalType>( iValue );
}

inline NumTraits<float>::baseType
memberShip( const float& iValue, float iX )
{
	if (iValue==iX)
		return NumTraits<NumTraits<float>::baseType>::one;
	else
		return NumTraits<NumTraits<float>::baseType>::zero;
}

inline NumTraits<float>::baseType   infinum( const float& iValue )
{
	return iValue;
}

inline NumTraits<float>::baseType   supinum( const float& iValue )
{
	return iValue;
}

inline NumTraits<float>::baseType   footprint( const float& iValue )
{
	return NumTraits<NumTraits<float>::baseType>::zero;
}

inline NumTraits<float>::baseType   entropy( const float& iValue )
{
	return NumTraits<NumTraits<float>::baseType>::zero;
}

inline NumTraits<float>::baseType   differentialEntropy( const float& iValue )
{
	return NumTraits<NumTraits<float>::baseType>::zero;
}

inline NumTraits<float>::baseType   nonSpecificity( const float& iValue )
{
	return NumTraits<NumTraits<float>::baseType>::zero;
}

inline NumTraits<float>::baseType   discord( const float& iValue )
{
	return NumTraits<NumTraits<float>::baseType>::zero;
}

inline NumTraits<float>::baseType   bandwidth( const float& iValue )
{
	return NumTraits<NumTraits<float>::baseType>::zero;
}

inline NumTraits<float>::baseType   bandwithRatio( const float& iValue )
{
	return NumTraits<NumTraits<float>::baseType>::one;
}

inline NumTraits<float>::baseType   maxMembership( const float& iValue )
{
	return iValue;
}

inline NumTraits<float>::baseType   centroid( const float& iValue )
{
	return iValue;
}
