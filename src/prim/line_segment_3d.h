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


 */


/** @class lass::prim::LineSegment3D
 *  @brief 3D Line Segment
 *  @author Bram de Greve [BdG]
 *  @date 3003
 */



#ifndef LASS_GUARDIAN_OF_INCLUSION_PRIM_LINE_SEGMENT_3D_H
#define LASS_GUARDIAN_OF_INCLUSION_PRIM_LINE_SEGMENT_3D_H
#pragma once



#include "prim_common.h"
#include "parameter_policy.h"
#include "point_3d.h"
#include "../io/matlab_o_stream.h"



namespace lass
{

namespace prim
{

template
<
	typename T, 
	class ParameterPolicy = Bounded
>
class LineSegment3D
{
public:

    typedef LineSegment3D<T, ParameterPolicy> TSelf;
    typedef ParameterPolicy TParameterPolicy;

    typedef Point3D<T> TPoint;
    typedef typename TPoint::TVector TVector;
    typedef typename TPoint::TNumTraits TNumTraits;
    typedef typename TPoint::TValue TValue;
    typedef typename TPoint::TParam TParam;
    typedef typename TPoint::TReference TReference;
    typedef typename TPoint::TConstReference TConstReference;

    enum 
    {
        dimension = TPoint::dimension
    };

    template <typename U> struct Rebind
    {
        typedef LineSegment3D<U, ParameterPolicy> Type;
    };

    LineSegment3D();
    LineSegment3D(const TPoint& iTail, const TPoint& iHead);

    const TPoint& tail() const;
    TPoint& tail();
    const TPoint& head() const;
    TPoint& head();

	const TPoint point(TParam iT) const;
    const TValue t(const TPoint& iPoint) const;
	const TVector vector() const;
	const TValue length() const;

private:

    TPoint tail_;
    TPoint head_;
};

template <typename T, class PPa, class PPb>
Result intersect(const LineSegment3D<T, PPa>& iA, const LineSegment3D<T, PPb>& iB, 
                 T& oTa, T& oTb);

template <typename T, class PPa, class PPb>
Result intersect(const LineSegment3D<T, PPa>& iA, const LineSegment3D<T, PPb>& iB, 
                 Point3D<T>& oPoint);

template<typename T, class PP> 
std::ostream& operator<<(std::ostream& oOStream, const LineSegment3D<T, PP>& iB);

template<typename T, class PP> 
io::XmlOStream& operator<<(io::XmlOStream& oOStream, const LineSegment3D<T, PP>& iB);

template<typename T, class PP> 
lass::io::MatlabOStream& operator<<(lass::io::MatlabOStream& oOStream, 
                                    const LineSegment3D<T, PP>& iLineSegment);


}

}

#include "line_segment_3d.inl"

#define LASS_PRIM_PYTHON_EXTENSION_LINE_SEGMENT_3D
#ifdef LASS_GUARDIAN_OF_INCLUSION_PRIM_PYOBJECT_UTIL_H
#	include "pyobject_util.h"
#endif

#endif
