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



#ifndef LASS_GUARDIAN_OF_INCLUSION_NUM_BASIC_OPS_H
#define LASS_GUARDIAN_OF_INCLUSION_NUM_BASIC_OPS_H

#include <cmath>

namespace lass
{
namespace num
{

// float

inline float exp(float iV);
inline float log(float iV);
inline float pow(float iX, float iY);
inline float sqr(float iV);
inline float sqrt(float iV);
inline float cos(float iV);
inline float sin(float iV);
inline float tan(float iV);
inline float acos(float iV);
inline float asin(float iV);
inline float atan(float iV);
inline float atan2(float iX, float iY);
inline float inv(float iV);
inline float sign(float iV);
inline float abs(float iV);
inline float floor(float iV);
inline float ceil(float iV);
inline float round(float iV);
inline float clamp(float iV, float iMin, float iMax);

inline void inpexp(float& ioV);
inline void inplog(float& ioV);
inline void inppow(float& iX, float iY);
inline void inpsqr(float& ioV);
inline void inpsqrt(float& ioV);
inline void inpcos(float& ioV);
inline void inpsin(float& ioV);
inline void inptan(float& ioV);
inline void inpacos(float& ioV);
inline void inpasin(float& ioV);
inline void inpatan(float& ioV);
inline void inpatan2(float& ioX, float iY);
inline void inpinv(float& ioV);
inline void inpsign(float& ioV);
inline void inpabs(float& ioV);
inline void inpfloor(float& ioV);
inline void inpceil(float& ioV);
inline void inpround(float& ioV);
inline void inpclamp(float& ioV, float iMin, float iMax);

inline void compnorm(float iV, float& oV);
inline void compinv(float iV, float& oV);
inline float norm(const float iV);
inline float conj(const float iV);



// double

inline double exp(double iV);
inline double log(double iV);
inline double pow(double iX, double iY);
inline double sqr(double iV);
inline double sqrt(double iV);
inline double cos(double iV);
inline double sin(double iV);
inline double tan(double iV);
inline double acos(double iV);
inline double asin(double iV);
inline double atan(double iV);
inline double atan2(double iX, double iY);
inline double inv(double iV);
inline double sign(double iV);
inline double abs(double iV);
inline double floor(double iV);
inline double ceil(double iV);
inline double round(double iV);
inline double clamp(double iV, double iMin, double iMax);

inline void inpexp(double& ioV);
inline void inplog(double& ioV);
inline void inppow(double& ioX, double iY);
inline void inpsqr(double& ioV);
inline void inpsqrt(double& ioV);
inline void inpcos(double& ioV);
inline void inpsin(double& ioV);
inline void inptan(double& ioV);
inline void inpacos(double& ioV);
inline void inpasin(double& ioV);
inline void inpatan(double& ioV);
inline void inpatan2(double& ioX, double iY);
inline void inpinv(double& ioV);
inline void inpsign(double& ioV);
inline void inpabs(double& ioV);
inline void inpfloor(double& ioV);
inline void inpceil(double& ioV);
inline void inpround(double& ioV);
inline void inpclamp(double& ioV, double iMin, double iMax);

inline void compnorm(double iV, double& oV);
inline void compinv(double iV, double& oV);
inline double norm(const double iV);
inline double conj(const double iV);



// int

inline int	mod(int iV,unsigned int iMod);
inline int	pow(int iX, int iY);
inline int	sqr(int iV);
inline int	sign(int iV);
inline int	abs(int iV);
inline int	floor(int iV);
inline int	ceil(int iV);
inline int	round(int iV);
inline int	clamp(int iV, int iMin, int iMax);

inline void inppow(int& iX, int iY);
inline void inpsqr(int& ioV);
inline void inpsign(int& ioV);
inline void inpabs(int& ioV);
inline void inpfloor(int& ioV);
inline void inpceil(int& ioV);
inline void inpround(int& ioV);
inline void inpclamp(int& ioV, int iMin, int iMax);

inline void compnorm(int iV, int& oV);
inline int norm(const int iV);
inline int conj(const int iV);



// long

inline long	mod(long iV,unsigned long iMod);
inline long	pow(long iX, long iY);
inline long	sqr(long iV);
inline long	sign(long iV);
inline long	abs(long iV);
inline long	floor(long iV);
inline long	ceil(long iV);
inline long	round(long iV);
inline long	clamp(long iV, long iMin, long iMax);

inline void inppow(long& iX, long iY);
inline void inpsqr(long& ioV);
inline void inpsign(long& ioV);
inline void inpabs(long& ioV);
inline void inpfloor(long& ioV);
inline void inpceil(long& ioV);
inline void inpround(long& ioV);
inline void inpclamp(long& ioV, long iMin, long iMax);

inline void compnorm(long iV, long& oV);
inline long norm(const long iV);
inline long conj(const long iV);



// complex

template <typename T> inline std::complex<T> exp(const std::complex<T>& ioV);
template <typename T> inline std::complex<T> log(const std::complex<T>& ioV);
template <typename T> inline std::complex<T> pow(const std::complex<T>& iX, 
												 const std::complex<T>& iY);
template <typename T> inline std::complex<T> sqr(const std::complex<T>& ioV);
template <typename T> inline std::complex<T> sqrt(const std::complex<T>& ioV);
template <typename T> inline std::complex<T> cos(const std::complex<T>& ioV);
template <typename T> inline std::complex<T> sin(const std::complex<T>& ioV);
template <typename T> inline std::complex<T> tan(const std::complex<T>& ioV);
template <typename T> inline std::complex<T> atan(const std::complex<T>& ioV);
template <typename T> inline std::complex<T> inv(const std::complex<T>& ioV);
template <typename T> inline std::complex<T> abs(const std::complex<T>& iV);

template <typename T> inline void inpexp(std::complex<T>& ioV);
template <typename T> inline void inplog(std::complex<T>& ioV);
template <typename T> inline void inppow(std::complex<T>& iX, const std::complex<T>& iY);
template <typename T> inline void inpsqr(std::complex<T>& ioV);
template <typename T> inline void inpsqrt(std::complex<T>& ioV);
template <typename T> inline void inpcos(std::complex<T>& ioV);
template <typename T> inline void inpsin(std::complex<T>& ioV);
template <typename T> inline void inptan(std::complex<T>& ioV);
template <typename T> inline void inpacos(std::complex<T>& ioV);
template <typename T> inline void inpasin(std::complex<T>& ioV);
template <typename T> inline void inpatan(std::complex<T>& ioV);
template <typename T> inline void inpinv(std::complex<T>& ioV);

template <typename T> inline void compnorm(const std::complex<T>& iV, T& oV);
template <typename T> inline void compinv(const std::complex<T>& iV, std::complex<T>& oV);
template <typename T> inline T norm(const std::complex<T>& iV);
template <typename T> inline std::complex<T> conj(const std::complex<T>& iV);

#include "basic_ops.inl"

}

}

#endif
