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

/** construct an identity transformation.
 *  An identity transformation transforms every point to itself.
 */
template <typename T>
Transformation3D<T>::Transformation3D():
	matrix_(new TValue[16]),
	inverseMatrix_(0)
{
	matrix_[ 0] = TNumTraits::one;
	matrix_[ 1] = TNumTraits::zero;
	matrix_[ 2] = TNumTraits::zero;
	matrix_[ 3] = TNumTraits::zero;
	matrix_[ 4] = TNumTraits::zero;
	matrix_[ 5] = TNumTraits::one;
	matrix_[ 6] = TNumTraits::zero;
	matrix_[ 7] = TNumTraits::zero;
	matrix_[ 8] = TNumTraits::zero;
	matrix_[ 9] = TNumTraits::zero;
	matrix_[10] = TNumTraits::one;
	matrix_[11] = TNumTraits::zero;
	matrix_[12] = TNumTraits::zero;
	matrix_[13] = TNumTraits::zero;
	matrix_[14] = TNumTraits::zero;
	matrix_[15] = TNumTraits::one;
}



/** construct a transformation from a 4x4 tranformation matrix.
 *  The elements of the 4x4 matrix will represented in a row major way by an iterator
 *  range [iBegin, iEnd) of 16 elements.
 */
template <typename T>
template <typename InputIterator>
Transformation3D<T>::Transformation3D(InputIterator iBegin, InputIterator iEnd):
	matrix_(new TValue[16]),
	inverseMatrix_(0)
{
	LASS_ENFORCE(std::distance(iBegin, iEnd) == 16);
	std::copy(iBegin, iEnd, matrix_.get());
}



/** return the inverse transformation.
 *  The inverse is calculated on the first call, and then cached for later use.
 *  For the transformation, we've use the C version of Cramer's rule as described in
 *  the Intel (R) article "Streaming SIMD Extensions -Inverse of 4x4 Matrix" which
 *  can be found here: http://www.intel.com/design/pentiumiii/sml/245043.htm
 */
