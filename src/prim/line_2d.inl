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
 *  @param oPoint	intersection point
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
