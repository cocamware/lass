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



#ifndef LASS_GUARDIAN_OF_INCLUSION_NUM_REFL_DIFFR_H
#define LASS_GUARDIAN_OF_INCLUSION_NUM_REFL_DIFFR_H

#include <complex>

namespace lass
{
namespace num
{
	#define PI 3.1415926535897932384626433832795

	/** Computes the reflection coefficient for planar waves */
	std::complex<double> planeReflection(const double& psi,const double& k,const std::complex<double>& Z,const double& R);

	/** Computes the reflection coefficient for spherical waves */
	std::complex<double> sphericalReflection(const double& psi,const double& k,const std::complex<double>& Z,const double& R);

	/** diffractionNord2000.  Computes the diffraction coefficient according to the
	*   Nord2000 standard.  This should be the Hadden-Pierce solution */
	std::complex<double> diffractionNord2000(
			const double& thetaR,const double& thetaS, const double& beta,
			const std::complex<double>& ZR,const std::complex<double>& ZS,
			const double& RS, const double& RR,const double& freq);

	namespace impl
	{
		double cot(double iArg);
		double ap(double beta,double n);
		double am(double beta,double n);
		std::complex<double> F(double X);

		float cotf(float iArg);
		float apf(float beta,float n);
		float amf(float beta,float n);
		std::complex<float> Ff(float X);
	}


	struct UTD_SphericalWavePolicy;
	struct UTD_CilindricalWavePolicy;
	struct UTD_PlaneWavePolicy;

	template<class NTC> std::complex<NTC>	deJongDiffraction
		(	const NTC& thetaR,const NTC& thetaS, const NTC& beta,
			const std::complex<NTC>& ZR,const std::complex<NTC>& ZS,
			const NTC& RS, const NTC& RR,const NTC& freq );
	template<class WavePolicy> std::complex<double> D_UTD
		(	double wedgeangle, double k, double rS, double rR, 
			double thetaI, double alphaI, double alphaD);
	template<class WavePolicy> std::complex<float> D_UTD
		(	float wedgeangle, float k, float rS, float rR, 
			float thetaI, float alphaI, float alphaD);

	/** gives the diffraction coefficient according to deJong.  The coefficient is the insertion loss due to
	*   diffraction, compared to the direct path, attenuation is in amplitude */
	template<class NTC> std::complex<NTC>	deJongDiffraction( const NTC& thetaR,const NTC& thetaS, const NTC& beta,
			const std::complex<NTC>& ZR,const std::complex<NTC>& ZS,
			const NTC& RS, const NTC& RR,const NTC& freq)
	{
		NTC		value;
		NTC		directPath = sqrt(RS*RS+RR*RR-2*RS*RR*cos(thetaS-thetaR));
		NTC		delta = RS+RR-directPath;
		NTC		N = 2.0*delta*freq/340.0;

		NTC K = 25.0;
		if (N<-0.02)
			K = 5.65 + 66.0*N+244.0*N*N+287.0*N*N*N;
		if ((N>=-0.02) && (N<1.0))
			K = 5.02+21.1*N+19.9*N*N+6.69*N*N*N;
		if ((N>=1.0) && (N<18.0))
			K = 13.0+10.0*log(N)/log(10.0);

		value = pow(10.0,-K/20.0);
		return value;
	}

	/** gives the diffraction coefficient according to Deygout.  The coefficient is the insertion loss due to
	*   diffraction, compared to the direct path, attenuation is in amplitude */
	template<class NTC> std::complex<NTC>	deygoutDiffraction( const NTC& thetaR,const NTC& thetaS, const NTC& beta,
			const std::complex<NTC>& ZR,const std::complex<NTC>& ZS,
			const NTC& RS, const NTC& RR,const NTC& freq)
	{
		NTC		value;
		NTC		directPath = sqrt(RS*RS+RR*RR-2.0*RS*RR*cos(thetaS-thetaR));
		NTC		delta = RS+RR-directPath;
		NTC		N = 2.0*delta*freq/340.0;

		NTC K = 25.0;
		if (N<-0.25)
			K = 0.0;
		if ((N>=-0.25) && (N<0.25))
			K = 6.0+12.0*sqrt(fabs(N))*sign(N);
		if ((N>0.25) && (N<1.0))
			K = 8.0+8.0*sqrt(N);
		if (N>1.0)
			K = 16.0+10.0*lass::num::log(N)/lass::num::log(10.0);

		value = pow(10.0,-K/20.0);
		return value;
	}

