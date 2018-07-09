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
 *	Copyright (C) 2004-2011 the Initial Developer.
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



/** @class lass::prim::Transformation3D
 *  @brief a linear 3D transformation
 *  @author Bram de Greve [BdG]
 */

#ifndef LASS_GUARDIAN_OF_INCLUSION_PRIM_TRANSFORMATION_3D_H
#define LASS_GUARDIAN_OF_INCLUSION_PRIM_TRANSFORMATION_3D_H

#include "prim_common.h"
#include "point_3d.h"
#include "xyz.h"
#include "../io/xml_o_stream.h"
#include "../num/num_traits.h"
#include "../util/allocator.h"
#include "../util/shared_ptr.h"
#include "../util/thread.h"

namespace lass
{
namespace prim
{
namespace impl
{

template <typename T, size_t N>
struct LASS_SIMD_ALIGN TransformationImpl
{
	enum { matrixSize = N };
	LASS_SIMD_ALIGN T forward[matrixSize];
	LASS_SIMD_ALIGN T inverse[matrixSize];
	util::Semaphore sync;
	size_t referenceCount;
	bool hasInverse;
	bool isTranslation;
	TransformationImpl(): referenceCount(0), hasInverse(false), isTranslation(false) {}
};

template <typename U, typename Cascade>
class TransformationImplStorage: public util::ObjectStorage<U, Cascade>
{
public:
	TransformationImplStorage(): util::ObjectStorage<U, Cascade>() {}
	TransformationImplStorage(U* p): util::ObjectStorage<U, Cascade>(p) {}
	static U* allocate() { return allocator().allocate(); }
protected:
	void dispose()
	{
		allocator().deallocate(this->pointer());
	}
private:
	typedef util::AllocatorObject< U , util::AllocatorConcurrentFreeList<> > TAllocator;
	static TAllocator& allocator()
	{
		// this is _not_ thread safe, but it should be OK as it's called once
		// before main, to init the identity matrix.
		static TAllocator alloc;
		return alloc;
	}
};

}

template <typename T>
class Transformation3D
{
public:

	typedef Transformation3D<T> TSelf;

	typedef Point3D<T> TPoint;
	typedef typename TPoint::TVector TVector;
	typedef typename util::CallTraits<T>::TValue TValue;
	typedef typename util::CallTraits<T>::TParam TParam;
	typedef typename util::CallTraits<T>::TReference TReference;
	typedef typename util::CallTraits<T>::TConstReference TConstReference;
	typedef num::NumTraits<T> TNumTraits;
	typedef size_t TSize;

	enum { dimension = TPoint::dimension }; /**< number of dimensions of vector */

	template <typename U> struct Rebind
	{
		typedef Transformation3D<U> Type;
	};

	Transformation3D();
	Transformation3D(const TPoint& origin, const TVector& baseX, const TVector& baseY, 
		const TVector& baseZ);
	template <typename InputIterator> Transformation3D(InputIterator first, InputIterator last);

	const TSelf inverse() const;

	const TValue* matrix() const;
	const TValue* inverseMatrix() const;

	bool isIdentity() const;
	bool isTranslation() const;

	void swap(TSelf& other);

	static const TSelf identity();
	static const TSelf translation(const TVector& offset);
	static const TSelf scaler(TParam scale);
	static const TSelf scaler(const TVector& scale);
	static const TSelf rotation(XYZ axis, TParam radians);
	static const TSelf rotation(const TVector& axis, TParam radians);
	static const TSelf lookAt(const TPoint& eye, const TPoint& target, const TVector& sky);

private:

	enum { matrixSize_ = 16 };

	typedef T TMatrix[matrixSize_];
	typedef const T TConstMatrix[matrixSize_];
	typedef impl::TransformationImpl<T, matrixSize_> TImpl;
	typedef util::SharedPtr<TImpl, impl::TransformationImplStorage, util::IntrusiveCounter<TImpl, size_t, &TImpl::referenceCount> > TImplPtr;

	Transformation3D(const TImplPtr& pimpl, bool isInversed = false);
	void computeInverse() const;

	static void translate(const TConstMatrix &source, TMatrix& dest);
	static void identity(TMatrix& dest);
	static TImplPtr makeIdentity();

	TImplPtr pimpl_;
	bool isInversed_;
	static TImplPtr identity_;
};

template <typename T> Transformation3D<T> concatenate(const Transformation3D<T>& first, const Transformation3D<T>& second);

template <typename T> Vector3D<T> transform(const Vector3D<T>& subject, const Transformation3D<T>& transformation);
template <typename T> Point3D<T> transform(const Point3D<T>& subject, const Transformation3D<T>& transformation);
template <typename T> Vector3D<T> normalTransform(const Vector3D<T>& subject, const Transformation3D<T>& transformation);
template <typename T> std::pair<Vector3D<T>, T> normalTransform(const std::pair<Vector3D<T>, T>& subject, const Transformation3D<T>& transformation);

template<typename T, typename Char, typename Traits>
std::basic_ostream<Char, Traits>& operator<<(std::basic_ostream<Char, Traits>& stream, const Transformation3D<T>& transformation);

template<typename T> io::XmlOStream& operator<<(io::XmlOStream& stream, const Transformation3D<T>& transformation);

}

}

#include "transformation_3d.inl"

#define LASS_PRIM_HAVE_PY_EXPORT_TRAITS_TRANSFORMATION_3D
#ifdef LASS_GUARDIAN_OF_INCLUSION_UTIL_PYOBJECT_PLUS_H
#	include "../python/export_traits_prim.h"
#endif

#ifdef LASS_GUARDIAN_OF_INCLUSION_PRIM_AABB_3D_H
#	include "aabb_3d_transformation_3d.h"
#endif

#ifdef LASS_GUARDIAN_OF_INCLUSION_PRIM_PLANE_3D_H
#	include "plane_3d_transformation_3d.h"
#endif

#ifdef LASS_GUARDIAN_OF_INCLUSION_PRIM_RAY_3D_H
#	include "ray_3d_transformation_3d.h"
#endif

#ifdef LASS_GUARDIAN_OF_INCLUSION_PRIM_COLOR_RGBA_H
#	include "color_rgba_transformation_3d.h"
#endif

#endif

// EOF
