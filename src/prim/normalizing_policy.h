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
