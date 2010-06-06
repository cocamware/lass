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
 *	Copyright (C) 2004-2010 the Initial Developer.
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

#ifndef LASS_GUARDIAN_OF_INCLUSION_PRIM_TRANSFORMATION_2D_INL
#define LASS_GUARDIAN_OF_INCLUSION_PRIM_TRANSFORMATION_2D_INL

#include "transformation_2d.h"

#if LASS_COMPILER_TYPE == LASS_COMPILER_TYPE_MSVC
#	pragma warning(push)
#	pragma warning(disable:4996) // std::copy may be unsafe
#endif

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
	matrix_(impl::allocateArray<T>(matrixSize_)),
	inverseMatrix_(0)
{
	matrix_[ 0] = TNumTraits::one;
	matrix_[ 1] = TNumTraits::zero;
	matrix_[ 2] = TNumTraits::zero;
	matrix_[ 3] = TNumTraits::zero;
	matrix_[ 4] = TNumTraits::one;
	matrix_[ 5] = TNumTraits::zero;
	matrix_[ 6] = TNumTraits::zero;
	matrix_[ 7] = TNumTraits::zero;
	matrix_[ 8] = TNumTraits::one;
}



template <typename T>
Transformation2D<T>::Transformation2D(const TPoint& origin, const TVector& baseX, const TVector& baseY):
	matrix_(impl::allocateArray<T>(matrixSize_)),
	inverseMatrix_(0)
{
	matrix_[ 0] = baseX.x;
	matrix_[ 1] = baseY.x;
	matrix_[ 2] = origin.x;
	matrix_[ 3] = baseX.y;
	matrix_[ 4] = baseY.y;
	matrix_[ 5] = origin.y;
	matrix_[ 6] = TNumTraits::one;
	matrix_[ 7] = TNumTraits::zero;
	matrix_[ 8] = TNumTraits::zero;
}



/** construct a transformation from a 3x3 tranformation matrix.
 *  The elements of the 3x3 matrix will represented in a row major way by an iterator
 *  range [first, last) of 9 elements.
 */
