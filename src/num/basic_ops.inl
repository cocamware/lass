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

// --- float ---------------------------------------------------------------------------------------

float exp( float iV ) { return ::expf( iV ); }
float log( float iV ) { return ::logf( iV ); }
float pow( float iX, float iY ) { return ::powf( iX, iY ); }
float sqr( float iV ) { return iV*iV; }
float sqrt( float iV ){ return ::sqrtf( iV ); }
float cos( float iV ) { return ::cosf( iV ); }
float sin( float iV ) { return ::sinf( iV ); }
float tan( float iV ) { return ::tanf( iV ); }
float acos( float iV ) { return ::acosf( iV ); }
float asin( float iV ) { return ::asinf( iV ); }
float atan( float iV ) { return ::atanf( iV ); }
float atan2( float iX, float iY ) { return ::atan2f( iX, iY ); }
float inv( float iV ) { return 1.f/iV; }
float sign( float iV ) { if (iV<0.f) return -1.f;if (iV>0.f) return 1.f; return 0.f; }
float abs( float iV ) { return ::fabsf( iV ); }
float floor( float iV ) { return ::floorf( iV ); }
float ceil( float iV ) { return ::ceilf( iV ); }
float round( float iV ) { return ::floorf( iV + .5f ); }
float clamp( float iV, float iMin, float iMax ) { return iV < iMin ? iMin : (iV > iMax ? iMax : iV); }
float mod( float iV, float iMod) { return iV - iMod * ::floorf(iV / iMod); }


void inpexp(float& ioV) { ioV = num::exp( ioV ); }
void inplog(float& ioV) { ioV = num::log( ioV ); }
void inppow(float& ioX, float iY) { ioX = lass::num::pow( ioX, iY ); }
void inpsqr(float& ioV) { ioV*= ioV; }
void inpsqrt(float& ioV) { ioV = num::sqrt( ioV ); }
void inpcos(float& ioV) { ioV = num::cos( ioV ); }
void inpsin(float& ioV) { ioV = num::sin( ioV ); }
void inptan(float& ioV) { ioV = num::tan( ioV ); }
void inpacos(float& ioV) { ioV = num::acos( ioV ); }
void inpasin(float& ioV) { ioV = num::asin( ioV ); }
void inpatan(float& ioV) { ioV = num::atan( ioV ); }
void inpatan2(float& ioX, float iY) { ioX = num::atan2( ioX, iY ); }
void inpinv(float& ioV) { ioV = 1.f/ioV; }
void inpsign(float& ioV) { ioV = num::sign(ioV); }
void inpabs(float& ioV) { ioV = num::abs(ioV); }
void inpfloor(float& ioV) { ioV = num::floor(ioV); }
void inpceil(float& ioV) { ioV = num::ceil(ioV); }
void inpround(float& ioV) { ioV = num::round(ioV); }
void inpclamp(float& ioV, float iMin, float iMax) { ioV = num::clamp( ioV, iMin, iMax ); }

void compnorm(float iV, float& oV) { oV = iV*iV; }
void compinv(float iV, float& oV) { oV = 1.f/iV; }
float norm(const float iV) { return iV*iV; }
float conj(const float iV) { return iV; }

// --- double --------------------------------------------------------------------------------------

double exp( double iV ) { return ::exp( iV ); }
double log( double iV ) { return ::log( iV ); }
double pow(double iX, double iY ) { return ::pow( iX, iY ); }
double sqr( double iV ) { return iV*iV; }
double sqrt( double iV ){ return ::sqrt( iV ); }
double cos( double iV ) { return ::cos( iV ); }
double sin( double iV ) { return ::sin( iV ); }
double tan( double iV ) { return ::tan( iV ); }
double acos( double iV ) { return ::acos( iV ); }
double asin( double iV ) { return ::asin( iV ); }
double atan( double iV ) { return ::atan( iV ); }
double atan2( double iX, double iY ) { return ::atan2( iX, iY ); }
double inv( double iV ) { return 1.f/iV; }
double sign( double iV ) { if (iV<0.0) return -1.0;if (iV>0.0) return 1.0; return 0.0; }
double abs( double iV ) { return fabs( iV ); }
double floor( double iV ) { return ::floor( iV ); }
double ceil( double iV ) { return ::ceil( iV ); }
double round( double iV ) { return ::floor( iV + .5 ); }
double clamp( double iV, double iMin, double iMax ) { return iV < iMin ? iMin : (iV > iMax ? iMax : iV); }
double mod( double iV, double iMod) { return iV - iMod * ::floor(iV / iMod); }

