/**	@file
 *	@author Bram de Greve (bramz@users.sourceforge.net)
 *	@author Tom De Muer (tomdemuer@users.sourceforge.net)
 *
 *	Distributed under the terms of the GPL (GNU Public License)
 *
 * 	The LASS License:
 *
 *	Copyright 2004 Bram de Greve and Tom De Muer
 *
 *	LASS is free software; you can redistribute it and/or modify
 *	it under the terms of the GNU General Public License as published by
 *	the Free Software Foundation; either version 2 of the License, or
 *	(at your option) any later version.
 *
 *	This program is distributed in the hope that it will be useful,
 *	but WITHOUT ANY WARRANTY; without even the implied warranty of
 *	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *	GNU General Public License for more details.
 *
 *	You should have received a copy of the GNU General Public License
 *	along with this program; if not, write to the Free Software
 *	Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */



/** @class lass::prim::Line3D
 *  @brief 3D Line
 *  @author Bram de Greve [BdG]
 *  @date 3003
 */



#ifndef LASS_GUARDIAN_OF_INCLUSION_PRIM_LINE_3D_H
#define LASS_GUARDIAN_OF_INCLUSION_PRIM_LINE_3D_H




#include "prim_common.h"
#include "normalizing_policy.h"
#include "point_3d.h"



namespace lass
{

namespace prim
{

template
<
	typename T, 
	class NormalizingPolicy = Normalized
>
class Line3D
{
public:

    typedef Line3D<T, NormalizingPolicy> TSelf;
    typedef NormalizingPolicy TNormalizingPolicy;

    typedef Point3D<T> TPoint;
    typedef typename TPoint::TVector TVector;

    typedef typename TPoint::TValue TValue;
    typedef typename TPoint::TParam TParam;
    typedef typename TPoint::TReference TReference;
    typedef typename TPoint::TConstReference TConstReference;
    typedef typename TPoint::TNumTraits TNumTraits;

    enum { dimension = TPoint::dimension };	/**< number of dimensions of vector */

    /** rebind to other value type
     */
    template <typename U>
    struct Rebind
    {
        typedef Line3D<U, NormalizingPolicy> Type;
    };

	Line3D();
	Line3D(const TPoint& iSupport, const TPoint& iPoint);
	Line3D(const TPoint& iSupport, const TVector& iDirection);

	const TPoint& support() const;
	TPoint& support();
    const TVector& direction() const;
    void setDirection(const TVector& iDirection);
    void lookAt(const TPoint& iLookAt);

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

    TPoint support_;
    TVector direction_;
};



template<typename T, class NP> 
std::ostream& operator<<(std::ostream& oOStream, const Line3D<T, NP>& iB);

template<typename T, class NP> 
io::XmlOStream& operator<<(io::XmlOStream& oOStream, const Line3D<T, NP>& iB);


}

}

#include "line_3d.inl"

#endif
