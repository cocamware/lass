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
 *	Copyright (C) 2004-2010 the Initial Developer.
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



#ifndef LASS_GUARDIAN_OF_INCLUSION_PRIM_SIMPLE_PARALLELOGRAM_3D_INL
#define LASS_GUARDIAN_OF_INCLUSION_PRIM_SIMPLE_PARALLELOGRAM_3D_INL

#include "prim_common.h"
#include "parallelogram_3d.h"
#include "impl/plane_3d_impl_detail.h"

namespace lass
{
namespace prim
{

/** constructs an empty parallelogram.
 *  support is origin and sizes are zero.
 */
template <typename T>
Parallelogram3D<T>::Parallelogram3D()
{
}



/** Constructs a parallelogram with a support and two sizes
 */
template <typename T>
Parallelogram3D<T>::Parallelogram3D(const TPoint& support, const TVector& sizeU, const TVector& sizeV):
	support_(support),
	sizeU_(sizeU),
	sizeV_(sizeV)
{
}



template <typename T> inline
const typename Parallelogram3D<T>::TPoint&
Parallelogram3D<T>::support() const
{
	return support_;
}



template <typename T> inline
typename Parallelogram3D<T>::TPoint&
Parallelogram3D<T>::support()
{
	return support_;
}



template <typename T> inline
const typename Parallelogram3D<T>::TVector&
Parallelogram3D<T>::sizeU() const
{
	return sizeU_;
}



template <typename T> inline
typename Parallelogram3D<T>::TVector&
Parallelogram3D<T>::sizeU()
{
	return sizeU_;
}



template <typename T> inline
const typename Parallelogram3D<T>::TVector&
Parallelogram3D<T>::sizeV() const
{
	return sizeV_;
}



template <typename T> inline
typename Parallelogram3D<T>::TVector&
Parallelogram3D<T>::sizeV()
{
	return sizeV_;
}



template <typename T>
const typename Parallelogram3D<T>::TPlane
Parallelogram3D<T>::plane() const
{
	return TPlane(cross(sizeU_, sizeV_), support_);
}



/** returns squared area of parallelogram to avoid the square root.
 *  @return num::sqr(area()) but faster :)
 */
template <typename T>
const typename Parallelogram3D<T>::TValue
Parallelogram3D<T>::squaredArea() const
{
	return cross(sizeU_, sizeV_).squaredNorm();
}



/** @copydoc SimplePolygon3D::area
 */
template <typename T>
const typename Parallelogram3D<T>::TValue
Parallelogram3D<T>::area() const
{
	return num::sqrt(squaredArea());
}



/** @copydoc SimplePolygon3D::perimeter
 */
template <typename T>
const typename Parallelogram3D<T>::TValue
Parallelogram3D<T>::perimeter() const
{
	return 2 * (sizeU_.norm() + sizeV_.norm());
}



template <typename T>
const typename Parallelogram3D<T>::TPoint 
Parallelogram3D<T>::point(TParam u, TParam v) const
{
	return support_ + u * sizeU_ + v * sizeV_;
}



template <typename T> inline
const typename Parallelogram3D<T>::TPoint
Parallelogram3D<T>::point(const TUV& uv) const
{
	return point(uv.x, uv.y);
}



template <typename T>
const typename Parallelogram3D<T>::TUV
Parallelogram3D<T>::uv(const TPoint& point) const
{
	TVector reciprocalU;
	TVector reciprocalV;
	impl::Plane3DImplDetail::generateReciprocal(sizeU_, sizeV_, reciprocalU, reciprocalV);
	const TVector relative = point - support_;
	return TUV(dot(relative, reciprocalU), dot(relative, reciprocalV));
}



/** @copydoc SimplePolygon3D::isSimple
 *  @par Parallelogram specific:
 *      A parallelogram is always simple
 */
template <typename T>
bool Parallelogram3D<T>::isSimple() const
{
	return true;
}



/** @copydoc SimplePolygon3D::isConvex
 *  @par Parallelogram specific:
 *      A parallelogram is always convex
 */
template <typename T>
bool Parallelogram3D<T>::isConvex() const
{
	return true;
}



/** @copydoc SimplePolygon3D::isReflex
 *  @par Parallelogram specific:
 *		A parallelogram never has reflex vertices
 */
template <typename T>
bool Parallelogram3D<T>::isReflex(int) const
{
	return false;
}



// --- private -------------------------------------------------------------------------------------



// --- free ----------------------------------------------------------------------------------------

/** @relates lass::prim::Parallelogram3D
 */
template <typename T>
io::XmlOStream& operator<<(io::XmlOStream& stream, const Parallelogram3D<T>& parallelogram)
{
	LASS_ENFORCE_STREAM(stream) << "<Parallelogram3D>\n";
	stream << "<support>" << parallelogram.support() << "</support>\n";
	stream << "<sizeU>" << parallelogram.sizeU() << "</sizeU>\n";
	stream << "<sizeV>" << parallelogram.sizeV() << "</sizeV>\n";
	stream << "</Parallelogram3D>\n";
	return stream;
}



/** @relates lass::prim::Parallelogram3D
 */
template <typename T>
std::ostream& operator<<(std::ostream& stream, const Parallelogram3D<T>& parallelogram)
{
	LASS_ENFORCE_STREAM(stream) 
		<< "{S=" << parallelogram.support() << ", U=" << parallelogram.sizeU() 
		<< ", V=" << parallelogram.sizeV() << "}";
	return stream;
}



}

}

#endif

// EOF