template <typename T>
const Transformation3D<T>
Transformation3D<T>::inverse() const
{
	if (inverseMatrix_.isEmpty())
	{
		inverseMatrix_.reset(new TValue[16]);

		const TValue v1015 = matrix_[10] * matrix_[15];
		const TValue v1411 = matrix_[14] * matrix_[11];
		const TValue v0615 = matrix_[ 6] * matrix_[15];
		const TValue v1407 = matrix_[14] * matrix_[ 7];
		const TValue v0611 = matrix_[ 6] * matrix_[11];
		const TValue v1007 = matrix_[10] * matrix_[ 7];
		const TValue v0215 = matrix_[ 2] * matrix_[15];
		const TValue v1403 = matrix_[14] * matrix_[ 3];
		const TValue v0211 = matrix_[ 2] * matrix_[11];
		const TValue v1003 = matrix_[10] * matrix_[ 3];
		const TValue v0207 = matrix_[ 2] * matrix_[ 7];
		const TValue v0603 = matrix_[ 6] * matrix_[ 3];

		inverseMatrix_[0] =  v1015 * matrix_[ 5] + v1407 * matrix_[ 9] + v0611 * matrix_[13];
		inverseMatrix_[0] -= v1411 * matrix_[ 5] + v0615 * matrix_[ 9] + v1007 * matrix_[13];
		inverseMatrix_[1] =  v1411 * matrix_[ 1] + v0215 * matrix_[ 9] + v1003 * matrix_[13];
		inverseMatrix_[1] -= v1015 * matrix_[ 1] + v1403 * matrix_[ 9] + v0211 * matrix_[13];
		inverseMatrix_[2] =  v0615 * matrix_[ 1] + v1403 * matrix_[ 5] + v0207 * matrix_[13];
		inverseMatrix_[2] -= v1407 * matrix_[ 1] + v0215 * matrix_[ 5] + v0603 * matrix_[13];
		inverseMatrix_[3] =  v1007 * matrix_[ 1] + v0211 * matrix_[ 5] + v0603 * matrix_[ 9];
		inverseMatrix_[3] -= v0611 * matrix_[ 1] + v1003 * matrix_[ 5] + v0207 * matrix_[ 9];
		inverseMatrix_[4] =  v1411 * matrix_[ 4] + v0615 * matrix_[ 8] + v1007 * matrix_[12];
		inverseMatrix_[4] -= v1015 * matrix_[ 4] + v1407 * matrix_[ 8] + v0611 * matrix_[12];
		inverseMatrix_[5] =  v1015 * matrix_[ 0] + v1403 * matrix_[ 8] + v0211 * matrix_[12];
		inverseMatrix_[5] -= v1411 * matrix_[ 0] + v0215 * matrix_[ 8] + v1003 * matrix_[12];
		inverseMatrix_[6] =  v1407 * matrix_[ 0] + v0215 * matrix_[ 4] + v0603 * matrix_[12];
		inverseMatrix_[6] -= v0615 * matrix_[ 0] + v1403 * matrix_[ 4] + v0207 * matrix_[12];
		inverseMatrix_[7] =  v0611 * matrix_[ 0] + v1003 * matrix_[ 4] + v0207 * matrix_[ 8];
		inverseMatrix_[7] -= v1007 * matrix_[ 0] + v0211 * matrix_[ 4] + v0603 * matrix_[ 8];

		const TValue v0813 = matrix_[ 8] * matrix_[13];
		const TValue v1209 = matrix_[12] * matrix_[ 9];
		const TValue v0413 = matrix_[ 4] * matrix_[13];
		const TValue v1205 = matrix_[12] * matrix_[ 5];
		const TValue v0409 = matrix_[ 4] * matrix_[ 9];
		const TValue v0805 = matrix_[ 8] * matrix_[ 5];
		const TValue v0013 = matrix_[ 0] * matrix_[13];
		const TValue v1201 = matrix_[12] * matrix_[ 1];
		const TValue v0009 = matrix_[ 0] * matrix_[ 9];
		const TValue v0801 = matrix_[ 8] * matrix_[ 1];
		const TValue v0005 = matrix_[ 0] * matrix_[ 5];
		const TValue v0401 = matrix_[ 4] * matrix_[ 1];

		inverseMatrix_[ 8]  = v0813 * matrix_[ 7] + v1205 * matrix_[11] + v0409 * matrix_[15];
		inverseMatrix_[ 8] -= v1209 * matrix_[ 7] + v0413 * matrix_[11] + v0805 * matrix_[15];
		inverseMatrix_[ 9]  = v1209 * matrix_[ 3] + v0013 * matrix_[11] + v0801 * matrix_[15];
		inverseMatrix_[ 9] -= v0813 * matrix_[ 3] + v1201 * matrix_[11] + v0009 * matrix_[15];
		inverseMatrix_[10]  = v0413 * matrix_[ 3] + v1201 * matrix_[ 7] + v0005 * matrix_[15];
		inverseMatrix_[10] -= v1205 * matrix_[ 3] + v0013 * matrix_[ 7] + v0401 * matrix_[15];
		inverseMatrix_[11]  = v0805 * matrix_[ 3] + v0009 * matrix_[ 7] + v0401 * matrix_[11];
		inverseMatrix_[11] -= v0409 * matrix_[ 3] + v0801 * matrix_[ 7] + v0005 * matrix_[11];
		inverseMatrix_[12]  = v0413 * matrix_[10] + v0805 * matrix_[14] + v1209 * matrix_[ 6];
		inverseMatrix_[12] -= v0409 * matrix_[14] + v0813 * matrix_[ 6] + v1205 * matrix_[10];
		inverseMatrix_[13]  = v0009 * matrix_[14] + v0813 * matrix_[ 2] + v1201 * matrix_[10];
		inverseMatrix_[13] -= v0013 * matrix_[10] + v0801 * matrix_[14] + v1209 * matrix_[ 2];
		inverseMatrix_[14]  = v0013 * matrix_[ 6] + v0401 * matrix_[14] + v1205 * matrix_[ 2];
		inverseMatrix_[14] -= v0005 * matrix_[14] + v0413 * matrix_[ 2] + v1201 * matrix_[ 6];
		inverseMatrix_[15]  = v0005 * matrix_[10] + v0409 * matrix_[ 2] + v0801 * matrix_[ 6];
		inverseMatrix_[15] -= v0009 * matrix_[ 6] + v0401 * matrix_[10] + v0805 * matrix_[ 2];

		const TValue det = matrix_[0] * inverseMatrix_[0] + matrix_[4] * inverseMatrix_[1] + matrix_[8] * inverseMatrix_[2] + matrix_[12] * inverseMatrix_[3];
		if (det == TNumTraits::zero)
		{
			inverseMatrix_.reset();
			LASS_THROW("transformation not invertible");
		}
		const TValue invDet = num::inv(det);
		std::transform(inverseMatrix_.get(), inverseMatrix_.get() + 16,
			inverseMatrix_.get(),
			std::bind2nd(std::multiplies<TValue>(), num::inv(det)));
	}

	LASS_ASSERT(inverseMatrix_ && matrix_);
	return TSelf(inverseMatrix_, matrix_, false);
}



