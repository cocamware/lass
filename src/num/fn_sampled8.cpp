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



#include "num_common.h"
#include "fn_sampled8.h"

#define DISCRETISATION_OF_ENTROPY 32
#define PDF_INTEGRAL_DISCRETISATION 32

namespace lass
{
	namespace num
	{
		PY_DECLARE_CLASS( FNSampled8 )
		PY_CLASS_CONSTRUCTOR_0( FNSampled8 )
		PY_CLASS_CONSTRUCTOR_1( FNSampled8, lass::util::CallTraits<FNSampled8::TBaseType>::TParam )
		PY_CLASS_METHOD( FNSampled8, makeUnit );
		PY_CLASS_METHOD( FNSampled8, makeZero );
		PY_CLASS_METHOD( FNSampled8, makeTriangular );
		PY_CLASS_METHOD( FNSampled8, makeGaussian );
		PY_CLASS_METHOD( FNSampled8, makeProbGaussian);
		PY_CLASS_METHOD( FNSampled8, makeTrapezoidal );
		PY_CLASS_METHOD( FNSampled8, getMembership );
		PY_CLASS_METHOD( FNSampled8, getEntropy );
		PY_CLASS_METHOD( FNSampled8, getDifferentialEntropy );
		PY_CLASS_METHOD( FNSampled8, getNonspecificity );
		PY_CLASS_METHOD( FNSampled8, getDiscord );
		PY_CLASS_METHOD_QUALIFIED_1( FNSampled8, getSupport, FNSampled8::TInterval, lass::util::CallTraits<FNSampled8::TBaseType>::TParam );
		PY_CLASS_METHOD( FNSampled8, defuzzifyCentroid );
		PY_CLASS_METHOD( FNSampled8, defuzzifyMaxMembership );

		FNSampled8::TBaseType FNSampled8::alphaLevel_[4] = {TBaseType(0.0),TBaseType(0.05),TBaseType(0.31),TBaseType(0.99)};

		FNSampled8::FNSampled8() :PyObjectPlus( &Type )
		{
			makeZero();
		}

		FNSampled8::~FNSampled8()
		{
		}

		#define LASS_NUM_DEFINE_NUMTRAITS_FNSAMPLED \
		const FNSampled8 NumTraits< FNSampled8 >::one = FNSampled8( NumTraits< FNSampled8::TBaseType >::one ); \
		const FNSampled8 NumTraits< FNSampled8 >::zero = FNSampled8( NumTraits< FNSampled8::TBaseType >::zero );\
		const FNSampled8 NumTraits< FNSampled8 >::pi = FNSampled8( NumTraits< FNSampled8::TBaseType >::pi );\
		const FNSampled8 NumTraits< FNSampled8 >::e = FNSampled8( NumTraits< FNSampled8::TBaseType >::e );\
		const FNSampled8 NumTraits< FNSampled8 >::sqrt2 = FNSampled8( NumTraits< FNSampled8::TBaseType >::sqrt2 );\
		const FNSampled8 NumTraits< FNSampled8 >::sqrtPi = FNSampled8( NumTraits< FNSampled8::TBaseType >::sqrtPi );

		LASS_NUM_DEFINE_NUMTRAITS_FNSAMPLED

		std::ostream& FNSampled8::write(std::ostream& os) const
		{
			int i;
			os << "FNSampled8(";
			os << alpha_[0].inf();
			for (i=1;i<3;++i)
			{
				os << "," << alpha_[i].inf();
			}
			os << ";" << alpha_[3].sup();
			for (i=2;i>=0;--i)
			{
				os << "," << alpha_[i].sup();
			}

			os << ")";
			return os;
		}


		lass::io::BinaryOStream& FNSampled8::write( lass::io::BinaryOStream& os) const
		{
			int i;
			for (i=0;i<4;++i)
			{
				os << alpha_[i].inf();
				os << alpha_[i].sup();
			}
			return os;
		}

		lass::io::BinaryIStream& FNSampled8::read( lass::io::BinaryIStream& is)
		{
			int i;
			for (i=0;i<4;++i)
			{
				lass::num::FNSampled8::TBaseType inf,sup;
				is >> inf;
				is >> sup;
				alpha_[i].set(inf,sup);
			}
			return is;
		}


