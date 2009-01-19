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



/** @class lass::prim::Triangle3D
 *  @brief A very simple 3D polygon :)
 *  @author Bram de Greve [BdG]
 */

#ifndef LASS_GUARDIAN_OF_INCLUSION_PRIM_TRIANGLE_3D_H
#define LASS_GUARDIAN_OF_INCLUSION_PRIM_TRIANGLE_3D_H

#include "prim_common.h"
#include "line_segment_3d.h"
#include "plane_3d.h"

namespace lass
{
namespace prim
{

template <typename T>
class Triangle3D
{
public:

	typedef Triangle3D<T> TSelf;

	typedef Point3D<T> TPoint;
	typedef Point3DH<T> TPointH;
	typedef typename TPoint::TVector TVector;
	typedef LineSegment3D<T> TLineSegment;
	typedef Plane3D<T, Cartesian, Normalized> TPlane;

	typedef typename TPoint::TValue TValue;
	typedef typename TPoint::TParam TParam;
	typedef typename TPoint::TReference TReference;
	typedef typename TPoint::TConstReference TConstReference;
	typedef typename TPoint::TNumTraits TNumTraits;

	enum { dimension = TPoint::dimension }; /**< number of dimensions */

	template <typename U> struct Rebind
	{
		typedef Triangle3D<U> Type;
	};

	Triangle3D();
	Triangle3D(const TPoint& iA, const TPoint& iB, const TPoint& iC);

	const TPoint& operator[](int iIndexOfVertex) const;
	TPoint& operator[](int iIndexOfVertex);
	const TPoint& at(int iIndexOfVertex) const;
	TPoint& at(int iIndexOfVertex);
	const TLineSegment edge(int iIndexOfTailVertex) const;
	const TVector vector(int iIndexOfTailVertex) const;
	const TPlane plane() const;

	const bool isEmpty() const;
	const int size() const;

	const TValue squaredArea() const;
	const TValue area() const;
	const TValue perimeter() const;
	const TPointH vertexCentroid() const;
	const TPointH surfaceCentroid() const;

	const bool isSimple() const;
	const bool isConvex() const;

	const bool isReflex(int iIndexOfVertex) const;

private:

	const bool isInRange(int iIndexOfVertex) const;

	enum { size_ = 3 };

	TPoint vertices_[size_];
};

template <typename T>
io::XmlOStream& operator<<(io::XmlOStream& ioOStream, const Triangle3D<T>& iTriangle);
template <typename T>
std::ostream& operator<<(std::ostream& ioOStream, const Triangle3D<T>& iTriangle);



}

}

#include "triangle_3d.inl"

#ifdef LASS_GUARDIAN_OF_INCLUSION_PRIM_AABB_3D_H
#	include "aabb_3d_triangle_3d.h"
#endif

#ifdef LASS_GUARDIAN_OF_INCLUSION_PRIM_RAY_3D_H
#	include "ray_3d_triangle_3d.h"
#endif

#endif

// EOF
