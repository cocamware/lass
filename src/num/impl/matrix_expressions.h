/**	@file
 *	@author Bram de Greve (bramz@users.sourceforge.net)
 *	@author Tom De Muer (tomdemuer@users.sourceforge.net)
 *
 *	Distributed under the terms of the GPL (GNU Public License)
 *
 * 	The LASS License:
 *
 *	Copyright 2004 Bram de Greve and Tom De Muer
 * 
 *	LASS is free software; you can redistribute it and/or modify
 *	it under the terms of the GNU General Public License as published by
 *	the Free Software Foundation; either version 2 of the License, or
 *	(at your option) any later version.
 *
 *	This program is distributed in the hope that it will be useful,
 *	but WITHOUT ANY WARRANTY; without even the implied warranty of
 *	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *	GNU General Public License for more details.
 *
 *	You should have received a copy of the GNU General Public License
 *	along with this program; if not, write to the Free Software
 *	Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
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
    MStorage(TSize iRows, TSize iCols): storage_(iRows * iCols, T()), 
rows_(iRows), cols_(iCols) {}
    TConstReference operator()(TSize iI, TSize iJ) const { return 
storage_[iI * cols_ + iJ]; }
    TReference operator()(TSize iI, TSize iJ) { return storage_[iI * cols_ + 
iJ]; }
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

	MScalar(TParam iValue, TSize iRows, TSize iCols): value_(iValue), 
rows_(iRows), cols_(iCols) {}
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
