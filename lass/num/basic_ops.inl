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

	static T eval(TSigned x, TSigned p)
	{		
		LASS_ASSERT(p >= 0);
		return eval(x, static_cast<TUnsigned>(p));
	}

	static T eval(TSigned x, TUnsigned p)
	{
		if (p == 0)
		{
			return 1;
		}

		TSigned result = 0;
		TSigned partialPower = 1;
		do
		{
			partialPower *= x;
			if (p & 0x1)
			{
				result += partialPower;
			}
			p >>= 1;
		}
		while (p);
		return result;
	}
};

template <typename T> 
struct IntDiv
{
	typedef typename NumTraits<T>::signedType TSigned;
	typedef typename NumTraits<T>::unsignedType TUnsigned;

	static T eval(TSigned x, TSigned m)
	{
		LASS_ASSERT(m > 0);
		const TSigned tempDiv = x / m;
		return x % m >= 0 ? tempDiv : (tempDiv - 1);
	}

	static T eval(TSigned x, TUnsigned m)
	{
		LASS_ASSERT(m > 0);
		const TSigned signedMod = static_cast<TSigned>(m);
		LASS_ASSERT(signedMod >= 0);
		const TSigned tempDiv = x / signedMod;
		return x % signedMod >= 0 ? tempDiv : (tempDiv - 1);
	}
};

template <typename T> 
struct IntMod
{
	typedef typename NumTraits<T>::signedType TSigned;
	typedef typename NumTraits<T>::unsignedType TUnsigned;

	static T eval(TSigned x, TSigned m)
	{
		LASS_ASSERT(m > 0);
		const TSigned tempMod = x % m;
		return tempMod >= 0 ? tempMod : (tempMod + m);
	}

	static T eval(TSigned x, TUnsigned m)
	{
		LASS_ASSERT(m > 0);
		const TSigned signedMod = static_cast<TSigned>(m);
		LASS_ASSERT(signedMod >= 0);
		const TSigned tempMod = x % signedMod;
		return tempMod >= 0 ? tempMod : (tempMod + signedMod);
	}
};

}

// --- generic functions ---------------------------------------------------------------------------

/** if x < 0 return -x, else return x.
 *  @ingroup BasicOps
 */
template <typename T> inline T abs(const T& x)
{
	return x < T() ? -x : x;
}

/** if x < 0 return -1, else if x > 0 return 1, else return 0.
 *  @ingroup BasicOps
 */
template <typename T> inline T sign(const T& x)
{
	const T zero = T();
	return x > zero ? T(1) : (x < zero ? T(-1) : zero);
}

/** return x * x
 *  @ingroup BasicOps
 */
template <typename T> inline T sqr(const T& x) 
{ 
	return x * x; 
}

/** return x * x * x
 *  @ingroup BasicOps
 */
template <typename T> inline T cubic(const T& x) 
{ 
	return x * x * x; 
}

/** return x ^ -1
 *  @ingroup BasicOps
 */
template <typename T> inline T inv(const T& x) 
{ 
	return T(1) / x;
}

/** return exp(p * log(x));
 *  @ingroup BasicOps
 *  @warning GENERIC FUNCTION USES EXP AND LOG!
 */
template <typename T> inline T pow(const T& x, const T& p) 
{ 
	return exp(p * log(x));
}

/** return log(x) / log(2)
 *  @ingroup BasicOps
 */
template <typename T> inline T log2(const T& x, const T& p) 
{ 
	return T(LASS_NUM_INVLOG2) * log(x);
}

/** return log(x) / log(10)
 *  @ingroup BasicOps
 */
template <typename T> inline T log10(const T& x, const T& p) 
{ 
	return T(LASS_NUM_INVLOG10) * log(x);
}

/** return norm of x as if x is real part of complex number: sqr(x)
 *  @ingroup BasicOps
 */
template <typename T> inline T norm(const T& x)
{
	return sqr(x);
}

/** return conjugate as if x is a complex number: x
 */
template <typename T> inline T conj(const T& x)
{
	return x;
}

/** if x < min return min, else if x > max return max, else return x.
 *  @ingroup BasicOps
 */
template <typename T> inline const T& clamp(const T& x, const T& min, const T& max)
{
	return x < min ? min : (x > max ? max : x);
}

/** linear interpolation between @a a and @a b
 *  @ingroup BasicOps
 */
template <typename T> inline T lerp(const T& a, const T& b, const T& f)
{
	return a + f * (b - a);
}

/** @ingroup BasicOps
 */
template<typename T,typename f> T applyFunction(const T& x, f func ) 
{ 
	return func(x); 
}



// --- generic inplace rerouters -------------------------------------------------------------------

