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



#ifndef LASS_GUARDIAN_OF_INCLUSION_NUM_FN_SAMPLED8_H
#define LASS_GUARDIAN_OF_INCLUSION_NUM_FN_SAMPLED8_H

#include "num_common.h"
#include "num_traits.h"
#include "interval.h"
#include "../util/call_traits.h"
#include "../io/binary_i_stream.h"
#include "../io/binary_o_stream.h"
#include "../util/pyobject_plus.h"

namespace lass
{
	namespace num
	{
		/** Sampled Fuzzy Number with 8 control points.  A fuzzy number sampled with 4 alpha-cuts.  The alpha-cuts are
		*   statically determined, otherwise the alphas would need to be stored too.  Other classes
		*   can be used for that.  Speed was the main interest in developing this class.
		* @author Tom De Muer
		* @date 2002-2003
		*/
		class FNSampled8 : public lass::python::PyObjectPlus
		{
			PY_HEADER( lass::python::PyObjectPlus ) ;

			virtual std::string pyStr(void);
		public:
			typedef float   TBaseType;
			typedef float   TIntervalBaseType;
			typedef interval<float> TInterval;
		private:
			static  TBaseType   alphaLevel_[4];
		protected:
			TInterval   alpha_[4];  /**< 0->alpha = 0; 1->alpha = 0.33; 2->alpha = 0.66; 3->alpha = 1.0 */

			FNSampled8(const TInterval& ialphacut0,const TInterval& ialphacut1,const TInterval& ialphacut2,const TInterval& ialphacut3) : PyObjectPlus( &Type )
			{
				alpha_[0] = ialphacut0;
				alpha_[1] = ialphacut1;
				alpha_[2] = ialphacut2;
				alpha_[3] = ialphacut3;
			}
		public:
			static  const FNSampled8& zero() { static FNSampled8 sZero = FNSampled8(0.0); return sZero; }
			static  const FNSampled8& one()  { static FNSampled8 sOne  = FNSampled8(1.0); return sOne; }

			FNSampled8();
			/** By making this constructor explicit, the user of this class is pointed
			*   to the fact that these conversion are rather heavy-weight.  When a
			*   standard STL implementation is used, this may cause some problems with
			*   the complex class, but nothing that can't be quickly solved by explicitely
			*   casting the used int's there with the available template argument.
			*
			*   One such place is in the MSVC 2003 implementation of std::complex.  The
			*   default constructor fills in 0 as default.  To accomplish this the
			*   MSVC implementation uses default arguments for a two-valued constructor.
			*   A general default constructor is missing.  The ISO standard however does
			*   not demand such a constructor.
			*/
			FNSampled8(util::CallTraits<TBaseType>::TParam iV);
			FNSampled8(const FNSampled8& sV) : PyObjectPlus( &Type ) { int i=3; for (i=3;i>=0;--i) alpha_[i] = sV.alpha_[i]; };
			~FNSampled8();

			TInterval   getSupport() const  { return alpha_[0]; }
			TInterval   getSupport(util::CallTraits<TBaseType>::TParam iAlpha)  const;
			TBaseType   getMembership(util::CallTraits<TBaseType>::TParam iX) const;
			TBaseType   getEntropy() const;
			TBaseType   getDifferentialEntropy() const;
			TBaseType   getNonspecificity() const;
			TBaseType   getDiscreteNonspecificity() const;
			TBaseType   getDiscord() const;
			TBaseType   getDiscreteDiscord() const;

			TBaseType   defuzzifyCentroid() const;
			TBaseType   defuzzifyMaxMembership() const;

					void    makeUnit();
					void    makeZero();
					void    makeTriangular(util::CallTraits<TBaseType>::TParam left,util::CallTraits<TBaseType>::TParam mid,util::CallTraits<TBaseType>::TParam right);
					void    makeGaussian(util::CallTraits<TBaseType>::TParam mean, util::CallTraits<TBaseType>::TParam stddeviation);
					void    makeProbGaussian(util::CallTraits<TBaseType>::TParam mean, util::CallTraits<TBaseType>::TParam stddeviation);
					void    makeTrapezoidal(util::CallTraits<TBaseType>::TParam a,util::CallTraits<TBaseType>::TParam b,util::CallTraits<TBaseType>::TParam c,util::CallTraits<TBaseType>::TParam d);
					void	clamp(util::CallTraits<TBaseType>::TParam left,util::CallTraits<TBaseType>::TParam right);

			FNSampled8& operator+=(const FNSampled8& sfn);
			FNSampled8& operator-=(const FNSampled8& sfn);
			FNSampled8& operator*=(const FNSampled8& sfn);
			FNSampled8& operator/=(const FNSampled8& sfn);

			FNSampled8& operator+=(util::CallTraits<TBaseType>::TParam n);
			FNSampled8& operator-=(util::CallTraits<TBaseType>::TParam n);
			FNSampled8& operator*=(util::CallTraits<TBaseType>::TParam n);
			FNSampled8& operator/=(util::CallTraits<TBaseType>::TParam n);

