/** 
*   @author Bram de Greve (bramz@users.sourceforge.net)
*   @author Tom De Muer (tomdemuer@users.sourceforge.net)
*
*	Distributed under the terms of the GPL (GNU Public License)
*
* 	The LASS License:
*   
*	Copyright 2004 Bram de Greve and Tom De Muer
*
*   LASS is free software; you can redistribute it and/or modify
*   it under the terms of the GNU General Public License as published by
*   the Free Software Foundation; either version 2 of the License, or
*   (at your option) any later version.
*
*   This program is distributed in the hope that it will be useful,
*   but WITHOUT ANY WARRANTY; without even the implied warranty of
*   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
*   GNU General Public License for more details.
*
*   You should have received a copy of the GNU General Public License
*   along with this program; if not, write to the Free Software
*   Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
*


 *  @date 2003
 */

/** @class lass::prim::impl::Line2DCartesian
 *  @brief implementation of 2d line with both cartesian and parametric equation.
 *  @author Bram de Greve [BdG]
 */



#ifndef LASS_GUARDIAN_OF_INCLUSION_PRIM_IMPL_LINE_2D_CARTESIAN_H
#define LASS_GUARDIAN_OF_INCLUSION_PRIM_IMPL_LINE_2D_CARTESIAN_H

#include "../prim_common.h"

namespace lass
{
namespace prim
{
namespace impl
{

template <typename T, class NormalizingPolicy = Normalized>
class Line2DCartesian
{
public:

    typedef NormalizingPolicy TNormalizingPolicy;

    typedef typename Point2D<T> TPoint;
    typedef typename TPoint::TVector TVector;

    typedef typename TPoint::TValue TValue;
    typedef typename TPoint::TParam TParam;
    typedef typename TPoint::TReference TReference;
    typedef typename TPoint::TConstReference TConstReference;
    typedef typename TPoint::TNumTraits TNumTraits;

    enum { dimension = TPoint::dimension };	/**< number of dimensions of vector */

	Line2DCartesian();
	Line2DCartesian(const TPoint& iSupport, const TPoint& iPoint);
	Line2DCartesian(const TPoint& iSupport, const TVector& iDirection);
	Line2DCartesian(const TVector& iNormal, const TPoint& iSupport);
	Line2DCartesian(const TVector& iNormal, TParam iD);

	const TPoint support() const;
    const TVector direction() const;

    void getCartesian(TVector& oNormal, TReference oD) const;
    const TVector& normal() const;
	const TParam d() const;

    const Side classify(const TPoint& iPoint) const;
    const TValue equation(const TPoint& iPoint) const;
    const TValue signedDistance(const TPoint& iPoint) const;

	const TVector reject(const TPoint& iPoint) const;
	const TVector reject(const TVector& iVector) const;
	const TPoint project(const TPoint& iPoint) const;
	const TVector project(const TVector& iVector) const;
	const TPoint reflect(const TPoint& iPoint) const;
	const TVector reflect(const TVector& iVector) const;

	const TPoint point(TParam iT) const;
    const TValue t(const TPoint& iPoint) const;

    void flip();
    const bool isValid() const;

private:

    TVector normal_;
	TValue d_;
};



}

}

}

#include "line_2d_cartesian.inl"

#endif

// EOF