template <typename T> void inpabs(T& x)					{ x = num::abs(x); }		/**< @ingroup BasicOps */
template <typename T> void inpsign(T& x)				{ x = num::sign(x); }		/**< @ingroup BasicOps */
template <typename T> void inpinv(T& x)					{ x = num::inv(x); }		/**< @ingroup BasicOps */
template <typename T> void inpsqrt(T& x)				{ x = num::sqrt(x); }		/**< @ingroup BasicOps */
template <typename T> void inpsqr(T& x)					{ x = num::sqr(x); }		/**< @ingroup BasicOps */
template <typename T> void inpcubic(T& x)				{ x = num::cubic(x); }		/**< @ingroup BasicOps */
template <typename T> void inppow(T& x, const T& p)		{ x = num::pow(x, p); }		/**< @ingroup BasicOps */
template <typename T> void inpexp(T& x)					{ x = num::exp(x); }		/**< @ingroup BasicOps */
template <typename T> void inplog(T& x)					{ x = num::log(x); }		/**< @ingroup BasicOps */
template <typename T> void inpcos(T& x)					{ x = num::cos(x); }		/**< @ingroup BasicOps */
template <typename T> void inpsin(T& x)					{ x = num::sin(x); }		/**< @ingroup BasicOps */
template <typename T> void inptan(T& x)					{ x = num::tan(x); }		/**< @ingroup BasicOps */
template <typename T> void inpacos(T& x)				{ x = num::acos(x); }		/**< @ingroup BasicOps */
template <typename T> void inpasin(T& x)				{ x = num::asin(x); }		/**< @ingroup BasicOps */
template <typename T> void inpatan(T& x)				{ x = num::atan(x); }		/**< @ingroup BasicOps */
template <typename T> void inpatan2(T& y, const T& x)	{ y = num::atan2(y, x); }	/**< @ingroup BasicOps */
template <typename T> void inpfloor(T& x)				{ x = num::floor(x); }		/**< @ingroup BasicOps */
template <typename T> void inpceil(T& x)				{ x = num::ceil(x); }		/**< @ingroup BasicOps */
template <typename T> void inpround(T& x)				{ x = num::round(x); }		/**< @ingroup BasicOps */
template <typename T> void inpfractional(T& x)			{ x -= num::floor(x); }		/**< @ingroup BasicOps */
template <typename T> void inpdiv(T& x, const T& m)		{ x = num::div(x, m); }		/**< @ingroup BasicOps */
template <typename T> void inpmod(T& x, const T& m)		{ x = num::mod(x, m); }		/**< @ingroup BasicOps */
template <typename T> void inpclamp(T& x, const T& min, const T& max)	{ x = num::clamp(x, min, max); }	/** @ingroup BasicOps */

template <typename T> void compnorm(const T& x, T& y)	{ y = num::norm(x); }		/**< @ingroup BasicOps */
template <typename T> void compinv(const T& x, T& y)	{ y = num::inv(x); }		/**< @ingroup BasicOps */



// --- float ---------------------------------------------------------------------------------------

float abs(float x)			{ return ::fabsf(x); }			/**< @ingroup BasicOps */
float inv(float x)			{ return 1.f / x; }				/**< @ingroup BasicOps */
float sqrt(float x)			{ LASS_ASSERT(!(x < 0.f)); return ::sqrtf(x); }	/**< @ingroup BasicOps */
float pow(float x, float p)	{ return ::powf(x, p); }		/**< @ingroup BasicOps */
float exp(float x)			{ return ::expf(x); }			/**< @ingroup BasicOps */
float log(float x)			{ return ::logf(x); }			/**< @ingroup BasicOps */
float log2(float x)			{ return float(LASS_NUM_INVLOG2) * ::logf(x); } /**< @ingroup BasicOps */
float log10(float x)		{ return ::log10f(x); }			/**< @ingroup BasicOps */
float cos(float x)			{ return ::cosf(x); }			/**< @ingroup BasicOps */
float sin(float x)			{ return ::sinf(x); }			/**< @ingroup BasicOps */
float tan(float x)			{ return ::tanf(x); }			/**< @ingroup BasicOps */
float acos(float x)			{ return ::acosf(x); }			/**< @ingroup BasicOps */
float asin(float x)			{ return ::asinf(x); }			/**< @ingroup BasicOps */
float atan(float x)			{ return ::atanf(x); }			/**< @ingroup BasicOps */
float atan2(float y, float x)	{ return ::atan2f(y, x); }	/**< @ingroup BasicOps */
float sinc(float x)			{ return ::fabsf(x) < 1e-4f ? 1.f : (::sinf(x) / x); }	/**< @ingroup BasicOps */
float floor(float x)		{ return ::floorf(x); }			/**< @ingroup BasicOps */
float ceil(float x)			{ return ::ceilf(x); }			/**< @ingroup BasicOps */
float round(float x)		{ return ::floorf(x + .5f); }	/**< @ingroup BasicOps */
float fractional(float x)	{ return x - ::floorf(x); }		/**< @ingroup BasicOps */

