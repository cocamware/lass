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
 *	Copyright (C) 2004-2009 the Initial Developer.
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



#ifndef LASS_GUARDIAN_OF_INCLUSION_PRIM_LINE_2D_INL
#define LASS_GUARDIAN_OF_INCLUSION_PRIM_LINE_2D_INL




#include "line_2d.h"



namespace lass
{

namespace prim
{

template<typename T, typename EP, typename NP>
Line2D<T, EP, NP>::Line2D():
	TImpl()
{
}




template<typename T, typename EP, typename NP>
Line2D<T, EP, NP>::Line2D(const TPoint& iSupport, const TPoint& iPoint):
	TImpl(iSupport, iPoint)
{
}



template<typename T, typename EP, typename NP>
Line2D<T, EP, NP>::Line2D(const TPoint& iSupport, const TVector& iDirection):
	TImpl(iSupport, iDirection)
{
}



template<typename T, typename EP, typename NP>
Line2D<T, EP, NP>::Line2D(const TVector& iNormal, const TPoint& iSupport):
	TImpl(iNormal, iSupport)
{
}



template<typename T, typename EP, typename NP>
Line2D<T, EP, NP>::Line2D(const TVector& iNormal, TParam iD):
	TImpl(iNormal, iD)
{
}



/** Return on what side a point is located.
 */
template<typename T, typename EP, typename NP>
const Side Line2D<T, EP, NP>::classify(const TPoint& iPoint) const
{
	const TValue eq = this->equation(iPoint);
	return eq > TNumTraits::zero ? sFront : (eq < TNumTraits::zero ? sBack : sSurface);
}



/** Return signed distance of point to line.
 *  negative value means point is in the back.
 */
template<typename T, typename EP, typename NP>
const typename Line2D<T, EP, NP>::TValue
Line2D<T, EP, NP>::signedDistance(const TPoint& iPoint) const
{
	return NP::divideByNorm(this->equation(iPoint), this->normal());
}



/** Return signed distance of point to line.
 *  negative value means point is in the back.
 */
template<typename T, typename EP, typename NP>
const typename Line2D<T, EP, NP>::TValue
Line2D<T, EP, NP>::squaredDistance(const TPoint& iPoint) const
{
	return num::sqr(this->signedDistance(iPoint));
}



/** Return on what side a point is located.
 */
template<typename T, typename EP, typename NP>
const Side Line2D<T, EP, NP>::classify(const TPoint& iPoint, TParam iRelativeTolerance) const
{
	const TValue eq = this->equation(iPoint, iRelativeTolerance);
	return eq > TNumTraits::zero ? sFront : (eq < TNumTraits::zero ? sBack : sSurface);
}



/** Return signed distance of point to line.
 *  negative value means point is in the back.
 */
template<typename T, typename EP, typename NP>
const typename Line2D<T, EP, NP>::TValue
Line2D<T, EP, NP>::signedDistance(const TPoint& iPoint, TParam iRelativeTolerance) const
{
	return NP::divideByNorm(this->equation(iPoint, iRelativeTolerance), this->normal());
}



/** Return signed distance of point to line.
 *  negative value means point is in the back.
 */
template<typename T, typename EP, typename NP>
const typename Line2D<T, EP, NP>::TValue
Line2D<T, EP, NP>::squaredDistance(const TPoint& iPoint, TParam iRelativeTolerance) const
{
	return num::sqr(this->signedDistance(iPoint, iRelativeTolerance));
}



// --- protected -----------------------------------------------------------------------------------



// --- private -------------------------------------------------------------------------------------



// --- free ----------------------------------------------------------------------------------------

/** absolute distance between point and line.
 *  @relates Line2D
 *  @param iA   point
 *  @param iB   line
 *  @return absolute distance.
 */
template <typename T, class EP, class NP>
T distance(const Point2D<T>& iA, const Line2D<T, EP, NP>& iB)
{
	return num::abs(iB.signedDistance(iA));
}



/** absolute distance between two lines
 *  @relates Line2D
 *  @param iA   line A
 *  @param iB   line B
 *  @return absolute distance.
 */
template <typename T, class EPa, class NPa, class EPb, class NPb>
T distance(const Line2D<T, EPa, NPa>& iA, const Line2D<T, EPb, NPb>& iB)
{
	LASS_ASSERT(iA.isValid() && iB.isValid());

	typedef typename Line2D<T, EPa, NPa>::TNumTraits TNumTraits;

	if (perpDot(iA.direction(), iB.direction()) == TNumTraits::zero)
	{
		return num::abs(iA.signedDistance(iB.support())); // parallel
	}
	else
	{
		return TNumTraits::zero; // intersecting
	}
}



/** intersection of two lines
 *  @relates Line2D
 *  @param iA   line A
 *  @param iB   line B
 *  @param oTa  parameter of intersection point on line A
 *  @param oTb  parameter of intersection point on line B
 *  @return @arg rNone      the lines don't intersect, they have no points in common.
 *                          @a oTa and @a oTb are not assigned.
 *          @arg rOne       both lines have exactly one point in common.
 *                          @a oTa and @a oTb contain parameters of intersection point.
 *          @arg rInfinite  the lines are coincident, they have all points in common.
 *                          @a oTa and @a oTb are not assigned.
 */
template <typename T, class EPa, class NPa, class EPb, class NPb>
Result intersect(const Line2D<T, EPa, NPa>& iA, const Line2D<T, EPb, NPb>& iB,
				 T& oTa, T& oTb)
{
	LASS_ASSERT(iA.isValid() && iB.isValid());

	typedef typename Line2D<T, EPa, NPa>::TVector TVector;
	typedef typename Line2D<T, EPa, NPa>::TValue TValue;
	typedef typename Line2D<T, EPa, NPa>::TNumTraits TNumTraits;

	const TVector dirA(iA.direction());
	const TVector dirB(iB.direction());

	const TValue denominator = -perpDot(dirA, dirB);
	if (denominator == TNumTraits::zero)
	{
		switch (iA.classify(iB.support()))
		{
		case sFront:
		case sBack:
			return rNone; // parallel
		case sSurface:
			return rInfinite; // coincident
		default:
			return rInvalid;
		}
	}
	else
	{
		const TVector difference = iB.support() - iA.support();
		oTa = -perpDot(difference, dirB) / denominator;
		oTb = perpDot(dirA, difference) / denominator;
		return rOne; // intersecting
	}
}



/** intersection of two lines
 *  @relates Line2D
 *  @param iA   line A
 *  @param iB   line B
 *  @param oPoint   intersection point
 *  @return @arg rNone      the lines don't intersect, they have no points in common.
 *                          @a oPoint is not assigned.
 *          @arg rOne       both lines have exactly one point in common.
 *                          @a oPoint contains intersection point.
 *          @arg rInfinite  the lines are coincident, they have all points in common.
 *                          @a oPoint is not assigned.
 */
template <typename T, class EPa, class NPa, class EPb, class NPb>
Result intersect(const Line2D<T, EPa, NPa>& iA, const Line2D<T, EPb, NPb>& iB,
				 Point2D<T>& oPoint)
{
	T tA;
	T tB;
	Result result = intersect(iA, iB, tA, tB);
	if (result == rOne)
	{
		oPoint = (iA.point(tA) + iB.point(tB)).affine(); // take average for more stable point.
	}
	return result;
}



/** @relates Line2D
 */
template <typename T>
io::XmlOStream& operator<<(io::XmlOStream& ioOStream, const Line2D<T, Cartesian>& iLine)
{
	LASS_ENFORCE_STREAM(ioOStream)
		<< "<Line2D>\n"
		<< "<normal>" << iLine.normal() << "</normal>\n"
		<< "<d>" << iLine.d() << "</d>\n"
		<< "</Line2D>\n";

	return ioOStream;
}



/** @relates Line2D
 */
template <typename T>
io::XmlOStream& operator<<(io::XmlOStream& ioOStream, const Line2D<T, Parametric>& iLine)
{
	LASS_ENFORCE_STREAM(ioOStream)
		<< "<Line2D>\n"
		<< "<support>" << iLine.support() << "</support>\n"
		<< "<direction>" << iLine.direction() << "</direction>\n"
		<< "</Line2D>\n";

	return ioOStream;
}



/** @relates Line2D
 */
template <typename T>
io::XmlOStream& operator<<(io::XmlOStream& ioOStream, const Line2D<T, Combined>& iLine)
{
	LASS_ENFORCE_STREAM(ioOStream)
		<< "<Line2D>\n"
		<< "<support>" << iLine.support() << "</support>\n"
		<< "<direction>" << iLine.direction() << "</direction>\n"
		<< "<normal>" << iLine.normal() << "</normal>\n"
		<< "<d>" << iLine.d() << "</d>\n"
		<< "</Line2D>\n";

	return ioOStream;
}



}

}

#endif

// EOF
