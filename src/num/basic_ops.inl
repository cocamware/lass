/** @file
 *  @author Bram de Greve (bramz@users.sourceforge.net)
 *  @author Tom De Muer (tomdemuer@users.sourceforge.net)
 *
 *  Distributed under the terms of the GPL (GNU Public License)
 *
 *  The LASS License:
 *
 *  Copyright 2004-2006 Bram de Greve and Tom De Muer
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

// --- implementation details ----------------------------------------------------------------------

// http://pi.lacim.uqam.ca/eng/table_en.html
#define LASS_NUM_INVLOG2  1.442695040888963407359924681001892137426645954152985934135449406931109219181185079885526622893506344
#define LASS_NUM_INVLOG10 0.434294481903251827651128918916605082294397005803666566114453783165864649208870774729224949338431748

namespace impl
{

template <typename T> 
struct IntPow
{
	typedef typename NumTraits<T>::signedType TSigned;
	typedef typename NumTraits<T>::unsignedType TUnsigned;

	static T eval(TSigned iX, TSigned iY)
	{		LASS_ASSERT(iY >= 0);
		return eval(iX, static_cast<TUnsigned>(iY));
	}

	static T eval(TSigned iX, TUnsigned iY)
	{
		if (iY == 0)
		{
			return 1;
		}

		TSigned result = 0;
		TSigned partialPower = 1;
		do
		{
			partialPower *= iX;
			if (iY & 0x1)
			{
				result += partialPower;
			}
			iY >>= 1;
		}
		while (iY);
		return result;
	}
};

template <typename T> 
struct IntDiv
{
	typedef typename NumTraits<T>::signedType TSigned;
	typedef typename NumTraits<T>::unsignedType TUnsigned;

	static T eval(TSigned iV, TSigned iMod)
	{
		LASS_ASSERT(iMod > 0);
		const TSigned tempDiv = iV / iMod;
		return iV % iMod >= 0 ? tempDiv : (tempDiv - 1);
	}

	static T eval(TSigned iV, TUnsigned iMod)
	{
		LASS_ASSERT(iMod > 0);
		const TSigned signedMod = static_cast<TSigned>(iMod);
		LASS_ASSERT(signedMod >= 0);
		const TSigned tempDiv = iV / signedMod;
		return iV % signedMod >= 0 ? tempDiv : (tempDiv - 1);
	}
};

template <typename T> 
struct IntMod
{
	typedef typename NumTraits<T>::signedType TSigned;
	typedef typename NumTraits<T>::unsignedType TUnsigned;

	static T eval(TSigned iV, TSigned iMod)
	{
		LASS_ASSERT(iMod > 0);
		const TSigned tempMod = iV % iMod;
		return tempMod >= 0 ? tempMod : (tempMod + iMod);
	}

	static T eval(TSigned iV, TUnsigned iMod)
	{
		LASS_ASSERT(iMod > 0);
		const TSigned signedMod = static_cast<TSigned>(iMod);
		LASS_ASSERT(signedMod >= 0);
		const TSigned tempMod = iV % signedMod;
		return tempMod >= 0 ? tempMod : (tempMod + signedMod);
	}
};

}

// --- generic functions ---------------------------------------------------------------------------

/** if iV < 0 return -iV, else return iV.
 *  @ingroup BasicOps
 */
template <typename T> inline T abs(const T& iV)
{
	return iV < T() ? -iV : iV;
}

/** if iV < 0 return -1, else if iV > 0 return 1, else return 0.
 *  @ingroup BasicOps
 */
template <typename T> inline T sign(const T& iV)
{
	const T zero = T();
	return iV > zero ? T(1) : (iV < zero ? T(-1) : zero);
}

/** return iV * iV
 *  @ingroup BasicOps
 */
template <typename T> inline T sqr(const T& iV) 
{ 
	return iV * iV; 
}

/** return iV * iV * iV
 *  @ingroup BasicOps
 */
template <typename T> inline T cubic(const T& iV) 
{ 
	return iV * iV * iV; 
}

/** return iV ^ -1
 *  @ingroup BasicOps
 */
template <typename T> inline T inv(const T& iV) 
{ 
	return T(1) / iV;
}

/** return exp(iPow * log(iV));
 *  @ingroup BasicOps
 *  @warning GENERIC FUNCTION USES EXP AND LOG!
 */
template <typename T> inline T pow(const T& iV, const T& iPow) 
{ 
	return exp(iPow * log(iV));
}

/** return log(iV) / log(2)
 *  @ingroup BasicOps
 *  @warning GENERIC FUNCTION USES DIVISION AND LOG!
 */
