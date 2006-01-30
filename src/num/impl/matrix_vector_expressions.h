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
	TValue operator()(TSize iI, TSize iJ) const { return operand1_[iI]; }
	TReference operator()(TSize iI, TSize iJ) { return operand1_[iI]; }
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
