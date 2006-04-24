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



/** @struct lass::prim::Capsule3D
 *  @brief 3D Capsule
 *  @author Tom Willems
 *  @date 2002-2003
 */



#ifndef LASS_GUARDIAN_OF_INCLUDION_PRIM_CAPSULE_3D_H
#define LASS_GUARDIAN_OF_INCLUDION_PRIM_CAPSULE_3D_H




// --- OLD INTERFACES ---------------------------------------------------------------------------

#include "prim_common.h"
#include "line_segment_3d.h"



// --- NEW INTERFACES ---------------------------------------------------------------------------

namespace lass
{

namespace prim
{

template<
	typename T,
	class ParameterPolicy = Bounded
>
class Capsule3D
{
public:

	typedef Capsule3D<T, ParameterPolicy> TSelf;
	typedef ParameterPolicy TParameterPolicy;

	typedef LineSegment3D<T,ParameterPolicy> TLineSegment;
	typedef typename TLineSegment::TPoint TPoint;
	typedef typename TPoint::TVector TVector;

	typedef typename TPoint::TValue TValue;
	typedef typename TPoint::TParam TParam;
	typedef typename TPoint::TReference TReference;
	typedef typename TPoint::TConstReference TConstReference;
	typedef typename TPoint::TNumTraits TNumTraits;

	enum { dimension = TPoint::dimension }; /**< number of dimensions */

	template <typename U> struct Rebind
	{
		typedef Capsule3D<U, ParameterPolicy> Type;
	};

	Capsule3D();
	Capsule3D(const TLineSegment& iAxis, TParam iRadius);

	const TLineSegment& axis() const;
	TLineSegment& axis();

	TConstReference radius() const;
	TReference radius();

	const TValue area() const;
	const TValue volume() const;

	const bool isValid() const;
	const bool contains(const TPoint& iPoint) const;

private:

	TLineSegment	axis_;		 /**< axis of capsule */
	TValue			radius_;     /**< radius of capsule */

};



template<typename T> std::ostream& operator<<(std::ostream& oOStream, const Capsule3D<T>& iB);

template <typename T>
io::XmlOStream& operator<<(io::XmlOStream& ioOStream, const Capsule3D<T>& iB);



}

}

#include "capsule_3d.inl"

#ifdef LASS_GUARDIAN_OF_INCLUSION_PRIM_AABB_3D_H
#	include "aabb_3d_capsule_3d.h"
#endif

#ifdef LASS_GUARDIAN_OF_INCLUSION_PRIM_RAY_3D_H
#include "capsule_3d_ray_3d.h"
#endif

#endif

// --- END OF FILE ------------------------------------------------------------------------------