template <typename T> inline T log2(const T& iV, const T& iPow) 
{ 
	return log(iV) / log(T(2));
}

/** return log(iV) / log(10)
 *  @ingroup BasicOps
 *  @warning GENERIC FUNCTION USES DIVISION AND LOG!
 */
template <typename T> inline T log10(const T& iV, const T& iPow) 
{ 
	return log(iV) / log(T(10));
}

/** return norm of iV as if iV is real part of complex number: sqr(iV)
 *  @ingroup BasicOps
 */
template <typename T> inline T norm(const T& iV)
{
	return sqr(iV);
}

/** return conjugate as if iV is a complex number: iV
 */
template <typename T> inline T conj(const T& iV)
{
	return iV;
}

/** if iV < iMin return iMin, else if iV > iMax return iMax, else return iV.
 *  @ingroup BasicOps
 */
template <typename T> inline const T& clamp(const T& iV, const T& iMin, const T& iMax)
{
	return iV < iMin ? iMin : (iV > iMax ? iMax : iV);
}

/** @ingroup BasicOps
 */
template<typename T,typename f> T applyFunction(const T& iV, f func ) 
{ 
	return func(iV); 
}



// --- generic inplace rerouters -------------------------------------------------------------------

template <typename T> void inpabs(T& ioV)						{ ioV = num::abs(ioV); }				/**< @ingroup BasicOps */
template <typename T> void inpsign(T& ioV)						{ ioV = num::sign(ioV); }				/**< @ingroup BasicOps */
template <typename T> void inpinv(T& ioV)						{ ioV = num::inv(ioV); }				/**< @ingroup BasicOps */
template <typename T> void inpsqrt(T& ioV)						{ ioV = num::sqrt(ioV); }				/**< @ingroup BasicOps */
template <typename T> void inpsqr(T& ioV)						{ ioV = num::sqr(ioV); }				/**< @ingroup BasicOps */
template <typename T> void inpcubic(T& ioV)						{ ioV = num::cubic(ioV); }				/**< @ingroup BasicOps */
template <typename T> void inppow(T& ioV, const T& iPow)		{ ioV = num::pow(ioV, iPow); }			/**< @ingroup BasicOps */
template <typename T> void inpexp(T& ioV)						{ ioV = num::exp(ioV); }				/**< @ingroup BasicOps */
template <typename T> void inplog(T& ioV)						{ ioV = num::log(ioV); }				/**< @ingroup BasicOps */
template <typename T> void inpcos(T& ioV)						{ ioV = num::cos(ioV); }				/**< @ingroup BasicOps */
template <typename T> void inpsin(T& ioV)						{ ioV = num::sin(ioV); }				/**< @ingroup BasicOps */
template <typename T> void inptan(T& ioV)						{ ioV = num::tan(ioV); }				/**< @ingroup BasicOps */
template <typename T> void inpacos(T& ioV)						{ ioV = num::acos(ioV); }				/**< @ingroup BasicOps */
template <typename T> void inpasin(T& ioV)						{ ioV = num::asin(ioV); }				/**< @ingroup BasicOps */
template <typename T> void inpatan(T& ioV)						{ ioV = num::atan(ioV); }				/**< @ingroup BasicOps */
template <typename T> void inpatan2(T& ioY, const T& iX)		{ ioX = num::atan2(ioY, iX); }			/**< @ingroup BasicOps */
template <typename T> void inpfloor(T& ioV)						{ ioV = num::floor(ioV); }				/**< @ingroup BasicOps */
template <typename T> void inpceil(T& ioV)						{ ioV = num::ceil(ioV); }				/**< @ingroup BasicOps */
template <typename T> void inpround(T& ioV)						{ ioV = num::round(ioV); }				/**< @ingroup BasicOps */
template <typename T> void inpfractional(T& ioV)				{ ioV -= num::floor(ioV); }				/**< @ingroup BasicOps */
template <typename T> void inpdiv(T& ioV, const T& iMod)		{ ioV = num::div(ioV, iMod); }			/**< @ingroup BasicOps */
template <typename T> void inpmod(T& ioV, const T& iMod)		{ ioV = num::mod(ioV, iMod); }			/**< @ingroup BasicOps */
template <typename T> void inpclamp(T& ioV, const T& iMin, const T& iMax)	{ ioV = num::clamp(ioV, iMin, iMax); }	/** @ingroup BasicOps */

template <typename T> void compnorm(const T& iV, T& oV)			{ oV = num::norm(iV); }					/**< @ingroup BasicOps */
template <typename T> void compinv(const T& iV, T& oV)			{ oV = num::inv(iV); }					/**< @ingroup BasicOps */