		std::string str(const FNSampled8& iObj)
		{
			std::string temp = "FNS8[";
			int i;
			temp += str(iObj.alpha_[0].inf());
			for (i=1;i<4;++i)
				temp += "," + str(iObj.alpha_[i].inf());
			temp += "|" + str(iObj.alpha_[3].sup());
			for (i=2;i>=0;--i)
				temp += "," + str( iObj.alpha_[i].sup() );
			temp += "]";
			return temp;
		}

		FNSampled8::FNSampled8(util::CallTraits<FNSampled8::TBaseType>::TParam iV)
			: PyObjectPlus( &Type )
		{
			int i;
			for (i=3;i>=0;--i)
				alpha_[i].set(iV,iV);
		}

		void FNSampled8::makeZero()
		{
			int i;
			for (i=3;i>=0;--i)
				alpha_[i].set(0,0);
		}

		void FNSampled8::makeUnit()
		{
			int i;
			for (i=3;i>=0;--i)
				alpha_[i].set(0,0);
		}

		void FNSampled8::makeTriangular(util::CallTraits<TBaseType>::TParam left,util::CallTraits<TBaseType>::TParam mid,util::CallTraits<TBaseType>::TParam right)
		{
			for (int i=0; i<4; ++i)
				alpha_[i].set(
					left+(mid-left)*alphaLevel_[i],
					mid+(right-mid)*(static_cast<FNSampled8::TBaseType>(1.0)-alphaLevel_[i])
				);
		}

		void FNSampled8::makeTrapezoidal(util::CallTraits<TBaseType>::TParam a,util::CallTraits<TBaseType>::TParam b,util::CallTraits<TBaseType>::TParam c,util::CallTraits<TBaseType>::TParam d)
		{
			for (int i=0; i<4; ++i)
				alpha_[i].set(
					a+(b-a)*alphaLevel_[i],
					c+(d-c)*(static_cast<FNSampled8::TBaseType>(1.0)-alphaLevel_[i])
				);
		}


		/** produces a fuzzy number with a gaussian form.  Because the Gaussian distribution extends to
		*   infinity the alpha cut at zero actually represents the 3*sigma confidence interval.  This leads
		*   to an error of only 0.3 %.  The mean is situated at the y-axis at 1.0 instead of 1/(stddev*sqrt(2*PI))

			range CI
			sigma       0.6826895
			2*sigma     0.9544997
			3*sigma     0.9973002
		*/
		void FNSampled8::makeGaussian(util::CallTraits<TBaseType>::TParam mean, util::CallTraits<TBaseType>::TParam stddeviation)
		{
			for (int i=0; i<4; ++i)
			{
				TBaseType   temp;
				temp = num::sqrt( -((TBaseType)2.0/(TBaseType)num::log(2.7182818284590452353602874713527)) * num::log(std::max(static_cast<FNSampled8::TBaseType>(0.0015),(FNSampled8::alphaLevel_[i]))) );
				alpha_[i].set(mean-temp*stddeviation,mean+temp*stddeviation);
			}
		}

		/** constructs a gaussian which in the limit will have approximately the same uncertainty
		*   as the probability counterpart
		*
		*/
		void FNSampled8::makeProbGaussian(util::CallTraits<TBaseType>::TParam mean, util::CallTraits<TBaseType>::TParam stddeviation)
		{
			for (int i=0; i<4; ++i)
			{
				TBaseType   temp;
				temp = num::sqrt( -(TBaseType(2.0)/num::log(TBaseType(2.7182818284590452353602874713527))) * log(std::max(static_cast<FNSampled8::TBaseType>(0.0015),(pow(FNSampled8::alphaLevel_[i],static_cast<FNSampled8::TBaseType>(0.45))))) );
				alpha_[i].set(mean-temp*stddeviation,mean+temp*stddeviation);
			}
		}


		/** clamps the fuzzy number with left as the infinum and right as the supinum.  This function is useful when 
		*	numbers are generated externally but need to be within in a certain range to have physical
		*	meaning.
		*/
		void FNSampled8::clamp(util::CallTraits<TBaseType>::TParam left,util::CallTraits<TBaseType>::TParam right)
		{
			TInterval limits(left,right);
			for (int i=0;i<4;++i)
				alpha_[i] = set_intersect(limits,alpha_[i]);
		}