	/** gives the diffraction coefficient according to Deygout.  The coefficient is the insertion loss due to
	*   diffraction, compared to the direct path, attenuation is in amplitude, continuous over shadow zone */
	template<class NTC> std::complex<NTC>	deygoutContinuousDiffraction( const NTC& thetaR,const NTC& thetaS, const NTC& beta,
			const std::complex<NTC>& ZR,const std::complex<NTC>& ZS,
			const NTC& RS, const NTC& RR,const NTC& freq)
	{
		NTC		value;
		NTC		directPath = sqrt(RS*RS+RR*RR-2.0*RS*RR*cos(thetaS-thetaR));
		NTC		delta = RS+RR-directPath;
		NTC		N = 2.0*delta*freq/340.0;

		NTC K = 25.0;
		if (N<-0.25)
			K = 0.0;
		if ((N>=-0.25) && (N<0.25))
		{
			NTC NA = N*2.0-0.25;
			K = 6.0+12.0*sqrt(fabs(NA))*sign(NA);
		}
		if ((N>0.25) && (N<1.0))
			K = 8.0+8.0*sqrt(N);
		if (N>1.0)
			K = 16.0+10.0*lass::num::log(N)/lass::num::log(10.0);

		value = pow(10.0,-K/20.0);
		return value;
	}

	/** gives the diffraction coefficient according to Deygout.  The coefficient is the insertion loss due to
	*   diffraction, compared to the direct path, attenuation is in amplitude, continuous over shadow zone */
	template<class NTC> NTC	deygoutContinuousDiffraction2( const NTC& pathDiff,	const NTC& freq)
	{
		NTC		value;
		NTC		delta = pathDiff;
		NTC		N = 2.0*delta*freq/340.0;

		NTC K = 25.0;
		if (N<-0.25)
			K = 0.0;
		if ((N>=-0.25) && (N<0.25))
		{
			NTC NA = N*2.0-0.25;
			K = 6.0+12.0*sqrt(fabs(NA))*sign(NA);
		}
		if ((N>0.25) && (N<1.0))
			K = 8.0+8.0*sqrt(N);
		if (N>1.0)
			K = 16.0+10.0*lass::num::log(N)/lass::num::log(10.0);

		value = pow(10.0,-K/20.0);
		return value;
	}


	struct UTD_SphericalWavePolicy
	{
		static double L(double rS, double rR, double thetaI) { return ((rS*rR)/(rS+rR))*pow(sin(thetaI),2.0); }
		static float L(float rS, float rR, float thetaI) { return ((rS*rR)/(rS+rR))*powf(sinf(thetaI),2.0); }
	};
	struct UTD_CilindricalWavePolicy
	{
		static double L(double rS, double rR, double thetaI) { return ((rS*rR)/(rS+rR)); }
		static float L(float rS, float rR, float thetaI) { return ((rS*rR)/(rS+rR)); }
	};
	struct UTD_PlaneWavePolicy
	{
		static double L(double rS, double rR, double thetaI) { return rR*pow(sin(thetaI),2.0); }
		static float L(float rS, float rR, float thetaI) { return rR*powf(sinf(thetaI),2.0); }
	};

	template<typename WP>
	std::complex<double> D_UTD(double wedgeangle, double k, double rS, double rR, double thetaI, double alphaI, double alphaD)
	{
		std::complex<double> D;
		double beta = wedgeangle;
		double n = beta/PI;
		D = -std::exp(std::complex<double>(0.0,-PI*0.25));
		D/= 2.0*n*sqrt(2.0*k*PI)*sin(thetaI);

		double L = WP::L(rS,rR,thetaI);
		std::complex<double> T;

		T = impl::cot((PI  + (alphaD-alphaI))/(2.0*n))*impl::F(k*L*impl::ap(alphaD-alphaI,n));
		T+= impl::cot((PI  - (alphaD-alphaI))/(2.0*n))*impl::F(k*L*impl::am(alphaD-alphaI,n));
		T+= impl::cot((PI  + (alphaD+alphaI))/(2.0*n))*impl::F(k*L*impl::ap(alphaD+alphaI,n));
		T+= impl::cot((PI  - (alphaD+alphaI))/(2.0*n))*impl::F(k*L*impl::am(alphaD+alphaI,n));

		return D*T;
	}

	template<typename WP>
	std::complex<float> D_UTD(float wedgeangle, float k, float rS, float rR, float thetaI, float alphaI, float alphaD)
	{
		std::complex<float> D;
		float beta = wedgeangle;
		float n = beta/PI;
		D = -std::exp(std::complex<float>(0.0,-PI*0.25));
		D/= 2.0*n*sqrtf(2.0*k*PI)*sinf(thetaI);

		float L = WP::L(rS,rR,thetaI);
		std::complex<float> T;

		T = impl::cotf((PI  + (alphaD-alphaI))/(2.0f*n))*impl::Ff(k*L*impl::apf(alphaD-alphaI,n));
		T+= impl::cotf((PI  - (alphaD-alphaI))/(2.0f*n))*impl::Ff(k*L*impl::amf(alphaD-alphaI,n));
		T+= impl::cotf((PI  + (alphaD+alphaI))/(2.0f*n))*impl::Ff(k*L*impl::apf(alphaD+alphaI,n));
		T+= impl::cotf((PI  - (alphaD+alphaI))/(2.0f*n))*impl::Ff(k*L*impl::amf(alphaD+alphaI,n));

		return D*T;
	}

}
}

#endif