			FNSampled8  operator-() const;
			FNSampled8& operator+()     { return *this; }

			/** certainly less than */
			bool operator<(const FNSampled8& sfn)   const;
			/** certainly equal */
			bool    operator==(const FNSampled8& sfn)   const;
			/** certainly not equal */
			bool    operator!=(const FNSampled8& sfn)   const;
			/** certainly less than or equal */
			bool    operator<=(const FNSampled8& sfn)   const;
			/** certainly greater than */
			bool    operator>(const FNSampled8& sfn)    const;
			/** certainly greater than or equal */
			bool    operator>=(const FNSampled8& sfn)   const;

			/** certainly equal (provided to break as less code as possible, fi std::complex */
			bool operator==(int i) const;
			/** certainly less than (provided to break as less code as possible, fi std::complex */
			bool operator<(int i) const;

			/** possibly equal */
			bool pe(const FNSampled8& sfn) const;
			/** possibly not equal */
			bool pne(const FNSampled8& sfn) const;
			/** possibly greater than */
			bool pg(const FNSampled8& sfn) const;
			/** possibly greater or equal than */
			bool pge(const FNSampled8& sfn) const;
			/** possibly less than */
			bool pl(const FNSampled8& sfn) const;
			/** possibly less or equal than */
			bool ple(const FNSampled8& sfn) const;

			/** fuzzy equal */
			TBaseType fe(const FNSampled8& sfn) const;
			/** fuzzy not equal */
			TBaseType fne(const FNSampled8& sfn) const;
			/** fuzzy greater than */
			TBaseType fg(const FNSampled8& sfn) const;
			/** fuzzy greater or equal than */
			TBaseType fge(const FNSampled8& sfn) const;
			/** fuzzy less than */
			TBaseType fl(const FNSampled8& sfn) const;
			/** fuzzy less or equal than */
			TBaseType fle(const FNSampled8& sfn) const;

			/** generate a pdf - Dubois and Prade method */
			void generatePDF(std::vector<TBaseType>& oPDF , int iResolution) const;
			/** discretise the possibility function */
			void generateDistribution(std::vector<TBaseType>& oDis , int iResolution) const;


			friend  void inpexp(FNSampled8& sfn);
			friend  void inplog(FNSampled8& sfn);
			friend  void inpsqr(FNSampled8& sfn);
			friend  void inpsqrt(FNSampled8& sfn);
			friend  void inpsin(FNSampled8& sfn);
			friend  void inpcos(FNSampled8& sfn);
			friend  void inptan(FNSampled8& sfn);
			friend  void inpatan(FNSampled8& sfn);
			friend  void inpinv(FNSampled8& sfn);
			friend  void inpnorm(FNSampled8& sfn);

			friend  FNSampled8 exp(const FNSampled8& sfn);
			friend  FNSampled8 log(const FNSampled8& sfn);
			friend  FNSampled8 sqr(const FNSampled8& sfn);
			friend  FNSampled8 sqrt(const FNSampled8& sfn);
			friend  FNSampled8 sin(const FNSampled8& sfn);
			friend  FNSampled8 cos(const FNSampled8& sfn);
			friend  FNSampled8 tan(const FNSampled8& sfn);
			friend  FNSampled8 atan(const FNSampled8& sfn);
			friend  FNSampled8 inv(const FNSampled8& sfn);

			friend  void inpinv(std::complex<FNSampled8 >& ioComplSFN);
			friend  std::complex<FNSampled8 > inv(const std::complex<FNSampled8 >& ioComplSFN);
			friend  void inpnorm(std::complex<FNSampled8 >& iComplSFN);
			friend  FNSampled8 norm(const std::complex<FNSampled8 >& iComplSFN);

			friend  std::string str(const FNSampled8& iObj);

			lass::io::BinaryOStream& write(lass::io::BinaryOStream& os) const;
			lass::io::BinaryIStream& read(lass::io::BinaryIStream& is);
			std::ostream& write(std::ostream& os) const;

			friend void events(const FNSampled8& sfn1,const FNSampled8& sfn2,std::vector<FNSampled8::TBaseType>& outEvents);
		};

		FNSampled8  operator+( const FNSampled8& iV1, const FNSampled8& iV2 );
		FNSampled8  operator-( const FNSampled8& iV1, const FNSampled8& iV2 );
		FNSampled8  operator*( const FNSampled8& iV1, const FNSampled8& iV2 );
		FNSampled8  operator/( const FNSampled8& iV1, const FNSampled8& iV2 );

		FNSampled8  operator+( const FNSampled8& iV1, const FNSampled8::TBaseType& iV2 );
		FNSampled8  operator-( const FNSampled8& iV1, const FNSampled8::TBaseType& iV2 );
		FNSampled8  operator*( const FNSampled8& iV1, const FNSampled8::TBaseType& iV2 );
		FNSampled8  operator/( const FNSampled8& iV1, const FNSampled8::TBaseType& iV2 );