		FNSampled8::TInterval FNSampled8::getSupport(util::CallTraits<TBaseType>::TParam iAlpha)    const
		{
			if ((iAlpha>TBaseType(1.0)) || (iAlpha<TBaseType(0.0)))
				return TInterval::empty;

			FNSampled8::TBaseType   prop;

			int i=0;
			if (iAlpha>FNSampled8::alphaLevel_[1])  // i>0.33
			{
				i = 1;
				if (iAlpha>FNSampled8::alphaLevel_[2]) // i>0.66
				{
					i = 2;
					if (iAlpha>FNSampled8::alphaLevel_[3])
					{
						if ( alphaLevel_[3] < 1.0 )
						{
							prop = iAlpha-FNSampled8::alphaLevel_[3] / (static_cast<FNSampled8::TBaseType>(1.0) - alphaLevel_[3]);
							return TInterval(   alpha_[3].inf()+prop*(alpha_[3].mid()-alpha_[3].inf()),
											alpha_[3].sup()-prop*(alpha_[3].sup()-alpha_[3].mid()));
						}
						else
							return alpha_[3];
					}
				}

			}

			prop = (iAlpha-FNSampled8::alphaLevel_[i] ) / ( alphaLevel_[i+1] - alphaLevel_[i]);

			return TInterval(   alpha_[i].inf()+prop*(alpha_[i+1].inf()-alpha_[i].inf()),
								alpha_[i].sup()-prop*(alpha_[i].sup()-alpha_[i+1].sup()));
		}


		/**
		* @bug [VALIDATE] Needs to be checked if this is correct!
		*/
		FNSampled8::TBaseType   FNSampled8::getMembership(util::CallTraits<TBaseType>::TParam iX) const
		{
			if ((iX>=alpha_[0].sup()) || (iX<=alpha_[0].inf()))
				return 0.0;

			int i=0;
			if (alpha_[1].contains(iX))
			{
				i = 1;
				if (alpha_[2].contains(iX))
				{
					i = 2;
					if (alpha_[3].contains(iX))
					{
						if ( alpha_[3].diam() > 0.0)
							return static_cast<FNSampled8::TBaseType>(1.0 - fabs( (iX - alpha_[3].mid()) ) * 2.0 * fabs( (1.0 - alphaLevel_[3]) / alpha_[3].diam() ) );
						return static_cast<FNSampled8::TBaseType>(1.0);
					}
				}
			}

			FNSampled8::TBaseType prop;

			// left or right of max membership?
			if (iX < alpha_[i+1].inf())
			{
				// left
				prop = iX - alpha_[i].inf();
				return alphaLevel_[i] + prop * ( alphaLevel_[i+1] - alphaLevel_[i]) / ( alpha_[i+1].inf() - alpha_[i].inf() );
			}
			else
			{   // right
				prop = alpha_[i].sup() - iX;
				return alphaLevel_[i] + prop * ( alphaLevel_[i+1] - alphaLevel_[i]) / ( alpha_[i].sup() - alpha_[i+1].sup() );
			}
		}


		/** @warning This value is dependent on the discretisation step which may yield counterintuitive results!
		*/
		FNSampled8::TBaseType   FNSampled8::getEntropy() const
		{
			return getNonspecificity()+getDiscord();
		}

		/** Returns the differential shannon entropy, before that the curve is normalized to a probability distribution
		*   to make a valid comparison possible.  The same discretisation of the entropy is used.  However now the
		*   integral is discretised and not the possibility curve.
		*/
		FNSampled8::TBaseType   FNSampled8::getDifferentialEntropy() const
		{
			int i;
			TBaseType diffentropy = 0.0;
			TBaseType dx = alpha_[0].diam() / ( (TBaseType) DISCRETISATION_OF_ENTROPY );
			TBaseType norm = 0.0;

			for (i=0;i<DISCRETISATION_OF_ENTROPY;++i)
			{
				TBaseType x = alpha_[0].inf() + alpha_[0].diam()*(static_cast<TBaseType>(0.5)+(TBaseType)i)/((TBaseType)DISCRETISATION_OF_ENTROPY);
				TBaseType p = getMembership(x);
				norm += p*dx;
			}

			for (i=0;i<DISCRETISATION_OF_ENTROPY;++i)
			{
				TBaseType x = alpha_[0].inf() + alpha_[0].diam()*(static_cast<TBaseType>(0.5)+(TBaseType)i)/((TBaseType)DISCRETISATION_OF_ENTROPY);
				TBaseType p = getMembership(x)/norm;

				if ( p>0.0)
					diffentropy += static_cast<TBaseType>(p * log(p));
			}
			diffentropy *= dx;

			return diffentropy;
		}