// --- float ---------------------------------------------------------------------------------------

float abs(float iV)				{ return ::fabsf(iV); }			/**< @ingroup BasicOps */
float inv(float iV)				{ return 1.f / iV; }			/**< @ingroup BasicOps */
float sqrt(float iV)			{ LASS_ASSERT(iV >= 0.f); return ::sqrtf(iV); }	/**< @ingroup BasicOps */
float pow(float iV, float iPow)	{ return ::powf(iV, iPow); }	/**< @ingroup BasicOps */
float exp(float iV)				{ return ::expf(iV); }			/**< @ingroup BasicOps */
float log(float iV)				{ return ::logf(iV); }			/**< @ingroup BasicOps */
float log2(float iV)			{ return float(LASS_NUM_INVLOG2) * ::logf(iV); } /**< @ingroup BasicOps */
float log10(float iV)			{ return ::log10f(iV); }		/**< @ingroup BasicOps */
float cos(float iV)				{ return ::cosf(iV); }			/**< @ingroup BasicOps */
float sin(float iV)				{ return ::sinf(iV); }			/**< @ingroup BasicOps */
float tan(float iV)				{ return ::tanf(iV); }			/**< @ingroup BasicOps */
float acos(float iV)			{ return ::acosf(iV); }			/**< @ingroup BasicOps */
float asin(float iV)			{ return ::asinf(iV); }			/**< @ingroup BasicOps */
float atan(float iV)			{ return ::atanf(iV); }			/**< @ingroup BasicOps */
float atan2(float iY, float iX)	{ return ::atan2f(iY, iX); }	/**< @ingroup BasicOps */
float sinc(float iV)			{ return ::fabsf(iV) < 1e-4f ? 1.f : (::sinf(iV) / iV); }	/**< @ingroup BasicOps */
float floor(float iV)			{ return ::floorf(iV); }		/**< @ingroup BasicOps */
float ceil(float iV)			{ return ::ceilf(iV); }			/**< @ingroup BasicOps */
float round(float iV)			{ return ::floorf(iV + .5f); }	/**< @ingroup BasicOps */
float fractional(float iV)		{ return iV - ::floorf(iV); }	/**< @ingroup BasicOps */

/** @ingroup BasicOps */
float mod(float iV, float iMod) 
{ 
	const float result = ::fmodf(iV, iMod);
	return result < 0.f ? result + iMod : result;
}	



// --- double --------------------------------------------------------------------------------------

double abs(double iV)				{ return ::fabs(iV); }			/**< @ingroup BasicOps */
double inv(double iV)				{ return 1. / iV; }				/**< @ingroup BasicOps */
double sqrt(double iV)				{ LASS_ASSERT(iV >= 0.); return ::sqrt(iV); }	/**< @ingroup BasicOps */
double pow(double iV, double iPow)	{ return ::pow(iV, iPow); }		/**< @ingroup BasicOps */
double exp(double iV)				{ return ::exp(iV); }			/**< @ingroup BasicOps */
double log(double iV)				{ return ::log(iV); }			/**< @ingroup BasicOps */
double log2(double iV)				{ return double(LASS_NUM_INVLOG2) * ::log(iV); } /**< @ingroup BasicOps */
double log10(double iV)				{ return ::log10(iV); }			/**< @ingroup BasicOps */
double cos(double iV)				{ return ::cos(iV); }			/**< @ingroup BasicOps */
double sin(double iV)				{ return ::sin(iV); }			/**< @ingroup BasicOps */
double tan(double iV)				{ return ::tan(iV); }			/**< @ingroup BasicOps */
double acos(double iV)				{ return ::acos(iV); }			/**< @ingroup BasicOps */
double asin(double iV)				{ return ::asin(iV); }			/**< @ingroup BasicOps */
double atan(double iV)				{ return ::atan(iV); }			/**< @ingroup BasicOps */
double atan2(double iY, double iX)	{ return ::atan2(iY, iX); }		/**< @ingroup BasicOps */
double sinc(double iV)				{ return ::fabs(iV) < 1e-8 ? 1. : (::sin(iV) / iV); }	/**< @ingroup BasicOps */
double floor(double iV)				{ return ::floor(iV); }			/**< @ingroup BasicOps */
double ceil(double iV)				{ return ::ceil(iV); }			/**< @ingroup BasicOps */
double round(double iV)				{ return ::floor(iV + .5); }	/**< @ingroup BasicOps */
double fractional(double iV)		{ return iV - ::floor(iV); }	/**< @ingroup BasicOps */

