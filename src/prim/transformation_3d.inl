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
template <typename InputIterator>
Transformation3D<T>::Transformation3D(InputIterator iBegin, InputIterator iEnd)
{
	LASS_ENFORCE(std::distance(iBegin, iEnd) == 16);
	std::copy(iBegin, iEnd, v_);
}



#pragma LASS_TODO("make a DeepCopy pointer")
template <typename T>
Transformation3D<T>::Transformation3D(const TSelf& iOther)
{
	std::copy(iOther.v_, iOther.v_ + 16, v_);
	inverse_.reset(iOther.inverse_ ? new TSelf(*iOther.inverse_) : 0);
}



template <typename T>
typename Transformation3D<T>::TSelf&
Transformation3D<T>::operator=(const TSelf& iOther)
{
	std::copy(iOther.v_, iOther.v_ + 16, v_);
	inverse_.reset(iOther.inverse_ ? new TSelf(*iOther.inverse_) : 0);
	return *this;
}



template <typename T>
const Transformation3D<T>&
Transformation3D<T>::inverse() const
{
	if (!inverse_)
	{
		TSelf temp;

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

		temp.v_[0] =  v1015 * v_[ 5] + v1407 * v_[ 9] + v0611 * v_[13];
		temp.v_[0] -= v1411 * v_[ 5] + v0615 * v_[ 9] + v1007 * v_[13];
		temp.v_[1] =  v1411 * v_[ 1] + v0215 * v_[ 9] + v1003 * v_[13];
		temp.v_[1] -= v1015 * v_[ 1] + v1403 * v_[ 9] + v0211 * v_[13];
		temp.v_[2] =  v0615 * v_[ 1] + v1403 * v_[ 5] + v0207 * v_[13];
		temp.v_[2] -= v1407 * v_[ 1] + v0215 * v_[ 5] + v0603 * v_[13];
		temp.v_[3] =  v1007 * v_[ 1] + v0211 * v_[ 5] + v0603 * v_[ 9];
		temp.v_[3] -= v0611 * v_[ 1] + v1003 * v_[ 5] + v0207 * v_[ 9];
		temp.v_[4] =  v1411 * v_[ 4] + v0615 * v_[ 8] + v1007 * v_[12];
		temp.v_[4] -= v1015 * v_[ 4] + v1407 * v_[ 8] + v0611 * v_[12];
		temp.v_[5] =  v1015 * v_[ 0] + v1403 * v_[ 8] + v0211 * v_[12];
		temp.v_[5] -= v1411 * v_[ 0] + v0215 * v_[ 8] + v1003 * v_[12];
		temp.v_[6] =  v1407 * v_[ 0] + v0215 * v_[ 4] + v0603 * v_[12];
		temp.v_[6] -= v0615 * v_[ 0] + v1403 * v_[ 4] + v0207 * v_[12];
		temp.v_[7] =  v0611 * v_[ 0] + v1003 * v_[ 4] + v0207 * v_[ 8];
		temp.v_[7] -= v1007 * v_[ 0] + v0211 * v_[ 4] + v0603 * v_[ 8];

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

		temp.v_[ 8]  = v0813 * v_[ 7] + v1205 * v_[11] + v0409 * v_[15];
		temp.v_[ 8] -= v1209 * v_[ 7] + v0413 * v_[11] + v0805 * v_[15];
		temp.v_[ 9]  = v1209 * v_[ 3] + v0013 * v_[11] + v0801 * v_[15];
		temp.v_[ 9] -= v0813 * v_[ 3] + v1201 * v_[11] + v0009 * v_[15];
		temp.v_[10]  = v0413 * v_[ 3] + v1201 * v_[ 7] + v0005 * v_[15];
		temp.v_[10] -= v1205 * v_[ 3] + v0013 * v_[ 7] + v0401 * v_[15];
		temp.v_[11]  = v0805 * v_[ 3] + v0009 * v_[ 7] + v0401 * v_[11];
		temp.v_[11] -= v0409 * v_[ 3] + v0801 * v_[ 7] + v0005 * v_[11];
		temp.v_[12]  = v0413 * v_[10] + v0805 * v_[14] + v1209 * v_[ 6];
		temp.v_[12] -= v0409 * v_[14] + v0813 * v_[ 6] + v1205 * v_[10];
		temp.v_[13]  = v0009 * v_[14] + v0813 * v_[ 2] + v1201 * v_[10];
		temp.v_[13] -= v0013 * v_[10] + v0801 * v_[14] + v1209 * v_[ 2];
		temp.v_[14]  = v0013 * v_[ 6] + v0401 * v_[14] + v1205 * v_[ 2];
		temp.v_[14] -= v0005 * v_[14] + v0413 * v_[ 2] + v1201 * v_[ 6];
		temp.v_[15]  = v0005 * v_[10] + v0409 * v_[ 2] + v0801 * v_[ 6];
		temp.v_[15] -= v0009 * v_[ 6] + v0401 * v_[10] + v0805 * v_[ 2];

		const TValue det = v_[0] * temp.v_[0] + v_[4] * temp.v_[1] + v_[8] * temp.v_[2] + v_[12] * temp.v_[3];
		if (det == TNumTraits::zero)
		{
			LASS_THROW("transformation not invertible");
		}
		const TValue invDet = num::inv(det);
		for (unsigned j = 0; j < 16; j++)
		{
				temp.v_[j] *= invDet;
		}

		inverse_.reset(new TSelf(temp));
	}

	return *inverse_;
}