		inline FNSampled8& clamp(FNSampled8& iV, const FNSampled8::TBaseType& iLeft, const FNSampled8::TBaseType& iRight ) { iV.clamp(iLeft,iRight); return iV; }

		/** Shadow type for FNSampled8 for use in Python.  This makes the original class
		*   as lightweight as possible and once used in Python speed/footprint isn't that
		*   critical anymore.
		* @author Tom De Muer
		* @date 2003
		*/
		/*
		class PyFNSampled8 :
			public lass::python::PyObjectPlus
		{
			PY_HEADER( lass::python::PyObjectPlus ) ;

			std::string repr(void);// _repr method

			typedef FNSampled8::TBaseType TBaseType;

			PyObject* pyMakeUnit(PyObject *args);
			PyObject* pyMakeZero(PyObject *args);
			PyObject* pyMakeTriangular(PyObject *args);
			PyObject* pyMakeGaussian(PyObject *args);
			PyObject* pyMakeProbGaussian(PyObject *args);
			PyObject* pyMakeTrapezoidal(PyObject *args);

			TBaseType   getMembership(util::CallTraits<TBaseType>::TParam iX) const { return value.getMembership(iX); }
			TBaseType   getEntropy() const { return value.getEntropy(); }
			TBaseType   getDifferentialEntropy() const { return value.getDifferentialEntropy(); }
			TBaseType   getNonspecificity() const { return value.getNonspecificity(); }
			TBaseType   getDiscreteNonspecificity() const { return value.getDiscreteNonspecificity(); }
			TBaseType   getDiscord() const { return value.getDiscord(); }
			TBaseType   getDiscreteDiscord() const { return value.getDiscreteDiscord(); }

			TBaseType   defuzzifyCentroid() const { return value.defuzzifyCentroid(); }
			TBaseType   defuzzifyMaxMembership() const { return value.defuzzifyMaxMembership(); }

		public:
			FNSampled8  value;

			static  PyFNSampled8 zero()     { return FNSampled8::zero(); }
			static  PyFNSampled8 one()      { return FNSampled8::one(); }

			PyFNSampled8() : PyObjectPlus( &Type ) {}
			PyFNSampled8(const FNSampled8& sV) : PyObjectPlus( &Type ) { value = sV; }
			virtual ~PyFNSampled8() {}

			virtual std::string pyStr(void);
		};

		extern "C" { void initPyFNSampled8(void); }
		*/



		template<>
		struct NumTraits< FNSampled8 >
		{
			typedef FNSampled8                  selfType;
			typedef FNSampled8::TBaseType   baseType;
			typedef FNSampled8::TInterval   intervalType;
			static const bool  isDistribution = true;
			static const bool  isIntegral = false;
			static const bool  isNative = false;
			static const bool  isSigned = true;
			static const bool  hasInfinity = false;
			static const int   memorySize = sizeof( FNSampled8 );
			static const std::string name() { return "FNSampled8<"+NumTraits<baseType>::name()+">"; }
			static const FNSampled8 one;
			static const FNSampled8 zero;
			static const FNSampled8 pi;
			static const FNSampled8 e;
			static const FNSampled8 sqrt2;
			static const FNSampled8 sqrtPi;
		};

		std::complex<FNSampled8 >& operator*=(std::complex<FNSampled8 >& isfn,const lass::num::FNSampled8::TBaseType& iN);
		std::complex<FNSampled8 >& operator/=(std::complex<FNSampled8 >& isfn,const lass::num::FNSampled8::TBaseType& iN);
		std::complex<FNSampled8 >& operator+=(std::complex<FNSampled8 >& isfn,const lass::num::FNSampled8::TBaseType& iN);
		std::complex<FNSampled8 >& operator-=(std::complex<FNSampled8 >& isfn,const lass::num::FNSampled8::TBaseType& iN);

	}

	namespace python
	{
		int pyGetSimpleObject( PyObject* iValue, lass::num::FNSampled8& oV );
		PyObject* pyBuildSimpleObject( const std::complex<lass::num::FNSampled8>& iV );
		PyObject* pyBuildSimpleObject( std::complex<lass::num::FNSampled8>& iV );
		PyObject* pyBuildSimpleObject( const lass::num::FNSampled8& iV );
	}

	namespace io
	{
		lass::io::BinaryOStream& operator<<(lass::io::BinaryOStream& os, const lass::num::FNSampled8& sfn);
		lass::io::BinaryIStream& operator>>(lass::io::BinaryIStream& is, lass::num::FNSampled8& sfn);
	}
}

inline std::ostream& operator<<(std::ostream& os, const lass::num::FNSampled8& sfn) { return sfn.write(os); }

#endif
