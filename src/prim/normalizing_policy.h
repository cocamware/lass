/** @file
 *  @author Bram de Greve (bramz@users.sourceforge.net)
 *  @author Tom De Muer (tomdemuer@users.sourceforge.net)
 *
 *  Distributed under the terms of the GPL (GNU Public License)
 *
 *  The LASS License:
 *
 *  Copyright 2004-2006 Bram de Greve and Tom De Muer
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


/** @defgroup NormalizingPolicy NormalizingPolicy
 *  @brief policies to specify if direction vectors (and normals) should be normalized.
 *  @author Bram de Greve
 *  @date 2003
 *
 *  For direction vectors and normal vectors, you can be in the situation you want to
 *  have them normalized (unit length) and that you want this to be done automatically.
 *  Or you can be in the situation where you want them to be anything but normalized.
 *  These policies will let you set this.
 *
 *  The content of the policies are rather implementation details, clients shouldn't
 *  worry about it.
 *
 *  @note these policies can be used for vectors of any dimension.
 */

/** @struct lass::prim::Normalized
 *  @ingroup NormalizingPolicy
 *  @brief Policy to auto-normalize normals.
 *  @author Bram de Greve [BdG]
 *  @date 2003
 *
 *  Using this policy will automatically normalize all direction vectors and normals.
 *  That way, you don't have to bother what you have as input, you know that they will
 *  be normalized anyway.  Also, for the implementation of the primitives, optimized
 *  routines are provided that take advantage of the knowledge that the vectors are
 *  normalized.  But this are implementation details you should not worry about.
 */

/** @struct lass::prim::Unnormalized
 *  @ingroup NormalizingPolicy
 *  @brief Policy to keep normals unnormalized.
 *  @author Bram de Greve [BdG]
 *  @date 2003
 *
 *  Using this policy will keep direction vectors and normals at their original length.
 *  You can do this for precision reasons, for speed issues, or any other reason that
 *  is yours.  However, the maths will still be correct, because the implementations
 *  will know of this unnormalized vectors, and will correct with extra factors where
 *  necessary.  But, these corrections can sometimes be faster than using normalized
 *  vectors for start, because they often can avoid the square root.
 */

/** @struct lass::prim::IsAlreadyNormalized
 *  @ingroup NormalizingPolicy
 *  @date 2006
 *
 *  Empty argument to tell a function the client assures the vector coming in is already
 *  normalized, so the function doesn't has to.
 */

#ifndef LASS_GUARDIAN_OF_INCLUSION_PRIM_NORMALIZING_POLICY_H
#define LASS_GUARDIAN_OF_INCLUSION_PRIM_NORMALIZING_POLICY_H

#include "prim_common.h"



namespace lass
{

namespace prim
{

struct Normalized
{
	/** Normalize a vector iSubject
	 */
	template<typename VectorType>
	static void normalize(VectorType& ioSubject)
	{
		ioSubject.normalize();
	}

	/** Normalize iSubject, and scale iExtraValue as well so iExtraValue / iSubject.norm() is constant.
	 *  Typically used by cartesian equations.
	 *  @post iExtraValue / iSubject.norm() is the same as before the call.
	 */
	template<typename VectorType, typename ValueType>
	static void normalizeAndScale(VectorType& ioSubject, ValueType& ioExtraValue)
	{
		const typename VectorType::TValue norm = ioSubject.norm();
		LASS_ASSERT(norm != VectorType::TNumTraits::zero);
		ioExtraValue /= norm;
		ioSubject /= norm;
	}

	/** since iNormObject should be normalized by now, we can "divide by 1".
	 *  @return iValue
	 *  @pre iNormObject is supposed to be normalized!
	 */
	template<typename ValueType, typename VectorType>
	static ValueType divideByNorm(ValueType iValue, const VectorType& /*iNormObject*/)
	{
		return iValue;
	}

	/** since iNormObject should be normalized by now, we can "divide by 1".
	 *  @return iValue
	 *  @pre iNormObject is supposed to be normalized!
	 */
	template<typename ValueType, typename VectorType>
	static ValueType divideBySquaredNorm(ValueType iValue, const VectorType& /*iNormObject*/)
	{
		return iValue;
	}
};



struct Unnormalized
{
	/** Don't normalize normals we want to keep unnormalized! :)
	 *  this is a noop.
	 */
	template<typename VectorType>
	static void normalize(VectorType& /*ioSubject*/)
	{
	}


	/** Don't normalize and don't scale
	 */
	template<typename VectorType, typename ValueType>
	static void normalizeAndScale(VectorType& /*ioSubject*/, ValueType& /*ioExtraValue*/)
	{
	}

	/** @return iValue divided by the norm of iNormObject
	 */
	template<typename ValueType, typename VectorType>
	static ValueType divideByNorm(ValueType iValue, const VectorType& iNormObject)
	{
		return iValue / iNormObject.norm();
	}

	/** @return iValue divided by the squared norm of iNormObject
	 */
	template<typename ValueType, typename VectorType>
	static ValueType divideBySquaredNorm(ValueType iValue, const VectorType& iNormObject)
	{
		return iValue / iNormObject.squaredNorm();
	}
};



struct IsAlreadyNormalized
{
};




}

}

#endif