/** @ingroup BasicOps */
float mod(float x, float m) 
{ 
	const float result = ::fmodf(x, m);
	return result < 0.f ? result + m : result;
}	

/** @ingroup BasicOps */
float fastSin(float x)
{
	const float a = 1.2732395447351628f;
	const float b = -0.4052847345693511f;
	const float c = 0.2248391028f;
	const float y = a * x + b * x * fabsf(x);
	return c * (y * fabsf(y) - y) + y;
}


// --- double --------------------------------------------------------------------------------------

double abs(double x)			{ return ::fabs(x); }			/**< @ingroup BasicOps */
double inv(double x)			{ return 1. / x; }				/**< @ingroup BasicOps */
double sqrt(double x)			{ LASS_ASSERT(!(x < 0.)); return ::sqrt(x); }	/**< @ingroup BasicOps */
double pow(double x, double p)	{ return ::pow(x, p); }			/**< @ingroup BasicOps */
double exp(double x)			{ return ::exp(x); }			/**< @ingroup BasicOps */
double log(double x)			{ return ::log(x); }			/**< @ingroup BasicOps */
double log2(double x)			{ return double(LASS_NUM_INVLOG2) * ::log(x); } /**< @ingroup BasicOps */
double log10(double x)			{ return ::log10(x); }			/**< @ingroup BasicOps */
double cos(double x)			{ return ::cos(x); }			/**< @ingroup BasicOps */
double sin(double x)			{ return ::sin(x); }			/**< @ingroup BasicOps */
double tan(double x)			{ return ::tan(x); }			/**< @ingroup BasicOps */
double acos(double x)			{ return ::acos(x); }			/**< @ingroup BasicOps */
double asin(double x)			{ return ::asin(x); }			/**< @ingroup BasicOps */
double atan(double x)			{ return ::atan(x); }			/**< @ingroup BasicOps */
double atan2(double y, double x)	{ return ::atan2(y, x); }	/**< @ingroup BasicOps */
double sinc(double x)			{ return ::fabs(x) < 1e-8 ? 1. : (::sin(x) / x); }	/**< @ingroup BasicOps */
double floor(double x)			{ return ::floor(x); }			/**< @ingroup BasicOps */
double ceil(double x)			{ return ::ceil(x); }			/**< @ingroup BasicOps */
double round(double x)			{ return ::floor(x + .5); }		/**< @ingroup BasicOps */
double fractional(double x)		{ return x - ::floor(x); }		/**< @ingroup BasicOps */

/** @ingroup BasicOps */
double mod(double x, double m) 
{ 
	const double result = ::fmod(x, m);
	return result < 0. ? result + m : result;
}	

/** @ingroup BasicOps */
double fastSin(double x)
{
	const double a = 1.2732395447351628;
	const double b = -0.4052847345693511;
	const double c = 0.2248391028;
	const double y = a * x + b * x * fabs(x);
	return c * (y * fabs(y) - y) + y;
}



// --- long double ---------------------------------------------------------------------------------

#ifdef LASS_NUM_BASIC_OPS_USE_BUILTIN_LONG_DOUBLE

long double abs(long double x)					{ return __builtin_fabsl(x); }		/**< @ingroup BasicOps */
long double inv(long double x)					{ return 1. / x; }					/**< @ingroup BasicOps */
long double sqrt(long double x)					{ LASS_ASSERT(!(x < 0.)); return __builtin_sqrtl(x); }	/**< @ingroup BasicOps */
long double pow(long double x, long double p)	{ return __builtin_powl(x, p); }	/**< @ingroup BasicOps */
long double exp(long double x)					{ return __builtin_expl(x); }		/**< @ingroup BasicOps */
long double log(long double x)					{ return __builtin_logl(x); }		/**< @ingroup BasicOps */
long double log2(long double x)					{ return (long double)(LASS_NUM_INVLOG2) * __builtin_logl(x); } /**< @ingroup BasicOps */
long double log10(long double x)				{ return __builtin_log10l(x); }		/**< @ingroup BasicOps */
long double cos(long double x)					{ return __builtin_cosl(x); }		/**< @ingroup BasicOps */
long double sin(long double x)					{ return __builtin_sinl(x); }		/**< @ingroup BasicOps */
long double tan(long double x)					{ return __builtin_tanl(x); }		/**< @ingroup BasicOps */
long double acos(long double x)					{ return __builtin_acosl(x); }		/**< @ingroup BasicOps */
long double asin(long double x)					{ return __builtin_asinl(x); }		/**< @ingroup BasicOps */
long double atan(long double x)					{ return __builtin_atanl(x); }		/**< @ingroup BasicOps */
long double atan2(long double y, long double x)	{ return __builtin_atan2l(y, x); }	/**< @ingroup BasicOps */
long double sinc(long double x)					{ return __builtin_fabsl(x) < 1e-10 ? 1. : (__builtin_sinl(x) / x); }	/**< @ingroup BasicOps */
long double floor(long double x)				{ return __builtin_floorl(x); }		/**< @ingroup BasicOps */
long double ceil(long double x)					{ return __builtin_ceill(x); }		/**< @ingroup BasicOps */
long double round(long double x)				{ return __builtin_floorl(x + .5); }	/**< @ingroup BasicOps */
long double fractional(long double x)			{ return x - __builtin_floorl(x); }	/**< @ingroup BasicOps */