		/** Nonspecificity is only defined on discrete sets.  So the continu set is first converted
		*   to a discrete one by equidistant sampling of the domain and not the image because that
		*   would yield the same value of the nonspecificity for all the fuzzy numbers.
		*/
		FNSampled8::TBaseType   FNSampled8::getNonspecificity() const
		{
			std::vector<FNSampled8::TBaseType>  poss;
			int i;
			for (i=0;i<DISCRETISATION_OF_ENTROPY;++i)
				poss.push_back(static_cast<FNSampled8::TBaseType>(getMembership(alpha_[0].inf()+alpha_[0].diam()*(((TBaseType)i)+static_cast<TBaseType>(0.5))/((TBaseType)DISCRETISATION_OF_ENTROPY) )));
			std::sort(poss.begin(),poss.end());

			FNSampled8::TBaseType   temp=0.0;
			if (poss.front()>=1.0)
				for (i=2;i<DISCRETISATION_OF_ENTROPY;++i)
					temp += poss[i-1]*static_cast<FNSampled8::TBaseType>(log((FNSampled8::TBaseType)i/(FNSampled8::TBaseType)(i-1)))/static_cast<FNSampled8::TBaseType>(log(2.0));
			else
				for (i=2;i<DISCRETISATION_OF_ENTROPY+1;++i)
					temp += poss[i-2]*static_cast<FNSampled8::TBaseType>(log((FNSampled8::TBaseType)i/(FNSampled8::TBaseType)(i-1)))/static_cast<FNSampled8::TBaseType>(log(2.0));

			return temp;
		}

		FNSampled8::TBaseType   FNSampled8::getDiscord() const
		{
			std::vector<FNSampled8::TBaseType>  poss;
			int i;
			for (i=0;i<DISCRETISATION_OF_ENTROPY;++i)
				poss.push_back(getMembership(alpha_[0].inf()+alpha_[0].diam()*(((TBaseType)i)+static_cast<TBaseType>(0.5))/((TBaseType)DISCRETISATION_OF_ENTROPY) ));
			std::sort(poss.begin(),poss.end());

			FNSampled8::TBaseType   temp=0.0;
			for (i=1;i<DISCRETISATION_OF_ENTROPY;++i)
			{
				double t=0.0;
				for (int j=i+1;j<DISCRETISATION_OF_ENTROPY;++j)
					t += poss[j-1]/((FNSampled8::TBaseType)(j*(j-1)));
				temp += (poss[i-1]-poss[i])*static_cast<FNSampled8::TBaseType>(log(1.0-(FNSampled8::TBaseType)i*t))/static_cast<FNSampled8::TBaseType>(log(2.0));
			}

			return -temp;
		}


		/** generate a pdf, method as in dubois and prade */
		void FNSampled8::generatePDF(std::vector<TBaseType>& oPDF, int iResolution ) const
		{
			int i;
			oPDF.clear();
			for (i=0;i<(iResolution+1);++i)
			{
				TBaseType x = alpha_[0].inf() + alpha_[0].diam()*((TBaseType)i)/((TBaseType)iResolution);
				TBaseType px = 0.0;
				// discretise the integral
				int j;
				TBaseType pix = getMembership(x);
				TBaseType da = pix/((TBaseType)PDF_INTEGRAL_DISCRETISATION);
				for (j=0;j<PDF_INTEGRAL_DISCRETISATION;++j)
				{
					TBaseType ipix = pix * ((TBaseType)j)/((TBaseType)PDF_INTEGRAL_DISCRETISATION);
					TBaseType supp = getSupport(ipix).diam();
					if (supp!=0.0)
						px += da/supp;
				}
				oPDF.push_back(px);
			}
		}