/** Return pointer to row major matrix representation of transformation.
 *  This is for immediate use only, like @c std::basic_string::data().
 */
template <typename T> inline
const typename Transformation3D<T>::TValue*
Transformation3D<T>::matrix() const
{
	return v_;
}



/** make a 3D transformation representing a translation
 */
template <typename T>
const Transformation3D<T> Transformation3D<T>::translation(const Vector3D<T>& iOffset)
{
	Transformation3D<T> result;
	result.v_[3] = iOffset.x;
	result.v_[7] = iOffset.y;
	result.v_[11] = iOffset.z;
	return result;
}



/** make a 3D transformation representing a uniform scaling
 */
template <typename T>
const Transformation3D<T> Transformation3D<T>::scaler(const T& iScale)
{
	Transformation3D<T> result;
	result.v_[0] = iScale;
	result.v_[5] = iScale;
	result.v_[10] = iScale;
	return result;
}



/** make a 3D transformation representing a scaling with different factors per axis
 */
template <typename T>
const Transformation3D<T> Transformation3D<T>::scaler(const Vector3D<T>& iScale)
{
	Transformation3D<T> result;
	result.v_[0] = iScale.x;
	result.v_[5] = iScale.y;
	result.v_[10] = iScale.z;
	return result;
}



/** make a 3D transformation representing a rotation around a primary axis
 */
template <typename T>
const Transformation3D<T> Transformation3D<T>::rotation(XYZ iAxis, const T& iRadians)
{
	const T c = num::cos(iRadians);
	const T s = num::sin(iRadians);
	const size_t a = (iAxis + 1);
	const size_t b = (iAxis + 2);
	LASS_ASSERT(a < 3 && b < 3);

	Transformation3D<T> result;
	result.v_[5 * a] = c;
	result.v_[5 * b] = c;
	result.v_[4 * a + b] = -s;
	result.v_[4 * b + a] = s;
	return result;
}



/** make a 3D transformation representing a rotation around an arbitrary axis
 */
template <typename T>
const Transformation3D<T> Transformation3D<T>::rotation(const Vector3D<T>& iAxis, const T& iRadians)
{
	Vector3D<T> a = iAxis.normal();
	const T c = num::cos(iRadians);
	const T s = num::sin(iRadians);
	const TValue oneMinusC = TNumTraits::one - c;

	Transformation3D<T> result;
	result.v_[ 0] = a.x * a.x * oneMinusC + c;
	result.v_[ 1] = a.x * a.y * oneMinusC - a.z * s;
	result.v_[ 2] = a.x * a.z * oneMinusC + a.y * s;
	result.v_[ 4] = a.y * a.x * oneMinusC + a.z * s;
	result.v_[ 5] = a.y * a.y + oneMinusC + c;
	result.v_[ 6] = a.y * a.z * oneMinusC - a.x * s;
	result.v_[ 8] = a.z * a.x * oneMinusC - a.y * s;
	result.v_[ 9] = a.z * a.y * oneMinusC + a.x * s;
	result.v_[10] = a.z * a.z + oneMinusC + c;
	return result;
}



// --- protected -----------------------------------------------------------------------------------



// --- private -------------------------------------------------------------------------------------





// --- free ----------------------------------------------------------------------------------------

template <typename T>
Transformation3D<T> concatenate(const Transformation3D<T>& iA, const Transformation3D<T>& iB)
{
	const T* const a = iA.matrix();
	const T* const b = iB.matrix();
	T result[16];
	for (size_t i = 0; i < 16; i += 4)
	{
		for (size_t j = 0; j < 4; ++j)
		{
			result[i + j] = 
				a[i    ] * b[     j] + 
				a[i + 1] * b[ 4 + j] + 
				a[i + 2] * b[ 8 + j] + 
				a[i + 3] * b[12 + j];
		}
	}
	return Transformation3D<T>(result, result + 16);
}



