/** 
*   @author Bram de Greve (bramz@users.sourceforge.net)
*   @author Tom De Muer (tomdemuer@users.sourceforge.net)
*
*	Distributed under the terms of the GPL (GNU Public License)
*
* 	The LASS License:
*   
*	Copyright 2004 Bram de Greve and Tom De Muer
*
*   LASS is free software; you can redistribute it and/or modify
*   it under the terms of the GNU General Public License as published by
*   the Free Software Foundation; either version 2 of the License, or
*   (at your option) any later version.
*
*   This program is distributed in the hope that it will be useful,
*   but WITHOUT ANY WARRANTY; without even the implied warranty of
*   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
*   GNU General Public License for more details.
*
*   You should have received a copy of the GNU General Public License
*   along with this program; if not, write to the Free Software
*   Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
*
*/

inline void createGaussian( FNSampled8 & oValue, float iMean, float iStdDev)
{
	oValue.makeGaussian(iMean,iStdDev);
}
	
inline void createTriangular( FNSampled8 & oValue,	float iLeft,
										float iMid,
										float iRight)
{
	oValue.makeTriangular(iLeft,iMid,iRight);
}

inline void createTrapezoidal( FNSampled8 & oValue,float iA,
									  float iB,
									  float iC,
									  float iD)
{
	oValue.makeTrapezoidal(iA,iB,iC,iD);
}
	
inline FNSampled8::TInterval	support( const FNSampled8 & iValue )
{
	return iValue.getSupport();
}
	
inline FNSampled8::TInterval	support( const FNSampled8 & iValue, float iAlpha )
{
	return iValue.getSupport(iAlpha);
}

inline FNSampled8::TBaseType
memberShip( const FNSampled8 & iValue, float iX )
{
	return iValue.getMembership(iX);
}
 
inline FNSampled8::TBaseType	infinum( const FNSampled8 & iValue )
{
	return iValue.getSupport().inf();
}
 
inline FNSampled8::TBaseType	supinum( const FNSampled8 & iValue )
{
	return iValue.getSupport().sup();
}
 
inline FNSampled8::TBaseType	footprint( const FNSampled8 & iValue )
{
	return iValue.getSupport().diam();
}
 
inline FNSampled8::TBaseType	entropy( const FNSampled8 & iValue )
{
	return iValue.getEntropy();
}
 
inline FNSampled8::TBaseType	differentialEntropy( const FNSampled8 & iValue )
{
	return iValue.getDifferentialEntropy();
}
 
inline FNSampled8::TBaseType	nonSpecificity( const FNSampled8 & iValue )
{
	return iValue.getNonspecificity();
}
 
inline FNSampled8::TBaseType	discord( const FNSampled8 & iValue )
{
	return iValue.getDiscord();
}
 
inline FNSampled8::TBaseType	bandwidth( const FNSampled8 & iValue )
{
	return support( iValue, 0.5 ).diam();
}

inline FNSampled8::TBaseType	bandwidthRatio( const FNSampled8 & iValue )
{
	return support( iValue, 0.5 ).ratio();
}


inline FNSampled8::TBaseType	maxMembership( const FNSampled8  & iValue )
{
	return iValue.defuzzifyMaxMembership();
}

inline FNSampled8::TBaseType	centroid( const FNSampled8  & iValue )
{
	return iValue.defuzzifyCentroid();
}