/** @ingroup BasicOps */
long double mod(long double x, long double m) 
{ 
	const long double result = __builtin_fmodl(x, m);
	return result < 0. ? result + m : result;
}	

#else

long double abs(long double x)					{ return ::fabsl(x); }			/**< @ingroup BasicOps */
long double inv(long double x)					{ return 1. / x; }				/**< @ingroup BasicOps */
long double sqrt(long double x)					{ LASS_ASSERT(!(x < 0.)); return ::sqrtl(x); }	/**< @ingroup BasicOps */
long double pow(long double x, long double p)	{ return ::powl(x, p); }		/**< @ingroup BasicOps */
long double exp(long double x)					{ return ::expl(x); }			/**< @ingroup BasicOps */
long double log(long double x)					{ return ::logl(x); }			/**< @ingroup BasicOps */
long double log2(long double x)					{ return (long double)(LASS_NUM_INVLOG2) * ::logl(x); } /**< @ingroup BasicOps */
long double log10(long double x)				{ return ::log10l(x); }			/**< @ingroup BasicOps */
long double cos(long double x)					{ return ::cosl(x); }			/**< @ingroup BasicOps */
long double sin(long double x)					{ return ::sinl(x); }			/**< @ingroup BasicOps */
long double tan(long double x)					{ return ::tanl(x); }			/**< @ingroup BasicOps */
long double acos(long double x)					{ return ::acosl(x); }			/**< @ingroup BasicOps */
long double asin(long double x)					{ return ::asinl(x); }			/**< @ingroup BasicOps */
long double atan(long double x)					{ return ::atanl(x); }			/**< @ingroup BasicOps */
long double atan2(long double y, long double x)	{ return ::atan2l(y, x); }		/**< @ingroup BasicOps */
long double sinc(long double x)					{ return ::fabsl(x) < 1e-10 ? 1. : (::sinl(x) / x); }	/**< @ingroup BasicOps */
long double floor(long double x)				{ return ::floorl(x); }			/**< @ingroup BasicOps */
long double ceil(long double x)					{ return ::ceill(x); }			/**< @ingroup BasicOps */
long double round(long double x)				{ return ::floorl(x + .5); }	/**< @ingroup BasicOps */
long double fractional(long double x)			{ return x - ::floorl(x); }		/**< @ingroup BasicOps */

/** @ingroup BasicOps */
long double mod(long double x, long double m) 
{ 
	const long double result = ::fmodl(x, m);
	return result < 0. ? result + m : result;
}	

/** @ingroup BasicOps */
long double fastSin(long double x)
{
	const long double a = 1.2732395447351628;
	const long double b = -0.4052847345693511;
	const long double c = 0.2248391028;
	const long double y = a * x + b * x * fabs(x);
	return c * (y * fabs(y) - y) + y;
}

#endif


// --- char ----------------------------------------------------------------------------------------

#ifdef LASS_CHAR_IS_SIGNED

char pow(char x, char p)			{ return impl::IntPow<char>::eval(x, static_cast<signed char>(p)); }	/**< @ingroup BasicOps */
char pow(char x, unsigned char p)	{ return impl::IntPow<char>::eval(x, p); }	/**< @ingroup BasicOps */
char div(char x, char m)			{ return impl::IntDiv<char>::eval(x, static_cast<signed char>(m)); }	/**< @ingroup BasicOps */
char div(char x, unsigned char m)	{ return impl::IntDiv<char>::eval(x, m); }	/**< @ingroup BasicOps */
char mod(char x, char m)			{ return impl::IntMod<char>::eval(x, static_cast<signed char>(m)); }	/**< @ingroup BasicOps */
char mod(char x, unsigned char m)	{ return impl::IntMod<char>::eval(x, m); }	/**< @ingroup BasicOps */

#else

char abs(char x)			{ return x; }				/**< @ingroup BasicOps */
char sign(char x)			{ return x > 0 ? 1 : 0; }	/**< @ingroup BasicOps */
char pow(char x, char p)	{ return impl::IntPow<char>::eval(x, static_cast<unsigned char>(p)); }	/**< @ingroup BasicOps */
char div(char x, char p)	{ return x / p; }			/**< @ingroup BasicOps */
char mod(char x, char p)	{ return x % p; }			/**< @ingroup BasicOps */

#endif

char floor(char x)			{ return x; }				/**< @ingroup BasicOps */
char ceil(char x)			{ return x; }				/**< @ingroup BasicOps */
char round(char x)			{ return x; }				/**< @ingroup BasicOps */
char fractional(char /*x*/)	{ return 0; }				/**< @ingroup BasicOps */