/** Return pointer to row major matrix representation of transformation.
 *  This is for immediate use only, like @c std::basic_string::data().
 */
template <typename T> inline
const typename Transformation3D<T>::TValue*
Transformation3D<T>::matrix() const
{
	return matrix_.get();
}



template <typename T>
void Transformation3D<T>::swap(TSelf& ioOther)
{
	matrix_.swap(ioOther.matrix_);
	inverseMatrix_.swap(ioOther.inverseMatrix_);
}



/** make a 3D identity transformation
 */
template <typename T>
const Transformation3D<T> Transformation3D<T>::identity()
{
	return TSelf();
}



/** make a 3D transformation representing a translation
 */
template <typename T>
const Transformation3D<T> Transformation3D<T>::translation(const Vector3D<T>& iOffset)
{
	TSelf result;
	result.matrix_[3] = iOffset.x;
	result.matrix_[7] = iOffset.y;
	result.matrix_[11] = iOffset.z;
	return result;
}



/** make a 3D transformation representing a uniform scaling
 */
template <typename T>
const Transformation3D<T> Transformation3D<T>::scaler(const T& iScale)
{
	TSelf result;
	result.matrix_[0] = iScale;
	result.matrix_[5] = iScale;
	result.matrix_[10] = iScale;
	return result;
}



/** make a 3D transformation representing a scaling with different factors per axis
 */
template <typename T>
const Transformation3D<T> Transformation3D<T>::scaler(const Vector3D<T>& iScale)
{
	TSelf result;
	result.matrix_[0] = iScale.x;
	result.matrix_[5] = iScale.y;
	result.matrix_[10] = iScale.z;
	return result;
}



/** make a 3D transformation representing a rotation around a primary axis
 */
template <typename T>
const Transformation3D<T> Transformation3D<T>::rotation(XYZ iAxis, TParam iRadians)
{
	const T c = num::cos(iRadians);
	const T s = num::sin(iRadians);
	const size_t a = (iAxis + 1);
	const size_t b = (iAxis + 2);
	LASS_ASSERT(a < 3 && b < 3);

	Transformation3D<T> result;
	result.matrix_[5 * a] = c;
	result.matrix_[5 * b] = c;
	result.matrix_[4 * a + b] = -s;
	result.matrix_[4 * b + a] = s;
	return result;
}



/** make a 3D transformation representing a rotation around an arbitrary axis
 */
