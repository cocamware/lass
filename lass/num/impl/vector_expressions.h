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
 *	Copyright (C) 2004-2024 the Initial Developer.
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
	VStorage(TSize iSize, TParam iInitialValue): storage_(iSize, iInitialValue) {}
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

	VScalar(TSize iSize, TParam iValue): value_(iValue), size_(iSize) {}
	TParam operator[]([[maybe_unused]] TSize iIndex) const { LASS_ASSERT(iIndex < size_); return value_; }
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
