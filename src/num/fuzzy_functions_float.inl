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



inline void createGaussian( float& oValue, float iMean, float iStdDev)
{
	oValue = iMean;
}

inline void createTriangular( float& oValue,    float iLeft,
										float iMid,
										float iRight)
{
	oValue = iMid;
}

inline void createTrapezoidal( float& oValue,float iA,
									  float iB,
									  float iC,
									  float iD)
{
	oValue = (iB + iC)*0.5f;
}

inline interval<NumTraits<float>::intervalType> support( const float& iValue )
{
	return interval<NumTraits<float>::intervalType>( iValue );
}

inline interval<NumTraits<float>::intervalType> support( const float& iValue, float iAlpha )
{
	return interval<NumTraits<float>::intervalType>( iValue );
}

inline NumTraits<float>::baseType
memberShip( const float& iValue, float iX )
{
	if (iValue==iX)
		return NumTraits<NumTraits<float>::baseType>::one;
	else
		return NumTraits<NumTraits<float>::baseType>::zero;
}

inline NumTraits<float>::baseType   infinum( const float& iValue )
{
	return iValue;
}

inline NumTraits<float>::baseType   supinum( const float& iValue )
{
	return iValue;
}

inline NumTraits<float>::baseType   footprint( const float& iValue )
{
	return NumTraits<NumTraits<float>::baseType>::zero;
}

inline NumTraits<float>::baseType   entropy( const float& iValue )
{
	return NumTraits<NumTraits<float>::baseType>::zero;
}

inline NumTraits<float>::baseType   differentialEntropy( const float& iValue )
{
	return NumTraits<NumTraits<float>::baseType>::zero;
}

inline NumTraits<float>::baseType   nonSpecificity( const float& iValue )
{
	return NumTraits<NumTraits<float>::baseType>::zero;
}

inline NumTraits<float>::baseType   discord( const float& iValue )
{
	return NumTraits<NumTraits<float>::baseType>::zero;
}

inline NumTraits<float>::baseType   bandwidth( const float& iValue )
{
	return NumTraits<NumTraits<float>::baseType>::zero;
}

inline NumTraits<float>::baseType   bandwithRatio( const float& iValue )
{
	return NumTraits<NumTraits<float>::baseType>::one;
}

inline NumTraits<float>::baseType   maxMembership( const float& iValue )
{
	return iValue;
}

inline NumTraits<float>::baseType   centroid( const float& iValue )
{
	return iValue;
}
