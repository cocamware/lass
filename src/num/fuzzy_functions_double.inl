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



inline void createGaussian( double& oValue, double iMean, double iStdDev)
{
	oValue = iMean;
}
	
inline void createTriangular( double& oValue,	double iLeft,
										double iMid,
										double iRight)
{
	oValue = iMid;
}

inline void createTrapezoidal( double& oValue,double iA,
									  double iB,
									  double iC,
									  double iD)
{
	oValue = (iB + iC)*0.5f;
}
	
inline interval<NumTraits<double>::intervalType>	support( const double& iValue )
{
	return interval<NumTraits<double>::intervalType>( iValue );
}
	
inline interval<NumTraits<double>::intervalType>	support( const double& iValue, double iAlpha )
{
	return interval<NumTraits<double>::intervalType>( iValue );
}

inline NumTraits<double>::baseType
memberShip( const double& iValue, double iX )
{
	if (iValue==iX)
		return NumTraits<NumTraits<double>::baseType>::one;
	else
		return NumTraits<NumTraits<double>::baseType>::zero;
}
 
inline NumTraits<double>::baseType	infinum( const double& iValue )
{
	return iValue;
}
 
inline NumTraits<double>::baseType	supinum( const double& iValue )
{
	return iValue;
}
 
inline NumTraits<double>::baseType	footprint( const double& iValue )
{
	return NumTraits<NumTraits<double>::baseType>::zero;
}
 
inline NumTraits<double>::baseType	entropy( const double& iValue )
{
	return NumTraits<NumTraits<double>::baseType>::zero;
}
 
inline NumTraits<double>::baseType	differentialEntropy( const double& iValue )
{
	return NumTraits<NumTraits<double>::baseType>::zero;
}
 
inline NumTraits<double>::baseType	nonSpecificity( const double& iValue )
{
	return NumTraits<NumTraits<double>::baseType>::zero;
}
 
inline NumTraits<double>::baseType	discord( const double& iValue )
{
	return NumTraits<NumTraits<double>::baseType>::zero;
}
 
inline NumTraits<double>::baseType	bandwidth( const double& iValue )
{
	return NumTraits<NumTraits<double>::baseType>::zero;
}

inline NumTraits<double>::baseType	bandwithRatio( const double& iValue )
{
	return NumTraits<NumTraits<double>::baseType>::one;
}
 
inline NumTraits<double>::baseType	maxMembership( const double& iValue )
{
	return iValue;
}

inline NumTraits<double>::baseType	centroid( const double& iValue )
{
	return iValue;
}
