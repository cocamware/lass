/**	@file
 *	@author Bram de Greve (bramz@users.sourceforge.net)
 *	@author Tom De Muer (tomdemuer@users.sourceforge.net)
 *
 *	Distributed under the terms of the GPL (GNU Public License)
 *
 * 	The LASS License:
 *
 *	Copyright 2004 Bram de Greve and Tom De Muer
 *
 *	LASS is free software; you can redistribute it and/or modify
 *	it under the terms of the GNU General Public License as published by
 *	the Free Software Foundation; either version 2 of the License, or
 *	(at your option) any later version.
 *
 *	This program is distributed in the hope that it will be useful,
 *	but WITHOUT ANY WARRANTY; without even the implied warranty of
 *	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *	GNU General Public License for more details.
 *
 *	You should have received a copy of the GNU General Public License
 *	along with this program; if not, write to the Free Software
 *	Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */



#include "num_common.h"
#include "refl_diffr.h"
#include "num_traits.h"
#include "..\util\pyobject_plus.h"
#include "..\util\pyobject_util.h"

namespace lass
{
namespace num
{


/** impl. private implementation functions */
namespace impl
{

	template<class NTC>	NTC heaviside(const NTC& iValue)
	{
		if (iValue>0.0)
			return iValue;
		return NTC(0.0);
	}

	/**
	*	w-function as in Nord2000 DRAFT page 114
	*/
	template<class NTC> std::complex<NTC>	w(const std::complex<NTC> ro)
	{
		NTC x,y;
		std::complex<NTC>	tempc,ro2;
		std::complex<NTC> J(0.0,1.0);

		x = ro.real();
		y = ro.imag();

		ro2 = ro*ro;

		if ((x>static_cast<NTC>(6.0)) || (y>static_cast<NTC>(6.0)))
		{
			tempc = static_cast<NTC>(0.5124242) / (ro2-static_cast<NTC>(0.2752551));
			tempc+= static_cast<NTC>(0.05176536) / (ro2-static_cast<NTC>(2.724745));
			tempc*= ro*J;

			return tempc;
		}
		else
		{
			if ((x>static_cast<NTC>(3.9)) || (y>static_cast<NTC>(3.0)))
			{
				tempc = static_cast<NTC>(0.4613135)	/ (ro2-static_cast<NTC>(0.1901635));
				tempc+= static_cast<NTC>(0.09999216)/ (ro2-static_cast<NTC>(1.7844927));
				tempc+= static_cast<NTC>(0.002883894)/ (ro2-static_cast<NTC>(5.5253437));
				tempc*= ro*J;
				return tempc;
			}	
			else
			{
				NTC A1,B1,C1,D1,P2,Q2,H,K;
				const NTC h = 0.8;
				NTC temp1,temp2,temp3;	

				temp1 = 2.0*x*y;
				temp2 = x*x+y*y;
				temp3 = y*y-x*x;

				A1 = num::cos(temp1);
				B1 = num::sin(temp1);
				C1 = num::exp(-y*(NumTraits<double>::pi*0.5)/h)-cos(x*(NumTraits<double>::pi*0.5)/h);
				D1 = num::sin(x*(NumTraits<double>::pi*0.5)/h);
				
				NTC	t;

				t = 2.0*exp(-(x*x+(y*(NumTraits<double>::pi*0.5)/h)-y*y))/(C1*C1+D1*D1);
				P2 = t* (A1*C1 - B1*D1);
				Q2 = t* (A1*D1 + B1*C1);

				t = 0.0;

				long i;
				NTC n,tn;
				
				n = 0.0; 
				
				for (i=0;i<5;++i)
				{
					n+=1.0;
					tn = n*n*h*h;
					
					NTC t2,n2;
					n2 = (temp3+tn);
					n2*= n2;
					n2+= temp1*temp1;
					t2 = num::exp(-tn)*(temp2+tn)/n2;
					t += t2;
				}

				H = (h*y)/(NumTraits<double>::pi*(temp2));
				H+= y*h*t/(NumTraits<double>::pi*0.5);

				K = (h*x)/(NumTraits<double>::pi*(temp2));
				K+= x*h*t/(NumTraits<double>::pi*0.5);

				if (y>((NumTraits<double>::pi)/h))
				{
					tempc = std::complex<NTC>(H,K);
					return tempc;
				}
				if (y==(NumTraits<double>::pi/h))
				{
					tempc = std::complex<NTC>(H+(P2/2.0),K-(Q2/2.0));
					return tempc;
				}
				if (y<(NumTraits<double>::pi/h))
				{
					tempc = std::complex<NTC>(H+P2,K-Q2);
					return tempc;
				}
			}
		}
		LASS_THROW("Unforeseen condition in calculation of w");
	};