		void FNSampled8::generateDistribution(std::vector<TBaseType>& oDis , int iResolution) const
		{
			int i;
			oDis.clear();
			for (i=0;i<(iResolution+1);++i)
			{
				TBaseType x = alpha_[0].inf() + alpha_[0].diam()*((TBaseType)i)/((TBaseType)iResolution);
				TBaseType px = getMembership(x);
				oDis.push_back(px);
			}
		}


		FNSampled8::TBaseType   FNSampled8::defuzzifyCentroid() const
		{
			FNSampled8::TBaseType f = alphaLevel_[0] + static_cast<FNSampled8::TBaseType>(1.0 + 2.0 * ( alphaLevel_[1] + alphaLevel_[2] + alphaLevel_[3]));
			f = static_cast<FNSampled8::TBaseType>(1.0)/f;
			return  f *(      ( alphaLevel_[0] +    alphaLevel_[1] ) * alpha_[0].mid()
							+ ( alphaLevel_[1] + alphaLevel_[2] ) * alpha_[1].mid()
							+ ( alphaLevel_[2] + alphaLevel_[3] ) * alpha_[2].mid()
							+ ( alphaLevel_[3] + static_cast<FNSampled8::TBaseType>(1.0) )          * alpha_[3].mid() );
		}

		FNSampled8::TBaseType   FNSampled8::defuzzifyMaxMembership() const
		{
			return alpha_[3].mid();
		}

		FNSampled8  FNSampled8::operator-() const
		{
			return FNSampled8(-alpha_[0],-alpha_[1],-alpha_[2],-alpha_[3]);
		}

		bool FNSampled8::operator<(const FNSampled8& sfn)   const
		{
			return alpha_[0] < sfn.alpha_[0];
		}

		bool FNSampled8::operator==(const FNSampled8& sfn)  const
		{
			int i;
			for (i=3;i>=0;--i)
			{
				if (alpha_[i]!=sfn.alpha_[i])
					return false;
			}
			return true;
		}

		bool FNSampled8::operator==(int iV) const
		{
			for (int i=0;i<4;++i)
				if ((alpha_[i].sup()!=iV) || (alpha_[i].inf()!=iV))
					return false;
			return true;
		}

		bool FNSampled8::operator<(int iV) const
		{
			for (int i=0;i<4;++i)
				if ((alpha_[i].sup()>=iV) || (alpha_[i].inf()>=iV))
					return false;
			return true;
		}


		bool FNSampled8::operator!=(const FNSampled8& sfn)  const
		{
			return !(*this==sfn);
		}

		bool FNSampled8::operator<=(const FNSampled8& sfn)  const
		{
			return (*this<sfn) || (*this==sfn);
		}


		bool FNSampled8::operator>(const FNSampled8& sfn)   const
		{
			return alpha_[0] > sfn.alpha_[0];
		}

		bool FNSampled8::operator>=(const FNSampled8& sfn)  const
		{
			return (*this>sfn) || (*this==sfn);
		}

		bool FNSampled8::pe(const FNSampled8& sfn) const
		{
			return alpha_[0].pe(sfn.alpha_[0]);
		}

		bool FNSampled8::pne(const FNSampled8& sfn) const
		{
			return alpha_[0].pne(sfn.alpha_[0]);
		}

		bool FNSampled8::pge(const FNSampled8& sfn) const
		{
			return alpha_[0].pge(sfn.alpha_[0]);
		}

		bool FNSampled8::pg(const FNSampled8& sfn) const
		{
			return alpha_[0].pg(sfn.alpha_[0]);
		}

		bool FNSampled8::ple(const FNSampled8& sfn) const
		{
			return alpha_[0].ple(sfn.alpha_[0]);
		}

		bool FNSampled8::pl(const FNSampled8& sfn) const
		{
			return alpha_[0].pl(sfn.alpha_[0]);
		}