/** @ingroup BasicOps */
double mod(double iV, double iMod) 
{ 
	const double result = ::fmod(iV, iMod);
	return result < 0. ? result + iMod : result;
}	



// --- long double ---------------------------------------------------------------------------------

#if LASS_PLATFORM_TYPE == LASS_PLATFORM_TYPE_CYGWIN

long double abs(long double iV)	{ return static_cast<long double>(abs(static_cast<double>(iV))); }
long double inv(long double iV) { return static_cast<long double>(inv(static_cast<double>(iV))); }
long double sqrt(long double iV) { return static_cast<long double>(sqrt(static_cast<double>(iV))); }
long double pow(long double iV, long double iPow) { return static_cast<long double>(pow(static_cast<double>(iV), static_cast<double>(iPow))); }
long double exp(long double iV) { return static_cast<long double>(exp(static_cast<double>(iV))); }
long double log(long double iV) { return static_cast<long double>(log(static_cast<double>(iV))); }
long double log2(long double iV) { return static_cast<long double>(log2(static_cast<double>(iV))); }
long double log10(long double iV) { return static_cast<long double>(log10(static_cast<double>(iV))); }
long double cos(long double iV) { return static_cast<long double>(cos(static_cast<double>(iV))); }
long double sin(long double iV) { return static_cast<long double>(sin(static_cast<double>(iV))); }
long double tan(long double iV) { return static_cast<long double>(tan(static_cast<double>(iV))); }
long double acos(long double iV) { return static_cast<long double>(acos(static_cast<double>(iV))); }
long double asin(long double iV) { return static_cast<long double>(asin(static_cast<double>(iV))); }
long double atan(long double iV) { return static_cast<long double>(atan(static_cast<double>(iV))); }
long double atan2(long double iY, long double iX) { return static_cast<long double>(atan2(static_cast<double>(iY), static_cast<double>(iX))); }
long double sinc(long double iV) { return static_cast<long double>(sinc(static_cast<double>(iV))); }
long double floor(long double iV) { return static_cast<long double>(floor(static_cast<double>(iV))); }
long double ceil(long double iV) { return static_cast<long double>(ceil(static_cast<double>(iV))); }
long double round(long double iV) { return static_cast<long double>(round(static_cast<double>(iV))); }
long double fractional(long double iV) { return static_cast<long double>(fractional(static_cast<double>(iV))); }
long double mod(long double iV, long double iMod) { return static_cast<long double>(mod(static_cast<double>(iV), static_cast<double>(iMod))); } 

#else

long double abs(long double iV)						{ return ::fabsl(iV); }			/**< @ingroup BasicOps */
long double inv(long double iV)						{ return 1. / iV; }				/**< @ingroup BasicOps */
long double sqrt(long double iV)					{ LASS_ASSERT(iV >= 0.); return ::sqrtl(iV); }	/**< @ingroup BasicOps */
long double pow(long double iV, long double iPow)	{ return ::powl(iV, iPow); }	/**< @ingroup BasicOps */
long double exp(long double iV)						{ return ::expl(iV); }			/**< @ingroup BasicOps */
long double log(long double iV)						{ return ::logl(iV); }			/**< @ingroup BasicOps */
long double log2(long double iV)					{ return long double(LASS_NUM_INVLOG2) * ::logl(iV); } /**< @ingroup BasicOps */
long double log10(long double iV)					{ return ::log10l(iV); }		/**< @ingroup BasicOps */
long double cos(long double iV)						{ return ::cosl(iV); }			/**< @ingroup BasicOps */
long double sin(long double iV)						{ return ::sinl(iV); }			/**< @ingroup BasicOps */
long double tan(long double iV)						{ return ::tanl(iV); }			/**< @ingroup BasicOps */
long double acos(long double iV)					{ return ::acosl(iV); }			/**< @ingroup BasicOps */
long double asin(long double iV)					{ return ::asinl(iV); }			/**< @ingroup BasicOps */
long double atan(long double iV)					{ return ::atanl(iV); }			/**< @ingroup BasicOps */
long double atan2(long double iY, long double iX)	{ return ::atan2l(iY, iX); }	/**< @ingroup BasicOps */
long double sinc(long double iV)					{ return ::fabsl(iV) < 1e-10 ? 1. : (::sinl(iV) / iV); }	/**< @ingroup BasicOps */
long double floor(long double iV)					{ return ::floorl(iV); }		/**< @ingroup BasicOps */
long double ceil(long double iV)					{ return ::ceill(iV); }			/**< @ingroup BasicOps */
long double round(long double iV)					{ return ::floorl(iV + .5); }	/**< @ingroup BasicOps */
long double fractional(long double iV)				{ return iV - ::floorl(iV); }	/**< @ingroup BasicOps */