template <typename T>
template <typename InputIterator>
Transformation2D<T>::Transformation2D(InputIterator first, InputIterator last):
	matrix_(impl::allocateArray<T>(matrixSize_)),
	inverseMatrix_(0)
{
	LASS_ENFORCE(std::distance(first, last) == matrixSize_);
	std::copy(first, last, matrix_.get());
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
		TMatrix inverseMatrix(impl::allocateArray<T>(matrixSize_));
		const TValue* const mat = matrix_.get();
		TValue* const inv = inverseMatrix.get();

		inv[0] = mat[4] * mat[8] - mat[5] * mat[7];
		inv[1] = mat[7] * mat[2] - mat[8] * mat[1];
		inv[2] = mat[1] * mat[5] - mat[2] * mat[4];

		inv[3] = mat[6] * mat[5] - mat[8] * mat[3];
		inv[4] = mat[0] * mat[8] - mat[2] * mat[6];
		inv[5] = mat[3] * mat[2] - mat[5] * mat[0];

		inv[6] = mat[3] * mat[7] - mat[4] * mat[6];
		inv[7] = mat[6] * mat[1] - mat[7] * mat[0];
		inv[8] = mat[0] * mat[4] - mat[1] * mat[3];

		const TValue det = mat[0] * inv[0] + mat[1] * inv[3] + mat[2] * inv[6];
		if (det == TNumTraits::zero)
		{
			inverseMatrix.reset();
			LASS_THROW_EX(util::SingularityError, "transformation not invertible");
		}
		const TValue invDet = num::inv(det);
		for (int i = 0; i < 9; ++i)
		{
			inv[i] *= invDet;
		}

		sync_.lock();
		inverseMatrix_.swap(inverseMatrix);
		sync_.unlock();
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



template <typename T>
bool Transformation2D<T>::isIdentity() const
{
	const TValue* const forward = matrix();
	for (size_t i = 0; i < 3; ++i)
	{
		for (size_t j = 0; j < 3; ++j)
		{
			if (forward[i * 3 + j] != (i == j ? TNumTraits::one : TNumTraits::zero))
			{
				return false;
			}
		}
	}
	return true;
}



template <typename T>
bool Transformation2D<T>::isTranslation() const
{
	const TValue* const forward = matrix();
	for (size_t i = 0; i < 3; ++i)
	{
		for (size_t j = 0; j < 3; ++j)
		{
			if (j < 2 && forward[i * 3 + j] != (i == j ? TNumTraits::one : TNumTraits::zero))
			{
				return false;
			}
		}
	}
	return true;
}



template <typename T>
void Transformation2D<T>::swap(TSelf& other)
{
	matrix_.swap(other.matrix_);
	inverseMatrix_.swap(other.inverseMatrix_);
}



/** make a 2D identity transformation 
 */
template <typename T> 
const Transformation2D<T> Transformation2D<T>::identity()
{
	return TSelf();
}



/** make a 2D transformation representing a translation
 */
template <typename T>
const Transformation2D<T> Transformation2D<T>::translation(const Vector2D<T>& offset)
{
	Transformation2D<T> result;
	result.matrix_[2] = offset.x;
	result.matrix_[5] = offset.y;
	return result;
}



/** make a 3D transformation representing a uniform scaling
 */
template <typename T>
const Transformation2D<T> Transformation2D<T>::scaler(const T& scale)
{
	Transformation2D<T> result;
	result.matrix_[0] = scale;
	result.matrix_[4] = scale;
	return result;
}



/** make a 3D transformation representing a scaling with different factors per axis
 */
template <typename T>
const Transformation2D<T> Transformation2D<T>::scaler(const Vector2D<T>& scale)
{
	Transformation2D<T> result;
	result.matrix_[0] = scale.x;
	result.matrix_[4] = scale.y;
	return result;
}



/** make a 3D transformation representing a counterclockwise rotation
 */
template <typename T>
const Transformation2D<T> Transformation2D<T>::rotation(TParam radians)
{
	const T c = num::cos(radians);
	const T s = num::sin(radians);

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
Transformation2D<T>::Transformation2D(const TMatrix& matrix, const TMatrix& inverseMatrix, bool):
	matrix_(matrix),
	inverseMatrix_(inverseMatrix)
{
}



// --- free ----------------------------------------------------------------------------------------

/** concatenate two transformations @a first and @a second in one.
 *  @relates Transformation2D
 *  The result is one transformation that performs the same actions as first performing
 *  @a first and then @a second.  Hence, the following lines of code are equivalent (ignoring
 *  numerical imprecions):
 *
 *  @code
 *	y = transform(x, concatenate(first, second));
 *  y = transform(transform(x, first), second);
 *  @endcode
 */
template <typename T>
Transformation2D<T> concatenate(const Transformation2D<T>& first, const Transformation2D<T>& second)
{
	// right-handed vector product, so it's @a second * @a first instead of @a first * @a second
	const T* const a = second.matrix();
	const T* const b = first.matrix();
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
	return Transformation2D<T>(result, result + 9);
}



/** apply transformation to a vector
 *  @relates Transformation2D
 */
template <typename T>
Vector2D<T> transform(const Vector2D<T>& subject, const Transformation2D<T>& transformation)
{
	const T* const mat = transformation.matrix();
	return Vector2D<T>(
		mat[0] * subject.x + mat[1] * subject.y,
		mat[3] * subject.x + mat[4] * subject.y);
}



/** apply transformation to a point
 *  @relates Transformation2D
 */
template <typename T>
Point2D<T> transform(const Point2D<T>& subject, const Transformation2D<T>& transformation)
{
	const T* const mat = transformation.matrix();
	const T weight = num::inv(mat[6] * subject.x + mat[7] * subject.y + mat[8]);
	return Point2D<T>(
		weight * (mat[0] * subject.x + mat[1] * subject.y + mat[2]),
		weight * (mat[3] * subject.x + mat[4] * subject.y + mat[5]));

}



/** apply transformation to a normal vector.
 *  @relates Transformation2D
 *  Vectors that represent a normal vector should transform differentely than ordinary
 *  vectors.  Use this transformation function for normals.
 */
template <typename T>
Vector2D<T> normalTransform(const Vector2D<T>& subject, const Transformation2D<T>& transformation)
{
	const T* const invMat = transformation.inverse().matrix();
	return Vector2D<T>(
		invMat[0] * subject.x + invMat[3] * subject.y,
		invMat[1] * subject.x + invMat[4] * subject.y);
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
std::pair<Vector2D<T>, T> normalTransform(const std::pair<Vector2D<T>, T>& subject, 
										  const Transformation2D<T>& transformation)
{
	const T* const invMat = transformation.inverse().matrix();
	const Vector2D<T>& n = subject.first;
	const T c = subject.second;
	return std::make_pair(
		Vector2D<T>(
			invMat[0] * n.x + invMat[3] * n.y + invMat[6] * c,
			invMat[1] * n.x + invMat[4] * n.y + invMat[7] * c),
		invMat[2] * n.x + invMat[5] * n.y + invMat[8] * c);
}



/** @relates Transformation2D
 */
template<typename T, typename Char, typename Traits>
std::basic_ostream<Char, Traits>& operator<<(std::basic_ostream<Char, Traits>& stream,
											 const Transformation2D<T>& transformation)
{
	const T* const mat = transformation.matrix();
	LASS_ENFORCE_STREAM(stream) << "(("
		<< mat[0] << ", " << mat[1] << ", " << mat[2] << "), ("
		<< mat[3] << ", " << mat[4] << ", " << mat[5] << "), ("
		<< mat[6] << ", " << mat[7] << ", " << mat[9] << "))";
	return stream;
}



/** @relates Transformation2D
 */
template<typename T>
io::XmlOStream& operator<<(io::XmlOStream& stream, const Transformation2D<T>& transformation)
{
	const T* const mat = transformation.matrix();
	LASS_ENFORCE_STREAM(stream) << "<Transformation2D>"
		<< mat[0] << " " << mat[1] << " " << mat[2] << " "
		<< mat[3] << " " << mat[4] << " " << mat[5] << " "
		<< mat[6] << " " << mat[7] << " " << mat[9]
		<< "</Transformation2D>\n";
	return stream;
}



}

}

#if LASS_COMPILER_TYPE == LASS_COMPILER_TYPE_MSVC
#	pragma warning(pop)
#endif

#endif
