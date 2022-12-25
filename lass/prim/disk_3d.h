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
 *	Copyright (C) 2004-2022 the Initial Developer.
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



/** @struct lass::prim::Disk3D
 *  @brief 3D Disk
 *  @author Bram de Greve
 *  @date 2010
 */

#ifndef LASS_GUARDIAN_OF_INCLUSION_PRIM_DISK_3D_H
#define LASS_GUARDIAN_OF_INCLUSION_PRIM_DISK_3D_H

#include "prim_common.h"
#include "disk_3d.h"
#include "plane_3d.h"

namespace lass
{

namespace prim
{

template<typename T>
class Disk3D
{
public:

	typedef Disk3D<T> TSelf;

	typedef Point3D<T> TPoint;
	typedef typename TPoint::TVector TVector;

	typedef typename TPoint::TValue TValue;
	typedef typename TPoint::TParam TParam;
	typedef typename TPoint::TReference TReference;
	typedef typename TPoint::TConstReference TConstReference;
	typedef typename TPoint::TNumTraits TNumTraits;
	typedef Plane3D<T, Combined, Normalized> TPlane;
	typedef typename TPlane::TUV TUV;

	enum { dimension = TPoint::dimension }; /**< number of dimensions */

	template <typename U> struct Rebind
	{
		typedef Disk3D<U> Type;
	};

	Disk3D();
	Disk3D(const TPoint& center, const TVector& normal, TParam radius);

	const TPoint& center() const;
	void setCenter(const TPoint& center);

	const TVector& normal() const;
	void setNormal(const TVector& normal);

	TConstReference radius() const;
	TReference radius();

	const TPlane& plane() const;
	const TValue area() const;

	const TPoint point(TParam u, TParam v) const;
	const TPoint point(const TUV& uv) const;
	const TUV uv(const TPoint& point) const;

	bool isValid() const;

private:

	TPlane plane_;
	TValue radius_;
};

template<typename T> std::ostream& operator<<(std::ostream& stream, const Disk3D<T>& disk);
template <typename T> io::XmlOStream& operator<<(io::XmlOStream& stream, const Disk3D<T>& disk);

}

}

#include "disk_3d.inl"

#ifdef LASS_GUARDIAN_OF_INCLUSION_PRIM_AABB_3D_H
#	include "aabb_3d_disk_3d.h"
#endif

#ifdef LASS_GUARDIAN_OF_INCLUSION_PRIM_SPHERE_3D_H
#	include "disk_3d_sphere_3d.h"
#endif

#ifdef LASS_GUARDIAN_OF_INCLUSION_PRIM_RAY_3D_H
#include "disk_3d_ray_3d.h"
#endif

#endif

// --- END OF FILE ------------------------------------------------------------------------------
