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
Parallelogram3D<T>::Parallelogram3D(const TPoint& iSupport, const TVector& iSizeU, 
									const TVector& iSizeV):
	support_(iSupport),
	sizeU_(iSizeU),
	sizeV_(iSizeV)
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
Parallelogram3D<T>::point(TParam iU, TParam iV) const
{
	return support_ + iU * sizeU_ + iV * sizeV_;
}



template <typename T>
const typename Parallelogram3D<T>::TPoint inline
Parallelogram3D<T>::point(const TUV& iUv) const
{
	return point(iUv.x, iUv.y);
}



template <typename T>
const typename Parallelogram3D<T>::TUV
Parallelogram3D<T>::uv(const TPoint& iPoint) const
{
	TVector reciprocalU;
	TVector reciprocalV;
	impl::Plane3DImplDetail::generateReciprocal(sizeU_, sizeV_, reciprocalU, reciprocalV);
	const TVector relative = iPoint - support_;
	return TUV(dot(relative, reciprocalU), dot(relative, reciprocalV));
}



/** @copydoc SimplePolygon3D::isSimple
 *  @par Parallelogram specific:
 *      A parallelogram is always simple
 */
template <typename T>
const bool Parallelogram3D<T>::isSimple() const
{
	return true;
}



/** @copydoc SimplePolygon3D::isConvex
 *  @par Parallelogram specific:
 *      A parallelogram is always convex
 */
template <typename T>
const bool Parallelogram3D<T>::isConvex() const
{
	return true;
}



/** @copydoc SimplePolygon3D::isReflex
 *  @par Parallelogram specific:
 *		A parallelogram never has reflex vertices
 */
template <typename T>
const bool Parallelogram3D<T>::isReflex(int iIndexOfVertex) const
{
	return false;
}



// --- private -------------------------------------------------------------------------------------



// --- free ----------------------------------------------------------------------------------------

/** @relates lass::prim::Parallelogram3D
 */
template <typename T>
io::XmlOStream& operator<<(io::XmlOStream& ioOStream, const Parallelogram3D<T>& iParallelogram)
{
	LASS_ENFORCE_STREAM(ioOStream) << "<Parallelogram3D>\n";
	ioOStream << "<support>" << iParallelogram.support() << "</support>\n";
	ioOStream << "<sizeU>" << iParallelogram.sizeU() << "</sizeU>\n";
	ioOStream << "<sizeV>" << iParallelogram.sizeV() << "</sizeV>\n";
	ioOStream << "</Parallelogram3D>\n";
	return ioOStream;
}



/** @relates lass::prim::Parallelogram3D
 */
template <typename T>
std::ostream& operator<<(std::ostream& ioOStream, const Parallelogram3D<T>& iParallelogram)
{
	LASS_ENFORCE_STREAM(ioOStream) 
		<< "{S=" << iParallelogram.support() << ", U=" << iParallelogram.sizeU() 
		<< ", V=" << iParallelogram.sizeV() << "}";
	return ioOStream;
}



}

}

#endif

// EOF