void inpexp(double& ioV) { ioV = num::exp( ioV ); }
void inplog(double& ioV) { ioV = num::log( ioV ); }
void inppow(double& ioX, double iY) { ioX = lass::num::pow( ioX, iY ); }
void inpsqr(double& ioV) { ioV *= ioV ; }
void inpsqrt(double& ioV) { ioV = num::sqrt( ioV ); }
void inpcos(double& ioV) { ioV = num::cos( ioV ); }
void inpsin(double& ioV) { ioV = num::sin( ioV ); }
void inptan(double& ioV) { ioV = num::tan( ioV ); }
void inpacos(double& ioV) { ioV = num::acos( ioV ); }
void inpasin(double& ioV) { ioV = num::asin( ioV ); }
void inpatan(double& ioV) { ioV = num::atan( ioV ); }
void inpatan2(double& ioX, double iY) { ioX = num::atan2( ioX, iY); }
void inpinv(double& ioV) { ioV = 1.0/ioV; }
void inpsign(double& ioV) { ioV = num::sign(ioV); }
void inpabs(double& ioV) { ioV = num::abs(ioV); }
void inpfloor(double& ioV) { ioV = num::floor(ioV); }
void inpceil(double& ioV) { ioV = num::ceil(ioV); }
void inpround(double& ioV) { ioV = num::round(ioV); }
void inpclamp(double& ioV, double iMin, double iMax) { ioV = num::clamp( ioV, iMin, iMax ); }

void compnorm(double iV, double& oV) { oV = iV*iV; }
void compinv(double iV, double& oV) { oV = 1.0/iV; }
double norm(const double iV) { return iV*iV; }
double conj(const double iV) { return iV; }

// --- int -----------------------------------------------------------------------------------------

// TODO can some think of a better idea to implement pow of two integers? [BdG]
int pow(int iX, int iY)
{
	if (iY < 0)
	{
		LASS_THROW("invalid paramater iY '" << iY << "'.");
	}
	if (iY < 32)
	{
		int result = 1;
		for (int i = 0; i < iY; ++i)
		{
			result *= iY;
		}
		return result;
	}
	int result = 0;
	int pow = 1;
	for (int i = 0; i < sizeof(int) * lass::bitsPerByte; ++i)
	{
		if (iY & (1 << i))
		{
			result += pow;
		}
		pow *= iX;
	}
	return result;
}

int sqr(int iV) { return iV*iV; }
int sign(int iV) { if (iV<0) return -1;if (iV>0) return 1; return 0; }
int abs(int iV) { return ::abs(iV); }
int floor(int iV) { return iV; }
int ceil(int iV) { return iV; }
int round(int iV) { return iV; }
int clamp( int iV, int iMin, int iMax ) { return iV < iMin ? iMin : (iV > iMax ? iMax : iV); }

void inppow(int& ioX, int iY) { ioX = lass::num::pow( ioX, iY ); }
void inpsqr(int& ioV) { ioV *= ioV ; }
void inpsign(int& ioV) { ioV = num::sign(ioV); }
void inpabs(int& ioV) { ioV = lass::num::abs(ioV); }
void inpfloor(int& ioV) { }
void inpceil(int& ioV) { }
void inpround(int& ioV) { }
void inpclamp(int& ioV, int iMin, int iMax) { ioV = num::clamp( ioV, iMin, iMax ); }

void compnorm(int iV, int& oV) { oV = iV*iV; }
int norm(const int iV) { return iV*iV; }
int conj(const int iV) { return iV; }

int mod(int iV,unsigned int iMod)
{
	int intMod = (int)iMod;
	int tempMod = iV%(intMod);
	if (tempMod>=0)
		return tempMod;
	else
		return tempMod+intMod;
}

// --- long -----------------------------------------------------------------------------------------

// TODO can some think of a better idea to implement pow of two integers? [BdG]
long pow(long iX, long iY)
{
	if (iY < 0)
	{
		LASS_THROW("invalid paramater iY '" << iY << "'.");
	}
	if (iY < 32)
	{
		long result = 1;
		for (long i = 0; i < iY; ++i)
		{
			result *= iY;
		}
		return result;
	}
	long result = 0;
	long pow = 1;
	for (long i = 0; i < sizeof(long) * lass::bitsPerByte; ++i)
	{
		if (iY & (1 << i))
		{
			result += pow;
		}
		pow *= iX;
	}
	return result;
}

