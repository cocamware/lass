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



#ifndef LASS_GUARDIAN_OF_INCLUSION_NUM_IMPL_MATRIX_VECTOR_EXPRESSIONS_H
#define LASS_GUARDIAN_OF_INCLUSION_NUM_IMPL_MATRIX_VECTOR_EXPRESSIONS_H

#include "../num_common.h"

namespace lass
{
namespace num
{
namespace impl
{

template <typename T, typename VectorOperand1>
class MVColumn
{
public:
	enum { lvalue = VectorOperand1::lvalue };
	typedef typename util::CallTraits<T>::TValue TValue;
	typedef typename util::CallTraits<T>::TReference TReference;
	typedef size_t TSize;
	MVColumn(VectorOperand1& iA): operand1_(iA) {}
	TValue operator()(TSize iI, TSize /*iJ*/) const { return operand1_[iI]; }
	TReference operator()(TSize iI, TSize /*iJ*/) { return operand1_[iI]; }
	TSize rows() const { return operand1_.size(); }\
	TSize columns() const { return 1; }\

	void resize(TSize iRows, TSize iCols)
	{
		LASS_ENFORCE(iCols == 1);
		operand1_.resize(iRows);
	}
private:
	VectorOperand1& operand1_;
};



template <typename T, typename VectorOperand1>
class MVDiag
{
public:
	enum { lvalue = false };
	typedef typename util::CallTraits<T>::TValue TValue;
	typedef size_t TSize;
	MVDiag(const VectorOperand1& iA): operand1_(iA) {}
	TValue operator()(TSize iI, TSize iJ) const
	{
		return iI == iJ ? operand1_[iI] : NumTraits<T>::zero;
	}
	TSize rows() const { return operand1_.size(); }\
	TSize columns() const { return operand1_.size(); }\
private:
	typename VectorExpressionTraits<VectorOperand1>::TStorage operand1_;
};



template <typename T, typename MatrixOperand1, typename VectorOperand2>
class MVRightProd
{
public:
	enum { lvalue = false };
	typedef typename util::CallTraits<T>::TValue TValue;
	typedef size_t TSize;
	MVRightProd(const MatrixOperand1& iA, const VectorOperand2& iB):
operand1_(iA), operand2_(iB)
	{
		LASS_ASSERT(iA.columns() == iB.size());
	}
	TValue operator[](TSize iIndex) const
	{
		const TSize n = operand1_.columns();
		TValue result = NumTraits<T>::zero;
		for (TSize k = 0; k < n; ++k)
		{
			result += operand1_(iIndex, k) * operand2_[k];
		}
		return result;
	}
	TSize size() const { return operand1_.rows(); }
private:
	typename MatrixExpressionTraits<MatrixOperand1>::TStorage operand1_;
	typename VectorExpressionTraits<VectorOperand2>::TStorage operand2_;
};



}

}

}

#endif

// EOF
