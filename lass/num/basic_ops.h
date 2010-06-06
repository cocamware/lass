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
 *	Copyright (C) 2004-2010 the Initial Developer.
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



/** @defgroup BasicOps
 *  Basic Operators
 */


#ifndef LASS_GUARDIAN_OF_INCLUSION_NUM_BASIC_OPS_H
#define LASS_GUARDIAN_OF_INCLUSION_NUM_BASIC_OPS_H

#include "num_common.h"
#include <cmath>

// The friendly folks of Cygwin have implemented log2 as a macro.
// Well, actually, they forgot to undef it in cmath.  Do it now!
//
#if LASS_PLATFORM_TYPE == LASS_PLATFORM_TYPE_CYGWIN && defined(log2)
#	undef log2
namespace std
{
    template <typename T> inline T log2(T x) { return std::log(x) / T(M_LN2); }
}
#endif

namespace lass
{
namespace num
{

// generic functions

template <typename T> inline T abs(const T& x);
template <typename T> inline T sign(const T& x);
template <typename T> inline T sqr(const T& x);
template <typename T> inline T inv(const T& x);
template <typename T> inline T cubic(const T& x);
template <typename T> inline T pow(const T& x, const T& p);
template <typename T> inline T log2(const T& x);
template <typename T> inline T log10(const T& x);
template <typename T> inline T norm(const T& x);
template <typename T> inline T conj(const T& x);
template <typename T> inline const T& clamp(const T& x, const T& min, const T& max);
template <typename T> inline T lerp(const T& a, const T& b, const T& f);

template <typename T> inline void inpabs(T& x);
template <typename T> inline void inpsign(T& x);
template <typename T> inline void inpinv(T& x);
template <typename T> inline void inpsqrt(T& x);
template <typename T> inline void inpsqr(T& x);
template <typename T> inline void inpcubic(T& x);
template <typename T> inline void inppow(T& x, const T& p);
template <typename T> inline void inpexp(T& x);
template <typename T> inline void inplog(T& x);
template <typename T> inline void inplog2(T& x);
template <typename T> inline void inplog10(T& x);
template <typename T> inline void inpcos(T& x);
template <typename T> inline void inpsin(T& x);
template <typename T> inline void inptan(T& x);
template <typename T> inline void inpacos(T& x);
template <typename T> inline void inpasin(T& x);
template <typename T> inline void inpatan(T& x);
template <typename T> inline void inpatan2(T& x, const T& y);
template <typename T> inline void inpfloor(T& x);
template <typename T> inline void inpceil(T& x);
template <typename T> inline void inpround(T& x);
template <typename T> inline void inpfractional(T& x);
template <typename T> inline void inpdiv(T& x, const T& d);
template <typename T> inline void inpmod(T& x, const T& d);
template <typename T> inline void inpclamp(T& x, const T& min, const T& max);
template <typename T> inline void compnorm(const T& x, T& y);
template <typename T> inline void compinv(const T& x, T& y);

template<typename T,typename f> inline T applyFunction(const T& x, f func );



// float

inline float abs(float x);
inline float inv(float x);
inline float sqrt(float x);
inline float pow(float x, float p);
inline float exp(float x);
inline float log(float x);
inline float log2(float x);
inline float log10(float x);
inline float cos(float x);
inline float sin(float x);
inline float tan(float x);
inline float acos(float x);
inline float asin(float x);
inline float atan(float x);
inline float atan2(float y, float x);
inline float sinc(float x);
inline float floor(float x);
inline float ceil(float x);
inline float round(float x);
inline float fractional(float x);
inline float div(float x, float d);
inline float mod(float x, float d);

inline float fastSin(float x);



// double

inline double abs(double x);
inline double inv(double x);
inline double sqrt(double x);
inline double pow(double x, double p);
inline double exp(double x);
inline double log(double x);
inline double log2(double x);
inline double log10(double x);
inline double cos(double x);
inline double sin(double x);
inline double tan(double x);
inline double acos(double x);
inline double asin(double x);
inline double atan(double x);
inline double atan2(double y, double x);
inline double sinc(double x);
inline double floor(double x);
inline double ceil(double x);
inline double round(double x);
inline double fractional(double x);
inline double div(double x, double d);
inline double mod(double x, double d);

inline double fastSin(double x);



// long double

inline long double abs(long double x);
inline long double inv(long double x);
inline long double sqrt(long double x);
inline long double pow(long double x, long double p);
inline long double exp(long double x);
inline long double log(long double x);
inline long double log2(long double x);
inline long double log10(long double x);
inline long double cos(long double x);
inline long double sin(long double x);
inline long double tan(long double x);
inline long double acos(long double x);
inline long double asin(long double x);
inline long double atan(long double x);
inline long double atan2(long double y, long double x);
inline long double sinc(long double x);
inline long double floor(long double x);
inline long double ceil(long double x);
inline long double round(long double x);
inline long double fractional(long double x);
inline long double div(long double x, long double d);
inline long double mod(long double x, long double d);

inline long double fastSin(long double x);



// --- char ----------------------------------------------------------------------------------------

inline char pow(char x, char p);
inline char floor(char x);
inline char ceil(char x);
inline char round(char x);
inline char fractional(char x);
inline char div(char x, char p);
inline char mod(char x, char p);

inline void inpabs(char& x);
inline void inpfloor(char& x);
inline void inpceil(char& x);
inline void inpround(char& x);
inline void inpfractional(char& x);

#ifdef LASS_CHAR_IS_SIGNED

inline char pow(char x, unsigned char p);
inline char div(char x, unsigned char d);
inline char mod(char x, unsigned char d);

#else

inline char abs(char x);
inline char sign(char x);

#endif



// signed char

inline signed char abs(signed char x);
inline signed char pow(signed char x, signed char p);
inline signed char pow(signed char x, unsigned char p);
inline signed char floor(signed char x);
inline signed char ceil(signed char x);
inline signed char round(signed char x);
inline signed char fractional(signed char x);
inline signed char div(signed char x, signed char d);
inline signed char div(signed char x, unsigned char d);
inline signed char mod(signed char x, signed char d);
inline signed char mod(signed char x, unsigned char d);

inline void inpfloor(signed char& x);
inline void inpceil(signed char& x);
inline void inpround(signed char& x);
inline void inpfractional(signed char& x);

// unsigned char

inline unsigned char abs(unsigned char x);
inline unsigned char sign(unsigned char x);
inline unsigned char pow(unsigned char x, unsigned char p);
inline unsigned char floor(unsigned char x);
inline unsigned char ceil(unsigned char x);
inline unsigned char round(unsigned char x);
inline unsigned char fractional(unsigned char x);
inline unsigned char div(unsigned char x, unsigned char d);
inline unsigned char mod(unsigned char x, unsigned char d);

inline void inpabs(unsigned char& x);
inline void inpfloor(unsigned char& x);
inline void inpceil(unsigned char& x);
inline void inpround(unsigned char& x);
inline void inpfractional(unsigned char& x);



// signed short

inline signed short abs(signed short x);
inline signed short pow(signed short x, signed short p);
inline signed short pow(signed short x, unsigned short p);
inline signed short floor(signed short x);
inline signed short ceil(signed short x);
inline signed short round(signed short x);
inline signed short fractional(signed short x);
inline signed short div(signed short x, signed short d);
inline signed short div(signed short x, unsigned short d);
inline signed short mod(signed short x, signed short d);
inline signed short mod(signed short x, unsigned short d);

inline void inpfloor(signed short& x);
inline void inpceil(signed short& x);
inline void inpround(signed short& x);
inline void inpfractional(signed short& x);

// unsigned short

inline unsigned short abs(unsigned short x);
inline unsigned short sign(unsigned short x);
inline unsigned short pow(unsigned short x, unsigned short p);
inline unsigned short floor(unsigned short x);
inline unsigned short ceil(unsigned short x);
inline unsigned short round(unsigned short x);
inline unsigned short fractional(unsigned short x);
inline unsigned short div(unsigned short x, unsigned short d);
inline unsigned short mod(unsigned short x, unsigned short d);

inline void inpabs(unsigned short& x);
inline void inpfloor(unsigned short& x);
inline void inpceil(unsigned short& x);
inline void inpround(unsigned short& x);
inline void inpfractional(unsigned short& x);



// signed int

inline signed int abs(signed int x);
inline signed int pow(signed int x, signed int p);
inline signed int pow(signed int x, unsigned int p);
inline signed int floor(signed int x);
inline signed int ceil(signed int x);
inline signed int round(signed int x);
inline signed int fractional(signed int x);
inline signed int div(signed int x, signed int d);
inline signed int div(signed int x, unsigned int d);
inline signed int mod(signed int x, signed int d);
inline signed int mod(signed int x, unsigned int d);

inline void inpfloor(signed int& x);
inline void inpceil(signed int& x);
inline void inpround(signed int& x);
inline void inpfractional(signed int& x);

// unsigned int

inline unsigned int abs(unsigned int x);
inline unsigned int sign(unsigned int x);
inline unsigned int pow(unsigned int x, unsigned int y);
inline unsigned int floor(unsigned int x);
inline unsigned int ceil(unsigned int x);
inline unsigned int round(unsigned int x);
inline unsigned int fractional(unsigned int x);
inline unsigned int div(unsigned int x, unsigned int d);
inline unsigned int mod(unsigned int x, unsigned int d);

inline void inpabs(unsigned int& x);
inline void inpfloor(unsigned int& x);
inline void inpceil(unsigned int& x);
inline void inpround(unsigned int& x);
inline void inpfractional(unsigned int& x);



// signed long

inline signed long abs(signed long x);
inline signed long pow(signed long x, signed long y);
inline signed long pow(signed long x, unsigned long y);
inline signed long floor(signed long x);
inline signed long ceil(signed long x);
inline signed long round(signed long x);
inline signed long fractional(signed long x);
inline signed long div(signed long x, signed long d);
inline signed long div(signed long x, unsigned long d);
inline signed long mod(signed long x, signed long d);
inline signed long mod(signed long x, unsigned long d);

inline void inpfloor(signed long& x);
inline void inpceil(signed long& x);
inline void inpround(signed long& x);
inline void inpfractional(signed long& x);

// unsigned long

inline unsigned long abs(unsigned long x);
inline unsigned long sign(unsigned long x);
inline unsigned long pow(unsigned long x, unsigned long p);
inline unsigned long floor(unsigned long x);
inline unsigned long ceil(unsigned long x);
inline unsigned long round(unsigned long x);
inline unsigned long fractional(unsigned long x);
inline unsigned long div(unsigned long x, unsigned long d);
inline unsigned long mod(unsigned long x, unsigned long d);

inline void inpabs(unsigned long& x);
inline void inpfloor(unsigned long& x);
inline void inpceil(unsigned long& x);
inline void inpround(unsigned long& x);
inline void inpfractional(unsigned long& x);



#if LASS_HAVE_LONG_LONG

// signed long long

inline signed long long abs(signed long long x);
inline signed long long pow(signed long long x, signed long long y);
inline signed long long pow(signed long long x, unsigned long long y);
inline signed long long floor(signed long long x);
inline signed long long ceil(signed long long x);
inline signed long long round(signed long long x);
inline signed long long fractional(signed long long x);
inline signed long long div(signed long long x, signed long long d);
inline signed long long div(signed long long x, unsigned long long d);
inline signed long long mod(signed long long x, signed long long d);
inline signed long long mod(signed long long x, unsigned long long d);

inline void inpfloor(signed long long& x);
inline void inpceil(signed long long& x);
inline void inpround(signed long long& x);
inline void inpfractional(signed long long& x);


// unsigned long long

inline unsigned long long abs(unsigned long long x);
inline unsigned long long sign(unsigned long long x);
inline unsigned long long pow(unsigned long long x, unsigned long long p);
inline unsigned long long floor(unsigned long long x);
inline unsigned long long ceil(unsigned long long x);
inline unsigned long long round(unsigned long long x);
inline unsigned long long fractional(unsigned long long x);
inline unsigned long long div(unsigned long long x, unsigned long long d);
inline unsigned long long mod(unsigned long long x, unsigned long long d);

inline void inpabs(unsigned long long& x);
inline void inpfloor(unsigned long long& x);
inline void inpceil(unsigned long long& x);
inline void inpround(unsigned long long& x);
inline void inpfractional(unsigned long long& x);

#endif



// complex numbers

template <typename T> inline std::complex<T> abs(const std::complex<T>& x);
template <typename T> inline std::complex<T> inv(const std::complex<T>& x);
template <typename T> inline std::complex<T> sqrt(const std::complex<T>& x);
template <typename T> inline std::complex<T> pow(const std::complex<T>& x, const std::complex<T>& p);
template <typename T> inline std::complex<T> exp(const std::complex<T>& x);
template <typename T> inline std::complex<T> log(const std::complex<T>& x);
template <typename T> inline std::complex<T> log2(const std::complex<T>& x);
template <typename T> inline std::complex<T> log10(const std::complex<T>& x);
template <typename T> inline std::complex<T> cos(const std::complex<T>& x);
template <typename T> inline std::complex<T> sin(const std::complex<T>& x);
template <typename T> inline std::complex<T> tan(const std::complex<T>& x);
template <typename T> inline std::complex<T> atan(const std::complex<T>& x);

template <typename T> inline T norm(const std::complex<T>& x);
template <typename T> inline std::complex<T> conj(const std::complex<T>& x);

#include "basic_ops.inl"

}

}

#endif
