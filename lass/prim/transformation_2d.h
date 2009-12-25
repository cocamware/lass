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



/** @class lass::prim::Transformation2D
 *  @brief a linear 2D transformation
 *  @author Bram de Greve [BdG]
 */

#ifndef LASS_GUARDIAN_OF_INCLUSION_PRIM_TRANSFORMATION_2D_H
#define LASS_GUARDIAN_OF_INCLUSION_PRIM_TRANSFORMATION_2D_H

#include "prim_common.h"
#include "point_2d.h"
#include "xyz.h"
#include "impl/prim_allocator.h"
#include "../io/xml_o_stream.h"
#include "../num/num_traits.h"
#include "../util/shared_ptr.h"
#include "../util/thread.h"

namespace lass
{
namespace prim
{
namespace impl
{
	template <typename T, typename Cascade>
	class Transformation2DStorage: public util::ArrayStorage<T, Cascade>
	{
	public:
		Transformation2DStorage(): util::ArrayStorage<T, Cascade>() {}
		Transformation2DStorage(T* p): util::ArrayStorage<T, Cascade>(p) {}
	protected:
		void dispose()
		{
			impl::deallocateArray(this->pointer(), 9);
		}
	};
}

template <typename T>
class Transformation2D
{
public:

	typedef Transformation2D<T> TSelf;

	typedef typename util::CallTraits<T>::TValue TValue;
	typedef typename util::CallTraits<T>::TParam TParam;
	typedef typename util::CallTraits<T>::TReference TReference;
	typedef typename util::CallTraits<T>::TConstReference TConstReference;
	typedef num::NumTraits<T> TNumTraits;
	typedef size_t TSize;

	enum { dimension = 2 }; /**< number of dimensions of vector */

	template <typename U> struct Rebind
	{
		typedef Transformation2D<U> Type;
	};

	Transformation2D();
	template <typename InputIterator> Transformation2D(InputIterator first, InputIterator last);

	const Transformation2D<T> inverse() const;

	const TValue* matrix() const;
	void swap(TSelf& other);

	static const TSelf identity();
	static const TSelf translation(const Vector2D<T>& offset);
	static const TSelf scaler(const T& scale);
	static const TSelf scaler(const Vector2D<T>& scale);
	static const TSelf rotation(TParam radians);

private:

	enum { matrixSize_ = 9 };

	typedef util::SharedPtr<TValue, impl::Transformation2DStorage> TMatrix;

	Transformation2D(const TMatrix& matrix, const TMatrix& inverseMatrix, bool dummy);

	static const TMatrix& getIdentityMatrix();

	TMatrix matrix_;
	mutable TMatrix inverseMatrix_;

	static TMatrix identityMatrix_;

	static util::Semaphore sync_;
};

template <typename T> Transformation2D<T> concatenate(const Transformation2D<T>& first, const Transformation2D<T>& second);

template <typename T> Vector2D<T> transform(const Vector2D<T>& subject, const Transformation2D<T>& transformation);
template <typename T> Point2D<T> transform(const Point2D<T>& subject, const Transformation2D<T>& transformation);
template <typename T> Vector2D<T> normalTransform(const Vector2D<T>& subject, const Transformation2D<T>& transformation);
template <typename T> std::pair<Vector2D<T>, T> normalTransform(const std::pair<Vector2D<T>, T>& subject, const Transformation2D<T>& transformation);

template<typename T, typename Char, typename Traits>
std::basic_ostream<Char, Traits>& operator<<(std::basic_ostream<Char, Traits>& stream, const Transformation2D<T>& transformation);

template<typename T> io::XmlOStream& operator<<(io::XmlOStream& stream, const Transformation2D<T>& transformation);

// static member initialisation

template <typename T> util::Semaphore Transformation2D<T>::sync_;

}

}

#include "transformation_2d.inl"

#define LASS_PRIM_HAVE_PY_EXPORT_TRAITS_TRANSFORMATION_2D
#ifdef LASS_GUARDIAN_OF_INCLUSION_UTIL_PYOBJECT_PLUS_H
#	include "../python/export_traits_prim.h"
#endif

#ifdef LASS_GUARDIAN_OF_INCLUSION_PRIM_AABB_2D_H
#	include "aabb_2d_transformation_2d.h"
#endif

#endif

// EOF