void inpfloor(char& /*x*/)	{}							/**< @ingroup BasicOps */
void inpceil(char& /*x*/)	{}							/**< @ingroup BasicOps */
void inpround(char& /*x*/)	{}							/**< @ingroup BasicOps */
void inpfractional(char& x)	{ x = 0; }					/**< @ingroup BasicOps */

// --- signed char ----------------------------------------------------------------------------------

signed char abs(signed char x)						{ return static_cast<signed char>(::abs(x)); }	/**< @ingroup BasicOps */
signed char pow(signed char x, signed char p)		{ return impl::IntPow<char>::eval(x, p); }		/**< @ingroup BasicOps */
signed char pow(signed char x, unsigned char p)		{ return impl::IntPow<char>::eval(x, p); }		/**< @ingroup BasicOps */
signed char floor(signed char x)					{ return x; }									/**< @ingroup BasicOps */
signed char ceil(signed char x)						{ return x; }									/**< @ingroup BasicOps */
signed char round(signed char x)					{ return x; }									/**< @ingroup BasicOps */
signed char fractional(signed char /*x*/)			{ return 0; }									/**< @ingroup BasicOps */
signed char div(signed char x, signed char p)		{ return impl::IntDiv<char>::eval(x, p); }		/**< @ingroup BasicOps */
signed char div(signed char x, unsigned char p)		{ return impl::IntDiv<char>::eval(x, p); }		/**< @ingroup BasicOps */
signed char mod(signed char x, signed char p)		{ return impl::IntMod<char>::eval(x, p); }		/**< @ingroup BasicOps */
signed char mod(signed char x, unsigned char p)		{ return impl::IntMod<char>::eval(x, p); }		/**< @ingroup BasicOps */

void inpfloor(signed char& /*x*/)	{}				/**< @ingroup BasicOps */
void inpceil(signed char& /*x*/)	{}				/**< @ingroup BasicOps */
void inpround(signed char& /*x*/)	{}				/**< @ingroup BasicOps */
void inpfractional(signed char& x)	{ x = 0; }		/**< @ingroup BasicOps */



// --- unsigned char ----------------------------------------------------------------------------------

unsigned char abs(unsigned char x)					{ return x; }				/**< @ingroup BasicOps */
unsigned char sign(unsigned char x)					{ return x > 0 ? 1 : 0; }	/**< @ingroup BasicOps */
unsigned char pow(unsigned char x, unsigned char p)	{ return impl::IntPow<unsigned char>::eval(x, p); }	/**< @ingroup BasicOps */
unsigned char floor(unsigned char x)				{ return x; }				/**< @ingroup BasicOps */
unsigned char ceil(unsigned char x)					{ return x; }				/**< @ingroup BasicOps */
unsigned char round(unsigned char x)				{ return x; }				/**< @ingroup BasicOps */
unsigned char fractional(unsigned char /*x*/)		{ return 0; }				/**< @ingroup BasicOps */
unsigned char div(unsigned char x, unsigned char p)	{ return x / p; }			/**< @ingroup BasicOps */
unsigned char mod(unsigned char x, unsigned char p)	{ return x % p; }			/**< @ingroup BasicOps */

void inpabs(unsigned char& /*x*/)		{}			/**< @ingroup BasicOps */
void inpfloor(unsigned char& /*x*/)		{}			/**< @ingroup BasicOps */
void inpceil(unsigned char& /*x*/)		{}			/**< @ingroup BasicOps */
void inpround(unsigned char& /*x*/)		{}			/**< @ingroup BasicOps */
void inpfractional(unsigned char& x)	{ x = 0; }	/**< @ingroup BasicOps */



// --- signed short ----------------------------------------------------------------------------------

signed short abs(signed short x)					{ return static_cast<signed short>(::abs(x)); }	/**< @ingroup BasicOps */
signed short pow(signed short x, signed short p)	{ return impl::IntPow<short>::eval(x, p); }		/**< @ingroup BasicOps */
signed short pow(signed short x, unsigned short p)	{ return impl::IntPow<short>::eval(x, p); }		/**< @ingroup BasicOps */
signed short floor(signed short x)					{ return x; }									/**< @ingroup BasicOps */
signed short ceil(signed short x)					{ return x; }									/**< @ingroup BasicOps */
signed short round(signed short x)					{ return x; }									/**< @ingroup BasicOps */
signed short fractional(signed short /*x*/)			{ return 0; }									/**< @ingroup BasicOps */
signed short div(signed short x, signed short p)	{ return impl::IntDiv<short>::eval(x, p); }		/**< @ingroup BasicOps */
signed short div(signed short x, unsigned short p)	{ return impl::IntDiv<short>::eval(x, p); }		/**< @ingroup BasicOps */
signed short mod(signed short x, signed short p)	{ return impl::IntMod<short>::eval(x, p); }		/**< @ingroup BasicOps */
signed short mod(signed short x, unsigned short p)	{ return impl::IntMod<short>::eval(x, p); }		/**< @ingroup BasicOps */

