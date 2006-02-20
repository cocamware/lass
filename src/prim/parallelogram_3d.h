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
class Parallelogram3D
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
	Parallelogram3D(const TPoint& iSupport, const TVector& iSizeX, const TVector& iSizeY);

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

	const TPoint point(TParam iU, TParam iV) const;
	const TPoint point(const TUV& iUV) const;
	const TUV uv(const TPoint& iPoint) const;

	const bool isSimple() const;
	const bool isConvex() const;
	const bool isReflex(int iIndexOfVertex) const;

private:

	TPoint support_;
	TVector sizeU_;
	TVector sizeV_;
};

template <typename T>
io::XmlOStream& operator<<(io::XmlOStream& ioOStream, const Parallelogram3D<T>& iParallelogram);
template <typename T>
std::ostream& operator<<(std::ostream& ioOStream, const Parallelogram3D<T>& iParallelogram);



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