long sqr(long iV) { return iV*iV; }
long sign(long iV) { if (iV<0) return -1;if (iV>0) return 1; return 0; }
long abs(long iV) { return ::abs(iV); }
long floor(long iV) { return iV; }
long ceil(long iV) { return iV; }
long round(long iV) { return iV; }
long clamp( long iV, long iMin, long iMax ) { return iV < iMin ? iMin : (iV > iMax ? iMax : iV); }

void inppow(long& ioX, long iY) { ioX = lass::num::pow( ioX, iY ); }
void inpsqr(long& ioV) { ioV *= ioV ; }
void inpsign(long& ioV) { ioV = num::sign(ioV); }
void inpabs(long& ioV) { ioV = lass::num::abs(ioV); }
void inpfloor(long& ioV) { }
void inpceil(long& ioV) { }
void inpround(long& ioV) { }
void inpclamp(long& ioV, long iMin, long iMax) { ioV = num::clamp( ioV, iMin, iMax ); }

void compnorm(long iV, long& oV) { oV = iV*iV; }
long norm(const long iV) { return iV*iV; }
long conj(const long iV) { return iV; }

long    mod(long iV,unsigned long iMod)
{
	long intMod = (long)iMod;
	long tempMod = iV%(intMod);
	if (tempMod>=0)
		return tempMod;
	else
		return tempMod+intMod;
}

// --- complex -------------------------------------------------------------------------------------

template <typename T> std::complex<T> exp( const std::complex<T>& ioV) { return std::exp( ioV ); }
template <typename T> std::complex<T> log( const std::complex<T>& ioV) { return std::log( ioV ); }
template <typename T> std::complex<T> pow(const std::complex<T>& iX, double iY) { return std::pow(iX, iY); }
template <typename T> std::complex<T> sqr( const std::complex<T>& ioV) { return ioV*ioV; }
template <typename T> std::complex<T> sqrt( const std::complex<T>& ioV){ return std::sqrt( ioV ); }
template <typename T> std::complex<T> cos( const std::complex<T>& ioV) { return std::cos( ioV ); }
template <typename T> std::complex<T> sin( const std::complex<T>& ioV) { return std::sin( ioV ); }
template <typename T> std::complex<T> tan( const std::complex<T>& ioV) { return std::tan( ioV ); }
template <typename T> std::complex<T> acos( const std::complex<T>& ioV){ return std::acos( ioV ); }
template <typename T> std::complex<T> asin( const std::complex<T>& ioV){ return std::asin( ioV ); }
template <typename T> std::complex<T> atan( const std::complex<T>& ioV){ return std::atan( ioV ); }
template <typename T> std::complex<T> inv( const std::complex<T>& ioV) { return T(1)/ioV; }
template <typename T> std::complex<T> abs( const std::complex<T>& iV) { return std::abs( iV ); }

template <typename T> void inpexp(std::complex<T>& ioV) { ioV = num::exp( ioV ); }
template <typename T> void inplog(std::complex<T>& ioV) { ioV = num::log( ioV ); }
template <typename T> void inppow(std::complex<T>& ioX, const std::complex<T>& iY) { ioX = lass::num::pow( ioX, iY ); }
template <typename T> void inpsqr(std::complex<T>& ioV) { ioV*= ioV; }
template <typename T> void inpsqrt(std::complex<T>& ioV){ ioV = num::sqrt( ioV ); }
template <typename T> void inpcos(std::complex<T>& ioV) { ioV = num::cos( ioV ); }
template <typename T> void inpsin(std::complex<T>& ioV) { ioV = num::sin( ioV ); }
template <typename T> void inptan(std::complex<T>& ioV) { ioV = num::tan( ioV ); }
template <typename T> void inpacos(std::complex<T>& ioV){ ioV = num::acos( ioV ); }
template <typename T> void inpasin(std::complex<T>& ioV){ ioV = num::asin( ioV ); }
template <typename T> void inpatan(std::complex<T>& ioV){ ioV = num::atan( ioV ); }
template <typename T> void inpinv(std::complex<T>& ioV) { ioV = num::inv( ioV ); }

template <typename T> void compnorm(const std::complex<T>& iV, T& oV) { oV = std::norm( iV ); }
template <typename T> void compinv(const std::complex<T>& iV, std::complex<T>& oV) { oV = T(1)/iV; }
template <typename T> T norm(const std::complex<T>& iV) { return std::norm( iV ); }
template <typename T> std::complex<T> conj(const std::complex<T>& iV) { return std::conj(iV); }