template <typename T>
Vector3D<T> transform(const Vector3D<T>& iSubject, const Transformation3D<T>& iTransformation)
{
	const T* const mat = iTransformation.matrix();
	return Vector3D<T>(
		mat[ 0] * iSubject.x + mat[ 1] * iSubject.y + mat[ 2] * iSubject.z,
		mat[ 4] * iSubject.x + mat[ 5] * iSubject.y + mat[ 6] * iSubject.z,
		mat[ 8] * iSubject.x + mat[ 9] * iSubject.y + imatrix[10] * iSubject.z);
}



template <typename T>
Point3D<T> transform(const Point3D<T>& iSubject, const Transformation3D<T>& iTransformation)
{
	const T* const mat = iTransformation.matrix();
	const T weight = num::inv(mat[12] * iSubject.x + mat[13] * iSubject.y + mat[14] * iSubject.z + mat[15]);
	return Point3D<T>(
		weight * (mat[ 0] * iSubject.x + mat[ 1] * iSubject.y + mat[ 2] * iSubject.z + mat[ 3]),
		weight * (mat[ 4] * iSubject.x + mat[ 5] * iSubject.y + mat[ 6] * iSubject.z + mat[ 7]),
		weight * (mat[ 8] * iSubject.x + mat[ 9] * iSubject.y + mat[10] * iSubject.z + mat[11]));

}



template <typename T>
Vector3D<T> normalTransform(const Vector3D<T>& iSubject, const Transformation3D<T>& iTransformation)
{
	const T* const invMat = iTransformation.inverse().matrix();
	return Vector3D<T>(
		invMat[ 0] * iSubject.x + invMat[ 4] * iSubject.y + invMat[ 8] * iSubject.z,
		invMat[ 1] * iSubject.x + invMat[ 5] * iSubject.y + invMat[ 9] * iSubject.z,
		invMat[ 2] * iSubject.x + invMat[ 6] * iSubject.y + invMat[10] * iSubject.z);
}



template <typename T>
std::pair<Vector3D<T>, T> normalTransform(const std::pair<Vector3D<T>, T>& iSubject, 
										  const Transformation3D<T>& iTransformation)
{
	const T* const invMat = iTransformation.inverse().matrix();
	const Vector3D<T>& n = iSubject.first;
	const T d = iSubject.second;
	return std::make_pair(
		Vector3D<T>(
			invMat[ 0] * n.x + invMat[ 4] * n.y + invMat[ 8] * n.z + invMat[12] * d,
			invMat[ 1] * n.x + invMat[ 5] * n.y + invMat[ 9] * n.z + invMat[13] * d,
			invMat[ 2] * n.x + invMat[ 6] * n.y + invMat[10] * n.z + invMat[14] * d),
		invMat[ 3] * n.x + invMat[ 7] * n.y + invMat[11] * n.z + invMat[15] * d));
}



/** @relates lass::prim::Vector3D
 */
template<typename T, typename Char, typename Traits>
std::basic_ostream<Char, Traits>& operator<<(std::basic_ostream<Char, Traits>& oOStream,
											 const Transformation3D<T>& iTransformation)
{
	const T* const mat = iTransformation.matrix();
	LASS_ENFORCE_STREAM(oOStream) << "(("
		<< mat[ 0] << ", " << mat[ 1] << ", " << mat[ 2] << ", " << mat[ 3] << "), ("
		<< mat[ 4] << ", " << mat[ 5] << ", " << mat[ 6] << ", " << mat[ 7] << "), ("
		<< mat[ 8] << ", " << mat[ 9] << ", " << mat[10] << ", " << mat[11] << "), ("
		<< mat[12] << ", " << mat[13] << ", " << mat[14] << ", " << mat[15] << "))";
	return oOStream;
}



/** @relates lass::prim::Vector3D
 */
template<typename T>
io::XmlOStream& operator<<(io::XmlOStream& oOStream, const Transformation3D<T>& iTransformation)
{
	const T* const mat = iTransformation.matrix();
	LASS_ENFORCE_STREAM(oOStream) << "<Transformation3D>"
		<< mat[ 0] << " " << mat[ 1] << " " << mat[ 2] << " " << mat[ 3] << " "
		<< mat[ 4] << " " << mat[ 5] << " " << mat[ 6] << " " << mat[ 7] << " "
		<< mat[ 8] << " " << mat[ 9] << " " << mat[10] << " " << mat[11] << " "
		<< mat[12] << " " << mat[13] << " " << mat[14] << " " << mat[15]
		<< "</Transformation3D>\n";
	return oOStream;
}



}

}

#endif
