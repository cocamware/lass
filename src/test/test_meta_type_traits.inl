/**	@file
 *  @internal
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
    std::cout << "TNonConst: " << typeid(typename TypeTraits<type>::TNonConst).name() << std::endl;\
    std::cout << "TConst: " << typeid(typename TypeTraits<type>::TConst).name() << std::endl;\
    std::cout << "TPointee: " << typeid(typename TypeTraits<type>::TPointee).name() << std::endl;\
    std::cout << "TReferred: " << typeid(typename TypeTraits<type>::TReferred).name() << std::endl;\
    std::cout << "TReference: " << typeid(typename TypeTraits<type>::TReference).name() << std::endl;\
    std::cout << "TStorage: " << typeid(typename TypeTraits<type>::TStorage).name() << std::endl;\
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
    BOOST_CHECK((IsSameType<typename TTraits::Type, T>::value));
    BOOST_CHECK((IsSameType<typename TTraits::TNonConst, T>::value));
    BOOST_CHECK((IsSameType<typename TTraits::TConst, T const>::value));
    BOOST_CHECK((IsSameType<typename TTraits::TPointee, NullType>::value));
    BOOST_CHECK((IsSameType<typename TTraits::TReferred, T>::value));
    BOOST_CHECK((IsSameType<typename TTraits::TReference, T&>::value));
    BOOST_CHECK((IsSameType<typename TTraits::TStorage, T>::value));

    typedef TypeTraits<const T> TConstTraits;
    BOOST_CHECK(TConstTraits::isConst);
    BOOST_CHECK(!TConstTraits::isPointer);
    BOOST_CHECK(!TConstTraits::isReference);
    BOOST_CHECK((IsSameType<typename TConstTraits::Type, const T>::value));
    BOOST_CHECK((IsSameType<typename TConstTraits::TNonConst, T>::value));
    BOOST_CHECK((IsSameType<typename TConstTraits::TConst, const T>::value));
    BOOST_CHECK((IsSameType<typename TConstTraits::TPointee, NullType>::value));
    BOOST_CHECK((IsSameType<typename TConstTraits::TReferred, const T>::value));
    BOOST_CHECK((IsSameType<typename TConstTraits::TReference, const T&>::value));
    BOOST_CHECK((IsSameType<typename TConstTraits::TStorage, T>::value));

    typedef TypeTraits<T*> TPtrTraits;
    BOOST_CHECK(!TPtrTraits::isConst);
    BOOST_CHECK(TPtrTraits::isPointer);
    BOOST_CHECK(!TPtrTraits::isReference);
    BOOST_CHECK((IsSameType<typename TPtrTraits::Type, T*>::value));
    BOOST_CHECK((IsSameType<typename TPtrTraits::TNonConst, T*>::value));
    BOOST_CHECK((IsSameType<typename TPtrTraits::TConst, T* const>::value));
    BOOST_CHECK((IsSameType<typename TPtrTraits::TPointee, T>::value));
    BOOST_CHECK((IsSameType<typename TPtrTraits::TReferred, T*>::value));
    BOOST_CHECK((IsSameType<typename TPtrTraits::TReference, T*&>::value));
    BOOST_CHECK((IsSameType<typename TPtrTraits::TStorage, T*>::value));

    typedef TypeTraits<const T*> TConstPtrTraits;
    BOOST_CHECK(!TConstPtrTraits::isConst);
    BOOST_CHECK(TConstPtrTraits::isPointer);
    BOOST_CHECK(!TConstPtrTraits::isReference);
    BOOST_CHECK((IsSameType<typename TConstPtrTraits::Type, const T*>::value));
    BOOST_CHECK((IsSameType<typename TConstPtrTraits::TNonConst, const T*>::value));
    BOOST_CHECK((IsSameType<typename TConstPtrTraits::TConst, const T* const>::value));
    BOOST_CHECK((IsSameType<typename TConstPtrTraits::TPointee, const T>::value));
    BOOST_CHECK((IsSameType<typename TConstPtrTraits::TReferred, const T*>::value));
    BOOST_CHECK((IsSameType<typename TConstPtrTraits::TReference, const T*&>::value));
    BOOST_CHECK((IsSameType<typename TConstPtrTraits::TStorage, const T*>::value));

    typedef TypeTraits<T* const> TPtrConstTraits;
    BOOST_CHECK(TPtrConstTraits::isConst);
    BOOST_CHECK(TPtrConstTraits::isPointer);
    BOOST_CHECK(!TPtrConstTraits::isReference);
    BOOST_CHECK((IsSameType<typename TPtrConstTraits::Type, T* const>::value));
    BOOST_CHECK((IsSameType<typename TPtrConstTraits::TNonConst, T*>::value));
    BOOST_CHECK((IsSameType<typename TPtrConstTraits::TConst, T* const>::value));
    BOOST_CHECK((IsSameType<typename TPtrConstTraits::TPointee, T>::value));
    BOOST_CHECK((IsSameType<typename TPtrConstTraits::TReferred, T* const>::value));
    BOOST_CHECK((IsSameType<typename TPtrConstTraits::TReference, T* const&>::value));
    BOOST_CHECK((IsSameType<typename TPtrConstTraits::TStorage, T*>::value));

    typedef TypeTraits<const T* const> TConstPtrConstTraits;
    BOOST_CHECK(TConstPtrConstTraits::isConst);
    BOOST_CHECK(TConstPtrConstTraits::isPointer);
    BOOST_CHECK(!TConstPtrConstTraits::isReference);
    BOOST_CHECK((IsSameType<typename TConstPtrConstTraits::Type, const T* const>::value));
    BOOST_CHECK((IsSameType<typename TConstPtrConstTraits::TNonConst, const T*>::value));
    BOOST_CHECK((IsSameType<typename TConstPtrConstTraits::TConst, const T* const>::value));
    BOOST_CHECK((IsSameType<typename TConstPtrConstTraits::TPointee, const T>::value));
    BOOST_CHECK((IsSameType<typename TConstPtrConstTraits::TReferred, const T* const>::value));
    BOOST_CHECK((IsSameType<typename TConstPtrConstTraits::TReference, const T* const &>::value));
    BOOST_CHECK((IsSameType<typename TConstPtrConstTraits::TStorage, const T*>::value));

    typedef TypeTraits<T&> TRefTraits;
    BOOST_CHECK(!TRefTraits::isConst);
    BOOST_CHECK(!TRefTraits::isPointer);
    BOOST_CHECK(TRefTraits::isReference);
    BOOST_CHECK((IsSameType<typename TRefTraits::Type, T&>::value));
    BOOST_CHECK((IsSameType<typename TRefTraits::TNonConst, T&>::value));
    BOOST_CHECK((IsSameType<typename TRefTraits::TConst, const T&>::value));
    BOOST_CHECK((IsSameType<typename TRefTraits::TPointee, NullType>::value));
    BOOST_CHECK((IsSameType<typename TRefTraits::TReferred, T>::value));
    BOOST_CHECK((IsSameType<typename TRefTraits::TReference, T&>::value));
    BOOST_CHECK((IsSameType<typename TRefTraits::TStorage, T>::value));

    typedef TypeTraits<const T&> TConstRefTraits;
    BOOST_CHECK(TConstRefTraits::isConst);
    BOOST_CHECK(!TConstRefTraits::isPointer);
    BOOST_CHECK(TConstRefTraits::isReference);
    BOOST_CHECK((IsSameType<typename TConstRefTraits::Type, const T&>::value));
    BOOST_CHECK((IsSameType<typename TConstRefTraits::TNonConst, T&>::value));
    BOOST_CHECK((IsSameType<typename TConstRefTraits::TConst, const T&>::value));
    BOOST_CHECK((IsSameType<typename TConstRefTraits::TPointee, NullType>::value));
    BOOST_CHECK((IsSameType<typename TConstRefTraits::TReferred, const T >::value));
    BOOST_CHECK((IsSameType<typename TConstRefTraits::TReference, const T&>::value));
    BOOST_CHECK((IsSameType<typename TConstRefTraits::TStorage, T>::value));

    typedef TypeTraits<T*&> TPtrRefTraits;
    BOOST_CHECK(!TPtrRefTraits::isConst);
    BOOST_CHECK(TPtrRefTraits::isPointer); //?????????
    BOOST_CHECK(TPtrRefTraits::isReference);
    BOOST_CHECK((IsSameType<typename TPtrRefTraits::Type, T*&>::value));
    BOOST_CHECK((IsSameType<typename TPtrRefTraits::TNonConst, T*&>::value));
    BOOST_CHECK((IsSameType<typename TPtrRefTraits::TConst, T* const&>::value));
    BOOST_CHECK((IsSameType<typename TPtrRefTraits::TPointee, T>::value));
    BOOST_CHECK((IsSameType<typename TPtrRefTraits::TReferred, T*>::value));
    BOOST_CHECK((IsSameType<typename TPtrRefTraits::TReference, T*&>::value));
    BOOST_CHECK((IsSameType<typename TPtrRefTraits::TStorage, T*>::value));

    typedef TypeTraits<const T*&> TConstPtrRefTraits;
    BOOST_CHECK(!TConstPtrRefTraits::isConst);
    BOOST_CHECK(TConstPtrRefTraits::isPointer);
    BOOST_CHECK(TConstPtrRefTraits::isReference);
    BOOST_CHECK((IsSameType<typename TConstPtrRefTraits::Type, const T*&>::value));
    BOOST_CHECK((IsSameType<typename TConstPtrRefTraits::TNonConst, const T*&>::value));
    BOOST_CHECK((IsSameType<typename TConstPtrRefTraits::TConst, const T* const&>::value));
    BOOST_CHECK((IsSameType<typename TConstPtrRefTraits::TPointee, const T>::value));
    BOOST_CHECK((IsSameType<typename TConstPtrRefTraits::TReferred, const T*>::value));
    BOOST_CHECK((IsSameType<typename TConstPtrRefTraits::TReference, const T*&>::value));
    BOOST_CHECK((IsSameType<typename TConstPtrRefTraits::TStorage, const T*>::value));

    typedef TypeTraits<T* const&> TPtrConstRefTraits;
    BOOST_CHECK(TPtrConstRefTraits::isConst);
    BOOST_CHECK(TPtrConstRefTraits::isPointer);
    BOOST_CHECK(TPtrConstRefTraits::isReference);
    BOOST_CHECK((IsSameType<typename TPtrConstRefTraits::Type, T* const&>::value));
    BOOST_CHECK((IsSameType<typename TPtrConstRefTraits::TNonConst, T*&>::value));
    BOOST_CHECK((IsSameType<typename TPtrConstRefTraits::TConst, T* const&>::value));
    BOOST_CHECK((IsSameType<typename TPtrConstRefTraits::TPointee, T>::value));
    BOOST_CHECK((IsSameType<typename TPtrConstRefTraits::TReferred, T* const>::value));
    BOOST_CHECK((IsSameType<typename TPtrConstRefTraits::TReference, T* const&>::value));
    BOOST_CHECK((IsSameType<typename TPtrConstRefTraits::TStorage, T*>::value));

    typedef TypeTraits<const T* const&> TConstPtrConstRefTraits;
    BOOST_CHECK(TConstPtrConstRefTraits::isConst);
    BOOST_CHECK(TConstPtrConstRefTraits::isPointer);
    BOOST_CHECK(TConstPtrConstRefTraits::isReference);
    BOOST_CHECK((IsSameType<typename TConstPtrConstRefTraits::Type, const T* const&>::value));
    BOOST_CHECK((IsSameType<typename TConstPtrConstRefTraits::TNonConst, const T*&>::value));
    BOOST_CHECK((IsSameType<typename TConstPtrConstRefTraits::TConst, const T* const&>::value));
    BOOST_CHECK((IsSameType<typename TConstPtrConstRefTraits::TPointee, const T>::value));
    BOOST_CHECK((IsSameType<typename TConstPtrConstRefTraits::TReferred, const T* const>::value));
    BOOST_CHECK((IsSameType<typename TConstPtrConstRefTraits::TReference, const T* const &>::value));
    BOOST_CHECK((IsSameType<typename TConstPtrConstRefTraits::TStorage, const T*>::value));
}

}

}

#endif