void inpfloor(signed short& /*x*/)	{}			/**< @ingroup BasicOps */
void inpceil(signed short& /*x*/)	{}			/**< @ingroup BasicOps */
void inpround(signed short& /*x*/)	{}			/**< @ingroup BasicOps */
void inpfractional(signed short& x)	{ x = 0; }	/**< @ingroup BasicOps */



// --- unsigned short ----------------------------------------------------------------------------------

unsigned short abs(unsigned short x)					{ return x; }											/**< @ingroup BasicOps */
unsigned short sign(unsigned short x)					{ return x > 0 ? 1 : 0; }								/**< @ingroup BasicOps */
unsigned short pow(unsigned short x, unsigned short p)	{ return impl::IntPow<unsigned short>::eval(x, p); }	/**< @ingroup BasicOps */
unsigned short floor(unsigned short x)					{ return x; }											/**< @ingroup BasicOps */
unsigned short ceil(unsigned short x)					{ return x; }											/**< @ingroup BasicOps */
unsigned short round(unsigned short x)					{ return x; }											/**< @ingroup BasicOps */
unsigned short fractional(unsigned short /*x*/)			{ return 0; }											/**< @ingroup BasicOps */
unsigned short div(unsigned short x, unsigned short p)	{ return x / p; }										/**< @ingroup BasicOps */
unsigned short mod(unsigned short x, unsigned short p)	{ return x % p; }										/**< @ingroup BasicOps */

void inpabs(unsigned short& /*x*/)		{}			/**< @ingroup BasicOps */
void inpfloor(unsigned short& /*x*/)	{}			/**< @ingroup BasicOps */
void inpceil(unsigned short& /*x*/)		{}			/**< @ingroup BasicOps */
void inpround(unsigned short& /*x*/)	{}			/**< @ingroup BasicOps */
void inpfractional(unsigned short& x)	{ x = 0; }	/**< @ingroup BasicOps */



// --- signed int ----------------------------------------------------------------------------------

signed int abs(signed int x)					{ return ::abs(x); }						/**< @ingroup BasicOps */
signed int pow(signed int x, signed int p)		{ return impl::IntPow<int>::eval(x, p); }	/**< @ingroup BasicOps */
signed int pow(signed int x, unsigned int p)	{ return impl::IntPow<int>::eval(x, p); }	/**< @ingroup BasicOps */
signed int floor(signed int x)					{ return x; }								/**< @ingroup BasicOps */
signed int ceil(signed int x)					{ return x; }								/**< @ingroup BasicOps */
signed int round(signed int x)					{ return x; }								/**< @ingroup BasicOps */
signed int fractional(signed int /*x*/)			{ return 0; }								/**< @ingroup BasicOps */
signed int div(signed int x, signed int p)		{ return impl::IntDiv<int>::eval(x, p); }	/**< @ingroup BasicOps */
signed int div(signed int x, unsigned int p)	{ return impl::IntDiv<int>::eval(x, p); }	/**< @ingroup BasicOps */
signed int mod(signed int x, signed int p)		{ return impl::IntMod<int>::eval(x, p); }	/**< @ingroup BasicOps */
signed int mod(signed int x, unsigned int p)	{ return impl::IntMod<int>::eval(x, p); }	/**< @ingroup BasicOps */

void inpfloor(signed int& /*x*/)	{}			/**< @ingroup BasicOps */
void inpceil(signed int& /*x*/)		{}			/**< @ingroup BasicOps */
void inpround(signed int& /*x*/)	{}			/**< @ingroup BasicOps */
void inpfractional(signed int& x)	{ x = 0; }	/**< @ingroup BasicOps */



// --- unsigned int ----------------------------------------------------------------------------------

unsigned int abs(unsigned int x)					{ return x; }										/**< @ingroup BasicOps */
unsigned int sign(unsigned int x)					{ return x > 0 ? 1 : 0; }							/**< @ingroup BasicOps */
unsigned int pow(unsigned int x, unsigned int p)	{ return impl::IntPow<unsigned int>::eval(x, p); }	/**< @ingroup BasicOps */
unsigned int floor(unsigned int x)					{ return x; }										/**< @ingroup BasicOps */
unsigned int ceil(unsigned int x)					{ return x; }										/**< @ingroup BasicOps */
unsigned int round(unsigned int x)					{ return x; }										/**< @ingroup BasicOps */
unsigned int fractional(unsigned int /*x*/)			{ return 0; }										/**< @ingroup BasicOps */
unsigned int div(unsigned int x, unsigned int p)	{ return x / p; }									/**< @ingroup BasicOps */
unsigned int mod(unsigned int x, unsigned int p)	{ return x % p; }									/**< @ingroup BasicOps */

