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




/** @class lass::prim::impl::Plane3DCombined
 *  @brief implementation of plane 3d with both cartesian and parametric equation,
 *         and plus extra reciprocal vectors and board.
 *  @author Bram de Greve [BdG]
 */



#ifndef LASS_GUARDIAN_OF_INCLUSION_PRIM_IMPL_PLANE_3D_COMBINED_H
#define LASS_GUARDIAN_OF_INCLUSION_PRIM_IMPL_PLANE_3D_COMBINED_H

#include "../prim_common.h"

namespace lass
{
namespace prim
{
namespace impl
{

template <typename T, class NormalizingPolicy = Normalized>
class Plane3DCombined
{
public:

	typedef NormalizingPolicy TNormalizingPolicy;

	typedef Point3D<T> TPoint;
	typedef typename TPoint::TVector TVector;
	typedef Point2D<T> TUV;

	typedef typename TPoint::TValue TValue;
	typedef typename TPoint::TParam TParam;
	typedef typename TPoint::TReference TReference;
	typedef typename TPoint::TConstReference TConstReference;
	typedef typename TPoint::TNumTraits TNumTraits;

	enum { dimension = TPoint::dimension }; /**< number of dimensions of vector */

	Plane3DCombined();
	Plane3DCombined(const TPoint& iSupport, const TPoint& iPointU, const TPoint& iPointV);
	Plane3DCombined(const TPoint& iSupport, const TVector& iDirectionU, const TVector& iDirectionV);
	Plane3DCombined(const TVector& iNormal, const TPoint& iSupport);
	Plane3DCombined(const TVector& iNormal, TParam iD);

	const TPoint& support() const;
	void getDirections(TVector& oDirectionU, TVector& oDirectionV) const;
	const TVector& directionU() const;
	const TVector& directionV() const;

	void getReciprocals(TVector& oReciprocalU, TVector& oReciprocalV) const;
	const TVector& reciprocalU() const;
	const TVector& reciprocalV() const;

	void getCartesian(TVector& oNormal, TReference oD) const;
	const TVector& normal() const;
	const TParam d() const;

	const TValue equation(const TPoint& iPoint) const;
	const TValue equation(const TPoint& iPoint, TParam iRelativeTolerance) const;

	const TVector reject(const TPoint& iPoint) const;
	const TVector reject(const TVector& iVector) const;
	const TPoint project(const TPoint& iPoint) const;
	const TVector project(const TVector& iVector) const;
	const TPoint reflect(const TPoint& iPoint) const;
	const TVector reflect(const TVector& iVector) const;

	const TPoint point(TParam iU, TParam iV) const;
	const TPoint point(const TUV& iUV) const;
	const TUV uv(const TPoint& iPoint) const;

	void flip();
	const bool isValid() const;

private:

	TPoint support_;
	TVector directionU_;
	TVector directionV_;
	TVector reciprocalU_;
	TVector reciprocalV_;
	TVector normal_;
	TValue d_;
};



}

}

}

#include "plane_3d_combined.inl"

#endif

// EOF
