/** 
*   @author Bram de Greve (bramz@users.sourceforge.net)
*   @author Tom De Muer (tomdemuer@users.sourceforge.net)
*
*	Distributed under the terms of the GPL (GNU Public License)
*
* 	The LASS License:
*   
*	Copyright 2004 Bram de Greve and Tom De Muer
*
*   LASS is free software; you can redistribute it and/or modify
*   it under the terms of the GNU General Public License as published by
*   the Free Software Foundation; either version 2 of the License, or
*   (at your option) any later version.
*
*   This program is distributed in the hope that it will be useful,
*   but WITHOUT ANY WARRANTY; without even the implied warranty of
*   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
*   GNU General Public License for more details.
*
*   You should have received a copy of the GNU General Public License
*   along with this program; if not, write to the Free Software
*   Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
*
*/
/** @internal
 *  @file test/test_meta_type_traits.inl
 *  @brief implementation of lass::test::testMetaTypeList
 *  @author [BdG] Ghent University - INTEC (acoustics)
 *  @date 2004
 */

#ifndef LASS_GUARDIAN_OF_INCLUSION_TEST_TEST_META_TYPE_TRAITS_INL
#define LASS_GUARDIAN_OF_INCLUSION_TEST_TEST_META_TYPE_TRAITS_INL

#include "test_common.h"

#include "../meta/type_traits.h"
#include "../meta/is_same_type.h"