/** @ingroup BasicOps */
long double mod(long double iV, long double iMod) 
{ 
	const long double result = ::fmodl(iV, iMod);
	return result < 0. ? result + iMod : result;
}	

#endif


// --- char ----------------------------------------------------------------------------------------

#ifdef LASS_CHAR_IS_SIGNED

char pow(char iV, char iPow)			{ return impl::IntPow<char>::eval(iV, static_cast<signed char>(iPow)); }	/**< @ingroup BasicOps */
char pow(char iV, unsigned char iPow)	{ return impl::IntPow<char>::eval(iV, iPow); }	/**< @ingroup BasicOps */
char div(char iV, char iMod)			{ return impl::IntDiv<char>::eval(iV, static_cast<signed char>(iMod)); }	/**< @ingroup BasicOps */
char div(char iV, unsigned char iMod)	{ return impl::IntDiv<char>::eval(iV, iMod); }	/**< @ingroup BasicOps */
char mod(char iV, char iMod)			{ return impl::IntMod<char>::eval(iV, static_cast<signed char>(iMod)); }	/**< @ingroup BasicOps */
char mod(char iV, unsigned char iMod)	{ return impl::IntMod<char>::eval(iV, iMod); }	/**< @ingroup BasicOps */

#else

char abs(char iV)				{ return iV; }				/**< @ingroup BasicOps */
char sign(char iV)				{ return iV > 0 ? 1 : 0; }	/**< @ingroup BasicOps */
char pow(char iV, char iPow)	{ return impl::IntPow<char>::eval(iV, static_cast<unsigned char>(iPow)); }	/**< @ingroup BasicOps */
char div(char iV, char iPow)	{ return iV / iPow; }		/**< @ingroup BasicOps */
char mod(char iV, char iPow)	{ return iV % iPow; }		/**< @ingroup BasicOps */

#endif

char floor(char iV)				{ return iV; }				/**< @ingroup BasicOps */
char ceil(char iV)				{ return iV; }				/**< @ingroup BasicOps */
char round(char iV)				{ return iV; }				/**< @ingroup BasicOps */
char fractional(char /*iV*/)	{ return 0; }				/**< @ingroup BasicOps */

void inpfloor(char& /*ioV*/)	{}							/**< @ingroup BasicOps */
void inpceil(char& /*ioV*/)		{}							/**< @ingroup BasicOps */
void inpround(char& /*ioV*/)	{}							/**< @ingroup BasicOps */
void inpfractional(char& ioV)	{ ioV = 0; }				/**< @ingroup BasicOps */

// --- signed char ----------------------------------------------------------------------------------

signed char abs(signed char iV)							{ return static_cast<signed char>(::abs(iV)); }	/**< @ingroup BasicOps */
signed char pow(signed char iV, signed char iPow)		{ return impl::IntPow<char>::eval(iV, iPow); }	/**< @ingroup BasicOps */
signed char pow(signed char iV, unsigned char iPow)		{ return impl::IntPow<char>::eval(iV, iPow); }	/**< @ingroup BasicOps */
signed char floor(signed char iV)						{ return iV; }									/**< @ingroup BasicOps */
signed char ceil(signed char iV)						{ return iV; }									/**< @ingroup BasicOps */
signed char round(signed char iV)						{ return iV; }									/**< @ingroup BasicOps */
signed char fractional(signed char /*iV*/)				{ return 0; }									/**< @ingroup BasicOps */
signed char div(signed char iV, signed char iPow)		{ return impl::IntDiv<char>::eval(iV, iPow); }	/**< @ingroup BasicOps */
signed char div(signed char iV, unsigned char iPow)		{ return impl::IntDiv<char>::eval(iV, iPow); }	/**< @ingroup BasicOps */
signed char mod(signed char iV, signed char iPow)		{ return impl::IntMod<char>::eval(iV, iPow); }	/**< @ingroup BasicOps */
signed char mod(signed char iV, unsigned char iPow)		{ return impl::IntMod<char>::eval(iV, iPow); }	/**< @ingroup BasicOps */

void inpfloor(signed char& /*ioV*/)		{}				/**< @ingroup BasicOps */
void inpceil(signed char& /*ioV*/)		{}				/**< @ingroup BasicOps */
void inpround(signed char& /*ioV*/)		{}				/**< @ingroup BasicOps */
void inpfractional(signed char& ioV)	{ ioV = 0; }	/**< @ingroup BasicOps */



// --- unsigned char ----------------------------------------------------------------------------------

