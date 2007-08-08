/** @file
 *  @internal
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



#ifndef LASS_GUARDIAN_OF_INCLUSION_TEST_TEST_META_TYPE_TRAITS_INL
#define LASS_GUARDIAN_OF_INCLUSION_TEST_TEST_META_TYPE_TRAITS_INL

#include "test_common.h"

#include "../meta/type_traits.h"
#include "../meta/is_same.h"

#define LASS_TEST_META_TYPE_TRAITS_EVAL(type)\
	LASS_COUT << LASS_STRINGIFY(type) << std::endl;\
	LASS_COUT << "Type: " << typeid(TypeTraits<type>::Type).name() << std::endl;\
	LASS_COUT << (TypeTraits<type>::isConst ? "isConst" : "---") << ", ";\
	LASS_COUT << (TypeTraits<type>::isPointer ? "isPointer" : "---") << ", ";\
	LASS_COUT << (TypeTraits<type>::isReference ? "isReference" : "---") << std::endl;\
	LASS_COUT << "TNonConst: " << typeid(typename TypeTraits<type>::TNonConst).name() << std::endl;\
	LASS_COUT << "TConst: " << typeid(typename TypeTraits<type>::TConst).name() << std::endl;\
	LASS_COUT << "TPointee: " << typeid(typename TypeTraits<type>::TPointee).name() << std::endl;\
	LASS_COUT << "TReferred: " << typeid(typename TypeTraits<type>::TReferred).name() << std::endl;\
	LASS_COUT << "TReference: " << typeid(typename TypeTraits<type>::TReference).name() << std::endl;\
	LASS_COUT << "TStorage: " << typeid(typename TypeTraits<type>::TStorage).name() << std::endl;\
	LASS_COUT << std::endl

namespace lass
{
namespace test
{

// --- public --------------------------------------------------------------------------------------

template <typename T>
void testMetaTypeTraits()
{
	using namespace meta;

	typedef TypeTraits<T> TTraits;
	LASS_TEST_CHECK(!TTraits::isConst);
	LASS_TEST_CHECK(!TTraits::isPointer);
	LASS_TEST_CHECK(!TTraits::isReference);
	LASS_TEST_CHECK((IsSame<typename TTraits::Type, T>::value));
	LASS_TEST_CHECK((IsSame<typename TTraits::TNonConst, T>::value));
	LASS_TEST_CHECK((IsSame<typename TTraits::TConst, T const>::value));
	LASS_TEST_CHECK((IsSame<typename TTraits::TPointee, NullType>::value));
	LASS_TEST_CHECK((IsSame<typename TTraits::TReferred, T>::value));
	LASS_TEST_CHECK((IsSame<typename TTraits::TReference, T&>::value));
	LASS_TEST_CHECK((IsSame<typename TTraits::TStorage, T>::value));

	typedef TypeTraits<const T> TConstTraits;
	LASS_TEST_CHECK(TConstTraits::isConst);
	LASS_TEST_CHECK(!TConstTraits::isPointer);
	LASS_TEST_CHECK(!TConstTraits::isReference);
	LASS_TEST_CHECK((IsSame<typename TConstTraits::Type, const T>::value));
	LASS_TEST_CHECK((IsSame<typename TConstTraits::TNonConst, T>::value));
	LASS_TEST_CHECK((IsSame<typename TConstTraits::TConst, const T>::value));
	LASS_TEST_CHECK((IsSame<typename TConstTraits::TPointee, NullType>::value));
	LASS_TEST_CHECK((IsSame<typename TConstTraits::TReferred, const T>::value));
	LASS_TEST_CHECK((IsSame<typename TConstTraits::TReference, const T&>::value));
	LASS_TEST_CHECK((IsSame<typename TConstTraits::TStorage, T>::value));

	typedef TypeTraits<T*> TPtrTraits;
	LASS_TEST_CHECK(!TPtrTraits::isConst);
	LASS_TEST_CHECK(TPtrTraits::isPointer);
	LASS_TEST_CHECK(!TPtrTraits::isReference);
	LASS_TEST_CHECK((IsSame<typename TPtrTraits::Type, T*>::value));
	LASS_TEST_CHECK((IsSame<typename TPtrTraits::TNonConst, T*>::value));
	LASS_TEST_CHECK((IsSame<typename TPtrTraits::TConst, T* const>::value));
	LASS_TEST_CHECK((IsSame<typename TPtrTraits::TPointee, T>::value));
	LASS_TEST_CHECK((IsSame<typename TPtrTraits::TReferred, T*>::value));
	LASS_TEST_CHECK((IsSame<typename TPtrTraits::TReference, T*&>::value));
	LASS_TEST_CHECK((IsSame<typename TPtrTraits::TStorage, T*>::value));

	typedef TypeTraits<const T*> TConstPtrTraits;
	LASS_TEST_CHECK(!TConstPtrTraits::isConst);
	LASS_TEST_CHECK(TConstPtrTraits::isPointer);
	LASS_TEST_CHECK(!TConstPtrTraits::isReference);
	LASS_TEST_CHECK((IsSame<typename TConstPtrTraits::Type, const T*>::value));
	LASS_TEST_CHECK((IsSame<typename TConstPtrTraits::TNonConst, const T*>::value));
	LASS_TEST_CHECK((IsSame<typename TConstPtrTraits::TConst, const T* const>::value));
	LASS_TEST_CHECK((IsSame<typename TConstPtrTraits::TPointee, const T>::value));
	LASS_TEST_CHECK((IsSame<typename TConstPtrTraits::TReferred, const T*>::value));
	LASS_TEST_CHECK((IsSame<typename TConstPtrTraits::TReference, const T*&>::value));
	LASS_TEST_CHECK((IsSame<typename TConstPtrTraits::TStorage, const T*>::value));

	typedef TypeTraits<T* const> TPtrConstTraits;
	LASS_TEST_CHECK(TPtrConstTraits::isConst);
	LASS_TEST_CHECK(TPtrConstTraits::isPointer);
	LASS_TEST_CHECK(!TPtrConstTraits::isReference);
	LASS_TEST_CHECK((IsSame<typename TPtrConstTraits::Type, T* const>::value));
	LASS_TEST_CHECK((IsSame<typename TPtrConstTraits::TNonConst, T*>::value));
	LASS_TEST_CHECK((IsSame<typename TPtrConstTraits::TConst, T* const>::value));
	LASS_TEST_CHECK((IsSame<typename TPtrConstTraits::TPointee, T>::value));
	LASS_TEST_CHECK((IsSame<typename TPtrConstTraits::TReferred, T* const>::value));
	LASS_TEST_CHECK((IsSame<typename TPtrConstTraits::TReference, T* const&>::value));
	LASS_TEST_CHECK((IsSame<typename TPtrConstTraits::TStorage, T*>::value));

	typedef TypeTraits<const T* const> TConstPtrConstTraits;
	LASS_TEST_CHECK(TConstPtrConstTraits::isConst);
	LASS_TEST_CHECK(TConstPtrConstTraits::isPointer);
	LASS_TEST_CHECK(!TConstPtrConstTraits::isReference);
	LASS_TEST_CHECK((IsSame<typename TConstPtrConstTraits::Type, const T* const>::value));
	LASS_TEST_CHECK((IsSame<typename TConstPtrConstTraits::TNonConst, const T*>::value));
	LASS_TEST_CHECK((IsSame<typename TConstPtrConstTraits::TConst, const T* const>::value));
	LASS_TEST_CHECK((IsSame<typename TConstPtrConstTraits::TPointee, const T>::value));
	LASS_TEST_CHECK((IsSame<typename TConstPtrConstTraits::TReferred, const T* const>::value));
	LASS_TEST_CHECK((IsSame<typename TConstPtrConstTraits::TReference, const T* const &>::value));
	LASS_TEST_CHECK((IsSame<typename TConstPtrConstTraits::TStorage, const T*>::value));

	typedef TypeTraits<T&> TRefTraits;
	LASS_TEST_CHECK(!TRefTraits::isConst);
	LASS_TEST_CHECK(!TRefTraits::isPointer);
	LASS_TEST_CHECK(TRefTraits::isReference);
	LASS_TEST_CHECK((IsSame<typename TRefTraits::Type, T&>::value));
	LASS_TEST_CHECK((IsSame<typename TRefTraits::TNonConst, T&>::value));
	LASS_TEST_CHECK((IsSame<typename TRefTraits::TConst, const T&>::value));
	LASS_TEST_CHECK((IsSame<typename TRefTraits::TPointee, NullType>::value));
	LASS_TEST_CHECK((IsSame<typename TRefTraits::TReferred, T>::value));
	LASS_TEST_CHECK((IsSame<typename TRefTraits::TReference, T&>::value));
	LASS_TEST_CHECK((IsSame<typename TRefTraits::TStorage, T>::value));

	typedef TypeTraits<const T&> TConstRefTraits;
	LASS_TEST_CHECK(TConstRefTraits::isConst);
	LASS_TEST_CHECK(!TConstRefTraits::isPointer);
	LASS_TEST_CHECK(TConstRefTraits::isReference);
	LASS_TEST_CHECK((IsSame<typename TConstRefTraits::Type, const T&>::value));
	LASS_TEST_CHECK((IsSame<typename TConstRefTraits::TNonConst, T&>::value));
	LASS_TEST_CHECK((IsSame<typename TConstRefTraits::TConst, const T&>::value));
	LASS_TEST_CHECK((IsSame<typename TConstRefTraits::TPointee, NullType>::value));
	LASS_TEST_CHECK((IsSame<typename TConstRefTraits::TReferred, const T >::value));
	LASS_TEST_CHECK((IsSame<typename TConstRefTraits::TReference, const T&>::value));
	LASS_TEST_CHECK((IsSame<typename TConstRefTraits::TStorage, T>::value));

	typedef TypeTraits<T*&> TPtrRefTraits;
	LASS_TEST_CHECK(!TPtrRefTraits::isConst);
	LASS_TEST_CHECK(TPtrRefTraits::isPointer); //?????????
	LASS_TEST_CHECK(TPtrRefTraits::isReference);
	LASS_TEST_CHECK((IsSame<typename TPtrRefTraits::Type, T*&>::value));
	LASS_TEST_CHECK((IsSame<typename TPtrRefTraits::TNonConst, T*&>::value));
	LASS_TEST_CHECK((IsSame<typename TPtrRefTraits::TConst, T* const&>::value));
	LASS_TEST_CHECK((IsSame<typename TPtrRefTraits::TPointee, T>::value));
	LASS_TEST_CHECK((IsSame<typename TPtrRefTraits::TReferred, T*>::value));
	LASS_TEST_CHECK((IsSame<typename TPtrRefTraits::TReference, T*&>::value));
	LASS_TEST_CHECK((IsSame<typename TPtrRefTraits::TStorage, T*>::value));

	typedef TypeTraits<const T*&> TConstPtrRefTraits;
	LASS_TEST_CHECK(!TConstPtrRefTraits::isConst);
	LASS_TEST_CHECK(TConstPtrRefTraits::isPointer);
	LASS_TEST_CHECK(TConstPtrRefTraits::isReference);
	LASS_TEST_CHECK((IsSame<typename TConstPtrRefTraits::Type, const T*&>::value));
	LASS_TEST_CHECK((IsSame<typename TConstPtrRefTraits::TNonConst, const T*&>::value));
	LASS_TEST_CHECK((IsSame<typename TConstPtrRefTraits::TConst, const T* const&>::value));
	LASS_TEST_CHECK((IsSame<typename TConstPtrRefTraits::TPointee, const T>::value));
	LASS_TEST_CHECK((IsSame<typename TConstPtrRefTraits::TReferred, const T*>::value));
	LASS_TEST_CHECK((IsSame<typename TConstPtrRefTraits::TReference, const T*&>::value));
	LASS_TEST_CHECK((IsSame<typename TConstPtrRefTraits::TStorage, const T*>::value));

	typedef TypeTraits<T* const&> TPtrConstRefTraits;
	LASS_TEST_CHECK(TPtrConstRefTraits::isConst);
	LASS_TEST_CHECK(TPtrConstRefTraits::isPointer);
	LASS_TEST_CHECK(TPtrConstRefTraits::isReference);
	LASS_TEST_CHECK((IsSame<typename TPtrConstRefTraits::Type, T* const&>::value));
	LASS_TEST_CHECK((IsSame<typename TPtrConstRefTraits::TNonConst, T*&>::value));
	LASS_TEST_CHECK((IsSame<typename TPtrConstRefTraits::TConst, T* const&>::value));
	LASS_TEST_CHECK((IsSame<typename TPtrConstRefTraits::TPointee, T>::value));
	LASS_TEST_CHECK((IsSame<typename TPtrConstRefTraits::TReferred, T* const>::value));
	LASS_TEST_CHECK((IsSame<typename TPtrConstRefTraits::TReference, T* const&>::value));
	LASS_TEST_CHECK((IsSame<typename TPtrConstRefTraits::TStorage, T*>::value));

	typedef TypeTraits<const T* const&> TConstPtrConstRefTraits;
	LASS_TEST_CHECK(TConstPtrConstRefTraits::isConst);
	LASS_TEST_CHECK(TConstPtrConstRefTraits::isPointer);
	LASS_TEST_CHECK(TConstPtrConstRefTraits::isReference);
	LASS_TEST_CHECK((IsSame<typename TConstPtrConstRefTraits::Type, const T* const&>::value));
	LASS_TEST_CHECK((IsSame<typename TConstPtrConstRefTraits::TNonConst, const T*&>::value));
	LASS_TEST_CHECK((IsSame<typename TConstPtrConstRefTraits::TConst, const T* const&>::value));
	LASS_TEST_CHECK((IsSame<typename TConstPtrConstRefTraits::TPointee, const T>::value));
	LASS_TEST_CHECK((IsSame<typename TConstPtrConstRefTraits::TReferred, const T* const>::value));
	LASS_TEST_CHECK((IsSame<typename TConstPtrConstRefTraits::TReference, const T* const &>::value));
	LASS_TEST_CHECK((IsSame<typename TConstPtrConstRefTraits::TStorage, const T*>::value));
}

}

}

#endif
