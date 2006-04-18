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



/** @defgroup BasicOps
 *  Basic Operators
 */


#ifndef LASS_GUARDIAN_OF_INCLUSION_NUM_BASIC_OPS_H
#define LASS_GUARDIAN_OF_INCLUSION_NUM_BASIC_OPS_H

#include <cmath>
#include "num_traits.h"

namespace lass
{
namespace num
{

inline long sqrt(long iV) 
{ 
	LASS_THROW("Danger will robinson!"); 
	return 0; 
}

// generic functions

template <typename T> inline T abs(const T& iV);
template <typename T> inline T sign(const T& iV);
template <typename T> inline T sqr(const T& iV);
template <typename T> inline T inv(const T& iV);
template <typename T> inline T cubic(const T& iV);
template <typename T> inline T pow(const T& iV, const T& iPow);
template <typename T> inline T norm(const T& iV);
template <typename T> inline T conj(const T& iV);
template <typename T> inline const T& clamp(const T& iV, const T& iMin, const T& iMax);

template <typename T> inline void inpabs(T& ioV);
template <typename T> inline void inpsign(T& ioV);
template <typename T> inline void inpinv(T& ioV);
template <typename T> inline void inpsqrt(T& ioV);
template <typename T> inline void inpsqr(T& ioV);
template <typename T> inline void inpcubic(T& ioV);
template <typename T> inline void inppow(T& ioV, const T& iPow);
template <typename T> inline void inpexp(T& ioV);
template <typename T> inline void inplog(T& ioV);
template <typename T> inline void inpcos(T& ioV);
template <typename T> inline void inpsin(T& ioV);
template <typename T> inline void inptan(T& ioV);
template <typename T> inline void inpacos(T& ioV);
template <typename T> inline void inpasin(T& ioV);
template <typename T> inline void inpatan(T& ioV);
template <typename T> inline void inpatan2(T& ioX, const T& iY);
template <typename T> inline void inpfloor(T& ioV);
template <typename T> inline void inpceil(T& ioV);
template <typename T> inline void inpround(T& ioV);
template <typename T> inline void inpfractional(T& ioV);
template <typename T> inline void inpdiv(T& ioV, const T& iMod);
template <typename T> inline void inpmod(T& ioV, const T& iMod);
template <typename T> inline void inpclamp(T& ioV, const T& iMin, const T& iMax);
template <typename T> inline void compnorm(const T& iV, T& oV);
template <typename T> inline void compinv(const T& iV, T& oV);

template<typename T,typename f> inline T applyFunction(const T& iV, f func );



// float

inline float abs(float iV);
inline float inv(float iV);
inline float sqrt(float iV);
inline float pow(float iV, float iPow);
inline float exp(float iV);
inline float log(float iV);
inline float cos(float iV);
inline float sin(float iV);
inline float tan(float iV);
inline float acos(float iV);
inline float asin(float iV);
inline float atan(float iV);
inline float atan2(float iX, float iY);
inline float floor(float iV);
inline float ceil(float iV);
inline float round(float iV);
inline float fractional(float iV);
inline float div(float iV, float iMod);
inline float mod(float iV, float iMod);



// double

inline double abs(double iV);
inline double inv(double iV);
inline double sqrt(double iV);
inline double pow(double iV, double iPow);
inline double exp(double iV);
inline double log(double iV);
inline double cos(double iV);
inline double sin(double iV);
inline double tan(double iV);
inline double acos(double iV);
inline double asin(double iV);
inline double atan(double iV);
inline double atan2(double iX, double iY);
inline double floor(double iV);
inline double ceil(double iV);
inline double round(double iV);
inline double fractional(double iV);
inline double div(double iV, double iMod);
inline double mod(double iV, double iMod);



// long double

inline long double abs(long double iV);
inline long double inv(long double iV);
inline long double sqrt(long double iV);
inline long double pow(long double iV, long double iPow);
inline long double exp(long double iV);
inline long double log(long double iV);
inline long double cos(long double iV);
inline long double sin(long double iV);
inline long double tan(long double iV);
inline long double acos(long double iV);
inline long double asin(long double iV);
inline long double atan(long double iV);
inline long double atan2(long double iX, long double iY);
inline long double floor(long double iV);
inline long double ceil(long double iV);
inline long double round(long double iV);
inline long double fractional(long double iV);
inline long double div(long double iV, long double iMod);
inline long double mod(long double iV, long double iMod);



// --- char ----------------------------------------------------------------------------------------

inline char pow(char iV, char iPow);
inline char floor(char iV);
inline char ceil(char iV);
inline char round(char iV);
inline char fractional(char iV);
inline char div(char iV, char iPow);
inline char mod(char iV, char iPow);

inline void inpabs(char& ioV);
inline void inpfloor(char& ioV);
inline void inpceil(char& ioV);
inline void inpround(char& ioV);
inline void inpfractional(char& ioV);

#ifdef LASS_CHAR_IS_SIGNED

inline char pow(char iV, unsigned char iPow);
inline char div(char iV, unsigned char iMod);
inline char mod(char iV, unsigned char iMod);

#else

inline char abs(char iV);
inline char sign(char iV);

#endif



// signed char

inline signed char abs(signed char iV);
inline signed char pow(signed char iV, signed char iPow);
inline signed char pow(signed char iV, unsigned char iPow);
inline signed char floor(signed char iV);
inline signed char ceil(signed char iV);
inline signed char round(signed char iV);
inline signed char fractional(signed char iV);
inline signed char div(signed char iV, signed char iMod);
inline signed char div(signed char iV, unsigned char iMod);
inline signed char mod(signed char iV, signed char iMod);
inline signed char mod(signed char iV, unsigned char iMod);

inline void inpfloor(signed char& ioV);
inline void inpceil(signed char& ioV);
inline void inpround(signed char& ioV);
inline void inpfractional(signed char& ioV);

// unsigned char

inline unsigned char abs(unsigned char iV);
inline unsigned char sign(unsigned char iV);
inline unsigned char pow(unsigned char iV, unsigned char iPow);
inline unsigned char floor(unsigned char iV);
inline unsigned char ceil(unsigned char iV);
inline unsigned char round(unsigned char iV);
inline unsigned char fractional(unsigned char iV);
inline unsigned char div(unsigned char iV, unsigned char iMod);
inline unsigned char mod(unsigned char iV, unsigned char iMod);

inline void inpabs(unsigned char& ioV);
inline void inpfloor(unsigned char& ioV);
inline void inpceil(unsigned char& ioV);
inline void inpround(unsigned char& ioV);
inline void inpfractional(unsigned char& ioV);



// signed short

inline signed short abs(signed short iV);
inline signed short pow(signed short iV, signed short iPow);
inline signed short pow(signed short iV, unsigned short iPow);
inline signed short floor(signed short iV);
inline signed short ceil(signed short iV);
inline signed short round(signed short iV);
inline signed short fractional(signed short iV);
inline signed short div(signed short iV, signed short iMod);
inline signed short div(signed short iV, unsigned short iMod);
inline signed short mod(signed short iV, signed short iMod);
inline signed short mod(signed short iV, unsigned short iMod);

inline void inpfloor(signed short& ioV);
inline void inpceil(signed short& ioV);
inline void inpround(signed short& ioV);
inline void inpfractional(signed short& ioV);

// unsigned short

inline unsigned short abs(unsigned short iV);
inline unsigned short sign(unsigned short iV);
inline unsigned short pow(unsigned short iV, unsigned short iPow);
inline unsigned short floor(unsigned short iV);
inline unsigned short ceil(unsigned short iV);
inline unsigned short round(unsigned short iV);
inline unsigned short fractional(unsigned short iV);
inline unsigned short div(unsigned short iV, unsigned short iMod);
inline unsigned short mod(unsigned short iV, unsigned short iMod);

inline void inpabs(unsigned short& ioV);
inline void inpfloor(unsigned short& ioV);
inline void inpceil(unsigned short& ioV);
inline void inpround(unsigned short& ioV);
inline void inpfractional(unsigned short& ioV);



// signed int

inline signed int abs(signed int iV);
inline signed int pow(signed int iV, signed int iPow);
inline signed int pow(signed int iV, unsigned int iPow);
inline signed int floor(signed int iV);
inline signed int ceil(signed int iV);
inline signed int round(signed int iV);
inline signed int fractional(signed int iV);
inline signed int div(signed int iV, signed int iMod);
inline signed int div(signed int iV, unsigned int iMod);
inline signed int mod(signed int iV, signed int iMod);
inline signed int mod(signed int iV, unsigned int iMod);

inline void inpfloor(signed int& ioV);
inline void inpceil(signed int& ioV);
inline void inpround(signed int& ioV);
inline void inpfractional(signed int& ioV);

// unsigned int

inline unsigned int abs(unsigned int iV);
inline unsigned int sign(unsigned int iV);
inline unsigned int pow(unsigned int iX, unsigned int iY);
inline unsigned int floor(unsigned int iV);
inline unsigned int ceil(unsigned int iV);
inline unsigned int round(unsigned int iV);
inline unsigned int fractional(unsigned int iV);
inline unsigned int div(unsigned int iV, unsigned int iMod);
inline unsigned int mod(unsigned int iV, unsigned int iMod);

inline void inpabs(unsigned int& ioV);
inline void inpfloor(unsigned int& ioV);
inline void inpceil(unsigned int& ioV);
inline void inpround(unsigned int& ioV);
inline void inpfractional(unsigned int& ioV);



// signed long

inline signed long abs(signed long iV);
inline signed long pow(signed long iX, signed long iY);
inline signed long pow(signed long iX, unsigned long iY);
inline signed long floor(signed long iV);
inline signed long ceil(signed long iV);
inline signed long round(signed long iV);
inline signed long fractional(signed long iV);
inline signed long div(signed long iV, signed long iMod);
inline signed long div(signed long iV, unsigned long iMod);
inline signed long mod(signed long iV, signed long iMod);
inline signed long mod(signed long iV, unsigned long iMod);

inline void inpfloor(signed long& ioV);
inline void inpceil(signed long& ioV);
inline void inpround(signed long& ioV);
inline void inpfractional(signed long& ioV);

// unsigned long

inline unsigned long abs(unsigned long iV);
inline unsigned long sign(unsigned long iV);
inline unsigned long pow(unsigned long iV, unsigned long iPow);
inline unsigned long floor(unsigned long iV);
inline unsigned long ceil(unsigned long iV);
inline unsigned long round(unsigned long iV);
inline unsigned long fractional(unsigned long iV);
inline unsigned long div(unsigned long iV, unsigned long iMod);
inline unsigned long mod(unsigned long iV, unsigned long iMod);

inline void inpabs(unsigned long& ioV);
inline void inpfloor(unsigned long& ioV);
inline void inpceil(unsigned long& ioV);
inline void inpround(unsigned long& ioV);
inline void inpfractional(unsigned long& ioV);



// complex numbers

template <typename T> inline std::complex<T> abs(const std::complex<T>& iV);
template <typename T> inline std::complex<T> inv(const std::complex<T>& iV);
template <typename T> inline std::complex<T> sqrt(const std::complex<T>& iV);
template <typename T> inline std::complex<T> pow(const std::complex<T>& iV, const std::complex<T>& iPow);
template <typename T> inline std::complex<T> exp(const std::complex<T>& iV);
template <typename T> inline std::complex<T> log(const std::complex<T>& iV);
template <typename T> inline std::complex<T> cos(const std::complex<T>& iV);
template <typename T> inline std::complex<T> sin(const std::complex<T>& iV);
template <typename T> inline std::complex<T> tan(const std::complex<T>& iV);
template <typename T> inline std::complex<T> atan(const std::complex<T>& iV);

template <typename T> inline T norm(const std::complex<T>& iV);
template <typename T> inline std::complex<T> conj(const std::complex<T>& iV);

#include "basic_ops.inl"

}

}

#endif
