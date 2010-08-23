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

#ifndef LASS_GUARDIAN_OF_INCLUSION_PRIM_TRANSFORMATION_3D_INL
#define LASS_GUARDIAN_OF_INCLUSION_PRIM_TRANSFORMATION_3D_INL

#include "transformation_3d.h"

#if LASS_COMPILER_TYPE == LASS_COMPILER_TYPE_MSVC
#	pragma warning(push)
#	pragma warning(disable: 4996) // std::copy: function call with parameters that may be unsafe
#endif

namespace lass
{

namespace prim
{

template <typename T> typename Transformation3D<T>::TImplPtr Transformation3D<T>::identity_ = Transformation3D<T>::makeIdentity();

// --- public --------------------------------------------------------------------------------------

/** construct an identity transformation.
 *  An identity transformation transforms every point to itself.
 */
template <typename T>
Transformation3D<T>::Transformation3D():
	pimpl_(identity_),
	isInversed_(false)
{
}



/** construct an matrix from an origin and three base vectors
 *  
 *	@code
 *	TVector3D<T> i, j, k;
 *	TPoint3D<T> o, p;
 *	// ...
 *
 *	Transformation3D<T> transformation(o, i, j, k);
 *	LASS_ASSERT(transform(p, transformation) == (o + i * p.x + j * p.y + k * p.z));
 *	@endcode
 */
template <typename T>
Transformation3D<T>::Transformation3D(
		const TPoint& origin, const TVector& baseX, const TVector& baseY, const TVector& baseZ):
	pimpl_(TImplPtr::allocate()),
	isInversed_(false)
{
	TValue* mat = pimpl_->forward;
	mat[ 0] = baseX.x;	mat[ 1] = baseY.x;	mat[ 2] = baseZ.x;	mat[ 3] = origin.x;
	mat[ 4] = baseX.y;	mat[ 5] = baseY.y;	mat[ 6] = baseZ.y;	mat[ 7] = origin.y;
	mat[ 8] = baseX.z;	mat[ 9] = baseY.z;	mat[10] = baseZ.z;	mat[11] = origin.z;
	mat[12] = 0;	mat[13] = 0;	mat[14] = 0;	mat[15] = 1;
}



/** construct a transformation from a 4x4 tranformation matrix.
 *  The elements of the 4x4 matrix will represented in a row major way by an iterator
 *  range [first, last) of 16 elements.
 */
template <typename T>
template <typename InputIterator>
Transformation3D<T>::Transformation3D(InputIterator first, InputIterator last):
	pimpl_(TImplPtr::allocate()),
	isInversed_(false)
{
	LASS_ENFORCE(std::distance(first, last) == matrixSize_);
	std::copy(first, last, pimpl_->forward);
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
	computeInverse();
	LASS_ASSERT(pimpl_->hasInverse);
	return TSelf(pimpl_, !isInversed_);
}



/** Return pointer to row major matrix representation of transformation.
 *  This is for immediate use only, like @c std::basic_string::data().
 */
template <typename T> inline
const typename Transformation3D<T>::TValue*
Transformation3D<T>::matrix() const
{
	LASS_ASSERT(!(isInversed_ && !pimpl_->hasInverse));
	return isInversed_ ? pimpl_->inverse : pimpl_->forward;
}



/** Return pointer to row major matrix representation of inverse transformation.
 *  This is for immediate use only, like @c std::basic_string::data().
 */
template <typename T> inline
const typename Transformation3D<T>::TValue*
Transformation3D<T>::inverseMatrix() const
{
	computeInverse();
	LASS_ASSERT(pimpl_->hasInverse);
	return isInversed_ ? pimpl_->forward : pimpl_->inverse;
}



template <typename T>
bool Transformation3D<T>::isIdentity() const
{
	if (pimpl_ == identity_)
	{
		return true;
	}
	const TValue* const forward = pimpl_->forward;
	for (size_t i = 0; i < 4; ++i)
	{
		for (size_t j = 0; j < 4; ++j)
		{
			if (forward[i * 4 + j] != (i == j ? TNumTraits::one : TNumTraits::zero))
			{
				return false;
			}
		}
	}
	return true;
}



template <typename T>
bool Transformation3D<T>::isTranslation() const
{
	if (pimpl_->isTranslation)
	{
		return true;
	}
	const TValue* const forward = pimpl_->forward;
	for (size_t i = 0; i < 4; ++i)
	{
		for (size_t j = 0; j < 4; ++j)
		{
			if (j < 3 && forward[i * 4 + j] != (i == j ? TNumTraits::one : TNumTraits::zero))
			{
				return false;
			}
		}
	}
	return true;
}



template <typename T>
void Transformation3D<T>::swap(TSelf& other)
{
	pimpl_.swap(other.pimpl_);
	std::swap(isInversed_, other.isInversed_);
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
const Transformation3D<T> Transformation3D<T>::translation(const TVector& offset)
{
	TImplPtr pimpl(TImplPtr::allocate());

	TValue* const forward = pimpl->forward;
	identity(forward);
	forward[ 3] = offset.x;
	forward[ 7] = offset.y;
	forward[11] = offset.z;

	TValue* const inverse = pimpl->inverse;
	identity(inverse);
	inverse[ 3] = -offset.x;
	inverse[ 7] = -offset.y;
	inverse[11] = -offset.z;

	pimpl->hasInverse = true;
	pimpl->isTranslation = true;

	return TSelf(pimpl);
}



/** make a 3D transformation representing a uniform scaling
 */
template <typename T>
const Transformation3D<T> Transformation3D<T>::scaler(TParam scale)
{
	TImplPtr pimpl(TImplPtr::allocate());

	TValue* const forward = pimpl->forward;
	identity(forward);
	forward[ 0] = forward[ 5] = forward[10] = scale;

	TValue* const inverse = pimpl->inverse;
	identity(inverse);
	inverse[ 0] = inverse[ 5] = inverse[10] = num::inv(scale);
	
	pimpl->hasInverse = true;

	return TSelf(pimpl);
}



/** make a 3D transformation representing a scaling with different factors per axis
 */
template <typename T>
const Transformation3D<T> Transformation3D<T>::scaler(const TVector& scale)
{
	TImplPtr pimpl(TImplPtr::allocate());

	TValue* const forward = pimpl->forward;
	identity(forward);
	forward[ 0] = scale.x;
	forward[ 5] = scale.y;
	forward[10] = scale.z;

	TValue* const inverse = pimpl->inverse;
	identity(inverse);
	inverse[ 0] = num::inv(scale.x);
	inverse[ 5] = num::inv(scale.y);
	inverse[10] = num::inv(scale.z);
	
	pimpl->hasInverse = true;

	return TSelf(pimpl);
}



/** make a 3D transformation representing a rotation around a primary axis
 */
template <typename T>
const Transformation3D<T> Transformation3D<T>::rotation(XYZ axis, TParam radians)
{
	TImplPtr pimpl(TImplPtr::allocate());

	const T c = num::cos(radians);
	const T s = num::sin(radians);
	const size_t a = (axis + 1);
	const size_t b = (axis + 2);
	LASS_ASSERT(a < 3 && b < 3);

	TValue* const forward = pimpl->forward;
	identity(forward);
	forward[5 * a] = c;
	forward[5 * b] = c;
	forward[4 * a + b] = -s;
	forward[4 * b + a] = s;

	TValue* const inverse = pimpl->inverse;
	identity(forward);
	inverse[5 * a] = c;
	inverse[5 * b] = c;
	inverse[4 * a + b] = s;
	inverse[4 * b + a] = -s;

	pimpl->hasInverse = true;

	return TSelf(pimpl);
}



/** make a 3D transformation representing a rotation around an arbitrary axis
 */
template <typename T>
const Transformation3D<T> Transformation3D<T>::rotation(const TVector& axis, TParam radians)
{
	TImplPtr pimpl(TImplPtr::allocate());

	const TVector a = axis.normal();
	const T c = num::cos(radians);
	const T s = num::sin(radians);
	const TValue oneMinusC = TNumTraits::one - c;

	TValue* const forward = pimpl->forward;
	identity(forward);
	forward[ 0] = a.x * a.x * oneMinusC + c;
	forward[ 1] = a.x * a.y * oneMinusC - a.z * s;
	forward[ 2] = a.x * a.z * oneMinusC + a.y * s;
	forward[ 4] = a.y * a.x * oneMinusC + a.z * s;
	forward[ 5] = a.y * a.y * oneMinusC + c;
	forward[ 6] = a.y * a.z * oneMinusC - a.x * s;
	forward[ 8] = a.z * a.x * oneMinusC - a.y * s;
	forward[ 9] = a.z * a.y * oneMinusC + a.x * s;
	forward[10] = a.z * a.z * oneMinusC + c;

	translate(pimpl->forward, pimpl->inverse);

	pimpl->hasInverse = true;

	return TSelf(pimpl);
}



template <typename T>
const Transformation3D<T> Transformation3D<T>::lookAt(const TPoint& eye, const TPoint& target, const TVector& sky)
{
	const TVector dir = target - eye;
	const TVector right = cross(dir, sky);
	const TVector up = cross(right, dir);
	return Transformation3D<T>(eye, right.normal(), dir.normal(), up.normal());
}



// --- protected -----------------------------------------------------------------------------------



// --- private -------------------------------------------------------------------------------------

template <typename T> inline
Transformation3D<T>::Transformation3D(const TImplPtr& impl, bool isInversed):
	pimpl_(impl),
	isInversed_(isInversed)
{
}



template <typename T>
void Transformation3D<T>::computeInverse() const
{	
	LASS_LOCK(pimpl_->sync)
	{
		if (pimpl_->hasInverse)
		{
			return;
		}

		const TValue* const mat = pimpl_->forward;
		TValue* const inv = pimpl_->inverse;

		const TValue v1015 = mat[10] * mat[15];
		const TValue v1411 = mat[14] * mat[11];
		const TValue v0615 = mat[ 6] * mat[15];
		const TValue v1407 = mat[14] * mat[ 7];
		const TValue v0611 = mat[ 6] * mat[11];
		const TValue v1007 = mat[10] * mat[ 7];
		const TValue v0215 = mat[ 2] * mat[15];
		const TValue v1403 = mat[14] * mat[ 3];
		const TValue v0211 = mat[ 2] * mat[11];
		const TValue v1003 = mat[10] * mat[ 3];
		const TValue v0207 = mat[ 2] * mat[ 7];
		const TValue v0603 = mat[ 6] * mat[ 3];

		inv[0] = v1015 * mat[ 5] + v1407 * mat[ 9] + v0611 * mat[13]
			   - v1411 * mat[ 5] - v0615 * mat[ 9] - v1007 * mat[13];
		inv[1] = v1411 * mat[ 1] + v0215 * mat[ 9] + v1003 * mat[13]
			   - v1015 * mat[ 1] - v1403 * mat[ 9] - v0211 * mat[13];
		inv[2] = v0615 * mat[ 1] + v1403 * mat[ 5] + v0207 * mat[13]
			   - v1407 * mat[ 1] - v0215 * mat[ 5] - v0603 * mat[13];
		inv[3] = v1007 * mat[ 1] + v0211 * mat[ 5] + v0603 * mat[ 9]
			   - v0611 * mat[ 1] - v1003 * mat[ 5] - v0207 * mat[ 9];
		inv[4] = v1411 * mat[ 4] + v0615 * mat[ 8] + v1007 * mat[12]
			   - v1015 * mat[ 4] - v1407 * mat[ 8] - v0611 * mat[12];
		inv[5] = v1015 * mat[ 0] + v1403 * mat[ 8] + v0211 * mat[12]
			   - v1411 * mat[ 0] - v0215 * mat[ 8] - v1003 * mat[12];
		inv[6] = v1407 * mat[ 0] + v0215 * mat[ 4] + v0603 * mat[12]
			   - v0615 * mat[ 0] - v1403 * mat[ 4] - v0207 * mat[12];
		inv[7] = v0611 * mat[ 0] + v1003 * mat[ 4] + v0207 * mat[ 8]
			   - v1007 * mat[ 0] - v0211 * mat[ 4] - v0603 * mat[ 8];

		const TValue v0813 = mat[ 8] * mat[13];
		const TValue v1209 = mat[12] * mat[ 9];
		const TValue v0413 = mat[ 4] * mat[13];
		const TValue v1205 = mat[12] * mat[ 5];
		const TValue v0409 = mat[ 4] * mat[ 9];
		const TValue v0805 = mat[ 8] * mat[ 5];
		const TValue v0013 = mat[ 0] * mat[13];
		const TValue v1201 = mat[12] * mat[ 1];
		const TValue v0009 = mat[ 0] * mat[ 9];
		const TValue v0801 = mat[ 8] * mat[ 1];
		const TValue v0005 = mat[ 0] * mat[ 5];
		const TValue v0401 = mat[ 4] * mat[ 1];

		inv[ 8] = v0813 * mat[ 7] + v1205 * mat[11] + v0409 * mat[15]
				- v1209 * mat[ 7] - v0413 * mat[11] - v0805 * mat[15];
		inv[ 9] = v1209 * mat[ 3] + v0013 * mat[11] + v0801 * mat[15]
				- v0813 * mat[ 3] - v1201 * mat[11] - v0009 * mat[15];
		inv[10] = v0413 * mat[ 3] + v1201 * mat[ 7] + v0005 * mat[15]
				- v1205 * mat[ 3] - v0013 * mat[ 7] - v0401 * mat[15];
		inv[11] = v0805 * mat[ 3] + v0009 * mat[ 7] + v0401 * mat[11]
				- v0409 * mat[ 3] - v0801 * mat[ 7] - v0005 * mat[11];
		inv[12] = v0413 * mat[10] + v0805 * mat[14] + v1209 * mat[ 6]
				- v0409 * mat[14] - v0813 * mat[ 6] - v1205 * mat[10];
		inv[13] = v0009 * mat[14] + v0813 * mat[ 2] + v1201 * mat[10]
				- v0013 * mat[10] - v0801 * mat[14] - v1209 * mat[ 2];
		inv[14] = v0013 * mat[ 6] + v0401 * mat[14] + v1205 * mat[ 2]
				- v0005 * mat[14] - v0413 * mat[ 2] - v1201 * mat[ 6];
		inv[15] = v0005 * mat[10] + v0409 * mat[ 2] + v0801 * mat[ 6]
				- v0009 * mat[ 6] - v0401 * mat[10] - v0805 * mat[ 2];

		const TValue det = mat[0] * inv[0] + mat[4] * inv[1] + mat[8] * inv[2] + mat[12] * inv[3];
		if (det == TNumTraits::zero)
		{
			LASS_THROW_EX(util::SingularityError, "transformation not invertible");
		}
		const TValue invDet = num::inv(det);
		for (size_t i = 0; i < matrixSize_; ++i)
		{
			inv[i] *= invDet;
		}
		pimpl_->hasInverse = true;
	}
}



template <typename T>
void Transformation3D<T>::translate(const TValue* source, TValue* dest)
{
	for(size_t i = 0; i < 4; ++i)
	{
		for(size_t j = 0; j < 4; ++j)
		{
			dest[4 * i + j] = source[4 * j + i];
		}
	}
}



template <typename T>
void Transformation3D<T>::identity(TValue* dest)
{
	static TValue mat[matrixSize_] = 
	{
		1, 0, 0, 0,
		0, 1, 0, 0,
		0, 0, 1, 0,
		0, 0, 0, 1,
	};
	std::copy(mat, mat + matrixSize_, dest);
}



template <typename T>
typename Transformation3D<T>::TImplPtr Transformation3D<T>::makeIdentity()
{
	TImplPtr pimpl(TImplPtr::allocate());
	identity(pimpl->forward);
	identity(pimpl->inverse);
	pimpl->hasInverse = true;
	pimpl->isTranslation = true;
	std::cout << "been here";
	return pimpl;
}



// --- free ----------------------------------------------------------------------------------------

/** concatenate two transformations @a first and @a second in one.
 *  @relates Transformation3D
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
Transformation3D<T> concatenate(const Transformation3D<T>& first, const Transformation3D<T>& second)
{
	// right-handed vector product, so it's @a second * @a first instead of @a first * @a second
	const T* const a = second.matrix();
	const T* const b = first.matrix();
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
Vector3D<T> transform(const Vector3D<T>& subject, const Transformation3D<T>& transformation)
{
	const T* const mat = transformation.matrix();
	return Vector3D<T>(
		mat[ 0] * subject.x + mat[ 1] * subject.y + mat[ 2] * subject.z,
		mat[ 4] * subject.x + mat[ 5] * subject.y + mat[ 6] * subject.z,
		mat[ 8] * subject.x + mat[ 9] * subject.y + mat[10] * subject.z);
}



/** apply transformation to a point
 *  @relates Transformation3D
 */
template <typename T>
Point3D<T> transform(const Point3D<T>& subject, const Transformation3D<T>& transformation)
{
	const T* const mat = transformation.matrix();
	const T weight = num::inv(mat[12] * subject.x + mat[13] * subject.y + mat[14] * subject.z + mat[15]);
	return Point3D<T>(
		weight * (mat[ 0] * subject.x + mat[ 1] * subject.y + mat[ 2] * subject.z + mat[ 3]),
		weight * (mat[ 4] * subject.x + mat[ 5] * subject.y + mat[ 6] * subject.z + mat[ 7]),
		weight * (mat[ 8] * subject.x + mat[ 9] * subject.y + mat[10] * subject.z + mat[11]));

}



/** apply transformation to a normal vector.
 *  @relates Transformation3D
 *  Vectors that represent a normal vector should transform differentely than ordinary
 *  vectors.  Use this transformation function for normals.
 */
template <typename T>
Vector3D<T> normalTransform(const Vector3D<T>& subject, const Transformation3D<T>& transformation)
{
	const T* const invMat = transformation.inverseMatrix();
	return Vector3D<T>(
		invMat[ 0] * subject.x + invMat[ 4] * subject.y + invMat[ 8] * subject.z,
		invMat[ 1] * subject.x + invMat[ 5] * subject.y + invMat[ 9] * subject.z,
		invMat[ 2] * subject.x + invMat[ 6] * subject.y + invMat[10] * subject.z);
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
std::pair<Vector3D<T>, T> normalTransform(const std::pair<Vector3D<T>, T>& subject, 
										  const Transformation3D<T>& transformation)
{
	const T* const invMat = transformation.inverseMatrix();
	const Vector3D<T>& n = subject.first;
	const T d = subject.second;
	const Vector3D<T> transformedN(
		invMat[ 0] * n.x + invMat[ 4] * n.y + invMat[ 8] * n.z + invMat[12] * d,
		invMat[ 1] * n.x + invMat[ 5] * n.y + invMat[ 9] * n.z + invMat[13] * d,
		invMat[ 2] * n.x + invMat[ 6] * n.y + invMat[10] * n.z + invMat[14] * d);
	const T transformedD = 
		invMat[ 3] * n.x + invMat[ 7] * n.y + invMat[11] * n.z + invMat[15] * d;
	return std::make_pair(transformedN, transformedD);
}



/** @relates Transformation3D
 */
template<typename T, typename Char, typename Traits>
std::basic_ostream<Char, Traits>& operator<<(std::basic_ostream<Char, Traits>& stream,
											 const Transformation3D<T>& transformation)
{
	const T* const mat = transformation.matrix();
	LASS_ENFORCE_STREAM(stream) << "(("
		<< mat[ 0] << ", " << mat[ 1] << ", " << mat[ 2] << ", " << mat[ 3] << "), ("
		<< mat[ 4] << ", " << mat[ 5] << ", " << mat[ 6] << ", " << mat[ 7] << "), ("
		<< mat[ 8] << ", " << mat[ 9] << ", " << mat[10] << ", " << mat[11] << "), ("
		<< mat[12] << ", " << mat[13] << ", " << mat[14] << ", " << mat[15] << "))";
	return stream;
}



/** @relates Transformation3D
 */
template<typename T>
io::XmlOStream& operator<<(io::XmlOStream& stream, const Transformation3D<T>& transformation)
{
	const T* const mat = transformation.matrix();
	LASS_ENFORCE_STREAM(stream) << "<Transformation3D>"
		<< mat[ 0] << " " << mat[ 1] << " " << mat[ 2] << " " << mat[ 3] << " "
		<< mat[ 4] << " " << mat[ 5] << " " << mat[ 6] << " " << mat[ 7] << " "
		<< mat[ 8] << " " << mat[ 9] << " " << mat[10] << " " << mat[11] << " "
		<< mat[12] << " " << mat[13] << " " << mat[14] << " " << mat[15]
		<< "</Transformation3D>\n";
	return stream;
}



}

}

#if LASS_COMPILER_TYPE == LASS_COMPILER_TYPE_MSVC
#	pragma warning(pop)
#endif

#endif