unsigned char abs(unsigned char iV)						{ return iV; }				/**< @ingroup BasicOps */
unsigned char sign(unsigned char iV)					{ return iV > 0 ? 1 : 0; }	/**< @ingroup BasicOps */
unsigned char pow(unsigned char iV, unsigned char iPow)	{ return impl::IntPow<unsigned char>::eval(iV, iPow); }	/**< @ingroup BasicOps */
unsigned char floor(unsigned char iV)					{ return iV; }				/**< @ingroup BasicOps */
unsigned char ceil(unsigned char iV)					{ return iV; }				/**< @ingroup BasicOps */
unsigned char round(unsigned char iV)					{ return iV; }				/**< @ingroup BasicOps */
unsigned char fractional(unsigned char /*iV*/)			{ return 0; }				/**< @ingroup BasicOps */
unsigned char div(unsigned char iV, unsigned char iPow)	{ return iV / iPow; }		/**< @ingroup BasicOps */
unsigned char mod(unsigned char iV, unsigned char iPow)	{ return iV % iPow; }		/**< @ingroup BasicOps */

void inpabs(unsigned char& /*ioV*/)		{}				/**< @ingroup BasicOps */
void inpfloor(unsigned char& /*ioV*/)	{}				/**< @ingroup BasicOps */
void inpceil(unsigned char& /*ioV*/)	{}				/**< @ingroup BasicOps */
void inpround(unsigned char& /*ioV*/)	{}				/**< @ingroup BasicOps */
void inpfractional(unsigned char& ioV)	{ ioV = 0; }	/**< @ingroup BasicOps */



// --- signed short ----------------------------------------------------------------------------------

signed short abs(signed short iV)						{ return static_cast<signed short>(::abs(iV)); }	/**< @ingroup BasicOps */
signed short pow(signed short iV, signed short iPow)	{ return impl::IntPow<short>::eval(iV, iPow); }		/**< @ingroup BasicOps */
signed short pow(signed short iV, unsigned short iPow)	{ return impl::IntPow<short>::eval(iV, iPow); }		/**< @ingroup BasicOps */
signed short floor(signed short iV)						{ return iV; }										/**< @ingroup BasicOps */
signed short ceil(signed short iV)						{ return iV; }										/**< @ingroup BasicOps */
signed short round(signed short iV)						{ return iV; }										/**< @ingroup BasicOps */
signed short fractional(signed short /*iV*/)			{ return 0; }										/**< @ingroup BasicOps */
signed short div(signed short iV, signed short iPow)	{ return impl::IntDiv<short>::eval(iV, iPow); }		/**< @ingroup BasicOps */
signed short div(signed short iV, unsigned short iPow)	{ return impl::IntDiv<short>::eval(iV, iPow); }		/**< @ingroup BasicOps */
signed short mod(signed short iV, signed short iPow)	{ return impl::IntMod<short>::eval(iV, iPow); }		/**< @ingroup BasicOps */
signed short mod(signed short iV, unsigned short iPow)	{ return impl::IntMod<short>::eval(iV, iPow); }		/**< @ingroup BasicOps */

void inpfloor(signed short& /*ioV*/)	{}				/**< @ingroup BasicOps */
void inpceil(signed short& /*ioV*/)		{}				/**< @ingroup BasicOps */
void inpround(signed short& /*ioV*/)	{}				/**< @ingroup BasicOps */
void inpfractional(signed short& ioV)	{ ioV = 0; }	/**< @ingroup BasicOps */



// --- unsigned short ----------------------------------------------------------------------------------

unsigned short abs(unsigned short iV)						{ return iV; }											/**< @ingroup BasicOps */
unsigned short sign(unsigned short iV)						{ return iV > 0 ? 1 : 0; }								/**< @ingroup BasicOps */
unsigned short pow(unsigned short iV, unsigned short iPow)	{ return impl::IntPow<unsigned short>::eval(iV, iPow); }	/**< @ingroup BasicOps */
unsigned short floor(unsigned short iV)						{ return iV; }											/**< @ingroup BasicOps */
unsigned short ceil(unsigned short iV)						{ return iV; }											/**< @ingroup BasicOps */
unsigned short round(unsigned short iV)						{ return iV; }											/**< @ingroup BasicOps */
unsigned short fractional(unsigned short /*iV*/)			{ return 0; }											/**< @ingroup BasicOps */
unsigned short div(unsigned short iV, unsigned short iPow)	{ return iV / iPow; }										/**< @ingroup BasicOps */
unsigned short mod(unsigned short iV, unsigned short iPow)	{ return iV % iPow; }										/**< @ingroup BasicOps */