template <typename T>
const Transformation3D<T> Transformation3D<T>::rotation(const Vector3D<T>& iAxis, TParam iRadians)
{
	Vector3D<T> a = iAxis.normal();
	const T c = num::cos(iRadians);
	const T s = num::sin(iRadians);
	const TValue oneMinusC = TNumTraits::one - c;

	Transformation3D<T> result;
	result.matrix_[ 0] = a.x * a.x * oneMinusC + c;
	result.matrix_[ 1] = a.x * a.y * oneMinusC - a.z * s;
	result.matrix_[ 2] = a.x * a.z * oneMinusC + a.y * s;
	result.matrix_[ 4] = a.y * a.x * oneMinusC + a.z * s;
	result.matrix_[ 5] = a.y * a.y + oneMinusC + c;
	result.matrix_[ 6] = a.y * a.z * oneMinusC - a.x * s;
	result.matrix_[ 8] = a.z * a.x * oneMinusC - a.y * s;
	result.matrix_[ 9] = a.z * a.y * oneMinusC + a.x * s;
	result.matrix_[10] = a.z * a.z + oneMinusC + c;
	return result;
}



// --- protected -----------------------------------------------------------------------------------



// --- private -------------------------------------------------------------------------------------

template <typename T> inline
Transformation3D<T>::Transformation3D(const TMatrix& iMatrix, const TMatrix& iInverseMatrix, bool):
	matrix_(iMatrix),
	inverseMatrix_(iInverseMatrix)
{
}



// --- free ----------------------------------------------------------------------------------------

/** concatenate two transformations @a iA and @a iB in one.
 *  @relates Transformation3D
 *  The result is one transformation that performs the same actions as first performing
 *  @a iA and then @a iB.  Hence, the following lines of code are equivalent (ignoring
 *  numerical imprecions):
 *
 *  @code
 *	y = transform(x, concatenate(iA, iB));
 *  y = transform(transform(x, iA), iB);
 *  @endcode
 */
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



/** apply transformation to a vector
 *  @relates Transformation3D
 */
template <typename T>
Vector3D<T> transform(const Vector3D<T>& iSubject, const Transformation3D<T>& iTransformation)
{
	const T* const mat = iTransformation.matrix();
	return Vector3D<T>(
		mat[ 0] * iSubject.x + mat[ 1] * iSubject.y + mat[ 2] * iSubject.z,
		mat[ 4] * iSubject.x + mat[ 5] * iSubject.y + mat[ 6] * iSubject.z,
		mat[ 8] * iSubject.x + mat[ 9] * iSubject.y + mat[10] * iSubject.z);
}



/** apply transformation to a point
 *  @relates Transformation3D
 */
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



/** apply transformation to a normal vector.
 *  @relates Transformation3D
 *  Vectors that represent a normal vector should transform differentely than ordinary
 *  vectors.  Use this transformation function for normals.
 */
template <typename T>
Vector3D<T> normalTransform(const Vector3D<T>& iSubject, const Transformation3D<T>& iTransformation)
{
	const T* const invMat = iTransformation.inverse().matrix();
	return Vector3D<T>(
		invMat[ 0] * iSubject.x + invMat[ 4] * iSubject.y + invMat[ 8] * iSubject.z,
		invMat[ 1] * iSubject.x + invMat[ 5] * iSubject.y + invMat[ 9] * iSubject.z,
		invMat[ 2] * iSubject.x + invMat[ 6] * iSubject.y + invMat[10] * iSubject.z);
}



/** apply transformation to a 4D normal vector.
 *  @relates Transformation3D
 *  Vectors that represent a normal vector should transform differentely than ordinary
 *  vectors.  Use this transformation function for normals.
 *
 *  Cartesian planes have a 4D normal vector that must be transformed in 3D.  Use this
 *  function to do it:
 *
 *  @code
 *  // ax + by + cz + d == 0
 *	normalTransform(std::make_pair(Vector3D<float>(a, b, c), d), transformation);
 *  @endcode
 */
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



/** @relates Transformation3D
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



/** @relates Transformation3D
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