		FNSampled8::TBaseType FNSampled8::fe(const FNSampled8& sfn) const
		{
			FNSampled8::TBaseType f = alphaLevel_[0] + TBaseType(1.0) + TBaseType(2.0) * ( alphaLevel_[1] + alphaLevel_[2] + alphaLevel_[3]);
			f = TBaseType(1.0)/f;
			return  f *  (      ( alphaLevel_[0] + alphaLevel_[1] ) * alpha_[0].fe(sfn.alpha_[0])
							+   ( alphaLevel_[1] + alphaLevel_[2] ) * alpha_[1].fe(sfn.alpha_[1])
							+   ( alphaLevel_[2] + alphaLevel_[3] ) * alpha_[2].fe(sfn.alpha_[2])
							+   ( alphaLevel_[3] + TBaseType(1.0) )           * alpha_[3].fe(sfn.alpha_[3]) );
		}

		FNSampled8::TBaseType FNSampled8::fne(const FNSampled8& sfn) const
		{
			return  static_cast<FNSampled8::TBaseType>(1.0)-fe(sfn);
		}

		FNSampled8::TBaseType FNSampled8::fg(const FNSampled8& sfn) const
		{
			FNSampled8::TBaseType f = alphaLevel_[0] + TBaseType(1.0) + TBaseType(2.0) * ( alphaLevel_[1] + alphaLevel_[2] + alphaLevel_[3]);
			f = TBaseType(1.0)/f;
			return  f   * (     ( alphaLevel_[0] + alphaLevel_[1] ) * alpha_[0].fg(sfn.alpha_[0])
							+   ( alphaLevel_[1] + alphaLevel_[2] ) * alpha_[1].fg(sfn.alpha_[1])
							+   ( alphaLevel_[2] + alphaLevel_[3] ) * alpha_[2].fg(sfn.alpha_[2])
							+   ( alphaLevel_[3] + TBaseType(1.0) )           * alpha_[3].fg(sfn.alpha_[3]) );
		}

		FNSampled8::TBaseType FNSampled8::fge(const FNSampled8& sfn) const
		{
			return  std::max(fg(sfn),fe(sfn));
		}

		FNSampled8::TBaseType FNSampled8::fl(const FNSampled8& sfn) const
		{
			return  static_cast<FNSampled8::TBaseType>(1.0)-fg(sfn);
		}

		FNSampled8::TBaseType FNSampled8::fle(const FNSampled8& sfn) const
		{
			return  std::max(fl(sfn),fe(sfn));
		}

		FNSampled8& FNSampled8::operator+=(const FNSampled8& sfn)
		{
			int i;
			for (i=3;i>=0;--i)
				alpha_[i] += sfn.alpha_[i];
			return *this;
		}

		FNSampled8& FNSampled8::operator-=(const FNSampled8& sfn)
		{
			int i;
			for (i=3;i>=0;--i)
				alpha_[i] -= sfn.alpha_[i];
			return *this;
		}

		FNSampled8& FNSampled8::operator*=(const FNSampled8& sfn)
		{
			int i;
			for (i=3;i>=0;--i)
				alpha_[i] *= sfn.alpha_[i];
			return *this;
		}

		FNSampled8& FNSampled8::operator/=(const FNSampled8& sfn)
		{
			int i;
			for (i=3;i>=0;--i)
				alpha_[i] /= sfn.alpha_[i];
			return *this;
		}

		FNSampled8& FNSampled8::operator+=(util::CallTraits<TBaseType>::TParam n)
		{
			int i;
			for (i=3;i>=0;--i)
				alpha_[i] += n;
			return *this;
		}

		FNSampled8& FNSampled8::operator-=(util::CallTraits<TBaseType>::TParam n)
		{
			int i;
			for (i=3;i>=0;--i)
				alpha_[i] -= n;
			return *this;
		}

		FNSampled8& FNSampled8::operator*=(util::CallTraits<TBaseType>::TParam n)
		{
			int i;
			for (i=3;i>=0;--i)
				alpha_[i] *= n;
			return *this;
		}

		FNSampled8& FNSampled8::operator/=(util::CallTraits<TBaseType>::TParam n)
		{
			int i;
			TBaseType       t = static_cast<TBaseType>(1.0)/n;
			for (i=3;i>=0;--i)
				alpha_[i] *= t;
			return *this;
		}