void inpabs(unsigned short& /*ioV*/)	{}				/**< @ingroup BasicOps */
void inpfloor(unsigned short& /*ioV*/)	{}				/**< @ingroup BasicOps */
void inpceil(unsigned short& /*ioV*/)	{}				/**< @ingroup BasicOps */
void inpround(unsigned short& /*ioV*/)	{}				/**< @ingroup BasicOps */
void inpfractional(unsigned short& ioV)	{ ioV = 0; }	/**< @ingroup BasicOps */



// --- signed int ----------------------------------------------------------------------------------

signed int abs(signed int iV)						{ return ::abs(iV); }						/**< @ingroup BasicOps */
signed int pow(signed int iV, signed int iPow)		{ return impl::IntPow<int>::eval(iV, iPow); }	/**< @ingroup BasicOps */
signed int pow(signed int iV, unsigned int iPow)	{ return impl::IntPow<int>::eval(iV, iPow); }	/**< @ingroup BasicOps */
signed int floor(signed int iV)						{ return iV; }								/**< @ingroup BasicOps */
signed int ceil(signed int iV)						{ return iV; }								/**< @ingroup BasicOps */
signed int round(signed int iV)						{ return iV; }								/**< @ingroup BasicOps */
signed int fractional(signed int /*iV*/)			{ return 0; }								/**< @ingroup BasicOps */
signed int div(signed int iV, signed int iPow)		{ return impl::IntDiv<int>::eval(iV, iPow); }	/**< @ingroup BasicOps */
signed int div(signed int iV, unsigned int iPow)	{ return impl::IntDiv<int>::eval(iV, iPow); }	/**< @ingroup BasicOps */
signed int mod(signed int iV, signed int iPow)		{ return impl::IntMod<int>::eval(iV, iPow); }	/**< @ingroup BasicOps */
signed int mod(signed int iV, unsigned int iPow)	{ return impl::IntMod<int>::eval(iV, iPow); }	/**< @ingroup BasicOps */

void inpfloor(signed int& /*ioV*/)	{}				/**< @ingroup BasicOps */
void inpceil(signed int& /*ioV*/)	{}				/**< @ingroup BasicOps */
void inpround(signed int& /*ioV*/)	{}				/**< @ingroup BasicOps */
void inpfractional(signed int& ioV)	{ ioV = 0; }	/**< @ingroup BasicOps */



// --- unsigned int ----------------------------------------------------------------------------------

unsigned int abs(unsigned int iV)						{ return iV; }											/**< @ingroup BasicOps */
unsigned int sign(unsigned int iV)						{ return iV > 0 ? 1 : 0; }								/**< @ingroup BasicOps */
unsigned int pow(unsigned int iV, unsigned int iPow)	{ return impl::IntPow<unsigned int>::eval(iV, iPow); }	/**< @ingroup BasicOps */
unsigned int floor(unsigned int iV)						{ return iV; }											/**< @ingroup BasicOps */
unsigned int ceil(unsigned int iV)						{ return iV; }											/**< @ingroup BasicOps */
unsigned int round(unsigned int iV)						{ return iV; }											/**< @ingroup BasicOps */
unsigned int fractional(unsigned int /*iV*/)			{ return 0; }											/**< @ingroup BasicOps */
unsigned int div(unsigned int iV, unsigned int iPow)	{ return iV / iPow; }										/**< @ingroup BasicOps */
unsigned int mod(unsigned int iV, unsigned int iPow)	{ return iV % iPow; }										/**< @ingroup BasicOps */

void inpabs(unsigned int& /*ioV*/)		{}	/**< @ingroup BasicOps */
void inpfloor(unsigned int& /*ioV*/)	{}	/**< @ingroup BasicOps */
void inpceil(unsigned int& /*ioV*/)		{}	/**< @ingroup BasicOps */
void inpround(unsigned int& /*ioV*/)	{}	/**< @ingroup BasicOps */
void inpfractional(unsigned int& ioV)	{ ioV = 0; }	/**< @ingroup BasicOps */



// --- signed long ----------------------------------------------------------------------------------

