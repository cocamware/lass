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



#ifndef LASS_GUARDIAN_OF_INCLUSION_NUM_IMPL_VECTOR_EXPRESSIONS_H
#define LASS_GUARDIAN_OF_INCLUSION_NUM_IMPL_VECTOR_EXPRESSIONS_H

#include "../num_common.h"

namespace lass
{
namespace num
{
namespace impl
{

template <typename T>
class VScalar
{
public:
	typedef typename util::CallTraits<T>::TValue TValue;
	typedef typename util::CallTraits<T>::TParam TParam;
	typedef size_t TSize;

	VScalar(TParam iValue, TSize iSize): value_(iValue), size_(iSize) {}
	TParam operator[](TSize iIndex) const {	LASS_ASSERT(iIndex < size_); return value_; }
	TSize size() const { return size_; }
	bool empty() const { return size_ == 0; }
private:
	TValue value_;
	TSize size_;
};



template <typename ExpressionType> 
struct VectorExpressionTraits
{
	typedef const ExpressionType& TStorage;
};

template <typename T> 
struct VectorExpressionTraits<VScalar<T> >
{
	typedef VScalar<T> TStorage;
};



#define LASS_NUM_VECTOR_BINARY_EXPRESSION(name__, operator__)\
template <typename T, typename Operand1, typename Operand2>\
class LASS_CONCATENATE(V, name__)\
{\
public:\
	typedef typename util::CallTraits<T>::TValue TValue;\
	typedef size_t TSize;\
	LASS_CONCATENATE(V, name__)(const Operand1& iA, const Operand2& iB):\
		operand1_(iA),\
		operand2_(iB)\
	{\
		LASS_ASSERT(operand1_.size() == operand2_.size());\
	}\
	TValue operator[](TSize iIndex) const\
	{\
		return operand1_[iIndex] operator__ operand2_[iIndex];\
	}\
	TSize size() const { return operand1_.size(); }\
	bool empty() const { return operand1_.empty(); }\
private:\
	typename VectorExpressionTraits<Operand1>::TStorage operand1_;\
	typename VectorExpressionTraits<Operand2>::TStorage operand2_;\
}

LASS_NUM_VECTOR_BINARY_EXPRESSION(Add, +);
LASS_NUM_VECTOR_BINARY_EXPRESSION(Sub, -);
LASS_NUM_VECTOR_BINARY_EXPRESSION(Mul, *);
LASS_NUM_VECTOR_BINARY_EXPRESSION(Div, /);



#define LASS_NUM_VECTOR_UNARY_EXPRESSION(name__, operator__)\
template <typename T, typename Operand1>\
class LASS_CONCATENATE(V, name__)\
{\
public:\
	typedef typename util::CallTraits<T>::TValue TValue;\
	typedef size_t TSize;\
	LASS_CONCATENATE(V, name__)(const Operand1& iA):\
		operand1_(iA)\
	{\
	}\
	TValue operator[](TSize iIndex) const\
	{\
		return operator__ operand1_[iIndex];\
	}\
	TSize size() const { return operand1_.size(); }\
	bool empty() const { return operand1_.empty(); }\
private:\
	typename VectorExpressionTraits<Operand1>::TStorage operand1_;\
}

LASS_NUM_VECTOR_UNARY_EXPRESSION(Neg, -);
LASS_NUM_VECTOR_UNARY_EXPRESSION(Rec, ::lass::num::NumTraits<T>::one /);



template <typename T, typename Operand1>
class VFun
{
public:
	typedef T (*TFunction)(T);
	typedef typename util::CallTraits<T>::TValue TValue;
	typedef size_t TSize;
	VFun(const Operand1& iA, TFunction iFun):
		operand1_(iA),
		function_(iFun)
	{
	}
	TValue operator[](TSize iIndex) const
	{
		return function_(operand1_[iIndex]);
	}
	TSize size() const { return operand1_.size(); }
	bool empty() const { return operand1_.empty(); }
private:
	typename VectorExpressionTraits<Operand1>::TStorage operand1_;
	TFunction function_;
};



}

}

}

#endif

// EOF
