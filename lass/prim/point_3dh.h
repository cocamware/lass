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



/** @struct lass::prim::Point3DH
 *  @brief homogenous 3D Point
 *  @author BdG
 *  @date 2003
 */



#ifndef LASS_GUARDIAN_OF_INCLUSION_PRIM_POINT_3DH_H
#define LASS_GUARDIAN_OF_INCLUSION_PRIM_POINT_3DH_H




#include "prim_common.h"
#include "point_3d.h"
#include "vector_4d.h"


namespace lass
{

namespace prim
{

template<typename T>
struct Point3DH
{
public:

	typedef Point3DH<T> TSelf;

	typedef Point3D<T> TPoint;
	typedef Vector4D<T> TVector;

	typedef typename TVector::TValue TValue;
	typedef typename TVector::TParam TParam;
	typedef typename TVector::TReference TReference;
	typedef typename TVector::TConstReference TConstReference;
	typedef typename TVector::TNumTraits TNumTraits;

	enum { dimension = TPoint::dimension }; /**< number of dimensions */

	template <typename U> struct Rebind
	{
		typedef Point3DH<U> Type;
	};

	TValue x;
	TValue y;
	TValue z;
	TValue w;

	Point3DH();
	Point3DH(TParam iX, TParam iY, TParam iZ, TParam iW = TNumTraits::one);
	Point3DH(const TPoint& iAffinePoint);
	explicit Point3DH(const TVector& iPositionVector);

	const TVector position() const;
	TConstReference operator[](size_t iIndex) const;
	TReference operator[](size_t iIndex);
	TConstReference at(signed iIndex) const;
	TReference at(signed iIndex);

	const Point3DH<T>& operator+() const;
	const Point3DH<T> operator-() const;

	Point3DH<T>& operator+=(const Point3DH<T>& iB);
	Point3DH<T>& operator-=(const Point3DH<T>& iB);
	Point3DH<T>& operator*=(TParam iB);
	Point3DH<T>& operator/=(TParam iB);

	const bool isZero() const;
	const bool isNaN() const;
	const bool isInfinite() const;
	const bool isValid() const;
	const TParam weight() const;
	const TPoint affine() const;

	void homogenize();
};

template<typename T> bool operator==(const Point3DH<T>& iA, const Point3DH<T>& iB);
template<typename T> bool operator!=(const Point3DH<T>& iA, const Point3DH<T>& iB);

template<typename T> Point3DH<T> operator+(const Point3DH<T>& iA, const Point3DH<T>& iB);
template<typename T> Point3DH<T> operator-(const Point3DH<T>& iA, Point3DH<T>& iB);

template<typename T> Point3DH<T> operator*(const Point3DH<T>& iA, typename Point3DH<T>::TParam iB);
template<typename T> Point3DH<T> operator/(const Point3DH<T>& iA, typename Point3DH<T>::TParam iB);
template<typename T> Point3DH<T> operator*(typename Point3DH<T>::TParam iA, const Point3DH<T>& iB);

template<typename T> std::ostream& operator<<(std::ostream& oOStream, const Point3DH<T>& iB);
template<typename T> io::XmlOStream& operator<<(io::XmlOStream& oOStream, const Point3DH<T>& iB);
template<typename T> std::istream& operator>>(std::istream& ioIStream, Point3DH<T>& oB);


// redirectors (stuff because template instantiation doesn't understand it can convert
// regular points to homogenous points:
template<typename T> inline Point3DH<T> operator+(const Point3D<T>& iA, const Point3D<T>& iB)
{
	return Point3DH<T>(iA) + Point3DH<T>(iB);
}
template<typename T> inline Point3DH<T> operator*(typename Point3D<T>::TParam iA, const Point3D<T>& iB)
{
	return iA * Point3DH<T>(iB);
}
template<typename T> inline Point3DH<T> operator*(const Point3D<T>& iA, typename Point3D<T>::TParam iB)
{
	return Point3DH<T>(iA) * iB;
}



}

}

#include "point_3dh.inl"

#endif

// --- END OF FILE ------------------------------------------------------------------------------
