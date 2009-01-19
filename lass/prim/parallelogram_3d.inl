/**	@file
 *	@author Bram de Greve (bramz@users.sourceforge.net)
 *	@author Tom De Muer (tomdemuer@users.sourceforge.net)
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
 *	Copyright (C) 2004-2007 the Initial Developer.
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



template <typename T> inline
const typename Parallelogram3D<T>::TPoint
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