signed long abs(signed long iV)							{ return ::labs(iV); }							/**< @ingroup BasicOps */
signed long pow(signed long iV, signed long iPow)		{ return impl::IntPow<long>::eval(iV, iPow); }	/**< @ingroup BasicOps */
signed long pow(signed long iV, unsigned long iPow)		{ return impl::IntPow<long>::eval(iV, iPow); }	/**< @ingroup BasicOps */
signed long floor(signed long iV)						{ return iV; }									/**< @ingroup BasicOps */
signed long ceil(signed long iV)						{ return iV; }									/**< @ingroup BasicOps */
signed long round(signed long iV)						{ return iV; }									/**< @ingroup BasicOps */
signed long fractional(signed long /*iV*/)				{ return 0; }									/**< @ingroup BasicOps */
signed long div(signed long iV, signed long iPow)		{ return impl::IntDiv<long>::eval(iV, iPow); }	/**< @ingroup BasicOps */
signed long div(signed long iV, unsigned long iPow)		{ return impl::IntDiv<long>::eval(iV, iPow); }	/**< @ingroup BasicOps */
signed long mod(signed long iV, signed long iPow)		{ return impl::IntMod<long>::eval(iV, iPow); }	/**< @ingroup BasicOps */
signed long mod(signed long iV, unsigned long iPow)		{ return impl::IntMod<long>::eval(iV, iPow); }	/**< @ingroup BasicOps */

void inpfloor(signed long& /*ioV*/)		{}				/**< @ingroup BasicOps */
void inpceil(signed long& /*ioV*/)		{}				/**< @ingroup BasicOps */
void inpround(signed long& /*ioV*/)		{}				/**< @ingroup BasicOps */
void inpfractional(signed long& ioV)	{ ioV = 0; }	/**< @ingroup BasicOps */



// --- unsigned long ----------------------------------------------------------------------------------

unsigned long abs(unsigned long iV)						{ return iV; }											/**< @ingroup BasicOps */
unsigned long sign(unsigned long iV)					{ return iV > 0 ? 1 : 0; }								/**< @ingroup BasicOps */
unsigned long pow(unsigned long iV, unsigned long iPow)	{ return impl::IntPow<unsigned long>::eval(iV, iPow); }	/**< @ingroup BasicOps */
unsigned long floor(unsigned long iV)					{ return iV; }											/**< @ingroup BasicOps */
unsigned long ceil(unsigned long iV)					{ return iV; }											/**< @ingroup BasicOps */
unsigned long round(unsigned long iV)					{ return iV; }											/**< @ingroup BasicOps */
unsigned long fractional(unsigned long /*iV*/)				{ return 0; }											/**< @ingroup BasicOps */
unsigned long div(unsigned long iV, unsigned long iPow)	{ return iV / iPow; }										/**< @ingroup BasicOps */
unsigned long mod(unsigned long iV, unsigned long iPow)	{ return iV % iPow; }										/**< @ingroup BasicOps */

void inpabs(unsigned long& /*ioV*/)		{}				/**< @ingroup BasicOps */
void inpfloor(unsigned long& /*ioV*/)	{}				/**< @ingroup BasicOps */
void inpceil(unsigned long& /*ioV*/)	{}				/**< @ingroup BasicOps */
void inpround(unsigned long& /*ioV*/)	{}				/**< @ingroup BasicOps */
void inpfractional(unsigned long& ioV)	{ ioV = 0; }	/**< @ingroup BasicOps */




// --- complex numbers -----------------------------------------------------------------------------

template <typename T> std::complex<T> abs( const std::complex<T>& iV) { return std::abs( iV ); }
template <typename T> std::complex<T> inv( const std::complex<T>& iV) { return T(1)/iV; }
template <typename T> std::complex<T> sqrt( const std::complex<T>& iV){ return std::sqrt( iV ); }
template <typename T> std::complex<T> pow(const std::complex<T>& iX, double iY) { return std::pow(iX, iY); }
template <typename T> std::complex<T> exp( const std::complex<T>& iV) { return std::exp( iV ); }
template <typename T> std::complex<T> log( const std::complex<T>& iV) { return std::log( iV ); }
template <typename T> std::complex<T> log2( const std::complex<T>& iV) { return T(LASS_NUM_INVLOG2) * std::log( iV ); }
template <typename T> std::complex<T> log10( const std::complex<T>& iV) { return T(LASS_NUM_INVLOG10) * std::log( iV ); }
template <typename T> std::complex<T> cos( const std::complex<T>& iV) { return std::cos( iV ); }
template <typename T> std::complex<T> sin( const std::complex<T>& iV) { return std::sin( iV ); }
template <typename T> std::complex<T> tan( const std::complex<T>& iV) { return std::tan( iV ); }
template <typename T> std::complex<T> acos( const std::complex<T>& iV){ return std::acos( iV ); }
template <typename T> std::complex<T> asin( const std::complex<T>& iV){ return std::asin( iV ); }
template <typename T> std::complex<T> atan( const std::complex<T>& iV){ return std::atan( iV ); }

template <typename T> T norm(const std::complex<T>& iV) { return std::norm( iV ); }
template <typename T> std::complex<T> conj(const std::complex<T>& iV) { return std::conj(iV); }
