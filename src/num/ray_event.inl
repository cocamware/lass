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



#include "ray_event.h"

template <class TN, RayEventType T, unsigned M, unsigned D, unsigned A, unsigned C>
RayEventType RayEventImpl<TN, T, M, D, A, C>::GetType() const
{
	return T;
}



template <class TN, RayEventType T, unsigned M, unsigned D, unsigned A, unsigned C>
Medium RayEventImpl<TN, T, M, D, A, C>::GetMedium(int iIndex) const
{
	return CheckMediumIndex(iIndex) ? medium_[iIndex] : 0;
}



template <class TN, RayEventType T, unsigned M, unsigned D, unsigned A, unsigned C>
TN RayEventImpl<TN, T, M, D, A, C>::GetDistance(int iIndex) const
{
	return CheckDistanceIndex(iIndex) ? distance_[iIndex] : 0.0;
}



template <class TN, RayEventType T, unsigned M, unsigned D, unsigned A, unsigned C>
TN RayEventImpl<TN, T, M, D, A, C>::GetAngle(int iIndex) const
{
	return CheckAngleIndex(iIndex) ? angle_[iIndex] : 0.0;
}



template <class TN, RayEventType T, unsigned M, unsigned D, unsigned A, unsigned C>
void RayEventImpl<TN, T, M, D, A, C>::GetCoords(int iIndex,TN& oX,TN& oY,TN& oZ) const
{
	if (CheckCoordIndex(iIndex))
	{
		oX = x_[iIndex];
		oY = y_[iIndex];
		oZ = z_[iIndex];
	}
}



template <class TN, RayEventType T, unsigned M, unsigned D, unsigned A, unsigned C>
void RayEventImpl<TN, T, M, D, A, C>::SetMedium(int iIndex, Medium iMedium)
{
	if (CheckMediumIndex(iIndex))
	{
		medium_[iIndex] = iMedium;
	}
}



template <class TN, RayEventType T, unsigned M, unsigned D, unsigned A, unsigned C>
void RayEventImpl<TN, T, M, D, A, C>::SetDistance(int iIndex, TN iDistance)
{
	if (CheckDistanceIndex(iIndex))
	{
		distance_[iIndex] = iDistance;
	}
}



template <class TN, RayEventType T, unsigned M, unsigned D, unsigned A, unsigned C>
void RayEventImpl<TN, T, M, D, A, C>::SetAngle(int iIndex, TN iAngle)
{
	if (CheckAngleIndex(iIndex))
	{
		angle_[iIndex] = iAngle;
	}
}



template <class TN, RayEventType T, unsigned M, unsigned D, unsigned A, unsigned C>
void RayEventImpl<TN, T, M, D, A, C>::SetCoords(int iIndex, TN iX, TN iY, TN iZ)
{
	if (CheckDistanceIndex(iIndex))
	{
		x_[iIndex] = iX;
		y_[iIndex] = iY;
		z_[iIndex] = iZ;
	}
}



////////////////////////////////////////////////////////////////////////////////

template <class TN, RayEventType T, unsigned M, unsigned D, unsigned A, unsigned C>
bool RayEventImpl<TN, T, M, D, A, C>::CheckMediumIndex(int iIndex) const
{
	if ((iIndex<0) || (iIndex>=M))
	{
		LASS_THROW("medium index '" << iIndex << "' out of bounds [0, " << M << ")");
		return false;
	}
	return true;
}



template <class TN, RayEventType T, unsigned M, unsigned D, unsigned A, unsigned C>
bool RayEventImpl<TN, T, M, D, A, C>::CheckDistanceIndex(int iIndex) const
{
	if ((iIndex<0) || (iIndex>=D))
	{
		LASS_THROW("distance index '" << iIndex << "' out of bounds [0, " << D << ")");
		return false;
	}
	return true;
}



template <class TN, RayEventType T, unsigned M, unsigned D, unsigned A, unsigned C>
bool RayEventImpl<TN, T, M, D, A, C>::CheckAngleIndex(int iIndex) const
{
	if ((iIndex<0) || (iIndex>=A))
	{
		LASS_THROW("angle index '" << iIndex << "' out of bounds [0, " << A << ")");
		return false;
	}
	return true;
}



template <class TN, RayEventType T, unsigned M, unsigned D, unsigned A, unsigned C>
bool RayEventImpl<TN, T, M, D, A, C>::CheckCoordIndex(int iIndex) const
{
	if ((iIndex<0) || (iIndex>=C))
	{
		LASS_THROW("coord index '" << iIndex << "' out of bounds [0, " << C << ")");
		return false;
	}
	return true;
}
