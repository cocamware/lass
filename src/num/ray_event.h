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



#if !defined(AFX_RAYEVENT_H__BE5FAEA2_278E_4F0C_BB01_458543EA838C__INCLUDED_)
#define AFX_RAYEVENT_H__BE5FAEA2_278E_4F0C_BB01_458543EA838C__INCLUDED_

#if _MSC_VER > 1000

#endif // _MSC_VER > 1000

#include <exception>
#include <lass/util/fixed_array.h>

namespace lass
{
namespace num
{

typedef void*       Medium;


enum enumEventType_ { Propagation, Reflection, DiffuseReflection, Diffraction, TopDiffraction };
typedef enum enumEventType_ RayEventType;

template<class T>
class RayEvent
{
public:
	virtual ~RayEvent() {}

	virtual RayEventType GetType() const = 0;

	virtual Medium  GetMedium(int iIndex) const = 0;
	virtual T  GetDistance(int iIndex) const = 0;
	virtual T  GetAngle(int iIndex) const = 0;
	virtual void    GetCoords(int iIndex, T& oX, T& oY, T& oZ) const = 0;

	virtual void SetMedium(int iIndex,Medium iMedium) = 0;
	virtual void SetDistance(int iIndex,T iDistance) = 0;
	virtual void SetAngle(int iIndex,T iAngle) = 0;
	virtual void SetCoords(int iIndex, T iX, T iY, T iZ) = 0;
};



template <class T, RayEventType type, unsigned numMedia, unsigned numDistances, unsigned numAngles, unsigned numCoords>
class RayEventImpl: public RayEvent<T>
{
public:

	inline  RayEventType GetType() const;

	inline  Medium  GetMedium(int iIndex) const;
	inline  T  GetDistance(int iIndex) const;
	inline  T  GetAngle(int iIndex) const;
	inline  void    GetCoords(int iIndex, T& oX, T& oY, T& oZ) const;

	inline  void    SetMedium(int iIndex,Medium iMedium);
	inline  void    SetDistance(int iIndex,T iDistance);
	inline  void    SetAngle(int iIndex,T iAngle);
	inline  void    SetCoords(int iIndex, T iX, T iY, T iZ);

private:

	bool CheckMediumIndex(int iIndex) const;
	bool CheckDistanceIndex(int iIndex) const;
	bool CheckAngleIndex(int iIndex) const;
	bool CheckCoordIndex(int iIndex) const;

	lass::util::FixedArray<Medium, numMedia> medium_;
	lass::util::FixedArray<T, numDistances> distance_;
	lass::util::FixedArray<T, numAngles> angle_;
	lass::util::FixedArray<T, numCoords> x_;
	lass::util::FixedArray<T, numCoords> y_;
	lass::util::FixedArray<T, numCoords> z_;
};

//
/////////////////////////////////////////////////////
//

template<class T>
struct RayEventTypes
{
	typedef RayEventImpl<T, Propagation, 1, 2, 0, 2> PropagationEvent;
	// medium
	// distance
	// coord: 0 = from
	//        1 = to

	typedef RayEventImpl<T, Reflection, 2, 2, 1, 0> ReflectionEvent;
	// medium: 0 = normally air, 1 = medium upon which is reflected
	// distance:
	//  0 distance from source to medium reflection point
	//  1 distance from medium reflection point to receiver
	// angle: angle with the normal

	typedef RayEventImpl<T, DiffuseReflection, 2, 0, 2, 0> DiffuseReflectionEvent;
	// medium: 0 = normally air,
	//         1 = medium upon which is reflected
	// angle: 0 = incoming angle,
	//        1 = outgoing angle with the normal

	typedef RayEventImpl<T,Diffraction, 3, 4, 4, 0> DiffractionEvent;
	// medium: 0 = normally air
	//         1 = medium upon which incoming ray is reflected,
	//         2 = medium upon which outgoing ray is reflected */
	// distance: 0 = distance from first point to wedge, r in cylindrical coord
	//           1 = distance from wedge to second point, r in cylindrical coord
	//           2 = distance along z-axis in cylindrical coordinates
	//           3 = direct path length between first and second point.
	// angle: 0 = angle between reflecting boundaries
	//        1 = incoming angle with the first medium
	//        2 = outgoing angle with the second medium
	//        3 = Keller cone angle
	typedef RayEventImpl<T,TopDiffraction, 3, 4, 4, 0> TopDiffractionEvent;
	// medium: 0 = normally air
	//         1 = medium upon which incoming ray is reflected,
	//         2 = medium upon which outgoing ray is reflected */
	// distance: 0 = distance from first point to wedge, r in cylindrical coord
	//           1 = distance from wedge to second point, r in cylindrical coord
	//           2 = distance along z-axis in cylindrical coordinates
	//           3 = direct path length between first and second point.
	// angle: 0 = angle between reflecting boundaries
	//        1 = incoming angle with the first medium
	//        2 = outgoing angle with the second medium
	//        3 = Keller cone angle
};

#include "ray_event.inl"

}
}

#endif // !defined(AFX_RAYEVENT_H__BE5FAEA2_278E_4F0C_BB01_458543EA838C__INCLUDED_)
