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



/** @class lass::prim::LineSegment3D
 *  @brief 3D Line Segment
 *  @author Bram de Greve [BdG]
 *  @date 3003
 */



#ifndef LASS_GUARDIAN_OF_INCLUSION_PRIM_LINE_SEGMENT_3D_H
#define LASS_GUARDIAN_OF_INCLUSION_PRIM_LINE_SEGMENT_3D_H

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
	template <typename PP2>
	LineSegment3D(const LineSegment3D<T, PP2>& other);

	const TPoint& tail() const;
	TPoint& tail();
	const TPoint& head() const;
	TPoint& head();

	const TPoint point(TParam iT) const;
	const TValue t(const TPoint& iPoint) const;
	const TVector vector() const;
	const TValue length() const;

	const TValue distance(const TPoint& iPoint) const;
	const TValue squaredDistance(const TPoint& iPoint) const;

	const TValue closestsPoint(const TPoint &iPoint, T &oT) const;


private:

	TPoint tail_;
	TPoint head_;
};

template <typename T, class PPa, class PPb> bool operator==(const LineSegment3D<T, PPa>& iA, const LineSegment3D<T, PPb>& iB);
template <typename T, class PPa, class PPb> bool operator!=(const LineSegment3D<T, PPa>& iA, const LineSegment3D<T, PPb>& iB);

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

#define LASS_PRIM_HAVE_PY_EXPORT_TRAITS_LINE_SEGMENT_3D
#ifdef LASS_GUARDIAN_OF_INCLUSION_UTIL_PYOBJECT_PLUS_H
#	include "../python/export_traits_prim.h"
#endif

#ifdef LASS_GUARDIAN_OF_INCLUSION_PRIM_PLANE_3D_H
#	include "line_segment_3d_plane_3d.h"
#endif

#ifdef LASS_GUARDIAN_OF_INCLUSION_PRIM_RAY_3D_H
#	include "line_segment_3d_ray_3d.h"
#endif

#endif
