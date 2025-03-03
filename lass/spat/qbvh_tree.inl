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
	*	Copyright (C) 2024-2025 the Initial Developer.
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

#pragma once

#include "spat_common.h"
#include "qbvh_tree.h"

#include <cstddef>
#include <cstring>

namespace lass
{
namespace spat
{
namespace impl::qbvh
{

template <typename T, size_t D>
void QAabb<T, D>::set(size_t k, const Aabb<T, D>& aabb)
{
	for (size_t i = 0; i < D; ++i)
	{
		corners[k][i][0] = aabb.corners[i][0];
		corners[k][i][1] = aabb.corners[i][1];
	}
}

template <typename T, size_t D>
int contains(const QAabb<T, D>& bounds, const Point<T, D>& point)
{
	int hitmask = 0;
	for (size_t k = 0; k < 4; ++k)
	{
		int hit = 1;
		for (size_t d = 0; d < D; ++d)
		{
			hit &= bounds.corners[k][d][0] <= point.x[d] && point.x[d] <= bounds.corners[k][d][1];
		}
		hitmask |= hit << k;
	}
	return hitmask;
}

template <typename T, size_t D>
int overlaps(const QAabb<T, D>& bounds, const Aabb<T, D>& box)
{
	int hitmask = 0;
	for (size_t k = 0; k < 4; ++k)
	{
		int hit = 1;
		for (size_t d = 0; d < D; ++d)
		{
			hit &= bounds.corners[k][d][0] <= box.corners[d][1] && box.corners[d][0] <= bounds.corners[k][d][1];
		}
		hitmask |= hit << k;
	}
	return hitmask;
}

template <typename T, size_t D>
int intersect(const QAabb<T, D>& bounds, const Ray<T, D>& ray, T tMin, T tMax, T ts[4])
{
	int hitmask = 0;
	for (size_t k = 0; k < 4; ++k)
	{
		T tmin = tMin;
		T tmax = tMax;
		for (size_t d = 0; d < D; ++d)
		{
			const bool sign = ray.sign[d];
			const T bmin = bounds.corners[k][d][sign];
			const T bmax = bounds.corners[k][d][sign ^ 1];
			const T dmin = (bmin - ray.support[d]) * ray.invDir[d];
			const T dmax = (bmax - ray.support[d]) * ray.invDir[d];
			tmin = std::max(dmin, tmin);
			tmax = std::min(dmax, tmax);
		}
		tmax *= 1 + 2 * num::NumTraits<T>::gamma(3);
		hitmask |= tmin <= tmax ? 1 << k : 0;
		ts[k] = tmin;
	}
	return hitmask;
}

template <typename T, size_t D>
void squaredDistance(const QAabb<T, D>& bounds, const Point<T, D>& point, T sds[4])
{
	for (size_t k = 0; k < 4; ++k)
	{
		T sd = 0;
		for (size_t i = 0; i < D; ++i)
		{
			const T bmin = bounds.corners[k][i][0];
			const T bmax = bounds.corners[k][i][1];
			const T dmin = bmin - point.x[i];
			const T dmax = point.x[i] - bmax;
			const T d = std::max(dmin, dmax);
			if (d > 0)
			{
				sd += d * d;
			}
		}
		sds[k] = sd;
	}
}

#if LASS_HAVE_AVX

template <size_t D>
void QAabb<float, D>::set(size_t k, const Aabb<float, D>& aabb)
{
	for (size_t i = 0; i < D; ++i)
	{
#if LASS_COMPILER_TYPE == LASS_COMPILER_TYPE_MSVC
		corners[i][0].m128_f32[k] = aabb.corners[i][0];
		corners[i][1].m128_f32[k] = aabb.corners[i][1];
#else
		corners[i][0][k] = aabb.corners[i][0];
		corners[i][1][k] = aabb.corners[i][1];
#endif
	}
}

template <size_t D>
void QAabb<double, D>::set(size_t k, const Aabb<double, D>& aabb)
{
	for (size_t i = 0; i < D; ++i)
	{
#if LASS_COMPILER_TYPE == LASS_COMPILER_TYPE_MSVC
		corners[i][0].m256d_f64[k] = aabb.corners[i][0];
		corners[i][1].m256d_f64[k] = aabb.corners[i][1];
#else
		corners[i][0][k] = aabb.corners[i][0];
		corners[i][1][k] = aabb.corners[i][1];
#endif
	}
}

template <size_t D>
int contains(const QAabb<float, D>& bounds, const Point<float, D>& point)
{
	__m128 h = _mm_castsi128_ps(_mm_set1_epi32(-1));
	for (size_t d = 0; d < D; ++d)
	{
		const __m128 p = _mm_set_ps1(point.x[d]);
		const __m128 hmin = _mm_cmp_ps(bounds.corners[d][0], p, _CMP_LE_OQ);
		const __m128 hmax = _mm_cmp_ps(p, bounds.corners[d][1], _CMP_LE_OQ);
		h = _mm_and_ps(h, hmin);
		h = _mm_and_ps(h, hmax);
	}
	const int hitmask = _mm_movemask_ps(h);
	return hitmask;
}

template <size_t D>
int contains(const QAabb<double, D>& bounds, const Point<double, D>& point)
{
	__m256d h = _mm256_castsi256_pd(_mm256_set1_epi32(-1));
	for (size_t d = 0; d < D; ++d)
	{
		const __m256d p = _mm256_set1_pd(point.x[d]);
		const __m256d hmin = _mm256_cmp_pd(bounds.corners[d][0], p, _CMP_LE_OQ);
		const __m256d hmax = _mm256_cmp_pd(p, bounds.corners[d][1], _CMP_LE_OQ);
		h = _mm256_and_pd(h, hmin);
		h = _mm256_and_pd(h, hmax);
	}
	const int hitmask = _mm256_movemask_pd(h);
	return hitmask;
}


template <size_t D>
int overlaps(const QAabb<float, D>& bounds, const Aabb<float, D>& box)
{
	__m128 h = _mm_castsi128_ps(_mm_set1_epi32(-1));
	for (size_t d = 0; d < D; ++d)
	{
		const __m128 pmin = _mm_set_ps1(box.corners[d][0]);
		const __m128 pmax = _mm_set_ps1(box.corners[d][1]);
		const __m128 hmin = _mm_cmp_ps(bounds.corners[d][0], pmax, _CMP_LE_OQ);
		const __m128 hmax = _mm_cmp_ps(pmin, bounds.corners[d][1], _CMP_LE_OQ);
		h = _mm_and_ps(h, hmin);
		h = _mm_and_ps(h, hmax);
	}
	const int hitmask = _mm_movemask_ps(h);
	return hitmask;
}


template <size_t D>
int overlaps(const QAabb<double, D>& bounds, const Aabb<double, D>& box)
{
	__m256d h = _mm256_castsi256_pd(_mm256_set1_epi32(-1));
	for (size_t d = 0; d < D; ++d)
	{
		const __m256d pmin = _mm256_set1_pd(box.corners[d][0]);
		const __m256d pmax = _mm256_set1_pd(box.corners[d][1]);
		const __m256d hmin = _mm256_cmp_pd(bounds.corners[d][0], pmax, _CMP_LE_OQ);
		const __m256d hmax = _mm256_cmp_pd(pmin, bounds.corners[d][1], _CMP_LE_OQ);
		h = _mm256_and_pd(h, hmin);
		h = _mm256_and_pd(h, hmax);
	}
	const int hitmask = _mm256_movemask_pd(h);
	return hitmask;
}

template <size_t D>
int intersect(const QAabb<float, D>& bounds, const Ray<float, D>& ray, float tMin, float tMax, float ts[4])
{
	__m128 tmin = _mm_set_ps1(tMin);
	__m128 tmax = _mm_set_ps1(tMax);
	for (size_t d = 0; d < D; ++d)
	{
		const __m128 support = _mm_set_ps1(ray.support[d]);
		const __m128 invDir = _mm_set_ps1(ray.invDir[d]);
		const bool sign = ray.sign[d];
		const __m128 bmin = bounds.corners[d][sign];
		const __m128 bmax = bounds.corners[d][sign ^ 1];
		const __m128 dmin = _mm_mul_ps(_mm_sub_ps(bmin, support), invDir);
		const __m128 dmax = _mm_mul_ps(_mm_sub_ps(bmax, support), invDir);
		tmin = _mm_max_ps(dmin, tmin);
		tmax = _mm_min_ps(dmax, tmax);
	}
	tmax = _mm_mul_ps(tmax, _mm_set_ps1(1 + 2 * num::NumTraits<float>::gamma(3)));
	_mm_storeu_ps(ts, tmin);
	__m128 h = _mm_cmp_ps(tmin, tmax, _CMP_LE_OQ);
	const int hitmask = _mm_movemask_ps(h);
	return hitmask;
}

template <size_t D>
int intersect(const QAabb<double, D>& bounds, const Ray<double, D>& ray, double tMin, double tMax, double ts[4])
{
	__m256d tmin = _mm256_set1_pd(tMin);
	__m256d tmax = _mm256_set1_pd(tMax);
	for (size_t d = 0; d < D; ++d)
	{
		const __m256d support = _mm256_set1_pd(ray.support[d]);
		const __m256d invDir = _mm256_set1_pd(ray.invDir[d]);
		const bool sign = ray.sign[d];
		const __m256d bmin = bounds.corners[d][sign];
		const __m256d bmax = bounds.corners[d][sign ^ 1];
		const __m256d dmin = _mm256_mul_pd(_mm256_sub_pd(bmin, support), invDir);
		const __m256d dmax = _mm256_mul_pd(_mm256_sub_pd(bmax, support), invDir);
		tmin = _mm256_max_pd(dmin, tmin);
		tmax = _mm256_min_pd(dmax, tmax);
	}
	_mm256_storeu_pd(ts, tmin);
	tmax = _mm256_mul_pd(tmax, _mm256_set1_pd(1 + 2 * num::NumTraits<double>::gamma(3)));
	const __m256d h = _mm256_cmp_pd(tmin, tmax, _CMP_LE_OQ);
	const int hitmask = _mm256_movemask_pd(h);
	return hitmask;
}


template <size_t D>
void squaredDistance(const QAabb<float, D>& bounds, const Point<float, D>& point, float sds[4])
{
	const __m128 zero = _mm_setzero_ps();
	__m128 sd = _mm_setzero_ps();
	for (size_t i = 0; i < D; ++i)
	{
		const __m128 p = _mm_set_ps1(point.x[i]);
		const __m128 dmin = _mm_sub_ps(bounds.corners[i][0], p);
		const __m128 dmax = _mm_sub_ps(p, bounds.corners[i][1]);
		const __m128 d = _mm_max_ps(_mm_max_ps(dmin, dmax), zero);
		const __m128 d2 = _mm_mul_ps(d, d);
		sd = _mm_add_ps(sd, d2);
	}
	_mm_storeu_ps(sds, sd);
}

template <size_t D>
void squaredDistance(const QAabb<double, D>& bounds, const Point<double, D>& point, double sds[4])
{
	const __m256d zero = _mm256_setzero_pd();
	__m256d sd = _mm256_setzero_pd();
	for (size_t i = 0; i < D; ++i)
	{
		const __m256d p = _mm256_set1_pd(point.x[i]);
		const __m256d dmin = _mm256_sub_pd(bounds.corners[i][0], p);
		const __m256d dmax = _mm256_sub_pd(p, bounds.corners[i][1]);
		const __m256d d = _mm256_max_pd(_mm256_max_pd(dmin, dmax), zero);
		const __m256d d2 = _mm256_mul_pd(d, d);
		sd = _mm256_add_pd(sd, d2);
	}
	_mm256_storeu_pd(sds, sd);
}

#endif


}


// --- public --------------------------------------------------------------------------------------



template <typename O, typename OT, typename SH>
QbvhTree<O, OT, SH>::QbvhTree(TSplitHeuristics heuristics) :
	SH(std::move(heuristics)),
	aabb_(TObjectTraits::aabbEmpty()),
	nodes_(),
	objects_(),
	end_(new TObjectIterator),
	root_()
{
}



template <typename O, typename OT, typename SH>
QbvhTree<O, OT, SH>::QbvhTree(TObjectIterator first, TObjectIterator last, TSplitHeuristics heuristics):
	SH(std::move(heuristics)),
	aabb_(TObjectTraits::aabbEmpty()),
	nodes_(),
	objects_(),
	end_(new TObjectIterator(last)),
	root_()
{
	if (first != last)
	{
		std::ptrdiff_t n = last - first;
		if (n < 0)
		{
			LASS_THROW("QbvhTree: invalid range");
		}
		if (static_cast<size_t>(n) > static_cast<size_t>(Child::maxNode) + 1)
		{
			LASS_THROW("QbvhTree: too many objects");
		}
		TInputs inputs;
		inputs.reserve(static_cast<size_t>(n));
		for (TObjectIterator i = first; i != last; ++i)
		{
			inputs.emplace_back(TObjectTraits::objectAabb(i), i);
		}
		root_ = balance(inputs.begin(), inputs.end(), aabb_);
	}
}


template <typename O, typename OT, typename SH>
QbvhTree<O, OT, SH>::QbvhTree(TSelf&& other) noexcept:
	SH(std::forward<TSplitHeuristics>(other)),
	aabb_(std::move(other.aabb_)),
	nodes_(std::move(other.nodes_)),
	objects_(std::move(other.objects_)),
	end_(std::move(other.end_)),
	root_(std::move(other.root_))
{
}


template <typename O, typename OT, typename SH>
QbvhTree<O, OT, SH>& QbvhTree<O, OT, SH>::operator=(TSelf&& other) noexcept
{
	TSplitHeuristics::operator=(std::forward<TSplitHeuristics>(other));
	aabb_ = std::move(other.aabb_);
	nodes_ = std::move(other.nodes_);
	objects_ = std::move(other.objects_);
	end_ = std::move(other.end_);
	root_ = std::move(other.root_);
	return *this;
}



/** Reset the tree to an empty one.
 * 
 *  Is equivalent to:
 *  @code
 *  *this = TSelf();
 *  @endcode
 */
template <typename O, typename OT, typename SH>
void QbvhTree<O, OT, SH>::reset()
{
	TSelf temp;
	swap(temp);
}



/** Reset the tree to a new one with objects in the range [@a first, @a last)
 * 
 *  Is equivalent to:
 *  @code
 *  *this = TSelf(first, last);
 *  @endcode
 */
template <typename O, typename OT, typename SH>
void QbvhTree<O, OT, SH>::reset(TObjectIterator first, TObjectIterator last)
{
	TSelf temp(first, last, static_cast<const SH&>(*this));
	swap(temp);
}



/** Return the total bounding box of all objecs in the tree 
 */
template <typename O, typename OT, typename SH> inline
const typename QbvhTree<O, OT, SH>::TAabb
QbvhTree<O, OT, SH>::aabb() const
{
	return aabb_;
}



/** Check whether there's any object in the tree that contains @a point.
 */
template <typename O, typename OT, typename SH>
bool QbvhTree<O, OT, SH>::contains(const TPoint& point, const TInfo* info) const
{
	if (isEmpty() || !TObjectTraits::aabbContains(aabb_, point))
	{
		return false;
	}
	LASS_ASSERT(!root_.isEmpty());
	return doContains(root_, point, info);
}



/** Find all objects that contain @a point.
	*/
template <typename O, typename OT, typename SH>
template <typename OutputIterator> inline
OutputIterator QbvhTree<O, OT, SH>::find(const TPoint& point, OutputIterator result, const TInfo* info) const
{
	if (isEmpty() || !TObjectTraits::aabbContains(aabb_, point))
	{
		return result;
	}
	LASS_ASSERT(!root_.isEmpty());
	return doFind(root_, point, result, info);
}



/** Find all objects that intersect with @a box.
 */
template <typename O, typename OT, typename SH>
template <typename OutputIterator> inline
OutputIterator QbvhTree<O, OT, SH>::find(const TAabb& box, OutputIterator result, const TInfo* info) const
{
	if (isEmpty() || !TObjectTraits::aabbIntersects(aabb_, box))
	{
		return result;
	}
	LASS_ASSERT(!root_.isEmpty());
	return doFind(root_, box, result, info);
}



/** Find all objects that have an intersection with @a ray in the interval [tMin, tMax].
 */
template <typename O, typename OT, typename SH>
template <typename OutputIterator>
OutputIterator QbvhTree<O, OT, SH>::find(const TRay& ray, TParam tMin, TParam tMax, OutputIterator result, const TInfo* info) const
{
	const TVector invDir = TObjectTraits::vectorReciprocal(TObjectTraits::rayDirection(ray));
	if (isEmpty() || !volumeIntersects(aabb_, ray, invDir, tMin, tMax))
	{
		return result;
	}
	LASS_ASSERT(!root_.isEmpty());
	return doFind(root_, ray, invDir, tMin, tMax, result, info);
}



/** Find the first object that is intersected by @a ray, so that t >= tMin and t is minimized.
 */
template <typename O, typename OT, typename SH>
typename QbvhTree<O, OT, SH>::TObjectIterator
QbvhTree<O, OT, SH>::intersect(const TRay& ray, TReference t, TParam tMin, const TInfo* info) const
{
	const TVector dir = TObjectTraits::rayDirection(ray);
	const TVector invDir = TObjectTraits::vectorReciprocal(dir);
	TValue tRoot;
	if (isEmpty() || !volumeIntersect(aabb_, ray, invDir, tRoot, tMin))
	{
		return *end_;
	}
	LASS_ASSERT(!root_.isEmpty());
	return doIntersect(root_, ray, invDir, t, tMin, info);
}



/** Check whether there's any object in the tree that is intersected by @a ray in the interval [tMin, tMax].
 */
template <typename O, typename OT, typename SH>
bool QbvhTree<O, OT, SH>::intersects(
	const TRay& ray, TParam tMin, TParam tMax, const TInfo* info) const
{
	LASS_ASSERT(tMax > tMin || (num::isInf(tMin) && num::isInf(tMax)));
	const TVector dir = TObjectTraits::rayDirection(ray);
	const TVector invDir = TObjectTraits::vectorReciprocal(dir);
	if (isEmpty() || !volumeIntersects(aabb_, ray, invDir, tMin, tMax))
	{
		return false;
	}
	LASS_ASSERT(!root_.isEmpty());
	return doIntersects(root_, ray, invDir, tMin, tMax, info);
}



/** Find the object that is closest to @a point.
 */
template <typename O, typename OT, typename SH>
const typename QbvhTree<O, OT, SH>::Neighbour
QbvhTree<O, OT, SH>::nearestNeighbour(const TPoint& point, const TInfo* info) const
{
	Neighbour nearest(*end_, std::numeric_limits<TValue>::infinity());
	if (isEmpty())
	{
		return nearest;
	}
	LASS_ASSERT(!root_.isEmpty());
	doNearestNeighbour(root_, point, info, nearest);
	return nearest;
}



/** Find all objects that are within @a maxRadius from @a center, up to @a maxCount.
 * 
 *  If more than @a maxCount objects within @a maxRadius are found, then the closest
 *  @a maxCount objects are returned.
 * 
 *  @param center The center of the search sphere.
 *  @param maxRadius The maximum radius of the search sphere.
 *  @param maxCount The maximum number of objects to find.
 *  @param first An output iterator to a container of Neighbour objects that will be filled with the found objects.
 *  @param info Optional information to pass to the object traits.
 * 
 *  @return The output iterator @a last, pointing to the first element beyond the last found object.
 * 
 *  The range [@a first, @a last) will contain the found objects and form a heap, so that @a first points to the
 *  farthest object from @a center: its squared distance to @a center is the largest in the range, and gives you
 *  the effective maximum radius of the search sphere.
 * 
 *  @note The output iterator must be able to handle at least @a maxCount objects.
 */
template <class O, class OT, typename SH>
template<typename RandomIterator>
RandomIterator
QbvhTree<O, OT, SH>::rangeSearch(
	const TPoint& center, TParam maxRadius, size_t maxCount, RandomIterator first, const TInfo* info) const
{
	if (isEmpty() || maxRadius == 0)
	{
		return first;
	}
	TValue squaredRadius = maxRadius * maxRadius;
	LASS_ASSERT(!root_.isEmpty());
	return doRangeSearch(root_, center, squaredRadius, maxCount, first, first, info);
}



/** Returns true if there are no objects in the tree.
 */
template <typename O, typename OT, typename SH>
bool QbvhTree<O, OT, SH>::isEmpty() const
{
	return objects_.empty();
}



/** Returns an iterator not pointing to any object, used to indicate when no object is found.
 * 
 *  This iterator can be used as the return value of the find functions when no object is found.
 *  You can compare those return values to the end() function to check if an object was found.
 */
template <typename O, typename OT, typename SH>
const typename QbvhTree<O, OT, SH>::TObjectIterator
QbvhTree<O, OT, SH>::end() const
{
	return *end_;
}



/** Swap the contents of this tree with another.
 * 
 *  Is equivalent to:
 *  @code
 *  TSelf tmp = std::move(other);
 *  other = std::move(*this);
 *  *this = std::move(tmp);
 *  @endcode
 */
template <typename O, typename OT, typename SH>
void QbvhTree<O, OT, SH>::swap(TSelf& other)
{
	SH::swap(other);
	std::swap(aabb_, other.aabb_);
	nodes_.swap(other.nodes_);
	objects_.swap(other.objects_);
	end_.swap(other.end_);
	std::swap(root_, other.root_);
}



// --- private -------------------------------------------------------------------------------------

template <typename O, typename OT, typename SH>
typename QbvhTree<O, OT, SH>::Child
QbvhTree<O, OT, SH>::balance(TInputIterator first, TInputIterator last, TAabb& bounds)
{
	if (first == last)
	{
		bounds = TObjectTraits::aabbEmpty();
		return Child();
	}

	LASS_ASSERT(nodes_.size() <= static_cast<size_t>(Child::maxNode));
	const TIndex index = static_cast<TIndex>(nodes_.size());
	nodes_.emplace_back();

	auto split0 = TSplitHeuristics::template split<OT>(first, last);
	bounds = split0.aabb;
	if (split0.isLeaf())
	{
		const auto count = last - first;
		if (count <= Child::maxCount)
		{
			const TIndex frst = static_cast<TIndex>(objects_.size());
			LASS_ASSERT(frst >= 0 && frst <= Child::maxObject);
			for (TInputIterator i = first; i != last; ++i)
			{
				objects_.push_back(i->object);
			}
			LASS_ASSERT(objects_.size() <= static_cast<size_t>(Child::maxObject) + 1);
			return Child(frst, static_cast<TIndex>(count));
		}
		else
		{
			split0 = forceSplit(split0.aabb);
		}
	}
	LASS_ASSERT(!split0.isLeaf());
	nodes_[index].axis[0] = static_cast<TAxis>(split0.axis);
	TInputIterator middle0 = std::partition(first, last, impl::Splitter<TObjectTraits>(split0));
	if (middle0 == first || middle0 == last)
	{
		const std::ptrdiff_t halfSize = (last - first) / 2;
		LASS_ASSERT(halfSize > 0);
		middle0 = first + halfSize;
		std::nth_element(first, middle0, last, impl::LessAxis<TObjectTraits>(split0.axis));
	}
	LASS_ASSERT(middle0 != first && middle0 != last);

	auto split1 = TSplitHeuristics::template split<OT>(first, middle0);
	if (split1.isLeaf())
	{
		const auto count = middle0 - first;
		if (count <= Child::maxCount)
		{
			const TIndex frst = static_cast<TIndex>(objects_.size());
			LASS_ASSERT(frst >= 0 && frst <= Child::maxObject);
			for (TInputIterator i = first; i != middle0; ++i)
			{
				objects_.push_back(i->object);
			}
			LASS_ASSERT(objects_.size() <= static_cast<size_t>(Child::maxObject) + 1);
			Node& node = nodes_[index];
			node.children[0] = Child(frst, static_cast<TIndex>(count));
			node.children[1] = Child();
			node.bounds.set(0, makeAabb(split1.aabb));
			node.bounds.set(1, makeAabb(TObjectTraits::aabbEmpty()));
			node.axis[1] = 0;
		}
		else
		{
			split1 = forceSplit(split1.aabb);
		}
	}
	if (!split1.isLeaf())
	{
		TInputIterator middle1 = std::partition(first, middle0, impl::Splitter<TObjectTraits>(split1));
		if (middle1 == first || middle1 == middle0)
		{
			const std::ptrdiff_t halfSize = (middle0 - first) / 2;
			LASS_ASSERT(halfSize > 0);
			middle1 = first + halfSize;
			std::nth_element(first, middle1, middle0, impl::LessAxis<TObjectTraits>(split1.axis));
		}
		TAabb bounds0, bounds1;
		Child child0 = balance(first, middle1, bounds0);
		Child child1 = balance(middle1, middle0, bounds1);
		Node& node = nodes_[index];
		node.children[0] = child0;
		node.children[1] = child1;
		node.bounds.set(0, makeAabb(bounds0));
		node.bounds.set(1, makeAabb(bounds1));
		node.axis[1] = static_cast<TAxis>(split1.axis);
	}

	auto split2 = TSplitHeuristics::template split<OT>(middle0, last);
	if (split2.isLeaf())
	{
		const auto count = last - middle0;
		if (count <= Child::maxCount)
		{
			const TIndex frst = static_cast<TIndex>(objects_.size());
			LASS_ASSERT(frst >= 0 && frst <= Child::maxObject);
			for (TInputIterator i = middle0; i != last; ++i)
			{
				objects_.push_back(i->object);
			}
			LASS_ASSERT(objects_.size() <= static_cast<size_t>(Child::maxObject) + 1);
			Node& node = nodes_[index];
			node.children[2] = Child(frst, static_cast<TIndex>(count));
			node.children[3] = Child();
			node.bounds.set(2, makeAabb(split2.aabb));
			node.bounds.set(3, makeAabb(TObjectTraits::aabbEmpty()));
			node.axis[2] = 0;
		}
		else
		{
			split2 = forceSplit(split2.aabb);
		}
	}
	if (!split2.isLeaf())
	{
		TInputIterator middle2 = std::partition(middle0, last, impl::Splitter<TObjectTraits>(split2));
		if (middle2 == middle0 || middle2 == last)
		{
			const std::ptrdiff_t halfSize = (last - middle0) / 2;
			LASS_ASSERT(halfSize > 0);
			middle2 = middle0 + halfSize;
			std::nth_element(middle0, middle2, last, impl::LessAxis<TObjectTraits>(split2.axis));
		}
		TAabb bounds2, bounds3;
		Child child2 = balance(middle0, middle2, bounds2);
		Child child3 = balance(middle2, last, bounds3);
		Node& node = nodes_[index];
		node.children[2] = child2;
		node.children[3] = child3;
		node.bounds.set(2, makeAabb(bounds2));
		node.bounds.set(3, makeAabb(bounds3));
		node.axis[2] = static_cast<TAxis>(split2.axis);
	}

	Node& node = nodes_[index];
	node.usedMask = 0;
	for (size_t k = 0; k < 4; ++k)
	{
		node.usedMask |= node.children[k].isEmpty() ? 0 : 1 << k;
	}

	return Child(index);
}



template <typename O, typename OT, typename SH>
typename QbvhTree<O, OT, SH>::TSplitInfo
QbvhTree<O, OT, SH>::forceSplit(const TAabb& bounds)
{
	const TPoint min = TObjectTraits::aabbMin(bounds);
	const TPoint max = TObjectTraits::aabbMax(bounds);
	size_t axis = 0;
	TValue maxSize = TObjectTraits::coord(max, 0) - TObjectTraits::coord(min, 0);
	for (size_t k = 1; k < TObjectTraits::dimension; ++k)
	{
		const TValue size = TObjectTraits::coord(max, k) - TObjectTraits::coord(min, k);
		if (size > maxSize)
		{
			axis = k;
			maxSize = size;
		}
	}
	const TValue x = (TObjectTraits::coord(min, axis) + TObjectTraits::coord(max, axis)) / 2;
	return TSplitInfo(bounds, x, axis);
}



template <typename O, typename OT, typename SH>
bool QbvhTree<O, OT, SH>::doContains(Child root, const TPoint& point, const TInfo* info) const
{
	const auto pnt = makePoint(point);

	Child stack[stackSize_];
	TIndex stackSize = 0;
	stack[stackSize++] = root;
	while (stackSize > 0)
	{
		const Child index = stack[--stackSize];
		LASS_ASSERT(!index.isEmpty());

		if (index.isLeaf())
		{
			const TIndex first = index.first();
			const TIndex last = first + index.count();
			for (TIndex i = first; i != last; ++i)
			{
				LASS_SPAT_OBJECT_TREES_DIAGNOSTICS_VISIT_OBJECT;
				if (TObjectTraits::objectContains(objects_[i], point, info))
				{
					return true;
				}
			}
			continue;
		}

		LASS_ASSERT(index.isInternal());
		LASS_ASSERT(index.node() < nodes_.size());
		LASS_SPAT_OBJECT_TREES_DIAGNOSTICS_INIT_NODE(TInfo, info);
		const Node& node = nodes_[index.node()];

		const int hits = node.usedMask & impl::qbvh::contains(node.bounds, pnt);
		for (int k = 3; k >= 0; --k)
		{
			if (!(hits & (1 << k)))
			{
				continue;
			}
			const Child child = node.children[k];
			if (child.isEmpty())
			{
				continue;
			}
			if (stackSize < stackSize_)
			{
				stack[stackSize++] = child;
			}
			else if (doContains(child, point, info))
			{
				return true;
			}
		}
	}
	return false;
}



template <typename O, typename OT, typename SH>
template <typename OutputIterator>
OutputIterator QbvhTree<O, OT, SH>::doFind(Child root, const TPoint& point, OutputIterator result, const TInfo* info) const
{
	const auto pnt = makePoint(point);

	Child stack[stackSize_];
	TIndex stackSize = 0;
	stack[stackSize++] = root;
	while (stackSize > 0)
	{
		const Child index = stack[--stackSize];
		LASS_ASSERT(!index.isEmpty());

		if (index.isLeaf())
		{
			const TIndex first = index.first();
			const TIndex last = first + index.count();
			for (TIndex i = first; i != last; ++i)
			{
				LASS_SPAT_OBJECT_TREES_DIAGNOSTICS_VISIT_OBJECT;
				if (TObjectTraits::objectContains(objects_[i], point, info))
				{
					*result++ = objects_[i];
				}
			}
			continue;
		}

		LASS_ASSERT(index.isInternal());
		LASS_ASSERT(index.node() < nodes_.size());
		LASS_SPAT_OBJECT_TREES_DIAGNOSTICS_INIT_NODE(TInfo, info);
		const Node& node = nodes_[index.node()];

		const int hits = node.usedMask & impl::qbvh::contains(node.bounds, pnt);
		for (int k = 3; k >= 0; --k)
		{
			if (!(hits & (1 << k)))
			{
				continue;
			}
			const Child child = node.children[k];
			if (child.isEmpty())
			{
				continue;
			}
			if (stackSize < stackSize_)
			{
				stack[stackSize++] = child;
			}
			else
			{
				result = doFind(child, point, result, info);
			}
		}
	}
	return result;
}


template <typename O, typename OT, typename SH>
template <typename OutputIterator>
OutputIterator QbvhTree<O, OT, SH>::doFind(Child root, const TAabb& box, OutputIterator result, const TInfo* info) const
{
	const auto bx = makeAabb(box);

	Child stack[stackSize_];
	TIndex stackSize = 0;
	stack[stackSize++] = root;
	while (stackSize > 0)
	{
		const Child index = stack[--stackSize];
		LASS_ASSERT(!index.isEmpty());

		if (index.isLeaf())
		{
			const TIndex first = index.first();
			const TIndex last = first + index.count();
			for (TIndex i = first; i != last; ++i)
			{
				LASS_SPAT_OBJECT_TREES_DIAGNOSTICS_VISIT_OBJECT;
				if (TObjectTraits::objectIntersects(objects_[i], box, info))
				{
					*result++ = objects_[i];
				}
			}
			continue;
		}

		LASS_ASSERT(index.isInternal());
		LASS_ASSERT(index.node() < nodes_.size());
		LASS_SPAT_OBJECT_TREES_DIAGNOSTICS_INIT_NODE(TInfo, info);
		const Node& node = nodes_[index.node()];

		const int hits = node.usedMask & impl::qbvh::overlaps(node.bounds, bx);
		for (int k = 3; k >= 0; --k)
		{
			if (!(hits & (1 << k)))
			{
				continue;
			}
			const Child child = node.children[k];
			if (child.isEmpty())
			{
				continue;
			}
			if (stackSize < stackSize_)
			{
				stack[stackSize++] = child;
			}
			else
			{
				result = doFind(child, box, result, info);
			}
		}
	}
	return result;
}



template <typename O, typename OT, typename SH>
template <typename OutputIterator>
OutputIterator QbvhTree<O, OT, SH>::doFind(Child root, const TRay& ray, const TVector& invDir, TParam tMin, TParam tMax, OutputIterator result, const TInfo* info) const
{
	const auto r = makeRay(ray);

	Child stack[stackSize_];
	TIndex stackSize = 0;
	stack[stackSize++] = root;
	while (stackSize > 0)
	{
		const Child index = stack[--stackSize];
		LASS_ASSERT(!index.isEmpty());

		if (index.isLeaf())
		{
			const TIndex first = index.first();
			const TIndex last = first + index.count();
			for (TIndex i = first; i != last; ++i)
			{
				LASS_SPAT_OBJECT_TREES_DIAGNOSTICS_VISIT_OBJECT;
				if (TObjectTraits::objectIntersects(objects_[i], ray, tMin, tMax, info))
				{
					*result++ = objects_[i];
				}
			}
			continue;
		}

		LASS_ASSERT(index.isInternal());
		LASS_SPAT_OBJECT_TREES_DIAGNOSTICS_INIT_NODE(TInfo, info);
		LASS_ASSERT(index.node() < nodes_.size());
		const Node& node = nodes_[index.node()];

		TValue ts[4];
		const int hits = node.usedMask & impl::qbvh::intersect(node.bounds, r, tMin, tMax, ts);
		for (int k = 3; k >= 0; --k)
		{
			if (!(hits & (1 << k)))
			{
				continue;
			}
			const Child child = node.children[k];
			if (stackSize < stackSize_)
			{
				stack[stackSize++] = child;
			}
			else
			{
				result = doFind(child, ray, invDir, tMin, tMax, result, info);
			}
		}
	}
	return result;
}





template <typename O, typename OT, typename SH>
typename QbvhTree<O, OT, SH>::TObjectIterator
QbvhTree<O, OT, SH>::doIntersect(Child root, const TRay& ray, const TVector& invDir, TReference t, TParam tMin, const TInfo* info) const
{
	const auto r = makeRay(ray);
#if LASS_HAVE_AVX
	__m128i signs[dimension];
	for (size_t i = 0; i < dimension; ++i)
	{
		signs[i] = _mm_set1_epi32(r.sign[i] ? -1 : 0);
	}
#endif

	struct Visit
	{
		TValue tNear;
		Child index;
		Visit() = default;
		Visit(Child index, TValue tNear): tNear(tNear), index(index) {}
	};
	Visit stack[stackSize_];
	size_t stackSize = 0;
	stack[stackSize++] = Visit(root, tMin);
	TValue tBest = TNumTraits::infinity;
	TObjectIterator best = *end_;
	while (stackSize > 0)
	{
		const Visit visit = stack[--stackSize];
		if (tBest <= visit.tNear)
		{
			continue; // we already have a closer hit than what this node can offer
		}

		const Child index = visit.index;
		LASS_ASSERT(!index.isEmpty());

		if (index.isLeaf())
		{
			const TIndex first = index.first();
			const TIndex last = first + index.count();
			for (TIndex i = first; i != last; ++i)
			{
				LASS_SPAT_OBJECT_TREES_DIAGNOSTICS_VISIT_OBJECT;
				TValue tCandidate = 0;
				if (TObjectTraits::objectIntersect(objects_[i], ray, tCandidate, tMin, info))
				{
					if (best == *end_ || tCandidate < tBest)
					{
						tBest = tCandidate;
						best = objects_[i];
					}
				}
			}
			continue;
		}

		LASS_ASSERT(index.isInternal());
		LASS_SPAT_OBJECT_TREES_DIAGNOSTICS_INIT_NODE(TInfo, info);
		LASS_ASSERT(index.node() < nodes_.size());
		const Node& node = nodes_[index.node()];

		TValue ts[4];
		const int hits = node.usedMask & impl::qbvh::intersect(node.bounds, r, tMin, tBest, ts);

#if LASS_HAVE_AVX
		alignas(16) int order[4];
		{
			auto order_ = _mm_set_epi32(3, 2, 1, 0);
			const auto m0 = _mm_set_epi32(-1, -1, -1, -1);
			const auto m1 = _mm_set_epi32(0, 0, -1, -1);
			const auto m2 = _mm_set_epi32(-1, -1, 0, 0);
			const auto mask0 = _mm_and_si128(m0, signs[node.axis[0]]);
			const auto mask1 = _mm_and_si128(m1, signs[node.axis[1]]);
			const auto mask2 = _mm_and_si128(m2, signs[node.axis[2]]);
			const auto mask21 = _mm_or_si128(mask2, mask1);
			const auto order21 = _mm_shuffle_epi32(order_, 0xB1);
			order_ = _mm_blendv_epi8(order_, order21, mask21);
			const auto order0 = _mm_shuffle_epi32(order_, 0x4E);
			order_ = _mm_blendv_epi8(order_, order0, mask0);
			static_assert(sizeof(order) == sizeof(order_));
			memcpy(order, &order_, sizeof(order_));
		}
#else
		int order[4] = { 0, 1, 2, 3 };
		if (r.sign[node.axis[1]])
		{
			std::swap(order[0], order[1]);
		}
		if (r.sign[node.axis[2]])
		{
			std::swap(order[2], order[3]);
		}
		if (r.sign[node.axis[0]])
		{
			std::swap(order[0], order[2]);
			std::swap(order[1], order[3]);
		}
#endif

		// push in reverse order, so that the first child is on top of the stack
		for (int k = 3; k >= 0; --k)
		{
			const auto o = order[k];
			if (!(hits & (1 << o)))
			{
				continue;
			}
			const Child child = node.children[o];
			if (stackSize < stackSize_)
			{
				stack[stackSize++] = Visit(child, ts[o]);
			}
			else
			{
				TValue tb;
				TObjectIterator b = doIntersect(child, ray, invDir, tb, tMin, info);
				if (b != *end_ && tb < tBest)
				{
					best = b;
					tBest = tb;
				}
			}
		}
	}
	if (best != *end_)
	{
		t = tBest;
	}
	return best;
}



template <typename O, typename OT, typename SH>
bool QbvhTree<O, OT, SH>::doIntersects(Child root, const TRay& ray, const TVector& invDir, TParam tMin, TParam tMax, const TInfo* info) const
{
	const auto r = makeRay(ray);

	Child stack[stackSize_];
	TIndex stackSize = 0;
	stack[stackSize++] = root;
	while (stackSize > 0)
	{
		const Child index = stack[--stackSize];
		LASS_ASSERT(!index.isEmpty());

		if (index.isLeaf())
		{
			const TIndex first = index.first();
			const TIndex last = first + index.count();
			for (TIndex i = first; i != last; ++i)
			{
				LASS_SPAT_OBJECT_TREES_DIAGNOSTICS_VISIT_OBJECT;
				if (TObjectTraits::objectIntersects(objects_[i], ray, tMin, tMax, info))
				{
					return true;
				}
			}
			continue;
		}

		LASS_ASSERT(index.isInternal());
		LASS_SPAT_OBJECT_TREES_DIAGNOSTICS_INIT_NODE(TInfo, info);
		LASS_ASSERT(index.node() < nodes_.size());
		const Node& node = nodes_[index.node()];

		TValue ts[4];
		const int hits = node.usedMask & impl::qbvh::intersect(node.bounds, r, tMin, tMax, ts);
		for (int k = 3; k >= 0; --k)
		{
			if (!(hits & (1 << k)))
			{
				continue;
			}
			const Child child = node.children[k];
			if (stackSize < stackSize_)
			{
				stack[stackSize++] = child;
			}
			else if (doIntersects(child, ray, invDir, tMin, tMax, info))
			{
				return true;
			}
		}
	}
	return false;
}


namespace impl::qbvh
{

template <typename T>
void sort4(size_t order[4], const T values[4])
{
	if (values[order[0]] > values[order[2]])
	{
		std::swap(order[0], order[2]);
	}
	if (values[order[1]] > values[order[3]])
	{
		std::swap(order[1], order[3]);
	}
	if (values[order[0]] > values[order[1]])
	{
		std::swap(order[0], order[1]);
	}
	if (values[order[2]] > values[order[3]])
	{
		std::swap(order[2], order[3]);
	}
	if (values[order[1]] > values[order[2]])
	{
		std::swap(order[1], order[2]);
	}
	LASS_ASSERT(values[order[0]] <= values[order[1]]);
	LASS_ASSERT(values[order[1]] <= values[order[2]]);
	LASS_ASSERT(values[order[2]] <= values[order[3]]);
}

}



template <typename O, typename OT, typename SH>
void QbvhTree<O, OT, SH>::doNearestNeighbour(
	Child root, const TPoint& target, const TInfo* info, Neighbour& nearest) const
{
	const auto pnt = makePoint(target);

	struct Visit
	{
		TValue squaredDistance;
		Child index;
		Visit() = default;
		Visit(Child index, TValue squaredDistance): squaredDistance(squaredDistance), index(index) {}
	};
	Visit stack[stackSize_];
	size_t stackSize = 0;
	stack[stackSize++] = Visit(root, 0);
	while (stackSize > 0)
	{
		LASS_ASSERT(nearest.squaredDistance() >= 0);
		const Visit visit = stack[--stackSize];
		if (nearest.squaredDistance() <= visit.squaredDistance)
		{
			continue; // we already have a closer neighbour than what this node can offer
		}

		const Child index = visit.index;
		LASS_ASSERT(!index.isEmpty());

		if (index.isLeaf())
		{
			const auto first = index.first();
			const auto last = first + index.count();
			for (auto i = first; i != last; ++i)
			{
				LASS_SPAT_OBJECT_TREES_DIAGNOSTICS_VISIT_OBJECT;
				const TValue squaredDistance = TObjectTraits::objectSquaredDistance(objects_[i], target, info);
				if (squaredDistance < nearest.squaredDistance())
				{
					nearest = Neighbour(objects_[i], squaredDistance);
				}
			}
			continue;
		}

		LASS_ASSERT(index.isInternal());
		LASS_SPAT_OBJECT_TREES_DIAGNOSTICS_INIT_NODE(TInfo, info);
		LASS_ASSERT(index.node() < nodes_.size());
		const Node& node = nodes_[index.node()];

		TValue squaredDistances[4];
		impl::qbvh::squaredDistance(node.bounds, pnt, squaredDistances);

		size_t order[4] = { 0, 1, 2, 3 };
		impl::qbvh::sort4(order, squaredDistances);

		// push in reverse order, so that the first child is on top of the stack
		for (int k = 3; k >= 0; --k)
		{
			const size_t o = order[k];
			const Child child = node.children[o];
			if (child.isEmpty())
			{
				continue;
			}
			if (stackSize < stackSize_)
			{
				stack[stackSize++] = Visit(child, squaredDistances[o]);
			}
			else
			{
				doNearestNeighbour(child, target, info, nearest);
			}
		}
	}
}



template <typename O, typename OT, typename SH>
template <typename RandomIterator>
RandomIterator
QbvhTree<O, OT, SH>::doRangeSearch(
	Child root, const TPoint& center, TReference squaredRadius, size_t maxCount, RandomIterator first, RandomIterator last, const TInfo* info) const
{
	const auto pnt = makePoint(center);

	struct Visit
	{
		TValue squaredDistance;
		Child index;
		Visit() = default;
		Visit(Child index, TValue squaredDistance): squaredDistance(squaredDistance), index(index) {}
	};
	Visit stack[stackSize_];
	size_t stackSize = 0;
	stack[stackSize++] = Visit(root, 0);
	while (stackSize > 0)
	{
		LASS_ASSERT(squaredRadius >= 0);
		const Visit visit = stack[--stackSize];
		if (squaredRadius <= visit.squaredDistance)
		{
			continue; // node is entirely outside range
		}

		const Child index = visit.index;
		LASS_ASSERT(!index.isEmpty());

		if (index.isLeaf())
		{
			const auto frst = index.first();
			const auto lst = frst + index.count();
			for (auto i = frst; i != lst; ++i)
			{
				LASS_SPAT_OBJECT_TREES_DIAGNOSTICS_VISIT_OBJECT;
				const TValue squaredDistance = TObjectTraits::objectSquaredDistance(objects_[i], center, info);
				if (squaredDistance < squaredRadius)
				{
					*last++ = Neighbour(objects_[i], squaredDistance);
					std::push_heap(first, last);
					LASS_ASSERT(last >= first);
					if (static_cast<size_t>(last - first) > maxCount)
					{
						std::pop_heap(first, last);
						--last;
						squaredRadius = first->squaredDistance();
					}
				}
			}
			continue;
		}

		LASS_ASSERT(index.isInternal());
		LASS_SPAT_OBJECT_TREES_DIAGNOSTICS_INIT_NODE(TInfo, info);
		LASS_ASSERT(index.node() < nodes_.size());
		const Node& node = nodes_[index.node()];

		TValue squaredDistances[4];
		impl::qbvh::squaredDistance(node.bounds, pnt, squaredDistances);

		size_t order[4] = { 0, 1, 2, 3 };
		impl::qbvh::sort4(order, squaredDistances);

		// push in reverse order, so that the first child is on top of the stack
		for (int k = 3; k >= 0; --k)
		{
			size_t o = order[k];
			const Child child = node.children[o];
			if (child.isEmpty())
			{
				continue;
			}
			if (stackSize < stackSize_)
			{
				stack[stackSize++] = Visit(child, squaredDistances[o]);
			}
			else
			{
				last = doRangeSearch(child, center, squaredRadius, maxCount, first, last, info);
			}
		}
	}

	return last;
}



template <typename O, typename OT, typename SH>
bool QbvhTree<O, OT, SH>::volumeIntersect(const TAabb& box, const TRay& ray, const TVector& invDir, TReference t, TParam tMin) const
{
	if (TObjectTraits::aabbContains(box, TObjectTraits::rayPoint(ray, tMin)))
	{
		t = tMin;
		return true;
	}
	return TObjectTraits::aabbIntersect(box, ray, invDir, t, tMin);
}



template <typename O, typename OT, typename SH>
bool QbvhTree<O, OT, SH>::volumeIntersects(const TAabb& box, const TRay& ray, const TVector& invDir, TParam tMin, TParam tMax) const
{
	TValue t = 0;
	return volumeIntersect(box, ray, invDir, t, tMin) && t <= tMax;
}



template <typename O, typename OT, typename SH>
typename QbvhTree<O, OT, SH>::TPoint_
QbvhTree<O, OT, SH>::makePoint(const TPoint& point)
{
	TPoint_ result;
	for (size_t d = 0; d < dimension; ++d)
	{
		result.x[d] = OT::coord(point, d);
	}
	return result;
}



template <typename O, typename OT, typename SH>
typename QbvhTree<O, OT, SH>::TRay_
QbvhTree<O, OT, SH>::makeRay(const TRay& ray)
{
	TRay_ result;
	const auto support = OT::raySupport(ray);
	const auto direction = OT::rayDirection(ray);
	const auto invDir = TObjectTraits::vectorReciprocal(direction);
	for (size_t d = 0; d < dimension; ++d)
	{
		result.support[d] = OT::coord(support, d);
		result.invDir[d] = OT::coord(invDir, d);
		result.sign[d] = std::signbit(OT::coord(direction, d));
	}
	return result;
}



template <typename O, typename OT, typename SH>
typename QbvhTree<O, OT, SH>::TAabb_
QbvhTree<O, OT, SH>::makeAabb(const TAabb& aabb)
{
	TAabb_ result;
	const auto min = OT::aabbMin(aabb);
	const auto max = OT::aabbMax(aabb);
	for (size_t d = 0; d < dimension; ++d)
	{
		result.corners[d][0] = OT::coord(min, d);
		result.corners[d][1] = OT::coord(max, d);
	}
	return result;
}



}

}
