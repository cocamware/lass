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



#ifndef LASS_GUARDIAN_OF_INCLUSION_PRIM_LINE_SEGMENT_3D_INL
#define LASS_GUARDIAN_OF_INCLUSION_PRIM_LINE_SEGMENT_3D_INL




#include "line_segment_3d.h"



namespace lass
{

namespace prim
{

template <typename T, class PP>
LineSegment3D<T, PP>::LineSegment3D():
	tail_(),
	head_()
{
	LASS_ASSERT(tail_.isZero());
	LASS_ASSERT(head_.isZero());
}



template <typename T, class PP>
LineSegment3D<T, PP>::LineSegment3D(const TPoint& iTail, const TPoint& iHead):
	tail_(iTail),
	head_(iHead)
{
}



template <typename T, class PP> inline
const typename LineSegment3D<T, PP>::TPoint& 
LineSegment3D<T, PP>::tail() const
{
	return tail_;
}



template <typename T, class PP> inline
typename LineSegment3D<T, PP>::TPoint& 
LineSegment3D<T, PP>::tail()
{
	return tail_;
}



template <typename T, class PP> inline
const typename LineSegment3D<T, PP>::TPoint& 
LineSegment3D<T, PP>::head() const
{
    return head_;
}



template <typename T, class PP> inline
typename LineSegment3D<T, PP>::TPoint& 
LineSegment3D<T, PP>::head()
{
    return head_;
}



/** Return point on ray by it's parameter.
 *  @return origin + t * direction
 */
template <typename T, class PP>
const typename LineSegment3D<T, PP>::TPoint 
LineSegment3D<T, PP>::point(TParam iT) const
{
	LASS_PRIM_ENFORCE_RANGE(PP, iT >= TNumTraits::zero && iT <= TNumTraits::one);
    return tail_ + iT * vector();
}



/** Return parameter of @e projection of @a iPoint on line segment.
 *  @warning the result can be out of bound [0, 1] regardless the parameter policy used.
 */
template <typename T, class PP>
const typename LineSegment3D<T, PP>::TValue 
LineSegment3D<T, PP>::t(const TPoint& iPoint) const
{
	const TVector v = vector();
	const TValue t1 =  dot(iPoint - tail_, v);
	const TValue t2 = -dot(iPoint - head_, v);
	const TValue t = std::max(t1,t2) / (t1 + t2);
	return t1 > t2 ? t : TNumTraits::one - t; 
}



/** Return vector from tail to head.
 */
template <typename T, class PP>
const typename LineSegment3D<T, PP>::TVector 
LineSegment3D<T, PP>::vector() const
{
    return head_ - tail_;
}



/** Return length of line segment.
 */
template <typename T, class PP>
const typename LineSegment3D<T, PP>::TValue 
LineSegment3D<T, PP>::length() const
{
	const TVector v = vector();
    return v.norm();
}



/** @relates lass::prim::LineSegment3D
 */
template<typename T, class PP> 
std::ostream& operator<<(std::ostream& ioOStream, const LineSegment3D<T, PP>& iLineSegment)
{
	LASS_ENFORCE_STREAM(ioOStream) << "{T=" << iLineSegment.tail() << ", H=" << iLineSegment.head() << "}";
	return ioOStream;
}



/** @relates lass::prim::LineSegment3D
 */
template<typename T, class PP> 
io::XmlOStream& operator<<(io::XmlOStream& ioOStream, const LineSegment3D<T, PP>& iLineSegment)
{
	LASS_ENFORCE_STREAM(ioOStream) 
		<< "<LineSegment3D>\n"
		<< "<tail>" << iLineSegment.tail() << "</tail>\n"
		<< "<head>" << iLineSegment.head() << "</head>\n"
		<< "</LineSegment3D>\n";

	return ioOStream;
}



}

}

#endif