void inpabs(unsigned int& /*x*/)	{}			/**< @ingroup BasicOps */
void inpfloor(unsigned int& /*x*/)	{}			/**< @ingroup BasicOps */
void inpceil(unsigned int& /*x*/)	{}			/**< @ingroup BasicOps */
void inpround(unsigned int& /*x*/)	{}			/**< @ingroup BasicOps */
void inpfractional(unsigned int& x)	{ x = 0; }	/**< @ingroup BasicOps */



// --- signed long ----------------------------------------------------------------------------------

signed long abs(signed long x)						{ return ::labs(x); }						/**< @ingroup BasicOps */
signed long pow(signed long x, signed long p)		{ return impl::IntPow<long>::eval(x, p); }	/**< @ingroup BasicOps */
signed long pow(signed long x, unsigned long p)		{ return impl::IntPow<long>::eval(x, p); }	/**< @ingroup BasicOps */
signed long floor(signed long x)					{ return x; }								/**< @ingroup BasicOps */
signed long ceil(signed long x)						{ return x; }								/**< @ingroup BasicOps */
signed long round(signed long x)					{ return x; }								/**< @ingroup BasicOps */
signed long fractional(signed long /*x*/)			{ return 0; }								/**< @ingroup BasicOps */
signed long div(signed long x, signed long p)		{ return impl::IntDiv<long>::eval(x, p); }	/**< @ingroup BasicOps */
signed long div(signed long x, unsigned long p)		{ return impl::IntDiv<long>::eval(x, p); }	/**< @ingroup BasicOps */
signed long mod(signed long x, signed long p)		{ return impl::IntMod<long>::eval(x, p); }	/**< @ingroup BasicOps */
signed long mod(signed long x, unsigned long p)		{ return impl::IntMod<long>::eval(x, p); }	/**< @ingroup BasicOps */

void inpfloor(signed long& /*x*/)	{}			/**< @ingroup BasicOps */
void inpceil(signed long& /*x*/)	{}			/**< @ingroup BasicOps */
void inpround(signed long& /*x*/)	{}			/**< @ingroup BasicOps */
void inpfractional(signed long& x)	{ x = 0; }	/**< @ingroup BasicOps */



// --- unsigned long ----------------------------------------------------------------------------------

unsigned long abs(unsigned long x)					{ return x; }	/**< @ingroup BasicOps */
unsigned long sign(unsigned long x)					{ return x > 0 ? 1 : 0; }	/**< @ingroup BasicOps */
unsigned long pow(unsigned long x, unsigned long p)	{ return impl::IntPow<unsigned long>::eval(x, p); }	/**< @ingroup BasicOps */
unsigned long floor(unsigned long x)				{ return x; }										/**< @ingroup BasicOps */
unsigned long ceil(unsigned long x)					{ return x; }										/**< @ingroup BasicOps */
unsigned long round(unsigned long x)				{ return x; }										/**< @ingroup BasicOps */
unsigned long fractional(unsigned long /*x*/)		{ return 0; }										/**< @ingroup BasicOps */
unsigned long div(unsigned long x, unsigned long p)	{ return x / p; }									/**< @ingroup BasicOps */
unsigned long mod(unsigned long x, unsigned long p)	{ return x % p; }									/**< @ingroup BasicOps */

void inpabs(unsigned long& /*x*/)		{}			/**< @ingroup BasicOps */
void inpfloor(unsigned long& /*x*/)		{}			/**< @ingroup BasicOps */
void inpceil(unsigned long& /*x*/)		{}			/**< @ingroup BasicOps */
void inpround(unsigned long& /*x*/)		{}			/**< @ingroup BasicOps */
void inpfractional(unsigned long& x)	{ x = 0; }	/**< @ingroup BasicOps */



#if LASS_HAVE_LONG_LONG

// --- signed long long ----------------------------------------------------------------------------------

/** @ingroup BasicOps */
signed long long abs(signed long long x)						
{
#if LASS_HAVE_LLABS
	return ::llabs(x);
#elif LASS_HAVE_ABS64
	return ::_abs64(x);
#else
	return abs<long long>(x); 
#endif
}						
signed long long pow(signed long long x, signed long long p)		{ return impl::IntPow<long long>::eval(x, p); }	/**< @ingroup BasicOps */
signed long long pow(signed long long x, unsigned long long p)		{ return impl::IntPow<long long>::eval(x, p); }	/**< @ingroup BasicOps */
signed long long floor(signed long long x)					{ return x; }								/**< @ingroup BasicOps */
signed long long ceil(signed long long x)						{ return x; }								/**< @ingroup BasicOps */
signed long long round(signed long long x)					{ return x; }								/**< @ingroup BasicOps */
signed long long fractional(signed long long /*x*/)			{ return 0; }								/**< @ingroup BasicOps */
signed long long div(signed long long x, signed long long p)		{ return impl::IntDiv<long long>::eval(x, p); }	/**< @ingroup BasicOps */
signed long long div(signed long long x, unsigned long long p)		{ return impl::IntDiv<long long>::eval(x, p); }	/**< @ingroup BasicOps */
signed long long mod(signed long long x, signed long long p)		{ return impl::IntMod<long long>::eval(x, p); }	/**< @ingroup BasicOps */
signed long long mod(signed long long x, unsigned long long p)		{ return impl::IntMod<long long>::eval(x, p); }	/**< @ingroup BasicOps */

