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



#ifndef LASS_GUARDIAN_OF_INCLUSION_NUM_IMPL_MATRIX_EXPRESSIONS_H
#define LASS_GUARDIAN_OF_INCLUSION_NUM_IMPL_MATRIX_EXPRESSIONS_H

#include "../num_common.h"

namespace lass
{
namespace num
{
namespace impl
{

/** @internal
*/
template <typename T>
class MStorage
{
public:
	enum { lvalue = true };
	typedef typename util::CallTraits<T>::TValue TValue;
	typedef typename util::CallTraits<T>::TParam TParam;
	typedef typename util::CallTraits<T>::TReference TReference;
	typedef typename util::CallTraits<T>::TConstReference TConstReference;
	typedef size_t TSize;

	MStorage(): storage_(), rows_(0), cols_(0) {}
	MStorage(TSize iRows, TSize iCols): storage_(iRows * iCols, T()), rows_(iRows), cols_(iCols) {}
	TConstReference operator()(TSize iI, TSize iJ) const { return storage_[iI * cols_ + iJ]; }
	TReference operator()(TSize iI, TSize iJ) { return storage_[iI * cols_ + iJ]; }
	TSize rows() const { return rows_; }
	TSize columns() const { return cols_; }

	// special MStorage members:
	//
	void resize(TSize iRows, TSize iCols)
	{
		storage_.resize(iRows * iCols, T());
		rows_ = iRows;
		cols_ = iCols;
	}
	void swap(MStorage<T>& iOther)
	{
		storage_.swap(iOther.storage_);
		std::swap(rows_, iOther.rows_);
		std::swap(cols_, iOther.cols_);
	}

	typename std::vector<T>::iterator rowMajor() { return storage_.begin(); } 
	typename std::vector<T>::const_iterator rowMajor() const { return storage_.begin(); } 

private:
	std::vector<T> storage_;
	TSize rows_;
	TSize cols_;
};

/** @internal
*/
template <typename T>
class MScalar
{
public:
	enum { lvalue = false };
	typedef typename util::CallTraits<T>::TValue TValue;
	typedef typename util::CallTraits<T>::TParam TParam;
	typedef size_t TSize;

	MScalar(TParam iValue, TSize iRows, TSize iCols): value_(iValue), rows_(iRows), cols_(iCols) {}
	TParam operator()(TSize iI, TSize iJ) const
	{
		LASS_ASSERT(iI < rows_ && iJ < cols_);
		return value_;
	}
	TSize rows() const { return rows_; }
	TSize columns() const { return cols_; }
private:
	TValue value_;
	TSize rows_;
	TSize cols_;
};



template <typename ExpressionType>
struct MatrixExpressionTraits
{
	typedef const ExpressionType& TStorage;
};

template <typename T>
struct MatrixExpressionTraits<MScalar<T> >
{
	typedef MScalar<T> TStorage;
};



#define LASS_NUM_MATRIX_BINARY_EXPRESSION(i_name, c_operator)\
template <typename T, typename Operand1, typename Operand2>\
class LASS_CONCATENATE(M, i_name)\
{\
public:\
	enum { lvalue = false };\
	typedef typename util::CallTraits<T>::TValue TValue;\
	typedef size_t TSize;\
	LASS_CONCATENATE(M, i_name)(const Operand1& iA, const Operand2& iB):\
		operand1_(iA), operand2_(iB)\
	{\
		LASS_ASSERT(operand1_.rows() == operand2_.rows() &&\
			operand1_.columns() == operand2_.columns());\
	}\
	TValue operator()(TSize iI, TSize iJ) const\
	{\
		return operand1_(iI, iJ) c_operator operand2_(iI, iJ);\
	}\
	TSize rows() const { return operand1_.rows(); }\
	TSize columns() const { return operand1_.columns(); }\
private:\
	typename MatrixExpressionTraits<Operand1>::TStorage operand1_;\
	typename MatrixExpressionTraits<Operand2>::TStorage operand2_;\
}

LASS_NUM_MATRIX_BINARY_EXPRESSION(Add, +);
LASS_NUM_MATRIX_BINARY_EXPRESSION(Sub, -);
LASS_NUM_MATRIX_BINARY_EXPRESSION(Mul, *);



#define LASS_NUM_MATRIX_UNARY_EXPRESSION(i_name, c_operator)\
template <typename T, typename Operand1>\
class LASS_CONCATENATE(M, i_name)\
{\
public:\
	enum { lvalue = false };\
	typedef typename util::CallTraits<T>::TValue TValue;\
	typedef size_t TSize;\
	LASS_CONCATENATE(M, i_name)(const Operand1& iA):\
		operand1_(iA)\
	{\
	}\
	TValue operator()(TSize iRow, TSize iCol) const\
	{\
		return c_operator operand1_(iRow, iCol);\
	}\
	TSize rows() const { return operand1_.rows(); }\
	TSize columns() const { return operand1_.columns(); }\
private:\
	typename MatrixExpressionTraits<Operand1>::TStorage operand1_;\
}

LASS_NUM_MATRIX_UNARY_EXPRESSION(Neg, -);
LASS_NUM_MATRIX_UNARY_EXPRESSION(Rec, ::lass::num::NumTraits<T>::one /);



/** @internal
*  matrix product
*/
template <typename T, typename Operand1, typename Operand2>
class MProd
{
public:
	enum { lvalue = false };
	typedef typename util::CallTraits<T>::TValue TValue;
	typedef size_t TSize;
	MProd(const Operand1& iA, const Operand2& iB):
		operand1_(iA), operand2_(iB), loopSize_(iA.columns())
	{
		LASS_ASSERT(operand1_.columns() == operand2_.rows());
	}
	TValue operator()(TSize iI, TSize iJ) const
	{
		TValue result = TValue();
		for (TSize k = 0; k < loopSize_; ++k)
		{
			result += operand1_(iI, k) * operand2_(k, iJ);
		}
		return result;
	}
	TSize rows() const { return operand1_.rows(); }
	TSize columns() const { return operand2_.columns(); }
private:
	typename MatrixExpressionTraits<Operand1>::TStorage operand1_;
	typename MatrixExpressionTraits<Operand2>::TStorage operand2_;
	TSize loopSize_;
};


/** @internal
*  transpose matrix
*/
template <typename T, typename Operand1>
class MTrans
{
public:
	enum { lvalue = false };
	typedef typename util::CallTraits<T>::TParam TParam;
	typedef size_t TSize;
	MTrans(const Operand1& iA): operand1_(iA) {}
	TParam operator()(TSize iI, TSize iJ) const { return operand1_(iJ, iI);
}
	TSize rows() const { return operand1_.columns(); }
	TSize columns() const { return operand1_.rows(); }
private:
	typename MatrixExpressionTraits<Operand1>::TStorage operand1_;
};



/** @internal
*  apply function
*/
template <typename T, typename Operand1>
class MFun
{
public:
	enum { lvalue = false };
	typedef T (*TOperator)(T);
	typedef typename util::CallTraits<T>::TValue TValue;
	typedef size_t TSize;
	MFun(const Operand1& iA, TOperator iOperator): operand1_(iA),
operator_(iOperator) {}
	TValue operator()(TSize iI, TSize iJ) const { return
operator_(operand1_(iI, iJ)); }
	TSize rows() const { return operand1_.rows(); }\
	TSize columns() const { return operand1_.columns(); }\
private:
	typename MatrixExpressionTraits<Operand1>::TStorage operand1_;
	TOperator operator_;
};



}

}

}

#endif

// EOF