		void inpexp(FNSampled8& sfn)
		{
			int i;
			for (i=3;i>=0;--i)
				inpexp(sfn.alpha_[i]);
		}

		void inplog(FNSampled8& sfn)
		{
			int i;
			for (i=3;i>=0;--i)
				inplog(sfn.alpha_[i]);
		}

		void inpsqr(FNSampled8& sfn)
		{
			int i;
			for (i=3;i>=0;--i)
				inpsqr(sfn.alpha_[i]);
		}

		void inpsqrt(FNSampled8&    sfn)
		{
			int i;
			for (i=3;i>=0;--i)
				inpsqrt(sfn.alpha_[i]);
		}

		void inpinv(FNSampled8& sfn)
		{
			int i;
			for (i=3;i>=0;--i)
				inpinv(sfn.alpha_[i]);
		}

		void inpnorm(FNSampled8& sfn)
		{
			int i;
			for (i=3;i>=0;--i)
				inpnorm(sfn.alpha_[i]);
		}

		void inpnorm(std::complex<FNSampled8>& sfn)
		{
			FNSampled8 temp,temp2;
			temp = sfn.real();
			temp*= sfn.real();
			temp2= sfn.imag();
			temp2*=sfn.imag();
			temp+= temp2;
			sfn = std::complex<FNSampled8>( temp, FNSampled8(0.0) );
		}

		void inpinv(std::complex<FNSampled8>& sfn)
		{
			FNSampled8 tempReal(sfn.real());
			FNSampled8 tempImag(sfn.imag());

			int i;
			for (i=3;i>=0;--i)
			{
				FNSampled8::TInterval alphaNorm = sfn.real().alpha_[i]*sfn.real().alpha_[i]+sfn.imag().alpha_[i]*sfn.imag().alpha_[i];
				tempReal.alpha_[i] /= alphaNorm;
				tempImag.alpha_[i] /= -alphaNorm;
			}
			sfn = std::complex<FNSampled8>( tempReal, tempImag );
		}


		FNSampled8  operator+(const FNSampled8& sfn1,const FNSampled8& sfn2)
		{
			FNSampled8  tmp = sfn1;
			tmp += sfn2;
			return tmp;
		}

		FNSampled8  operator-(const FNSampled8& sfn1,const FNSampled8& sfn2)
		{
			FNSampled8  tmp = sfn1;
			tmp -= sfn2;
			return tmp;
		}

		FNSampled8  operator*(const FNSampled8& sfn1,const FNSampled8& sfn2)
		{
			FNSampled8  tmp = sfn1;
			tmp *= sfn2;
			return tmp;
		}

		FNSampled8  operator/(const FNSampled8& sfn1,const FNSampled8& sfn2)
		{
			FNSampled8  tmp = sfn1;
			tmp /= sfn2;
			return tmp;
		}


		FNSampled8  operator+(const FNSampled8& sfn1,const FNSampled8::TBaseType& sfn2)
		{
			FNSampled8  tmp = sfn1;
			tmp += sfn2;
			return tmp;
		}

		FNSampled8  operator-(const FNSampled8& sfn1,const FNSampled8::TBaseType& sfn2)
		{
			FNSampled8  tmp = sfn1;
			tmp -= sfn2;
			return tmp;
		}

		FNSampled8  operator*(const FNSampled8& sfn1,const FNSampled8::TBaseType& sfn2)
		{
			FNSampled8  tmp = sfn1;
			tmp *= sfn2;
			return tmp;
		}

		FNSampled8  operator/(const FNSampled8& sfn1,const FNSampled8::TBaseType& sfn2)
		{
			FNSampled8  tmp = sfn1;
			tmp /= sfn2;
			return tmp;
		}


