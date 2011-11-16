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



#ifndef LASS_GUARDIAN_OF_INCLUSION_PRIM_DISK_3D_INL
#define LASS_GUARDIAN_OF_INCLUSION_PRIM_DISK_3D_INL


#include "disk_3d.h"
#include "../num/floating_point_comparison.h"

namespace lass
{
namespace prim
{

// --- public --------------------------------------------------------------------------------------

template <typename T>
Disk3D<T>::Disk3D():
	plane_(),
	radius_(TNumTraits::zero)
{
}



template <typename T>
Disk3D<T>::Disk3D(const TPoint& center, const TVector& normal, TParam radius):
	plane_(normal, center),
	radius_(radius)
{
}



template <typename T>
const typename Disk3D<T>::TPoint&
Disk3D<T>::center() const
{
	return plane_.support();
}



template <typename T>
void Disk3D<T>::setCenter(const TPoint& center)
{
	plane_ = TPlane(plane_.normal(), center);
}



template <typename T>
const typename Disk3D<T>::TVector&
Disk3D<T>::normal() const
{
	return plane_.normal();
}



template <typename T>
void Disk3D<T>::setNormal(const TVector& normal)
{
	plane_ = TPlane(normal, plane_.support());
}



template<typename T>
typename Disk3D<T>::TConstReference
Disk3D<T>::radius() const
{
	return radius_;
}



template<typename T>
typename Disk3D<T>::TReference
Disk3D<T>::radius()
{
	return radius_;
}



/** return area of surface of sphere
 */
template<typename T>
const typename Disk3D<T>::TValue
Disk3D<T>::area() const
{
	return TNumTraits::pi * num::sqr(radius_);
}



/** return area of surface of sphere
 */
template<typename T>
const typename Disk3D<T>::TPlane&
Disk3D<T>::plane() const
{
	return plane_;
}



template <typename T>
const typename Disk3D<T>::TPoint 
Disk3D<T>::point(TParam u, TParam v) const
{
	const TValue theta = 2 * TNumTraits::pi * v;
	return plane_.support() + (u * radius_) * (num::cos(theta) * plane_.directionU() + num::sin(theta) * plane_.directionV());
}



template <typename T> inline
const typename Disk3D<T>::TPoint
Disk3D<T>::point(const TUV& uv) const
{
	return point(uv.x, uv.y);
}



template <typename T>
const typename Disk3D<T>::TUV
Disk3D<T>::uv(const TPoint& point) const
{
	const TVector relative = point - plane_.support();
	const TValue x = dot(relative, plane_.reciprocalU());
	const TValue y = dot(relative, plane_.reciprocalU());
	const TValue u = relative.norm() / radius_;
	TValue v = num::atan2(y, x) / (2 * TNumTraits::pi);
	if (v < 0)
	{
		v += 1;
	}
	return TUV(u, v < 0 ? TNumTraits::one - v : v);
}



/** return true if sphere has a non-negative radius
 */
template <typename T>
bool Disk3D<T>::isValid() const
{
	return plane.isValid() && radius_ >= TNumTraits::zero;
}



// --- protected -----------------------------------------------------------------------------------



// --- private -------------------------------------------------------------------------------------



// --- free ----------------------------------------------------------------------------------------

/** @relates lass::prim::Disk3D
 */
template<typename T>
std::ostream& operator<<(std::ostream& stream, const Disk3D<T>& disk)
{
	LASS_ENFORCE(stream) << "{S=" << disk.center() << ", N=" << disk.normal() << ", r=" << disk.radius() << "}";
	return stream;
}



/** @relates lass::prim::Disk3D
 */
template<typename T>
io::XmlOStream& operator<<(io::XmlOStream& stream, const Disk3D<T>& disk)
{
	LASS_ENFORCE_STREAM(stream)
		<< "<Disk3D>\n"
		<< "<center>" << disk.center() << "</center>\n"
		<< "<normal>" << disk.normal() << "</normal>\n"
		<< "<radius>" << disk.radius() << "</radius>\n"
		<< "</Disk3D>\n";

	return stream;
}



}

}

#endif

// --- END OF FILE ------------------------------------------------------------------------------