	template<class NTC> void sphreflQ2(const NTC* psi,const NTC& k,const std::complex<NTC>* Z,const NTC& R,std::complex<NTC>* outSphRefl)
	{
		std::complex<NTC>	ro[2],ero[2],Rp[2];
		std::complex<NTC>	invZ[2];

		const NTC* lpsi = psi;
		NTC  lk = k;
		const std::complex<NTC>* lZ = Z;
		NTC	lR = R;

		
		try
		{
			std::complex<NTC> J( 0.0, 1.0 );
			invZ[0] = static_cast<NTC>(1.0)/Z[0];
			invZ[1] = static_cast<NTC>(1.0)/Z[1];

			ro[0] = std::complex<NTC>(0.5,0.5)*static_cast<NTC>(num::sqrt(k*R))*(static_cast<NTC>(num::cos((NumTraits<double>::pi*0.5)-psi[0]))+invZ[0]);
			ro[1] = std::complex<NTC>(0.5,0.5)*static_cast<NTC>(num::sqrt(k*R))*(static_cast<NTC>(num::cos((NumTraits<double>::pi*0.5)-psi[1]))+invZ[1]);
			ero[0]= static_cast<NTC>(1.0)+static_cast<NTC>(sqrt(NumTraits<double>::pi))*J*ro[0]*w(ro[0]);
			ero[1]= static_cast<NTC>(1.0)+static_cast<NTC>(sqrt(NumTraits<double>::pi))*J*ro[1]*w(ro[1]);

			Rp[0] = (static_cast<NTC>(num::cos((NumTraits<double>::pi*0.5)-psi[0]))-(invZ[0]))/(static_cast<NTC>(num::cos((NumTraits<double>::pi*0.5)-psi[0]))+(invZ[0]));
			Rp[1] = (static_cast<NTC>(num::cos((NumTraits<double>::pi*0.5)-psi[1]))-(invZ[1]))/(static_cast<NTC>(num::cos((NumTraits<double>::pi*0.5)-psi[1]))+(invZ[1]));

			outSphRefl[0] = Rp[0]+(static_cast<NTC>(1.0)-Rp[0])*ero[0];
			outSphRefl[1] = Rp[1]+(static_cast<NTC>(1.0)-Rp[1])*ero[1];
		}
		catch (std::exception& e)
		{
			std::cout << "\nSomething went wrong in calculation of spherical reflection coefficient\n";
			std::cout << "psi = " << lpsi[0] << "," << lpsi[1] << "\n";
			std::cout << "k = " << lk << "\n";
			std::cout << "Z = " << lZ[0] << "," << lZ[1] << "\n";
			std::cout << "R = " << lR << "\n";
			throw e;
		}
	}


	template<class NTC> std::complex<NTC> diffr2000AD(const NTC& B)
	{
		static NTC	fc[13] = 
		{	0.49997531354311, 0.00185249867385,-0.80731059547652,
			1.15348730691625,-0.89550049255859,0.44933436012454,
			-0.15130803310630,0.03357197760359,-0.00447236493671,
			0.00023357512010,0.00002262763737,-0.00000418231569,
			0.00000019048125};
		static NTC	gc[13] =  
		{	0.50002414586702,-1.00151717179967,0.80070190014386,
			-0.06004025873978,-0.50298686904881,0.55984929401694,
			-0.33675804584105,0.13198388204736,-0.03513592318103,
			0.00631958394266,-0.00073624261723,0.00005018358067,
			-0.00000151974284};


		NTC	tempf,tempg;
		NTC	tempv;
		NTC	BA;
		
		BA = fabs(B);
		
		if (BA>5.0)
		{
			tempf = 1.0/(NumTraits<double>::pi*BA);
			tempg = (1.0/NumTraits<double>::pi);
			tempg*= tempg;
			tempg/= BA*BA*BA;
		}
		else
		{
			long i;
			
			tempv = 1.0;
			tempf = fc[0];
			tempg = gc[0];
			
			for (i=1;i<13;++i)
			{
				tempv*= BA;

				tempf += tempv * fc[i];
				tempg += tempv * gc[i];
			}	
		}
		
		if (num::sign(B)==0.0)
		{
			return std::complex<NTC>(tempf,-tempg);
		}
		
		return std::complex<NTC>(num::sign(B)*tempf,-num::sign(B)*tempg);
	};
	