#define LASS_TEST_META_TYPE_TRAITS_EVAL(type)\
    std::cout << LASS_STRINGIFY(type) << std::endl;\
    std::cout << "Type: " << typeid(TypeTraits<type>::Type).name() << std::endl;\
    std::cout << (TypeTraits<type>::isConst ? "isConst" : "---") << ", ";\
    std::cout << (TypeTraits<type>::isPointer ? "isPointer" : "---") << ", ";\
    std::cout << (TypeTraits<type>::isReference ? "isReference" : "---") << std::endl;\
    std::cout << "TNonConst: " << typeid(TypeTraits<type>::TNonConst).name() << std::endl;\
    std::cout << "TConst: " << typeid(TypeTraits<type>::TConst).name() << std::endl;\
    std::cout << "TPointee: " << typeid(TypeTraits<type>::TPointee).name() << std::endl;\
    std::cout << "TReferred: " << typeid(TypeTraits<type>::TReferred).name() << std::endl;\
    std::cout << "TReference: " << typeid(TypeTraits<type>::TReference).name() << std::endl;\
    std::cout << "TStorage: " << typeid(TypeTraits<type>::TStorage).name() << std::endl;\
    std::cout << std::endl
  
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
    BOOST_CHECK(!TTraits::isConst);
    BOOST_CHECK(!TTraits::isPointer);
    BOOST_CHECK(!TTraits::isReference);
    BOOST_CHECK((IsSameType<TTraits::Type, T>::value));
    BOOST_CHECK((IsSameType<TTraits::TNonConst, T>::value));
    BOOST_CHECK((IsSameType<TTraits::TConst, T const>::value));
    BOOST_CHECK((IsSameType<TTraits::TPointee, NullType>::value));
    BOOST_CHECK((IsSameType<TTraits::TReferred, T>::value));
    BOOST_CHECK((IsSameType<TTraits::TReference, T&>::value));
    BOOST_CHECK((IsSameType<TTraits::TStorage, T>::value));

    typedef TypeTraits<const T> TConstTraits;
    BOOST_CHECK(TConstTraits::isConst);
    BOOST_CHECK(!TConstTraits::isPointer);
    BOOST_CHECK(!TConstTraits::isReference);
    BOOST_CHECK((IsSameType<TConstTraits::Type, const T>::value));
    BOOST_CHECK((IsSameType<TConstTraits::TNonConst, T>::value));
    BOOST_CHECK((IsSameType<TConstTraits::TConst, const T>::value));
    BOOST_CHECK((IsSameType<TConstTraits::TPointee, NullType>::value));
    BOOST_CHECK((IsSameType<TConstTraits::TReferred, const T>::value));
    BOOST_CHECK((IsSameType<TConstTraits::TReference, const T&>::value));
    BOOST_CHECK((IsSameType<TConstTraits::TStorage, T>::value));

    typedef TypeTraits<T*> TPtrTraits;
    BOOST_CHECK(!TPtrTraits::isConst);
    BOOST_CHECK(TPtrTraits::isPointer);
    BOOST_CHECK(!TPtrTraits::isReference);
    BOOST_CHECK((IsSameType<TPtrTraits::Type, T*>::value));
    BOOST_CHECK((IsSameType<TPtrTraits::TNonConst, T*>::value));
    BOOST_CHECK((IsSameType<TPtrTraits::TConst, T* const>::value));
    BOOST_CHECK((IsSameType<TPtrTraits::TPointee, T>::value));
    BOOST_CHECK((IsSameType<TPtrTraits::TReferred, T*>::value));
    BOOST_CHECK((IsSameType<TPtrTraits::TReference, T*&>::value));
    BOOST_CHECK((IsSameType<TPtrTraits::TStorage, T*>::value));

    typedef TypeTraits<const T*> TConstPtrTraits;
    BOOST_CHECK(!TConstPtrTraits::isConst);
    BOOST_CHECK(TConstPtrTraits::isPointer);
    BOOST_CHECK(!TConstPtrTraits::isReference);
    BOOST_CHECK((IsSameType<TConstPtrTraits::Type, const T*>::value));
    BOOST_CHECK((IsSameType<TConstPtrTraits::TNonConst, const T*>::value));
    BOOST_CHECK((IsSameType<TConstPtrTraits::TConst, const T* const>::value));
    BOOST_CHECK((IsSameType<TConstPtrTraits::TPointee, const T>::value));
    BOOST_CHECK((IsSameType<TConstPtrTraits::TReferred, const T*>::value));
    BOOST_CHECK((IsSameType<TConstPtrTraits::TReference, const T*&>::value));
    BOOST_CHECK((IsSameType<TConstPtrTraits::TStorage, const T*>::value));

    typedef TypeTraits<T* const> TPtrConstTraits;
    BOOST_CHECK(TPtrConstTraits::isConst);
    BOOST_CHECK(TPtrConstTraits::isPointer);
    BOOST_CHECK(!TPtrConstTraits::isReference);
    BOOST_CHECK((IsSameType<TPtrConstTraits::Type, T* const>::value));
    BOOST_CHECK((IsSameType<TPtrConstTraits::TNonConst, T*>::value));
    BOOST_CHECK((IsSameType<TPtrConstTraits::TConst, T* const>::value));
    BOOST_CHECK((IsSameType<TPtrConstTraits::TPointee, T>::value));
    BOOST_CHECK((IsSameType<TPtrConstTraits::TReferred, T* const>::value));
    BOOST_CHECK((IsSameType<TPtrConstTraits::TReference, T* const&>::value));
    BOOST_CHECK((IsSameType<TPtrConstTraits::TStorage, T*>::value));

    typedef TypeTraits<const T* const> TConstPtrConstTraits;
    BOOST_CHECK(TConstPtrConstTraits::isConst);
    BOOST_CHECK(TConstPtrConstTraits::isPointer);
    BOOST_CHECK(!TConstPtrConstTraits::isReference);
    BOOST_CHECK((IsSameType<TConstPtrConstTraits::Type, const T* const>::value));
    BOOST_CHECK((IsSameType<TConstPtrConstTraits::TNonConst, const T*>::value));
    BOOST_CHECK((IsSameType<TConstPtrConstTraits::TConst, const T* const>::value));
    BOOST_CHECK((IsSameType<TConstPtrConstTraits::TPointee, const T>::value));
    BOOST_CHECK((IsSameType<TConstPtrConstTraits::TReferred, const T* const>::value));
    BOOST_CHECK((IsSameType<TConstPtrConstTraits::TReference, const T* const &>::value));
    BOOST_CHECK((IsSameType<TConstPtrConstTraits::TStorage, const T*>::value));

    typedef TypeTraits<T&> TRefTraits;
    BOOST_CHECK(!TRefTraits::isConst);
    BOOST_CHECK(!TRefTraits::isPointer);
    BOOST_CHECK(TRefTraits::isReference);
    BOOST_CHECK((IsSameType<TRefTraits::Type, T&>::value));
    BOOST_CHECK((IsSameType<TRefTraits::TNonConst, T&>::value));
    BOOST_CHECK((IsSameType<TRefTraits::TConst, const T&>::value));
    BOOST_CHECK((IsSameType<TRefTraits::TPointee, NullType>::value));
    BOOST_CHECK((IsSameType<TRefTraits::TReferred, T>::value));
    BOOST_CHECK((IsSameType<TRefTraits::TReference, T&>::value));
    BOOST_CHECK((IsSameType<TRefTraits::TStorage, T>::value));

    typedef TypeTraits<const T&> TConstRefTraits;
    BOOST_CHECK(TConstRefTraits::isConst);
    BOOST_CHECK(!TConstRefTraits::isPointer);
    BOOST_CHECK(TConstRefTraits::isReference);
    BOOST_CHECK((IsSameType<TConstRefTraits::Type, const T&>::value));
    BOOST_CHECK((IsSameType<TConstRefTraits::TNonConst, T&>::value));
    BOOST_CHECK((IsSameType<TConstRefTraits::TConst, const T&>::value));
    BOOST_CHECK((IsSameType<TConstRefTraits::TPointee, NullType>::value));
    BOOST_CHECK((IsSameType<TConstRefTraits::TReferred, const T >::value));
    BOOST_CHECK((IsSameType<TConstRefTraits::TReference, const T&>::value));
    BOOST_CHECK((IsSameType<TConstRefTraits::TStorage, T>::value));

    typedef TypeTraits<T*&> TPtrRefTraits;
    BOOST_CHECK(!TPtrRefTraits::isConst);
    BOOST_CHECK(TPtrRefTraits::isPointer); //?????????
    BOOST_CHECK(TPtrRefTraits::isReference);
    BOOST_CHECK((IsSameType<TPtrRefTraits::Type, T*&>::value));
    BOOST_CHECK((IsSameType<TPtrRefTraits::TNonConst, T*&>::value));
    BOOST_CHECK((IsSameType<TPtrRefTraits::TConst, T* const&>::value));
    BOOST_CHECK((IsSameType<TPtrRefTraits::TPointee, T>::value));
    BOOST_CHECK((IsSameType<TPtrRefTraits::TReferred, T*>::value));
    BOOST_CHECK((IsSameType<TPtrRefTraits::TReference, T*&>::value));
    BOOST_CHECK((IsSameType<TPtrRefTraits::TStorage, T*>::value));

    typedef TypeTraits<const T*&> TConstPtrRefTraits;
    BOOST_CHECK(!TConstPtrRefTraits::isConst);
    BOOST_CHECK(TConstPtrRefTraits::isPointer);
    BOOST_CHECK(TConstPtrRefTraits::isReference);
    BOOST_CHECK((IsSameType<TConstPtrRefTraits::Type, const T*&>::value));
    BOOST_CHECK((IsSameType<TConstPtrRefTraits::TNonConst, const T*&>::value));
    BOOST_CHECK((IsSameType<TConstPtrRefTraits::TConst, const T* const&>::value));
    BOOST_CHECK((IsSameType<TConstPtrRefTraits::TPointee, const T>::value));
    BOOST_CHECK((IsSameType<TConstPtrRefTraits::TReferred, const T*>::value));
    BOOST_CHECK((IsSameType<TConstPtrRefTraits::TReference, const T*&>::value));
    BOOST_CHECK((IsSameType<TConstPtrRefTraits::TStorage, const T*>::value));

    typedef TypeTraits<T* const&> TPtrConstRefTraits;
    BOOST_CHECK(TPtrConstRefTraits::isConst);
    BOOST_CHECK(TPtrConstRefTraits::isPointer);
    BOOST_CHECK(TPtrConstRefTraits::isReference);
    BOOST_CHECK((IsSameType<TPtrConstRefTraits::Type, T* const&>::value));
    BOOST_CHECK((IsSameType<TPtrConstRefTraits::TNonConst, T*&>::value));
    BOOST_CHECK((IsSameType<TPtrConstRefTraits::TConst, T* const&>::value));
    BOOST_CHECK((IsSameType<TPtrConstRefTraits::TPointee, T>::value));
    BOOST_CHECK((IsSameType<TPtrConstRefTraits::TReferred, T* const>::value));
    BOOST_CHECK((IsSameType<TPtrConstRefTraits::TReference, T* const&>::value));
    BOOST_CHECK((IsSameType<TPtrConstRefTraits::TStorage, T*>::value));

    typedef TypeTraits<const T* const&> TConstPtrConstRefTraits;
    BOOST_CHECK(TConstPtrConstRefTraits::isConst);
    BOOST_CHECK(TConstPtrConstRefTraits::isPointer);
    BOOST_CHECK(TConstPtrConstRefTraits::isReference);
    BOOST_CHECK((IsSameType<TConstPtrConstRefTraits::Type, const T* const&>::value));
    BOOST_CHECK((IsSameType<TConstPtrConstRefTraits::TNonConst, const T*&>::value));
    BOOST_CHECK((IsSameType<TConstPtrConstRefTraits::TConst, const T* const&>::value));
    BOOST_CHECK((IsSameType<TConstPtrConstRefTraits::TPointee, const T>::value));
    BOOST_CHECK((IsSameType<TConstPtrConstRefTraits::TReferred, const T* const>::value));
    BOOST_CHECK((IsSameType<TConstPtrConstRefTraits::TReference, const T* const &>::value));
    BOOST_CHECK((IsSameType<TConstPtrConstRefTraits::TStorage, const T*>::value));
}

}

}

#endif