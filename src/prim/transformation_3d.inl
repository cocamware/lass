/** @file
 *  @author Bram de Greve (bramz@users.sourceforge.net)
 *  @author Tom De Muer (tomdemuer@users.sourceforge.net)
 *
 *  Distributed under the terms of the GPL (GNU Public License)
 *
 *  The LASS License:
 *
 *  Copyright 2004 Bram de Greve and Tom De Muer
 *
 *  LASS is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */



#ifndef LASS_GUARDIAN_OF_INCLUSION_PRIM_TRANSFORMATION_3D_INL
#define LASS_GUARDIAN_OF_INCLUSION_PRIM_TRANSFORMATION_3D_INL




#include "transformation_3d.h"

namespace lass
{

namespace prim
{

// --- public --------------------------------------------------------------------------------------

template <typename T>
Transformation3D<T>::Transformation3D()
{
	typedef TNumTraits TNT;
	v_[ 0] = TNT::one;  v_[ 1] = TNT::zero; v_[ 2] = TNT::zero; v_[ 3] = TNT::zero;
	v_[ 4] = TNT::zero; v_[ 5] = TNT::one;  v_[ 6] = TNT::zero; v_[ 7] = TNT::zero;
	v_[ 8] = TNT::zero; v_[ 9] = TNT::zero; v_[10] = TNT::one;  v_[11] = TNT::zero;
	v_[12] = TNT::zero; v_[13] = TNT::zero; v_[14] = TNT::zero; v_[15] = TNT::one;
}



template <typename T>
Transformation3D<T>::Transformation3D(bool iDontInitialise)
{
}



template <typename T> inline
typename util::CallTraits<T>::TConstReference
Transformation3D<T>::operator()(TSize iRow, TSize iColumn) const
{
	LASS_ASSERT(iRow < 4 && iColumn < 4);
	return v_[iRow * 4 + iColumn];
}



template <typename T> inline
typename util::CallTraits<T>::TReference
Transformation3D<T>::operator()(TSize iRow, TSize iColumn)
{
	LASS_ASSERT(iRow < 4 && iColumn < 4);
	return v_[iRow * 4 + iColumn];
}



template <typename T> inline
typename util::CallTraits<T>::TConstReference
Transformation3D<T>::at(TSize iRow, TSize iColumn) const
{
	return v_[num::mod(iRow, 4) * 4 + num::mod(iColumn, 4)];
}



template <typename T> inline
typename util::CallTraits<T>::TReference
Transformation3D<T>::at(TSize iRow, TSize iColumn)
{
	return v_[num::mod(iRow, 4) * 4 + num::mod(iColumn, 4)];
}



template <typename T>
const Transformation3D<T>
Transformation3D<T>::inverse() const
{
	Transformation3D<T> result;

	const TValue v1015 = v_[10] * v_[15];
	const TValue v1411 = v_[14] * v_[11];
	const TValue v0615 = v_[ 6] * v_[15];
	const TValue v1407 = v_[14] * v_[ 7];
	const TValue v0611 = v_[ 6] * v_[11];
	const TValue v1007 = v_[10] * v_[ 7];
	const TValue v0215 = v_[ 2] * v_[15];
	const TValue v1403 = v_[14] * v_[ 3];
	const TValue v0211 = v_[ 2] * v_[11];
	const TValue v1003 = v_[10] * v_[ 3];
	const TValue v0207 = v_[ 2] * v_[ 7];
	const TValue v0603 = v_[ 6] * v_[ 3];

	result.v_[0] =  v1015 * v_[ 5] + v1407 * v_[ 9] + v0611 * v_[13];
	result.v_[0] -= v1411 * v_[ 5] + v0615 * v_[ 9] + v1007 * v_[13];
	result.v_[1] =  v1411 * v_[ 1] + v0215 * v_[ 9] + v1003 * v_[13];
	result.v_[1] -= v1015 * v_[ 1] + v1403 * v_[ 9] + v0211 * v_[13];
	result.v_[2] =  v0615 * v_[ 1] + v1403 * v_[ 5] + v0207 * v_[13];
	result.v_[2] -= v1407 * v_[ 1] + v0215 * v_[ 5] + v0603 * v_[13];
	result.v_[3] =  v1007 * v_[ 1] + v0211 * v_[ 5] + v0603 * v_[ 9];
	result.v_[3] -= v0611 * v_[ 1] + v1003 * v_[ 5] + v0207 * v_[ 9];
	result.v_[4] =  v1411 * v_[ 4] + v0615 * v_[ 8] + v1007 * v_[12];
	result.v_[4] -= v1015 * v_[ 4] + v1407 * v_[ 8] + v0611 * v_[12];
	result.v_[5] =  v1015 * v_[ 0] + v1403 * v_[ 8] + v0211 * v_[12];
	result.v_[5] -= v1411 * v_[ 0] + v0215 * v_[ 8] + v1003 * v_[12];
	result.v_[6] =  v1407 * v_[ 0] + v0215 * v_[ 4] + v0603 * v_[12];
	result.v_[6] -= v0615 * v_[ 0] + v1403 * v_[ 4] + v0207 * v_[12];
	result.v_[7] =  v0611 * v_[ 0] + v1003 * v_[ 4] + v0207 * v_[ 8];
	result.v_[7] -= v1007 * v_[ 0] + v0211 * v_[ 4] + v0603 * v_[ 8];

	const TValue v0813 = v_[ 8] * v_[13];
	const TValue v1209 = v_[12] * v_[ 9];
	const TValue v0413 = v_[ 4] * v_[13];
	const TValue v1205 = v_[12] * v_[ 5];
	const TValue v0409 = v_[ 4] * v_[ 9];
	const TValue v0805 = v_[ 8] * v_[ 5];
	const TValue v0013 = v_[ 0] * v_[13];
	const TValue v1201 = v_[12] * v_[ 1];
	const TValue v0009 = v_[ 0] * v_[ 9];
	const TValue v0801 = v_[ 8] * v_[ 1];
	const TValue v0005 = v_[ 0] * v_[ 5];
	const TValue v0401 = v_[ 4] * v_[ 1];

	result.v_[ 8]  = v0813 * v_[ 7] + v1205 * v_[11] + v0409 * v_[15];
	result.v_[ 8] -= v1209 * v_[ 7] + v0413 * v_[11] + v0805 * v_[15];
	result.v_[ 9]  = v1209 * v_[ 3] + v0013 * v_[11] + v0801 * v_[15];
	result.v_[ 9] -= v0813 * v_[ 3] + v1201 * v_[11] + v0009 * v_[15];
	result.v_[10]  = v0413 * v_[ 3] + v1201 * v_[ 7] + v0005 * v_[15];
	result.v_[10] -= v1205 * v_[ 3] + v0013 * v_[ 7] + v0401 * v_[15];
	result.v_[11]  = v0805 * v_[ 3] + v0009 * v_[ 7] + v0401 * v_[11];
	result.v_[11] -= v0409 * v_[ 3] + v0801 * v_[ 7] + v0005 * v_[11];
	result.v_[12]  = v0413 * v_[10] + v0805 * v_[14] + v1209 * v_[ 6];
	result.v_[12] -= v0409 * v_[14] + v0813 * v_[ 6] + v1205 * v_[10];
	result.v_[13]  = v0009 * v_[14] + v0813 * v_[ 2] + v1201 * v_[10];
	result.v_[13] -= v0013 * v_[10] + v0801 * v_[14] + v1209 * v_[ 2];
	result.v_[14]  = v0013 * v_[ 6] + v0401 * v_[14] + v1205 * v_[ 2];
	result.v_[14] -= v0005 * v_[14] + v0413 * v_[ 2] + v1201 * v_[ 6];
	result.v_[15]  = v0005 * v_[10] + v0409 * v_[ 2] + v0801 * v_[ 6];
	result.v_[15] -= v0009 * v_[ 6] + v0401 * v_[10] + v0805 * v_[ 2];

	const TValue det = v_[0] * result.v_[0] + v_[4] * result.v_[1] + v_[8] * result.v_[2] + v_[12] * result.v_[3];
	if (det == TNumTraits::zero)
	{
		LASS_THROW("transformation not invertible");
	}

	const TValue invDet = num::inv(det);
	for (unsigned j = 0; j < 16; j++)
	{
		result.v_[j] *= invDet;
	}

	return result;
}



template <typename T> inline
const typename Transformation3D<T>::TValue*
Transformation3D<T>::data() const
{
	return v_;
}



template <typename T> inline
typename Transformation3D<T>::TValue*
Transformation3D<T>::data()
{
	return v_;
}



// --- protected -----------------------------------------------------------------------------------



// --- private -------------------------------------------------------------------------------------

template <typename T> inline
const typename Transformation3D<T>::TValue
Transformation3D<T>::determinant() const
{
	return v_[ 0] * (v_[ 5] * v_[10] * v_[15] - v_[ 7] * v_[10] * v_[13] +
					v_[ 6] * v_[11] * v_[13] - v_[ 5] * v_[11] * v_[14] +
					v_[ 7] * v_[ 9] * v_[14] - v_[ 6] * v_[ 9] * v_[15])
		 - v_[ 1] * (v_[ 4] * v_[10] * v_[15] - v_[ 7] * v_[10] * v_[12] +
					v_[ 6] * v_[11] * v_[12] - v_[ 4] * v_[11] * v_[14] +
					v_[ 7] * v_[ 8] * v_[14] - v_[ 6] * v_[ 8] * v_[15])
		 + v_[2] * (v_[ 4] * v_[ 9] * v_[15] - v_[ 7] * v_[ 9] * v_[12] +
					v_[ 5] * v_[11] * v_[12] - v_[ 4] * v_[11] * v_[13] +
					v_[ 7] * v_[ 8] * v_[13] - v_[ 5] * v_[ 8] * v_[15])
		 + v_[3] * (v_[ 4] * v_[ 9] * v_[14] - v_[ 6] * v_[ 9] * v_[12] +
					v_[ 5] * v_[10] * v_[12] - v_[ 4] * v_[10] * v_[13] +
					v_[ 6] * v_[ 8] * v_[13] - v_[ 5] * v_[ 8] * v_[14]);
}



// --- free ----------------------------------------------------------------------------------------

template <typename T>
Transformation3D<T> operator*(const Transformation3D<T>& iA, const Transformation3D<T>& iB)
{
	typedef typename Transformation3D<T>::TSize TSize;
	Transformation3D<T> result(false);
	for (TSize i = 0; i < 4; ++i)
	{
		for (TSize j = 0; j < 4; ++j)
		{
			result(i, j) =
				iA(i, 0) * iB(0, j) +
				iA(i, 1) * iB(1, j) +
				iA(i, 2) * iB(2, j) +
				iA(i, 3) * iB(3, j);
		}
	}
	return result;
}



template <typename T>
Vector3D<T> operator*(const Transformation3D<T>& iA, const Vector3D<T>& iB)
{
	return Point3D<T>(iA(0, 0) * iB.x + iA(0, 1) * iB.y + iA(0, 2) * iB.z,
					  iA(1, 0) * iB.x + iA(1, 1) * iB.y + iA(1, 2) * iB.z,
					  iA(2, 0) * iB.x + iA(2, 1) * iB.y + iA(2, 2) * iB.z);
}



template <typename T>
Point3D<T> operator*(const Transformation3D<T>& iA, const Point3D<T>& iB)
{
	return Point3D<T>(iA(0, 0) * iB.x + iA(0, 1) * iB.y + iA(0, 2) * iB.z + iA(0, 3),
					  iA(1, 0) * iB.x + iA(1, 1) * iB.y + iA(1, 2) * iB.z + iA(1, 3),
					  iA(2, 0) * iB.x + iA(2, 1) * iB.y + iA(2, 2) * iB.z + iA(2, 3));
}



/** @relates lass::prim::Vector3D
 */
template<typename T, typename Char, typename Traits>
std::basic_ostream<Char, Traits>& operator<<(std::basic_ostream<Char, Traits>& oOStream,
											 const Transformation3D<T>& iB)
{
	LASS_ENFORCE_STREAM(oOStream) << "(("
		<< iB(0, 0) << ", " << iB(0, 1)  << ", " << iB(0, 2) << ", " << iB(0, 3) << "), ("
		<< iB(1, 0) << ", " << iB(1, 1)  << ", " << iB(1, 2) << ", " << iB(1, 3) << "), ("
		<< iB(2, 0) << ", " << iB(2, 1)  << ", " << iB(2, 2) << ", " << iB(2, 3) << "), ("
		<< iB(3, 0) << ", " << iB(3, 1)  << ", " << iB(3, 2) << ", " << iB(3, 3) << "))";
	return oOStream;
}



/** @relates lass::prim::Vector3D
 */
template<typename T>
io::XmlOStream& operator<<(io::XmlOStream& oOStream, const Transformation3D<T>& iB)
{
	LASS_ENFORCE_STREAM(oOStream) << "<Transformation3D>"
		<< iB(0, 0) << " " << iB(0, 1)  << " " << iB(0, 2) << " " << iB(0, 3) << " "
		<< iB(1, 0) << " " << iB(1, 1)  << " " << iB(1, 2) << " " << iB(1, 3) << " "
		<< iB(2, 0) << " " << iB(2, 1)  << " " << iB(2, 2) << " " << iB(2, 3) << " "
		<< iB(3, 0) << " " << iB(3, 1)  << " " << iB(3, 2) << " " << iB(3, 3)
		<< "</Transformation3D>\n";
	return oOStream;
}



}

}

#endif