		void events(const FNSampled8& sfn1,const FNSampled8& sfn2,std::vector<FNSampled8::TBaseType>& outEvents)
		{
			outEvents.clear();
			int i,j;
			int F[4] = {0,0,1,1};
			int S[4] = {0,1,0,1};

			for (j=0;j<4;++j)
			{
				outEvents.push_back( sfn1.alpha_[j].inf() );
				outEvents.push_back( sfn2.alpha_[j].inf() );
				outEvents.push_back( sfn1.alpha_[j].sup() );
				outEvents.push_back( sfn2.alpha_[j].sup() );
				for (i=0;i<3;++i)
				{
					FNSampled8::TBaseType fma = std::max(sfn1.alpha_[i][F[j]],sfn1.alpha_[i+1][F[j]]);
					FNSampled8::TBaseType fmi = std::min(sfn1.alpha_[i][F[j]],sfn1.alpha_[i+1][F[j]]);
					FNSampled8::TInterval fi = FNSampled8::TInterval(fmi,fma);
					if ( fi.contains(sfn2.alpha_[i][S[j]]) || fi.contains(sfn2.alpha_[i+1][S[j]]))
					{
						FNSampled8::TBaseType x,C;
						C = (sfn1.alpha_[i+1][F[j]] - sfn1.alpha_[i][F[j]]);
						if ((sfn2.alpha_[i+1][S[j]] - sfn2.alpha_[i][S[j]])!=0.0)
						{
							C /= (sfn2.alpha_[i+1][S[j]] - sfn2.alpha_[i][S[j]]);
						}
						else
							break;
						x = sfn1.alpha_[i][F[j]]-sfn2.alpha_[i][S[j]]*C;
						if ((FNSampled8::TBaseType)(1.0-C) != (FNSampled8::TBaseType)0.0)
							x /= (FNSampled8::TBaseType)1.0-C;
						else
							break;
						if (fi.contains(x))
							outEvents.push_back(x);
					}
				}
			}
			std::sort( outEvents.begin(), outEvents.end() );
		}


		// in the global namespace
		/* functions are provided for convenience, but should be used with care as they require a lot of memory traffic */
		FNSampled8  log(const FNSampled8& sfn)
		{
			FNSampled8  temp=sfn;
			inplog(temp);
			return temp;
		}

		FNSampled8  exp(const FNSampled8& sfn)
		{
			FNSampled8  temp=sfn;
			inpexp(temp);
			return temp;
		}

		num::FNSampled8 sqr(const FNSampled8& sfn)
		{
			num::FNSampled8 temp=sfn;
			num::inpsqr(temp);
			return temp;
		}

		FNSampled8  sqrt(const FNSampled8& sfn)
		{
			FNSampled8  temp=sfn;
			inpsqrt(temp);
			return temp;
		}


		FNSampled8  inv(const FNSampled8& sfn)
		{
			num::FNSampled8 temp(sfn);
			num::inpinv(temp);
			return temp;
		}

		FNSampled8 norm(const std::complex<FNSampled8 >& iComplSFN)
		{
			std::complex<FNSampled8 > temp(iComplSFN.real(),iComplSFN.imag());
			num::inpnorm( temp );
			return temp.real();
		}

		std::string FNSampled8::pyStr(void)
		{
			return str( *this );
		}

	}

	namespace python
	{
		PyObject* pyBuildSimpleObject( const lass::num::FNSampled8& iV )
		{
			return new lass::num::FNSampled8(iV);
		}

		PyObject* pyBuildSimpleObject( const std::complex<lass::num::FNSampled8>& iV )
		{
			return Py_BuildValue("(O,O)", new lass::num::FNSampled8(iV.real()), new lass::num::FNSampled8(iV.imag()));
		}
		
		int pyGetSimpleObject( PyObject* iValue, lass::num::FNSampled8& oV )
		{
			if (iValue->ob_type==oV.ob_type)
			{
				oV = *static_cast<lass::num::FNSampled8*>(iValue);
				return 0;
			}
			return -1;
		}

		int pyGetSimpleObject( PyObject* iValue, std::complex<lass::num::FNSampled8>& oV )
		{
			std::pair<lass::num::FNSampled8,lass::num::FNSampled8> tempV;
			int r = pyGetSimpleObject(iValue, tempV);
			oV = std::complex<lass::num::FNSampled8>(tempV.first,tempV.second);
			return r;
		}

	}

	namespace io
	{
		lass::io::BinaryOStream& operator<<(lass::io::BinaryOStream& os, const lass::num::FNSampled8& sfn)
		{
			return sfn.write(os);
		}

		lass::io::BinaryIStream& operator>>(lass::io::BinaryIStream& is, lass::num::FNSampled8& sfn)
		{
			return sfn.read(is);
		}


	}
}