void inpfloor(signed long long& /*x*/)	{}			/**< @ingroup BasicOps */
void inpceil(signed long long& /*x*/)	{}			/**< @ingroup BasicOps */
void inpround(signed long long& /*x*/)	{}			/**< @ingroup BasicOps */
void inpfractional(signed long long& x)	{ x = 0; }	/**< @ingroup BasicOps */



// --- unsigned long long ----------------------------------------------------------------------------------

unsigned long long abs(unsigned long long x)					{ return x; }	/**< @ingroup BasicOps */
unsigned long long sign(unsigned long long x)					{ return x > 0 ? 1 : 0; }	/**< @ingroup BasicOps */
unsigned long long pow(unsigned long long x, unsigned long long p)	{ return impl::IntPow<unsigned long long>::eval(x, p); }	/**< @ingroup BasicOps */
unsigned long long floor(unsigned long long x)				{ return x; }										/**< @ingroup BasicOps */
unsigned long long ceil(unsigned long long x)					{ return x; }										/**< @ingroup BasicOps */
unsigned long long round(unsigned long long x)				{ return x; }										/**< @ingroup BasicOps */
unsigned long long fractional(unsigned long long /*x*/)		{ return 0; }										/**< @ingroup BasicOps */
unsigned long long div(unsigned long long x, unsigned long long p)	{ return x / p; }									/**< @ingroup BasicOps */
unsigned long long mod(unsigned long long x, unsigned long long p)	{ return x % p; }									/**< @ingroup BasicOps */

void inpabs(unsigned long long& /*x*/)		{}			/**< @ingroup BasicOps */
void inpfloor(unsigned long long& /*x*/)		{}			/**< @ingroup BasicOps */
void inpceil(unsigned long long& /*x*/)		{}			/**< @ingroup BasicOps */
void inpround(unsigned long long& /*x*/)		{}			/**< @ingroup BasicOps */
void inpfractional(unsigned long long& x)	{ x = 0; }	/**< @ingroup BasicOps */

#endif




// --- complex numbers -----------------------------------------------------------------------------

template <typename T> std::complex<T> abs( const std::complex<T>& x) { return std::abs( x ); }	/**< @ingroup BasicOps */
template <typename T> std::complex<T> inv( const std::complex<T>& x) { return T(1)/x; }	/**< @ingroup BasicOps */
template <typename T> std::complex<T> sqrt( const std::complex<T>& x){ return std::sqrt( x ); }	/**< @ingroup BasicOps */
template <typename T> std::complex<T> pow(const std::complex<T>& x, double p) { return std::pow(x, p); }	/**< @ingroup BasicOps */
template <typename T> std::complex<T> exp( const std::complex<T>& x) { return std::exp( x ); }	/**< @ingroup BasicOps */
template <typename T> std::complex<T> log( const std::complex<T>& x) { return std::log( x ); }	/**< @ingroup BasicOps */
template <typename T> std::complex<T> log2( const std::complex<T>& x) { return T(LASS_NUM_INVLOG2) * std::log( x ); }	/**< @ingroup BasicOps */
template <typename T> std::complex<T> log10( const std::complex<T>& x) { return T(LASS_NUM_INVLOG10) * std::log( x ); }	/**< @ingroup BasicOps */
template <typename T> std::complex<T> cos( const std::complex<T>& x) { return std::cos( x ); }	/**< @ingroup BasicOps */
template <typename T> std::complex<T> sin( const std::complex<T>& x) { return std::sin( x ); }	/**< @ingroup BasicOps */
template <typename T> std::complex<T> tan( const std::complex<T>& x) { return std::tan( x ); }	/**< @ingroup BasicOps */
template <typename T> std::complex<T> acos( const std::complex<T>& x){ return std::acos( x ); }	/**< @ingroup BasicOps */
template <typename T> std::complex<T> asin( const std::complex<T>& x){ return std::asin( x ); }	/**< @ingroup BasicOps */
template <typename T> std::complex<T> atan( const std::complex<T>& x){ return std::atan( x ); }	/**< @ingroup BasicOps */

template <typename T> T norm(const std::complex<T>& x) { return std::norm( x ); }	/**< @ingroup BasicOps */
template <typename T> std::complex<T> conj(const std::complex<T>& x) { return std::conj(x); }	/**< @ingroup BasicOps */
