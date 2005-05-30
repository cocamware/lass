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



#ifndef LASS_GUARDIAN_OF_INCLUSION_PRIM_TRANSFORMATION_2D_INL
#define LASS_GUARDIAN_OF_INCLUSION_PRIM_TRANSFORMATION_2D_INL




#include "transformation_2d.h"

namespace lass
{

namespace prim
{

// --- public --------------------------------------------------------------------------------------

/** construct an identity transformation.
 *  An identity transformation transforms every point to itself.
 */
template <typename T>
Transformation2D<T>::Transformation2D():
	matrix_(new TValue[matrixSize_]),
	inverseMatrix_(0)
{
	matrix_[ 0] = TNumTraits::one;
	matrix_[ 1] = TNumTraits::zero;
	matrix_[ 2] = TNumTraits::zero;
	matrix_[ 3] = TNumTraits::one;
	matrix_[ 4] = TNumTraits::zero;
	matrix_[ 5] = TNumTraits::zero;
	matrix_[ 6] = TNumTraits::one;
	matrix_[ 7] = TNumTraits::zero;
	matrix_[ 8] = TNumTraits::zero;
}



/** construct a transformation from a 3x3 tranformation matrix.
 *  The elements of the 3x3 matrix will represented in a row major way by an iterator
 *  range [iBegin, iEnd) of 9 elements.
 */
template <typename T>
template <typename InputIterator>
Transformation2D<T>::Transformation2D(InputIterator iBegin, InputIterator iEnd):
	matrix_(new TValue[matrixSize_]),
	inverseMatrix_(0)
{
	LASS_ENFORCE(std::distance(iBegin, iEnd) == matrixSize_);
	std::copy(iBegin, iEnd, matrix_.get());
}



/** return the inverse transformation.
 *  The inverse is calculated on the first call, and then cached for later use.
 *  For the transformation, we've use the C version of Cramer's rule as described in
 *  the Intel (R) article "Streaming SIMD Extensions -Inverse of 4x4 Matrix" which
 *  can be found here: http://www.intel.com/design/pentiumiii/sml/245043.htm
 */
template <typename T>
const Transformation2D<T>
Transformation2D<T>::inverse() const
{
	if (inverseMatrix_.isEmpty())
	{
		inverseMatrix_.reset(new TValue[matrixSize_]);

		inverseMatrix_[0] = matrix_[4] * matrix_[8] - matrix_[5] * matrix_[7];
		inverseMatrix_[1] = matrix_[7] * matrix_[2] - matrix_[8] * matrix_[1];
		inverseMatrix_[2] = matrix_[1] * matrix_[5] - matrix_[2] * matrix_[4];

		inverseMatrix_[3] = matrix_[6] * matrix_[5] - matrix_[8] * matrix_[3];
		inverseMatrix_[4] = matrix_[0] * matrix_[8] - matrix_[2] * matrix_[6];
		inverseMatrix_[5] = matrix_[3] * matrix_[2] - matrix_[5] * matrix_[0];

		inverseMatrix_[6] = matrix_[3] * matrix_[7] - matrix_[4] * matrix_[6];
		inverseMatrix_[7] = matrix_[6] * matrix_[1] - matrix_[7] * matrix_[0];
		inverseMatrix_[8] = matrix_[0] * matrix_[4] - matrix_[1] * matrix_[3];

		const TValue det = matrix_[0] * inverseMatrix_[0] + matrix_[1] * inverseMatrix_[3] + matrix_[2] * inverseMatrix_[6];
		if (det == TNumTraits::zero)
		{
			inverseMatrix_.reset();
			LASS_THROW("transformation not invertible");
		}
		const TValue invDet = num::inv(det);
		std::transform(inverseMatrix_.get(), inverseMatrix_.get() + matrixSize_,
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
const typename Transformation2D<T>::TValue*
Transformation2D<T>::matrix() const
{
	return matrix_.get();
}



/** make a 2D transformation representing a translation
 */
template <typename T>
const Transformation2D<T> Transformation2D<T>::translation(const Vector2D<T>& iOffset)
{
	Transformation2D<T> result;
	result.matrix_[2] = iOffset.x;
	result.matrix_[5] = iOffset.y;
	result.matrix_[8] = iOffset.z;
	return result;
}



/** make a 3D transformation representing a uniform scaling
 */
template <typename T>
const Transformation2D<T> Transformation2D<T>::scaler(const T& iScale)
{
	Transformation2D<T> result;
	result.matrix_[0] = iScale;
	result.matrix_[4] = iScale;
	return result;
}



/** make a 3D transformation representing a scaling with different factors per axis
 */
template <typename T>
const Transformation2D<T> Transformation2D<T>::scaler(const Vector2D<T>& iScale)
{
	Transformation2D<T> result;
	result.matrix_[0] = iScale.x;
	result.matrix_[4] = iScale.y;
	return result;
}



/** make a 3D transformation representing a counterclockwise rotation
 */
template <typename T>
const Transformation2D<T> Transformation2D<T>::rotation(TParam iRadians)
{
	const T c = num::cos(iRadians);
	const T s = num::sin(iRadians);

	Transformation2D<T> result;
	result.matrix_[0] = c;
	result.matrix_[4] = c;
	result.matrix_[1] = -s;
	result.matrix_[3] = s;
	return result;
}



// --- protected -----------------------------------------------------------------------------------



// --- private -------------------------------------------------------------------------------------

template <typename T> inline
Transformation2D<T>::Transformation2D(const TMatrix& iMatrix, const TMatrix& iInverseMatrix, bool):
	matrix_(iMatrix),
	inverseMatrix_(iInverseMatrix)
{
}



// --- free ----------------------------------------------------------------------------------------

/** concatenate two transformations @a iA and @a iB in one.
 *  @relates Transformation2D
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
Transformation2D<T> concatenate(const Transformation2D<T>& iA, const Transformation2D<T>& iB)
{
	const T* const a = iA.matrix();
	const T* const b = iB.matrix();
	T result[9];
	for (size_t i = 0; i < 9; i += 3)
	{
		for (size_t j = 0; j < 3; ++j)
		{
			result[i + j] = 
				a[i    ] * b[    j] + 
				a[i + 1] * b[3 + j] + 
				a[i + 2] * b[6 + j];
		}
	}
	return Transformation2D<T>(result, result + matrixSize_);
}



/** apply transformation to a vector
 *  @relates Transformation2D
 */
template <typename T>
Vector2D<T> transform(const Vector2D<T>& iSubject, const Transformation2D<T>& iTransformation)
{
	const T* const mat = iTransformation.matrix();
	return Vector2D<T>(
		mat[0] * iSubject.x + mat[1] * iSubject.y,
		mat[3] * iSubject.x + mat[4] * iSubject.y);
}



/** apply transformation to a point
 *  @relates Transformation2D
 */
template <typename T>
Point3D<T> transform(const Point3D<T>& iSubject, const Transformation2D<T>& iTransformation)
{
	const T* const mat = iTransformation.matrix();
	const T weight = num::inv(mat[6] * iSubject.x + mat[7] * iSubject.y + mat[8]);
	return Point3D<T>(
		weight * (mat[0] * iSubject.x + mat[1] * iSubject.y + mat[2]),
		weight * (mat[3] * iSubject.x + mat[4] * iSubject.y + mat[5]));

}



/** apply transformation to a normal vector.
 *  @relates Transformation2D
 *  Vectors that represent a normal vector should transform differentely than ordinary
 *  vectors.  Use this transformation function for normals.
 */
template <typename T>
Vector2D<T> normalTransform(const Vector2D<T>& iSubject, const Transformation2D<T>& iTransformation)
{
	const T* const invMat = iTransformation.inverse().matrix();
	return Vector2D<T>(
		invMat[0] * iSubject.x + invMat[3] * iSubject.y,
		invMat[1] * iSubject.x + invMat[4] * iSubject.y);
}



/** apply transformation to a 3D normal vector.
 *  @relates Transformation2D
 *  Vectors that represent a normal vector should transform differentely than ordinary
 *  vectors.  Use this transformation function for normals.
 *
 *  Cartesian lines have a 3D normal vector that must be transformed in 2D.  Use this
 *  function to do it:
 *
 *  @code
 *  // ax + by + c == 0
 *	normalTransform(std::make_pair(Vector2D<float>(a, b), c), transformation);
 *  @endcode
 */
template <typename T>
std::pair<Vector2D<T>, T> normalTransform(const std::pair<Vector2D<T>, T>& iSubject, 
										  const Transformation2D<T>& iTransformation)
{
	const T* const invMat = iTransformation.inverse().matrix();
	const Vector2D<T>& n = iSubject.first;
	const T c = iSubject.second;
	return std::make_pair(
		Vector2D<T>(
			invMat[0] * n.x + invMat[3] * n.y + invMat[6] * c,
			invMat[1] * n.x + invMat[4] * n.y + invMat[7] * c),
		invMat[2] * n.x + invMat[5] * n.y + invMat[8] * c));
}



/** @relates Transformation2D
 */
template<typename T, typename Char, typename Traits>
std::basic_ostream<Char, Traits>& operator<<(std::basic_ostream<Char, Traits>& oOStream,
											 const Transformation2D<T>& iTransformation)
{
	const T* const mat = iTransformation.matrix();
	LASS_ENFORCE_STREAM(oOStream) << "(("
		<< mat[0] << ", " << mat[1] << ", " << mat[2] << "), ("
		<< mat[3] << ", " << mat[4] << ", " << mat[5] << "), ("
		<< mat[6] << ", " << mat[7] << ", " << mat[9] << "))";
	return oOStream;
}



/** @relates Transformation2D
 */
template<typename T>
io::XmlOStream& operator<<(io::XmlOStream& oOStream, const Transformation2D<T>& iTransformation)
{
	const T* const mat = iTransformation.matrix();
	LASS_ENFORCE_STREAM(oOStream) << "<Transformation2D>"
		<< mat[0] << " " << mat[1] << " " << mat[2] << " "
		<< mat[3] << " " << mat[4] << " " << mat[5] << " "
		<< mat[6] << " " << mat[7] << " " << mat[9]
		<< "</Transformation2D>\n";
	return oOStream;
}



}

}

#endif
