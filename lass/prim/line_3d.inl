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



#ifndef LASS_GUARDIAN_OF_INCLUSION_PRIM_LINE_3D_INL
#define LASS_GUARDIAN_OF_INCLUSION_PRIM_LINE_3D_INL




#include "line_3d.h"



namespace lass
{

namespace prim
{

template <typename T, class NP>
Line3D<T, NP>::Line3D():
	support_(),
	direction_()
{
	LASS_ASSERT(support_.isZero());
	LASS_ASSERT(direction_.isZero());
}



template <typename T, class NP>
Line3D<T, NP>::Line3D(const TPoint& iSupport, const TVector& iDirection):
	support_(iSupport),
	direction_(iDirection)
{
	NP::normalize(direction_);
}



template <typename T, class NP>
Line3D<T, NP>::Line3D(const TPoint& iSupport, const TPoint& iLookAt):
	support_(iSupport),
	direction_(iLookAt - iSupport)
{
	NP::normalize(direction_);
}



template <typename T, class NP> inline
const typename Line3D<T, NP>::TPoint&
Line3D<T, NP>::support() const
{
	return support_;
}



template <typename T, class NP> inline
typename Line3D<T, NP>::TPoint&
Line3D<T, NP>::support()
{
	return support_;
}



/** Return direction of line.
 */
template <typename T, class NP> inline
const typename Line3D<T, NP>::TVector&
Line3D<T, NP>::direction() const
{
	return direction_;
}



/** Set direction and normalize it if that's the policy
 */
template <typename T, class NP> inline
void Line3D<T, NP>::setDirection(const TVector& iDirection)
{
	direction_ = iDirection;
	NP::normalize(direction_);
}



/** Set direction from support to look-at point.
 */
template <typename T, class NP>
void Line3D<T, NP>::lookAt(const TPoint& iLookAt)
{
	direction_ = iLookAt - support_;
	NP::normalize(direction_);
}



/** return the vector that, if added to the @e projection of @a iPoint, you get @a iPoint again.
 *  <tt>iPoint == (almost) project(iPoint) + reject(iPoint)</tt>
 */
template<typename T, class NP>
const typename Line3D<T, NP>::TVector
Line3D<T, NP>::reject(const TPoint& iPoint) const
{
	return reject(iPoint - support_);
}



/** return the part of @a iVector that is orthogonal to the line.
 *  it's the vector that, if added to the @e projection of @a iVector, you get @a iVector again.
 *  <tt>iVector == (almost) project(iVector) + reject(iVector)</tt>.
 */
template<typename T, class NP>
const typename Line3D<T, NP>::TVector
Line3D<T, NP>::reject(const TVector& iVector) const
{

	return iVector - project(iVector);
}



/** project a point orthogonally onto the line
 */
template<typename T, class NP>
const typename Line3D<T, NP>::TPoint
Line3D<T, NP>::project(const TPoint& iPoint) const
{
	return support_ + project(iPoint - support_);
}



/** project a vector orthogonally onto the line
 */
template<typename T, class NP>
const typename Line3D<T, NP>::TVector
Line3D<T, NP>::project(const TVector& iVector) const
{
	return direction_ * NP::divideBySquaredNorm(dot(iVector, direction_), direction_);
}



/** reflect a point orthogonally into the line.
 */
template<typename T, class NP>
const typename Line3D<T, NP>::TPoint
Line3D<T, NP>::reflect(const TPoint& iPoint) const
{
	return support_ + reflect(iPoint - support_);
}



/** reflect a vector orthogonally to the line
 */
template<typename T, class NP>
const typename Line3D<T, NP>::TVector
Line3D<T, NP>::reflect(const TVector& iVector) const
{
	return T(2) * project(iVector) - iVector;
}



/** Return point on line by it's parameter.
 *  @return support + t * direction
 */
template <typename T, class NP>
const typename Line3D<T, NP>::TPoint
Line3D<T, NP>::point(TParam iT) const
{
	return support_ + iT * direction_;
}



/** Return parameter of point on the line that is the projection of the given point.
 */
template <typename T, class NP>
const typename Line3D<T, NP>::TValue
Line3D<T, NP>::t(const TPoint& iPoint) const
{
	return NP::divideBySquaredNorm(dot(iPoint - support_, direction_), direction_);
}



/** Return true if line is valid (direction isn't a zero vector).
 */
template <typename T, class NP>
bool Line3D<T, NP>::isValid() const
{
	return !direction_.isZero();
}



/** @relates lass::prim::Line3D
 */
template<typename T, class NP>
std::ostream& operator<<(std::ostream& ioOStream, const Line3D<T, NP>& iLine)
{
	LASS_ENFORCE(ioOStream) << "{S=" << iLine.support() << ", U=" << iLine.direction() << "}";
	return ioOStream;
}



/** @relates lass::prim::Line3D
 */
template<typename T, class NP>
io::XmlOStream& operator<<(io::XmlOStream& ioOStream, const Line3D<T, NP>& iLine)
{
	LASS_ENFORCE_STREAM(ioOStream)
		<< "<Line3D>\n"
		<< "<support>" << iLine.support() << "</support>\n"
		<< "<direction>" << iLine.direction() << "</direction>\n"
		<< "</Line3D>\n";

	return ioOStream;
}



}

}

#endif

// EOF
