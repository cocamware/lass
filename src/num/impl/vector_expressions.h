/** @file
 *  @author Bram de Greve (bramz@users.sourceforge.net)
 *  @author Tom De Muer (tomdemuer@users.sourceforge.net)
 *
 *  Distributed under the terms of the GPL (GNU Public License)
 *
 *  The LASS License:
 *
 *  Copyright 2004 Bram de Greve and Tom De Muer
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
class VStorage
{
public:
	enum { lvalue = true };
	typedef typename util::CallTraits<T>::TValue TValue;
	typedef typename util::CallTraits<T>::TParam TParam;
	typedef typename util::CallTraits<T>::TReference TReference;
	typedef typename util::CallTraits<T>::TConstReference TConstReference;
	typedef size_t TSize;

	VStorage(): storage_() {}
	explicit VStorage(TSize iSize): storage_(iSize, T()) {}
	TReference operator[](TSize iIndex) { LASS_ASSERT(iIndex < size()); return storage_[iIndex]; }
	TConstReference operator[](TSize iIndex) const { LASS_ASSERT(iIndex < size()); return storage_[iIndex]; }
	TSize size() const { return storage_.size(); }

	// special VStorage members:
	//
	void resize(TSize iSize) { storage_.resize(iSize, T()); }
	void swap(VStorage<T>& iOther) { storage_.swap(iOther.storage_); }
private:
	std::vector<T> storage_;
};

/** @internal
 */
template <typename T>
class VScalar
{
public:
	enum { lvalue = false };
	typedef typename util::CallTraits<T>::TValue TValue;
	typedef typename util::CallTraits<T>::TParam TParam;
	typedef size_t TSize;

	VScalar(TParam iValue, TSize iSize): value_(iValue), size_(iSize) {}
	TParam operator[](TSize iIndex) const { LASS_ASSERT(iIndex < size_); return value_; }
	TSize size() const { return size_; }
private:
	TValue value_;
	TSize size_;
};



/** @internal
 */
template <typename ExpressionType>
struct VectorExpressionTraits
{
	typedef const ExpressionType& TStorage;
};

/** @internal
 */
template <typename T>
struct VectorExpressionTraits<VScalar<T> >
{
	typedef VScalar<T> TStorage;
};



/** @internal
 */
#define LASS_NUM_VECTOR_BINARY_EXPRESSION(i_name, c_operator)\
template <typename T, typename Operand1, typename Operand2>\
class LASS_CONCATENATE(V, i_name)\
{\
public:\
	enum { lvalue = false };\
	typedef typename util::CallTraits<T>::TValue TValue;\
	typedef size_t TSize;\
	LASS_CONCATENATE(V, i_name)(const Operand1& iA, const Operand2& iB):\
		operand1_(iA), operand2_(iB)\
	{\
		LASS_ASSERT(operand1_.size() == operand2_.size());\
	}\
	TValue operator[](TSize iIndex) const { return operand1_[iIndex] c_operator operand2_[iIndex]; }\
	TSize size() const { return operand1_.size(); }\
private:\
	typename VectorExpressionTraits<Operand1>::TStorage operand1_;\
	typename VectorExpressionTraits<Operand2>::TStorage operand2_;\
}

LASS_NUM_VECTOR_BINARY_EXPRESSION(Add, +);
LASS_NUM_VECTOR_BINARY_EXPRESSION(Sub, -);
LASS_NUM_VECTOR_BINARY_EXPRESSION(Mul, *);
LASS_NUM_VECTOR_BINARY_EXPRESSION(Div, /);



/** @internal
 */
#define LASS_NUM_VECTOR_UNARY_EXPRESSION(i_name, c_operator)\
template <typename T, typename Operand1>\
class LASS_CONCATENATE(V, i_name)\
{\
public:\
	enum { lvalue = false };\
	typedef typename util::CallTraits<T>::TValue TValue;\
	typedef size_t TSize;\
	LASS_CONCATENATE(V, i_name)(const Operand1& iA): operand1_(iA) {}\
	TValue operator[](TSize iIndex) const { return c_operator operand1_[iIndex]; }\
	TSize size() const { return operand1_.size(); }\
private:\
	typename VectorExpressionTraits<Operand1>::TStorage operand1_;\
}

LASS_NUM_VECTOR_UNARY_EXPRESSION(Neg, -);
LASS_NUM_VECTOR_UNARY_EXPRESSION(Rec, ::lass::num::NumTraits<T>::one /);



/** @internal
 */
template <typename T, typename Operand1>
class VFun
{
public:
	enum { lvalue = false };
	typedef T (*TOperator)(T);
	typedef typename util::CallTraits<T>::TValue TValue;
	typedef size_t TSize;
	VFun(const Operand1& iA, TOperator iOperator): operand1_(iA), operator_(iOperator) {}
	TValue operator[](TSize iIndex) const { return operator_(operand1_[iIndex]); }
	TSize size() const { return operand1_.size(); }
private:
	typename VectorExpressionTraits<Operand1>::TStorage operand1_;
	TOperator operator_;
};




}

}

}

#endif

// EOF
