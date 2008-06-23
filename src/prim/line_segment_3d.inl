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



template <typename T, class PP>
template <typename PP2>
LineSegment3D<T, PP>::LineSegment3D(const LineSegment3D<T, PP2>& iOther):
	tail_(iOther.tail()),
	head_(iOther.head())
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
	TParameterPolicy::enforceRange(iT, TNumTraits::zero, TNumTraits::one);
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
/** t is parameter of closests point and return squared distance of a point to the line segment. 
*/
template <typename T, class PP>
const typename LineSegment3D<T, PP>::TValue
LineSegment3D<T, PP>::closestsPoint(const TPoint &iPoint, T &oT) const
{
	oT = this->t(iPoint);
	if(oT < 0)
	{
		oT = 0.0;
		return (iPoint - tail_).squaredNorm();
	}
	if(oT > 1)
	{
		oT = 1.0;
		return (iPoint - head_).squaredNorm();
	}
	return (iPoint - this->point(oT)).squaredNorm();
}

/** Return squared distance of a point to line segment.
 */
template <typename T, class PP>
const typename LineSegment3D<T, PP>::TValue
LineSegment3D<T, PP>::squaredDistance(const TPoint& iPoint) const
{
	TParam t = this->t(iPoint);
	if(t < 0)
		return (iPoint - tail_).squaredNorm();
	if(t > 1)
		return (iPoint - head_).squaredNorm();
	return (iPoint - this->point(t)).squaredNorm();
}

/** Return distance of point to line segment.
 */
template <typename T, class PP>
const typename LineSegment3D<T, PP>::TValue
LineSegment3D<T, PP>::distance(const TPoint& iPoint) const
{
	TValue squaredDistance = this->squaredDistance(iPoint);
	return lass::num::sqrt(squaredDistance);
}

/** @relates lass::prim::LineSegment3D
 */
template <typename T, class PPa, class PPb> bool operator==(const LineSegment3D<T, PPa>& iA, const LineSegment3D<T, PPb>& iB)
{
	return iA.tail()==iB.tail() && iA.head()==iB.head();
}

/** @relates lass::prim::LineSegment3D
 */
template <typename T, class PPa, class PPb> bool operator!=(const LineSegment3D<T, PPa>& iA, const LineSegment3D<T, PPb>& iB)
{
	return !(iA==iB);
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
