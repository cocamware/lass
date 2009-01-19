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



/** @class lass::prim::LineSegment2D
 *  @brief 2D Line Segment
 *  @author Bram de Greve [BdG]
 *  @date 2003
 */



#ifndef LASS_GUARDIAN_OF_INCLUSION_PRIM_LINE_SEGMENT_2D_H
#define LASS_GUARDIAN_OF_INCLUSION_PRIM_LINE_SEGMENT_2D_H

#include "prim_common.h"
#include "parameter_policy.h"
#include "point_2d.h"
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
class LineSegment2D
{
public:

	typedef LineSegment2D<T, ParameterPolicy> TSelf;
	typedef ParameterPolicy TParameterPolicy;

	typedef Point2D<T> TPoint;
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
		typedef LineSegment2D<U, ParameterPolicy> Type;
	};

	LineSegment2D();
	LineSegment2D(const TPoint& tail, const TPoint& head);

	const TPoint& tail() const;
	TPoint& tail();
	const TPoint& head() const;
	TPoint& head();

	const TPoint point(TParam t) const;
	const TValue t(const TPoint& point) const;
	const TVector vector() const;
	const TValue length() const;

private:

	TPoint tail_;
	TPoint head_;
};

template <typename T, class PPa, class PPb> bool operator==(const LineSegment2D<T, PPa>& a, const LineSegment2D<T, PPb>& b);
template <typename T, class PPa, class PPb> bool operator!=(const LineSegment2D<T, PPa>& a, const LineSegment2D<T, PPb>& b);

template <typename T, class PP>
T squaredDistance(const LineSegment2D<T, PP>& segment, const Point2D<T>& point);

template <typename T, class PP>
T distance(const LineSegment2D<T, PP>& segment, const Point2D<T>& point);

template <typename T, class PPa, class PPb>
Result intersect(const LineSegment2D<T, PPa>& a, const LineSegment2D<T, PPb>& b, T& tA, T& tB);

template <typename T, class PPa, class PPb>
Result intersect(const LineSegment2D<T, PPa>& a, const LineSegment2D<T, PPb>& b, Point2D<T>& point);

template<typename T, class PP>
std::ostream& operator<<(std::ostream& stream, const LineSegment2D<T, PP>& segment);

template<typename T, class PP>
io::XmlOStream& operator<<(io::XmlOStream& stream, const LineSegment2D<T, PP>& segment);

template<typename T, class PP>
lass::io::MatlabOStream& operator<<(lass::io::MatlabOStream& stream, const LineSegment2D<T, PP>& segment);

}

}

#include "line_segment_2d.inl"

#define LASS_PRIM_HAVE_PY_EXPORT_TRAITS_LINE_SEGMENT_2D
#ifdef LASS_GUARDIAN_OF_INCLUSION_UTIL_PYOBJECT_PLUS_H
#	include "pyobject_util.h"
#endif

#ifdef LASS_GUARDIAN_OF_INCLUSION_PRIM_RAY_2D_H
#	include "line_segment_2d_ray_2d.h"
#endif

#endif
