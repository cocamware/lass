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



/** @class lass::prim::Parallelogram3D
 *  @brief A very simple 3D polygon :)
 *  @author Bram de Greve [BdG]
 */

#ifndef LASS_GUARDIAN_OF_INCLUSION_PRIM_PARALLELOGRAM_3D_H
#define LASS_GUARDIAN_OF_INCLUSION_PRIM_PARALLELOGRAM_3D_H

#include "prim_common.h"
#include "line_segment_3d.h"
#include "plane_3d.h"

namespace lass
{
namespace prim
{

template <typename T>
class LASS_SIMD_ALIGN Parallelogram3D
{
public:

	typedef Parallelogram3D<T> TSelf;

	typedef Point3D<T> TPoint;
	typedef Point3DH<T> TPointH;
	typedef typename TPoint::TVector TVector;
	typedef Point2D<T> TUV;
	typedef Plane3D<T, Cartesian, Normalized> TPlane;

	typedef typename TPoint::TValue TValue;
	typedef typename TPoint::TParam TParam;
	typedef typename TPoint::TReference TReference;
	typedef typename TPoint::TConstReference TConstReference;
	typedef typename TPoint::TNumTraits TNumTraits;

	enum { dimension = TPoint::dimension }; /**< number of dimensions */

	template <typename U> struct Rebind
	{
		typedef Parallelogram3D<U> Type;
	};

	Parallelogram3D();
	Parallelogram3D(const TPoint& support, const TVector& sizeX, const TVector& sizeY);

	const TPoint& support() const;
	TPoint& support();
	
	const TVector& sizeU() const;
	TVector& sizeU();
	
	const TVector& sizeV() const;
	TVector& sizeV();

	const TPlane plane() const;

	const TValue squaredArea() const;
	const TValue area() const;
	const TValue perimeter() const;

	const TPoint point(TParam u, TParam v) const;
	const TPoint point(const TUV& uv) const;
	const TUV uv(const TPoint& point) const;

	bool isSimple() const;
	bool isConvex() const;
	bool isReflex(int indexOfVertex) const;

private:

	TPoint support_;
	TVector sizeU_;
	TVector sizeV_;
};

template <typename T>
io::XmlOStream& operator<<(io::XmlOStream& stream, const Parallelogram3D<T>& parallelogram);
template <typename T>
std::ostream& operator<<(std::ostream& stream, const Parallelogram3D<T>& parallelogram);



}

}

#include "parallelogram_3d.inl"

#ifdef LASS_GUARDIAN_OF_INCLUSION_PRIM_AABB_3D_H
#	include "aabb_3d_parallelogram_3d.h"
#endif

#ifdef LASS_GUARDIAN_OF_INCLUSION_PRIM_RAY_3D_H
#	include "parallelogram_3d_ray_3d.h"
#endif

#endif

// EOF