	template<class NTC> NTC inline diffr2000C(const NTC& cA,const NTC& nu,const NTC& RS,const NTC& RR,const NTC& l)
	{
		NTC temp;
		temp = (cA/nu);
		temp*= temp;
		temp*= (2.0*RS*RR/(l*l)) + static_cast<NTC>(0.5);
		temp+= static_cast<NTC>(1.0);
		temp = num::sqrt(temp);

		return temp;
	};

	template<class NTC> NTC	diffr2000B(const NTC& CC,const NTC& cA,const NTC& nu,const NTC& RS,const NTC& RR,const NTC& l,const NTC& k)
	{
		NTC temp,tempm;

		temp = num::sqrt(static_cast<NTC>(4.0)*k*RS*RR/(NumTraits<double>::pi*l));
		temp*= cA;
		tempm = 1.f/(nu*CC);
		temp*= tempm;
		return temp;
	};
	
	template<class NTC> std::complex<NTC>	diffr2000Ev(const NTC& A,const std::complex<NTC>& AD,const NTC& C)
	{
		NTC temp,temp2;
		std::complex<NTC>	tempc;

		temp = static_cast<NTC>(NumTraits<double>::pi/NumTraits<double>::sqrt2);
		temp*= num::sin(A)/A;
		temp/= C;
		
		temp2 = static_cast<NTC>(NumTraits<double>::sqrt2*0.5);
		tempc = temp * AD * std::complex<NTC>(temp2 ,temp2);

		return tempc;
	};

}



#pragma warning(disable:4100)	// suppress "unreferenced formal parameter"
template<class NTC> std::complex<NTC> reflQ(const NTC& psi,const NTC& k,const std::complex<NTC>& Z,const NTC& R)
{
	std::complex<NTC>	Rp;
	std::complex<NTC>	invZ;
	NTC theta;

	invZ = static_cast<NTC>(1.0)/Z;

	theta = (PI2)-psi;
	Rp = (static_cast<NTC>(num::cos(theta))-(invZ))/(static_cast<NTC>(num::cos(theta))+(invZ));

	return Rp;
}
#pragma warning(default:4100)	// suppress "unreferenced formal parameter"

template<class NTC> std::complex<NTC> sphreflQ(const NTC& psi,const NTC& k,const std::complex<NTC>& Z,const NTC& R)
{
	std::complex<NTC>	ro,ero,Rp,tempc;
	std::complex<NTC>	invZ;
	NTC theta;
	std::complex<NTC> J(0.0,1.0);

	invZ = static_cast<NTC>(1.0)/Z;

	theta = (PI2)-psi;
	ro = std::complex<NTC>(0.5,0.5)*static_cast<NTC>(num::sqrt(k*R))*(static_cast<NTC>(num::cos(theta))+invZ);
	ero= static_cast<NTC>(1.0)+static_cast<NTC>(SQRTPI)*J*ro*w(ro);

	Rp = (static_cast<NTC>(num::cos(theta))-(invZ))/(static_cast<NTC>(num::cos(theta))+(invZ));

	tempc = Rp+(static_cast<NTC>(1.0)-Rp)*ero;

	return tempc;
}

std::complex<double> diffractionNord2000(
		const double& thetaR,const double& thetaS, const double& beta,
		const std::complex<double>& ZR,const std::complex<double>& ZS,
		const double& RS, const double& RR,const double& freq)
{
	long i;
	
	double	theta[4],A[4],B[4],C[4],l[4],nu,k;
	std::complex<double> Q[4];
	std::complex<double> QS,QR;
	std::complex<double> Ev,AD[4];
	std::complex<double> temp,tempex;
	std::complex<double> ZZ[2],QQ[2];

	static double c = 340.0;

	k = 2.0*NumTraits<double>::pi*freq/c;

	temp = std::complex<double>(0.0,0.0);
	tempex = std::complex<double>(0.0,0.0);
	nu = NumTraits<double>::pi/beta;

	double temppsi[2];

	temppsi[0] = beta-thetaS;
	if (temppsi[0]>(NumTraits<double>::pi*0.5))
		temppsi[0] = (NumTraits<double>::pi*0.5);
	temppsi[1] = thetaR;
	if (temppsi[1]>(NumTraits<double>::pi*0.5))
		temppsi[1] = (NumTraits<double>::pi*0.5);
	ZZ[0] = ZS;
	ZZ[1] = ZR;
	
	impl::sphreflQ2<double>(temppsi,k,ZZ,(RS+RR),QQ);

	QS = QQ[0];
	QR = QQ[1];

	theta[0] = thetaS - thetaR;							Q[0] = 1.0;
	theta[1] = thetaS + thetaR;							Q[1] = QR;
	theta[2] = 2.0*beta - (thetaR + thetaS);			Q[2] = QS;
	theta[3] = 2.0*beta - (thetaS - thetaR);			Q[3] = QR*QS;
	for (i=0;i<4;++i)
		l[i] = RS+RR;
	/**
		The structuring of the instructions below may seem odd but is intended to keep the
		instructions in the instructioncache at level 1.
	*/

	for (i=0;i<4;++i)
		A[i] = ((nu/2.0)*(beta-NumTraits<double>::pi+theta[i])) + NumTraits<double>::pi*impl::heaviside<double>(NumTraits<double>::pi-theta[i]);
	for (i=0;i<4;++i)
		C[i] = impl::diffr2000C<double>(cos(A[i]),nu,RS,RR,l[i]);
	for (i=0;i<4;++i)
		B[i] = impl::diffr2000B<double>(C[i],cos(A[i]),nu,RS,RR,l[i],k);
	for (i=0;i<4;++i)
		AD[i]= impl::diffr2000AD<double>(B[i]);
	for (i=0;i<4;++i)
	{
		Ev = impl::diffr2000Ev<double>(A[i],AD[i],C[i]);
		Q[i]*=std::exp(std::complex<double>(0.0,k*l[i]))/l[i];
		temp+= Q[i]*A[i]*Ev;
	}
	temp /= -NumTraits<double>::pi;
	for (i=0;i<4;++i)
		l[i] = sqrt(RS*RS + RR*RR - 2*RS*RR*cos(theta[i]));

	if (theta[0]<NumTraits<double>::pi)
		tempex += std::exp(std::complex<double>(0.0,k*l[0]))/l[0];
	if (theta[1]<NumTraits<double>::pi)
		tempex += QR*std::exp(std::complex<double>(0.0,k*l[1]))/l[1];
	if (theta[2]<NumTraits<double>::pi)
		tempex += QS*std::exp(std::complex<double>(0.0,k*l[2]))/l[2];
		
	temp += tempex;

	return temp;
}

PyObject* pyDiffractionNord2000(PyObject *ignored,PyObject *args)
{
	/*
	std::complex<double> diffractionNord2000(
		const double& thetaR,const double& thetaS, const double& beta,
		const std::complex<double>& ZR,const std::complex<double>& ZS,
		const double& RS, const double& RR,const double& freq)
	{
	*/
	double thetaR,thetaS,beta,ZR,ZS,RS,RR,freq;
	if(!PyArg_ParseTuple(args, const_cast<char*>("dddCCddd"),
		&thetaR,&thetaS,&beta,&ZR,&ZS,&RS,&RR,&freq) )
	{
		PyErr_BadArgument();
		Py_INCREF( Py_None );
		return Py_None;
	}
	return lass::python::pyBuildSimpleObject( diffractionNord2000(thetaR,thetaS,beta,ZR,ZS,RS,RR,freq) );
}


static PyMethodDef FileMethods[] = { 
	{"diffractionNord2000", pyDiffractionNord2000, METH_VARARGS},
	{NULL, NULL}// Sentinel
};

void pyInitNumReflDiffraction(void)
{      
	Py_InitModule(const_cast<char*>("LassNum") , FileMethods);      
}

}

}