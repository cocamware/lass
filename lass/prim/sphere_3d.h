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
 *	Copyright (C) 2004-2011 the Initial Developer.
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



/** @struct lass::prim::Sphere3D
 *  @brief 3D Sphere
 *  @author Bram de Greve
 *  @date 2002-2003
 */



#ifndef LASS_GUARDIAN_OF_INCLUSION_PRIM_SPHERE_3D_H
#define LASS_GUARDIAN_OF_INCLUSION_PRIM_SPHERE_3D_H




// --- OLD INTERFACES ---------------------------------------------------------------------------

#include "prim_common.h"
#include "point_3d.h"



// --- NEW INTERFACES ---------------------------------------------------------------------------

namespace lass
{

namespace prim
{

template<typename T>
class Sphere3D
{
public:

	typedef Sphere3D<T> TSelf;

	typedef Point3D<T> TPoint;
	typedef typename TPoint::TVector TVector;

	typedef typename TPoint::TValue TValue;
	typedef typename TPoint::TParam TParam;
	typedef typename TPoint::TReference TReference;
	typedef typename TPoint::TConstReference TConstReference;
	typedef typename TPoint::TNumTraits TNumTraits;

	enum { dimension = TPoint::dimension }; /**< number of dimensions */

	template <typename U> struct Rebind
	{
		typedef Sphere3D<U> Type;
	};

	Sphere3D();
	Sphere3D(const Point3D<T>& iCenter, TParam iRadius);

	const TPoint& center() const;
	TPoint& center();

	TConstReference radius() const;
	TReference radius();

	const TValue area() const;
	const TValue volume() const;

	Side classify(const TPoint& iPoint) const;
	const TValue equation(const TPoint& iPoint) const;
	const TValue signedDistance(const TPoint& iPoint) const;
	const TValue squaredDistance(const TPoint& iPoint) const;
	bool contains(const TPoint& iPoint) const;

	Side classify(const TPoint& iPoint, TParam iRelativeTolerance) const;
	const TValue equation(const TPoint& iPoint, TParam iRelativeTolerance) const;
	const TValue signedDistance(const TPoint& iPoint, TParam iRelativeTolerance) const;
	const TValue squaredDistance(const TPoint& iPoint, TParam iRelativeTolerance) const;
	bool contains(const TPoint& iPoint, TParam iRelativeTolerance) const;

	bool isValid() const;

private:

	TPoint center_;     /**< center of sphere */
	TValue radius_;     /**< radius of sphere */
};

template <typename T> const T squaredDistance(const Sphere3D<T>& sphere, const Point3D<T>& point);
template <typename T> const T distance(const Sphere3D<T>& sphere, const Point3D<T>& point);
template <typename T> bool intersects(const Sphere3D<T>& a, const Sphere3D<T>& b);

template<typename T> std::ostream& operator<<(std::ostream& oOStream, const Sphere3D<T>& iB);
template <typename T> io::XmlOStream& operator<<(io::XmlOStream& ioOStream, const Sphere3D<T>& iB);

}

}

#include "sphere_3d.inl"

#ifdef LASS_GUARDIAN_OF_INCLUSION_PRIM_AABB_3D_H
#	include "aabb_3d_sphere_3d.h"
#endif

#ifdef LASS_GUARDIAN_OF_INCLUSION_PRIM_RAY_3D_H
#include "ray_3d_sphere_3d.h"
#endif

#endif

// --- END OF FILE ------------------------------------------------------------------------------
